#include "ActsParserBase.h"


ActsParserBase::ActsParserBase(void)
{
}


ActsParserBase::~ActsParserBase(void)
{
}

void ActsParserBase::clear()
{
	m_iFrameCount=0;
	m_vdExtrinsicParameters.clear();
	m_vpMatchLinkers.clear();

	m_vdExtrinsicParameters.shrink_to_fit(); //reduce capacity  ���շ����Vector�ռ�
	m_vpFramesMatchPoints.shrink_to_fit();
	m_vpMatchLinkers.shrink_to_fit();
}

bool ActsParserBase::SaveProject(const std::string & fileName,const std::string &imageName)  //����ͬ������������Ŀ�������
{
	std::vector<int> FrameIndexs(m_iFrameCount);
	   for(int i=0;i<m_iFrameCount;++i)
	   {
		   FrameIndexs[i]=i;
	   }
	   return SaveProject(fileName,imageName,FrameIndexs);    
}