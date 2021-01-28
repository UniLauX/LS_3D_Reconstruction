#include <fstream>
#include <iostream>
#include "VisualHull.h"
#include "MarchingCube.h"
#include "LSStructureMotion.h"

CVisualHull::CVisualHull(void){
}

CVisualHull::~CVisualHull(void){
	 this->Clear();
}

void CVisualHull::Clear()
{
	for(int i = 0; i<m_SurFaceVoxel.size(); i++)
	{
		if(m_SurFaceVoxel[i] != NULL)
			delete m_SurFaceVoxel[i];
		m_SurFaceVoxel[i] = NULL;
	}


	for(int i = 0; i<m_SurefaceVoxelCells.size(); i++){
		if(m_SurefaceVoxelCells[i] != NULL)
			delete m_SurefaceVoxelCells[i];
		m_SurefaceVoxelCells[i] = NULL;
	}

	m_TriangleList.clear();
	m_VertexList.clear();
}


void CVisualHull::Init( const Wml::Vector3d& LowCorner,const Wml::Vector3d& HighCorner, int iDetailLevel, int iImgApron )
{
	this->Clear(); //free memory occupied by data structure
	m_LowCorner = LowCorner; 
	m_HighCorner = HighCorner;
	m_iDetailLevel = iDetailLevel;
	m_iImgApron = iImgApron;

	//ensure the detail level is appropriate(between 4 and 10)
	if(m_iDetailLevel > 10) 
		m_iDetailLevel = 10;
	else if(m_iDetailLevel < 4) 
		m_iDetailLevel = 4;

    //make bounding box be a  cube which is easy to cut
	double distx = fabs(m_HighCorner[0] - m_LowCorner[0]);
	double disty = fabs(m_HighCorner[1] - m_LowCorner[1]);
	double distz=fabs(m_HighCorner[2]- m_LowCorner[2]);
	m_dMaxLen = max( max(distx, disty), distz);
	m_HighCorner = m_LowCorner + Wml::Vector3d(m_dMaxLen, m_dMaxLen, m_dMaxLen); 

    //set up cell information for carve 
	m_iEdgeCells = 1<<m_iDetailLevel;     //cell numbers on every edge (equal to 1*(2^9))
	m_iEdgePoints = m_iEdgeCells + 1;     //point numbers on every edge (equal to 1*(2^9)+1)  
	m_iSlicePoints = m_iEdgePoints * m_iEdgePoints;  //point numbers on every level (equal to (1*(2^9)+1)*(1*(2^9)+1))  
	m_dCellLength = m_dMaxLen /(double) m_iEdgeCells;   //cell width ( edge length of voxel)

	//set up basic voxel
	m_VoxelBase=new VoxelCoordBase;
	m_VoxelBase->m_RootLowCorner = m_LowCorner;  //true low corner coordinate
	m_VoxelBase->m_dSmallestCellLength = m_dCellLength; //true cell width
    std::cout<<"Run visual hull init func over"<<std::endl;
}

//debug
//void Save(std::vector<CVoxelCell*>& m_SurefaceVoxelCells){
//	std::ofstream outfile("D:\\test.txt");
//	outfile << m_SurefaceVoxelCells[0]->m_pVoxel->m_pVoxelBase->m_dSmallestCellLength << " " << m_SurefaceVoxelCells[0]->m_pVoxel->m_pVoxelBase->m_RootLowCorner[0] 
//	<< " " << m_SurefaceVoxelCells[0]->m_pVoxel->m_pVoxelBase->m_RootLowCorner[1] << " " << m_SurefaceVoxelCells[0]->m_pVoxel->m_pVoxelBase->m_RootLowCorner[2] << std::endl;
//	outfile << m_SurefaceVoxelCells.size()<< std::endl;
//	for(int i = 0; i<m_SurefaceVoxelCells.size(); i++){
//		outfile << m_SurefaceVoxelCells[i]->m_pVoxel->m_iLowX << " " << m_SurefaceVoxelCells[i]->m_pVoxel->m_iLowY << " " 
//			<< m_SurefaceVoxelCells[i]->m_pVoxel->m_iLowZ << " " << m_SurefaceVoxelCells[i]->m_pVoxel->m_iSize << std::endl;
//		for(int j = 0; j<8; j++){
//			outfile << m_SurefaceVoxelCells[i]->m_IsoValues[j] << " ";
//		}
//		outfile << std::endl;
//	}
//	outfile.close();
//}
//void ReadVoxel(std::vector<CVoxelCell*>& m_SurefaceVoxelCells, VoxelCoordBase& vb){
//	std::ifstream infile("D:\\test.txt");
//	infile >> vb.m_dSmallestCellLength >>  vb.m_RootLowCorner[0] >> vb.m_RootLowCorner[1] >> vb.m_RootLowCorner[2];
//	int sizevc;
//	infile >> sizevc;
//	m_SurefaceVoxelCells.resize(sizevc);
//
//	int lowx, lowy, lowz, size;
//	for(int i = 0; i<m_SurefaceVoxelCells.size(); i++){
//		
//		infile >> lowx >> lowy >> lowz >> size;
//		CVoxel * pvoxel = new CVoxel();
//		pvoxel->setup(lowx, lowy, lowz, size, &vb);
//		 m_SurefaceVoxelCells[i] = new CVoxelCell(pvoxel);
//		for(int j = 0; j<8; j++){
//			infile >> m_SurefaceVoxelCells[i]->m_IsoValues[j];
//		}
//	}
//	infile.close();
//}

//NEW-ADD
void CVisualHull::CalVisualHull( LSStructureMotion* pMotion, int iStart, int iEnd )
{
	//assing frame be calcuated
	m_pMotion = pMotion;
	m_iStartFrame = iStart;
	m_iEndFrame = iEnd;
	if(m_iStartFrame==m_iEndFrame)
		m_iCurFrame=m_iStartFrame;
	else
		std::cout<<"m_iStartFrame is not equal to m_iEndFrame"<<std::endl;	

	//get file name for store VisualHull file
	m_sFrameName=m_pMotion->GetFrameAt(0,m_iCurFrame)->m_sName;
	std::string vhPlyName = GetVisualHullFileName();   //vh file name and path
	std::cout<<"vhPlyName: "<<vhPlyName<<std::endl;

// 暂注：目前在验证VisualHull求解过程中出现偏差导致Init中disparity的范围出现偏差！

	// Init six faces
	Wml::Vector3d  FaceNormal[6];   //six faces
	FaceNormal[0] = Wml::Vector3d(-1.0, 0.0, 0.0);FaceNormal[1] = -FaceNormal[0];
	FaceNormal[2] = Wml::Vector3d(0.0, -1.0, 0.0);FaceNormal[3] = -FaceNormal[2];
	FaceNormal[4] = Wml::Vector3d(0.0, 0.0, -1.0);FaceNormal[5] = -FaceNormal[4];

	for(int iFrame = iStart;iFrame <= iEnd; iFrame++)	
	{
		for(int m = 0; m < CamNum; m++)
		{
			m_pMotion->GetFrameAt(m, iFrame)->LoadMaskImg();
			m_pMotion->GetFrameAt(m,iFrame)->setup(m_pMotion->GetFrameAt(m,iFrame));  //NEW ADD (set up mask image）
			m_pMotion->GetFrameAt(m, iFrame)->SetFaceVis(FaceNormal);  //设置每帧的六个面是否可见（具体含义尚不清楚）(seems not be used)
		}
	}
	//以下3个函数可能影响VisualHull结果导致Init步骤中的Disparity的初值不一样。
	this->OctVH(); //八叉树求VisualHull    //执行此函数的过程中，结果出现不同

	// get vh file and store on the disk （transfer voxels to triangle meshes)
	CMarchingCube mc(m_SurefaceVoxelCells, 0.3F);
	//std::cout<<"m_SurefaceVoxelCells.size():"<<m_SurefaceVoxelCells.size()<<std::endl;   //216359
	mc.Start(m_iEdgePoints,	m_dCellLength,	vhPlyName);
	//clear mask
	for(int iFrame = iStart;iFrame <= iEnd; iFrame++)	{
		// for(int iFrame = iStart;iFrame <1; iFrame++)	{
		for(int camId= 0; camId < CamNum; camId++){
			m_pMotion->GetFrameAt(camId, iFrame)->Clear();
		}
	}
	//setup the geometry
	LoadVisualHull(pMotion);
}


/*Org
void CVisualHull::CalVisualHull( LSStructureMotion* pMotion, int iStart, int iEnd ){
	m_pMotion = pMotion;
	m_iStartFrame = iStart;
	m_iEndFrame = iEnd;
	if(m_iStartFrame==m_iEndFrame)
		m_iCurFrame=m_iStartFrame;
	else
		std::cout<<"m_iStartFrame is not equal to m_iEndFrame"<<std::endl;	
	
	m_sFrameName=m_pMotion->GetFrameAt(0,m_iCurFrame)->m_sName;

	//std::cout<<"m_sFrameName: "<<m_sFrameName<<std::endl;
	std::string vhPlyName = GetVisualHullFileName();   //vh file name and path
    std::cout<<"vhPlyName: "<<vhPlyName<<std::endl;
    // Init six faces
	Wml::Vector3d  FaceNormal[6];   //six faces
	FaceNormal[0] = Wml::Vector3d(-1.0, 0.0, 0.0);FaceNormal[1] = -FaceNormal[0];
	FaceNormal[2] = Wml::Vector3d(0.0, -1.0, 0.0);FaceNormal[3] = -FaceNormal[2];
	FaceNormal[4] = Wml::Vector3d(0.0, 0.0, -1.0);FaceNormal[5] = -FaceNormal[4];

     //先单帧上进行计算
	 for(int iFrame = iStart;iFrame <= iEnd; iFrame++)	{
		for(int m = 0; m < CamNum; m++){
			m_pMotion->GetFrameAt(m, iFrame)->LoadMaskImg();
			m_pMotion->GetFrameAt(m,iFrame)->setup(m_pMotion->GetFrameAt(m,iFrame));  //NEW ADD
		    m_pMotion->GetFrameAt(m, iFrame)->SetFaceVis(FaceNormal);  //设置每帧的六个面是否可见（具体含义尚不清楚）
		}
	}
	 //以下3个函数可能影响VisualHull结果导致Init步骤中的Disparity的初值不一样。
	this->OctVH();         //执行此函数的过程中，结果出现不同
    std::cout<<"CalVisualHull...func over"<<std::endl;
	//Save(m_SurefaceVoxelCells);
	//this->Clear();
	//ReadVoxel(m_SurefaceVoxelCells, m_VoxelBase);
     // get vh file and store on the disk
	CMarchingCube mc(m_SurefaceVoxelCells, 0.3F);
   //std::cout<<"m_SurefaceVoxelCells.size():"<<m_SurefaceVoxelCells.size()<<std::endl;   //216359
	mc.Start(m_iEdgePoints,	m_dCellLength,	vhPlyName);


	//clear mask
	 for(int iFrame = iStart;iFrame <= iEnd; iFrame++)	{
     // for(int iFrame = iStart;iFrame <1; iFrame++)	{
		for(int camId= 0; camId < CamNum; camId++){
			m_pMotion->GetFrameAt(camId, iFrame)->Clear();
		}
	}
	//setup the geometry
	LoadVisualHull(pMotion);
}
*/

bool CVisualHull::LoadVisualHull( LSStructureMotion* pMotion )
{
	m_pMotion = pMotion;
	std::string vhPlyName = GetVisualHullFileName();

	m_TriangleList.clear();
	m_VertexList.clear();

	return PlyReader( vhPlyName, m_TriangleList, m_VertexList );
}


bool CVisualHull::PlyReader( std::string plyFile,std::vector<IdxTraingle>& triangleList, std::vector<Wml::Vector3f>& vertexList )
{
	const int  MAXSTRLEN = 1024;
	std::ifstream mParser(plyFile.c_str() );
	if(mParser.good() == false)
		return false;

	int triangleNum;
	int vertexNum;
    
	std::cout<<"Visual Hull Ply Reader..."<<std::endl;


	//TCHAR  str[MAXSTRLEN+1 ];  
	char str[MAXSTRLEN+1 ];

	mParser.getline(str,MAXSTRLEN);
	mParser.getline(str,MAXSTRLEN);
	mParser.getline(str,MAXSTRLEN);
	//_stscanf(str, _T("element vertex %d"), &vertexNum );
	sscanf(str, "element vertex %d", &vertexNum );

	mParser.getline(str,MAXSTRLEN);
	mParser.getline(str,MAXSTRLEN);
	mParser.getline(str,MAXSTRLEN);

	mParser.getline(str,MAXSTRLEN);
	//_stscanf(str, _T("element face %d"), &triangleNum );
	sscanf(str, "element face %d", &triangleNum );
	mParser.getline(str,MAXSTRLEN);
	mParser.getline(str,MAXSTRLEN);

	vertexList.resize( vertexNum );
	triangleList.resize( triangleNum );

	if(vertexNum <= 0){
		std::cout<< "loading vertex" << vertexNum << "!!!!!!!!!!!!!!!!!!!!"<<std::endl;
	}

	if(triangleNum<=0){
		std::cout << "loading triangle"<< triangleNum << "!!!!!!!!!!!!!!!!!!!!"<<std::endl;
	}

	for(int  iVertex = 0; iVertex < vertexNum; iVertex++){
		Wml::Vector3f& curVertex =vertexList[iVertex];
		mParser.getline(str, MAXSTRLEN);
		//_stscanf(str, _T("%f %f %f\n"), &curVertex[0], &curVertex[1], &curVertex[2]);
	    sscanf(str, "%f %f %f\n", &curVertex[0], &curVertex[1], &curVertex[2]);
	}


	EdgeID* curTriangle;
	for(int iTriangle = 0;iTriangle<triangleNum;iTriangle++){
		curTriangle = triangleList[iTriangle].mVertexID;
		mParser.getline(str,MAXSTRLEN);
		//_stscanf(str, _T("3 %d %d %d\n"), &curTriangle[0], &curTriangle[1], &curTriangle[2] );
		sscanf(str, "3 %d %d %d\n", &curTriangle[0], &curTriangle[1], &curTriangle[2] );
	}
	std::cout <<"Read ply over\n";
    
	return true;
}

std::string CVisualHull::GetVisualHullFileName()
{
	  return m_pMotion->m_sVisualHullDir +m_sFrameName+std::string(".ply");
}
