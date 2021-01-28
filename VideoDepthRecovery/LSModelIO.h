#pragma once
#include <iostream>
#include <string>
#include "ZImage.h"
#include "LSCameraParameters.h"
#include "CxImage.h"

class LSModelIO
{
public:
	
	//constructor
	//LSModelIO(void);
	LSModelIO(float fMinDis, float fMaxDis, float fSmoothThreshold);

	//destructor
	~LSModelIO(void);

	//save model as obj
	void SaveModel(ZFloatImage& depth, std::string& maskImgPath, std::string desPath, LSCameraParameters& camerPara);

private:
	bool InMask(CxImage& maskImg, int u, int v){
		if(maskImg.GetSize() == 0)
			return true;
		BYTE gray = maskImg.GetPixelGray(u, v);
		return gray > 10;
	}

private:
	float m_fMinDsp;    // min disparity
	float m_fMaxDsp;    // max disparity
	float m_fSmoothThreshold;   // smooth threshold
};

