#pragma once
#include "LSVideoFrame.h"
#include "DataCost.h"

class LSSuperRefineWorkUnit
{
public:
	//constructor
	LSSuperRefineWorkUnit(LSVideoFrame* pCurrentFrame, std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, ZIntImage& labelImg, ZIntImage& offsetImg,
		DataCost& dataCost, int iLineIndex, int Width, int totalDspLevel, int SubSample, LSVideoFrame::LABEL_IMG_TYPE type);
	~LSSuperRefineWorkUnit(void);

	// execute function
	virtual void Execute();


private:

	int m_iLineIndex;    // ith line index

	int m_iWidth;       //image width

	LSVideoFrame * m_pCurrentFrame;   //pointer to current frame

	std::vector<LSVideoFrame*>& m_FwFrames;  // reference forward frames
	std::vector<LSVideoFrame*>& m_BwFrames;  // reference backward frames

	ZIntImage& m_labelImg;                  // label image

	DataCost& m_dataCost;                  // data cost

	//============================
	
	ZIntImage& m_OffsetImg;                // offset image
	int m_iTotalDspLevelCount;             // total dsparity level count
	int m_iSubSample;                      

	LSVideoFrame::LABEL_IMG_TYPE m_type;   // running type

};

