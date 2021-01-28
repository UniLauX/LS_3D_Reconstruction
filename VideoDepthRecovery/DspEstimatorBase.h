#pragma once
#include <QReadWriteLock>   //��д��ͷ�ļ�
#include "FrameSelectorBase.h"
#include"ImgPartitionBase.h"
#include "ImgPartiton.h"
#include "PixelCostComputorBase.h"
class DspEstimatorBase
{
public:
	//���캯��
	DspEstimatorBase(void):m_pImgPartition(nullptr),m_bCanceled(false){};

	//��������
	~DspEstimatorBase(void);


	//���� Init,BO,DEʵ�ֺ���
	virtual void Run_Init(int start,int end)=0;
	virtual void Run_BO(int start,int end,int pass)=0;
	virtual void Run_DE(int start,int end,int pass)=0;


	//����֡ѡ��ָ��
    void SetFrameSelector(FrameSelectorBase * selector)
	{
		m_pFrameSelector=selector;
	}
	void SetImgPartiton(ImgPartitionBase *imgParitition)
	{
		m_pImgPartition=imgParitition;
	}




	//�ж��Ƿ���Cancle��ť
    bool isCanceled()
	{
		bool canceled=false;
	    //lock.lockForRead();   //����������
	    canceled=m_bCanceled;
        //lock.unlock();        //����������
		return m_bCanceled;
	}



	//����Valֵ����Cancel��ť
	void SetCanceled(bool val)
	{
	  //  lock.lockForWrite(); //д��������
		m_bCanceled=val;
		//lock.unlock();       //д��������
	}

public:
	FrameSelectorBase *m_pFrameSelector;  //����֡ѡ��ָ��

	
	
protected:
	QReadWriteLock lock;  //ֻ����һ��д���������ܶ��������߶��ͬʱ��ȡ
	bool m_bCanceled;    //��ʶ�Ƿ�����Cancle��ť
    ImgPartitionBase * m_pImgPartition; 
	PixelCostComputorBase *m_pPixelCostComputer;

};

