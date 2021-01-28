#pragma once
#include "ImagePartition/LSImgPartition.h"
#include "DataCost.h"
//#include "DataCostUnit.h"
#include <vector>
#include <iostream>

class LSBeliefPropagation
{
public:
	//LSBeliefPropagation(void);
	LSBeliefPropagation(
		LSImgPartition& imgPartition,
		float disc_k,
		int max_d, 
		float sigma=0.7F, 
		int nIter=5, 
		int nLevels=5,
		/*int m_iThreadsCount = 2,*/
		int* segmId = NULL
		);

	~LSBeliefPropagation(void);

	//output BP parameters
	void printPara();

	//execute BP procedure
	void run(DataCost& DataCost, ZIntImage& LabelImg, bool addEdgeInfo, ZIntImage* offsetImg = NULL);

   //stereo match on block
   void stereoMatchingAt(DataCost& mDataCost, ZIntImage& LabelImg, ZIntImage* offsetImg, int blockWidth, int blockHeight, int TrueX1, int TrueY1, int TrueX2, int TrueY2);

   //belief propagation using checkerboard update scheme
   void translateMessage(float* ui, float* di, float* li, float* ri, DataCost* dataCosti, int widhti, int heighti, ZIntImage* offsetImg);


   //use bp on exact pixel
   void translateMessageAt(int x, int y, float* ui, float* di, float* li, float* ri, DataCost* dataCosti, int widhti, ZIntImage* offsetImg);


   void updateMessage(float* srcMsg1, float* srcMsg2, float* srcMsg3, DataCostUnit& dataCost,float* dstMsg, int offset);

   void updateMessage(float* dstMsg);

   void GetDepth(float* u0, float* d0, float* l0, float* r0, DataCost* dataCost, ZIntImage& DepthVisibleImg,  
	   int blockWidth, int blockHeight , int TrueX1, int TrueY1, int TrueX2, int TrueY2);

private:
	float m_fDisck;

	float m_fSigma;

	int m_iMaxD;

	int m_iIterCount;   //Iter Count

	int m_iLevelsCount;  
	//=============================
	//int m_iThreadsCount;
	//============================
	LSImgPartition& m_ImgPartion;   //ImgPatition object
	int m_iBlockIdX;                // block index in x ray
	int m_iBlockIdy;                // block index in y ray
	int m_iOffsetX;                 // coord in block offset from original image
	int m_iOffsetY;
	//=============================
	int* m_pSegmId;



};

