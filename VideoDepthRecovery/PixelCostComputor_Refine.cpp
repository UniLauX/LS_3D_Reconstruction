#include "PixelCostComputor_Refine.h"


//定义：构造函数
PixelCostComputor_Refine::PixelCostComputor_Refine(void)
{
}


//定义：析构函数
PixelCostComputor_Refine::~PixelCostComputor_Refine(void)
{
}


//计算（x,y)坐标各DepthLevel层上的最大DataCost（disparity likelihood值）和bestlabel  
void PixelCostComputor_Refine::PixelDataCost(int x, int y, const std::vector<double> &dspV, DataCostUnit &dataCosti, int &bestLabel)
{
    // std::cout<<"execute BO pixelDataCost"<<std::endl;
	 double MaxLikelihood = 1e-20F;             //定义：最大的disparity likelihood
	 int frameCount = m_pNearFrames->size();    //得到参考帧数
	 int dspLevelCount = dspV.size();           //得到disparity 分层数

	 Wml::Vector3d ptWorldCoord;
	 Wml::Vector3d CurrentColor, CorrespondingColor;

	 double colorSigma2 = m_dColorSigma*m_dColorSigma;   //sigma_c的平方（论文formula[2])
	 double dspSigma2 = m_dDspSigma * m_dDspSigma;       //sigma_d的平方 (论文formula[7])

	 m_pCurrentFrame->GetColorAt(x, y, CurrentColor);    //得到当前帧（x,y）位置的颜色值

	 for(int depthLeveli = 0; depthLeveli < dspLevelCount; ++depthLeveli)
	 {		
		 dataCosti[depthLeveli] = 0;     //每个depthLevel层上dataCost初始值
		 m_pCurrentFrame->GetWorldCoordFromImageCoord(x, y, dspV[depthLeveli], ptWorldCoord); //得到（当前帧）depthLevelI层（x,y)图像坐标点的世界坐标点
		 //double z = 1.0/dspV[depthLeveli];
	     // std::cout<<"dspV[i]:"<<dspV[depthLeveli]<<std::endl;
		// std::cout<<"ptWorldCoord: "<<ptWorldCoord[0]<<","<<ptWorldCoord[1]<<","<<ptWorldCoord[2]<<std::endl;
		 // std::cout<<"frameCount: "<<frameCount<<std::endl;
          
		 for(int i=0; i<frameCount; i++)
		 {
			 double dsp = 0, x2, y2;
			 m_pNearFrames->at(i)->GetImageCoordFromWorldCoord(x2, y2, dsp, ptWorldCoord);    //从世界坐标点ptWorldCoord得到参考帧在（x,y)位置的图像坐标（x2,y2)以及disparity值
			 //z2 = r[i][2]*z + b[i][2];
			 //u2 = (r[i][0]*z + b[i][0]) / z2;
			 //v2 = (r[i][1]*z + b[i][1]) / z2;	

			// std::cout<<"x2= "<<x2<<" , "<<"y2= "<<y2<<std::endl;


			 //三维投射后超过图像边界的惩罚值 【参看论文 formula(3)】
			 if(x2<0 || y2<0 || x2 > VDRVideoFrame::GetImageWidth()-1 || y2 > VDRVideoFrame::GetImageHeight()-1)
				 dataCosti[depthLeveli] += m_dColorSigma / (m_dColorSigma + m_dColorMissPenalty) * 0.01;
			 else
			 {
				 m_pNearFrames->at(i)->GetColorAt(x2, y2, CorrespondingColor);  

				 //（x,y)位置当前帧和参考帧的颜色差异（论文formula[2]的分母右边部分）
				 double colordist = (fabs(CurrentColor[0] - CorrespondingColor[0]) 
					 + fabs(CurrentColor[1] - CorrespondingColor[1]) 
					 + fabs(CurrentColor[2] - CorrespondingColor[2])) / 3.0;
				 colordist = min(30.0, colordist);      //颜色截断值


				 //P_c 颜色值相似性（参见论文formula[2],实际计算过程中略微有所不同）                                                       
				 //double wc = m_dColorSigma/(m_dColorSigma + colordist);
				 double wc = colorSigma2/(colorSigma2 + colordist*colordist);

				 
				  //p_v 空间一致性（参见论文formula[8],实际计算过程中有所不同）
				 //============================================================================================================
				 double d2 = m_pNearFrames->at(i)->GetDspAt(x2, y2);
				 double d2_INT = m_pNearFrames->at(i)->GetDspAt((int)(x2+0.5F), (int)(y2+0.5F));   //+0.5的作用为四舍五入
				 //dsp = 1.0/z2;

                 //坐标转换（三维投射）的disparity和通过计算的disparity值的差异
				 double dspDiff = min(fabs(d2 - dsp), fabs(d2_INT - dsp));
				 //double dspSigma = (m_dDspMax - m_dDspMin) * m_dDspSigma;

				 double wd = dspSigma2 / (dspSigma2 + dspDiff*dspDiff);      //和论文formula[8]计算过程有所不同

				 dataCosti[depthLeveli] += max(1e-10, wc * wd); // 参见论文formula[7]
			 }
		    }
		 
		 //对相邻几帧累加求得的DataCost求均值
		  dataCosti[depthLeveli] /= frameCount;
         
        //寻找depthLevelI层的最大DataCost（disparity likelihood)值，并设置相应label(bestLabel)值;
		 if(dataCosti[depthLeveli] > MaxLikelihood)
		 {
			 MaxLikelihood = dataCosti[depthLeveli];
			 bestLabel = depthLeveli;
		 }

		// std::cout<<"Best Label: "<<bestLabel<<std::endl;
	 }

	 //将所有层DataCost进行归一化（其值限定在0~1之间）（参见论文formula[3])
	 float maxCost = 1e-10F;
	 for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
	 {
		 dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;   //会将最小的DataCost和最大的DataCost颠倒？
		 maxCost = max(maxCost, dataCosti[depthLeveli]);
	 }
	 for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++)
	 {
		 dataCosti[depthLeveli] /= maxCost;               //归一化后的disparity值
		 dataCosti[depthLeveli] *= m_fDataCostWeight;     // 能量函数中DataCost的权重，为固定值
	 }

}

