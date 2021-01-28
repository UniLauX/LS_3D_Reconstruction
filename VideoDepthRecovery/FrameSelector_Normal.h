#pragma once
#include "FrameSelectorBase.h"
#include <iostream>
#include <iomanip>
class FrameSelector_Normal:public FrameSelectorBase
{
protected:
	// 构造函数
	FrameSelector_Normal(void);


public:
	// 获得静态实例，便于外部引用
	static FrameSelector_Normal *GetInstance(){
		static FrameSelector_Normal instance;
		return &instance;
	}
	    //析构函数
		~FrameSelector_Normal(void);


		//（赋值)初始化 成员变量
		 void Init(int startDist,int initStep,int initCount,int normapStep,int maxCount);

		//（计算disparity)获得参考帧(主要调用）
		virtual std::vector<int> GetReferenceFrames(int currentID,int frameCount) override;
		virtual std::vector<int> GetReferenceFrames(VDRVideoFrame &framei,int frameCount) override;

		//（计算disparity)获得参考帧（主要计算过程）
		void GetReferenceFrames(int currentID,std::vector<int> &backwardFrames,std::vector<int>forwardFrames,int frameCount);


private:
	   int m_iStartDist;   //第一帧（与当前帧t)的距离
	   int m_iInitStep;    //初始步长（默认为1）
	   int m_iInitCount;   //初始帧数（默认为10）
	   int m_iNormalStep;  //所取步长
	   int m_iMaxCount;    //（单侧所选）总的帧数

};



