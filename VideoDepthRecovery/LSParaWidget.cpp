#include "lsparawidget.h"
#include "LSVideoFrame.h"

LSParaWidget::LSParaWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.reset(new Ui::LSParaWidget());

	ui->setupUi(this);

	//必须初始化实例，否则会报错
    //Inialize();
/*
	UpdateSet();
	Publish();
*/
}

LSParaWidget::~LSParaWidget()
{
	m_dDspMin.clear();
	m_dDspMax.clear();
}
// have not finished 
void LSParaWidget::Inialize()
{
	//Mean Shift Segmentation
	m_iSegSpatial = 5;
	m_fSegColor = 5;
	m_iSegMinsize = 50;
	m_iPlaneFittingSize = 300;
	
	m_dSegErrRateThreshold = 2.5;


	// for Visualhull
	m_iDetailLevels = 9;
	m_iImgApron = 30;
	m_fVisualHullConstrianSigma = 0.80;  //org=0.95

	//AutoVisualHull
	m_dColorDiffThreshold=5.0;
	m_iMaxIntervalVoxel=100; 

	// Depth recovery
	m_iDspLevel=101;
	m_iSubSample = 15;  //org=10
	m_iSubIter = 1;

	//reference frames parameters ( for different cams in the same frames)
	m_iRefStart = 1;
	m_iRefStep = 1;
	//m_iRefCount = 10; 
    m_iRefCount = 2;     //forward and backward reference frames count


	//reference frames parameters ( for different frames in the same cam)
	m_iFrmRefStart = 1;
	m_iFrmRefStep = 1; 
	m_iFrmRefCount = 2;     //forward and backward reference frames count

	// wheather output temp result
	m_bGenerateTempResult = true;

    //Block 
   //  m_iBlockCountX=2; //ORG
	m_iBlockCountX=1; //new change
	m_iBlockCountY=1; 
	m_fOverlap=8/100.0F; 


	//project
   m_fColorSigma=10;   //sigma_c  o=5

   m_fDiffTruncated=20; // color difference truncated value

	//double m_dProjectSigma;                                                         
   m_fDspSigma=0.03;        //sigma_d
   m_fColorMissPenalty=20; // penalty factor  o=10

   //datacost weight( //smooth weight)
   m_fDataCostWeight = 20;   
    
 
   m_fDiscK = 3;   //truncate value   o=10    

   // for BO
   m_iRefinePassCount = 2;             //应设置为2

   //mesh
   m_fPoissonSamples = 5;

  //std::cout<<"Inialize System Parameters:: need to be finished"<<std::endl;
}

//  这里求深度和BDM相似，也是应该以每帧为单位（这样才能够把VisualHull信息用上）
void LSParaWidget::GetReferenceFrameIndex( std::vector<std::pair<int, int> >& FwFrameIndex, std::vector<std::pair<int, int> >& BwFrameIndex, 
	int CurrentCameraId, int CurrentFrameId, int StartIndex, int EndIndex ){

		FwFrameIndex.clear();
		BwFrameIndex.clear();
	  //	int anotherCameraId = (CurrentCameraId + 1) % CamNum;    //this code not be used

	   /*
		//add reference frames forward
		int iCount = 0;
		for(int index = CurrentIndex + m_iRefStart; index <= EndIndex && iCount < m_iRefCount; index += m_iRefStep){
			FwFrameIndex.push_back(std::pair<int, int>(CurrentCameraId, index));
			//Temp
			//FwFrameIndex.push_back(std::pair<int, int>(anotherCameraId, index));
			++iCount;
		}
	   */
		//add reference frames forward
		int iCount = 0;
		//for(int index = CurrentCameraId + m_iRefStart; index <= EndIndex && iCount < m_iRefCount; index += m_iRefStep){ //without circle
	    for(int index = CurrentCameraId + m_iRefStart; iCount < m_iRefCount; index += m_iRefStep){  //form a circle
			if(index>=CamNum)index=index%CamNum;
			FwFrameIndex.push_back(std::pair<int, int>(index, CurrentFrameId));
			//Temp
			//FwFrameIndex.push_back(std::pair<int, int>(anotherCameraId, index));
			++iCount;
		}


		/*
	    //add reference frames backward
		iCount = 0;
		for(int index = CurrentIndex - m_iRefStart; index >= StartIndex && iCount < m_iRefCount; index -= m_iRefStep){
			BwFrameIndex.push_back(std::pair<int, int>(CurrentCameraId, index));
			//Temp
			//BwFrameIndex.push_back(std::pair<int, int>(anotherCameraId, index));
			++iCount;
		}
		*/

		//add reference frames backward
		iCount = 0;
		//for(int index = CurrentCameraId - m_iRefStart; index >= StartIndex && iCount < m_iRefCount; index -= m_iRefStep){ //without circle
		for(int index = CurrentCameraId - m_iRefStart; iCount < m_iRefCount; index -= m_iRefStep){  //form a circle
			if(index<0)index=(index+CamNum)%CamNum;
			BwFrameIndex.push_back(std::pair<int, int>(index, CurrentFrameId));
			//Temp
			//BwFrameIndex.push_back(std::pair<int, int>(anotherCameraId, index));
			++iCount;
		}

		std::cout << "FW Frames: ";
		for(int i=0; i<FwFrameIndex.size(); ++i)
			std::cout << "(" << FwFrameIndex[i].first << "," << FwFrameIndex[i].second << ") ";
		std::cout << std::endl;

		std::cout << "BW Frames:";
		for(int i=0; i<BwFrameIndex.size(); ++i)
			std::cout << "(" << BwFrameIndex[i].first << "," << BwFrameIndex[i].second << ") ";
		std::cout << std::endl;


      //std::cout<<"GetReferenceFrameIndex Over...."<<std::endl;
}




//  这里求深度是在spatial 约束的基础上加上temporal 约束，用光流进行优化，不仅参考同一帧上的不同相机，而且要加入同一相机的不同帧进行约束
void LSParaWidget::GetOptReferenceFrameIndex( std::vector<std::pair<int, int> >& FwFrameIndex, std::vector<std::pair<int, int> >& BwFrameIndex, 
	int CurrentCameraId, int CurrentFrameId, int StartFrameIndex, int EndFrameIndex ){

		FwFrameIndex.clear();
		BwFrameIndex.clear();
		
		int iFrmCount=0;
		int iCount=0;
		// current cam, forward frames
		for(int frmIndex = CurrentFrameId + m_iFrmRefStart; frmIndex <= EndFrameIndex && iFrmCount < m_iFrmRefCount; frmIndex+= m_iFrmRefStep)
		{

			FwFrameIndex.push_back(std::pair<int, int>(CurrentCameraId, frmIndex));

			iFrmCount++;

			iCount++;

		}

		//current cam, backward frames
		iFrmCount=0;
		for(int frmIndex = CurrentFrameId -m_iFrmRefStart; frmIndex >= StartFrameIndex && iFrmCount < m_iFrmRefCount; frmIndex-= m_iFrmRefStep)
		{

			FwFrameIndex.push_back(std::pair<int, int>(CurrentCameraId, frmIndex));

			iFrmCount++;

			iCount++;
		}


		//add reference frames forward
		int iCamCount = 0;
		 
	        iCount=0;

		// cam forward
	    for(int camIndex = CurrentCameraId + m_iRefStart; iCamCount < m_iRefCount; camIndex += m_iRefStep)
		{  
			if(camIndex>=CamNum)camIndex=camIndex%CamNum;  //form a circle(cams)

			 //current frame (cam forward)
			 FwFrameIndex.push_back(std::pair<int, int>(camIndex, CurrentFrameId));
			 iCount++;


			//frame forward
			 iFrmCount=0;
			for(int frmIndex = CurrentFrameId + m_iFrmRefStart; frmIndex <= EndFrameIndex && iFrmCount < m_iFrmRefCount; frmIndex+= m_iFrmRefStep)
			{

				FwFrameIndex.push_back(std::pair<int, int>(camIndex, frmIndex));

				iFrmCount++;

				iCount++;

			}

			//frame backward
			iFrmCount=0;
			for(int frmIndex = CurrentFrameId -m_iFrmRefStart; frmIndex >= StartFrameIndex && iFrmCount < m_iFrmRefCount; frmIndex-= m_iFrmRefStep)
			{

				FwFrameIndex.push_back(std::pair<int, int>(camIndex, frmIndex));

				iFrmCount++;

				iCount++;

			}

			iCamCount++;
		
		}


		//add reference frames backward
		iCount = 0;
		iCamCount=0;
		iFrmCount=0;


		//cam backward
		for(int camIndex = CurrentCameraId - m_iRefStart; iCamCount < m_iRefCount; camIndex -= m_iRefStep)
		{ 
			if(camIndex<0)camIndex=(camIndex+CamNum)%CamNum;   //form a circle
			 
			// frame current(cam backward)
			BwFrameIndex.push_back(std::pair<int, int>(camIndex, CurrentFrameId));
			iCount++;


			//frame forward
			iFrmCount=0;
			for(int frmIndex = CurrentFrameId + m_iFrmRefStart; frmIndex <= EndFrameIndex && iFrmCount < m_iFrmRefCount; frmIndex+= m_iFrmRefStep)
			{

				BwFrameIndex.push_back(std::pair<int, int>(camIndex, frmIndex));

				iFrmCount++;

				iCount++;

			}


			//frame backward
			iFrmCount=0;
			 for(int frmIndex = CurrentFrameId -m_iFrmRefStart; frmIndex >= StartFrameIndex && iFrmCount < m_iFrmRefCount; frmIndex-= m_iFrmRefStep)
			 {

				 BwFrameIndex.push_back(std::pair<int, int>(camIndex, frmIndex));

				 iFrmCount++;

				 iCount++;

			 }

			
			 iCamCount++;
		}


		std::cout<<"CurrentCameraId:"<<CurrentCameraId<<",CurrentFrameId:"<<CurrentFrameId<<std::endl;

		std::cout << "FW Frames: ";
		for(int i=0; i<FwFrameIndex.size(); ++i)
			std::cout << "(" << FwFrameIndex[i].first << "," << FwFrameIndex[i].second << ") ";
		std::cout << std::endl;

		std::cout << "BW Frames:";
		for(int i=0; i<BwFrameIndex.size(); ++i)
			std::cout << "(" << BwFrameIndex[i].first << "," << BwFrameIndex[i].second << ") ";
		std::cout << std::endl;

      //std::cout<<"GetReferenceFrameIndex Over...."<<std::endl;
}
