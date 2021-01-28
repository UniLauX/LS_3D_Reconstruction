#pragma once
#include "VisualHull/Voxel.h"
/*
typedef enum{
	VOXEL_OUTSIDE,//在物体外，排除
	VOXEL_INSIDE, //在物体内，作为种子点候选
	VOXEL_CROSS, //在物体表面，排除
}VoxelType;
*/
class AVoxel
{
public:
	AVoxel(void)
	{
		flag=-1;
		color[0]=color[1]=color[2]=0;
		compId=-1;
		smplIdx=-1;     
	}
	


	~AVoxel(void);

	//获取voxel左下角顶点的真实坐标值
	Wml::Vector3d  GetLowCorner()
	{
		return Wml::Vector3d( (double)m_iLowX,(double)m_iLowY,(double)m_iLowZ) * m_pVoxelBase->m_dSmallestCellLength + m_pVoxelBase->m_RootLowCorner;
	}

	//获取voxel几何中心的真实坐标值
	Wml::Vector3d  GetCenterCoord()
	{
		return Wml::Vector3d( (double)(m_iLowX+0.5),(double)(m_iLowY+0.5),(double)(m_iLowZ+0.5)) * m_pVoxelBase->m_dSmallestCellLength + m_pVoxelBase->m_RootLowCorner;
	}

public:
	int flag;// -1 empty, 0 background, 1 foreground, 2 most probably background, 3 most probably foreground.
	int vIdx; //voxel Index in whole volume
    Wml::Vector3d color;  //rgb in this voxel
	int compId;          //Component Id

	int smplIdx;  //color sample Index 

	int m_iLowX,m_iLowY,m_iLowZ;  //Voxel方格左下角顶点的三个索引值(int型）
	VoxelCoordBase *  m_pVoxelBase;
};
