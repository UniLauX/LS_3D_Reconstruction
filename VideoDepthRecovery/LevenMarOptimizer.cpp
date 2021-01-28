#include "LevenMarOptimizer.h"
#include "WmlLinearSystem.h"
//包含库目录
#include "..\VideoDepthRecovery\libs\WmlMath\lib\wmlmathlink.h"

/*
LevenMarOptimizer::LevenMarOptimizer(void)
{
}
*/
//构造函数(初始化赋值，和为相关数据结构分配存储空间）
LevenMarOptimizer::LevenMarOptimizer(const std::vector<blk::Point> &SegmPoints, int iterationCount, int LayerCount, float Disc_k)
	:m_SegmPoints(SegmPoints)
{
	m_iMaxIterationCount = iterationCount;
	m_iPointCount = SegmPoints.size();
	m_iLayerCount = LayerCount;
	m_fDiscK = Disc_k;

	m_DataCost.resize(m_iPointCount, NULL);
	m_DataCostGradForX.resize(m_iPointCount, NULL);

}


//析构函数（释放数据存储空间）
LevenMarOptimizer::~LevenMarOptimizer(void)
{
	for(auto i=0U; i<m_DataCost.size(); ++i)
		delete[] m_DataCost[i];

	for(auto i=0U; i<m_DataCostGradForX.size(); ++i)
		delete[] m_DataCostGradForX[i];
}


// 设置某个Index上（不同layer上）的dataCostUnit值
void LevenMarOptimizer::SetDataCostAt(int index, DataCostUnit& dataCosti)
{
	m_DataCost[index] = new float[m_iLayerCount];
	m_DataCostGradForX[index] = new float[m_iLayerCount];

	//memcpy(m_DataCost[index], dataCosti, m_iLayerCount * sizeof(float));
	for(int i=0; i<m_iLayerCount; i++)
		m_DataCost[index][i] = dataCosti[i];

	for(int i=1; i<m_iLayerCount-1; ++i){
		m_DataCostGradForX[index][i] = (dataCosti[i+1] - dataCosti[i-1])*0.5;   //中间值为左右邻居和的一半
	}

	m_DataCostGradForX[index][0] = 0;
	m_DataCostGradForX[index][m_iLayerCount-1] = 0;
}


//添加边缘点邻居的disparity
void LevenMarOptimizer::AddEdgeNeighborInfo( int PointIndex, int NeighborDsp )
{
	EdgeNeighborInfo newEdgeInfo = {PointIndex, NeighborDsp};
	m_EdgeNeighborInfos.push_back(newEdgeInfo);
}



//求利用LevenMar求a_i,b_i
void LevenMarOptimizer::Optimize(Wml::GVectord& x, double& MinValue)
{
		int n = x.GetSize();   //n=3

		double alamda = 1e-3;

		Wml::GVectord grad(n), dx(n),dir(n), mainDiagElement(n);
		Wml::GMatrixd hesse(n,n);


		double time, iterTime;
		double f = GetValue(x);  //利用约束条件求c_i
		double terminator_lastFuncValue = f;
		bool IsReCalculate = true;      

		int iValidIter = 0;   //控制迭代次数
		for( int iteration=1; iteration<= m_iMaxIterationCount; iteration++ )
		{
				//只有x变化才有必要重新计算！
                if(IsReCalculate)
				{
					//获得a_i,b_i,c_i对应的梯度方向
                    GetGradient(x,grad);

					//获得海赛矩阵（相当于求解二阶导数)
					GetHesse(x, hesse);
					
				}
				dir = -grad;			//direction	
				//tmpMtx.Attach(hesse.ToTaucsMatrix());

				for(int i=0; i<n; i++){
					mainDiagElement[i] = hesse(i,i);
					hesse(i,i) = mainDiagElement[i]*(1.0+alamda);
				}

				//		///////////////////////////////////////////////
				//		// LM算法核心
				//		///////////////////////////////////////////////

				//求解dx;
				//if(!Wml::LinearSystemd::SolveSymmetric(hesse,dir,dx))
				if(!Wml::LinearSystemd::SolveSymmetric(hesse,dir,dx))
				{
					//printf("Linear Matrix No Solution!\n");
					//printf(".");
					alamda *= 10.0;	
					for(int i=0; i<n; i++)
					{				
						if(mainDiagElement[i]<DBL_EPSILON)
						{
							dx[i] = dir[i]/alamda;
						}
						else
							dx[i] = dir[i]/(mainDiagElement[i]*(1+alamda));
					}

				}


				double f_2 = GetValue(x + dx);
				//printf("f:%f\t,x:%f,%f\n",f_2,dx[0],dx[1]);
				//printf("f:%f, f_2:%f\n",f, f_2);

				if(f_2<=f)
				{
					alamda *= 0.1;
					f = f_2;
					x += dx;			
					IsReCalculate = true;
					++iValidIter;
				}
				else
				{
					alamda *= 10.0;
					f_2 = f;
					IsReCalculate = false;
					//复原主对角元素
					for(int i=0; i<n; i++)
					{
						hesse(i,i) = mainDiagElement[i];
					}
				}
				//判断是否满足收敛判别准则
				//最后一次要令alamda=0
				if( IsReCalculate && iValidIter % 3 == 0 )
				{ 
					if(f / terminator_lastFuncValue > 1.0 - 5e-4)
						break;
					terminator_lastFuncValue = f;
				}

		}// end of the main iteration

		//FINAL:
		GetGradient(x, grad);
		GetHesse(x, hesse);
		dir = -grad;

		if(!Wml::LinearSystemd::SolveSymmetric(hesse,dir,dx)){
			alamda *= 10.0;	
			for(int i=0;i<n;i++){
				if(hesse(i,i)<DBL_EPSILON){
					dx[i] = dir[i]/alamda;
				}
				else
					dx[i] = dir[i]/(hesse(i,i)*(1+alamda));
			}
		}


		float f_2 = GetValue(x+dx);
		if(f_2<f){
			f = f_2;
			x += dx;
		}
		MinValue = f;


}

//（利用x向量）设置a_i,b_i,c_i的值
void LevenMarOptimizer::SetVar(const Wml::GVectord& x)
{
	m_a = x[0];
	m_b = x[1];
	m_c = x[2];
}


//利用约束得到c_i的值
double LevenMarOptimizer::GetValue(Wml::GVectord& x)
{
	SetVar(x);  //设置m_a,m_b,m_c的值

	int u, v;    //代表x,y

	double val = 0;	


	for(int index = 0; index < m_iPointCount; index++)
	{
		u = m_SegmPoints[index].X;
		v = m_SegmPoints[index].Y;
		float d = m_a * u + m_b * v + m_c;      //disparity=a_i*x+b_i*y+c

	    double tmpVal = GetDataCostValueAt(index, d);
	    val += tmpVal * tmpVal;
	}

	//Constraint Points
	double sVal=0;
	for(std::vector<EdgeNeighborInfo>::iterator pPointInfo = m_EdgeNeighborInfos.begin(); pPointInfo < m_EdgeNeighborInfos.end(); pPointInfo++){
		int edgePointIndex = pPointInfo->EdgePointIndex;
		u = m_SegmPoints[edgePointIndex].X;
		v = m_SegmPoints[edgePointIndex].Y;
		float d = m_a*u + m_b*v + m_c;
		sVal += std::min(m_fDiscK*m_fDiscK, (d - pPointInfo->NeighborDsp) * (d - pPointInfo->NeighborDsp));    //不理解
	}
	//printf("sVal:%f\t",sqrt(sVal/m_constPts.size()));
	val += sVal;

	return val;

}

//获得在分割索引index上的DataCostValue（不理解）
float LevenMarOptimizer::GetDataCostValueAt(int index, float d)
{
	if(d > 0 && d < m_iLayerCount-1){
		int id0 = (int)d;
		float dx = d - id0;
		return m_DataCost[index][id0] * (1.0-dx) + m_DataCost[index][id0+1] * dx;  //不理解此计算过程
	}
	else if(d <= 0){
		return m_DataCost[index][0] + 20;              //  根据经验设置？
	}
	else 
		return m_DataCost[index][m_iLayerCount-1] + 20;    //同上
}

//类似于GetDataCostValue函数
float LevenMarOptimizer::GetDataCostGradForX(int index, float d)
{
	if(d>0 && d < m_iLayerCount-1){
		int id0 = (int)d;
		float dx = d - id0;
		return m_DataCostGradForX[index][id0] * (1.0-dx) + m_DataCostGradForX[index][id0+1] * dx;
	}
	else if(d <= 0){
		return m_DataCostGradForX[index][0];
	}
	else
		return m_DataCostGradForX[index][m_iLayerCount-1];
}




//得到a_i,b_i,c_i的梯度
void LevenMarOptimizer::GetGradient(const Wml::GVectord& x,Wml::GVectord& Direction)
{
	//初始化
	SetVar(x);
	for(int i=0;i<Direction.GetSize();i++)    //size=3;
		Direction[i] = 0.0;



	int u,v;  //x,y方向
	for(int index=0; index<m_iPointCount; ++index)
	{

		u = m_SegmPoints[index].X;
		v = m_SegmPoints[index].Y;

		float d = m_a*u + m_b*v + m_c;
		
		//获得在分割索引index上的DataCostValue（值）
		float tmpVal = GetDataCostValueAt(index, d);
		float val = 2 * tmpVal;


		float gradValueForX = GetDataCostGradForX(index,d);


		//获得在a_i,b_i,c_i三个分量的梯度方向
		Direction[0] += val * gradValueForX * u;
		Direction[1] += val * gradValueForX * v;
		Direction[2] += val * gradValueForX;

	}


//Constraint Points(针对约束点求解）
	for(std::vector<EdgeNeighborInfo>::iterator pPointInfo=m_EdgeNeighborInfos.begin();pPointInfo<m_EdgeNeighborInfos.end();pPointInfo++)
	{
		int edgePointIndex=pPointInfo->EdgePointIndex;
	    u=m_SegmPoints[edgePointIndex].X;
		v=m_SegmPoints[edgePointIndex].Y;
		float d=m_a*u+m_b*v+m_c;
		float val = (d - pPointInfo->NeighborDsp);

		if(fabs(val) <= m_fDiscK)
			val = 2 * val;
		else
			val = 2 * val * 0.05 * 0.05;

		Direction[0] += val * u;
		Direction[1] += val * v;
		Direction[2] += val;

	}

}

//得到海赛矩阵（相当于求二阶导数）
void LevenMarOptimizer::GetHesse(const Wml::GVectord& x, Wml::GMatrixd& Hesse)
{
	  /*
	    Hesse 矩阵相当于函数的“二阶导数”
		f(x)：一个n元函数
		那么f(x)的梯度g(x)是一个n维向量，然后再对g(x)每一个分量求梯度
		得到Hesse矩阵H(x)，它是一个n*n的矩阵。
		*/


	//初始化
	SetVar(x);
	for(int i=0;i<Hesse.GetRows();i++)
	{
		for(int j=0;j<Hesse.GetColumns();j++)
		{
			Hesse(i,j) = 0;
		}
	}	
	
//设置存储结构大小
	Wml::GMatrixf J, JTJ;
	int n = x.GetSize();
	J.SetSize(m_iPointCount, n);


	int u,v;
	for(int index=0; index<m_iPointCount; index++){
		u = m_SegmPoints[index].X;
		v = m_SegmPoints[index].Y;

		float d = m_a*u + m_b*v + m_c;
		float gradValueForX = GetDataCostGradForX(index,d);

		J(index, 0) = gradValueForX*u;
		J(index, 1) = gradValueForX*v;
		J(index, 2) = gradValueForX;
	}
	
	JTJ = J.Transpose() * J;
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			Hesse(i, j) += 2 * JTJ(i,j);
		}
	}

	if(m_EdgeNeighborInfos.size() == 0)//very important when the block is very small!
		return;

	J.SetSize(m_EdgeNeighborInfos.size(), n);
	for(int index=0; index<m_EdgeNeighborInfos.size(); index++){
		int edgePointIndex = m_EdgeNeighborInfos[index].EdgePointIndex;

		u = m_SegmPoints[edgePointIndex].X;
		v = m_SegmPoints[edgePointIndex].Y;
		float d = m_a*u + m_b*v + m_c;

		float val = (d - m_EdgeNeighborInfos[index].NeighborDsp);

		if(fabs(val) <= m_fDiscK){
			J(index,0) = u;
			J(index,1) = v;
			J(index,2) = 1;
		}
		else{
			J(index,0) = u * 0.05F;
			J(index,1) = v * 0.05F;
			J(index,2) = 1 * 0.05F;
		}
	}

	JTJ = J.Transpose() * J;

	for(int i=0; i<n; i++){
		for(int j=0;j<n;j++){
			Hesse(i, j) += 2 * JTJ(i,j);
		}
	}
}


//获得分割的smoothCost
double LevenMarOptimizer::GetAbsValue( const Wml::GVectord& x )
{
	SetVar(x);
	int u, v;
	double val = 0;	

	for(int index = 0; index < m_iPointCount; index++){
		u = m_SegmPoints[index].X;
		v = m_SegmPoints[index].Y;
		float d = m_a * u + m_b * v + m_c;

		double tmpVal = GetDataCostValueAt(index, d);
		val += tmpVal;
	}

	//Constraint Points
	double sVal=0;
	for(std::vector<EdgeNeighborInfo>::iterator pPointInfo = m_EdgeNeighborInfos.begin(); pPointInfo < m_EdgeNeighborInfos.end(); pPointInfo++){
		int edgePointIndex = pPointInfo->EdgePointIndex;
		u = m_SegmPoints[edgePointIndex].X;
		v = m_SegmPoints[edgePointIndex].Y;
		float d = m_a*u + m_b*v + m_c;
		sVal += std::min(m_fDiscK, abs(d - pPointInfo->NeighborDsp));
	}

	//printf("segDataCost:%lf segSmoothCost:%lf\n", val, sVal);
	val += sVal;
	return val;
}