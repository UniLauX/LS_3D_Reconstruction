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
	Q_OBJECT         //ֻ�м�����Q_OBJECT,����ʹ��QT�е�signal��slot����

public:

	//���캯��
	DepthRecoveryMultiRuner(){};
 
	//����runConfigDlg�����еĲ���
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
	//ѡ��ִ��init,BO,DE�е�ĳһ������ĳ����
	void run();

	/*
private signal:
	void start();

private slots:
	void on_start();
*/
private:
	int m_iStart;   //��ʼ֡��
	int m_iEnd;     //����֡��
	int m_iPass;    //ִ�м���

	//��ȡcheckBox�е�ֵ
	bool m_bRunInit;
	bool m_bRunBO;
	bool m_bRunDE;

	//QThread *m_thread;
};


//����
class RunConfigDlg : public QDialog
{
	Q_OBJECT

private:
	DepthRecoveryMultiRuner m_MultiRunner;   //����һ��DepthRecoveryʵ��

private:
	RunConfigDlg(QWidget *parent = 0);
	
	//���� RunconfigDlg ��ʼ������������ֵ��֤��
	void Initialize();

	//����RunconfigDlg ���������ֵ��ȡ
    void UpdateGet();
	
	//����RunconfigDlg ���������ֵ�趨
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

	/***   ��־�Ƿ����� ***/
	bool m_bRunInit;
	bool m_bRunBO;
	bool m_bRunDE;

    bool m_bOutputTmpData;

public slots:
	   
	//����ԭ��������
	virtual void accept() override;   //��ӦOK ��ť
	virtual void reject() override;  //��Ӧreject��ť

	int exec();

	void onFinished();
	void onStarted();

};

#endif // RUNCONFIGDLG_H
