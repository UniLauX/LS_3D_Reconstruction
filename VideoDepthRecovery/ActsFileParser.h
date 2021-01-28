#pragma once
#include "FileNameParser.h"
#include "ActsParserBase.h"
#include "fileParser.h"
#include "MatchLinker.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
class ActsFileParser: public fileParser ,public ActsParserBase
{
public:
	ActsFileParser(void);

	~ActsFileParser(void);

public:
	static ActsFileParser * GetInstance()
	{
		static ActsFileParser instance;
		return &instance;
	}

	bool LoadProject(const std::string & fileName);    //������Ŀ

	//virtual bool SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index) override;
     bool SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index);


private:
    void LocateContext(const char *str, int &startLocation,int &endLocation); //��λ�ؼ��ַ���������</Image Sequence>����ʼλ�ã�startLocation��endLocationΪ�����
	void LocateContext(const char *str,int rangeBegin,int rangeEnd,int &startLocation,int &endLocation);//ͬ���溯��������rangeBegin��rangeEnd�����޶�OffsetToString��λ�Ͷ�ȡ��Χ
	                                                                         


	bool ParseSequence();   //������ʼ֡����������Ϣ����ǩ<Image Sequence>��</Image Sequence>֮�����Ϣ��
	void ParseMotionType(); //��ȡ�˶�������Ϣ����ǩ<Motion Type>��</Motion Type>֮�����Ϣ��
	void ParseIntrinsicParameter(); //��ȡ�ڲ���Ϣ(��ǩ<intrinsic parameter>��</intrinsic parameter>֮�����Ϣ)
	void ParseFeatureTrack();      //��ȡ�����������Ϣ(��ǩ<Feature Tracks>��</Feature Tracks>֮�����Ϣ)
	void ParseCameraTrack();       //��ȡ����˶��켣��Ϣ����ǩ<Camera Track>��</Camera Track>֮�����Ϣ��
};


