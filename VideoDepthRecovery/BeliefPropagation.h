#pragma once
#include "ZImage.h"
#include "DataCost.h"
#include "DataCostUnit.h"
//#include "DataCostWorkUnit.h"
//class Block;
#include "Block.h"
#include <iostream>
#include <Windows.h>
class BeliefPropagation
{
public:
	//构造函数
	//BeliefPropagation(void);
	BeliefPropagation(float disc_k,int max_d, float sigma=0.7F,int nIter=5, int nLevels=5/*,int m_iThreadsCount = 2*/);



	//析构函数
	~BeliefPropagation(void);

	//打印（输出）参数信息
	void printPara();

	
	//运行BP(构建数据金字塔等）
	void run(const Block &block, DataCost& dataCost, ZIntImage& LabelImg, ZIntImage* offsetImg = NULL );
	void translateMessageAt(int x, int y, float* ui, float* di, float* li, float* ri, DataCost* dataCosti, int widhti, ZIntImage* offsetImg, const Block &block);



private:
	float m_fDisck;   //平滑截断项
	float m_fSigma;   
	int m_iMaxD;       //disparity层数
	int m_iIterCount;   //迭代次数
	int m_iLevelsCount; //计算层数
	//int m_iThreadsCount;  //并行线程数

    void translateMessage(float* ui, float* di, float* li, float* ri, DataCost* dataCosti, int widthi, int heighti, ZIntImage* offsetImg, const Block &block);	

	void updateMessage(float* srcMsg1, float* srcMsg2, float* srcMsg3, DataCostUnit& dataCost,float* dstMsg, int offset);

	void GetDepth(const Block &block, float* u0, float* d0, float* l0, float* r0, DataCost* dataCost, ZIntImage& DepthVisibleImg);

};

