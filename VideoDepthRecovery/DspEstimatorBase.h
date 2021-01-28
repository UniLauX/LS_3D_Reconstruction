#pragma once
#include <QReadWriteLock>   //读写锁头文件
#include "FrameSelectorBase.h"
#include"ImgPartitionBase.h"
#include "ImgPartiton.h"
#include "PixelCostComputorBase.h"
class DspEstimatorBase
{
public:
	//构造函数
	DspEstimatorBase(void):m_pImgPartition(nullptr),m_bCanceled(false){};

	//析构函数
	~DspEstimatorBase(void);


	//声明 Init,BO,DE实现函数
	virtual void Run_Init(int start,int end)=0;
	virtual void Run_BO(int start,int end,int pass)=0;
	virtual void Run_DE(int start,int end,int pass)=0;


	//设置帧选择指针
    void SetFrameSelector(FrameSelectorBase * selector)
	{
		m_pFrameSelector=selector;
	}
	void SetImgPartiton(ImgPartitionBase *imgParitition)
	{
		m_pImgPartition=imgParitition;
	}




	//判断是否点击Cancle按钮
    bool isCanceled()
	{
		bool canceled=false;
	    //lock.lockForRead();   //读（加锁）
	    canceled=m_bCanceled;
        //lock.unlock();        //读（解锁）
		return m_bCanceled;
	}



	//根据Val值设置Cancel按钮
	void SetCanceled(bool val)
	{
	  //  lock.lockForWrite(); //写（加锁）
		m_bCanceled=val;
		//lock.unlock();       //写（解锁）
	}

public:
	FrameSelectorBase *m_pFrameSelector;  //定义帧选择指针

	
	
protected:
	QReadWriteLock lock;  //只允许一个写（其他不能读）；或者多个同时读取
	bool m_bCanceled;    //标识是否点击了Cancle按钮
    ImgPartitionBase * m_pImgPartition; 
	PixelCostComputorBase *m_pPixelCostComputer;

};

