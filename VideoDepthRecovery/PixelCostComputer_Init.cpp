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
	double maxLikelihood=1e-20F;    //��������disparity likelihood ֵ
    int frameCount=m_pNearFrames->size();
	int dspLevelCount=dspV.size();

	double colorSigmaSquare=m_dColorSigma*m_dColorSigma;  // sigma_c��ƽ��

	Wml::Vector3d ptWorldCoord;
	Wml::Vector3d currentColor,correspondingColor;

	m_pCurrentFrame->GetColorAt(x,y,currentColor);


	//�ڸ���level����(x,y)��DataCost��������Ӧ�����label(bestLabel)ֵ
    for(int depthLevelI=0;depthLevelI<dspLevelCount;depthLevelI++)
	{
		dataCostI[depthLevelI]=0;

		//�õ�����ǰ֡��depthLevelI�㣨x,y)ͼ�����������������
        m_pCurrentFrame->GetWorldCoordFromImageCoord(x,y,dspV[depthLevelI],ptWorldCoord);

	    for(int i=0;i<frameCount;i++)
		{
			double dsp=0;
			double x2,y2;
	     
			//�����������ptWorldCoord�õ�����֡�ڣ�x,y)λ�õ�ͼ�����꣨x2,y2)�Լ�disparity
			m_pNearFrames->at(i)->GetImageCoordFromWorldCoord(x2,y2,dsp,ptWorldCoord);

			//��άͶ��󳬹�ͼ��߽�ĳͷ�ֵ ���ο����� formula(3)��
			if(x2<0 || y2<0 || x2 > VDRVideoFrame::GetImageWidth()-1 || y2 > VDRVideoFrame::GetImageHeight()-1)
				dataCostI[depthLevelI] += m_dColorSigma / (m_dColorSigma + (m_dColorMissPenalty/3)*(m_dColorMissPenalty/3));
			else
			{
				m_pNearFrames->at(i)->GetColorAt(x2, y2, correspondingColor);
				double colorDist = (fabs(currentColor[0] - correspondingColor[0]) 
					+ fabs(currentColor[1] - correspondingColor[1]) 
					+ fabs(currentColor[2] - correspondingColor[2])) / 3.0;

						colorDist = min(30.0, colorDist);       //30.0Ϊ��ɫ�ͷ������ֵ
						
				        //���ο�����formula(3)��
						double wc = colorSigmaSquare/(colorSigmaSquare+ colorDist*colorDist);
						                      //wc = colorSigmaSquare/(colorSigmaSquare+ colorDist);     

						//depthLevelI�������أ�x,y)��DataCostֵ�����ڼ�֡���ۼӣ�
						dataCostI[depthLevelI]+=wc;			   
			}
	
		}

		//�����ڼ�֡�ۼ���õ�DataCost���ֵ
	     dataCostI[depthLevelI]/=frameCount;    

		 //Ѱ��depthLevelI������DataCost��disparity likelihood)ֵ����������Ӧlabel;
		   if(dataCostI[depthLevelI]>maxLikelihood)
		   {
			   maxLikelihood=dataCostI[depthLevelI];
			   bestLabel=depthLevelI;
		   }

	}

	std::cout<<"bestLabel:"<<bestLabel<<std::endl;
	//�����в�DataCost���й�һ������ֵ�޶���0~1֮�䣩
	float maxCost = 1e-6F;     
	for(int depthLevelI = 0; depthLevelI < dspLevelCount; depthLevelI++)
	{
		dataCostI[depthLevelI] =  1.0 - dataCostI[depthLevelI] / maxLikelihood;   //�Ὣ��С��DataCost������DataCost�ߵ���
		maxCost = max(maxCost, dataCostI[depthLevelI]);      
	}


	for(int depthLevelI=0; depthLevelI<dspLevelCount; depthLevelI++)
	{
		dataCostI[depthLevelI] /= maxCost;
		dataCostI[depthLevelI] *= m_fDataCostWeight;              //���������Ϊlamda*DataCost[depthLevelI](�����ֵΪ0~1�ķ�Χ��
	}
}
*/

//���㣨x,y)�����DepthLevel���ϵ����DataCost��disparity likelihoodֵ����bestlabel
void PixelCostComputer_Init::PixelDataCost( int x, int y, const std::vector<double> &dspV, DataCostUnit &dataCosti, int &bestLabel )
{

	//m_dColorSigma=5;
	double MaxLikelihood = 1e-20F;    //��������disparity likelihoodֵ
	int frameCount = m_pNearFrames->size();  //�õ��ο�֡��
	int dspLevelCount = dspV.size();    // dspLevelCount=101
	

	double colorSigma2 = m_dColorSigma*m_dColorSigma;     //sigma_c��ƽ����m_dColorSigma=5��

	Wml::Vector3d ptWorldCoord;
	Wml::Vector3d CurrentColor, CorrespondingColor;

	m_pCurrentFrame->GetColorAt(x, y, CurrentColor);   //�õ�(x,y)λ�����ص���ɫֵ

    //�ڸ���level����(x,y)��DataCost��������Ӧ�����label(bestLabel)ֵ
	for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
	{		
		dataCosti[depthLeveli] = 0;
	
	//�õ�����ǰ֡��depthLevelI�㣨x,y)ͼ�����������������
    m_pCurrentFrame->GetWorldCoordFromImageCoord(x, y, dspV[depthLeveli], ptWorldCoord);

    // std::cout<<"dspV[i]:"<<dspV[depthLeveli]<<std::endl;
    // std::cout<<"ptWorldCoord: "<<ptWorldCoord[0]<<","<<ptWorldCoord[1]<<","<<ptWorldCoord[2]<<std::endl;
	// std::cout<<"frameCount: "<<frameCount<<std::endl;
	// double z = 1.0/dspV[depthLeveli];

		for(int i=0; i<frameCount; i++)
		{

		    //�����������ptWorldCoord�õ��ο�֡�ڣ�x,y)λ�õ�ͼ�����꣨x2,y2)�Լ�disparityֵ
			double dsp = 0, x2, y2;
			m_pNearFrames->at(i)->GetImageCoordFromWorldCoord(x2, y2, dsp, ptWorldCoord);
			//z2 = r[i][2]*z + b[i][2];
			//u2 = (r[i][0]*z + b[i][0]) / z2;
			//v2 = (r[i][1]*z + b[i][1]) / z2;	
			//std::cout<<"x2= "<<x2<<" , "<<"y2= "<<y2<<std::endl;

		   //��άͶ��󳬹�ͼ��߽�ĳͷ�ֵ ���ο����� formula(3)��
			if(x2<0 || y2<0 || x2 > VDRVideoFrame::GetImageWidth()-1 || y2 > VDRVideoFrame::GetImageHeight()-1)
				dataCosti[depthLeveli] += m_dColorSigma / (m_dColorSigma + (m_dColorMissPenalty/3)*(m_dColorMissPenalty/3));
			else
			{
				//��x,y)λ�õ�ǰ֡�Ͳο�֡����ɫ���죨����formula[2]�ķ�ĸ�ұ߲��֣�
				m_pNearFrames->at(i)->GetColorAt(x2, y2, CorrespondingColor);
				double colordist = (fabs(CurrentColor[0] - CorrespondingColor[0]) 
					+ fabs(CurrentColor[1] - CorrespondingColor[1]) 
					+ fabs(CurrentColor[2] - CorrespondingColor[2])) / 3.0;

				colordist = min(30.0, colordist);           //30.0Ϊ��ɫ�ͷ������ֵ
				

				//���ο�����formula(3)��
				double wc = colorSigma2/(colorSigma2 + colordist*colordist);
				//double wc = m_dColorSigma/(m_dColorSigma + colordist);


				//depthLevelI�������أ�x,y)��DataCostֵ�����ڼ�֡���ۼӣ�
				dataCosti[depthLeveli] += wc; //m_dColorSigma / (m_dColorSigma + colordist * colordist);
			}
		}

	   //�����ڼ�֡�ۼ���õ�DataCost���ֵ
		dataCosti[depthLeveli] /= frameCount;

	   //Ѱ��depthLevelI������DataCost��disparity likelihood)ֵ����������Ӧlabel(bestLabel)ֵ;
		if(dataCosti[depthLeveli] > MaxLikelihood){
			MaxLikelihood = dataCosti[depthLeveli];
			bestLabel = depthLeveli;
		}
	}

  // std::cout<<"BestLabel: "<<bestLabel<<std::endl;

	//�����в�DataCost���й�һ������ֵ�޶���0~1֮�䣩���μ�����formula[3])
	float maxCost = 1e-6F;
	for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++){
		dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;   //�Ὣ��С��DataCost������DataCost�ߵ���
		maxCost = max(maxCost, dataCosti[depthLeveli]);
	}
	for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++){
		dataCosti[depthLeveli] /= maxCost;
		dataCosti[depthLeveli] *= m_fDataCostWeight;                              //���������Ϊlamda*DataCost[depthLevelI](�����ֵΪ0~1�ķ�Χ��
	}
}
