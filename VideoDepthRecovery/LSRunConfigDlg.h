#ifndef LSRunConfigDlg_H
#define LSRunConfigDlg_H

#include <QDialog>
#include <ui_LSRunConfigDlg.h>
#include "LSStructureMotion.h"

#include <iostream>


class LightstageMultiRuner:public QObject /*:public QThread*/
{
	Q_OBJECT         //ֻ�м�����Q_OBJECT,����ʹ��QT�е�signal��slot����

public:
	//���캯��
	LightstageMultiRuner(){};

	//����runConfigDlg�����еĲ���
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
	//ѡ��ִ��init,BO,DE�е�ĳһ������ĳ����
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
	int m_iLsStart;   //��ʼ֡��
	int m_iLsEnd;     //����֡��
	int m_iLsPass;    //ִ�м���


	//��ȡcheckBox�е�ֵ
	bool m_bLsRunVisualhull;      //visualhull
	bool m_bLsRunInit;           //depth init
	bool m_bLsRunRefine;         //depth refine��BO)
    bool m_bLsRunDepthExpansion; //depth expansion
	bool m_bLsRunMesh;           //depth mesh
	bool m_bLsRunAll;            //run all operations

	//QThread *m_thread;
};

class LSRunConfigDlg : public QDialog
{
	Q_OBJECT

private:
	LightstageMultiRuner m_LsMultiRunner;   //����һ���������е�ʵ��

private:
     LSRunConfigDlg(QWidget *parent=0);   //parent=0�ܹؼ�����Ȼ���ܹ���ʼ����̬ʵ��
	
	 
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
	std::shared_ptr<Ui::LSRunConfigDlg> ui;    //�������Ӧ��UI��������
	
private:

	int m_iLsStart;
	int m_iLsEnd;
	int m_iLsPass;

	/***   ��־�Ƿ����� ***/
	bool m_bLsRunVisualhull;
	bool m_bLsRunInit;
	bool m_bLsRunRefine;
	bool m_bLsRunDepthExpansion;
	bool m_bLsRunMesh;
	bool m_bLsRunAll;
	bool m_bLsOutputTmpData;



public slots:
		//����ԭ��������
	    void Ls_accept();   //��ӦOK ��ť
		void Ls_reject();  //��Ӧreject��ť

		int exec();      //����Dialog�е�ͬ����exec������

};

#endif // LSRunConfigDlg_H
