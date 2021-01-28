//#include "LSStructureMotion.h"
#pragma once
#include "wmlvector3.h"
#include "wmlmatrix3.h"
#include "AutoVoxel.h"
#include "GMM.h"      //2D
#include "GMM3D.h"
#include <math.h>
#include <iostream>
#include <vector>
#include "LSVideoFrame.h"
#include "GCApplication.h"
#include "GrabCut2D.h"
class LSStructureMotion;
//AutoVisualHull Method

enum
{
	GC3D_BGD    = 0,  //!< background
	GC3D_FGD    = 1,  //!< foreground
	GC3D_PR_BGD = 2,  //!< most probably background
	GC3D_PR_FGD = 3   //!< most probably foreground 
};
class AVisualHull
{
public:
	AVisualHull(void);
	~AVisualHull(void);

public:
	

	//Free the memory occupied by data structure
	void Clear();

	//method1: through background subtraction,not enlarge the volume
	//Init the cube and all voxel information (and generate the Volume according to the BoundingBox)
	void InitGenerateVolume(const Wml::Vector3d& LowCorner,const Wml::Vector3d& HighCorner, int iMaxIntervalVoxel, int iImgApron);

	//method1: through background subtraction
	// Generate Labels in volume according cast result
	void GenerateSeedVoxels(LSStructureMotion* pMotion, int iStart, int iEnd);
	

	//method2: through seed ball enlarge
	void GenerateSeedVoxelBall(LSStructureMotion* pMotion, int iStart, int iEnd,const Wml::Vector3d& LowCorner,const Wml::Vector3d& HighCorner);   //
	//Get VisualHull file name&path
	std::string  GetVisualHullFileName();

	// Check whether current voxel is all in mask (and select the voxel cast in all masks as the seed voxels）
	VoxelType  CheckVoxel(AVoxel* curVoxel);	

	VoxelType CheckVoxel1(AVoxel* curVoxel); //new-add for 3维上补全二维


	//Check  whether current voxel is cast in mask of one camFrame
	VoxelType IsVoxelInMaskFaceSamp(AVoxel* curVoxel, LSVideoFrame* curVideoFrame);

	//Generate some tmp files(such as cast image/ m_maskimage)

	bool TmpGenerateFunc(/*LSStructureMotion* pMotion, int iStart, int iEnd*/);

    //Cast VisualHull of current iteration to image mask
    bool CastVisualHulltoMask(/*LSStructureMotion* pMotion, int iStart, int iEnd*/);

	//Execute 3D-GrabCut on voxelVolume
	void Execute3DGraphCut(int iterCount,bool isConverged ); //paper method
	void Execute3DGraphCut1(cv::Mat _bgdModel,cv::Mat _fgdModel,int iterCount,bool isConverged );  //avgColor method
	
	void InitGMMs(const std::vector<AVoxel*>& volumeVoxels, const std::vector<AVoxel*>& maskVoxels, GMM3D& bgdGMM, GMM3D& fgdGMM );  //此处maskVoxels和二维上mask上有所不同，其只是起到在Object中确定Fgd的像素
	double CalcBeta( const std::vector<AVoxel*>& volumeVoxels,int &edgeNum );


	void CalcNWeights( const std::vector<AVoxel*>& volumeVoxels, std::vector<double>& leftW, std::vector<double>& upW, std::vector<double>& frontW, double gamma,int& edgeNum ); //Paper method
	//目前是按照不同相机的平均color值进行计算的，也可以按自动求VisualHull论文中的方法计算。
	void CalcNWeights( const std::vector<AVoxel*>& volumeVoxels, std::vector<double>& leftW, std::vector<double>& upW, std::vector<double>& frontW, double beta, double gamma ); //avgcolor method
   
	//Assign GMMs components for each voxel.
    //static void AssignGMMsComponents( const std::vector<AVoxel*>& volumeVoxels, const std::vector<AVoxel*>& maskVoxels, const GMM& bgdGMM, const GMM& fgdGMM, Mat& compIdxs );
	void AssignGMMsComponents( const std::vector<AVoxel*>& volumeVoxels, const std::vector<AVoxel*>& maskVoxels, const GMM3D& bgdGMM, const GMM3D& fgdGMM/*, Mat& compIdxs */);

    //Learn GMMs parameters.
    //static void LearnGMMs( const std::vector<AVoxel*>&volumeVoxels, const std::vector<AVoxel*>& maskVoxels,/*const Mat& compIdxs,*/ GMM& bgdGMM, GMM& fgdGMM )
    void LearnGMMs( const std::vector<AVoxel*>&volumeVoxels, const std::vector<AVoxel*>& maskVoxels,GMM3D& bgdGMM, GMM3D& fgdGMM );

    
    //Construct GCGraph

	//M2:paper method
	void ConstructGCGraph( const std::vector<AVoxel*>&volumeVoxels,double lambda,
		const std::vector<double>& leftW, const std::vector<double>& upW, const std::vector<double>& frontW,GCGraph<double>& graph );
	//M1:avgcolor method
	 void ConstructGCGraph( const std::vector<AVoxel*>&volumeVoxels, const std::vector<AVoxel*>& maskVoxels, const GMM3D& bgdGMM, const GMM3D& fgdGMM, double lambda,
		const std::vector<double>& leftW, const std::vector<double>& upW, const std::vector<double>& frontW,GCGraph<double>& graph );

	
     //Estimate segmentation using MaxFlow algorithm
     // static void estimateSegmentation( GCGraph<double>& graph, Mat& mask )
     void EstimateSegmentation( GCGraph<double>& graph,const std::vector<AVoxel*>&volumeVoxels);

	
     //2D-GrabCut
	 void Execute2DGrabCut();  //papar method
	 void Execute2DGrabCut1(); //calculate according average color

	 //Calculate Distance between two voxels
	 inline double GetDistanceFromCenter(AVoxel*& targetVoxel,AVoxel* & centerVoxel)
	 {
		 double dist=0;
		 dist=((targetVoxel->m_iLowX-centerVoxel->m_iLowX)*(targetVoxel->m_iLowX-centerVoxel->m_iLowX)+(targetVoxel->m_iLowY-centerVoxel->m_iLowY)*(targetVoxel->m_iLowY-centerVoxel->m_iLowY)
			 +(targetVoxel->m_iLowZ-centerVoxel->m_iLowZ)*(targetVoxel->m_iLowZ-centerVoxel->m_iLowZ))*m_dCellLength*m_dCellLength;
		 return dist;
	 }


	//通过k-means算法来初始化背景GMM和前景GMM模型  
  //Initialize GMM background and foreground models using kmeans algorithm. 
  
 
public:
		int edgeNumbers;             //edge number formed by the 3D voxels
		int m_iCellNumInX;           // cells number in X axis 
		int m_iCellNumInY;           // cells number in Y axis
		int m_iCellNumInZ;           // cells number in Z axis 

		int m_iImgWidth;
		int m_iImgHeight;
		std::vector<Mat>m_vProbFgdImgs;  //store 2D GrabCut result
		std::vector<double>m_vBetas;   //store beta calculated from images

		//Wml::Vector3d m_wCentraPoint;   //CenraPoint of volume;
		AVoxel *m_avCenterVoxel;         //Center Voxel of Volume;
		
private:
	int m_iStartFrame;            //start frame number 
	int m_iEndFrame;              //end frame number 
	int m_iCurFrame;              //current frame number (add by ly 3/13/2014)
	std::string m_sFrameName;     // frameName represent by number (add by ly 3/13/2014)
	LSStructureMotion* m_pMotion; //LSStructureMotion object


	Wml::Vector3d m_LowCorner;  //minBox coordinate
	Wml::Vector3d m_HighCorner; //maxBox coordinate
	int m_iMaxIntervalVoxel;    //voxel number in largest edge of boundingBox among x,y,z axis (100)    
	int m_iImgApron;            //image extension in decide wether a pixel is in mask area (30)

	double m_dMaxLen;            // the max distance among x,y,z( HighCorner to LowCorner）
	double m_dCellLength;        // small cell width (aka. voxel size)
	VoxelCoordBase *m_VoxelBase;  // pointer point to smallest voxel 
	std::vector<AVoxel*> m_VoxelVolume;  //tmp var, store for the voxels which belongs to the VisualHull of current iteration
	std::vector<AVoxel*>m_SeedVoxels;   //used for VisualHull Initialization.

	
};

