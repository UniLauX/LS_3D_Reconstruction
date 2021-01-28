#include "runconfigdlg.h"


/************************************************************
   1.根据checkBox 和Cancle按钮状态 选择运行Init,BO,DE
   2.RunConfigDlg类继承了QThread,这里重写了run函数。调用start()函数之后就会执行run函数 
*************************************************************/
void DepthRecoveryMultiRuner::run()
{
	DspEstimatorBase *dspEstimator=DepthParaWidget::GetInstance()->m_pDspEstimator;

	dspEstimator->SetCanceled(false);

	if(m_bRunInit==true&&dspEstimator->isCanceled()==false)
		dspEstimator->Run_Init(m_iStart,m_iEnd);
	if(m_bRunBO==true&&dspEstimator->isCanceled()==false)
	    dspEstimator->Run_BO(m_iStart,m_iEnd,m_iPass);
	if(m_bRunDE==true&&dspEstimator->isCanceled()==false)
		dspEstimator->Run_DE(m_iStart,m_iEnd,m_iPass);

	std::cout<<"run depth recovery"<<std::endl;

}


RunConfigDlg::RunConfigDlg(QWidget *parent)
	: QDialog(parent)
{

	ui.reset(new Ui::RunConfigDlg());  
	ui->setupUi(this);

    QObject::connect(ui->pBtn_OK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->pBtn_Cancle, SIGNAL(clicked()), this, SLOT(reject()));
	
	Initialize();

	//不清楚其执行步骤
    // connect(&m_MultiRunner,SIGNAL(finished()),this,SLOT(onFinished()));
	// connect(&m_MultiRunner,SIGNAL(started()),this,SLOT(onStarted()));
	
	onStarted();
    std::cout<<"construct RunConfigDlg"<<std::endl;
}

RunConfigDlg::~RunConfigDlg()
{

}

int RunConfigDlg::exec()
{
	UpdateSet();
	return QDialog::exec();     //提供消息循环，等待消息被触发
}

void RunConfigDlg::accept()
{
	UpdateGet();
	QDialog::accept(); 

	m_MultiRunner.SetParameters(m_iStart,m_iEnd,m_iPass,m_bRunInit,m_bRunBO,m_bRunDE);
	//m_MultiRunner.start();  //QTread自带函数，接下来执行run函数
	m_MultiRunner.run();
	std::cout<<"execute accept function"<<std::endl;
}



void RunConfigDlg::reject()
{
	std::cout<<"execute reject function"<<std::endl;
	QDialog::reject();
}


void RunConfigDlg::onFinished()
{
   std::cout<<"execute onFinished function"<<std::endl;
	//MainWindowControler::GetInstance()->
   QMessageBox::information(NULL,tr("error"),tr("hello message"));
}

void RunConfigDlg::onStarted()
{
     //MainWindowControler::GetInstance()->
	 std::cout<<"excute onStarted function"<<std::endl;
	// m_MultiRunner.run();
}


/****   初始化RunConfigDlg并对各项数值进行验证  ****/
void RunConfigDlg::Initialize()
{
	setWindowFlags(static_cast<Qt::WindowFlags>(windowFlags() &Qt::WindowCloseButtonHint));    //设置窗口样式（只有一个关闭按钮）
	QIntValidator *intValidator=new QIntValidator(this);
	/*
	QIntValidator类提供了一个确保一个字符串包含一个在一定有效范围内的整数的验证器。

	validate()函数返回Acceptable、Intermediate或Invalid:

	Acceptable是指字符串是一个在指定范围内的有效整数;
	Intermediate是指字符串是一个有效整数，但不在指定范围内;
	Invalid是指字符串不是一个有效整数;
    */
	ui->lnEdt_Start->setValidator(intValidator);
	ui->lnEdt_End->setValidator(intValidator);
	ui->lnEdt_Passes->setValidator(intValidator);
	

	m_iStart=0;
	m_iEnd=-1;
	m_iPass=2;

	m_bRunInit=true;
	m_bRunBO=true;
	m_bRunDE=true;

	m_bOutputTmpData=true;

}


/*** 根据界面设置更新相应变量值  ****/
void RunConfigDlg::UpdateGet()
{
	m_iStart=ui->lnEdt_Start->text().toInt();
	m_iEnd=ui->lnEdt_End->text().toInt();
	m_iPass=ui->lnEdt_Passes->text().toInt();

    m_bRunInit=ui->chkBox_Init->isChecked();
	m_bRunBO=ui->chkBox_BO->isChecked();
	m_bRunDE=ui->chkBox_DE->isChecked();
}



/***  根据相应变量值更新界面设置  ****/
void RunConfigDlg::UpdateSet()
{
	//设置start,end,passes值
	ui->lnEdt_Start->setText(QString::number(m_iStart));
	ui->lnEdt_End->setText(QString::number(m_iEnd));
	ui->lnEdt_Passes->setText(QString::number(m_iPass));


	//设置是否运行init,BO, DE 三个步骤
	ui->chkBox_Init->setChecked(this->m_bRunInit);
	ui->chkBox_BO->setChecked(this->m_bRunBO);
	ui->chkBox_DE->setChecked(this->m_bRunDE);

    //设置是否输出中间结果文件
	ui->chkBox_OutputTmpDat->setChecked(this->m_bOutputTmpData);
     
}


