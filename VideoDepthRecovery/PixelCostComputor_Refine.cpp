#include "PixelCostComputor_Refine.h"


//���壺���캯��
PixelCostComputor_Refine::PixelCostComputor_Refine(void)
{
}


//���壺��������
PixelCostComputor_Refine::~PixelCostComputor_Refine(void)
{
}


//���㣨x,y)�����DepthLevel���ϵ����DataCost��disparity likelihoodֵ����bestlabel  
void PixelCostComputor_Refine::PixelDataCost(int x, int y, const std::vector<double> &dspV, DataCostUnit &dataCosti, int &bestLabel)
{
    // std::cout<<"execute BO pixelDataCost"<<std::endl;
	 double MaxLikelihood = 1e-20F;             //���壺����disparity likelihood
	 int frameCount = m_pNearFrames->size();    //�õ��ο�֡��
	 int dspLevelCount = dspV.size();           //�õ�disparity �ֲ���

	 Wml::Vector3d ptWorldCoord;
	 Wml::Vector3d CurrentColor, CorrespondingColor;

	 double colorSigma2 = m_dColorSigma*m_dColorSigma;   //sigma_c��ƽ��������formula[2])
	 double dspSigma2 = m_dDspSigma * m_dDspSigma;       //sigma_d��ƽ�� (����formula[7])

	 m_pCurrentFrame->GetColorAt(x, y, CurrentColor);    //�õ���ǰ֡��x,y��λ�õ���ɫֵ

	 for(int depthLeveli = 0; depthLeveli < dspLevelCount; ++depthLeveli)
	 {		
		 dataCosti[depthLeveli] = 0;     //ÿ��depthLevel����dataCost��ʼֵ
		 m_pCurrentFrame->GetWorldCoordFromImageCoord(x, y, dspV[depthLeveli], ptWorldCoord); //�õ�����ǰ֡��depthLevelI�㣨x,y)ͼ�����������������
		 //double z = 1.0/dspV[depthLeveli];
	     // std::cout<<"dspV[i]:"<<dspV[depthLeveli]<<std::endl;
		// std::cout<<"ptWorldCoord: "<<ptWorldCoord[0]<<","<<ptWorldCoord[1]<<","<<ptWorldCoord[2]<<std::endl;
		 // std::cout<<"frameCount: "<<frameCount<<std::endl;
          
		 for(int i=0; i<frameCount; i++)
		 {
			 double dsp = 0, x2, y2;
			 m_pNearFrames->at(i)->GetImageCoordFromWorldCoord(x2, y2, dsp, ptWorldCoord);    //�����������ptWorldCoord�õ��ο�֡�ڣ�x,y)λ�õ�ͼ�����꣨x2,y2)�Լ�disparityֵ
			 //z2 = r[i][2]*z + b[i][2];
			 //u2 = (r[i][0]*z + b[i][0]) / z2;
			 //v2 = (r[i][1]*z + b[i][1]) / z2;	

			// std::cout<<"x2= "<<x2<<" , "<<"y2= "<<y2<<std::endl;


			 //��άͶ��󳬹�ͼ��߽�ĳͷ�ֵ ���ο����� formula(3)��
			 if(x2<0 || y2<0 || x2 > VDRVideoFrame::GetImageWidth()-1 || y2 > VDRVideoFrame::GetImageHeight()-1)
				 dataCosti[depthLeveli] += m_dColorSigma / (m_dColorSigma + m_dColorMissPenalty) * 0.01;
			 else
			 {
				 m_pNearFrames->at(i)->GetColorAt(x2, y2, CorrespondingColor);  

				 //��x,y)λ�õ�ǰ֡�Ͳο�֡����ɫ���죨����formula[2]�ķ�ĸ�ұ߲��֣�
				 double colordist = (fabs(CurrentColor[0] - CorrespondingColor[0]) 
					 + fabs(CurrentColor[1] - CorrespondingColor[1]) 
					 + fabs(CurrentColor[2] - CorrespondingColor[2])) / 3.0;
				 colordist = min(30.0, colordist);      //��ɫ�ض�ֵ


				 //P_c ��ɫֵ�����ԣ��μ�����formula[2],ʵ�ʼ����������΢������ͬ��                                                       
				 //double wc = m_dColorSigma/(m_dColorSigma + colordist);
				 double wc = colorSigma2/(colorSigma2 + colordist*colordist);

				 
				  //p_v �ռ�һ���ԣ��μ�����formula[8],ʵ�ʼ��������������ͬ��
				 //============================================================================================================
				 double d2 = m_pNearFrames->at(i)->GetDspAt(x2, y2);
				 double d2_INT = m_pNearFrames->at(i)->GetDspAt((int)(x2+0.5F), (int)(y2+0.5F));   //+0.5������Ϊ��������
				 //dsp = 1.0/z2;

                 //����ת������άͶ�䣩��disparity��ͨ�������disparityֵ�Ĳ���
				 double dspDiff = min(fabs(d2 - dsp), fabs(d2_INT - dsp));
				 //double dspSigma = (m_dDspMax - m_dDspMin) * m_dDspSigma;

				 double wd = dspSigma2 / (dspSigma2 + dspDiff*dspDiff);      //������formula[8]�������������ͬ

				 dataCosti[depthLeveli] += max(1e-10, wc * wd); // �μ�����formula[7]
			 }
		    }
		 
		 //�����ڼ�֡�ۼ���õ�DataCost���ֵ
		  dataCosti[depthLeveli] /= frameCount;
         
        //Ѱ��depthLevelI������DataCost��disparity likelihood)ֵ����������Ӧlabel(bestLabel)ֵ;
		 if(dataCosti[depthLeveli] > MaxLikelihood)
		 {
			 MaxLikelihood = dataCosti[depthLeveli];
			 bestLabel = depthLeveli;
		 }

		// std::cout<<"Best Label: "<<bestLabel<<std::endl;
	 }

	 //�����в�DataCost���й�һ������ֵ�޶���0~1֮�䣩���μ�����formula[3])
	 float maxCost = 1e-10F;
	 for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
	 {
		 dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;   //�Ὣ��С��DataCost������DataCost�ߵ���
		 maxCost = max(maxCost, dataCosti[depthLeveli]);
	 }
	 for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++)
	 {
		 dataCosti[depthLeveli] /= maxCost;               //��һ�����disparityֵ
		 dataCosti[depthLeveli] *= m_fDataCostWeight;     // ����������DataCost��Ȩ�أ�Ϊ�̶�ֵ
	 }

}

