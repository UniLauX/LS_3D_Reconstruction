#ifndef DEPTHPARAWIDGET_H
#define DEPTHPARAWIDGET_H

#include <QWidget>
#include "ui_depthparawidget.h"
#include "DspEstimatorBase.h"
#include "DspEstimator_Normal.h"
#include "ImgPartitionBase.h"
#include "ImgPartiton.h"
#include "FrameSelectorBase.h"
#include "FrameSelector_MatchPoints.h"

class DepthParaWidget : public QWidget
{
	Q_OBJECT

public:
	DepthParaWidget(QWidget *parent = 0);
	~DepthParaWidget();

	void Inialize();  //��ʼ��ʵ��
    void UpdateSet();  //���ݱ�������DeptthParaWidget�����ϵĲ���
	void UpDateGet();  //��DepthParaWidget�����ϻ�ȡ������ֵ����Ӧ�ı���
	void Publish();    


private:
	 /***********************************************************/
	       /***  Publisher   ***/
	 /**********************************************************/

	 // block & scale
	void PublishImgPartition();
	void PublishScale();
	//disparity
	void PublishDisparityPara();

	// FrameSelector
	void PublishFrameSelector();

public:
	   double GetTrueDisck();   //�õ�ƽ���ض�ֵ
	   double GetTruetSegErrRateThreshold();

	   void UpdateDspMax(bool updateData);//update ui, it is determined by parameters two whether should update data.



public: 

    //�õ���̬ʵ���������ⲿ����
	static DepthParaWidget *GetInstance()
		{
			static DepthParaWidget instance;
			return &instance;
		}

private:
	 double GetTureDataCostWeight();

public:
    DspEstimatorBase *m_pDspEstimator;          //����disparity �Ļ�����ʵ��
	ImgPartitionBase *m_pImgPartition;         //����֡�����У��ֿ�Ļ���ʵ��
	FrameSelectorBase * m_pFrameSelector;      //ѡȡ�ο�֡�Ļ���ʵ��



	// �ο�֡ѡȡ

	bool m_bUnorderSequence;  //ͼ�������Ƿ���
	bool m_bAutoOrderSelect;  //�Ƿ��Զ�ѡ��

	int m_iStartDist;//��ʼ����
	int m_iInitStep; //��ʼ����
	int m_iInitCount; //��ʼ���ο���֡��
	int m_iNormalStep;//��������
	int m_iMaxCount;  //��󣨵��ࣩ֡��

// MeanShift Segmentation
	int m_iSegSpatial;     //�������
    float m_fSegColor;     //��ɫ����
	int m_iSegMinSize;     
	int m_iPlaneFittingSize;
	double m_dSegErrRateThreshold;






	//block & resample
	int m_iBlockWidth;   //����
	int m_iBlockheight;  //��߶�
    double m_dOverlap;   //���ص���С
	double m_dResampleScale;    //��������ϵ��


	//disparity
	double m_dDspMin;   //��Сdisparityֵ  d.min
	double m_dDspMax;   //���disparityֵ  d.max
	int  m_iDspLevel;   //disparity�Ĳ���  ��[d.min,d.max]֮�乲m+1�㣨Level)       
	bool m_bEstimateDspRangeAutomatically;     //�Զ�����disparity(Ĭ������£�

	//DE 
	int m_iSubIter;               //�����������
	int m_iSubLevels;

private:
	//weight
	double m_dDataCostWeight;   //DataCost��Ȩ��ϵ��
	double m_dSmoothTruncated;  //ƽ���ض���ο�����ʽ4.1��

	//disparity
	double m_dColorSigma;       //sigma_c
	double m_dDspSigma;         //sigma_d
private:
	std::shared_ptr<Ui::DepthParaWidget> ui;

};

#endif // DEPTHPARAWIDGET_H
