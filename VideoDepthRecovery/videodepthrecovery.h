#ifndef VIDEODEPTHRECOVERY_H
#define VIDEODEPTHRECOVERY_H

//Qtͷ�ļ�
#include <QtGui/QMainWindow>
#include "ui_videodepthrecovery.h"

//VideoDepthRecovery
#include "VDRStructureMotion.h"
#include "runconfigdlg.h"
#include "depthparawidget.h"

//��׼C++�Դ���ͷ�ļ�
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
	//���캯��
	VideoDepthRecovery(QWidget *parent = 0, Qt::WFlags flags = 0);
	
	//������̬ʵ������������������
	static VideoDepthRecovery *GetInstance()             
	{
		static VideoDepthRecovery instance;
	    return &instance;
	}
    //��̬����
	~VideoDepthRecovery();

	/***   ��ͼ���   ***/
     void ReleaseAllItems();


private:
	 void InitActions();

	
protected slots:       //�����ۺ�����������Ӧ�źŵĺ�����

	
		/*********���ļ�***********/
		   void onFileOpen();
		   void onSaveAs();
		   void onExit();

		   
		/******  ��Ȳ��� *********/
		   void onDepthParameters();



		/********������************/
		   void onRunInitialization();        

		   void onRunBundleOptimization();

		   void onRunDepthExpansion();


       /**********Lightstage����*******/
		   void onLightstageParameters();

		//********Lightstage��ά�ؽ�*********/
		   void onLightstageLoadProject();               //load project(with txt format��
		   void onLightstageRunVisualHull();             //run visualhull
           void onLightstageRunInit();                   //depth init
		   void onLightstageRunRefine();                 //depth refine(BO)
		   void onLightstageRunDepthExpansion();         //depth expansion
		   void onLightstageRunMesh();                   //3D mesh
		   void onLightstageRunAll();                    //run all operations

private:
	//���棨ui)
   // std::shared_ptr<Ui::VideoDepthRecoveryClass> ui;  
	Ui::VideoDepthRecoveryClass ui;
   	

	//��ͼ��View)
	bool m_bLocked;     //���г�������в�����ı����
};

#endif // VIDEODEPTHRECOVERY_H
