
#pragma once
#include "FileNameParser.h"
#include "ActsParserBase.h"
#include "fileParser.h"

class CamFileParser:public fileParser ,public ActsParserBase
{
public:
	CamFileParser(void);
	~CamFileParser(void);
	
  //get instance
 static CamFileParser * GetInstance()
	{
	    static CamFileParser instance;
		return &instance;
	}
bool LoadProject(const std::string & fileName);    //load project


bool SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index); //save project


private:
	void LocateContext(const char *str, int &startLocation,int &endLocation); //��λ�ؼ��ַ���������</Image Sequence>����ʼλ�ã�startLocation��endLocationΪ�����
	void LocateContext(const char *str,int rangeBegin,int rangeEnd,int &startLocation,int &endLocation);//ͬ���溯��������rangeBegin��rangeEnd�����޶�OffsetToString��λ�Ͷ�ȡ��Χ

    bool ParseSequence();   //������ʼ֡����������Ϣ����ǩ<Image Sequence>��</Image Sequence>֮�����Ϣ��
    void ParseMotionType(); //��ȡ�˶�������Ϣ����ǩ<Motion Type>��</Motion Type>֮�����Ϣ��
    void ParseIntrinsicParameter(); //��ȡ�ڲ���Ϣ(��ǩ<intrinsic parameter>��</intrinsic parameter>֮�����Ϣ)

	void ParseCameraTrack();       //��ȡ����˶��켣��Ϣ����ǩ<Camera Track>��</Camera Track>֮�����Ϣ��-->�����Ϣ
};

