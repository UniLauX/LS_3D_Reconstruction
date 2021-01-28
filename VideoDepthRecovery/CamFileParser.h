
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
	void LocateContext(const char *str, int &startLocation,int &endLocation); //定位关键字符串（例如</Image Sequence>）起始位置，startLocation和endLocation为输出。
	void LocateContext(const char *str,int rangeBegin,int rangeEnd,int &startLocation,int &endLocation);//同上面函数，此外rangeBegin和rangeEnd用来限定OffsetToString定位和读取范围

    bool ParseSequence();   //分离起始帧、步长等信息（标签<Image Sequence>和</Image Sequence>之间的信息）
    void ParseMotionType(); //提取运动类型信息（标签<Motion Type>和</Motion Type>之间的信息）
    void ParseIntrinsicParameter(); //提取内参信息(标签<intrinsic parameter>和</intrinsic parameter>之间的信息)

	void ParseCameraTrack();       //提取相机运动轨迹信息（标签<Camera Track>和</Camera Track>之间的信息）-->外参信息
};

