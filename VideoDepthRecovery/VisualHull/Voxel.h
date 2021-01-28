#ifndef   Voxel_H
#define Voxel_H

#include "WmlMathLib.h"

#define  INSIDE  0
#define  OUTSIDE 1

typedef enum{
	VOXEL_OUTSIDE,//�������⣬���ٷָ�
	VOXEL_INSIDE, //�������ڣ����ٷָ�
	VOXEL_CROSS, //�ָ�ɰ˸���Voxel���������Ҫ�ָ��Voxel����VisualHull����  
}VoxelType;

//ֻ������Voxel��С����ı�ţ�x��y��z��,0=<    x,y,z  <=   1 << nposinedge,
//mSizeҲֻ������СVoxel������Ϊ��λ���ȣ������Ľ��

/*��Voxel���ʷ֣��Լ�����Ļ�ö��������˳��
{0, 0, 0},{1, 0, 0},{1, 1, 0},{0, 1, 0},
{0, 0, 1},{1, 0, 1},{1, 1, 1},{0, 1, 1}
*/

//��������������������������
//���ｫ mCellLength���ó�static ���ã�ֻ��Ϊ�˽�ʡ�ռ䣬���Բ�����ͬʱ������VisualHull
//��������
struct VoxelCoordBase
{
	Wml::Vector3d  m_RootLowCorner;
	double m_dSmallestCellLength; 
};

class CVoxel
{
public:
	//Ĭ����������֮��
	CVoxel();
	~CVoxel();

	void setup(int lowx,int lowy,int lowz,int size,VoxelCoordBase* voxelBase);
	
	//��Voxel�ֳ�8��
	void divide(CVoxel** childVoxels);//eight child

	//cellLength is the samllest cell Lenghth
	//void getCorners(Wml::Vector3f* eightCorners);

	//Get the real coordinate of the lowCorner of current voxel
	Wml::Vector3d  getLowCorner(){
		return Wml::Vector3d( (double)m_iLowX,(double)m_iLowY,(double)m_iLowZ) * m_pVoxelBase->m_dSmallestCellLength + m_pVoxelBase->m_RootLowCorner;
	}

	int m_iLowX, m_iLowY, m_iLowZ;
	int m_iSize;	//һ������cell�ĸ���
	VoxelCoordBase* m_pVoxelBase;
};


class CVoxelCell 
{
public:
	CVoxel*  m_pVoxel;
	float m_IsoValues[8];//����marching cube�����д洢ÿ��voxel��8������isoValue�������ǿ�����char��ʾ�ģ�����ڴ治�����Խ������float�ĳ�char��

	CVoxelCell(CVoxel*  voxel){
		m_pVoxel = voxel;
	}

	~CVoxelCell()
	{
		if(m_pVoxel) 
			delete m_pVoxel;
		m_pVoxel = NULL;
	}
};
#endif