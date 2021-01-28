#pragma once
#include "ZImage.h"
#include "videodepthrecovery.h"

class VideoDepthRecovery;
class QImage;
class MainWindowControler
{
private:
	MainWindowControler(void);  //构造函数

public:
	~MainWindowControler(void); //析构函数
  
	//获取一个静态实例，主要用于实例化主界面
	 static MainWindowControler *GetInstance()
	 {
		 static MainWindowControler instance;
		 return &instance;
	 }
public:
 void SetMainWindow(VideoDepthRecovery *pMainWindow);
 void ReleaseAllItems();

private:
		 VideoDepthRecovery *m_pMainWindow; //VdieoDepthRecovery实例



};

