#pragma once

#include <vector>

#include "LSVideoFrame.h"
#include "DataCost.h"



class LSStereoWorkUnit
{
public:
	// indicate which type to call
	enum CAL_TYPE{
		INIT,
		BO,
		OPT
	};

	//constructor
	LSStereoWorkUnit(LSVideoFrame* pCurrentFrame, std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, ZIntImage& labelImg, 
		DataCost& dataCost, std::vector<float>& dspV, int iLineIndex, int Width, CAL_TYPE callType);

	~LSStereoWorkUnit(void);

public:
	virtual void Execute();

private:
	int m_iLineIndex;

	int m_iWidth;

	LSVideoFrame * m_pCurrentFrame;    //current video frame

	std::vector<LSVideoFrame*>& m_FwFrames;   //forward reference frames
	std::vector<LSVideoFrame*>& m_BwFrames;   //backward reference frames

	std::vector<float>& m_dspV;               //dsp vector(101 level)

	ZIntImage& m_labelImg;                    //label image

	DataCost& m_dataCost;                     //dataCost

	CAL_TYPE m_callType;                      //Init or BO

};

