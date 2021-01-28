#pragma once
#include <vector>
#include "LSVideoFrame.h"
#include "DataCostUnit.h"
#include "LSparawidget.h"
#include "DataCost.h"
// segement
#include "MeanShiftSeg.h"
#include "Block.h"
#include "PlanFittingWorkUnit.h"
//#include "LevenMarOptimizer.h"
#include "ExWRegularGridBP.h"
#include "ZCubeImage.h"
#include "BoxFilter.h"


class LSDepthEstimator
{
public:
	LSDepthEstimator(void);
	~LSDepthEstimator(void);

	// return a static instance of the class
	static LSDepthEstimator *GetInstance()
	{
		static LSDepthEstimator instance;
		return &instance;
	}
	//(init)estimate depth( combine nearby reference frames)
	void EstimateDepth( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, DataCost& DataCost );


	//Bundle Optimization for depth
	void BundleOptimization( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, DataCost& DataCost );

	//Opt Optimization for depth
	void OpticalFlowOptimization( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, DataCost& DataCost );



	//Depth Expansion for depth
    void SuperRefine( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame);


	//get global coord from block coord
	void GetGlobalCoordinateFrmBlockCoordinate(int XinBlock, int YinBlock, int& GlobalX, int& GlobalY )
	{
		GlobalX = m_iOffsetX + XinBlock;
		GlobalY = m_iOffsetY + YinBlock;
	}

	//get init datacost at(x,y) in the image
	void GetInitialDataCostAt(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, 
		int x, int y, DataCostUnit& dataCosti, int& bestLabel, std::vector<float>& dspV, bool ConsiderDspRang );

	//Bundle optimization and get datacost
	void GetBODataCostAt(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int x, int y, 
		DataCostUnit& dataCosti, int& bestLabel, std::vector<float>& dspV, bool DspRang); // the last parameters is dsp range

     //new add..-optical flow calculation-- for temporal consitence
	//Optical Flow and get datacost
	void GetOptDataCostAt(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int x, int y, 
		DataCostUnit& dataCosti, int& bestLabel, std::vector<float>& dspV, bool DspRang); // the last parameters is dsp range



	//get dataCosti[depthLeveli]
	float GetValue(std::vector<float>&  matchCostCandinate, int fwFrameCount, int bwFrameCount);

	// identify if(u,v) in block area
	bool IfInTrueRegion(int XinBlock, int YinBlock)
	{
		if(XinBlock + m_iOffsetX >= m_iTrueX1 && XinBlock + m_iOffsetX <= m_iTrueX2 && YinBlock + m_iOffsetY >= m_iTrueY1 && YinBlock + m_iOffsetY <= m_iTrueY2)
			return true;
		return false;
	}



	//set block realeted information
	void SetBlockState(int trueX1, int trueY1, int trueX2, int trueY2, int offsetX, int offsetY, int BlockWidth,int BlockHeight, int blockIdX, int blockIdY){
		m_iTrueX1 = trueX1;
		m_iTrueY1 = trueY1;
		m_iTrueX2 = trueX2;
		m_iTrueY2 = trueY2;
		m_iBlockWidth = BlockWidth;
		m_iBlockHeight = BlockHeight;
		m_iOffsetX = offsetX;
		m_iOffsetY = offsetY;
		m_iBlockIdx = blockIdX;
		m_iBlockIdy = blockIdY;	
	}

	// output block realeted information
	void PrintBlockState(){
		std::cout <<"BlcokIndex:("<< m_iBlockIdx << "," << m_iBlockIdy << ") OffsetX:" <<  m_iOffsetX << " OffsetY:" << m_iOffsetY << " Width:"
			<< m_iBlockWidth << " Height:" << m_iBlockHeight << " LabelRegion:(" << m_iTrueX1 << "," << m_iTrueY1 << "," << m_iTrueX2 << "," << m_iTrueY2 << ")"<<std::endl;
	}
	//get inital datacost
	void GetInitialDataCost(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames,  LSVideoFrame* pCurrentFrame, 
		DataCost& outDataCost, std::vector<float>& dspV, ZIntImage& labelImg);

	//get BO datacost
	void LSDepthEstimator::GetBODataCost( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame,
		DataCost& outDataCost, std::vector<float>& dspV, ZIntImage& labelImg );


	//new add----optical flow calculation for temporal consistence
	//get Opt datacost
	void LSDepthEstimator::GetOptDataCost( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame,
		DataCost& optDataCost, std::vector<float>& dspV, ZIntImage& labelImg );


	//LQL Version-BP  refine dsp result by BP
   void RefineDspByBP(DataCost& DataCost, ZIntImage& labelImg, bool addEdgeInfo, ZIntImage* offsetImg = NULL);
  

   //KXL Version-BP
   void TileOptimizeGeneral(
	   const int  apronedTileStartX,const int apronedTileStartY, 
	   const int apronedTileEndX,const int apronedTileEndY,
	   std::vector<ZFloatImage*>& matchCostCube,
	   ZIntImage& tmpLabelImg,bool useGaussianFilter = false	);


   //KXL Version-DE'BP
   void TileSubRefineOptimize(
	   const int initLayer,
	   const int subSample,
	   const int  apronedTileStartX,const int apronedTileStartY, 
	   const int apronedTileEndX,const int apronedTileEndY,
	   ZCubeFloatImage& subDataCost,
	   ZIntImage& offsetImg,//just a block
	   ZIntImage& tmpLabelImg//ans
	   );


/*
   void TileOptimizeGeneral(
	   const int  apronedTileStartX,
	   const int apronedTileStartY, 
	   const int apronedTileEndX,
	   const int apronedTileEndY,
	   DataCost& dtCost,
	   ZIntImage& labelImg,bool useGaussianFilter = false	);
*/



bool CopyValueFromDataCostToMatchCostCube(DataCost& dataCost,std::vector<ZFloatImage*>& matchCostCube);



   //1.3_用mean shift 分割结果对深度求解结果进行优化
   //refine dsp result by segement
   void RefineDspBySegm(const Block &block,const MeanShiftSeg &meanShiftSegm, LSVideoFrame* currentFrame, ZIntImage& labelImg, DataCost& DataCost );

   void PlanfittingOneSegm(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, 
	   LSVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost);
 
   float GetDspAtLeveli(LSVideoFrame* pCurrentFrame, float leveli);


   //Depth expansion (DE)
 //  void _SuperRefine(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int subIterCount, LSVideoFrame::LABEL_IMG_TYPE type);

   void _SuperRefine(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int subIterCount, LSVideoFrame::LABEL_IMG_TYPE type,ZIntImage& labelImg);


   // get initial datacost
   void GetSupreRefineDataCost( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, 
	   DataCost& outDataCost, ZIntImage& offsetImg, int totalDspLevel, int subSample, ZIntImage& labelImg, LSVideoFrame::LABEL_IMG_TYPE type);


   //new add-for optical flow
 //  ZFloatImage * LoadOptFlwImgU(int camId,const std::string & foreFrame,const std::string & backFrame);
 //  ZFloatImage * LoadOptFlwImgV(int camId,const std::string & foreFrame,const std::string & backFrame);

   //同时加载u方向和v方向上的光流
 //  ZFloatImage  LoadForeOptFlwImg(int camId,const std::string & foreFrame,const std::string & backFrame);
   


   /*
   float GetOptAt(ZFloatImage * optFlwImg,int x,int y)
   {
	   return optFlwImg->GetPixel(x,y,0);
   }
   */


private: 
	//============Block states =============
	int m_iBlockIdx, m_iBlockIdy;    //block index number in x ray and y ray
	int m_iOffsetX, m_iOffsetY;      //offset distance from original image coord
	int m_iBlockWidth, m_iBlockHeight;  //block size(width and height)
	int m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2;  // practical start(x,y) and end(x,y) on the block

	ZFloatImage* m_pOptFlwImg;  //disparity image

};

