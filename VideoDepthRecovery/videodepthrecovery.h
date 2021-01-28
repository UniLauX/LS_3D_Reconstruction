#ifndef VIDEODEPTHRECOVERY_H
#define VIDEODEPTHRECOVERY_H

//Qt头文件
#include <QtGui/QMainWindow>
#include "ui_videodepthrecovery.h"

//VideoDepthRecovery
#include "VDRStructureMotion.h"
#include "runconfigdlg.h"
#include "depthparawidget.h"

//标准C++自带库头文件
#include <iostream>
#include <string>


//Lightstage
#include "LSStructureMotion.h"
#include "LSRunConfigDlg.h"


class QAction;
class VideoDepthRecovery : public QMainWindow
{
	Q_OBJECT

public:
	//构造函数
	VideoDepthRecovery(QWidget *parent = 0, Qt::WFlags flags = 0);
	
	//建立静态实例，便于其他类引用
	static VideoDepthRecovery *GetInstance()             
	{
		static VideoDepthRecovery instance;
	    return &instance;
	}
    //静态函数
	~VideoDepthRecovery();

	/***   视图相关   ***/
     void ReleaseAllItems();


private:
	 void InitActions();

	
protected slots:       //建立槽函数（用来响应信号的函数）

	
		/*********打开文件***********/
		   void onFileOpen();
		   void onSaveAs();
		   void onExit();

		   
		/******  深度参数 *********/
		   void onDepthParameters();



		/********求解深度************/
		   void onRunInitialization();        

		   void onRunBundleOptimization();

		   void onRunDepthExpansion();


       /**********Lightstage参数*******/
		   void onLightstageParameters();

		//********Lightstage三维重建*********/
		   void onLightstageLoadProject();               //load project(with txt format）
		   void onLightstageRunVisualHull();             //run visualhull
           void onLightstageRunInit();                   //depth init
		   void onLightstageRunRefine();                 //depth refine(BO)
		   void onLightstageRunDepthExpansion();         //depth expansion
		   void onLightstageRunMesh();                   //3D mesh
		   void onLightstageRunAll();                    //run all operations

private:
	//界面（ui)
   // std::shared_ptr<Ui::VideoDepthRecoveryClass> ui;  
	Ui::VideoDepthRecoveryClass ui;
   	

	//视图（View)
	bool m_bLocked;     //运行程序过程中不允许改变组件
};

#endif // VIDEODEPTHRECOVERY_H
