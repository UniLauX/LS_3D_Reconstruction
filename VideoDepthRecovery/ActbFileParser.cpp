#include "ActbFileParser.h"


ActbFileParser::ActbFileParser(void)
{
}


ActbFileParser::~ActbFileParser(void)
{
}

bool ActbFileParser::LoadProject(const std::string & fileName)
{

	//�ж��Ƿ��ܹ�������ȡ�ļ�
	std::string projectFile;
	if(!ReadFile(fileName.c_str()))
		std::cout<<"Error: can't find"<<std::endl;


	//��ȡһ�У����ж��Ƿ�Ϊ�����ļ�
	ReadLine();
	if(strncmp(m_pBufferLine,"#camera track project file",strlen("#camera track project file")))
	{
		std::cout<<"This is not a camera track project file."<<std::endl;
		return 0;
	}
/*
	//��ȡ��ǩ</Image Sequence>�е���Ϣ
	if(!ParseSequence())
	{
		return false;
	}
	//��ȡ��ǩ</Motion Type>�е���Ϣ
	ParseMotionType();

	//��ȡ��ǩ</intrinsic parameter>�е���Ϣ
	ParseIntrinsicParameter();

	//��ȡ��ǩ</Feature Tracks>�е���Ϣ
	ParseFeatureTrack();

	//��ȡ��ǩ</Camera Track>�е���Ϣ
	ParseCameraTrack();
*/
	return true;
}

bool ActbFileParser::SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index)
{
	return false;
}
