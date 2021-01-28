#include "PlanFittingWorkUnit.h"

//默认构造函数


//重载构造函数（成员变量初始化）
PlanFittingWorkUnit::PlanFittingWorkUnit(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, 
   VDRVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost)
	:m_block(block), m_meanShiftSegm(meanShiftSegm), m_SegmPoints(SegmPoints), m_pCurrentFrame(pCurrentFrame), m_labelImg(labelImg), m_dataCost(dataCost)
{

}

PlanFittingWorkUnit::PlanFittingWorkUnit(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, 
	LSVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost)
	:m_block(block), m_meanShiftSegm(meanShiftSegm), m_SegmPoints(SegmPoints), m_pLsCurrentFrame(pCurrentFrame), m_labelImg(labelImg), m_dataCost(dataCost)
{

}

//析构函数
PlanFittingWorkUnit::~PlanFittingWorkUnit(void)
{
}

void PlanFittingWorkUnit::Execute()
{
	DspEstimator_Normal::GetInstance()->PlanfittingOneSegm(m_block,m_meanShiftSegm,m_SegmPoints,m_pCurrentFrame,m_labelImg,m_dataCost);
}

void PlanFittingWorkUnit::LsExecute()
{
	LSDepthEstimator::GetInstance()->PlanfittingOneSegm(m_block,m_meanShiftSegm,m_SegmPoints,m_pLsCurrentFrame,m_labelImg,m_dataCost);
}