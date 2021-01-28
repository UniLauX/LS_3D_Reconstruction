#include "ActsFileParser.h"


ActsFileParser::ActsFileParser(void)
{
}


ActsFileParser::~ActsFileParser(void)
{
}

//������Ƶ���У�����ȡact�ļ��е�����������Ϣ
bool ActsFileParser::LoadProject(const std::string & fileName)
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
   
	return true;
}

bool ActsFileParser::SaveProject(const std::string & fileName,const std::string &imageName, const std::vector<int> &index)
{
	return false;
}

//���壺��λ�ؼ��ַ���������</Image Sequence>����ʼλ��,
/**************************************************
  startLocation Ϊ<Image Sequence>֮���һ���ַ�λ��
  endLocation Ϊ</Image Sequence>֮ǰ�ĵ�һ���ַ�λ��
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
//����ͬ�����һ������,��������rangeBegin��rangeEnd���޶�OffsetToString�Ķ�λ��Χ
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

/***���루��ȡ����ʼ֡����������Ϣ***/
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
	LocateContext("Image Sequence",startLoation,endLocation);//ȷ��startLocation��endLocationλ��
	
       // ��λSequence:
	if(OffsetToString("Sequence:",startLoation,endLocation))
	{
		index=m_iIndexBuffer+strlen("Sequence:");
		strcpy(m_pBufferWord,m_pBufferLine+strlen("Sequence:"));    //m_pBufferLine+strlen("Sequence:")��ʾ��Sequence:���Ժ���ַ���
		std::string templeString=m_pBufferWord;
		int startIndex=templeString.find_first_not_of(' ');
	    int endIndex=templeString.find_last_not_of(' ');
		templeString.substr(startIndex,endIndex-startIndex+1);
	    if(templeString[0]='.')
			templeString=FileNameParser::findFileNameDir(m_sFileName)+std::string(templeString.substr(2));
		m_sImagePath=templeString;
	 }

	//��λstart:
     if(OffsetToString("start:",startLoation,endLocation))
	 {
		 index=m_iIndexBuffer+strlen("start:");
		 strcpy(m_pBufferWord,m_pBufferLine+strlen("start:"));
		 m_iStartFrame=atoi(m_pBufferWord);
	 }

	 //��λstep:
	 if(OffsetToString("step:",startLoation,endLocation))
	 {
		 index=m_iIndexBuffer+strlen("step:");
		 strcpy(m_pBufferWord,m_pBufferLine+strlen("step:"));
		 m_iFrameStep=std::max(1,atoi(m_pBufferWord));
	 }

	 //��λend:
	 if(OffsetToString("end:",startLoation,endLocation))
	 {
		 index=m_iIndexBuffer+strlen("end:");
		 strcpy(m_pBufferWord,m_pBufferLine+strlen("end:"));
		 m_iEndFrame=atoi(m_pBufferWord);
	 }
      m_iFrameStep=std::max(1,m_iFrameStep);
	  m_iFrameCount=(m_iEndFrame-m_iStartFrame)/m_iFrameStep+1;
	  m_vpFramesMatchPoints.assign(m_iFrameCount,std::shared_ptr<std::vector<MatchPoint>>());  //m_iFrameCount�ǳ�Ա��������������������
	
	return true;
}

/***   ��ȡ�˶�������Ϣ  ***/
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

	//����whileѭ�����������ȡstart��end֮��������ַ���
/*
enum MotionType{FREE_MOVE,ROTATION_ONLY};         //�˶�����{�����˶�������ת}
enum FocalType{FOCAL_KNOWN,FOCAL_CONSTANT,FOCAL_VARIABLE};     //��������{������֪�����಻�䣬����仯}
enum AspectType{ASPECT_KNOWN,ASPECT_CONSTANT,ASPECT_VARIABLE};  // 
enum principalType{principal_KNOWN,principal_CONSTANT,principal_VARIABLE};  //����{������֪�����㲻�䣬����仯}
enum SkewType{SKEW_KNOWN,SKEW_CONSTANT,SKEW_VARIABLE};     //��������{������֪���������䣬�����仯}
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


/***��ȡ�ڲ���Ϣ***/
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

/*** ��ȡ���ٵ���Ϣ ***/
/********************************************
<Feature Tracks>   
      ����
<Feature Tracks>
*******************************************/
void ActsFileParser::ParseFeatureTrack()
{

	int start=-1,end=-1;
	int index;
	int i,j;
	m_iIndexBuffer=0;

	LocateContext("Feature Tracks",start,end);
	
	//δ�ҵ�
	if(start<0||end<0)
		return;

	m_iIndexBuffer=start;
   
	int frameStart,frameEnd;
	std::string str;

	char buffer[255];

	int iTrackCount;   //���ٵ����
	bool bLoadKeyPoints=false;  
	int iDimension =0;

  //��ȡ���ٵ���� 
	ReadWord();
	//��ά����Ϣ
	if(strcmp(m_pBufferWord,"kpts:")==0)
	{
		ReadWord();
		iDimension=atoi(m_pBufferWord);
		bLoadKeyPoints=true;

		ReadWord();
        iTrackCount=atoi(m_pBufferWord);
	}
    else    //��ά����Ϣ
	{
		iTrackCount=atoi(m_pBufferWord);
	}

	//���ƥ���洢�������ڴ洢�����ض�iTrackCount������TrackPoint��Ϣ
	m_vpMatchLinkers.clear();

	for(i=0;i<iTrackCount;i++){
    std::shared_ptr<MatchLinker>pSet=std::make_shared<MatchLinker>();   //(����ָ������ ��Ҫ��һ���˽�shared_ptr��make_shared���÷�
	
	int iTrackLength,iFlag,iErrorLever;
	
	ReadWord();
	iTrackLength=atoi(m_pBufferWord);     //���ٳ��ȣ��ж���֡���ٵ����Ϣ

	ReadWord();
	iFlag=atoi(m_pBufferWord);    //�Ƿ�����ά�㣨��Ӧ�ø��ٵ㣺0��ʾ�����ά�㣬��ά������洢��m_v3D�У�1��ʾδ��ø��ٵ㣬��������Ϊ�̶�ֵ<0.0000000	0.0000000	17.3869146>)

	ReadWord();
	iErrorLever=atoi(m_pBufferWord); //-1,1��3����ʾ�������Ƿ���ȷ

	ReadWord();                         //�洢������ά���꣨��δ��ã���Ϊ�̶�ֵ<0.0000000	0.0000000	17.3869146>��
		pSet->m_v3D[0]=atof(m_pBufferWord);
	ReadWord();
	    pSet->m_v3D[1]=atof(m_pBufferWord);
	ReadWord();
	    pSet->m_v3D[2]=atof(m_pBufferWord);

	pSet->m_iFlag=iFlag;
	pSet->m_iErrorLevel=iErrorLever;
	pSet->Reserve(iTrackLength);         //����洢�ռ�


	//�����ض�iTrackCount,��ȡ���ٵ��ά������Ϣ
	for(j=0;j<iTrackLength;++j)
	{
		TrackPoint &trackPoint=pSet->Point(j);  
	   //��ȡ���ٵ�����֡��
		ReadWord();
        trackPoint.m_iFrameNo=atoi(m_pBufferWord);                    
		if(m_vpFramesMatchPoints[trackPoint.m_iFrameNo]==false)
			m_vpFramesMatchPoints[trackPoint.m_iFrameNo]=std::make_shared<std::vector<MatchPoint>>();//���ĳһ֡��û�и��ٵ㣬�򴴽�һ���洢����

        trackPoint.m_iIndex=m_vpFramesMatchPoints[trackPoint.m_iFrameNo]->size();             //ĳһ֡���ж��ٸ����ٵ�

	  //��ȡ���ٵ�Ķ�ά����
		ReadWord();                                
		trackPoint.m_dX=atof(m_pBufferWord);
		ReadWord();
		trackPoint.m_dY=atof(m_pBufferWord);

		//��matchPoint�洢trackPoint�������ָ����Ϣ��������m_vpFramesMatchPoints�С�
		MatchPoint matchPoint;
	    matchPoint.m_dX=trackPoint.m_dX;   
		matchPoint.m_dY=trackPoint.m_dY;
		matchPoint.m_pMatchLinker=pSet;     
        m_vpFramesMatchPoints[trackPoint.m_iFrameNo]->push_back(matchPoint); //���ض�iTrackCount��m_iFrameNo��ƥ����������m_vpFramesMatchPoints��

	}
    pSet->m_index=m_vpMatchLinkers.size();    
	m_vpMatchLinkers.push_back(pSet);        
	
//	std::cout<<"m_vpMatchLinkers.size()"<<m_vpMatchLinkers.size()<<std::endl;

	//load KeyPoints                         //��ȡά����Ϣ��act�ļ����޴���Ϣ��
		if(bLoadKeyPoints)            
		{
			for(j=0;j<iDimension;++j)
			{
				ReadWord();
			}
		}

	}

}


/***��������˶��켣����ȡ����֡����Σ�***/
/******************************************
 <Camera Track>
   <FRAMEn>
     ����
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
          //������תΪ�ַ���          
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

		 start=frameEnd;   //��ȡ����֡��β��ٰ����ڶ�λ�Ͷ�ȡ��Χ��
		
	}

}