#pragma once
#include "ZImage.h"
#include "videodepthrecovery.h"

class VideoDepthRecovery;
class QImage;
class MainWindowControler
{
private:
	MainWindowControler(void);  //���캯��

public:
	~MainWindowControler(void); //��������
  
	//��ȡһ����̬ʵ������Ҫ����ʵ����������
	 static MainWindowControler *GetInstance()
	 {
		 static MainWindowControler instance;
		 return &instance;
	 }
public:
 void SetMainWindow(VideoDepthRecovery *pMainWindow);
 void ReleaseAllItems();

private:
		 VideoDepthRecovery *m_pMainWindow; //VdieoDepthRecoveryʵ��



};

