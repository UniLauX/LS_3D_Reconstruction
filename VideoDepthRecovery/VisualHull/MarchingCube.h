
#ifndef MarchingCube_H
#define MarchingCube_H

#include "Voxel.h"
#include <vector>
#include <WmlVector3.h>
#include <map>
#include <string>
//#include "tstring.h"

/*这个类输入Voxels，输出一个三角形Mesh,
！！！！！！！！！！！！！！！！  这里开始的Voxel的x，y，z的Level不能超过10，
想将这个限制去掉，需要将
typedef  unsigned int EdgeID;
typedef  unsigned int VertexID;
中的 unsigned int 修改成 64位长整型*/


class CMarchingCube
{
public:
	CMarchingCube(std::vector<CVoxelCell* >&  surfaceVoxelCells, float isoLevel);

	void Start(int edgePoints, double cellLength, std::string&  plyName);

	//////////////////////////////////////////////////////////////////////////
	typedef  unsigned int EdgeID;
	typedef  unsigned int VertexID;

	struct  IdxTraingle	{		
		VertexID m_VertexID[3];
	};
	//这里如果两个点来自同一个Edge则他们的三维位置相同，所以将顶点索引与EdgeID对应，而且EdgeID唯一

	typedef  std::map<EdgeID,VertexID>  EVMAP;
	typedef EVMAP::iterator  MAPITER;

	//////////////////////////////////////////////////////////////////////////
private:
	void PlySave(std::string&  plyName);

	//return new triangles   generated,0 failed 
	int  Trianglelise(CVoxelCell* curVoxelCell);

	Wml::Vector3d  Interpolate(Wml::Vector3d&  point0,float isovalue0,Wml::Vector3d& point1,float isovalue1);

	EdgeID EncodeEdgeID(int x,int y,int z,int  iEdge);

	VertexID EncodeVertexID(int x, int y, int z){
		return x + y * m_iEdgePoints + z * m_iSlicePoints;
	}

	std::vector<CVoxelCell* >&  m_SurfaceVoxelCellList;
	
	EVMAP  mEdgeVertexMap;  

	std::vector<IdxTraingle>  m_TriangleList;
	std::vector<Wml::Vector3d> m_VertexList;

	float m_fIsoLevel;
	static int m_EdgeTable[256];
	static int m_TriangleTable[256][16];

	static int m_a2iEdgeConnection[12][2];//一条边连接那两个vertex

	//tmp
	int m_iEdgePoints;
	float m_dCellLength;
	int m_iSlicePoints;

};

#endif