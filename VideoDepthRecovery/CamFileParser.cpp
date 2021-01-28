#include "CamFileParser.h"


CamFileParser::CamFileParser(void)
{
}


CamFileParser::~CamFileParser(void)
{
}

//加载视频序列，并提取cam文件中的所有类型信息
bool CamFileParser::LoadProject(const std::string & fileName)
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
	
	//提取标签</Image Sequence>中的信息
	if(!ParseSequence())
	{
		return false;
	}
	//提取标签</Motion Type>中的信息
	ParseMotionType();


	//提取标签</intrinsic parameter>中的信息
	ParseIntrinsicParameter();

	//提取标签</Camera Track>中的信息
	ParseCameraTrack();
    //std::cout<<"test camera project file load func "<<std::endl;
	return true;
}


bool CamFileParser::SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index)
{
	return false;
}

//定义：定位关键字符串（例如</Image Sequence>）起始位置,
/**************************************************
  startLocation 为<Image Sequence>之后第一个字符位置
  endLocation 为</Image Sequence>之前的第一个字符位置
**************************************************/

void CamFileParser::LocateContext(const char *str, int &startLocation,int &endLocation)
{
	/*
   std::string keyStringStart,keyStringEnd;
   keyStringStart=std::string("<")+std::string(str)+std::string(">");
   keyStringEnd=std::string("</")+std::string(str)+std::string(">");

   if(OffsetToString(keyStringStart.c_str()))
   {
	 startLocation=m_iIndexBuffer+keyStringStart.length();   
   }
   if(OffsetToString(keyStringEnd.c_str()))
   {
	   endLocation=m_iIndexBuffer-1;
   }
   */
	std::string keyString1, keyString2;
	keyString1 = std::string("<") + std::string(str) + std::string(">");
	keyString2 = std::string("</") + std::string(str) + std::string(">");

	if(OffsetToString(keyString1.c_str())){
		startLocation = m_iIndexBuffer + keyString1.length();
	}
	if(OffsetToString(keyString2.c_str())){
		endLocation = m_iIndexBuffer - 1 ;
	}

}
//功能同上面的一个函数,此外利用rangeBegin和rangeEnd来限定OffsetToString的定位范围
void CamFileParser::LocateContext(const char *str,int rangeBegin,int rangeEnd,int &startLocation,int &endLocation)
{
	std::string keyStringStart,keyStringEnd;
	keyStringStart=std::string("<")+std::string(str)+std::string(">");
	keyStringEnd=std::string("</")+std::string(str)+std::string(">");
	if(OffsetToString(keyStringStart.c_str(),rangeBegin,rangeEnd))
	{
		startLocation=m_iIndexBuffer+keyStringStart.length();
	}
	if(OffsetToString(keyStringEnd.c_str(),rangeBegin,rangeEnd))
	{
		endLocation=m_iIndexBuffer-1;
	}
}



/***分离（提取）起始帧、步长等信息***/
/*******************************************
	<Image Sequence>
	Sequence:.\0000.jpg
	start:0
	step:1
	end:18
	</Image Sequence>
*******************************************/
bool CamFileParser::ParseSequence()
{
	int startLoation,endLocation;
	int index;
	LocateContext("Image Sequence",startLoation,endLocation);//确定startLocation和endLocation位置
	
       // 定位Sequence:
	if(OffsetToString("Sequence:",startLoation,endLocation))
	{
		index=m_iIndexBuffer+strlen("Sequence:");
		strcpy(m_pBufferWord,m_pBufferLine+strlen("Sequence:"));    //m_pBufferLine+strlen("Sequence:")表示“Sequence:”以后的字符串
		std::string templeString=m_pBufferWord;
		int startIndex=templeString.find_first_not_of(' ');
	    int endIndex=templeString.find_last_not_of(' ');
		templeString.substr(startIndex,endIndex-startIndex+1);
	    if(templeString[0]='.')
			templeString=FileNameParser::findFileNameDir(m_sFileName)+std::string(templeString.substr(2));
		m_sImagePath=templeString;
	 }

	//定位start:
     if(OffsetToString("start:",startLoation,endLocation))
	 {
		 index=m_iIndexBuffer+strlen("start:");
		 strcpy(m_pBufferWord,m_pBufferLine+strlen("start:"));
		 m_iStartFrame=atoi(m_pBufferWord);
	 }

	 //定位step:
	 if(OffsetToString("step:",startLoation,endLocation))
	 {
		 index=m_iIndexBuffer+strlen("step:");
		 strcpy(m_pBufferWord,m_pBufferLine+strlen("step:"));
		 m_iFrameStep=std::max(1,atoi(m_pBufferWord));
	 }

	 //定位end:
	 if(OffsetToString("end:",startLoation,endLocation))
	 {
		 index=m_iIndexBuffer+strlen("end:");
		 strcpy(m_pBufferWord,m_pBufferLine+strlen("end:"));
		 m_iEndFrame=atoi(m_pBufferWord);
	 }
      m_iFrameStep=std::max(1,m_iFrameStep);
	  m_iFrameCount=(m_iEndFrame-m_iStartFrame)/m_iFrameStep+1;
	  m_vpFramesMatchPoints.assign(m_iFrameCount,std::shared_ptr<std::vector<MatchPoint>>());  //m_iFrameCount是成员个数，后面是数据类型
	
	return true;
}

/***   提取运动类型信息  ***/
/*********************************
<Motion Type>
FREE_MOVE
FOCAL_CONSTANT
PRINCIPAL_KNOWN
SKEW_KNOWN
</Motion Type>
*********************************/
void CamFileParser::ParseMotionType()
{
	int start=-1,end=-2;
	int index;

	//Default Motion Type!
	m_motionType=MotionType::FREE_MOVE;
	m_focalType=FocalType::FOCAL_CONSTANT;
	m_principalType=PrincipalType::PRINCIPAL_KNOWN;
	m_skewType=SkewType::SKEW_KNOWN;


	std::string str;
	LocateContext("Motion Type",start,end);
	m_iIndexBuffer=start;

	//满足while循环条件，则读取start和end之间的所有字符串
	/*
	enum MotionType{FREE_MOVE,ROTATION_ONLY};         //运动类型{自由运动，仅旋转}
	enum FocalType{FOCAL_KNOWN,FOCAL_CONSTANT,FOCAL_VARIABLE};     //焦距类型{焦距已知，焦距不变，焦距变化}
	enum AspectType{ASPECT_KNOWN,ASPECT_CONSTANT,ASPECT_VARIABLE};  // 
	enum principalType{principal_KNOWN,principal_CONSTANT,principal_VARIABLE};  //主点{主点已知，主点不变，主点变化}
	enum SkewType{SKEW_KNOWN,SKEW_CONSTANT,SKEW_VARIABLE};     //放缩类型{放缩已知，放缩不变，放缩变化}
	*/
	while(start!=-1&&m_iIndexBuffer<end)  
	{
		ReadWord();
		str=m_pBufferWord;
		if(str=="FREE_MOVE")
			m_motionType=MotionType::FREE_MOVE;
		else if(str=="ROTATION_ONLY")
			m_motionType=MotionType::ROTATION_ONLY;
		else if(str == "FOCAL_KNOWN")
			m_focalType = FocalType::FOCAL_KNOWN;
		else if(str == "FOCAL_CONSTANT")
			m_focalType = FocalType::FOCAL_CONSTANT;
		else if(str == "FOCAL_VARIABLE")
			m_focalType = FocalType::FOCAL_VARIABLE;
		else if(str == "ASPECT_KNOWN")
			m_aspectType = AspectType::ASPECT_KNOWN;
		else if(str == "ASPECT_CONSTANT")
			m_aspectType = AspectType::ASPECT_CONSTANT;	
		else if(str == "ASPECT_VARIABLE")
			m_aspectType = AspectType::ASPECT_VARIABLE;
		else if(str == "PRINCIPAL_KNOWN")
			m_principalType = PrincipalType::PRINCIPAL_KNOWN;	
		else if(str == "PRINCIPAL_CONSTANT")
			m_principalType = PrincipalType::PRINCIPAL_CONSTANT;	
		else if(str == "PRINCIPAL_VARIABLE")
			m_principalType = PrincipalType::PRINCIPAL_VARIABLE;		
		else if(str == "SKEW_KNOWN")
			m_skewType = SkewType::SKEW_KNOWN;
		else if(str == "SKEW_CONSTANT")
			m_skewType = SkewType::SKEW_CONSTANT;	
		else if(str == "SKEW_VARIABLE")
			m_skewType = SkewType::SKEW_VARIABLE;	

	}

}

/***读取内参信息***/
/***************************************************************************
<intrinsic parameter>
   //……
</intrinsic parameter>
***************************************************************************/
void CamFileParser::ParseIntrinsicParameter()
{
	int start,end;
	m_iIndexBuffer = 0;
	LocateContext("intrinsic parameter",start,end);
	m_iIndexBuffer = start;

			for(int row=0;row<3;row++)
			{
				for(int col=0;col<3;col++)
				{
					ReadWord();
					m_dLsIntrinsicParameter(row,col)=atof(m_pBufferWord); 
				}
			}
		
	   /*
			K[camId].MakeZero();	
			K[camId](0, 0) = camFiles[camId].m_dFx;	K[camId](0, 1) = 0;	K[camId](0, 2) = camFiles[camId].m_dCx;
			K[camId](1, 1) = camFiles[camId].m_dFy;	K[camId](1, 2) = camFiles[camId].m_dCy;
			K[camId](2, 2) = 1;
	    */
			
			
			m_dFx=m_dLsIntrinsicParameter(0,0);
			m_dFy=m_dLsIntrinsicParameter(1,1);
			m_dCx=m_dLsIntrinsicParameter(0,2);
			m_dCy=m_dLsIntrinsicParameter(1,2);
			


}


    /***跟踪相机运动轨迹（读取所有帧的外参）***/
/******************************************
 <Camera Track>
     ……
 </Camera Track>
*******************************************/
void CamFileParser::ParseCameraTrack()
{
	int start,end;
    m_iIndexBuffer=0;
	LocateContext("Camera Track",start,end);
    m_iIndexBuffer=start;
		 ReadWord();
		 m_dSkew=atof(m_pBufferWord)/m_dFx;   //Alpha_x=f*Kx;   Kx=Alpha_x/f   //m_dSkew=1;

		 //std::cout<<"Alpha_x: "<<m_pBufferWord<<" ,f_x: "<<m_dFx<<", Scale: "<< m_dSkew<<std::endl;

		 for(int row=0;row<4;row++)
		 {
			 for(int col=0;col<4;col++)
			 {
				 ReadWord();
			     m_dLsExtrinsicParameter(row,col)=atof(m_pBufferWord); 
			 }
		 }
}