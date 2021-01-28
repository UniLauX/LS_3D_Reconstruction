#pragma once

#include <vector>
#include "WmlVector3.h"

class  LSVideoFrame;
/*
这里我们想得到最小的深度和做大的深度
这个深度是Camera局部坐标系中的深度
*/

typedef  unsigned int EdgeID;
typedef  unsigned int VertexID;

struct  IdxTraingle
{
	EdgeID mVertexID[3];
};

class SoftRasterize
{
public:
	SoftRasterize(float* minDepth,float* maxDepth,int width,int height,LSVideoFrame* camMat);  //NEW-ADD


	SoftRasterize(LSVideoFrame* pFrame);
	~SoftRasterize();

	void start(std::vector<IdxTraingle>&   mTriangleList,	std::vector<Wml::Vector3f>& mVertexList);

protected:

	void _triangleRasterize(Wml::Vector3f* V0,Wml::Vector3f* V1,Wml::Vector3f* V2);
	void _triangleRasterize2(Wml::Vector3f* V0,Wml::Vector3f* V1,Wml::Vector3f* V2);
private:

	LSVideoFrame* m_pFrame;
	int mRectMinX, mRectMinY, mRectMaxX, mRectMaxY;

	//NEW-ADD
	float* mMinDepth;
	float* mMaxDepth;
	int mWidth,mHeight;
	//LSVideoFrame* mCamMat;
};