#include "LevenMarOptimizer.h"
#include "WmlLinearSystem.h"
//������Ŀ¼
#include "..\VideoDepthRecovery\libs\WmlMath\lib\wmlmathlink.h"

/*
LevenMarOptimizer::LevenMarOptimizer(void)
{
}
*/
//���캯��(��ʼ����ֵ����Ϊ������ݽṹ����洢�ռ䣩
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


//�����������ͷ����ݴ洢�ռ䣩
LevenMarOptimizer::~LevenMarOptimizer(void)
{
	for(auto i=0U; i<m_DataCost.size(); ++i)
		delete[] m_DataCost[i];

	for(auto i=0U; i<m_DataCostGradForX.size(); ++i)
		delete[] m_DataCostGradForX[i];
}


// ����ĳ��Index�ϣ���ͬlayer�ϣ���dataCostUnitֵ
void LevenMarOptimizer::SetDataCostAt(int index, DataCostUnit& dataCosti)
{
	m_DataCost[index] = new float[m_iLayerCount];
	m_DataCostGradForX[index] = new float[m_iLayerCount];

	//memcpy(m_DataCost[index], dataCosti, m_iLayerCount * sizeof(float));
	for(int i=0; i<m_iLayerCount; i++)
		m_DataCost[index][i] = dataCosti[i];

	for(int i=1; i<m_iLayerCount-1; ++i){
		m_DataCostGradForX[index][i] = (dataCosti[i+1] - dataCosti[i-1])*0.5;   //�м�ֵΪ�����ھӺ͵�һ��
	}

	m_DataCostGradForX[index][0] = 0;
	m_DataCostGradForX[index][m_iLayerCount-1] = 0;
}


//��ӱ�Ե���ھӵ�disparity
void LevenMarOptimizer::AddEdgeNeighborInfo( int PointIndex, int NeighborDsp )
{
	EdgeNeighborInfo newEdgeInfo = {PointIndex, NeighborDsp};
	m_EdgeNeighborInfos.push_back(newEdgeInfo);
}



//������LevenMar��a_i,b_i
void LevenMarOptimizer::Optimize(Wml::GVectord& x, double& MinValue)
{
		int n = x.GetSize();   //n=3

		double alamda = 1e-3;

		Wml::GVectord grad(n), dx(n),dir(n), mainDiagElement(n);
		Wml::GMatrixd hesse(n,n);


		double time, iterTime;
		double f = GetValue(x);  //����Լ��������c_i
		double terminator_lastFuncValue = f;
		bool IsReCalculate = true;      

		int iValidIter = 0;   //���Ƶ�������
		for( int iteration=1; iteration<= m_iMaxIterationCount; iteration++ )
		{
				//ֻ��x�仯���б�Ҫ���¼��㣡
                if(IsReCalculate)
				{
					//���a_i,b_i,c_i��Ӧ���ݶȷ���
                    GetGradient(x,grad);

					//��ú��������൱�������׵���)
					GetHesse(x, hesse);
					
				}
				dir = -grad;			//direction	
				//tmpMtx.Attach(hesse.ToTaucsMatrix());

				for(int i=0; i<n; i++){
					mainDiagElement[i] = hesse(i,i);
					hesse(i,i) = mainDiagElement[i]*(1.0+alamda);
				}

				//		///////////////////////////////////////////////
				//		// LM�㷨����
				//		///////////////////////////////////////////////

				//���dx;
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
					//��ԭ���Խ�Ԫ��
					for(int i=0; i<n; i++)
					{
						hesse(i,i) = mainDiagElement[i];
					}
				}
				//�ж��Ƿ����������б�׼��
				//���һ��Ҫ��alamda=0
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

//������x����������a_i,b_i,c_i��ֵ
void LevenMarOptimizer::SetVar(const Wml::GVectord& x)
{
	m_a = x[0];
	m_b = x[1];
	m_c = x[2];
}


//����Լ���õ�c_i��ֵ
double LevenMarOptimizer::GetValue(Wml::GVectord& x)
{
	SetVar(x);  //����m_a,m_b,m_c��ֵ

	int u, v;    //����x,y

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
		sVal += std::min(m_fDiscK*m_fDiscK, (d - pPointInfo->NeighborDsp) * (d - pPointInfo->NeighborDsp));    //�����
	}
	//printf("sVal:%f\t",sqrt(sVal/m_constPts.size()));
	val += sVal;

	return val;

}

//����ڷָ�����index�ϵ�DataCostValue������⣩
float LevenMarOptimizer::GetDataCostValueAt(int index, float d)
{
	if(d > 0 && d < m_iLayerCount-1){
		int id0 = (int)d;
		float dx = d - id0;
		return m_DataCost[index][id0] * (1.0-dx) + m_DataCost[index][id0+1] * dx;  //�����˼������
	}
	else if(d <= 0){
		return m_DataCost[index][0] + 20;              //  ���ݾ������ã�
	}
	else 
		return m_DataCost[index][m_iLayerCount-1] + 20;    //ͬ��
}

//������GetDataCostValue����
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




//�õ�a_i,b_i,c_i���ݶ�
void LevenMarOptimizer::GetGradient(const Wml::GVectord& x,Wml::GVectord& Direction)
{
	//��ʼ��
	SetVar(x);
	for(int i=0;i<Direction.GetSize();i++)    //size=3;
		Direction[i] = 0.0;



	int u,v;  //x,y����
	for(int index=0; index<m_iPointCount; ++index)
	{

		u = m_SegmPoints[index].X;
		v = m_SegmPoints[index].Y;

		float d = m_a*u + m_b*v + m_c;
		
		//����ڷָ�����index�ϵ�DataCostValue��ֵ��
		float tmpVal = GetDataCostValueAt(index, d);
		float val = 2 * tmpVal;


		float gradValueForX = GetDataCostGradForX(index,d);


		//�����a_i,b_i,c_i�����������ݶȷ���
		Direction[0] += val * gradValueForX * u;
		Direction[1] += val * gradValueForX * v;
		Direction[2] += val * gradValueForX;

	}


//Constraint Points(���Լ������⣩
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

//�õ����������൱������׵�����
void LevenMarOptimizer::GetHesse(const Wml::GVectord& x, Wml::GMatrixd& Hesse)
{
	  /*
	    Hesse �����൱�ں����ġ����׵�����
		f(x)��һ��nԪ����
		��ôf(x)���ݶ�g(x)��һ��nά������Ȼ���ٶ�g(x)ÿһ���������ݶ�
		�õ�Hesse����H(x)������һ��n*n�ľ���
		*/


	//��ʼ��
	SetVar(x);
	for(int i=0;i<Hesse.GetRows();i++)
	{
		for(int j=0;j<Hesse.GetColumns();j++)
		{
			Hesse(i,j) = 0;
		}
	}	
	
//���ô洢�ṹ��С
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


//��÷ָ��smoothCost
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