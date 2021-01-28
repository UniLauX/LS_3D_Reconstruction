#ifndef RUNCONFIGDLG_H
#define RUNCONFIGDLG_H

#include <QDialog>
//#include <QThread>
#include "ui_runconfigdlg.h"
#include <iostream>

#include <QMessageBox>

#include "DspEstimatorBase.h"
#include "depthparawidget.h"



class DepthRecoveryMultiRuner:public QObject /*:public QThread*/
{
	Q_OBJECT         //只有加入了Q_OBJECT,才能使用QT中的signal和slot机制

public:

	//构造函数
	DepthRecoveryMultiRuner(){};
 
	//设置runConfigDlg界面中的参数
    void SetParameters(int iStart,int iEnd,int iPass,bool bInit,bool bBO,bool bDE)
	{
	     m_iStart=iStart;
		 m_iEnd=iEnd;
		 m_iPass=iPass;

		 m_bRunInit=bInit;
		 m_bRunBO=bBO;
		 m_bRunDE=bDE;
		//m_iStart=iStart;
	}

public:
	//选择执行init,BO,DE中的某一个或者某几个
	void run();

	/*
private signal:
	void start();

private slots:
	void on_start();
*/
private:
	int m_iStart;   //起始帧号
	int m_iEnd;     //结束帧号
	int m_iPass;    //执行几遍

	//获取checkBox中的值
	bool m_bRunInit;
	bool m_bRunBO;
	bool m_bRunDE;

	//QThread *m_thread;
};


//主类
class RunConfigDlg : public QDialog
{
	Q_OBJECT

private:
	DepthRecoveryMultiRuner m_MultiRunner;   //定义一个DepthRecovery实例

private:
	RunConfigDlg(QWidget *parent = 0);
	
	//声明 RunconfigDlg 初始化（包含整数值验证）
	void Initialize();

	//更新RunconfigDlg 界面各项数值获取
    void UpdateGet();
	
	//更新RunconfigDlg 界面各项数值设定
	void UpdateSet();


public:
	~RunConfigDlg();
	 static RunConfigDlg* GetInstance()
	 {
		 static RunConfigDlg instance;
		 return &instance;
	 }

	 void SetRunModel(bool runInit, bool runBO,bool runDE)
	 {
		 m_bRunInit=runInit;
		 m_bRunBO=runBO;
		 m_bRunDE=runDE;
	 }




private:
	//Ui::RunConfigDlg ui;
    std::shared_ptr<Ui::RunConfigDlg> ui;

private:

	int m_iStart;
	int m_iEnd;
	int m_iPass;

	/***   标志是否运行 ***/
	bool m_bRunInit;
	bool m_bRunBO;
	bool m_bRunDE;

    bool m_bOutputTmpData;

public slots:
	   
	//覆盖原函数声明
	virtual void accept() override;   //响应OK 按钮
	virtual void reject() override;  //响应reject按钮

	int exec();

	void onFinished();
	void onStarted();

};

#endif // RUNCONFIGDLG_H
