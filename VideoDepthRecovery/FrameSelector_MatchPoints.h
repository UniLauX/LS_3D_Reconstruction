#pragma once
#include "FrameSelectorBase.h"
#include <iomanip>

class FrameSelector_MatchPoints: public FrameSelectorBase
{
protected:
	FrameSelector_MatchPoints(void);

public:
public:
	static FrameSelector_MatchPoints *GetInstance()
	{
		static FrameSelector_MatchPoints instance;
		return &instance;
	}
	~FrameSelector_MatchPoints(void);

	void Init(int maxCount);
	virtual std::vector<int> GetReferenceFrames( int currentID, int frameCount );

	virtual std::vector<int> GetReferenceFrames( VDRVideoFrame &framei, int frameCount );
private:
	int m_iMaxCount;
};

