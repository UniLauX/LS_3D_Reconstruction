#pragma once
#include "FrameSelectorBase.h"
#include <iostream>
#include <iomanip>
class FrameSelector_Normal:public FrameSelectorBase
{
protected:
	// ���캯��
	FrameSelector_Normal(void);


public:
	// ��þ�̬ʵ���������ⲿ����
	static FrameSelector_Normal *GetInstance(){
		static FrameSelector_Normal instance;
		return &instance;
	}
	    //��������
		~FrameSelector_Normal(void);


		//����ֵ)��ʼ�� ��Ա����
		 void Init(int startDist,int initStep,int initCount,int normapStep,int maxCount);

		//������disparity)��òο�֡(��Ҫ���ã�
		virtual std::vector<int> GetReferenceFrames(int currentID,int frameCount) override;
		virtual std::vector<int> GetReferenceFrames(VDRVideoFrame &framei,int frameCount) override;

		//������disparity)��òο�֡����Ҫ������̣�
		void GetReferenceFrames(int currentID,std::vector<int> &backwardFrames,std::vector<int>forwardFrames,int frameCount);


private:
	   int m_iStartDist;   //��һ֡���뵱ǰ֡t)�ľ���
	   int m_iInitStep;    //��ʼ������Ĭ��Ϊ1��
	   int m_iInitCount;   //��ʼ֡����Ĭ��Ϊ10��
	   int m_iNormalStep;  //��ȡ����
	   int m_iMaxCount;    //��������ѡ���ܵ�֡��

};



