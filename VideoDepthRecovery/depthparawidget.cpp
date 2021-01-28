#include "depthparawidget.h"

DepthParaWidget::DepthParaWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.reset(new Ui::DepthParaWidget());
	ui->setupUi(this);
	//�����ʼ��ʵ��������ᱨ��
	Inialize();
	UpdateSet();
	Publish();
}

DepthParaWidget::~DepthParaWidget()
{

}

void DepthParaWidget::Inialize()
{

	/***  ��֤DepthParaWidget����LineEdit��������ֵ�Ƿ���Ϲ涨  ****/
	QIntValidator *intValidator=new QIntValidator(this);
	QDoubleValidator *doubleValidator=new QDoubleValidator(this);




	ui->lineEdit_BlockWidth->setValidator(intValidator);
	ui->lineEdit_BlockHeight->setValidator(intValidator);
	ui->lineEdit_BlockOverlap->setValidator(doubleValidator);  //��������ͼ���ص���С
    ui->lineEdit_Resample->setValidator(doubleValidator);

	       /****************
		            ����
		   ******************/


     ui->lineEdit_DspLevelNum->setValidator(intValidator);    //m+1,k
	 ui->lineEdit_DspMin->setValidator(doubleValidator);      //d.min
	 ui->lineEdit_DspMax->setValidator(doubleValidator);      //d.max
	 ui->lineEdit_DspColorSigma->setValidator(doubleValidator); //sigma_c
	 ui->lineEdit_DspDspSigma->setValidator(doubleValidator);   //sigma_d


	 //block | resample
	 m_iBlockWidth=2;    
	 m_iBlockheight=1;
	 m_dOverlap=5;  //���ص���С������5�����أ�
	 m_dResampleScale=1; //��������ϵ����Ĭ��Ϊ1��


	 //Reference Frames Selection
	 m_bUnorderSequence = true;     //��������������ⲻ���
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
	  
    //��ʼ��ʵ��������ᱨ��
	m_pDspEstimator=DspEstimator_Normal::GetInstance();
	m_pImgPartition=ImgPartiton::GetInstance();
	
	m_pFrameSelector=FrameSelector_MatchPoints::GetInstance();

	m_pDspEstimator->SetImgPartiton(m_pImgPartition);
	m_pDspEstimator->SetFrameSelector(m_pFrameSelector);

}

//���ݱ�������DepthParaWidget�����ϵĲ���
void DepthParaWidget::UpdateSet()
{

}

//��DepthParaWidget�����ϻ�ȡ������ֵ����Ӧ�ı���
void DepthParaWidget::UpDateGet()
{

     Publish();

	 m_bEstimateDspRangeAutomatically = ui->checkBox_AutoDspMax->isChecked();
}


void DepthParaWidget::Publish()
{
	PublishImgPartition();   //=ͼ��ֿ�
	PublishFrameSelector();  //ѡȡ�ο�֡
	PublishDisparityPara();  //disparity�������ã���ҪΪDataCost��MeanShift)
	PublishScale();
}


/*****************************************************/
     /****   Block& Scale   ***/
/*****************************************************/

void DepthParaWidget::PublishImgPartition()
{
	//���ó�ʼ�ֿ������2,1,0.05��
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
	//δд��

	m_pDspEstimator->SetFrameSelector(m_pFrameSelector);
	
	//FrameSelector_Normal::GetInstance()->Init(m_iStartDist,m_iInitStep,m_iInitCount,m_iNormalStep,m_iMaxCount);
}

// �õ�ƽ���ض���
double DepthParaWidget::GetTrueDisck()
{
   return m_dSmoothTruncated * (m_iDspLevel - 1)/ 100.0 ;   //�����
}

//not clearly
double DepthParaWidget::GetTruetSegErrRateThreshold()
{
	return m_dSegErrRateThreshold * GetTureDataCostWeight() / GetTrueDisck();
}


//�õ�DataCost��Ȩ��ϵ��
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
		p->SetDataCostWeight(GetTureDataCostWeight());                        //�õ�ʵ�ʵ�DataCostȨ��ϵ��
		p->SetSigma(m_dColorSigma,(m_dDspMax-m_dDspMin)*m_dDspSigma/100.0);   //����sigma_c��sigma_d

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