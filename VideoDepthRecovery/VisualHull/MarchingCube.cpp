#include "MarchingCube.h"
#include <iostream>
#include <fstream>
#include <math.h>
#pragma warning(disable:4996)


CMarchingCube::CMarchingCube(std::vector<CVoxelCell* >& surfaceVoxelCells, float isoLevel)
:m_SurfaceVoxelCellList(surfaceVoxelCells), m_fIsoLevel(isoLevel){ }

void CMarchingCube::Start(int edgePoints, double cellLength, std::string&  plyName){

	m_iEdgePoints = edgePoints;
	m_dCellLength = cellLength;
	m_iSlicePoints = m_iEdgePoints * m_iEdgePoints;

	m_VertexList.clear();
	m_TriangleList.clear();
	std::cout<<"Begin marching\n";


	//clock_t start = clock();
	for(int iVoxelCell = 0; iVoxelCell < m_SurfaceVoxelCellList.size(); iVoxelCell++)
		Trianglelise(m_SurfaceVoxelCellList[iVoxelCell]);
	//clock_t end = clock();

	std::cout<<"Marching is over, begin to generate traingle list\n";
	//std::cout<<"total time:"<<(float)(end-start)/CLOCKS_PER_SEC << " Voxel count:" << m_SurfaceVoxelCellList.size() << std::endl;
   
	PlySave(plyName);

}


int CMarchingCube::Trianglelise(CVoxelCell* curVoxelCell){

	CVoxel*  curVoxel = curVoxelCell->m_pVoxel;
	float* eightIsoValue = curVoxelCell->m_IsoValues;

	if( (curVoxel==NULL)  || (eightIsoValue==NULL) )
		return 0;

	//顶点的顺序必须和IsoValue顺序一致
	Wml::Vector3d curPoints[8];
	Wml::Vector3d lowCornerD = curVoxel->getLowCorner();
	Wml::Vector3d lowCorner = Wml::Vector3d(lowCornerD[0], lowCornerD[1], lowCornerD[2]);

	int index = 0;
	for(int z = 0; z<2; z++){
		for(int y = 0; y<2; y++){
			for(int x = 0; x<2; x++){
				curPoints[index] = Wml::Vector3d( x, y, z ) * (float)m_dCellLength  + lowCorner;
				index++;
			}// 见CVoxel中各顶点的顺序
		}
	}

	std::swap(curPoints[2], curPoints[3]);
	std::swap(curPoints[6], curPoints[7]);

	//Find which vertices are inside of the surface and which are outside
	int VertexFlags = 0;
	for(int iVertexIndex = 0; iVertexIndex < 8; iVertexIndex++){
		if(eightIsoValue[iVertexIndex] <= m_fIsoLevel) 
			VertexFlags |= 1<<iVertexIndex;
	}
	//Find which edges are intersected by the surface
	int iEdgeFlags = m_EdgeTable[VertexFlags];

	//If the cube is entirely inside or outside of the surface, then there will be no intersections
	if(iEdgeFlags == 0) //实际上这种情况不可能发生因为我们传进来的都是被表面穿过的CVoxel
		return 0;

	Wml::Vector3d edgeVertexs[12];
	EdgeID  edgeIDs[12];
	//Find the point of intersection of the surface with each edge
	//Then find the normal to the surface at those points
	for(int iEdge = 0; iEdge < 12; iEdge++){
		//if there is an intersection on this edge
		if(iEdgeFlags & (1<<iEdge)){
			int vertex0 = m_a2iEdgeConnection[iEdge][0];
			int vertex1 = m_a2iEdgeConnection[iEdge][1];
			edgeVertexs[iEdge] = Interpolate( curPoints[vertex0], eightIsoValue[vertex0 ], curPoints[vertex1], eightIsoValue [vertex1] );
			edgeIDs[iEdge] = EncodeEdgeID(curVoxel->m_iLowX, curVoxel->m_iLowY, curVoxel->m_iLowZ, iEdge);			
		}
	}	

	// There can be up to five per cube
	std::pair< MAPITER,bool >  iter;

	IdxTraingle  triCornerIdx;
	int iTriangle;
	for(iTriangle = 0; iTriangle < 5; iTriangle++){
		if(m_TriangleTable[VertexFlags][3*iTriangle] < 0)
			break;

		for(int iCorner = 0; iCorner < 3; iCorner++){
			int iVertex = m_TriangleTable[VertexFlags][3*iTriangle+iCorner];
			
			//here we must check if it is a new vertex,and index it
			VertexID tmpsize = (VertexID) mEdgeVertexMap.size();
			if(  mEdgeVertexMap.size() != m_VertexList.size() )
				std::cout<<"wrong\n";

			iter = mEdgeVertexMap.insert(    EVMAP::value_type(edgeIDs[iVertex],tmpsize)   );
			
			if(iter.second)//new vertex;
				m_VertexList.push_back(  edgeVertexs[iVertex]   );

			triCornerIdx.m_VertexID[iCorner] = iter.first->second;
		}

		m_TriangleList.push_back(triCornerIdx);
	}

	return  iTriangle;
}


Wml::Vector3d  CMarchingCube::Interpolate(Wml::Vector3d& point0, float isovalue0, Wml::Vector3d& point1, float isovalue1){
	if (fabs(m_fIsoLevel - isovalue0) < 0.00001F)
		return(point0);
	if (fabs(m_fIsoLevel - isovalue1) < 0.00001F)
		return(point1);
	if (fabs(isovalue0 - isovalue1) < 0.00001F)
		return(point0);

	double mu = (m_fIsoLevel-isovalue0) / (isovalue1 - isovalue0);
	Wml::Vector3d ans = point1 * mu + point0 * (1.0 - mu);
	return ans;
}

CMarchingCube::EdgeID  CMarchingCube::EncodeEdgeID(int x,int y,int z,int iEdge){
	//顶点编号 × 3 + （0，1，2）//对应x，y，z朝向
	if(iEdge == 0) //0 -1
		return EncodeVertexID(x,y,z) * 3;
	if(iEdge ==1) //1-2
		return EncodeVertexID(x + 1,y,z) * 3 + 1; //(x + 1,y,z) y direction
	if(iEdge ==2)//2-3
		return EncodeVertexID(x,y + 1,z) * 3;	//(x,y + 1,z) x direction
	if(iEdge ==3)//3-0
		return EncodeVertexID(x,y,z) * 3 + 1;
	if(iEdge ==4)//4-5
		return EncodeVertexID(x,y,z + 1) * 3;
	if(iEdge ==5)//5-6
		return EncodeVertexID(x + 1,y,z + 1) * 3 + 1;
	if(iEdge==6)//6-7
		return  EncodeVertexID(x,y + 1,z + 1) * 3;
	if(iEdge==7)//7-4
		return EncodeVertexID(x,y,z +1) * 3 + 1;
	if(iEdge==8)//0-4
		return EncodeVertexID(x,y,z) * 3+ 2;
	if(iEdge==9)//1-5
		return EncodeVertexID(x + 1,y,z) * 3 +2;
	if(iEdge==10)//2-6
		return EncodeVertexID(x+1,y+1,z) * 3 + 2;
	if(iEdge==11)//3-7
		return EncodeVertexID(x,y+1,z) * 3 + 2;
	else return -1;
}

void CMarchingCube::PlySave(std::string& plyName)
{
   std::cout<<"plyName: "<<plyName<<std::endl;

	
	std::ofstream outF(plyName.c_str());

	if(outF.good() == false){
		std::cout <<"Error when writing," << plyName << std::endl;
		return ;
	}
	outF << "ply\n";
	outF << "format ascii 1.0\n";
	outF << "element vertex " << m_VertexList.size() << std::endl;
	outF << "property float x\n";
	outF << "property float y\n";
	outF << "property float z\n";

	outF << "element face " << m_TriangleList.size() << std::endl;
	outF << "property list uchar int vertex_indices\nend_header\n";

	for(size_t  iVertex = 0; iVertex<m_VertexList.size(); iVertex++){
		Wml::Vector3d& curVertex =m_VertexList[iVertex];
		outF << curVertex[0] << " " << curVertex[1] <<" " << curVertex[2] << std::endl;
	}

	m_VertexList.clear();
	std::cout<<"Triangle Face: " << m_TriangleList.size() << std::endl;
	EdgeID* curTriangle;
	for(size_t iTriangle = 0;iTriangle<m_TriangleList.size();iTriangle++){
		//std::cout<<iTriangle<<"\n";
		curTriangle = m_TriangleList[iTriangle].m_VertexID;
		outF << "3 "<< curTriangle[2] <<" "<< curTriangle[1] << " " << curTriangle[0] << std::endl;
	}
	outF.close();
	std::cout<<"Save ply over\n";
}
