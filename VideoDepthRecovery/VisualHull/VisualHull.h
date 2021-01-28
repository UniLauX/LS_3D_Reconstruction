#pragma once

#include "wmlvector3.h"
#include "Voxel.h"
#include <vector>
#include "SoftRasterize.h"
#include <string>

class LSStructureMotion;

class CVisualHull
{
public:
	CVisualHull();
	~CVisualHull(void);

public:
	//Calculate VisualHull
	void CalVisualHull(LSStructureMotion* pMotion, int iStart, int iEnd);
	

	bool LoadVisualHull(LSStructureMotion* pMotion);


    //Init the cube and all voxel infomation
	void Init(const Wml::Vector3d& LowCorner,const Wml::Vector3d& HighCorner, int iDetailLevel, int iImgApron);
	
	//Free the memory occupied by data strcuture
	void Clear();

	//Get VisualHull file name&path
	std::string  GetVisualHullFileName();

private:
	bool PlyReader(std::string  plyFile,std::vector<IdxTraingle>& triangleList, std::vector<Wml::Vector3f>& vertexList);

	//Octree method for VisualHull(surface voxels)
	void OctVH();
	
	// Check wether current voxel is all in mask (/*Voxel投影在Mask之外，则把这个Voxel标记为物体之外,其他两种情况类似）
	VoxelType  CheckVoxel(CVoxel* curVoxel, int curLevel);	
	
	VoxelType IsVoxelInMaskFaceSamp(CVoxel* curVoxel, LSVideoFrame* curVideoFrame, int curLevel);

	void LastLevel();

	void LastTwoLevels();

	// 此函数可能有Bug...
	int PointInMasks(Wml::Vector3d& w_pt3d, int lastFailedFrame);

private:
	int m_iStartFrame;            //start frame number 
	int m_iEndFrame;              //end frame number 
    int m_iCurFrame;              //current frame number (add by ly 3/13/2014)
	std::string m_sFrameName;     // frameName represent by number (add by ly 3/13/2014)

	LSStructureMotion* m_pMotion; // LSStructionMotion object

	Wml::Vector3d m_LowCorner;    //minBox coordinate
	Wml::Vector3d m_HighCorner;  // maxBox coordinate
	int m_iDetailLevel;          //level number in Octree carving (9)    
	int m_iImgApron;             //image extension in decide wether a pixel is in mask area (30)

	double m_dMaxLen;            // the max distance among x,y,z( HighCorner to LowCorner）
	int m_iEdgeCells;            // cells number in one edge
	int m_iEdgePoints;           // points number in one edge (m_iEdgePoints=m_iEdgeCells+1)
	int m_iSlicePoints;          // point numbers on every level(m_iEdgePoints*m_iEdgePoints) 
	double m_dCellLength;        // small cell width

	//ORG VoxelCoordBase  m_VoxelBase; 
	VoxelCoordBase * m_VoxelBase;  // pointer point to smallest voxel

private:
	std::vector<CVoxel*>  m_SurFaceVoxel;//tmp var for saving voxel need be carved on different level during caving by Octree method
	std::vector<CVoxelCell*> m_SurefaceVoxelCells;  // saving final surface voxels which will used for transfer meshes from voxels 

public:
	std::vector<IdxTraingle> m_TriangleList;//tmp var for saving  mesh modle
	std::vector<Wml::Vector3f> m_VertexList;//tmp var for saving  mesh modle

};

