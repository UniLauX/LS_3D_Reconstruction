#include "videodepthrecovery.h"
#include <QtGui/QAction> 
#include <QtGui/QKeySequence> 
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
//#include <memory>


VideoDepthRecovery::VideoDepthRecovery(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	//ui->reset(new Ui::VideoDepthRecoveryClass());
	ui.setupUi(this);

   //��ʼ���źŲ�
	InitActions();

    //set depth parameters UI
    ui.dockWidget_DepthPara->setWidget(DepthParaWidget::GetInstance());
	ui.dockWidget_DepthPara->setVisible(false);
	//ui.dockWidget_DepthPara->setVisible(true);
	
/*
	openAction=new QAction(tr("&Open"),this);    //QAciton�������ò˵����͹���
   //openAction->setShortcut(QKeySequence::Open);             //QKeySequenceΪQt�ڲ��ṩ����ӿ�ݼ�
//�����˵���
	QMenu *file=menuBar()->addMenu(tr("&File"));
	file->addAction(openAction); //���½���QMenu��file)��֮ǰ������QAction(openAction)����
	//file->addAction(tr("bb"));
	//QMenu *vdr=menuBar()->addMenu(tr("&vdr"));
	menuBar()->addMenu(tr("&Open"));

//����������
	QToolBar *toolBar=addToolBar(tr("&File"));
	toolBar->addAction(openAction);                         //���½���QToolBar(toolBar)��֮ǰ������QAction(openAction)����
	connect(openAction,SIGNAL(triggered()),this,SLOT(onFileOpen()));    //�����ź���ۣ�triggered(��ΪQAciton�Ĵ�������
*/

/*  ע��
   //�����˵���
	QMenu *mFile=menuBar()->addMenu(tr("&File"));    
	QAction *actionOpen=mFile->addAction(tr("&Open"));
    QAction *actionSaveAs=mFile->addAction(tr("&SaveAs"));
	QAction *actionExit=mFile->addAction(tr("&Exit"));
	
   //���
	QMenu *mDepth=menuBar()->addMenu(tr("&Depth"));
	mDepth->addAction(tr("&Parameters"));

  //ģ��
	QMenu *mModel=menuBar()->addMenu(tr("&Model"));

 //�����Ȳ���
	QMenu *mRun=menuBar()->addMenu(tr("&Run"));
	QAction *actionRunInitialization=mRun->addAction(tr("RunInitialization"));
  	QAction *actionRunBundleOptimization=mRun->addAction(tr("RunBundleOptimization"));
    QAction *actionRunDepthExpansion=mRun->addAction(tr("RunDepthExpansion"));
	*/


	/**************************************************
	            �ź���������
    ****************************************************/

/*
	//�˵���
	connect(actionOpen,SIGNAL(triggered()),this,SLOT(onFileOpen())); 
	connect(actionSaveAs,SIGNAL(triggered()),this,SLOT(onFileOpen())); 
	connect(actionExit,SIGNAL(triggered()),this,SLOT(onFileOpen())); 


	//������
	connect(actionRunInitialization,SIGNAL(triggered()),this,SLOT(onRunInitialization()));
	connect(actionRunBundleOptimization,SIGNAL(triggered()),this,SLOT(onRunBundleOptimization()));
	connect(actionRunDepthExpansion,SIGNAL(triggered()),this,SLOT(onRunDepthExpansion()));
*/
}

//��������
VideoDepthRecovery::~VideoDepthRecovery()
{

}


void VideoDepthRecovery::InitActions()
{
	/**************************************************
	            �ź���������
    ***************************************************/

	
	//file
	connect(ui.actionOpen,SIGNAL(triggered()),this,SLOT(onFileOpen()));
	connect(ui.actionSaveAs,SIGNAL(triggered()),this,SLOT(onSaveAs())); 
	connect(ui.actionExit,SIGNAL(triggered()),this,SLOT(onExit())); 
    
	//set depth parameters
	connect(ui.actionDepthParameters,SIGNAL(triggered()),this,SLOT(onDepthParameters()));
	
	//for video depth recovery
	connect(ui.actionRun_Initialization,SIGNAL(triggered()),this,SLOT(onRunInitialization()));
	connect(ui.actionRun_BundleOptimization,SIGNAL(triggered()),this,SLOT(onRunBundleOptimization()));
	connect(ui.actionRun_DepthExpansion,SIGNAL(triggered()),this,SLOT(onRunDepthExpansion()));

	//set lightstage parameters
	connect(ui.actionLS_Parameters,SIGNAL(triggered()),this,SLOT(onLightstageParameters()));

	//for lightstage 
	connect(ui.actionLoadProject,SIGNAL(triggered()),this,SLOT(onLightstageLoadProject()));
	connect(ui.actionRun_VisualHull,SIGNAL(triggered()),this,SLOT(onLightstageRunVisualHull()));
    connect(ui.actionLSRun_Init,SIGNAL(triggered()),this,SLOT(onLightstageRunInit()));
	connect(ui.actionLSRun_Refine,SIGNAL(triggered()),this,SLOT(onLightstageRunRefine()));
	connect(ui.actionLSRun_DE,SIGNAL(triggered()),this,SLOT(onLightstageRunDepthExpansion()));
	connect(ui.actionLSRun_Mesh,SIGNAL(triggered()),this,SLOT(onLightstageRunMesh()));
	connect(ui.actionLSRun_All,SIGNAL(triggered()),this,SLOT(onLightstageRunAll()));

}


// Qt�������Ĳۺ��������Լ�ʵ�֣�����ᱨ��   //*����Ƶ��*///
void VideoDepthRecovery::onFileOpen()                 
{
	//QMessageBox::information(NULL,tr("Open"),tr("Open a file"));  
    QString qFilePath=QFileDialog::getOpenFileName(this,tr("Open video sequence"),".",tr("Act Files(*act *actb *wcb)"));    //�����ļ�
    if(qFilePath.length()==0)
	{
		QMessageBox::information(NULL,tr("Get Path Failed"),tr("You didn't select any files"));
	}
	else
	{
		//filePathΪact�ļ�·��
	     std::string filePath=qFilePath.toStdString();
		 std::cout<<"Load acts filePath: " <<filePath<<std::endl;   
		 //�����ļ��Ժ���Structure frome motion��ȡ�����ͽ��п���
		  VDRStructureMotion::GetInstance()->LoadFrames(filePath);

    //ȱ������

	}
}

void VideoDepthRecovery::onSaveAs()
{
	std::cout<<"on save as"<<std::endl;
}


void VideoDepthRecovery::onExit()
{
	std::cout<<"File Exit"<<std::endl;
}


//���г�ʼ��(Init)
void VideoDepthRecovery::onRunInitialization()
{
	std::cout<<"Run Init"<<std::endl;

	if(VDRStructureMotion::GetInstance()->GetFrameCount()==0)
	{
		QMessageBox::information(NULL, tr("Warning"), tr("Load project first,please!"));
		return;
	}

	RunConfigDlg::GetInstance()->SetRunModel(true,false,false);
	RunConfigDlg::GetInstance()->exec();

	std::cout<<"Init Over"<<std::endl;
}


//������Լ����Bundel Optimization��
void VideoDepthRecovery::onRunBundleOptimization()
{
	std::cout<<"Run BO"<<std::endl;

	RunConfigDlg::GetInstance()->SetRunModel(false,true,false);
	RunConfigDlg::GetInstance()->exec();

	std::cout<<"Init Over"<<std::endl;
}

void VideoDepthRecovery::onRunDepthExpansion()
{
    std::cout<<"Run DE"<<std::endl;
    RunConfigDlg::GetInstance()->SetRunModel(false,false,true);
	RunConfigDlg::GetInstance()->exec();
}

void VideoDepthRecovery::ReleaseAllItems()
{
	ui.menuBar->setEnabled(true);                  //�˵���
	ui.dockWidget_DepthPara->setEnabled(true);     //������Ȳ���������
	m_bLocked=false;	
}


void VideoDepthRecovery::onDepthParameters()
{
	ui.dockWidget_DepthPara->setVisible(true);
	std::cout<<"setup DepthParameters"<<std::endl;
}


//set lightstage parameters
void VideoDepthRecovery::onLightstageParameters()
{
	std::cout<<"setup LightstageParameters"<<std::endl;
}







/*************************************************************************/
             /** Lightstage  3D Recovery ***/
/***********************************************************************/


//lightstage�ṹ��Ҫ���ݵ����cam�ļ�������Ӧ�Ĺ���Ŀ¼��
//ÿ���������Ƶ���е���Ϊһ���ļ���
void VideoDepthRecovery::onLightstageLoadProject()
{ 
	//std::cout<<"on load camera file name list of lightstage project!"<<std::endl;  //print
	QString qFilePath=QFileDialog::getOpenFileName(this,tr("Load camera file name list"),".",tr("camFileList Files(*txt)"));    //load txt file
	
	if(qFilePath.length()==0)     // load txt file failed.
	{
		QMessageBox::information(NULL,tr("Get Path Failed"),tr("You didn't select any files"));
	}
	else
	{
		std::string filePath=qFilePath.toStdString();
		std::cout<<"Load camera file list Path: " <<filePath<<std::endl;
		std::cout<<std::endl;
		
	  //  VDRStructureMotion::GetInstance()->LoadLightstageFileFolders(filePath);

		LSStructureMotion::GetInstance()->LS_LoadProject(filePath);

		/*
		std::vector< std::string > camFileNameList;
		const int maxLine = 1024;
		static  char inLine[maxLine + 10];

        //read camera file name list
		std::ifstream  inStream;
		inStream.open(filePath);
		if( inStream.is_open() )
		{
			while ( !inStream.eof() )                                      
			{
				inStream.getline( inLine,maxLine );
				
				if( strlen(inLine)>5 &&inLine[0]!=' ')  //�������е�Сbug,�ʼ�inline[0]!=' �������ж϶����һ�������ַ���
				{
					camFileNameList.push_back( std::string( inLine ) );   //list of cam file
				}     
			}
		 }
		inStream.close();


		int ith = 0;
		for(std::vector<std::string>::iterator  iter = camFileNameList.begin();
			iter!=camFileNameList.end(); ++iter,++ith )
		{
			std::string   pathName(*iter);
			std::cout<<camFileNameList.size()<<" tasks in total,process "<<ith+1<<" at present"<<std::endl;
			std::cout<<"pathName: "<<pathName<<std::endl;
		    std::string   pathDir = FileNameParser::findFileNameDir( pathName );
			std::cout<<"pathDir: "<<pathDir<<std::endl;     //����07cam��ʱ����pathName��PathDir���֮������������У������ԭ��~����Ӱ��������ܣ�ֻ����������ۣ�
		    VDRStructureMotion::GetInstance()->LoadLightstageFrames(pathName);  //LoadFrames �еĸ�ʽ��������camFileParser�е�����	
		}
	*/
	 }
	
	std::cout<<"--------------------------------------------------"<<std::endl;
}




void VideoDepthRecovery::onLightstageRunVisualHull()
{
	std::cout<<"on lightstage run visualhull start"<<std::endl;	

	if(LSStructureMotion::GetInstance()->GetFrameCount()==0)
	{
		QMessageBox::information(NULL, tr("Warning"), tr("Load project first,please!"));
		return;
	}
	
	 LSRunConfigDlg::GetInstance()->SetRunModel(true,false,false,false,false,false); // assgin run VisualHull on ConfigDlg
	 LSRunConfigDlg::GetInstance()->exec();   // show run dlg

	if (LSRunConfigDlg::GetInstance()->result() == QDialog::Accepted)
	{
		LSRunConfigDlg::GetInstance()->Ls_accept();   //��config Dig ��accept��������ִ������
	}
	else if(LSRunConfigDlg::GetInstance()->result()==QDialog::Rejected)
	{
	    LSRunConfigDlg::GetInstance()->Ls_reject();
	}

	std::cout<<"onLightstageRunVisualHull over!"<<std::endl;
    std::cout<<"------------------------------------------"<<std::endl;
}

void VideoDepthRecovery::onLightstageRunInit()
{
   std::cout<<"on lightstage runInit"<<std::endl;
  
   
   if(LSStructureMotion::GetInstance()->GetFrameCount()==0)
   {
	   QMessageBox::information(NULL, tr("Warning"), tr("Load project first,please!"));
	   return;
   }

   /* �ж��Ƿ��Ѿ�����õ�VisualHull �ļ�
    //if(not found Vh.file)
   {
	   QMessageBox::information(NULL, tr("Warning"), tr("Make sure you have load VisualHull file,please!"));
	   return;
   }
   */

   LSRunConfigDlg::GetInstance()->SetRunModel(false,true,false,false,false,false);
   LSRunConfigDlg::GetInstance()->exec();


   if (LSRunConfigDlg::GetInstance()->result() == QDialog::Accepted)
   {
	   LSRunConfigDlg::GetInstance()->Ls_accept();   //��config Dig ��accept��������ִ������
   }
   else if(LSRunConfigDlg::GetInstance()->result()==QDialog::Rejected)
   {
	   LSRunConfigDlg::GetInstance()->Ls_reject();
   }

   std::cout<<"onLightstageRunInit over!"<<std::endl;
   std::cout<<"------------------------------------------"<<std::endl;

}



void VideoDepthRecovery::onLightstageRunRefine()
{
   std::cout<<"on lightstage runRefine start"<<std::endl;
 
   if(LSStructureMotion::GetInstance()->GetFrameCount()==0)
   {
	   QMessageBox::information(NULL, tr("Warning"), tr("Load project first,please!"));
	   return;
   }

   /* �ж��Ƿ��Ѿ�����õ�VisualHull �ļ�
    //if(not found Vh.file)
   {
	   QMessageBox::information(NULL, tr("Warning"), tr("Make sure you have load VisualHull file,please!"));
	   return;
   }
   */

   LSRunConfigDlg::GetInstance()->SetRunModel(false,false,true,false,false,false);
   LSRunConfigDlg::GetInstance()->exec();


   if (LSRunConfigDlg::GetInstance()->result() == QDialog::Accepted)
   {
	   LSRunConfigDlg::GetInstance()->Ls_accept();   //��config Dig ��accept��������ִ������
   }
   else if(LSRunConfigDlg::GetInstance()->result()==QDialog::Rejected)
   {
	   LSRunConfigDlg::GetInstance()->Ls_reject();
   }

   std::cout<<"onLightstageRunRefine over!"<<std::endl;
   std::cout<<"------------------------------------------"<<std::endl;


}

void VideoDepthRecovery::onLightstageRunDepthExpansion()
{
	std::cout<<"on lightstage runDepthExpansion"<<std::endl;
	if(LSStructureMotion::GetInstance()->GetFrameCount()==0)
   {
	   QMessageBox::information(NULL, tr("Warning"), tr("Load project first,please!"));
	   return;
   }

   /* �ж��Ƿ��Ѿ�����õ�VisualHull �ļ�
    //if(not found Vh.file)
   {
	   QMessageBox::information(NULL, tr("Warning"), tr("Make sure you have load VisualHull file,please!"));
	   return;
   }
   */

   LSRunConfigDlg::GetInstance()->SetRunModel(false,false,false,true,false,false);
   LSRunConfigDlg::GetInstance()->exec();


   if (LSRunConfigDlg::GetInstance()->result() == QDialog::Accepted)
   {
	   LSRunConfigDlg::GetInstance()->Ls_accept();   //��config Dig ��accept��������ִ������
   }
   else if(LSRunConfigDlg::GetInstance()->result()==QDialog::Rejected)
   {
	   LSRunConfigDlg::GetInstance()->Ls_reject();
   }

   std::cout<<"onLightstageRunDepthExpansion over!"<<std::endl;
   std::cout<<"------------------------------------------"<<std::endl;

}




void VideoDepthRecovery::onLightstageRunMesh()
{
   std::cout<<"on lightstage runMesh"<<std::endl;

   	if(LSStructureMotion::GetInstance()->GetFrameCount()==0)
   {
	   QMessageBox::information(NULL, tr("Warning"), tr("Load project first,please!"));
	   return;
   }

   /* �ж��Ƿ��Ѿ�����õ�VisualHull �ļ�
    //if(not found Vh.file)
   {
	   QMessageBox::information(NULL, tr("Warning"), tr("Make sure you have load VisualHull file,please!"));
	   return;
   }
   */
   LSRunConfigDlg::GetInstance()->SetRunModel(false,false,false,false,true,false);
   LSRunConfigDlg::GetInstance()->exec();

   if (LSRunConfigDlg::GetInstance()->result() == QDialog::Accepted)
   {
	   LSRunConfigDlg::GetInstance()->Ls_accept();   //��config Dig ��accept��������ִ������
   }
   else if(LSRunConfigDlg::GetInstance()->result()==QDialog::Rejected)
   {
	   LSRunConfigDlg::GetInstance()->Ls_reject();
   }

   std::cout<<"onLightstageRunMesh over!"<<std::endl;
   std::cout<<"------------------------------------------"<<std::endl;
}


void VideoDepthRecovery::onLightstageRunAll()
{
	std::cout<<"on lightstage runAll"<<std::endl;
	
	if(LSStructureMotion::GetInstance()->GetFrameCount()==0)
	{
		QMessageBox::information(NULL, tr("Warning"), tr("Load project first,please!"));
		return;
	}

	LSRunConfigDlg::GetInstance()->SetRunModel(false,false,false,false,false,true);
	LSRunConfigDlg::GetInstance()->exec();


	if (LSRunConfigDlg::GetInstance()->result() == QDialog::Accepted)
	{
		LSRunConfigDlg::GetInstance()->Ls_accept();   //��config Dig ��accept��������ִ������
	}
	else if(LSRunConfigDlg::GetInstance()->result()==QDialog::Rejected)
	{
		LSRunConfigDlg::GetInstance()->Ls_reject();
	}

	std::cout<<"onLightstageAll over!"<<std::endl;
	std::cout<<"------------------------------------------"<<std::endl;

}



/*************************************************************************/
/** <3D Recovery> ***/
/***********************************************************************/