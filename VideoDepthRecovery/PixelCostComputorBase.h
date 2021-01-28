#pragma once
#include <vector>
#include "VDRVideoFrame.h"
#include "DataCostUnit.h"
class PixelCostComputorBase
{
public:
	//������  ���캯��
	PixelCostComputorBase(void);
	
	//������  ��������
	virtual ~PixelCostComputorBase(void);

	//������   ���õ�ǰ֡�����ڣ����㣩֡
	virtual void SetFrames(VDRVideoFrame *currentFrame,std::vector<VDRVideoFrame*> * nearFrames);
	
	//������   ������ɫ��disparity��Sigmaֵ��sigma_c��sigma_d)
	void SetSigma(double dColorSigma,double dDspSigma);

	//���㣨x,y)�����DepthLevel���ϵ����DataCost��disparity likelihoodֵ����bestlabel  
	virtual void PixelDataCost(int x,int y,const std::vector<double> &dspV, DataCostUnit &dataCostI, int &bestLabel)=0;


	//�õ�������sigma_d
	double GetDspSigma()
	{
		return m_dDspSigma;
	}
	void SetDspSigma(double sigma)
	{
		m_dDspSigma = sigma;
	}

	//�õ�������sigma_c
	double GetColorSigma()
	{
		return m_dColorSigma;
	}
	void SetColorSigma(double simga)
	{
		m_dColorSigma = simga;
	}

	//�õ�������DataCostWeight
	double GetDataCostWeight(){
		return m_fDataCostWeight;
	}
	void SetDataCostWeight(double weight)
	{
		m_fDataCostWeight = weight;
	}

protected:
	std::vector<VDRVideoFrame*> *m_pNearFrames;    //�뵱ǰ֡�ٽ��Ĳο�ָ֡��
	VDRVideoFrame  *m_pCurrentFrame;       //��ǰָ֡��

	double m_dDspSigma;     //Sigma_d
	double m_dColorSigma;   //Sigma_c
	double m_dColorMissPenalty;    //��һ���ֵ�Ͷ�䵽ͼ���ⲿ���ĳͷ�ֵ��

	float  m_fDataCostWeight;     // ����������DataCost��Ȩ�أ�Ϊ�̶�ֵ

};

