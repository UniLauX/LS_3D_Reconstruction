#pragma once
#include "pixelcostcomputorbase.h"
class PixelCostComputor_Refine : public PixelCostComputorBase
{

private:
	//���캯��
	PixelCostComputor_Refine(void);


public:

	virtual void SetFrames(VDRVideoFrame *CurrentFrame, std::vector<VDRVideoFrame*> *nearFrames) override{
		PixelCostComputorBase::SetFrames(CurrentFrame, nearFrames);
		//m_pOcclutionHandler = NULL;
	}

	//�õ������һ����̬ʵ��
	static PixelCostComputor_Refine * GetInstance()
	{
		 static PixelCostComputor_Refine instance;
		 return &instance;
	}


//(���ǻ��ຯ�������㣨x,y)�����DepthLevel���ϵ����DataCost��disparity likelihoodֵ����bestlabel  
virtual void PixelDataCost(int x, int y, const std::vector<double> &dspV, DataCostUnit &dataCosti, int &bestLabel) override;
	
    //��������
	~PixelCostComputor_Refine(void);


};

