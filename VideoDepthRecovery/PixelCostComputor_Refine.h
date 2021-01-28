#pragma once
#include "pixelcostcomputorbase.h"
class PixelCostComputor_Refine : public PixelCostComputorBase
{

private:
	//构造函数
	PixelCostComputor_Refine(void);


public:

	virtual void SetFrames(VDRVideoFrame *CurrentFrame, std::vector<VDRVideoFrame*> *nearFrames) override{
		PixelCostComputorBase::SetFrames(CurrentFrame, nearFrames);
		//m_pOcclutionHandler = NULL;
	}

	//得到该类的一个静态实例
	static PixelCostComputor_Refine * GetInstance()
	{
		 static PixelCostComputor_Refine instance;
		 return &instance;
	}


//(覆盖基类函数）计算（x,y)坐标各DepthLevel层上的最大DataCost（disparity likelihood值）和bestlabel  
virtual void PixelDataCost(int x, int y, const std::vector<double> &dspV, DataCostUnit &dataCosti, int &bestLabel) override;
	
    //析构函数
	~PixelCostComputor_Refine(void);


};

