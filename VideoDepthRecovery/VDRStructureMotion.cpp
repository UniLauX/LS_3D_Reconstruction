#include "VDRStructureMotion.h"


VDRStructureMotion::VDRStructureMotion(void)
{

}


VDRStructureMotion::~VDRStructureMotion(void)
{
}

// 定义: 加载视频序列
bool VDRStructureMotion::LoadFrames(const std::string &actsFilePath)  
{
	      //重置所有参数（包括释放内存）
          Reset();	

		  //判断文件类型（后缀是否为act、actb)
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
		  
		  //加载项目（act/actb)文件是否成功
		  if(m_pActsParser->LoadProject(actsFilePath)==false)
		  {
			  std::cout<<"LoadProject Failed!"<<std::endl;
			  return false;
		  }




         //设置参数并创建图像序列存储空间
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
			   pFrame->m_iID=k;                //帧号（Frame Number）
			   pFrame->m_sImagePathName=imageFilePath;  //每张图像的路径名称(增加帧号之前）
			   pFrame->m_sName=FileNameParser::findFileNameNameWitoutExt(imageFilePath);//得到无后缀的图像名称（test0nnn)
               imageFilePath=FileNameParser::increaseFileNameNumber(imageFilePath,step);//得到图像全路径（包括图像及后缀名）F://Data/test/test0nnn.jpg

			   /*  test
			   {
			   std::cout<<"m_iD: "<<pFrame->m_iID<<std::endl;
			   std::cout<<"m_sImageNamePath: "<<pFrame->m_sImageNamePath<<std::endl;
			   std::cout<<"m_sName: "<<pFrame->m_sName<<std::endl;
			   std::cout<<"imageFilePath："<<imageFilePath<<std::endl;
			   }
			   */

			   //获得第k帧外参矩阵R,T
			   Wml::Matrix4d &RT=m_pActsParser->m_vdExtrinsicParameters[k];
			   for(int row=0;row<3;row++)
			   {
				   for(int col=0;col<4;col++)
				   {
					   double extriVal=RT(row,col);
					   if(col!=3)
						    pFrame->SetCameraParameterRAt(row,col,extriVal);   //给R赋值
					   else
						   pFrame->SetCameraParameterTAt(row,extriVal);        //给T赋值
				   }
				    
			   }
		  }

		  /******************************************************************
		          Size &  Folder
		  *******************************************************************/
		  if(count>0)
		  {
			 CxImage templeImage;
			 //判断图像文件是否能正确加载
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
		  
		  //对（0,1）位置内参不是很熟悉
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
		// std::cout<<"加载M_K:"<<m_K(0,0)<<":"<<m_K(1,1)<<std::endl;

		 std::cout<<"Load Frames Successfully!"<<std::endl;

		 /*
               DepthParaWidget 尚未写
		 */
		 //设置放缩后的内参矩阵元素值
		 SetScaleVal(DepthParaWidget::GetInstance()->m_dResampleScale);


		 m_vpMatchLinkers=std::move(m_pActsParser->m_vpMatchLinkers);


		 //bug!!!!!
	 // std::cout<<"mvvvvvpSize: "<<m_vpMatchLinkers.size()<<std::endl;

		  DepthParaWidget::GetInstance()->UpdateDspMax(true);
		  DepthParaWidget::GetInstance()->Publish();
		  return true;

}

//定义： 重置参数
void VDRStructureMotion::Reset()
{
	  m_iFrameCount=0;
	  mFrames.clear();
	  m_dAutoMaxDsp=-1;   //是否自动设置Disparity的最大值（与参数调用相关）
	  m_K.MakeZero();    //内参矩阵全部置0
	  m_ScaledK.MakeZero();  //放缩图像（Scaled) 内参矩阵全部置0
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
		make_shared()函数可以接受最多10个参数，然后把它们传递给类型T的构造函数，
		创建一个shared_ptr<T>的对 象并返回。make_shared()函数要比直接创建shared_ptr对象的方式快且高效，
		因为它内部仅分配一次内存，消除了shared_ptr 构造时的开销。
		*/
	}
}

//设置放缩后的内参矩阵元素值
void VDRStructureMotion::SetScaleVal(double dScale)
{
	 if(VDRVideoFrame::s_dScale!=dScale)
		 VDRVideoFrame::s_dScale=dScale;

	 m_ScaledK.MakeZero();
	 m_ScaledK(0,0)=m_K(0,0)*dScale;   m_ScaledK(0,1)=m_K(0,1)*dScale;      m_ScaledK(0,2)=m_K(0,2)*dScale;
	 m_ScaledK(1,1)=m_K(1,1)*dScale;   m_ScaledK(1,2)=m_K(1,2)*dScale;
	 m_ScaledK(2,2)=1;
}

//得到Dispartiy 的范围，需要仔细查看
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
			if(pTrack->m_iErrorLevel==-1||pTrack->m_iErrorLevel==3)   //对跟踪文件中此函数不是很理解（-1,3好像代表解得是否正确）
				continue;      
			Wml::Vector3d pt3d=pTrack->m_v3D;      //存储所解得三维点
			for(auto j=0;j<pTrack->GetCount();++j)
			{
				TrackPoint &trackpt = pTrack->Point(j);
				if(trackpt.m_iFrameNo >= m_iFrameCount)
					continue;
				
				VDRVideoFrame  *pFrame=GetFrameAt(trackpt.m_iFrameNo);

				double depthVal=pFrame->GetDepthFromWorldCoord(pt3d);   
					if(depthVal > 1e-10)
					{                              //不清楚
						//minDepth = min(minDepth, depthVal);
						depthList.push_back(depthVal);
						bValid = true;
					}				

			}

		}
		if(bValid)
		{                                                              //不清楚含义
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

	/***     读取cam文件列表    ***/
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

			if( strlen(inLine)>5 &&inLine[0]!=' ')  //程序中有点小bug,故加inline[0]!=' ’进行判断多读的一个空行字符串
			{
				camFileNameList.push_back( std::string( inLine ) );   //list of cam file
			}     
		}
	}
	inStream.close();


	m_sWorkDir=FileNameParser::findFileNameDir(cameraListPath);
	std::cout<<"m_sWorkDir: "<<m_sWorkDir<<std::endl;

	camNum=camFileNameList.size();     //相机数     
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
		std::cout<<"pathDir: "<<pathDir<<std::endl;     //读到07cam的时候在pathName和PathDir输出之间产生三个空行，不清楚原因~（不影响程序性能，只是输出不美观）
		
       //VDRStructureMotion::GetInstance()->LoadLightstageFrames(pathName);  //LoadFrames 中的格式不适用于camFileParser中的内容


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





//定义：lightstage中加载视频序列  (针对的是单个文件夹，其作用相当于加载单个文件夹中的工程）
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


	  //加载cam文件，并将相关信息赋值给m_pActsParser
	  if(m_pActsParser->LoadProject(actsFilePath)==false)
	  {
		  std::cout<<"LoadProject Failed!"<<std::endl;
		  return false;
	  }
	 
	  //设置参数并创建图像序列存储空间
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
			   pFrame->m_iID=k;                //帧号（Frame Number）
			   pFrame->m_sImagePathName=imageFilePath;  //每张图像的路径名称(增加帧号之前）
			   pFrame->m_sName=FileNameParser::findFileNameNameWitoutExt(imageFilePath);//得到无后缀的图像名称（test0nnn)
               imageFilePath=FileNameParser::increaseFileNameNumber(imageFilePath,step);//得到图像全路径（包括图像及后缀名）F://Data/test/test0nnn.jpg
			  
			   pFrame->m_sMaskImgPathName=VDRVideoFrame::s_maskDir+pFrame->m_sName+std::string(".bmp");

			  // std::cout<<"pFrame->m_sMaskImgPathName: "<<pFrame->m_sMaskImgPathName<<std::endl;


			   //pFrame->m_sImagePathName正常帧号，imageFilePath大一个帧号 
			   
			   /*test
			   {
			   std::cout<<"m_iD: "<<pFrame->m_iID<<std::endl;
			   std::cout<<"m_sImageNamePath: "<<pFrame->m_sImagePathName<<std::endl;   
			   std::cout<<"m_sName: "<<pFrame->m_sName<<std::endl;
			  // std::cout<<"imageFilePath："<<imageFilePath<<std::endl;
			   }
			   */
                
			   //注：外参矩阵的最后一行尚未用到！！！

			   //获得第k帧外参矩阵R,T
			   Wml::Matrix4d &RT=m_pActsParser->m_vdExtrinsicParameters[k];
			   for(int row=0;row<3;row++)
			   {
				   for(int col=0;col<4;col++)
				   {
					   double extriVal=RT(row,col);
					   if(col!=3)
						    pFrame->SetCameraParameterRAt(row,col,extriVal);   //给R赋值
					   else
						   pFrame->SetCameraParameterTAt(row,extriVal);        //给T赋值
				   }
				    
			   }

             //获得第K帧内参矩阵K
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
			 //判断图像文件是否能正确加载
			 if(!templeImage.Load(GetFrameAt(0)->m_sImagePathName.c_str(),CxImageFunction::GetImageFileType(imageFilePath)))
			 {
				 std::cout<<"can't correctly load the image file: "<<imageFilePath<<std::endl;
				 return false;
			 }
		    VDRVideoFrame::InitImageSzie(templeImage.GetWidth(),templeImage.GetHeight());
			 
			VDRVideoFrame::s_tempDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("TMP/");
            VDRVideoFrame::s_dataDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("DATA/");
			//VDRVideoFrame::s_removalDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("REMOVAL/");

			VDRVideoFrame::s_maskDir=FileNameParser::findFileNameDir(actsFilePath)+std::string("MASK/");   //只需读取，不用创建

			CreateDirectory(VDRVideoFrame::s_tempDir.c_str(),0);
			CreateDirectory(VDRVideoFrame::s_dataDir.c_str(),0);
			//CreateDirectory(VDRVideoFrame::s_removalDir.c_str(),0);

            std::cout<<"CreateDirectory successfully!"<<std::endl;


		   //得到当前的工作目录

			m_sWorkDir=FileNameParser::findFileNameDir(actsFilePath);
           
		   //std::cout<<"current work directory: "<<m_sWorkDir<<std::endl;


			//计算boundingBox
			//VDRStructureMotion::GetInstance()=sfm
			//此处对应唯一的工作目录actsFilePath，所以并不需要考虑更多的东西
			//VDRStructureMotion::GetInstance()->CalculateBox(0,m_iFrameCount-1);     //调用caculateBox


		  }


		    /************************************************************************/
		  /* Intrinsic Parameter*/
		  /************************************************************************/
		  
		   //既要将每个相机的连续帧信息用起来，又要用18台相机同一帧的信息。数据结构应该如何组织？
           //关键问题在于lightstage中18个相机的内参各不相同？？  
		   //而之前求深度是单个相机，内参固定。
		   //而且lightstage中各相机没有scale。
		   // 需要看lightstage代码
	
//并且在此需要设置系统中所需参数，包括计算visualhull,求Depth和Mesh的参数


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
	detaileLevels=9;              //参数为临时设置，需要修改
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