#include "PixelCostComputer_Init.h"


PixelCostComputer_Init::PixelCostComputer_Init(void)
{
}


PixelCostComputer_Init::~PixelCostComputer_Init(void)
{
}
/*
void PixelCostComputer_Init::PixelDataCost(int x,int y,const std::vector<double> &dspV, DataCostUnit &dataCostI, int &bestLabel)
{
	double maxLikelihood=1e-20F;    //定义最大的disparity likelihood 值
    int frameCount=m_pNearFrames->size();
	int dspLevelCount=dspV.size();

	double colorSigmaSquare=m_dColorSigma*m_dColorSigma;  // sigma_c的平方

	Wml::Vector3d ptWorldCoord;
	Wml::Vector3d currentColor,correspondingColor;

	m_pCurrentFrame->GetColorAt(x,y,currentColor);


	//在各个level上求(x,y)的DataCost并设置相应的最佳label(bestLabel)值
    for(int depthLevelI=0;depthLevelI<dspLevelCount;depthLevelI++)
	{
		dataCostI[depthLevelI]=0;

		//得到（当前帧）depthLevelI层（x,y)图像坐标点的世界坐标点
        m_pCurrentFrame->GetWorldCoordFromImageCoord(x,y,dspV[depthLevelI],ptWorldCoord);

	    for(int i=0;i<frameCount;i++)
		{
			double dsp=0;
			double x2,y2;
	     
			//从世界坐标点ptWorldCoord得到相邻帧在（x,y)位置的图像坐标（x2,y2)以及disparity
			m_pNearFrames->at(i)->GetImageCoordFromWorldCoord(x2,y2,dsp,ptWorldCoord);

			//三维投射后超过图像边界的惩罚值 【参看论文 formula(3)】
			if(x2<0 || y2<0 || x2 > VDRVideoFrame::GetImageWidth()-1 || y2 > VDRVideoFrame::GetImageHeight()-1)
				dataCostI[depthLevelI] += m_dColorSigma / (m_dColorSigma + (m_dColorMissPenalty/3)*(m_dColorMissPenalty/3));
			else
			{
				m_pNearFrames->at(i)->GetColorAt(x2, y2, correspondingColor);
				double colorDist = (fabs(currentColor[0] - correspondingColor[0]) 
					+ fabs(currentColor[1] - correspondingColor[1]) 
					+ fabs(currentColor[2] - correspondingColor[2])) / 3.0;

						colorDist = min(30.0, colorDist);       //30.0为颜色惩罚项的阈值
						
				        //【参看论文formula(3)】
						double wc = colorSigmaSquare/(colorSigmaSquare+ colorDist*colorDist);
						                      //wc = colorSigmaSquare/(colorSigmaSquare+ colorDist);     

						//depthLevelI层上像素（x,y)的DataCost值（相邻几帧的累加）
						dataCostI[depthLevelI]+=wc;			   
			}
	
		}

		//对相邻几帧累加求得的DataCost求均值
	     dataCostI[depthLevelI]/=frameCount;    

		 //寻找depthLevelI层的最大DataCost（disparity likelihood)值，并设置相应label;
		   if(dataCostI[depthLevelI]>maxLikelihood)
		   {
			   maxLikelihood=dataCostI[depthLevelI];
			   bestLabel=depthLevelI;
		   }

	}

	std::cout<<"bestLabel:"<<bestLabel<<std::endl;
	//将所有层DataCost进行归一化（其值限定在0~1之间）
	float maxCost = 1e-6F;     
	for(int depthLevelI = 0; depthLevelI < dspLevelCount; depthLevelI++)
	{
		dataCostI[depthLevelI] =  1.0 - dataCostI[depthLevelI] / maxLikelihood;   //会将最小的DataCost和最大的DataCost颠倒？
		maxCost = max(maxCost, dataCostI[depthLevelI]);      
	}


	for(int depthLevelI=0; depthLevelI<dspLevelCount; depthLevelI++)
	{
		dataCostI[depthLevelI] /= maxCost;
		dataCostI[depthLevelI] *= m_fDataCostWeight;              //最终算出来为lamda*DataCost[depthLevelI](后面的值为0~1的范围）
	}
}
*/

//计算（x,y)坐标各DepthLevel层上的最大DataCost（disparity likelihood值）和bestlabel
void PixelCostComputer_Init::PixelDataCost( int x, int y, const std::vector<double> &dspV, DataCostUnit &dataCosti, int &bestLabel )
{

	//m_dColorSigma=5;
	double MaxLikelihood = 1e-20F;    //定义最大的disparity likelihood值
	int frameCount = m_pNearFrames->size();  //得到参考帧数
	int dspLevelCount = dspV.size();    // dspLevelCount=101
	

	double colorSigma2 = m_dColorSigma*m_dColorSigma;     //sigma_c的平方（m_dColorSigma=5）

	Wml::Vector3d ptWorldCoord;
	Wml::Vector3d CurrentColor, CorrespondingColor;

	m_pCurrentFrame->GetColorAt(x, y, CurrentColor);   //得到(x,y)位置像素的颜色值

    //在各个level上求(x,y)的DataCost并设置相应的最佳label(bestLabel)值
	for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
	{		
		dataCosti[depthLeveli] = 0;
	
	//得到（当前帧）depthLevelI层（x,y)图像坐标点的世界坐标点
    m_pCurrentFrame->GetWorldCoordFromImageCoord(x, y, dspV[depthLeveli], ptWorldCoord);

    // std::cout<<"dspV[i]:"<<dspV[depthLeveli]<<std::endl;
    // std::cout<<"ptWorldCoord: "<<ptWorldCoord[0]<<","<<ptWorldCoord[1]<<","<<ptWorldCoord[2]<<std::endl;
	// std::cout<<"frameCount: "<<frameCount<<std::endl;
	// double z = 1.0/dspV[depthLeveli];

		for(int i=0; i<frameCount; i++)
		{

		    //从世界坐标点ptWorldCoord得到参考帧在（x,y)位置的图像坐标（x2,y2)以及disparity值
			double dsp = 0, x2, y2;
			m_pNearFrames->at(i)->GetImageCoordFromWorldCoord(x2, y2, dsp, ptWorldCoord);
			//z2 = r[i][2]*z + b[i][2];
			//u2 = (r[i][0]*z + b[i][0]) / z2;
			//v2 = (r[i][1]*z + b[i][1]) / z2;	
			//std::cout<<"x2= "<<x2<<" , "<<"y2= "<<y2<<std::endl;

		   //三维投射后超过图像边界的惩罚值 【参看论文 formula(3)】
			if(x2<0 || y2<0 || x2 > VDRVideoFrame::GetImageWidth()-1 || y2 > VDRVideoFrame::GetImageHeight()-1)
				dataCosti[depthLeveli] += m_dColorSigma / (m_dColorSigma + (m_dColorMissPenalty/3)*(m_dColorMissPenalty/3));
			else
			{
				//（x,y)位置当前帧和参考帧的颜色差异（论文formula[2]的分母右边部分）
				m_pNearFrames->at(i)->GetColorAt(x2, y2, CorrespondingColor);
				double colordist = (fabs(CurrentColor[0] - CorrespondingColor[0]) 
					+ fabs(CurrentColor[1] - CorrespondingColor[1]) 
					+ fabs(CurrentColor[2] - CorrespondingColor[2])) / 3.0;

				colordist = min(30.0, colordist);           //30.0为颜色惩罚项的阈值
				

				//【参看论文formula(3)】
				double wc = colorSigma2/(colorSigma2 + colordist*colordist);
				//double wc = m_dColorSigma/(m_dColorSigma + colordist);


				//depthLevelI层上像素（x,y)的DataCost值（相邻几帧的累加）
				dataCosti[depthLeveli] += wc; //m_dColorSigma / (m_dColorSigma + colordist * colordist);
			}
		}

	   //对相邻几帧累加求得的DataCost求均值
		dataCosti[depthLeveli] /= frameCount;

	   //寻找depthLevelI层的最大DataCost（disparity likelihood)值，并设置相应label(bestLabel)值;
		if(dataCosti[depthLeveli] > MaxLikelihood){
			MaxLikelihood = dataCosti[depthLeveli];
			bestLabel = depthLeveli;
		}
	}

  // std::cout<<"BestLabel: "<<bestLabel<<std::endl;

	//将所有层DataCost进行归一化（其值限定在0~1之间）（参见论文formula[3])
	float maxCost = 1e-6F;
	for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++){
		dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;   //会将最小的DataCost和最大的DataCost颠倒？
		maxCost = max(maxCost, dataCosti[depthLeveli]);
	}
	for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++){
		dataCosti[depthLeveli] /= maxCost;
		dataCosti[depthLeveli] *= m_fDataCostWeight;                              //最终算出来为lamda*DataCost[depthLevelI](后面的值为0~1的范围）
	}
}
