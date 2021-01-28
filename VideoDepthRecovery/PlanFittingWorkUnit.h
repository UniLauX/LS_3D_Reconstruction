#pragma once
#include "Block.h"
#include "MeanShiftSeg.h"
#include "VDRVideoFrame.h"
#include "DataCost.h"
#include "DspEstimator_Normal.h"

//LS
#include "LSVideoFrame.h"
#include "LSDepthEstimator.h"

class PlanFittingWorkUnit
{
public:
    //构造函数	
	PlanFittingWorkUnit(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, 
		VDRVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost);
	
	//重建构造函数
	PlanFittingWorkUnit(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, 
		LSVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost);
	//析构函数
	~PlanFittingWorkUnit(void);


	virtual void Execute();

	//for lightstage
	virtual void LsExecute();


	//meanShift 参数
private:
	const Block &m_block;
	const MeanShiftSeg &m_meanShiftSegm;
	const std::vector<blk::Point> &m_SegmPoints;
	VDRVideoFrame *m_pCurrentFrame;
	ZIntImage &m_labelImg;
	DataCost &m_dataCost;

	LSVideoFrame *m_pLsCurrentFrame;
};

