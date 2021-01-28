#include "MainWindowControler.h"


MainWindowControler::MainWindowControler(void)
{
	m_pMainWindow=nullptr;
}


MainWindowControler::~MainWindowControler(void)
{
}

//¶ÔÏó¸³Öµ
void MainWindowControler::SetMainWindow(VideoDepthRecovery *pMainWindow)
{
	m_pMainWindow=pMainWindow;
}

void MainWindowControler::ReleaseAllItems()
{
	//m_pMainWindow->ReleaseAllItems();
	  m_pMainWindow->ReleaseAllItems();
}

