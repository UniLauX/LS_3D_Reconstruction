#include "runconfigdlg.h"


/************************************************************
   1.����checkBox ��Cancle��ť״̬ ѡ������Init,BO,DE
   2.RunConfigDlg��̳���QThread,������д��run����������start()����֮��ͻ�ִ��run���� 
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

	//�������ִ�в���
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
	return QDialog::exec();     //�ṩ��Ϣѭ�����ȴ���Ϣ������
}

void RunConfigDlg::accept()
{
	UpdateGet();
	QDialog::accept(); 

	m_MultiRunner.SetParameters(m_iStart,m_iEnd,m_iPass,m_bRunInit,m_bRunBO,m_bRunDE);
	//m_MultiRunner.start();  //QTread�Դ�������������ִ��run����
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


/****   ��ʼ��RunConfigDlg���Ը�����ֵ������֤  ****/
void RunConfigDlg::Initialize()
{
	setWindowFlags(static_cast<Qt::WindowFlags>(windowFlags() &Qt::WindowCloseButtonHint));    //���ô�����ʽ��ֻ��һ���رհ�ť��
	QIntValidator *intValidator=new QIntValidator(this);
	/*
	QIntValidator���ṩ��һ��ȷ��һ���ַ�������һ����һ����Ч��Χ�ڵ���������֤����

	validate()��������Acceptable��Intermediate��Invalid:

	Acceptable��ָ�ַ�����һ����ָ����Χ�ڵ���Ч����;
	Intermediate��ָ�ַ�����һ����Ч������������ָ����Χ��;
	Invalid��ָ�ַ�������һ����Ч����;
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


/*** ���ݽ������ø�����Ӧ����ֵ  ****/
void RunConfigDlg::UpdateGet()
{
	m_iStart=ui->lnEdt_Start->text().toInt();
	m_iEnd=ui->lnEdt_End->text().toInt();
	m_iPass=ui->lnEdt_Passes->text().toInt();

    m_bRunInit=ui->chkBox_Init->isChecked();
	m_bRunBO=ui->chkBox_BO->isChecked();
	m_bRunDE=ui->chkBox_DE->isChecked();
}



/***  ������Ӧ����ֵ���½�������  ****/
void RunConfigDlg::UpdateSet()
{
	//����start,end,passesֵ
	ui->lnEdt_Start->setText(QString::number(m_iStart));
	ui->lnEdt_End->setText(QString::number(m_iEnd));
	ui->lnEdt_Passes->setText(QString::number(m_iPass));


	//�����Ƿ�����init,BO, DE ��������
	ui->chkBox_Init->setChecked(this->m_bRunInit);
	ui->chkBox_BO->setChecked(this->m_bRunBO);
	ui->chkBox_DE->setChecked(this->m_bRunDE);

    //�����Ƿ�����м����ļ�
	ui->chkBox_OutputTmpDat->setChecked(this->m_bOutputTmpData);
     
}


