#pragma once
#include <iostream>
class SelectPointParam
{
public:
	SelectPointParam(void);
	~SelectPointParam(void);

public:
	int mDensity;
	float mDspMin;
	float mDspMax;
	int mCoarseInteval;
	int mHalfWinSizeForPlaneFit;
	int mHalfWinSizeForDensity;		
	float mBackProjImgSigma;
	float mLeastConf;

public:
	void Dump()
	{
		printf("\nDesity:%d\nDspMin:%f,DspMax:%f,DepthInteval:%d\n",mDensity,mDspMin,mDspMax,mCoarseInteval);
		printf("HalfWSizePlaneFit:%d,HalfWSizeDensity:%d\n",mHalfWinSizeForPlaneFit,mHalfWinSizeForDensity);
		printf("BackProjImgSigma:%f\n",mBackProjImgSigma);
		printf("LeastConf:%f\n",mLeastConf);
	}

};

