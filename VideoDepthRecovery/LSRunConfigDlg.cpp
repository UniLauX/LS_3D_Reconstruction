#include "LSRunConfigDlg.h"

void LightstageMultiRuner::run()
{
    LSStructureMotion* LsSfm=LSStructureMotion::GetInstance();
	//m_iLsStart=0;
	//if(m_iLsEnd==-1)
   // m_iLsEnd=LsSfm->GetFrameCount()-1;
    //m_iLsEnd=1;    //先计算单帧上的结果，然后进行扩展

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

//信号槽函数用来响应OK和Cancle单击事件
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
	QDialog::accept();    //点击OK 按钮使configDlg 对话框正常消失
    m_LsMultiRunner.SetParameters(m_iLsStart,m_iLsEnd,m_iLsPass,m_bLsRunVisualhull,m_bLsRunInit,m_bLsRunRefine,m_bLsRunDepthExpansion,m_bLsRunMesh,m_bLsRunAll);
	//m_MultiRunner.start();  //QTread自带函数，接下来执行run函数
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


/******************************Lightstage消息传递机制**********************************/
                              //以visualhull为例
// 1.加载项目之后，点击菜单子栏run_visualhull按钮，调用videodepthrecovery::onLightstageRunVisualHull()。
// 2.onLightstageRunVisualHull()函数中调用LSRunConfigDlg::GetInstance()->exec()（该函数继承自Dialog，调用QDialog::exec(),弹出对话框。
// 3.点击OK按钮，由信号槽关联到LSRunConfigDlg:: accept()函数。
// 4.LSRunConfigDlg:: accept()函数中调用m_LsMultiRunner.run()。
// 5.m_LsMultiRunner.run()中调用CalculateVisuaHull()。
/****************************************************************************************/

void LSRunConfigDlg::Initialize()
{
     // UI design remaining.....
   /*
	setWindowFlags(static_cast<Qt::WindowFlags>(windowFlags() &Qt::WindowCloseButtonHint));    //设置窗口样式（只有一个关闭按钮）
	QIntValidator *intValidator=new QIntValidator(this);
	
	//QIntValidator类提供了一个确保一个字符串包含一个在一定有效范围内的整数的验证器。
	//validate()函数返回Acceptable、Intermediate或Invalid:
	//Acceptable是指字符串是一个在指定范围内的有效整数;
	//Intermediate是指字符串是一个有效整数，但不在指定范围内;
	//Invalid是指字符串不是一个有效整数;
    
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