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

	void Inialize();  //初始化实例
    void UpdateSet();  //根据变量设置DeptthParaWidget界面上的参数
	void UpDateGet();  //从DepthParaWidget界面上获取参数赋值到相应的变量
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
	   double GetTrueDisck();   //得到平滑截断值
	   double GetTruetSegErrRateThreshold();

	   void UpdateDspMax(bool updateData);//update ui, it is determined by parameters two whether should update data.



public: 

    //得到静态实例，便于外部引用
	static DepthParaWidget *GetInstance()
		{
			static DepthParaWidget instance;
			return &instance;
		}

private:
	 double GetTureDataCostWeight();

public:
    DspEstimatorBase *m_pDspEstimator;          //估计disparity 的基础类实例
	ImgPartitionBase *m_pImgPartition;         //进行帧（序列）分块的基类实例
	FrameSelectorBase * m_pFrameSelector;      //选取参考帧的基类实例



	// 参考帧选取

	bool m_bUnorderSequence;  //图像序列是否按序
	bool m_bAutoOrderSelect;  //是否自动选择

	int m_iStartDist;//初始距离
	int m_iInitStep; //初始步长
	int m_iInitCount; //初始（参考）帧数
	int m_iNormalStep;//正常步长
	int m_iMaxCount;  //最大（单侧）帧数

// MeanShift Segmentation
	int m_iSegSpatial;     //距离参数
    float m_fSegColor;     //颜色参数
	int m_iSegMinSize;     
	int m_iPlaneFittingSize;
	double m_dSegErrRateThreshold;






	//block & resample
	int m_iBlockWidth;   //块宽度
	int m_iBlockheight;  //块高度
    double m_dOverlap;   //块重叠大小
	double m_dResampleScale;    //采样放缩系数


	//disparity
	double m_dDspMin;   //最小disparity值  d.min
	double m_dDspMax;   //最大disparity值  d.max
	int  m_iDspLevel;   //disparity的层数  在[d.min,d.max]之间共m+1层（Level)       
	bool m_bEstimateDspRangeAutomatically;     //自动估计disparity(默认情况下）

	//DE 
	int m_iSubIter;               //计算迭代次数
	int m_iSubLevels;

private:
	//weight
	double m_dDataCostWeight;   //DataCost的权重系数
	double m_dSmoothTruncated;  //平滑截断项，参看【公式4.1】

	//disparity
	double m_dColorSigma;       //sigma_c
	double m_dDspSigma;         //sigma_d
private:
	std::shared_ptr<Ui::DepthParaWidget> ui;

};

#endif // DEPTHPARAWIDGET_H
