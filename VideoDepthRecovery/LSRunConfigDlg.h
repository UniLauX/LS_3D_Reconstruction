#ifndef LSRunConfigDlg_H
#define LSRunConfigDlg_H

#include <QDialog>
#include <ui_LSRunConfigDlg.h>
#include "LSStructureMotion.h"

#include <iostream>


class LightstageMultiRuner:public QObject /*:public QThread*/
{
	Q_OBJECT         //只有加入了Q_OBJECT,才能使用QT中的signal和slot机制

public:
	//构造函数
	LightstageMultiRuner(){};

	//设置runConfigDlg界面中的参数
    void SetParameters(int iLsStart,int iLsEnd,int iLsPass,bool bLsVH,bool bLsInit,bool bLsRef,bool bLsDE,bool bLsMesh,bool bLsAll)
	{
	     m_iLsStart=iLsStart;
		 m_iLsEnd=iLsEnd;
		 m_iLsPass=iLsPass;

		 m_bLsRunVisualhull=bLsVH;
		 m_bLsRunInit=bLsInit;
		 m_bLsRunRefine=bLsRef;
		 m_bLsRunDepthExpansion=bLsDE;
		 m_bLsRunMesh=bLsMesh;
		 m_bLsRunAll=bLsAll;
		//m_iStart=iStart;
	}

public:
	//选择执行init,BO,DE中的某一个或者某几个
	void run();

public:
	std::string m_sLsWorkDir;

	/*
private signal:
	void start();

private slots:
	void on_start();
*/

private:
	int m_iLsStart;   //起始帧号
	int m_iLsEnd;     //结束帧号
	int m_iLsPass;    //执行几遍


	//获取checkBox中的值
	bool m_bLsRunVisualhull;      //visualhull
	bool m_bLsRunInit;           //depth init
	bool m_bLsRunRefine;         //depth refine（BO)
    bool m_bLsRunDepthExpansion; //depth expansion
	bool m_bLsRunMesh;           //depth mesh
	bool m_bLsRunAll;            //run all operations

	//QThread *m_thread;
};

class LSRunConfigDlg : public QDialog
{
	Q_OBJECT

private:
	LightstageMultiRuner m_LsMultiRunner;   //定义一个程序运行的实例

private:
     LSRunConfigDlg(QWidget *parent=0);   //parent=0很关键，不然不能够初始化静态实例
	
	 
	 //initialize  LSRunconfigDlg( UI design remain...) 
	 void Initialize();



public:
	
	~LSRunConfigDlg();


	static LSRunConfigDlg *GetInstance()
	{
		static LSRunConfigDlg instance;
		return &instance;

	}


	//set and decide which part should be run
	void SetRunModel(bool runLsVH,bool runLsInit, bool runLsRefine,bool runLsDE,bool runLsMesh,bool runLsAll)
	{
		m_bLsRunVisualhull=runLsVH;
		m_bLsRunInit=runLsInit;
		m_bLsRunRefine=runLsRefine;
		m_bLsRunDepthExpansion=runLsDE;
		m_bLsRunMesh=runLsMesh;
		m_bLsRunAll=runLsAll;
	}

	int GetStartFrame()
	{
		return m_iLsStart;
	}

	int GetEndFrame()
	{
		return m_iLsEnd;
	}


	void SetFrameRange(int lsStart,int lsEnd)
	{
		m_iLsStart=lsStart;
		m_iLsEnd=lsEnd;
	}



private:
	//Ui::LSRunConfigDlg ui;
	std::shared_ptr<Ui::LSRunConfigDlg> ui;    //此类和相应的UI关联起来
	
private:

	int m_iLsStart;
	int m_iLsEnd;
	int m_iLsPass;

	/***   标志是否运行 ***/
	bool m_bLsRunVisualhull;
	bool m_bLsRunInit;
	bool m_bLsRunRefine;
	bool m_bLsRunDepthExpansion;
	bool m_bLsRunMesh;
	bool m_bLsRunAll;
	bool m_bLsOutputTmpData;



public slots:
		//覆盖原函数声明
	    void Ls_accept();   //响应OK 按钮
		void Ls_reject();  //响应reject按钮

		int exec();      //覆盖Dialog中的同名（exec）函数

};

#endif // LSRunConfigDlg_H
