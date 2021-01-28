#ifndef   Voxel_H
#define Voxel_H

#include "WmlMathLib.h"

#define  INSIDE  0
#define  OUTSIDE 1

typedef enum{
	VOXEL_OUTSIDE,//在物体外，不再分割
	VOXEL_INSIDE, //在物体内，不再分割
	VOXEL_CROSS, //分割成八个子Voxel，所以最后要分割的Voxel都是VisualHull表面  
}VoxelType;

//只给出了Voxel最小坐标的编号（x，y，z）,0=<    x,y,z  <=   1 << nposinedge,
//mSize也只是以最小Voxel长度作为单位长度，衡量的结果

/*对Voxel的剖分，以及顶点的获得都以下面的顺序
{0, 0, 0},{1, 0, 0},{1, 1, 0},{0, 1, 0},
{0, 0, 1},{1, 0, 1},{1, 1, 1},{0, 1, 1}
*/

//！！！！！！！！！！！！！
//这里将 mCellLength设置成static 不好，只是为了节省空间，所以不可以同时做两个VisualHull
//修正如下
struct VoxelCoordBase
{
	Wml::Vector3d  m_RootLowCorner;
	double m_dSmallestCellLength; 
};

class CVoxel
{
public:
	//默认是在物体之外
	CVoxel();
	~CVoxel();

	void setup(int lowx,int lowy,int lowz,int size,VoxelCoordBase* voxelBase);
	
	//将Voxel分成8份
	void divide(CVoxel** childVoxels);//eight child

	//cellLength is the samllest cell Lenghth
	//void getCorners(Wml::Vector3f* eightCorners);

	//Get the real coordinate of the lowCorner of current voxel
	Wml::Vector3d  getLowCorner(){
		return Wml::Vector3d( (double)m_iLowX,(double)m_iLowY,(double)m_iLowZ) * m_pVoxelBase->m_dSmallestCellLength + m_pVoxelBase->m_RootLowCorner;
	}

	int m_iLowX, m_iLowY, m_iLowZ;
	int m_iSize;	//一个边上cell的个数
	VoxelCoordBase* m_pVoxelBase;
};


class CVoxelCell 
{
public:
	CVoxel*  m_pVoxel;
	float m_IsoValues[8];//用于marching cube方法中存储每个voxel的8个顶点isoValue（这里是可以用char表示的，如果内存不够可以将这里的float改成char）

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