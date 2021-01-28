#include "depthparawidget.h"

DepthParaWidget::DepthParaWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.reset(new Ui::DepthParaWidget());
	ui->setupUi(this);
	//必须初始化实例，否则会报错
	Inialize();
	UpdateSet();
	Publish();
}

DepthParaWidget::~DepthParaWidget()
{

}

void DepthParaWidget::Inialize()
{

	/***  验证DepthParaWidget表单的LineEdit中输入数值是否符合规定  ****/
	QIntValidator *intValidator=new QIntValidator(this);
	QDoubleValidator *doubleValidator=new QDoubleValidator(this);




	ui->lineEdit_BlockWidth->setValidator(intValidator);
	ui->lineEdit_BlockHeight->setValidator(intValidator);
	ui->lineEdit_BlockOverlap->setValidator(doubleValidator);  //相邻两幅图像重叠大小
    ui->lineEdit_Resample->setValidator(doubleValidator);

	       /****************
		            ……
		   ******************/


     ui->lineEdit_DspLevelNum->setValidator(intValidator);    //m+1,k
	 ui->lineEdit_DspMin->setValidator(doubleValidator);      //d.min
	 ui->lineEdit_DspMax->setValidator(doubleValidator);      //d.max
	 ui->lineEdit_DspColorSigma->setValidator(doubleValidator); //sigma_c
	 ui->lineEdit_DspDspSigma->setValidator(doubleValidator);   //sigma_d


	 //block | resample
	 m_iBlockWidth=2;    
	 m_iBlockheight=1;
	 m_dOverlap=5;  //块重叠大小（设置5个像素）
	 m_dResampleScale=1; //采样放缩系数（默认为1）


	 //Reference Frames Selection
	 m_bUnorderSequence = true;     //对这两个参数理解不深刻
	 m_bAutoOrderSelect = true;

	 m_iStartDist=1;   
	 m_iInitStep=2;
	 m_iInitCount=3;
	 m_iNormalStep=2;
	 m_iMaxCount=8;


	 //Meanshift Segmentation
	 m_iSegSpatial=5;
	 m_fSegColor=5;
	 m_iSegMinSize=50;
	 m_iPlaneFittingSize=300;
	 m_dSegErrRateThreshold=2.5;



	 //disparity 
	  m_dDspMin=1e-8;
	  m_dDspMax=0.017;
	  m_iDspLevel=101;
	  m_dColorSigma=5;
	  m_dDspSigma=2;
	  m_bEstimateDspRangeAutomatically=true;

	  ui->checkBox_AutoDspMax->setChecked(this->m_bEstimateDspRangeAutomatically);

	  //DE
	   m_iSubIter=2;
	   m_iSubLevels=10;



	  //Weight
	  m_dDataCostWeight = 20;
	  m_dSmoothTruncated = 10;
	  
    //初始化实例，否则会报错
	m_pDspEstimator=DspEstimator_Normal::GetInstance();
	m_pImgPartition=ImgPartiton::GetInstance();
	
	m_pFrameSelector=FrameSelector_MatchPoints::GetInstance();

	m_pDspEstimator->SetImgPartiton(m_pImgPartition);
	m_pDspEstimator->SetFrameSelector(m_pFrameSelector);

}

//根据变量设置DepthParaWidget界面上的参数
void DepthParaWidget::UpdateSet()
{

}

//从DepthParaWidget界面上获取参数赋值到相应的变量
void DepthParaWidget::UpDateGet()
{

     Publish();

	 m_bEstimateDspRangeAutomatically = ui->checkBox_AutoDspMax->isChecked();
}


void DepthParaWidget::Publish()
{
	PublishImgPartition();   //=图像分块
	PublishFrameSelector();  //选取参考帧
	PublishDisparityPara();  //disparity参数设置（主要为DataCost和MeanShift)
	PublishScale();
}


/*****************************************************/
     /****   Block& Scale   ***/
/*****************************************************/

void DepthParaWidget::PublishImgPartition()
{
	//设置初始分块参数（2,1,0.05）
	m_pImgPartition->Init(m_iBlockWidth,m_iBlockheight,m_dOverlap/100);
}

void DepthParaWidget::PublishScale(){
	VDRStructureMotion::GetInstance()->SetScaleVal(m_dResampleScale);
}


/*********************************************************/
       /** Reference Frame Select **/
/********************************************************/
void DepthParaWidget::PublishFrameSelector()
{
	if(m_bUnorderSequence == true){
		FrameSelector_MatchPoints::GetInstance()->Init(m_iMaxCount);
		m_pFrameSelector = FrameSelector_MatchPoints::GetInstance();
	}
	//未写完

	m_pDspEstimator->SetFrameSelector(m_pFrameSelector);
	
	//FrameSelector_Normal::GetInstance()->Init(m_iStartDist,m_iInitStep,m_iInitCount,m_iNormalStep,m_iMaxCount);
}

// 得到平滑截断项
double DepthParaWidget::GetTrueDisck()
{
   return m_dSmoothTruncated * (m_iDspLevel - 1)/ 100.0 ;   //不清楚
}

//not clearly
double DepthParaWidget::GetTruetSegErrRateThreshold()
{
	return m_dSegErrRateThreshold * GetTureDataCostWeight() / GetTrueDisck();
}


//得到DataCost的权重系数
double DepthParaWidget::GetTureDataCostWeight()
{
	return (m_iDspLevel-1)*m_dDataCostWeight/100.0;

}

/**********************************************/
  /*** Disparity ***/
/***********************************************/

// set the parameters about disparity
void DepthParaWidget::PublishDisparityPara()
{
	std::vector<PixelCostComputorBase *> pixelCostComputers;
	pixelCostComputers.push_back(PixelCostComputer_Init::GetInstance());
	pixelCostComputers.push_back(PixelCostComputor_Refine::GetInstance());
    for(int i=0;i<pixelCostComputers.size();++i)
	{
		PixelCostComputorBase *p=pixelCostComputers[i];
		p->SetDataCostWeight(GetTureDataCostWeight());                        //得到实际的DataCost权重系数
		p->SetSigma(m_dColorSigma,(m_dDspMax-m_dDspMin)*m_dDspSigma/100.0);   //设置sigma_c和sigma_d

	}
}


void DepthParaWidget::UpdateDspMax( bool updateData)
{
	if(ui->checkBox_AutoPartition->isChecked() == false)
		return;
	double dspMax = VDRStructureMotion::GetInstance()->GetMaxDsp();
	if(dspMax > 0){
		ui->lineEdit_DspMax->setText(QString::number(dspMax));
		if(updateData == true)
			m_dDspMax = dspMax;
	}
}