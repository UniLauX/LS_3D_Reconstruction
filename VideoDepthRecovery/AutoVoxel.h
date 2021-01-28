#pragma once
#include "VisualHull/Voxel.h"
/*
typedef enum{
	VOXEL_OUTSIDE,//�������⣬�ų�
	VOXEL_INSIDE, //�������ڣ���Ϊ���ӵ��ѡ
	VOXEL_CROSS, //��������棬�ų�
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

	//��ȡvoxel���½Ƕ������ʵ����ֵ
	Wml::Vector3d  GetLowCorner()
	{
		return Wml::Vector3d( (double)m_iLowX,(double)m_iLowY,(double)m_iLowZ) * m_pVoxelBase->m_dSmallestCellLength + m_pVoxelBase->m_RootLowCorner;
	}

	//��ȡvoxel�������ĵ���ʵ����ֵ
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

	int m_iLowX,m_iLowY,m_iLowZ;  //Voxel�������½Ƕ������������ֵ(int�ͣ�
	VoxelCoordBase *  m_pVoxelBase;
};
