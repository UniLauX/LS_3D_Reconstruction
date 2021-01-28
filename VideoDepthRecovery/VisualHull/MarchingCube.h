
#ifndef MarchingCube_H
#define MarchingCube_H

#include "Voxel.h"
#include <vector>
#include <WmlVector3.h>
#include <map>
#include <string>
//#include "tstring.h"

/*���������Voxels�����һ��������Mesh,
��������������������������������  ���￪ʼ��Voxel��x��y��z��Level���ܳ���10��
�뽫�������ȥ������Ҫ��
typedef  unsigned int EdgeID;
typedef  unsigned int VertexID;
�е� unsigned int �޸ĳ� 64λ������*/


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
	//�����������������ͬһ��Edge�����ǵ���άλ����ͬ�����Խ�����������EdgeID��Ӧ������EdgeIDΨһ

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

	static int m_a2iEdgeConnection[12][2];//һ��������������vertex

	//tmp
	int m_iEdgePoints;
	float m_dCellLength;
	int m_iSlicePoints;

};

#endif