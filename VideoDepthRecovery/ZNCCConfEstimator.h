#pragma once
#include <vector>
#include < algorithm >
#include "ZImage.h"
#include "NCCCommonUse.h"
#include "LSVideoFrame.h"
class ZNCCConfEstimator
{
public:
	ZNCCConfEstimator(
		LSVideoFrame* refFrame,std::vector<LSVideoFrame* >& cmpFrameSet,
		ZByteImage* refImage,std::vector<ZByteImage* >& cmpImageSet);
	~ZNCCConfEstimator(void);


	LSVideoFrame* mRefFrame;
	std::vector<LSVideoFrame* >& mCmpFrameSet;
	ZByteImage* mRefImage;
	std::vector<ZByteImage* >& mCmpImageSet;

	float  estPixelConf(int x,int y,int halfWinSize);



	//My NewAdd
	// get world coord from image coord
	Wml::Vector3d WorldCoordFrmImgCoord(LSVideoFrame* pVFrm, Wml::Vector2d p, double z);
	
};

