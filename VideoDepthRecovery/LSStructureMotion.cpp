#include "LSStructureMotion.h"
#include "VisualHull/BoxCalculator.h"
#include "ImagePartition/LSImgPartition.h"
#include "DataCost.h"
#include "LSDepthEstimator.h"
#include "PointCloudSampling.h"
#include "PossionShExec.h"

LSStructureMotion::LSStructureMotion(void)
{
	m_sSystemDir="";
	m_sPossionPath="";
	m_iCamNum=-1;
    //m_pCamFiles=nullptr;
}

LSStructureMotion::~LSStructureMotion(void)
{
	//m_pCamFiles.shrink_to_fit();   //release the memory of the vector
	m_iCamNum=0;
	delete[]camFiles;
	camFiles=nullptr;
	for(int k=0; k<m_iFramCount; ++k)   //release the memory of the vector allocated
		delete m_frames[k];

	m_BoxMinXYZ.clear();
	m_BoxMaxXYZ.clear();
	m_fDspMin.clear();
	m_fDspMax.clear();
	m_VisualHull.clear();

	m_ATVisualHull.clear();
	m_LgrBoxMinXYZ.clear();
	m_LgrBoxMaxXYZ.clear();
}



//create frame for all cameras
void LSStructureMotion::LS_CreateFrames(int iCount)
{
	for(int k=0; k<m_frames.size(); ++k)
		delete m_frames[k];

	m_iFramCount = iCount;
	//int	totalFramCount=m_iFramCount*CamNum;
	int	totalFramCount=m_iFramCount*m_iCamNum;
	m_frames.resize(totalFramCount);

	for(int k=0; k<totalFramCount; ++k)
	{
		m_frames[k] = new LSVideoFrame;
	}
}

// init maskWidth and maskHeight according all mask images
void LSStructureMotion::InitMaxMaskRange(int start, int end){
	m_iMaxMaskWidth = -1; 
	m_iMaxMaskHeight = -1;
	int minX, minY, maxX, maxY;
	for(int iFrame= start; iFrame <= end; iFrame++){
		for(int camId = 0; camId < CamNum; camId++){
			LSVideoFrame * pFrame = GetFrameAt(camId, iFrame);
			pFrame->GetMaskRectangle(minX, minY, maxX, maxY);
			//std::cout<<"minX: "<<minX<<" , mintY: "<<mintY<<" , maxX: "<<maxX<<" , MaxY: "<<maxY<<std::endl;	
			if(maxX - minX + 1 >= m_iMaxMaskWidth)
				m_iMaxMaskWidth = maxX - minX + 1;
			if(maxY - minY + 1 >= m_iMaxMaskHeight)
				m_iMaxMaskHeight = maxY - minY + 1;
		}
	}
}

//Load lightstage project
bool LSStructureMotion::LS_LoadProject(const std::string &cameraListPath)
{
	// get/set system parameters(directories)    
	m_sSystemDir=FileNameParser::findFileNameDir(cameraListPath); //system Dir
	m_sPossionPath=m_sSystemDir+std::string("PoissonRecon.exe"); //possion path
	m_sMeshDir=m_sSystemDir+std::string("Mesh/");              //mesh Dir
	m_sVisualHullDir=m_sSystemDir+std::string("VisualHull/");  //visualhull Dir

	CreateDirectory(m_sMeshDir.c_str(),0);               //create directory for mesh file
	CreateDirectory(m_sVisualHullDir.c_str(),0);         //create directory for visualhull file

	std::cout<<"m_sSystemDir: "<<m_sSystemDir<<std::endl;
	std::cout<<"m_sPossionPath:"<<m_sPossionPath<<std::endl;
	std::cout<<"m_sMeshDir:"<<m_sMeshDir<<std::endl;
	std::cout<<"m_sVisualHullDir:"<<m_sVisualHullDir<<std::endl;

	// read camera file name list 
	int camNumCount=0;
	std::vector< std::string > camFileNameList;
	const int maxLine = 1024;
	static  char inLine[maxLine + 10];
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
				m_sWorkDirs.push_back(FileNameParser::findFileNameDir(inLine));
				camNumCount++;
			}     
		}
	}
	inStream.close();

	//allocate camfiles space 
	m_iCamNum=camNumCount;     // the number of cameras  
	std::cout<<m_iCamNum<<" cameras in total."<<std::endl;

	if(camFiles!=nullptr)
	{
		delete [] camFiles;
	    camFiles=nullptr;
	}
	 camFiles=new CamFileParser[m_iCamNum];      // 需要在用完之后显式释放内存


	//check camfile type & load camtype project      
	for(int camId=0;camId<m_iCamNum;camId++)
	{
		// check the camfile type  
		if(FileNameParser::findFileNameExt(camFileNameList[camId])!="cam")
		{
			std::cout<<"the input file isn't camera file"<<std::endl;
			return  false;
		}

		//  Load cam type project 
		bool succ=camFiles[camId].LoadProject(camFileNameList[camId]); 
		if(succ==false)
		{
			std::cout<<"Load " <<camId<<" Project Failed!"<<std::endl;
            return  false;
		}
	}
	std::cout<<"Load camfile Project Successful!"<<std::endl;

	//get cam file Parameters &  Directory &  Create frames      
	int startFrame=0,endFrame=0,step=1;  
	int count=0;                      //default parameters
	//startFrame=camFiles[0].m_iStartFrame;
	//step=camFiles[0].m_iFrameStep;
	//endFrame=camFiles[0].m_iEndFrame;  //real parameters
	//count=camFiles[0].m_iFrameCount;

	startFrame=40;
	step=1;
	endFrame=40;  //real parameters
	count=1;

	std::cout<<"start: "<<startFrame<<std::endl;
	std::cout<<"step: "<<step<<std::endl;
	std::cout<<"end: "<<endFrame<<std::endl;
	std::cout<<"Frame count: "<<count<<std::endl;


	// create child directories for different cameras
    if(count>0)
	{
		for(int camId=0;camId<m_iCamNum;camId++)
		{	
		// set TmpDir for different camId (tmp depth for visualization)
		 std::string dir=FileNameParser::findFileNameDir(camFiles[camId].m_sImagePath)+std::string("Tmp\\");
		 LSVideoFrame::SetTmpDirectory(camId, dir);
		 CreateDirectory(dir.c_str(), 0);
		
		 //set DataDir for different camId (depth file with extension of ".raw")
		 dir=FileNameParser::findFileNameDir(camFiles[camId].m_sImagePath)+std::string("Data\\");
		 LSVideoFrame::SetDataDirectory(camId, dir);
		 CreateDirectory(dir.c_str(), 0);
		 
		 //set RawDir for pointCloudSampling    //not used yet (5/25/2015) 
		 dir=FileNameParser::findFileNameDir(camFiles[camId].m_sImagePath)+std::string("Raw\\");
		 LSVideoFrame::SetRawDirectory(camId, dir);
		 //CreateDirectory(dir.c_str(), 0);

		 // Set SegDir for mean shift segment  //not used yet (5/25/2015)
		 dir=FileNameParser::findFileNameDir(camFiles[camId].m_sImagePath)+std::string("Seg\\");
		 LSVideoFrame::SetSegDirectory(camId, dir);
		 //CreateDirectory(dir.c_str(), 0);

		 // new add-for optical flow  //not used yet (5/25/2015)
		 dir=FileNameParser::findFileNameDir(camFiles[camId].m_sImagePath)+std::string("Opt\\");
		 LSVideoFrame::SetOptFlwDirectory(camId, dir);
		// CreateDirectory(dir.c_str(),0);
		}
	}
	else
	{
		std::cout<< "ERROR: Frame count is not bigger than 0 !" << std::endl;
		return false;
	}

	//Create frames
	LS_CreateFrames(count);

	std::vector<LSVideoFrame*> pFrame;
	pFrame.resize(m_iFramCount*m_iCamNum);
	
	int imgWidth=0, imgHeight=0;

	for(int framId=0;framId<count;framId++)
	{
		// get color image and mask image path && increase image names by number	
		for(int camId=0;camId<m_iCamNum;camId++)
		{

			pFrame[camId+framId*m_iCamNum]=GetFrameAt(camId,framId);     //  get ith frame at camId
			pFrame[camId+framId*m_iCamNum]->m_iCameraId=camId;
			pFrame[camId+framId*m_iCamNum]->m_iFrameId=framId;

			pFrame[camId+framId*m_iCamNum]->m_sColorImgPathName=FileNameParser::increaseFileNameNumber(camFiles[camId].m_sImagePath,step*framId);
			pFrame[camId+framId*m_iCamNum]->m_sName=FileNameParser::findFileNameNameWitoutExt(pFrame[camId+framId*m_iCamNum]->m_sColorImgPathName);
			pFrame[camId+framId*m_iCamNum]->m_sMaskImgPathName=FileNameParser::findFileNameDir(pFrame[camId+framId*m_iCamNum]->m_sColorImgPathName)+
				std::string("Mask\\")+pFrame[camId+framId*m_iCamNum]->m_sName+"."+FileNameParser::findFileNameExt(pFrame[camId+framId*m_iCamNum]->m_sColorImgPathName);  //mask图像后缀与colorImage一致
		
			//set image width and image height
			if(framId== 0 && camId == 0)
			{
				CxImage tmpImg;
				if(!tmpImg.Load(pFrame[camId+framId*m_iCamNum]->m_sColorImgPathName.c_str(), CxImageFunction::GetImageFileType(pFrame[camId+framId*m_iCamNum]->m_sColorImgPathName)))
				{
					std::cout<<"ERROR: Can not load image file: " << pFrame[camId+framId*m_iCamNum]->m_sColorImgPathName <<std::endl;
					return false;
				}
				imgWidth = tmpImg.GetWidth();
				imgHeight = tmpImg.GetHeight();
			}
			pFrame[camId+framId*m_iCamNum]->SetImgSize(imgWidth,imgHeight);
	      	
		}

		// assign value to extrinsic parameters
		std::vector<Wml::Matrix4d> RT;
		RT.resize(m_iFramCount*m_iCamNum);

		for(int camId=0;camId<m_iCamNum;camId++)
		{  
			//std::cout<<"Frame: "<<framId<<",Camera: "<<camId<<" Extrinsic Parameters:"<<std::endl;
			RT[camId+framId*m_iCamNum].MakeZero();
			RT[camId+framId*m_iCamNum]=camFiles[camId].m_dLsExtrinsicParameter;
			for(int row=0;row<3;row++)
			{
				for(int col=0;col<4;col++)
				{
					double extriVal=RT[camId+framId*m_iCamNum](row,col);
					if(col!=3)
						pFrame[camId+framId*m_iCamNum]->SetCameraParaRAt(row,col,extriVal);   //assign value to R
					else
						pFrame[camId+framId*m_iCamNum]->SetCameraParaTAt(row,extriVal);        //assign value to T

					//std::cout<<extriVal<<" ";
				}
				//std::cout<<std::endl;
			}
			pFrame[camId+framId*m_iCamNum]->InitCameraPos();   //Init the camera center position
		}

	// assign value to Intrinsic parameters
	std::vector<Wml::Matrix3d>K;
	K.resize(m_iFramCount*m_iCamNum);
	for(int camId=0;camId<m_iCamNum;camId++)
	{
		K[camId+framId*m_iCamNum].MakeZero();	
		K[camId+framId*m_iCamNum]=camFiles[camId].m_dLsIntrinsicParameter; 

	    // std::cout<<"frame: "<<framId<<",camera: "<<camId<<" intrinsic parameters:"<<std::endl;
		//for(int row=0;row<3;row++)
		//{
		//	for(int col=0;col<3;col++)
		//	{
		//		std::cout<<K[camId+framId*m_iCamNum](row,col)<<" ";
		//	}
		//	std::cout<<std::endl;
		//}
		pFrame[camId+framId*m_iCamNum]->SetCameraParaK(K[camId+framId*m_iCamNum]);     //set intrinsic parameters
	 }
	 
	 RT.clear();  
	 K.clear();
	}

     LSParaWidget::GetInstance()->SetFrameCount(m_iFramCount);    //frame count
	 LSParaWidget::GetInstance()->SetCameraNumber(m_iCamNum);     //camera number
     LSParaWidget::GetInstance()->Inialize();     //init system parameters  
	 std::cout<<"Load lightstage project over."<<std::endl;
	
     LSStructureMotion::GetInstance()->CalculateBox(0,GetFrameCount()-1);   //calculate boundingBox
     // LSStructureMotion::GetInstance()->CalculateEnlargeBox(0,GetFrameCount()-1);  //calculate EnlargeBoundingBox: for seedball based method
     return true;
}


// Calculate Bounding Box
bool  LSStructureMotion::CalculateBox(int startFrame, int endFrame)
{	
	return CBoxCalculator::GetInstance()->CalculateBox(this,startFrame,endFrame);// (0, frameCount-1)
}


bool  LSStructureMotion::CalculateEnlargeBox(int startFrame, int endFrame)
{	
	return CBoxCalculator::GetInstance()->CalculateEnlargeBox(this,startFrame,endFrame);
}

// Calculate Visual hull
void LSStructureMotion::CalculateVisualHull(int startFrame, int endFrame )
{
	//allocated space for VisualHull vector(different frames)
	//endFrame=1;
	//m_iFramCount=2;
	m_VisualHull.resize(m_iFramCount);
	std::cout << "Calculating Visual Hull..."<<std::endl;
	std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;
	int detaileLevels, imgApron;
	LSParaWidget::GetInstance()->GetVisualPara(detaileLevels, imgApron);  //9, 30
	//与BoundingBox(将所有帧的计算数据和结构放到vector中，一次计算所有帧）不同，
	//VisualHull利用循环，每次计算一帧，将最终的结果存储在VisualHull 目录下。
     for(int iFrame=startFrame;iFrame<=endFrame;iFrame++)
	{
		std::cout<<"Current calculate "<< iFrame<<" th's  Visualhull: "<<std::endl;
		std::cout<<"m_BoxMinXYZ:  "<<m_BoxMinXYZ[iFrame].X()<<","<<m_BoxMinXYZ[iFrame].Y()<<","<<m_BoxMinXYZ[iFrame].Z()<<std::endl;
		std::cout<<"m_BoxMaxXYZ:  "<<m_BoxMaxXYZ[iFrame].X()<<","<<m_BoxMaxXYZ[iFrame].Y()<<","<<m_BoxMaxXYZ[iFrame].Z()<<std::endl;
		m_VisualHull[iFrame].Init(m_BoxMinXYZ[iFrame], m_BoxMaxXYZ[iFrame], detaileLevels, imgApron);
	    m_VisualHull[iFrame].CalVisualHull(this, iFrame, iFrame);
	    std::cout<<std::endl;
	}	
	//(final code by Ly on Mar 13,2014).
}

//新添加自动计算VisualHull过程（结合3D-GraphCut和2D-GrabCut)
void LSStructureMotion::AutoCalcVisualHull(int startFrame,int endFrame)
{
	std::cout << "Auto Calculating Visual Hull start..."<<std::endl;
	std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;

	//allocated space for VisualHull vector(different frames)
	m_ATVisualHull.resize(m_iFramCount);
	
	//parameters  
	int maxIntervalVoxel,imgApron;
	LSParaWidget::GetInstance()->GetAutoVisualPara(maxIntervalVoxel,imgApron); //100,30
    
	// Init Mask Range & set mask size(width and height),through all frames
	//目前由于32位内存限制等原因，导致无法做到在图像上的更细粒度（也就说将程序扩展到64位后，可以考虑成倍增加MaxIntervalVoxel大小)
	InitMaxMaskRange(startFrame, endFrame);
	LSImgPartition::GetInstance()->SetImgSize(m_iMaxMaskWidth, m_iMaxMaskHeight);   //423，630
	maxIntervalVoxel=std::max(m_iMaxMaskWidth,m_iMaxMaskHeight)/5;   //maxIntervalVoxel不能固定死，而需要根据图像的大小（实际是boundingBox的大小）来确定
	std::cout<<"maxIntervalVoxel: "<<maxIntervalVoxel<<std::endl;
	
	//与BoundingBox(将所有帧的计算数据和结构放到vector中，一次计算所有帧）不同，
	//VisualHull利用循环，每次计算一帧，将最终的结果存储在VisualHull 目录下。
	for(int iFrame=startFrame;iFrame<=endFrame;iFrame++)
	{
		std::cout<<"Current Auto Calculate "<< iFrame<<" th's  Visualhull: "<<std::endl;
		std::cout<<"m_BoxMinXYZ:  "<<m_BoxMinXYZ[iFrame].X()<<","<<m_BoxMinXYZ[iFrame].Y()<<","<<m_BoxMinXYZ[iFrame].Z()<<std::endl;
		std::cout<<"m_BoxMaxXYZ:  "<<m_BoxMaxXYZ[iFrame].X()<<","<<m_BoxMaxXYZ[iFrame].Y()<<","<<m_BoxMaxXYZ[iFrame].Z()<<std::endl;

		//std::cout<<"m_LgrBoxMinXYZ:  "<<m_LgrBoxMinXYZ[iFrame].X()<<","<<m_LgrBoxMinXYZ[iFrame].Y()<<","<<m_LgrBoxMinXYZ[iFrame].Z()<<std::endl;
	    //std::cout<<"m_LgrBoxMaxXYZ:  "<<m_LgrBoxMaxXYZ[iFrame].X()<<","<<m_LgrBoxMaxXYZ[iFrame].Y()<<","<<m_LgrBoxMaxXYZ[iFrame].Z()<<std::endl;
		
		//method1:background subtraction version
		 m_ATVisualHull[iFrame].InitGenerateVolume(m_BoxMinXYZ[iFrame],m_BoxMaxXYZ[iFrame],maxIntervalVoxel,imgApron);

		 m_ATVisualHull[iFrame].GenerateSeedVoxels(this,iFrame,iFrame);  //method1: through background subtraction
	 
		/*
		//method2: seed ball based version
		//m_ATVisualHull[iFrame].InitGenerateVolume(m_LgrBoxMinXYZ[iFrame],m_LgrBoxMaxXYZ[iFrame],maxIntervalVoxel,imgApron);
		//m_ATVisualHull[iFrame].GenerateSeedVoxelBall(this,iFrame,iFrame,m_BoxMinXYZ[iFrame],m_BoxMaxXYZ[iFrame]);
		//
		m_ATVisualHull[iFrame].TmpGenerateFunc();

	 
		////Method1：在三维上用平均颜色结合GMM和GrabCut计算（无迭代）
	 //  //需要将以下两个函数结合起来，执行3D和2D的GrabCut并不断相互迭代（迭代的终止条件是VisualHull收敛，即前景Voxel的数量不再增加）
		//
		////for(int iter=0;iter<2;iter++)
		////{
		////	m_ATVisualHull[iFrame].Execute2DGrabCut1();
		////	cv::Mat bgdModel,fgdModel;
		////	m_ATVisualHull[iFrame].Execute3DGraphCut1(bgdModel,fgdModel,1,false);
		////	m_ATVisualHull[iFrame].CastVisualHulltoMask();
		////}
		//
	
		//////Method2: paper method
	    for(int iter=0;iter<1;iter++)
		{
			m_ATVisualHull[iFrame].Execute2DGrabCut();
		    m_ATVisualHull[iFrame].Execute3DGraphCut(1,false);    //迭代过程还没有很好的接起来，估计有点问题
		   //m_ATVisualHull[iFrame].CastVisualHulltoMask();  //not used 
		}
	    */
	}	
}

 // Depth recovery - Run init  //
// NEW ADD-Change for VisualHull Constraint
void LSStructureMotion::RunInitialization(int startFrame, int endFrame)
{
	std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;
	// Init Mask Range & set mask size(width and height),through all frames
	InitMaxMaskRange(startFrame, endFrame);
    LSImgPartition::GetInstance()->SetImgSize(m_iMaxMaskWidth, m_iMaxMaskHeight);   //423，630
    std::cout<<"m_iMaxMaskWidth: "<<m_iMaxMaskWidth<<" , "<<"m_iMaxMaskHeight: "<<m_iMaxMaskHeight<<std::endl;
	// block in whole image as (2,1）and get block size( width and height) 
	int maxBlockWidth;
	int maxBlockHeight;
	LSImgPartition::GetInstance()->SetBlocksCount(LSParaWidget::GetInstance()->m_iBlockCountX,LSParaWidget::GetInstance()->m_iBlockCountY); //2,1
	LSImgPartition::GetInstance()->GetMaxBlockSize(maxBlockWidth, maxBlockHeight);   //要弄清楚具体分块和坐标，需要查看此函数
     std::cout<<"maxBlockWidth: "<<maxBlockWidth<<" , "<<"maxBlockHeight: "<<maxBlockHeight<<std::endl;
	// disparity level 
	int iDspLevel = LSParaWidget::GetInstance()->GetDspLevel();   // 101
	 std::cout<<"dspLevel:"<<iDspLevel<<std::endl;

	//Init DataCost (allocate memory and set all the elem in zero)
//	DataCost dataCost(maxBlockWidth,maxBlockHeight,iDspLevel);  //Old
	for(int iFrame = startFrame; iFrame <= endFrame; iFrame++)
	{
		for(int camId = 0; camId < CamNum; camId++)
		{
		   
			DataCost dataCost; //Instead  放到里面主要是为及时释放内存
	  std::cout<<"Estimate ith frame:"<<iFrame<<"/"<< endFrame << " ith camera:"<<camId <<"/"<< CamNum-1 << std::endl;  //print
			// get current and reference frames 
			/// all the M cams form a circle to make sure that the forward and backward references
			/// be equal to 2( 2 can be adjusted according the practical situation)
			LSVideoFrame * pFrame = GetFrameAt(camId, iFrame);
			std::vector<std::pair<int, int> > FwFramesIndex;
			std::vector<std::pair<int, int> > BwFramesIndex;
		
			LSParaWidget::GetInstance()->GetReferenceFrameIndex(FwFramesIndex, BwFramesIndex, camId, iFrame, 0, CamNum-1);
			std::vector<LSVideoFrame *> FwFrames(FwFramesIndex.size());    //2
			std::vector<LSVideoFrame *> BwFrames(BwFramesIndex.size());
			for(int i=0; i<FwFramesIndex.size(); i++)
				FwFrames[i] = GetFrameAt(FwFramesIndex[i].first, FwFramesIndex[i].second);
			for(int i=0; i<BwFramesIndex.size(); i++)
				BwFrames[i] = GetFrameAt(BwFramesIndex[i].first, BwFramesIndex[i].second);
		

	  /*  //下面参数仅针对清华LiWen模型
			if(pFrame->m_pMaskImg == NULL)
				pFrame->LoadMaskImg();
			if(camId==0)
				pFrame->SetDspRange(0.033985,0.0528687);
			else if(camId==1)
				pFrame->SetDspRange(0.033036,0.0520719);
			else if(camId==2)
				pFrame->SetDspRange(0.0293875,0.0509773);
			else if(camId==3)
				pFrame->SetDspRange(0.0295431,0.0497313);
			else if(camId==4)
				pFrame->SetDspRange(0.0317659,0.0492307);
			else if(camId==5)
				pFrame->SetDspRange(0.0298284,0.0574242);
			else if(camId==6)
				pFrame->SetDspRange(0.0316513,0.0477797);
			else if(camId==7)
				pFrame->SetDspRange(0.0267716,0.0485651);
			else if(camId==8)
				pFrame->SetDspRange(0.0318402,0.0491032);
			else if(camId==9)
				pFrame->SetDspRange(0.0322652,0.0497569);
			else if(camId==10)
				pFrame->SetDspRange(0.0325853,0.0514124);
			else if(camId==11)
				pFrame->SetDspRange(0.0320028,0.053334);
			else if(camId==12)
				pFrame->SetDspRange(0.0337704,0.0553398);
			else if(camId==13)
				pFrame->SetDspRange(0.0343912,0.0565927);
			else if(camId==14)
				pFrame->SetDspRange(0.033261,0.0713723);
			else if(camId==15)
				pFrame->SetDspRange(0.0346033,0.067916);
			else if(camId==16)
				pFrame->SetDspRange(0.0350186,0.0618822);
			else if(camId==17)
				pFrame->SetDspRange(0.0349882,0.0572529);
			else if(camId==18)
				pFrame->SetDspRange(0.031134,0.0681231);
			else if(camId==19)
				pFrame->SetDspRange(0.0340484,0.0557552);

			pFrame->LoadMinLabelImg();
			pFrame->LoadMaxLabelImg();
	*/	
		// std::cout<<"minLabel: "<<pFrame->m_pMinLabelImg->at(359,42)<<" ,maxLabel: "<<pFrame->m_pMinLabelImg->at(359,42)<<std::endl;
		    float  curMinDsp;
	        float  curMaxDsp;	
			pFrame->GetDspRange(curMinDsp,curMaxDsp);
		    std::cout<<"mindsp:"<<curMinDsp<<",maxdsp"<<curMaxDsp<<"\n";   
          //该步骤之前KXL程序和LY程序所求disparity范围基本一致（故之前代码没有问题）

		   	//get the maxDepth,minDepth(m_fMaxDsp,m_fMinDsp)in this view through visual hull
			//init maxDspImage,minDspImage with m_fMaxDsp, m_fMinDsp
		    pFrame->InitDspConstrain(m_VisualHull[iFrame].m_TriangleList, m_VisualHull[iFrame].m_VertexList);
			float VisualHullConstrianSigma = LSParaWidget::GetInstance()->GetVisualHullConstrianSigma(); // 0.95
	    	
			//Init maxLabelImage and minLabelImage
			pFrame->InitLabelRangeByDspRange(LSParaWidget::GetInstance()->GetDspLevel() - 1, VisualHullConstrianSigma, true);//output temporary result
			//estimate depth (Get DataTerm and find the inital best label)

			ZFloatImage *varianceMap=new ZFloatImage();   //不能建局部变量，否则在后面释放m_pVarianceMap时会报错
			varianceMap->CreateAndInit(pFrame->GetImgWidth(),pFrame->GetImgHeight(),1,0);
			pFrame->m_pVarianceMap=varianceMap;
		    pFrame->LoadColorImg(); // new add (意味着后面可能多一个类似操作）
		    pFrame->GenerateVariance(*(pFrame->m_pColorImg),*varianceMap,3);
			LSDepthEstimator::GetInstance()->EstimateDepth(FwFrames, BwFrames, pFrame, dataCost);

		}
	}
   //CO std::cout<<"Init Depth over!"<<std::endl;
}

/*
 //2014/12/09
 //ORG Depth recovery - Run init  //
void LSStructureMotion::RunInitialization(int startFrame, int endFrame)
{
	std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;
	// Init Mask Range & set mask size(width and height)
	InitMaxMaskRange(startFrame, endFrame);
    LSImgPartition::GetInstance()->SetImgSize(m_iMaxMaskWidth, m_iMaxMaskHeight);   //423，630
    std::cout<<"m_iMaxMaskWidth: "<<m_iMaxMaskWidth<<" , "<<"m_iMaxMaskHeight: "<<m_iMaxMaskHeight<<std::endl;

	// block in whole image as (2,1）and get block size( width and height) 
	int maxBlockWidth;
	int maxBlockHeight;
	LSImgPartition::GetInstance()->SetBlocksCount(LSParaWidget::GetInstance()->m_iBlockCountX,LSParaWidget::GetInstance()->m_iBlockCountY); //2,1
	LSImgPartition::GetInstance()->GetMaxBlockSize(maxBlockWidth, maxBlockHeight);
	std::cout<<"maxBlockWidth: "<<maxBlockWidth<<" , "<<"maxBlockHeight: "<<maxBlockHeight<<std::endl;

	
	// disparity level 
	int iDspLevel = LSParaWidget::GetInstance()->GetDspLevel();   // 101
	std::cout<<"dspLevel:"<<iDspLevel<<std::endl;

	//Init DataCost (allocate memory and set all the elem in zero)
	DataCost dataCost(maxBlockWidth,maxBlockHeight,iDspLevel);


	for(int iFrame = startFrame; iFrame <= endFrame; iFrame++){
		for(int camId = 0; camId < CamNum; camId++){

			std::cout<<"Estimate ith frame:"<<iFrame<<"/"<< endFrame << " ith camera:"<<camId <<"/"<< CamNum-1 << std::endl;  //print

			// get current and reference frames 
			/// all the M cams form a circle to make sure that the forward and backward references
			/// be equal to 5( 5 can be adjusted according the practical situation)
			LSVideoFrame * pFrame = GetFrameAt(camId, iFrame);
			std::vector<std::pair<int, int> > FwFramesIndex;
			std::vector<std::pair<int, int> > BwFramesIndex;
			LSParaWidget::GetInstance()->GetReferenceFrameIndex(FwFramesIndex, BwFramesIndex, camId, iFrame, 0, CamNum-1);

			std::vector<LSVideoFrame *> FwFrames(FwFramesIndex.size());    //5
			std::vector<LSVideoFrame *> BwFrames(BwFramesIndex.size());
			for(int i=0; i<FwFramesIndex.size(); i++)
				FwFrames[i] = GetFrameAt(FwFramesIndex[i].first, FwFramesIndex[i].second);
			for(int i=0; i<BwFramesIndex.size(); i++)
				BwFrames[i] = GetFrameAt(BwFramesIndex[i].first, BwFramesIndex[i].second);

			
		   	//get the maxDepth,minDepth(m_fMaxDsp,m_fMinDsp)in this view through visual hull
			//init maxDspImage,minDspImage with m_fMaxDsp, m_fMinDsp
			pFrame->InitDspConstrain(m_VisualHull[iFrame].m_TriangleList, m_VisualHull[iFrame].m_VertexList);
			
		
			float VisualHullConstrianSigma = LSParaWidget::GetInstance()->GetVisualHullConstrianSigma(); // 0.95

			//Init maxLabelImage and minLabelImage
			pFrame->InitLabelRangeByDspRange(LSParaWidget::GetInstance()->GetDspLevel() - 1, VisualHullConstrianSigma, true);//output temporary result
		

			//estimate depth (Get DataTerm and find the inital best label)
			LSDepthEstimator::GetInstance()->EstimateDepth(FwFrames, BwFrames, pFrame, dataCost);
            
   //        // Get the remain memory of the system. 
			//MEMORYSTATUS memstatus;
			//memset(&memstatus,0,sizeof(MEMORYSTATUS));
			//memstatus.dwLength =sizeof(MEMORYSTATUS);
			//GlobalMemoryStatus(&memstatus);
			//DWORD mem=memstatus.dwAvailPhys ;
		 //   std::cout << "Free Memory is:" << mem/(1024*1024) << " MB!"<<std::endl;

		}
	}
}
*/
// run refine for depth recovery
void LSStructureMotion::RunRefine(int startFrame, int endFrame)
{ 
   std::cout<<"LSStructuremotion  RunRefine "<<std::endl;
   std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;

   // mask size( width and height)
   InitMaxMaskRange(startFrame, endFrame);
   LSImgPartition::GetInstance()->SetImgSize(m_iMaxMaskWidth, m_iMaxMaskHeight);   //423，630
   std::cout<<"m_iMaxMaskWidth: "<<m_iMaxMaskWidth<<" , "<<"m_iMaxMaskHeight: "<<m_iMaxMaskHeight<<std::endl;

   // block size(width and height)
   int maxBlockWidth;
   int maxBlockHeight;
   LSImgPartition::GetInstance()->SetBlocksCount(LSParaWidget::GetInstance()->m_iBlockCountX,LSParaWidget::GetInstance()->m_iBlockCountY); //2,1
   LSImgPartition::GetInstance()->GetMaxBlockSize(maxBlockWidth, maxBlockHeight);
   std::cout<<"maxBlockWidth: "<<maxBlockWidth<<" , "<<"maxBlockHeight: "<<maxBlockHeight<<std::endl;


   //set dspLevel and dataCost
   int iDspLevel = LSParaWidget::GetInstance()->GetDspLevel();  //101
   std::cout<<"dspLevel:"<<iDspLevel<<std::endl;
   
   // DataCost dataCost(maxBlockWidth, maxBlockHeight, iDspLevel);  //Old

   	int passCount = LSParaWidget::GetInstance()->GetRefinePassCount(); //2
	std::cout<<"passCount: "<<passCount<<std::endl;

	for(int passi = 0; passi < passCount; passi++)
	{
		for(int iFrame = startFrame; iFrame <=endFrame ; iFrame++)
		{  
			for(int camId = 0; camId < CamNum; camId++)
			{
				    std::cout << "BO ith frame:"<<iFrame<<"/" << endFrame << " ith camera: " << camId<<"/" << CamNum-1 << std::endl;
				    DataCost dataCost; //Instead  放到里面主要是为及时释放内存

				    // get current and reference frames (all camId form a circle to ensure the number of the forward equal to backward)
					LSVideoFrame * pFrame = GetFrameAt(camId, iFrame);
					std::vector<std::pair<int, int> > FwFramesIndex;
					std::vector<std::pair<int, int> > BwFramesIndex;
					LSParaWidget::GetInstance()->GetReferenceFrameIndex(FwFramesIndex, BwFramesIndex, camId, iFrame, 0, CamNum-1);
					std::vector<LSVideoFrame *> FwFrames(FwFramesIndex.size());
					std::vector<LSVideoFrame *> BwFrames(BwFramesIndex.size());
					for(int i=0; i<FwFramesIndex.size(); i++)
					FwFrames[i] = GetFrameAt(FwFramesIndex[i].first, FwFramesIndex[i].second);
					for(int i=0; i<BwFramesIndex.size(); i++)
					BwFrames[i] = GetFrameAt(BwFramesIndex[i].first, BwFramesIndex[i].second);
				
                    float mindsp,maxdsp;
					pFrame->GetDspRange(mindsp,maxdsp);
					std::cout<<"_mindsp: "<<mindsp<<",_maxdsp: "<<maxdsp<<std::endl;

                  /*
				    //Difference: 孔相澧代码BO中并没有再用VisuallHull再做约束，也未用到minDspImg 和maxDspImg, 只用到之前得到的m_fMaxDsp,m_fMinDsp
					//所以以下三行代码在修正过程应该可以去掉。
           
					//get the maxDepth,minDepth(m_fMaxDsp,m_fMinDsp)in this view through visual hull
					//init maxDspImage,minDspImage with m_fMaxDsp, m_fMinDsp
					pFrame->InitDspConstrain(m_VisualHull[iFrame].m_TriangleList, m_VisualHull[iFrame].m_VertexList);

				    float VisualHullConstrianSigma = LSParaWidget::GetInstance()->GetVisualHullConstrianSigma(); //(here assign 0.95)
					
					//另外，此函数可能未对Mask边缘部分处理，导致重建的三维模型会变细
				    //pFrame->InitLabelRangeByDspRange(LSParaWidget::GetInstance()->GetDspLevel() - 1, 
					//	(VisualHullConstrianSigma - 0.1 >= 0 ? VisualHullConstrianSigma - 0.1: 0), true);  //原来为0.1   
				  	pFrame->InitLabelRangeByDspRange(LSParaWidget::GetInstance()->GetDspLevel() - 1,VisualHullConstrianSigma,true);
			     */
					LSDepthEstimator::GetInstance()->BundleOptimization(FwFrames, BwFrames, pFrame, dataCost);
			}
		}
	}
	
}


//new add-optical flow information
 void LSStructureMotion::RunOptRefine(int startFrame,int endFrame)
 {
/*
    // test
	 LSVideoFrame * pPreFrame=GetFrameAt(0,0);

	 LSVideoFrame *pNextFrame=GetFrameAt(0,1);

	 pPreFrame->LoadForeOptFlwImg();
	 pNextFrame->LoadBackOptFlwImg();
	 pPreFrame->LoadMaskImg();
	 pNextFrame->LoadMaskImg();

	 int iWidth=pNextFrame->GetImgWidth();
	 int iHeight=pNextFrame->GetImgHeight();

	 for(int h=0;h<iHeight;h++)
	 {
		 for(int w=0;w<iWidth;w++)
		 {
            int xSmaller,ySmaller; 
			int xBigger,yBigger;  
			float xRatio,yRatio;

			 int x1=w, y1=h;
			 int x2,y2;
			 float u1=pPreFrame->m_pForeOptImg->at(x1,y1,0);
			 float v1=pPreFrame->m_pForeOptImg->at(x1,y1,1);


          //method1: 双线性插值法

			 float fx2=x1+u1;
			 float fy2=y1+v1;

			 xSmaller=floor(fx2);  //向下取整
			 ySmaller=floor(fy2);

			 xBigger=ceil(fx2);  // 向上取整
			 yBigger=ceil(fy2);

             xRatio=(fx2-xSmaller)/1.0;
			 yRatio=(fy2-ySmaller)/1.0;
	
		   //for x  
			float ux11=(pNextFrame->m_pBackOptImg->at(xSmaller,ySmaller,0))*(1-xRatio)*(1-yRatio);
			float ux12=(pNextFrame->m_pBackOptImg->at(xBigger,ySmaller,0))*xRatio*(1-yRatio);
		    float ux21=(pNextFrame->m_pBackOptImg->at(xSmaller,yBigger,0))*(1-xRatio)*yRatio;
			float ux22=(pNextFrame->m_pBackOptImg->at(xBigger,yBigger,0))*xRatio*yRatio;

			float u2=ux11+ux12+ux21+ux22;

          // for y
			float vx11=(pNextFrame->m_pBackOptImg->at(xSmaller,ySmaller,1))*(1-xRatio)*(1-yRatio);
			float vx12=(pNextFrame->m_pBackOptImg->at(xBigger,ySmaller,1))*xRatio*(1-yRatio);
			float vx21=(pNextFrame->m_pBackOptImg->at(xSmaller,yBigger,1))*(1-xRatio)*yRatio;
			float vx22=(pNextFrame->m_pBackOptImg->at(xBigger,yBigger,1))*xRatio*yRatio;

			float v2=vx11+vx12+vx21+vx22;

			// method 2,四舍五入
	         x2=int(x1+u1+0.5);
			 y2=int(y1+v1+0.5);


			 float u2x=pNextFrame->m_pBackOptImg->at(x2,y2,0);
			 float v2x=pNextFrame->m_pBackOptImg->at(x2,y2,1);
			


			 if(pPreFrame->IsInMask(x1,y1))
			 {
				 std::cout<<"x1:"<<x1<<",y1:" <<y1<<std::endl;
				 std::cout<<"x2:"<<x2<<",y2:"<<y2<<std::endl;
				 std::cout<<"xSmaller:"<<xSmaller<<",xBigger:"<<xBigger<<std::endl;
				 std::cout<<"ySmaller:"<<ySmaller<<",yBigger:"<<yBigger<<std::endl;
				 
				 std::cout<<"u1:"<<u1<<",v1:"<<v1<<std::endl;
				 std::cout<<"u2x:"<<u2x<<",v2x:"<<v2x<<std::endl;
				 std::cout<<"u2:"<<u2<<",v2:"<<v2<<std::endl;
			 }
		 }
	 
	 }
*/
   std::cout<<"LSStructuremotion  RunOptRefine "<<std::endl;
   std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;

   int iFrameCount=endFrame-startFrame+1;
   int frmRefCount=LSParaWidget::GetInstance()->GetFrmRefCount();
   
   if(iFrameCount<=1||frmRefCount<=0)
   {
	   std::cout<<"Can't using optical flow to track! too few FrameCount or FrmRefCount."<<std::endl;
	   system("pause");
   }

   // mask size( width and height)
   InitMaxMaskRange(startFrame, endFrame);
   LSImgPartition::GetInstance()->SetImgSize(m_iMaxMaskWidth, m_iMaxMaskHeight);   //423，630
   std::cout<<"m_iMaxMaskWidth: "<<m_iMaxMaskWidth<<" , "<<"m_iMaxMaskHeight: "<<m_iMaxMaskHeight<<std::endl;

   // block size(width and height)
   int maxBlockWidth;
   int maxBlockHeight;
   LSImgPartition::GetInstance()->SetBlocksCount(LSParaWidget::GetInstance()->m_iBlockCountX,LSParaWidget::GetInstance()->m_iBlockCountY); //2,1
   LSImgPartition::GetInstance()->GetMaxBlockSize(maxBlockWidth, maxBlockHeight);
   std::cout<<"maxBlockWidth: "<<maxBlockWidth<<" , "<<"maxBlockHeight: "<<maxBlockHeight<<std::endl;

   //dsplevels
   int iDspLevel = LSParaWidget::GetInstance()->GetDspLevel();  //101
   std::cout<<"dspLevel:"<<iDspLevel<<std::endl;

   //dataCost
   DataCost dataCost(maxBlockWidth, maxBlockHeight, iDspLevel);

   //Opt refine count
   int passCount = LSParaWidget::GetInstance()->GetRefinePassCount(); //1
	std::cout<<"passCount: "<<passCount<<std::endl;

	for(int passi = 0; passi < passCount; passi++)
	{
		for(int iFrame = startFrame; iFrame <=endFrame ; iFrame++)
		{  
			for(int camId = 0; camId < CamNum; camId++)
			{
			
				std::cout << "Opt refine ith frame:"<<iFrame<<"/" << endFrame << " ith camera: " << camId<<"/" << CamNum-1 << std::endl;

				    //current frames
					LSVideoFrame * pFrame = GetFrameAt(camId, iFrame);
					
					// get current and reference frames (all camId form a circle to ensure the number of the forward equal to backward)
					std::vector<std::pair<int, int> > FwFramesIndex;  
					std::vector<std::pair<int, int> > BwFramesIndex;

					LSParaWidget::GetInstance()->GetOptReferenceFrameIndex(FwFramesIndex, BwFramesIndex, camId, iFrame, startFrame, endFrame);

					std::vector<LSVideoFrame *> FwFrames(FwFramesIndex.size());
					std::vector<LSVideoFrame *> BwFrames(BwFramesIndex.size());
					for(int i=0; i<FwFramesIndex.size(); i++)
					FwFrames[i] = GetFrameAt(FwFramesIndex[i].first, FwFramesIndex[i].second);
					for(int i=0; i<BwFramesIndex.size(); i++)
					BwFrames[i] = GetFrameAt(BwFramesIndex[i].first, BwFramesIndex[i].second);
                
				   //=========================================================================================//
					                      // 以下几行代码同BO，需要根据实际情况进行调整//

					//get the maxDepth,minDepth(m_fMaxDsp,m_fMinDsp)in this view through visual hull
					//init maxDspImage,minDspImage with m_fMaxDsp, m_fMinDsp
					pFrame->InitDspConstrain(m_VisualHull[iFrame].m_TriangleList, m_VisualHull[iFrame].m_VertexList);
             
                
					float VisualHullConstrianSigma = LSParaWidget::GetInstance()->GetVisualHullConstrianSigma(); //(here assign 0.95)
					
					// visualHullSigma值越小，lable值范围越大（上界不变，下界扩展）
				    pFrame->InitLabelRangeByDspRange(LSParaWidget::GetInstance()->GetDspLevel() - 1, 
						(VisualHullConstrianSigma-0.1 >= 0 ? VisualHullConstrianSigma-0.1: 0), true);  //原来为0.1  
					
			    //=======================================================================================================//

				 std::cout<<"Optical Flow Optimization function!"<<std::endl;

				 LSDepthEstimator::GetInstance()->OpticalFlowOptimization(FwFrames, BwFrames, pFrame, dataCost);
             
			}
		}
	}
 }


 void LSStructureMotion::RunDepthExpansion(int startFrame, int endFrame)
 {
	 std::cout<<"LSStructuremotion  RunDepthExpansion "<<std::endl;
	// if(endFrame==-1)
	//	 endFrame=LSStructureMotion::GetInstance()->GetFrameCount()-1;  //7
	 std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;

	 InitMaxMaskRange(startFrame, endFrame);
	 LSImgPartition::GetInstance()->SetImgSize(m_iMaxMaskWidth, m_iMaxMaskHeight);   //423，630
	  for(int iFrame = startFrame; iFrame <= endFrame; iFrame++){   //先计算单帧，然后进行扩展
		// for(int iFrame = startFrame; iFrame <1; iFrame++){
         std::cout << "Super Refine Frame:"<<iFrame <<"/" << endFrame << std::endl;
		
		 for(int camId = 0; camId < CamNum; camId++){
			 clock_t beginTime = clock();
			 std::cout << "DE for ith cam:"<<camId <<"/" << CamNum-1 << std::endl;

			 LSVideoFrame * pFrame = GetFrameAt(camId, iFrame);
			 std::vector<std::pair<int, int> > FwFramesIndex;
			 std::vector<std::pair<int, int> > BwFramesIndex;
			 LSParaWidget::GetInstance()->GetReferenceFrameIndex(FwFramesIndex, BwFramesIndex, camId, iFrame, 0, CamNum-1);

			 std::vector<LSVideoFrame *> FwFrames(FwFramesIndex.size());
			 std::vector<LSVideoFrame *> BwFrames(BwFramesIndex.size());
			 for(int i=0; i<FwFramesIndex.size(); i++)
				 FwFrames[i] = GetFrameAt(FwFramesIndex[i].first, FwFramesIndex[i].second);
			 for(int i=0; i<BwFramesIndex.size(); i++)
				 BwFrames[i] = GetFrameAt(BwFramesIndex[i].first, BwFramesIndex[i].second);

			  LSDepthEstimator::GetInstance()->SuperRefine(FwFrames, BwFrames, pFrame);

			 std::cout<<"===>Total Time:"<<(clock()-beginTime)/CLOCKS_PER_SEC<<" s"<<std::endl<<std::endl;
		 }
	 }
 }

 //NEW ADD-KXL Vision
 void LSStructureMotion::_DepthToMesh(LSStructureMotion*pMotion,int startFrame, int endFrame,SelectPointParam&param)
 {
	 //std::cout << "Depth to mesh...start"<<std::endl;
	 std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;
	 std::cout<<"start the depthToMesh,PossionPath: "<<std::endl;
	 std::cout<<m_sPossionPath<<std::endl;

	 for(int ithFrame=startFrame;ithFrame<=endFrame;ithFrame++)  //如果报错，可能是此处目录重复问题。
	 {
		 SelectPoint  aSelectP(pMotion,ithFrame,ithFrame); //针对同一帧的不同相机计算
		 aSelectP.SetParam( param );
		 //set file location for storing npts files and ply files.
		 std::string frameName=this->GetFrameAt(0,ithFrame)->m_sName;
		 std::string meshDir=this->m_sMeshDir;
		 std::string nptsFilePath=meshDir+"model_"+GetFrameAt(0,ithFrame)->m_sName+".npts";

	     aSelectP.doStart(nptsFilePath);
		                                                                                                                                                                                                                                                          
		 std::string  savePlyFileName = meshDir + "model_"+GetFrameAt(0,ithFrame)->m_sName+".ply";
	 
		 std::cout<<"m_sPossionPath: "<<m_sPossionPath<<std::endl;
		 std::cout<<"nptsFilePath: "<<nptsFilePath<<std::endl;
		 std::cout<<"savePlyFileName: "<<savePlyFileName<<std::endl;

		 // 调用possion function (不需要修改）
		 PossionShExec(m_sPossionPath, nptsFilePath, savePlyFileName, LSParaWidget::GetInstance()->GetPoissonSamples());
		
	 }

 }



//ORG-LQL Vision
 void LSStructureMotion::DepthToMesh( int startFrame, int endFrame){
	 std::cout << "Depth to mesh...start"<<std::endl;
	 std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;
	
	 //Point Cloud Sampling
	 PointCloudSampling* pPCS = PointCloudSampling::GetInstance();
	 pPCS->mMotion = this;

	 for(int ithFrame=startFrame;ithFrame<=endFrame;ithFrame++)
	 {
		 pPCS->mStartFrame = ithFrame;
		 pPCS->mEndFrame = ithFrame;
		 pPCS->fPointCloudSampling();

		 //set file location for storing npts files and ply files.
		 std::string frameName=this->GetFrameAt(0,ithFrame)->m_sName;
		 std::string meshDir=this->m_sMeshDir;
		 std::string nptsFilePath=meshDir+"model_"+GetFrameAt(0,ithFrame)->m_sName+".npts";
		 //std::string nptsFilePath = pPCS->GetNptsFilePath();

		 std::string  savePlyFileName = meshDir + "model_"+GetFrameAt(0,ithFrame)->m_sName+".ply";
	     std::cout<<"m_sPossionPath: "<<m_sPossionPath<<std::endl;
	   	 std::cout<<"nptsFilePath: "<<nptsFilePath<<std::endl;
		 std::cout<<"savePlyFileName: "<<savePlyFileName<<std::endl;
		 std::cout<<"getPossionSamples: "<<LSParaWidget::GetInstance()->GetPoissonSamples()<<std::endl;

		 // 调用possion function
		 PossionShExec(m_sPossionPath, nptsFilePath, savePlyFileName, LSParaWidget::GetInstance()->GetPoissonSamples());
	 }
	
 }

 //Run all parts.
void LSStructureMotion::RunAll( int startFrame, int endFrame )
{
	endFrame=0;
	std::cout << "RunAll...start!"<<std::endl;
	std::cout<<"startFrame: "<<startFrame<<" , "<<"endFrame: "<<endFrame<<std::endl;  
	CalculateVisualHull(startFrame,endFrame);  //need manual mask in different cams as/////////// prequiste for getting visualhull file
	//AutoCalcVisualHull(startFrame,endFrame);
	RunInitialization(startFrame, endFrame); //Init
    RunRefine( startFrame, endFrame);    //BO (DE）in_use  
    //RunOptRefine(startFrame,endFrame);  //Opt
    //RunDepthExpansion( startFrame, endFrame);  //DE
    // DepthToMesh( startFrame, endFrame);  //LQL version
    _OnMesh(startFrame,endFrame);   //KXL version
    //TextureMapping( startFrame, endFrame);
	std::cout << "Run all...end!"<<std::endl;
	//std::cout<<"===>Total Time:"<<(clock()-beginTime)/CLOCKS_PER_SEC<<" s"<<std::endl<<std::endl;	
}


//NEW ADD-KXL version
void LSStructureMotion::_OnMesh(int startFrame,int endFrame)
{
	std::cout<<"begin mesh construct\n";
    SelectPointParam  param;
	param.mDensity = 1;
	param.mHalfWinSizeForDensity = 0;
	param.mHalfWinSizeForPlaneFit = 2;
	param.mCoarseInteval = 18;
	param.mBackProjImgSigma = 1.0f;
	param.mLeastConf = 0.25;
	for(int iFrame=startFrame;iFrame<=endFrame;iFrame++)
	{
		param.mDspMin = LSStructureMotion::GetInstance()->m_fDspMin[iFrame]; //这两个参数由BoundingBox计算过程中得到。
		param.mDspMax = LSStructureMotion::GetInstance()->m_fDspMax[iFrame];
		//param.mDspMin = 0.0274324; //按KXL输入临时赋值（实际需要根据boundingBox计算得到）
		//param.mDspMax = 0.0773577;'
	    _DepthToMesh(this,iFrame,iFrame,param);
	}
	std::cout<<"KXL version run mesh!"<<std::endl;
}







