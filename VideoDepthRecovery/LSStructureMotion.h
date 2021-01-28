/*******************************************************************/
/* note: LSStructureMotion.h and LSStructureMotion.cpp are write for Lightstage 
/* write by Yu Liu;   02/25/2014    Email: UniLau13@gmail.com
/********************************************************************/
#pragma once
#include <Windows.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "ximage.h"
#include "CxImageFunction.h"
#include "FileNameParser.h"
#include "CamFileParser.h"

#include "LSVideoFrame.h"
#include "LSparawidget.h"

#include "VisualHull/VisualHull.h"

#include "AutoVisualHull.h"


#include"SelectPoint.h"
#include "SelectPointParam.h"

//class SelectPoint;
//VisualHull
//#include "VisualHull/BoxCalculator.h"


#ifndef _LSStructureMotion
#define _LSStructureMotion

class LSStructureMotion
{
public:
	LSStructureMotion(void);
	~LSStructureMotion(void);


	//get a static instance
   static LSStructureMotion* GetInstance()
   {
	   static LSStructureMotion instance;
	   return &instance;
   }

   //load the lightstage project
   bool LS_LoadProject(const std::string &cameraListPath);
 
   //create frames for all cam
   void LS_CreateFrames(int iCount);
   

   // get ith frame of the camId
   LSVideoFrame* GetFrameAt(int cameraId, int index){
	   return m_frames.at(cameraId * m_iFramCount + index);
   }


   //get the frame count be loaded
   int GetFrameCount()
   {
	   return m_iFramCount;
   }

   void InitMaxMaskRange(int start, int end);

   // calculate Box for every frame
   bool CalculateBox(int startFrame, int endFrame);

   //for seedball based method: enlarge boundingbox
   bool CalculateEnlargeBox(int startFrame,int endFrame);

   //calculate VisualHull for every frame
   void CalculateVisualHull(int startFrame, int endFrame);

   //Combine 3D-GraphCut and Background subtraction automatically get the VisualHull  (code by Ly,3/21/2015) 
   void AutoCalcVisualHull(int startFrame,int endFrame);

   
   //run init for depth recovery
   void RunInitialization(int startFrame, int endFrame);

   //run refine for depth recovery
   void RunRefine(int startFrame, int endFrame);

   
   //new add..Optical flow information
   void RunOptRefine(int startFrame,int endFrame);


   //run DE for depth recovery
   void RunDepthExpansion(int startFrame, int endFrame);

   //run depth to Mesh

   //ORG-LQL version
   void DepthToMesh( int startFrame, int endFrame);

   //NEW ADD-KXL version
   void _DepthToMesh(LSStructureMotion* pMotion,int startFrame,int endFrame,SelectPointParam& param);

   //run all operations for lightstage system
   void RunAll(int startFrame, int endFrame);

   //NEW ADD-KXL version
   void _OnMesh(int startFrame,int endFrame);


   /*
   bool LS_InitFrames(const std::string &actsFilePath);   //加载视频序列，针对lightstage中的cam文件

   bool CalculateBox(int startFrame, int endFrame);  //计算boundingBox

   void CalculateVisualHull(int startFrame, int endFrame);
   void LS_RunInit(int startFrame,int endFrame);
   void LS_RunRefine(int startFrame,int endFrame);
   void LS_RunMesh(int startFrame,int endFrame);
   */

public:
	std::string m_sSystemDir;   // the system work directory
	std::string m_sPossionPath; //  location of the possion exe
	std::string m_sMeshDir;     // the mesh file directory
	std::string m_sVisualHullDir;  // the visual hull file directory
    std::vector<std::string> m_sWorkDirs;   // the different camera file directory
    
	// for BoundingBox /  should be a vector or an array for different frames
	std::vector<Wml::Vector3d> m_BoxMinXYZ;
	std::vector<Wml::Vector3d> m_BoxMaxXYZ;
	std::vector<float> m_fDspMin;
	std::vector<float>m_fDspMax;
	

	//For seedball based method, enlarge BoundingBox to make sure the outer voxel label as GC_BGD
	std::vector<Wml::Vector3d> m_LgrBoxMinXYZ;  
	std::vector<Wml::Vector3d>m_LgrBoxMaxXYZ;



	//for Auto VisualHull 
	//std::vector<std::vector<AVoxel*>>VolumeVoxels
	//std::vector<AVoxel*>& colorModelVoxels,Wml::Vector3d minXYZ, Wml::Vector3d maxXYZ, cv::Mat bgdModel,cv::Mat fgdModel,*/bool isConverged

protected:
	std::vector<LSVideoFrame*>m_frames;    // total frames that the lightstage project should have created.
	int m_iFramCount;   //frame numbers in a single framDirectory
	int m_iCamNum;    // the number of camera

	int m_iMaxMaskWidth;
	int m_iMaxMaskHeight;

public:
	//vector<*CamFileParser>m_pCamFiles;
	std::vector<std::shared_ptr<CamFileParser>> m_pCamFiles;
	CamFileParser *camFiles;


	//CVisualHull m_VisualHull;  // a Visualhull object
	std::vector<CVisualHull>m_VisualHull;

	//AutoVisualHull  Object
	std::vector<AVisualHull>m_ATVisualHull;

	



	// image partition
    //ImgPartitionBase * m_pImgPartition; 
};

#endif  _LSStructureMotion