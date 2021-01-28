#pragma once
#include <vector>
#include "VDRVideoFrame.h"

class FrameSelectorBase
{
public:
	//构造函数
	FrameSelectorBase(void);

	//析构函数
	~FrameSelectorBase(void);

	//获得参考帧
	 virtual std::vector<int> GetReferenceFrames(int currentID,int frameCount)=0;
	 virtual std::vector<int> GetReferenceFrames(VDRVideoFrame &framei,int frameCount)=0;




};

