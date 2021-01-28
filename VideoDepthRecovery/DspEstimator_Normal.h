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
    
	 //������DspEstimatorBase�еĺ�������
	virtual void Run_Init(int start,int end) override;  
	virtual void Run_BO(int start,int end,int pass) override;
	virtual void Run_DE(int start,int end,int pass) override;


	void PlanfittingOneSegm(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, 
		VDRVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost);


private:
	//�õ�leveli���ϵ�disparityֵ
	float GetDspAtLeveli(float leveli);

	//�ڵ�ǰ֡�Ͻ��г�ʼ����disparity����Run_Init����
	bool Run_Init_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost);

    //�ڵ�ǰ֡��Ӧ��BO���Ż�Init����disparity�Ľ������Run_BO����
	bool Run_BO_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost);

	//�ڵ�ǰ֡��Ӧ��DE,��BO֮���ۺ�Ӧ�ÿռ䣬ʱ���SFM����������Ϣ��disparity����Ż�����Run_DE����
	bool Run_DE_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost, int iters, int expanLevls);



	//��ʼ��DataCost���õ�labelImage
	void GetDataCost(const Block &block,DataCost &outDataCost,ZIntImage & lableImg,const std::vector<double> &dspV,ZIntImage *pOffsetImg);

	//1_2,����BP�����Ż�
	void RefineDspByBP(float disck, const Block &block, DataCost &DataCost, ZIntImage &labelImg, bool addEdgeInfo, ZIntImage* offsetImg = NULL);


	//1_3,��MeanShift�ķָ��������Ż�
	void RefineDspBySegm( const Block &block, const MeanShiftSeg &meanShiftSegm, VDRVideoFrame* currentFrame, ZIntImage& labelImg, DataCost& DataCost );
	//void RefineDspByBP()



private:
	//���ͼ����ء��ֿ����Ϣ���Ƿ���ȷ��
	bool check();

};

