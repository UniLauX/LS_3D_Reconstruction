#include "ActsFileParser.h"


ActsFileParser::ActsFileParser(void)
{
}


ActsFileParser::~ActsFileParser(void)
{
}

//加载视频序列，并提取act文件中的所有类型信息
bool ActsFileParser::LoadProject(const std::string & fileName)
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

	//提取标签</Feature Tracks>中的信息
	ParseFeatureTrack();

	//提取标签</Camera Track>中的信息
	ParseCameraTrack();
   
	return true;
}

bool ActsFileParser::SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index)
{
	return false;
}

//定义：定位关键字符串（例如</Image Sequence>）起始位置,
/**************************************************
  startLocation 为<Image Sequence>之后第一个字符位置
  endLocation 为</Image Sequence>之前的第一个字符位置
**************************************************/

void ActsFileParser::LocateContext(const char *str, int &startLocation,int &endLocation)
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
void ActsFileParser::LocateContext(const char *str,int rangeBegin,int rangeEnd,int &startLocation,int &endLocation)
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
	Sequence:.\test0000.jpg
	start:0
	step:1
	end:140
	</Image Sequence>
*******************************************/
bool ActsFileParser::ParseSequence()
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
void ActsFileParser::ParseMotionType()
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
1475.2344540	1475.2344540	479.5000000	269.5000000	0.0000000	1.0000000	
</intrinsic parameter>
***************************************************************************/
void ActsFileParser::ParseIntrinsicParameter()
{
    int start,end;
	m_iIndexBuffer=0;
	LocateContext("intrinsic parameter",start,end);
	m_iIndexBuffer=start;
    ReadWord();
	m_dFx=atof(m_pBufferWord);

    ReadWord();
	m_dFy=atof(m_pBufferWord);

	ReadWord();
	m_dCx=atof(m_pBufferWord);

	ReadWord();
	m_dCy=atof(m_pBufferWord);
	
	
	ReadWord();
	m_dSkew = atof(m_pBufferWord);

	ReadWord();
	if(m_iIndexBuffer<=end)
	{
		m_dAspectRatio=atof(m_pBufferWord);
		m_dFy=m_dAspectRatio*m_dFx;
	}
}

/*** 读取跟踪点信息 ***/
/********************************************
<Feature Tracks>   
      ……
<Feature Tracks>
*******************************************/
void ActsFileParser::ParseFeatureTrack()
{

	int start=-1,end=-1;
	int index;
	int i,j;
	m_iIndexBuffer=0;

	LocateContext("Feature Tracks",start,end);
	
	//未找到
	if(start<0||end<0)
		return;

	m_iIndexBuffer=start;
   
	int frameStart,frameEnd;
	std::string str;

	char buffer[255];

	int iTrackCount;   //跟踪点个数
	bool bLoadKeyPoints=false;  
	int iDimension =0;

  //读取跟踪点个数 
	ReadWord();
	//有维度信息
	if(strcmp(m_pBufferWord,"kpts:")==0)
	{
		ReadWord();
		iDimension=atoi(m_pBufferWord);
		bLoadKeyPoints=true;

		ReadWord();
        iTrackCount=atoi(m_pBufferWord);
	}
    else    //无维度信息
	{
		iTrackCount=atoi(m_pBufferWord);
	}

	//清空匹配点存储器，用于存储对于特定iTrackCount的所有TrackPoint信息
	m_vpMatchLinkers.clear();

	for(i=0;i<iTrackCount;i++){
    std::shared_ptr<MatchLinker>pSet=std::make_shared<MatchLinker>();   //(创建指针链表） 需要进一步了解shared_ptr和make_shared的用法
	
	int iTrackLength,iFlag,iErrorLever;
	
	ReadWord();
	iTrackLength=atoi(m_pBufferWord);     //跟踪长度：有多少帧跟踪点的信息

	ReadWord();
	iFlag=atoi(m_pBufferWord);    //是否解出三维点（对应该跟踪点：0表示解得三维点，三维点坐标存储在m_v3D中；1表示未解得跟踪点，三个坐标为固定值<0.0000000	0.0000000	17.3869146>)

	ReadWord();
	iErrorLever=atoi(m_pBufferWord); //-1,1或3，表示所解结果是否正确

	ReadWord();                         //存储所解三维坐标（若未解得，则为固定值<0.0000000	0.0000000	17.3869146>）
		pSet->m_v3D[0]=atof(m_pBufferWord);
	ReadWord();
	    pSet->m_v3D[1]=atof(m_pBufferWord);
	ReadWord();
	    pSet->m_v3D[2]=atof(m_pBufferWord);

	pSet->m_iFlag=iFlag;
	pSet->m_iErrorLevel=iErrorLever;
	pSet->Reserve(iTrackLength);         //分配存储空间


	//对于特定iTrackCount,读取跟踪点二维坐标信息
	for(j=0;j<iTrackLength;++j)
	{
		TrackPoint &trackPoint=pSet->Point(j);  
	   //读取跟踪点所属帧号
		ReadWord();
        trackPoint.m_iFrameNo=atoi(m_pBufferWord);                    
		if(m_vpFramesMatchPoints[trackPoint.m_iFrameNo]==false)
			m_vpFramesMatchPoints[trackPoint.m_iFrameNo]=std::make_shared<std::vector<MatchPoint>>();//如果某一帧上没有跟踪点，则创建一个存储向量

        trackPoint.m_iIndex=m_vpFramesMatchPoints[trackPoint.m_iFrameNo]->size();             //某一帧上有多少个跟踪点

	  //读取跟踪点的二维坐标
		ReadWord();                                
		trackPoint.m_dX=atof(m_pBufferWord);
		ReadWord();
		trackPoint.m_dY=atof(m_pBufferWord);

		//用matchPoint存储trackPoint的坐标和指针信息，并存入m_vpFramesMatchPoints中。
		MatchPoint matchPoint;
	    matchPoint.m_dX=trackPoint.m_dX;   
		matchPoint.m_dY=trackPoint.m_dY;
		matchPoint.m_pMatchLinker=pSet;     
        m_vpFramesMatchPoints[trackPoint.m_iFrameNo]->push_back(matchPoint); //将特定iTrackCount，m_iFrameNo的匹配点坐标存入m_vpFramesMatchPoints中

	}
    pSet->m_index=m_vpMatchLinkers.size();    
	m_vpMatchLinkers.push_back(pSet);        
	
//	std::cout<<"m_vpMatchLinkers.size()"<<m_vpMatchLinkers.size()<<std::endl;

	//load KeyPoints                         //读取维度信息（act文件中无此信息）
		if(bLoadKeyPoints)            
		{
			for(j=0;j<iDimension;++j)
			{
				ReadWord();
			}
		}

	}

}


/***跟踪相机运动轨迹（读取所有帧的外参）***/
/******************************************
 <Camera Track>
   <FRAMEn>
     ……
   </FRAMEn>
 </Camera Track>
*******************************************/
void ActsFileParser::ParseCameraTrack()
{
	int start,end;
	int index;
	int i;
    m_iIndexBuffer=0;

	LocateContext("Camera Track",start,end);
    m_iIndexBuffer=start;


	int frameStart,frameEnd;
    std::string str;
	  
	m_vdExtrinsicParameters.resize(m_iFrameCount);

	   char buffer[100];
    for(int i=0;i<m_iFrameCount;i++)
	{
          //整形数转为字符串          
		sprintf(buffer,"%d",i);
		str=std::string("FRAME")+buffer;

	//	std::cout<<str<<std::endl;
		 LocateContext(str.c_str(),start,end,frameStart,frameEnd);
		 m_iIndexBuffer=frameStart;

		 ReadWord();

		 double scale=atof(m_pBufferWord)/m_dFx;   //Alpha_x=f*Kx;   Kx=Alpha_x/f

		 for(int row=0;row<4;row++)
		 {
			 for(int col=0;col<4;col++)
			 {
				 ReadWord();
			     m_vdExtrinsicParameters[i](row,col)=atof(m_pBufferWord); 
			 }
		 }

		 start=frameEnd;   //读取过的帧外参不再包含在定位和读取范围内
		
	}

}