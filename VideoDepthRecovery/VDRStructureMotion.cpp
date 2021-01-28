#include "VDRStructureMotion.h"


VDRStructureMotion::VDRStructureMotion(void)
{

}


VDRStructureMotion::~VDRStructureMotion(void)
{
}

// ����: ������Ƶ����
bool VDRStructureMotion::LoadFrames(const std::string &actsFilePath)  
{
	      //�������в����������ͷ��ڴ棩
          Reset();	

		  //�ж��ļ����ͣ���׺�Ƿ�Ϊact��actb)
		  if(FileNameParser::findFileNameExt(actsFilePath)=="act")
		  {
			  m_pActsParser=ActsFileParser::GetInstance();
		  }
		  else if(FileNameParser::findFileNameExt(actsFilePath)=="actb")
		  {
			  m_pActsParser=ActbFileParser::GetInstance();                       //should be the actb file
		  }
		  /*
		  else if(FileNameParser::findFileNameExt(actsFilePath)=="cam")
		  {
			 m_pActsParser=CamFileParser::GetInstance();
		  }
		 */
		  else 
		  {
			  std::cout<<"the input file isn't act/actb file"<<std::endl;
			  return  false;
		  }
		  
		  //������Ŀ��act/actb)�ļ��Ƿ�ɹ�
		  if(m_pActsParser->LoadProject(actsFilePath)==false)
		  {
			  std::cout<<"LoadProject Failed!"<<std::endl;
			  return false;
		  }




         //���ò���������ͼ�����д洢�ռ�
		  int start=m_pActsParser->m_iStartFrame;
		  int step=m_pActsParser->m_iFrameStep;
		  int count=m_pActsParser->m_iFrameCount;
          std::cout<<"Total Frame count: "<<count<<std::endl;

		  CreateFrames(count);


		  /**********************************************************************/
		  /*         Path & Extrinsic Parameters                             */
		  /**********************************************************************/
	    std::string imageFilePath=m_pActsParser->m_sImagePath;
		  imageFilePath=FileNameParser::increaseFileNameNumber(imageFilePath,start); //F://Data/test/test0000.jpg
		  std::cout<<imageFilePath<<std::endl;
		 
		  for(int k=0;k<count;++k)
		  {
			   VDRVideoFrame *pFrame=GetFrameAt(k);
			   pFrame->m_pvMatchPoints=m_pActsParser->m_vpFramesMatchPoints[k];
			   pFrame->m_iID=k;                //֡�ţ�Frame Number��
			   pFrame->m_sImagePathName=imageFilePath;  //ÿ��ͼ���·������(����֡��֮ǰ��
			   pFrame->m_sName=FileNameParser::findFileNameNameWitoutExt(imageFilePath);//�õ��޺�׺��ͼ�����ƣ�test0nnn)
               imageFilePath=FileNameParser::increaseFileNameNumber(imageFilePath,step);//�õ�ͼ��ȫ·��������ͼ�񼰺�׺����F://Data/test/test0nnn.jpg

			   /*  test
			   {
			   std::cout<<"m_iD: "<<pFrame->m_iID<<std::endl;
			   std::cout<<"m_sImageNamePath: "<<pFrame->m_sImageNamePath<<std::endl;
			   std::cout<<"m_sName: "<<pFrame->m_sName<<std::endl;
			   std::cout<<"imageFilePath��"<<imageFilePath<<std::endl;
			   }
			   */

			   //��õ�k֡��ξ���R,T
			   Wml::Matrix4d &RT=m_pActsParser->m_vdExtrinsicParameters[k];
			   for(int row=0;row<3;row++)
			   {
				   for(int col=0;col<4;col++)
				   {
					   double extriVal=RT(row,col);
					   if(col!=3)
						    pFrame->SetCameraParameterRAt(row,col,extriVal);   //��R��ֵ
					   else
						   pFrame->SetCameraParameterTAt(row,extriVal);        //��T��ֵ
				   }
				    
			   }
		  }

		  /******************************************************************
		          Size &  Folder
		  *******************************************************************/
		  if(count>0)
		  {
			 CxImage templeImage;
			 //�ж�ͼ���ļ��Ƿ�����ȷ����
			 if(!templeImage.Load(GetFrameAt(0)->m_sImagePathName.c_str(),CxImageFunction::GetImageFileType(imageFilePath)))
			 {
				 std::cout<<"can't correctly load the image file: "<<imageFilePath<<std::endl;
				 return false;
			 }
		    VDRVideoFrame::InitImageSzie(templeImage.GetWidth(),templeImage.GetHeight());
			 
			VDRVideoFrame::s_tempDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("TMP/");
            VDRVideoFrame::s_dataDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("DATA/");
			//VDRVideoFrame::s_removalDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("REMOVAL/");

			CreateDirectory(VDRVideoFrame::s_tempDir.c_str(),0);
			CreateDirectory(VDRVideoFrame::s_dataDir.c_str(),0);
			//CreateDirectory(VDRVideoFrame::s_removalDir.c_str(),0);

		  }

		  /************************************************************************/
		  /* Intrinsic Parameter  & Scale                                         */
		  /************************************************************************/
		  
		  //�ԣ�0,1��λ���ڲβ��Ǻ���Ϥ
		 /*
		  m_K.MakeZero();
		  m_K(0,0)=m_pActsParser->m_dFx;  m_K(0,1)=m_pActsParser->m_dSkew*m_pActsParser->m_dFx;   m_K(0,2)=m_pActsParser->m_dCx;
		  m_K(1,1)=m_pActsParser->m_dFy;  m_K(1,2)=m_pActsParser->m_dCy;
		  m_K(2,2)=1;
		  */
		  m_K.MakeZero();
		  m_K(0, 0) = m_pActsParser->m_dFx;		m_K(0, 1) = m_pActsParser->m_dSkew * m_pActsParser->m_dFx;	m_K(0, 2) = m_pActsParser->m_dCx;
		  m_K(1, 1) = m_pActsParser->m_dFy;		m_K(1, 2) = m_pActsParser->m_dCy;
		  m_K(2, 2) = 1;


		 // std::cout<<"M_K"<<m_K(0,0)<<m_K(0,1)<<m_K(0,2)<<m_K(1,0)<<m_K(1,1)<<std::endl;
		// std::cout<<"����M_K:"<<m_K(0,0)<<":"<<m_K(1,1)<<std::endl;

		 std::cout<<"Load Frames Successfully!"<<std::endl;

		 /*
               DepthParaWidget ��δд
		 */
		 //���÷�������ڲξ���Ԫ��ֵ
		 SetScaleVal(DepthParaWidget::GetInstance()->m_dResampleScale);


		 m_vpMatchLinkers=std::move(m_pActsParser->m_vpMatchLinkers);


		 //bug!!!!!
	 // std::cout<<"mvvvvvpSize: "<<m_vpMatchLinkers.size()<<std::endl;

		  DepthParaWidget::GetInstance()->UpdateDspMax(true);
		  DepthParaWidget::GetInstance()->Publish();
		  return true;

}

//���壺 ���ò���
void VDRStructureMotion::Reset()
{
	  m_iFrameCount=0;
	  mFrames.clear();
	  m_dAutoMaxDsp=-1;   //�Ƿ��Զ�����Disparity�����ֵ�������������أ�
	  m_K.MakeZero();    //�ڲξ���ȫ����0
	  m_ScaledK.MakeZero();  //����ͼ��Scaled) �ڲξ���ȫ����0
	  if(m_pActsParser!=nullptr)
		 m_pActsParser->clear();
	  m_pActsParser=nullptr;
}

void VDRStructureMotion::CreateFrames(int iCount)
{
	std::cout<<"exec the CreateFrames func"<<std::endl;

	m_iFrameCount=iCount;
	mFrames.resize(m_iFrameCount);
	for(int k=0;k<m_iFrameCount;++k)
	{
		mFrames[k]=std::make_shared<VDRVideoFrame>(m_K,m_ScaledK);
		
		/*
		make_shared()�������Խ������10��������Ȼ������Ǵ��ݸ�����T�Ĺ��캯����
		����һ��shared_ptr<T>�Ķ� �󲢷��ء�make_shared()����Ҫ��ֱ�Ӵ���shared_ptr����ķ�ʽ���Ҹ�Ч��
		��Ϊ���ڲ�������һ���ڴ棬������shared_ptr ����ʱ�Ŀ�����
		*/
	}
}

//���÷�������ڲξ���Ԫ��ֵ
void VDRStructureMotion::SetScaleVal(double dScale)
{
	 if(VDRVideoFrame::s_dScale!=dScale)
		 VDRVideoFrame::s_dScale=dScale;

	 m_ScaledK.MakeZero();
	 m_ScaledK(0,0)=m_K(0,0)*dScale;   m_ScaledK(0,1)=m_K(0,1)*dScale;      m_ScaledK(0,2)=m_K(0,2)*dScale;
	 m_ScaledK(1,1)=m_K(1,1)*dScale;   m_ScaledK(1,2)=m_K(1,2)*dScale;
	 m_ScaledK(2,2)=1;
}

//�õ�Dispartiy �ķ�Χ����Ҫ��ϸ�鿴
double  VDRStructureMotion::EstimateDisparityRange()
{
	if(m_vpMatchLinkers.size()<=0)
		std::cout<<"m_vpMatchLinkers<=0"<<std::endl;

	if(m_vpMatchLinkers.size()>0)
	{
		double minDepth = 1e10;	
	    bool bValid = false;
	    std::vector<double> depthList;

		for(auto i=0;i<m_vpMatchLinkers.size();++i)
		{
			std::shared_ptr<MatchLinker> &pTrack=m_vpMatchLinkers[i];
			if(pTrack->m_iErrorLevel==-1||pTrack->m_iErrorLevel==3)   //�Ը����ļ��д˺������Ǻ���⣨-1,3����������Ƿ���ȷ��
				continue;      
			Wml::Vector3d pt3d=pTrack->m_v3D;      //�洢�������ά��
			for(auto j=0;j<pTrack->GetCount();++j)
			{
				TrackPoint &trackpt = pTrack->Point(j);
				if(trackpt.m_iFrameNo >= m_iFrameCount)
					continue;
				
				VDRVideoFrame  *pFrame=GetFrameAt(trackpt.m_iFrameNo);

				double depthVal=pFrame->GetDepthFromWorldCoord(pt3d);   
					if(depthVal > 1e-10)
					{                              //�����
						//minDepth = min(minDepth, depthVal);
						depthList.push_back(depthVal);
						bValid = true;
					}				

			}

		}
		if(bValid)
		{                                                              //���������
			std::sort(depthList.begin(),depthList.end());
			minDepth = depthList.at(depthList.size() * 0.015);
			minDepth *= 0.7;
			return 1.0/minDepth;
		}

   }
	return -1.0;
}





/***************************************************************************/
                      //for 3D recovery of lightstage
/***************************************************************************/


bool VDRStructureMotion::LoadLightstageFileFolders(const std::string &cameraListPath)
{

	/***     ��ȡcam�ļ��б�    ***/
	std::vector< std::string > camFileNameList;
	const int maxLine = 1024;
	static  char inLine[maxLine + 10];

	//read camera file name list
	std::ifstream  inStream;
	inStream.open(cameraListPath);
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


	m_sWorkDir=FileNameParser::findFileNameDir(cameraListPath);
	std::cout<<"m_sWorkDir: "<<m_sWorkDir<<std::endl;

	camNum=camFileNameList.size();     //�����     
	std::cout<<"camNum: "<<camNum<<std::endl;

	//CamFileParser camFiles[];
	m_pCamListParser=new CamFileParser[camNum];

	
	//m_sMeshDir+=





	/*
	int ith = 0;
	for(std::vector<std::string>::iterator  iter = camFileNameList.begin();
		iter!=camFileNameList.end(); ++iter,++ith )
	{
		std::string   pathName(*iter);
		std::cout<<camFileNameList.size()<<" tasks in total,process "<<ith+1<<" at present"<<std::endl;
		std::cout<<"pathName: "<<pathName<<std::endl;
		std::string   pathDir = FileNameParser::findFileNameDir( pathName );
		std::cout<<"pathDir: "<<pathDir<<std::endl;     //����07cam��ʱ����pathName��PathDir���֮������������У������ԭ��~����Ӱ��������ܣ�ֻ����������ۣ�
		
       //VDRStructureMotion::GetInstance()->LoadLightstageFrames(pathName);  //LoadFrames �еĸ�ʽ��������camFileParser�е�����


		if(FileNameParser::findFileNameExt(pathName)=="cam")
		{
			m_pActsParser=CamFileParser::GetInstance();
		}
		else 
		{
			std::cout<<"the input file isn't camera file"<<std::endl;
			return  false;
		}
	}
	*/



	return true;
}





//���壺lightstage�м�����Ƶ����  (��Ե��ǵ����ļ��У��������൱�ڼ��ص����ļ����еĹ��̣�
bool VDRStructureMotion::LoadLightstageFrames(const std::string &actsFilePath)
{
	  Reset();	
	  if(FileNameParser::findFileNameExt(actsFilePath)=="cam")
	  {
		  m_pActsParser=CamFileParser::GetInstance();
	  }
	  else 
	  {
		  std::cout<<"the input file isn't camera file"<<std::endl;
		  return  false;
	  }


	  //����cam�ļ������������Ϣ��ֵ��m_pActsParser
	  if(m_pActsParser->LoadProject(actsFilePath)==false)
	  {
		  std::cout<<"LoadProject Failed!"<<std::endl;
		  return false;
	  }
	 
	  //���ò���������ͼ�����д洢�ռ�
	  int start=m_pActsParser->m_iStartFrame;           //0
	  int step=m_pActsParser->m_iFrameStep;             //1
	  int count=m_pActsParser->m_iFrameCount;          //19
	  std::cout<<"Total Frame count: "<<count<<std::endl;
	//  std::cout<<"start: "<<start<<" step: "<<step<<" count: "<<count<<std::endl;

	  CreateFrames(count);    

	  
	  /**********************************************************************/
	  /*         Path & Extrinsic Parameters                             */
	  /**********************************************************************/
	  std::string imageFilePath=m_pActsParser->m_sImagePath;

	  std::cout<<"imageFilePath: "<<imageFilePath<<std::endl;    //F:\Data\show\0293\0000.jpg

	  imageFilePath=FileNameParser::increaseFileNameNumber(imageFilePath,start);   //F:\Data\show\0293\0000.jpg
	 // std::cout<<"imageFilePathOOO"<<imageFilePath<<std::endl;  
	  
	 
	  VDRVideoFrame::s_maskDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("MASK/");
	 // std::cout<<"mask directory: "<<VDRVideoFrame::s_maskDir<<std::endl;

	   for(int k=0;k<count;++k)
		  {
			   VDRVideoFrame *pFrame=GetFrameAt(k);
			   //pFrame->m_pvMatchPoints=m_pActsParser->m_vpFramesMatchPoints[k];
			   pFrame->m_iID=k;                //֡�ţ�Frame Number��
			   pFrame->m_sImagePathName=imageFilePath;  //ÿ��ͼ���·������(����֡��֮ǰ��
			   pFrame->m_sName=FileNameParser::findFileNameNameWitoutExt(imageFilePath);//�õ��޺�׺��ͼ�����ƣ�test0nnn)
               imageFilePath=FileNameParser::increaseFileNameNumber(imageFilePath,step);//�õ�ͼ��ȫ·��������ͼ�񼰺�׺����F://Data/test/test0nnn.jpg
			  
			   pFrame->m_sMaskImgPathName=VDRVideoFrame::s_maskDir+pFrame->m_sName+std::string(".bmp");

			  // std::cout<<"pFrame->m_sMaskImgPathName: "<<pFrame->m_sMaskImgPathName<<std::endl;


			   //pFrame->m_sImagePathName����֡�ţ�imageFilePath��һ��֡�� 
			   
			   /*test
			   {
			   std::cout<<"m_iD: "<<pFrame->m_iID<<std::endl;
			   std::cout<<"m_sImageNamePath: "<<pFrame->m_sImagePathName<<std::endl;   
			   std::cout<<"m_sName: "<<pFrame->m_sName<<std::endl;
			  // std::cout<<"imageFilePath��"<<imageFilePath<<std::endl;
			   }
			   */
                
			   //ע����ξ�������һ����δ�õ�������

			   //��õ�k֡��ξ���R,T
			   Wml::Matrix4d &RT=m_pActsParser->m_vdExtrinsicParameters[k];
			   for(int row=0;row<3;row++)
			   {
				   for(int col=0;col<4;col++)
				   {
					   double extriVal=RT(row,col);
					   if(col!=3)
						    pFrame->SetCameraParameterRAt(row,col,extriVal);   //��R��ֵ
					   else
						   pFrame->SetCameraParameterTAt(row,extriVal);        //��T��ֵ
				   }
				    
			   }

             //��õ�K֡�ڲξ���K
			   Wml::Matrix3d &K=m_pActsParser->m_vdIntrinsicParameters[k];
			   for(int row=0;row<3;row++)
			   {
				   for(int col=0;col<3;col++)
				   {
					   double intriVal=K(row,col);
					   pFrame->SetIntrinsicParameterAt(row,col,intriVal);
				   }
				   
			   }
	       
		  }


	     /******************************************************************
		          Size &  Folder
		  *******************************************************************/
		  if(count>0)
		  {
			 CxImage templeImage;
			 //�ж�ͼ���ļ��Ƿ�����ȷ����
			 if(!templeImage.Load(GetFrameAt(0)->m_sImagePathName.c_str(),CxImageFunction::GetImageFileType(imageFilePath)))
			 {
				 std::cout<<"can't correctly load the image file: "<<imageFilePath<<std::endl;
				 return false;
			 }
		    VDRVideoFrame::InitImageSzie(templeImage.GetWidth(),templeImage.GetHeight());
			 
			VDRVideoFrame::s_tempDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("TMP/");
            VDRVideoFrame::s_dataDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("DATA/");
			//VDRVideoFrame::s_removalDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("REMOVAL/");

			VDRVideoFrame::s_maskDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("MASK/");   //ֻ���ȡ�����ô���

			CreateDirectory(VDRVideoFrame::s_tempDir.c_str(),0);
			CreateDirectory(VDRVideoFrame::s_dataDir.c_str(),0);
			//CreateDirectory(VDRVideoFrame::s_removalDir.c_str(),0);

            std::cout<<"CreateDirectory successfully!"<<std::endl;


		   //�õ���ǰ�Ĺ���Ŀ¼

			m_sWorkDir=FileNameParser::findFileNameDir(actsFilePath);
           
		   //std::cout<<"current work directory: "<<m_sWorkDir<<std::endl;


			//����boundingBox
			//VDRStructureMotion::GetInstance()=sfm
			//�˴���ӦΨһ�Ĺ���Ŀ¼actsFilePath�����Բ�����Ҫ���Ǹ���Ķ���
			//VDRStructureMotion::GetInstance()->CalculateBox(0,m_iFrameCount-1);     //����caculateBox


		  }


		    /************************************************************************/
		  /* Intrinsic Parameter*/
		  /************************************************************************/
		  
		   //��Ҫ��ÿ�����������֡��Ϣ����������Ҫ��18̨���ͬһ֡����Ϣ�����ݽṹӦ�������֯��
           //�ؼ���������lightstage��18��������ڲθ�����ͬ����  
		   //��֮ǰ������ǵ���������ڲι̶���
		   //����lightstage�и����û��scale��
		   // ��Ҫ��lightstage����
	
//�����ڴ���Ҫ����ϵͳ�������������������visualhull,��Depth��Mesh�Ĳ���


		 // LS_DepthParaWidget::GetInstance()->UpdateDspMax(true);
		 // LS_DepthParaWidget::GetInstance()->Publish();
         
		  
		
	  
		  std::cout<<"loadCircleFrame over!"<<std::endl;


		  return true;

}


/*
bool VDRStructureMotion::CalculateBox( int startFrame, int endFrame ){

	return CBoxCalculator::GetInstance()->CalculateBox(this, startFrame, endFrame);
}


void VDRStructureMotion::CalculateVisualHull(int startFrame, int endFrame)
{
	std::cout<<"Calculating Visual Hull..."<<std::endl;
	int detaileLevels, imgApron;
	detaileLevels=9;              //����Ϊ��ʱ���ã���Ҫ�޸�
	imgApron=30;      


	std::cout<<"m_BoxMinXYZ:  "<<m_BoxMinXYZ[0]<<","<<m_BoxMinXYZ[1]<<","<<m_BoxMinXYZ[2]<<std::endl;
	std::cout<<"m_BoxMinXYZ:  "<<m_BoxMaxXYZ[0]<<","<<m_BoxMaxXYZ[1]<<","<<m_BoxMaxXYZ[2]<<std::endl;


	m_Visualhull.Init(m_BoxMinXYZ,m_BoxMaxXYZ,detaileLevels,imgApron);

	m_Visualhull.CalVisualHull(this,startFrame,endFrame);
}

void VDRStructureMotion::LS_RunInit(int startFrame,int endFrame)
{
     std::cout<<"on run lightstage RunInit"<<std::endl;
}


void VDRStructureMotion::LS_RunRefine(int startFrame,int endFrame)
{
	std::cout<<"on run lightstage RunRefine"<<std::endl;
}


void VDRStructureMotion::LS_RunMesh(int startFrame,int endFrame)
{
	std::cout<<"on run lightstage RunMesh"<<std::endl;
}

*/
/***************************************************************************/
/***************************************************************************/