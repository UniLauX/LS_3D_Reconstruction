#include "FrameSelector_Normal.h"


FrameSelector_Normal::FrameSelector_Normal(void)
{
}

//析构函数
FrameSelector_Normal::~FrameSelector_Normal(void)
{
}

//（初始化）赋值
void FrameSelector_Normal::Init(int startDist,int initStep,int initCount,int normalStep,int maxCount)
{
	m_iStartDist=startDist;
	m_iInitStep=initStep;
	m_iInitCount=initCount;
	m_iNormalStep=normalStep;
	m_iMaxCount=maxCount;
}


std::vector<int> FrameSelector_Normal::GetReferenceFrames( int currentID, int frameCount)
{
	std::vector<int> resFrames; //后遍历
	std::vector<int> forWardFrames; //前遍历
	GetReferenceFrames(currentID, resFrames, forWardFrames, frameCount);
	                        
	resFrames.insert(resFrames.end(), forWardFrames.begin(), forWardFrames.end()); //将前遍历帧插入到后遍历帧中
	return resFrames;   //返回前后遍历结果
}


void FrameSelector_Normal::GetReferenceFrames(int currentID,std::vector<int> &backWardFrames,std::vector<int>forwardFrames,int frameCount)
{
      forwardFrames.clear();
	  backWardFrames.clear();

	  int iCount=0;

	  //向前遍历，取相邻（计算）帧(初始化步长和帧数）
      for(int index=currentID+m_iStartDist;index<frameCount&&iCount<m_iInitCount;index+=m_iInitStep)
	  {
         forwardFrames.push_back(index);
		 ++iCount;
	  }

	  //向前遍历，取相邻（计算）帧（实际步长和帧数）
	  if(iCount>0)
	  {
		  for(int index = forwardFrames[iCount-1] + m_iNormalStep; index < frameCount && iCount < m_iMaxCount; index += m_iNormalStep){
			  forwardFrames.push_back(index);
			  ++iCount;
		  }
	  }

	 //向后遍历，取相邻（计算）帧(初始化步长和帧数）
	  iCount = 0;
	  for(int index = currentID - m_iStartDist; index >= m_iStartDist && iCount < m_iInitCount; index -= m_iInitStep){
		  backWardFrames.push_back(index);
		  ++iCount;
	  }	
	  
	  //向前遍历，取相邻（计算）帧（实际步长和帧数）
	  if(iCount > 0){
		  for(int index = backWardFrames[iCount-1] - m_iNormalStep; index >= m_iStartDist && iCount < m_iMaxCount; index -= m_iNormalStep){
			  backWardFrames.push_back(index);
			  ++iCount;
		  }
	  }

	  //前遍历（输出）帧号
	  std::cout << std::left << std::setw(27) <<"Forward Reference Frames: ";
	  for(auto i=0U; i<forwardFrames.size(); ++i)
		  std::cout << std::setw(4) << forwardFrames[i];
	  std::cout << std::endl;


	  //后遍历（输出）帧号
	  std::cout << std::left << std::setw(27) <<"Backward Reference Frames: ";
	  for(auto i=0U; i<backWardFrames.size(); ++i)
		  std::cout << std::setw(4) << backWardFrames[i];
	  std::cout << std::endl;
	  
}


std::vector<int> FrameSelector_Normal::GetReferenceFrames( VDRVideoFrame &framei, int frameCount )
{
	return GetReferenceFrames(framei.m_iID, frameCount);
	//throw std::exception("The method or operation is not implemented.");
}