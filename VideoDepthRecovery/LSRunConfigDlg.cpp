#include "LSRunConfigDlg.h"

void LightstageMultiRuner::run()
{
    LSStructureMotion* LsSfm=LSStructureMotion::GetInstance();
	//m_iLsStart=0;
	//if(m_iLsEnd==-1)
   // m_iLsEnd=LsSfm->GetFrameCount()-1;
    //m_iLsEnd=1;    //�ȼ��㵥֡�ϵĽ����Ȼ�������չ

	std::cout<<"m_iLsStart: "<<m_iLsStart<<std::endl;
	std::cout<<"m_iLsEnd: "<<m_iLsEnd<<std::endl;

	if(m_bLsRunVisualhull==true)
	{
	    std::cout<<"execute lightstage Visualhull func"<<std::endl;
		LsSfm->CalculateVisualHull(m_iLsStart,m_iLsEnd);   // init m_iLsStart 0 and m_iLsEnd -1;
	}
	else if(m_bLsRunInit==true)
	{
		std::cout<<"execute lightstage Init func"<<std::endl;
	   LsSfm->RunInitialization(m_iLsStart,m_iLsEnd);

	}
	else if(m_bLsRunRefine==true)
	{
		std::cout<<"execute lightstage Refine func"<<std::endl;
		LsSfm->RunRefine(m_iLsStart,m_iLsEnd);

	}
	else if(m_bLsRunDepthExpansion==true)
	{
		std::cout<<"execute lightstage depthExpansion func"<<std::endl;
		LsSfm->RunDepthExpansion(m_iLsStart,m_iLsEnd);
	}


	else if(m_bLsRunMesh==true)
	{
	   LsSfm->DepthToMesh(m_iLsStart,m_iLsEnd);
       std::cout<<"execute lightstage Mesh func"<<std::endl;
	}

	else if(m_bLsRunAll==true)
	{

		LsSfm->RunAll(m_iLsStart,m_iLsEnd);
		std::cout<<"execute lightstage all func"<<std::endl;
	}
}

LSRunConfigDlg::LSRunConfigDlg(QWidget *parent)
	: QDialog(parent)
{
	 ui.reset(new Ui::LSRunConfigDlg());  
	 ui->setupUi(this);

//�źŲۺ���������ӦOK��Cancle�����¼�
   // QObject::connect(ui->pBtnLS_OK, SIGNAL(clicked()), this, SLOT(accept()));
   // QObject::connect(ui->LsBtnCancle, SIGNAL(clicked()), this, SLOT(reject()));
	 
	 // initialize parameters
	 Initialize();
	// onStarted();
	 std::cout<<"construct RunConfigDlg"<<std::endl;
}

LSRunConfigDlg::~LSRunConfigDlg()
{
}

 void LSRunConfigDlg:: Ls_accept()
{
	//UpdateGet();
	QDialog::accept();    //���OK ��ťʹconfigDlg �Ի���������ʧ
    m_LsMultiRunner.SetParameters(m_iLsStart,m_iLsEnd,m_iLsPass,m_bLsRunVisualhull,m_bLsRunInit,m_bLsRunRefine,m_bLsRunDepthExpansion,m_bLsRunMesh,m_bLsRunAll);
	//m_MultiRunner.start();  //QTread�Դ�������������ִ��run����
	m_LsMultiRunner.run();
}

void LSRunConfigDlg::Ls_reject()
{	
	QDialog::reject();
    std::cout<<"LSRunConfigDlg reject"<<std::endl;
}

int LSRunConfigDlg::exec()
{
	return QDialog::exec();

}


/******************************Lightstage��Ϣ���ݻ���**********************************/
                              //��visualhullΪ��
// 1.������Ŀ֮�󣬵���˵�����run_visualhull��ť������videodepthrecovery::onLightstageRunVisualHull()��
// 2.onLightstageRunVisualHull()�����е���LSRunConfigDlg::GetInstance()->exec()���ú����̳���Dialog������QDialog::exec(),�����Ի���
// 3.���OK��ť�����źŲ۹�����LSRunConfigDlg:: accept()������
// 4.LSRunConfigDlg:: accept()�����е���m_LsMultiRunner.run()��
// 5.m_LsMultiRunner.run()�е���CalculateVisuaHull()��
/****************************************************************************************/

void LSRunConfigDlg::Initialize()
{
     // UI design remaining.....
   /*
	setWindowFlags(static_cast<Qt::WindowFlags>(windowFlags() &Qt::WindowCloseButtonHint));    //���ô�����ʽ��ֻ��һ���رհ�ť��
	QIntValidator *intValidator=new QIntValidator(this);
	
	//QIntValidator���ṩ��һ��ȷ��һ���ַ�������һ����һ����Ч��Χ�ڵ���������֤����
	//validate()��������Acceptable��Intermediate��Invalid:
	//Acceptable��ָ�ַ�����һ����ָ����Χ�ڵ���Ч����;
	//Intermediate��ָ�ַ�����һ����Ч������������ָ����Χ��;
	//Invalid��ָ�ַ�������һ����Ч����;
    
	ui->lnEdt_Start->setValidator(intValidator);
	ui->lnEdt_End->setValidator(intValidator);
	ui->lnEdt_Passes->setValidator(intValidator);
	*/

	m_iLsStart=0;
	m_iLsEnd=0;
	m_iLsPass=2;   // may be changed according the depth recovery

    m_bLsRunVisualhull=true;
	m_bLsRunInit=true;
	m_bLsRunRefine=true;
	m_bLsRunDepthExpansion=true;
	m_bLsRunMesh=true;
	m_bLsRunAll=true;
	m_bLsOutputTmpData=true;   //wether output the tempory data

}