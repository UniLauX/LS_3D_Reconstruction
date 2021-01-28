#include "PixelCostComputorBase.h"


//构造函数，对成员变量进行初始化
PixelCostComputorBase::PixelCostComputorBase(void)
{
	m_pNearFrames=nullptr;
	m_pCurrentFrame=nullptr;
	m_fDataCostWeight=1.0;
	m_dColorMissPenalty=30;
}

//析构函数
PixelCostComputorBase::~PixelCostComputorBase(void)
{

}

//设置当前帧和相邻（计算）帧
void PixelCostComputorBase::SetFrames(VDRVideoFrame *currentFrame,std::vector<VDRVideoFrame*> * nearFrames)
{
	m_pCurrentFrame=currentFrame;
	m_pNearFrames=nearFrames;
}

// 设置颜色和disparity的Sigma值（sigma_c和sigma_d)
void PixelCostComputorBase::SetSigma(double dColorSigma,double dDspSigma)
{
	m_dColorSigma=dColorSigma;
	m_dDspSigma=dDspSigma;
}






