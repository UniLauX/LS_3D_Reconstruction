#pragma once
#include "DspEstimatorBase.h"
#include "VDRStructureMotion.h"
#include "depthparawidget.h"

#include "PixelCostComputer_Init.h"
#include "PixelCostComputor_Refine.h"

#include "MeanShiftSeg.h"
#include <iostream>

#include "DataCost.h"
#include "DataCostWorkUnit.h"

#include "BeliefPropagation.h"

#include <iostream>
#include "Block.h"
#include "PlanFittingWorkUnit.h"
#include "LevenMarOptimizer.h"

//#include <WinDef.h>
//#include <Windows.h>

class DspEstimator_Normal:
	public DspEstimatorBase
{
private:
	DspEstimator_Normal(void);

public:
	static DspEstimator_Normal *GetInstance();

	~DspEstimator_Normal(void);
    
	 //覆盖在DspEstimatorBase中的函数声明
	virtual void Run_Init(int start,int end) override;  
	virtual void Run_BO(int start,int end,int pass) override;
	virtual void Run_DE(int start,int end,int pass) override;


	void PlanfittingOneSegm(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, 
		VDRVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost);


private:
	//得到leveli层上的disparity值
	float GetDspAtLeveli(float leveli);

	//在当前帧上进行初始化求disparity，被Run_Init调用
	bool Run_Init_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost);

    //在当前帧上应用BO，优化Init所求disparity的结果，被Run_BO调用
	bool Run_BO_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost);

	//在当前帧上应用DE,在BO之后综合应用空间，时域和SFM的特征点信息对disparity结果优化；被Run_DE调用
	bool Run_DE_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost, int iters, int expanLevls);



	//初始化DataCost并得到labelImage
	void GetDataCost(const Block &block,DataCost &outDataCost,ZIntImage & lableImg,const std::vector<double> &dspV,ZIntImage *pOffsetImg);

	//1_2,调用BP进行优化
	void RefineDspByBP(float disck, const Block &block, DataCost &DataCost, ZIntImage &labelImg, bool addEdgeInfo, ZIntImage* offsetImg = NULL);


	//1_3,用MeanShift的分割结果进行优化
	void RefineDspBySegm( const Block &block, const MeanShiftSeg &meanShiftSegm, VDRVideoFrame* currentFrame, ZIntImage& labelImg, DataCost& DataCost );
	//void RefineDspByBP()



private:
	//检查图像加载、分块等信息（是否正确）
	bool check();

};

