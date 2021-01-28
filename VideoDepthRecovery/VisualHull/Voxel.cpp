#include "Voxel.h"

#pragma warning(disable:4244)

//Wml::Vector3f  CVoxel::mRootLowCorner = Wml::Vector3f(0.0f,0.0f,0.0f);
//float CVoxel::mSmallestCellLength = 1.0f;

CVoxel::CVoxel() {  }

CVoxel::~CVoxel() {  }

void CVoxel::setup(int lowx,int lowy,int lowz,int size,VoxelCoordBase* voxelBase)
{
	m_iLowX = lowx;
	m_iLowY = lowy;
	m_iLowZ = lowz;
	m_iSize = size;
	m_pVoxelBase = voxelBase;
}

/*
{0, 0, 0},{1, 0, 0},{1, 1, 0},{0, 1, 0},
{0, 0, 1},{1, 0, 1},{1, 1, 1},{0, 1, 1}
*/

void CVoxel::divide(CVoxel** childVoxels)
{
	//ensure childVoxels not point to Null
	if(childVoxels==0)
		return;
	
	int halfsz = m_iSize>>1; //half size of cell number in one edge

	for(int i = 0;i<8;i++)
		childVoxels[i] = new CVoxel;
    childVoxels[0]->setup(m_iLowX, m_iLowY, m_iLowZ, halfsz, m_pVoxelBase);
	childVoxels[1]->setup(m_iLowX + halfsz, m_iLowY, m_iLowZ, halfsz, m_pVoxelBase);
	childVoxels[2]->setup(m_iLowX + halfsz, m_iLowY + halfsz, m_iLowZ, halfsz, m_pVoxelBase);
	childVoxels[3]->setup(m_iLowX, m_iLowY + halfsz, m_iLowZ, halfsz, m_pVoxelBase);

	childVoxels[4]->setup(m_iLowX, m_iLowY, m_iLowZ + halfsz, halfsz, m_pVoxelBase);
	childVoxels[5]->setup(m_iLowX + halfsz, m_iLowY, m_iLowZ + halfsz , halfsz, m_pVoxelBase);
	childVoxels[6]->setup(m_iLowX + halfsz, m_iLowY + halfsz, m_iLowZ + halfsz, halfsz, m_pVoxelBase);
	childVoxels[7]->setup(m_iLowX, m_iLowY + halfsz, m_iLowZ + halfsz, halfsz, m_pVoxelBase);
} 

//void CVoxel::getCorners(Wml::Vector3f* eightCorners)
//{
//	const float cellLength = mVoxelBase->mSmallestCellLength;
//	eightCorners[0] = Wml::Vector3f( m_iLowX,m_iLowY,m_iLowZ) * cellLength;
//	eightCorners[1] = Wml::Vector3f( m_iLowX + mSize,m_iLowY,m_iLowZ) * cellLength;
//	eightCorners[2] = Wml::Vector3f( m_iLowX + mSize,m_iLowY + mSize,m_iLowZ)* cellLength;
//	eightCorners[3] = Wml::Vector3f(m_iLowX,m_iLowY + mSize,m_iLowZ) * cellLength;
//	eightCorners[4] = Wml::Vector3f(m_iLowX,m_iLowY,m_iLowZ + mSize) * cellLength;
//	eightCorners[5] = Wml::Vector3f(m_iLowX + mSize,m_iLowY,m_iLowZ + mSize) * cellLength;
//	eightCorners[6] = Wml::Vector3f(m_iLowX + mSize,m_iLowY + mSize,m_iLowZ + mSize) * cellLength;
//	eightCorners[7] = Wml::Vector3f(m_iLowX,m_iLowY + mSize,m_iLowZ + mSize)*cellLength;
//}