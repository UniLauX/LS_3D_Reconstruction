#include "ActbFileParser.h"


ActbFileParser::ActbFileParser(void)
{
}


ActbFileParser::~ActbFileParser(void)
{
}

bool ActbFileParser::LoadProject(const std::string & fileName)
{

	//判断是否能够正常读取文件
	std::string projectFile;
	if(!ReadFile(fileName.c_str()))
		std::cout<<"Error: can't find"<<std::endl;


	//读取一行，并判断是否为跟踪文件
	ReadLine();
	if(strncmp(m_pBufferLine,"#camera track project file",strlen("#camera track project file")))
	{
		std::cout<<"This is not a camera track project file."<<std::endl;
		return 0;
	}
/*
	//提取标签</Image Sequence>中的信息
	if(!ParseSequence())
	{
		return false;
	}
	//提取标签</Motion Type>中的信息
	ParseMotionType();

	//提取标签</intrinsic parameter>中的信息
	ParseIntrinsicParameter();

	//提取标签</Feature Tracks>中的信息
	ParseFeatureTrack();

	//提取标签</Camera Track>中的信息
	ParseCameraTrack();
*/
	return true;
}

bool ActbFileParser::SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index)
{
	return false;
}
