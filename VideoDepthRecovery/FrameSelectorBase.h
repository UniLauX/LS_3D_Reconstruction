#pragma once
#include <vector>
#include "VDRVideoFrame.h"

class FrameSelectorBase
{
public:
	//���캯��
	FrameSelectorBase(void);

	//��������
	~FrameSelectorBase(void);

	//��òο�֡
	 virtual std::vector<int> GetReferenceFrames(int currentID,int frameCount)=0;
	 virtual std::vector<int> GetReferenceFrames(VDRVideoFrame &framei,int frameCount)=0;




};

