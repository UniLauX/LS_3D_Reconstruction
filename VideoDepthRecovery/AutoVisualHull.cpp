#include "AutoVisualHull.h"
#include <iostream>
#include "LSStructureMotion.h"

AVisualHull::AVisualHull(void)
{
	m_iImgWidth=0;
	m_iImgHeight=0;
	//volumeVoxels=NULL;
}


AVisualHull::~AVisualHull(void)
{
}

//method1: not enlarge the volume
void AVisualHull::InitGenerateVolume(const Wml::Vector3d& LowCorner,const Wml::Vector3d& HighCorner, int iMaxIntervalVoxel, int iImgApron)
{ 
      this->Clear(); 
	  m_LowCorner=LowCorner;
	  m_HighCorner=HighCorner;
	  m_iMaxIntervalVoxel=iMaxIntervalVoxel;
	  m_iImgApron=iImgApron;

	  double distx = fabs(m_HighCorner[0] - m_LowCorner[0]);
	  double disty = fabs(m_HighCorner[1] - m_LowCorner[1]);
	  double distz=fabs(m_HighCorner[2]- m_LowCorner[2]);
	  m_dMaxLen = std::max<double>( std::max<double>(distx, disty), distz);

	  m_dCellLength=m_dMaxLen/(double)m_iMaxIntervalVoxel;

	  m_iCellNumInX=int((m_HighCorner[0]-m_LowCorner[0])/m_dCellLength)+1;
	  m_iCellNumInY=int((m_HighCorner[1]-m_LowCorner[1])/m_dCellLength)+1;
	  m_iCellNumInZ=int((m_HighCorner[2]-m_LowCorner[2])/m_dCellLength)+1;
	  int voxelNum=m_iCellNumInX*m_iCellNumInY*m_iCellNumInZ;
      std::cout<<"voxelNum: "<<voxelNum<<std::endl;
	 
	  m_VoxelBase=new VoxelCoordBase;
	  m_VoxelBase->m_RootLowCorner=m_LowCorner;       //true low corner coordinate
	  m_VoxelBase->m_dSmallestCellLength=m_dCellLength; //true cell width

	  //Generate Volume;
      int vId=0;
	  for(int xId=0;xId<m_iCellNumInX;xId++)
	  {
		  for(int yId=0;yId<m_iCellNumInY;yId++)
		  {
			  //vId=yId+xId*m_iCellNumInY;
			  for(int zId=0;zId<m_iCellNumInZ;zId++)
			  {
				 //vId=zId+vId*m_iCellNumInZ;
				   vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
				   AVoxel* curVoxel=new AVoxel();
				   curVoxel->vIdx=vId;
				   curVoxel->m_iLowX=xId;
                   curVoxel->m_iLowY=yId;
				   curVoxel->m_iLowZ=zId;
				   curVoxel->m_pVoxelBase=m_VoxelBase;
				   m_VoxelVolume.push_back(curVoxel);
			  }
		  }
	  }

	  //test:
	  std::cout<<"m_dMaxLen: "<<m_dMaxLen<<std::endl;
	  std::cout<<"m_dCellLength: "<<m_dCellLength<<std::endl;
	  std::cout<<"m_iCellNumInX: "<<m_iCellNumInX<<std::endl;
	  std::cout<<"m_iCellNumInY: "<<m_iCellNumInY<<std::endl;
	  std::cout<<"m_iCellNumInZ: "<<m_iCellNumInZ<<std::endl;
      std::cout<<"Auto VisualHull Init finish..."<<std::endl;
}

void AVisualHull::Clear()
{
   for(int i=0;i<m_VoxelVolume.size();i++)
   {
	   if(m_VoxelVolume[i]!=NULL)
		   delete m_VoxelVolume[i];
	   m_VoxelVolume[i]=NULL;
   }

  for(int i=0;i<m_SeedVoxels.size();i++)
  {
	  if(m_SeedVoxels[i]!=NULL)
		  delete m_SeedVoxels[i];
	  m_SeedVoxels[i]=NULL;
  }
  
  for(int i=0;i<m_vProbFgdImgs.size();i++)
  {
	  if(!m_vProbFgdImgs[i].empty())
		  m_vProbFgdImgs[i].release();
  }


	std::cout<<"Auto VisualHull Clear..."<<std::endl;
}

void AVisualHull::GenerateSeedVoxels(LSStructureMotion* pMotion, int iStart, int iEnd)
{	
	//assign frame be calculated
	m_pMotion = pMotion;
	m_iStartFrame = iStart;
	m_iEndFrame = iEnd;
	if(m_iStartFrame==m_iEndFrame)
		m_iCurFrame=m_iStartFrame;
	else
	{
		std::cout<<"m_iStartFrame is not equal to m_iEndFrame"<<std::endl;	
	}
	
	m_iImgWidth=m_pMotion->GetFrameAt(0,m_iCurFrame)->GetImgWidth();
	m_iImgHeight=m_pMotion->GetFrameAt(0,m_iCurFrame)->GetImgHeight();

     //get file name for store VisualHull file
	 m_sFrameName=m_pMotion->GetFrameAt(0,m_iCurFrame)->m_sName;
	 std::string vhPlyName = GetVisualHullFileName();   //vh file name and path
	 std::cout<<"vhPlyName: "<<vhPlyName<<std::endl;


	 //进行后续过程之前，首先确保mask存储正确：
	  //load mask image
		 for(int m = 0; m < CamNum; m++)
		 {
			 m_pMotion->GetFrameAt(m,m_iCurFrame)->LoadMaskImg();
		     m_pMotion->GetFrameAt(m,m_iCurFrame)->setup(m_pMotion->GetFrameAt(m,m_iCurFrame));  //NEW ADD (set up mask image）
		 }
	    
		//set labels for the voxels
         for(int idx=0;idx<m_VoxelVolume.size();idx++)
		 {
			 //std::cout<<"wCoord: "<<wCoord[0]<<","<<wCoord[1]<<","<<wCoord[2];
			 //std::cout<<std::endl;
			 AVoxel *curVoxel=m_VoxelVolume.at(idx);
			 //VoxelType voxelType=CheckVoxel(curVoxel); //org
			 VoxelType voxelType=CheckVoxel(curVoxel);  //new add
			 if(voxelType==VOXEL_INSIDE)
			 {
				 m_VoxelVolume[idx]->flag=GC3D_FGD;     //Assign flags here
			 }
			 else if(voxelType==VOXEL_OUTSIDE)
				// m_VoxelVolume[idx]->flag=GC3D_PR_BGD;  //Assign flags here
				 m_VoxelVolume[idx]->flag=GC3D_BGD; 

			 else if(voxelType==VOXEL_CROSS)
				 m_VoxelVolume[idx]->flag=GC3D_PR_FGD;  //Assign flags here
		 }
		 
}


void AVisualHull::GenerateSeedVoxelBall(LSStructureMotion* pMotion, int iStart, int iEnd,const Wml::Vector3d& LowCorner,const Wml::Vector3d& HighCorner)
{
	//assign frame be calculated
	m_pMotion = pMotion;
	m_iStartFrame = iStart;
	m_iEndFrame = iEnd;
	if(m_iStartFrame==m_iEndFrame)
		m_iCurFrame=m_iStartFrame;
	else
	{
		std::cout<<"m_iStartFrame is not equal to m_iEndFrame"<<std::endl;	
	}

	m_iImgWidth=m_pMotion->GetFrameAt(0,m_iCurFrame)->GetImgWidth();
	m_iImgHeight=m_pMotion->GetFrameAt(0,m_iCurFrame)->GetImgHeight();

	//get file name for store VisualHull file
	m_sFrameName=m_pMotion->GetFrameAt(0,m_iCurFrame)->m_sName;
	std::string vhPlyName = GetVisualHullFileName();   //vh file name and path
	std::cout<<"vhPlyName: "<<vhPlyName<<std::endl;


	//center Voxel
	m_avCenterVoxel=new AVoxel();
    m_avCenterVoxel->m_iLowX=m_iCellNumInX/2;
	m_avCenterVoxel->m_iLowY=m_iCellNumInY/2;
	m_avCenterVoxel->m_iLowZ=m_iCellNumInZ/2;
	m_avCenterVoxel->vIdx=m_avCenterVoxel->m_iLowZ+(m_avCenterVoxel->m_iLowY+m_avCenterVoxel->m_iLowX*m_iCellNumInY)*m_iCellNumInZ;
	m_avCenterVoxel->m_pVoxelBase=m_VoxelBase;
	int iRadius=min(m_iCellNumInX,min(m_iCellNumInY,m_iCellNumInZ))/15;  //先试试1/15，该值可以调节
	double dRduDist=3*(m_dCellLength*iRadius)*(m_dCellLength*iRadius);  //x^2+y^2+z^2;
	
	//distinguish outer  voxel
	int trucIdX1,trucIdY1,trucIdZ1;
    int trucIdX2,trucIdY2,trucIdZ2;

	trucIdX1=max(0,(LowCorner[0]-m_LowCorner[0])/m_dCellLength-1);
	trucIdY1=max(0,(LowCorner[1]-m_LowCorner[1])/m_dCellLength-1);
	trucIdZ1=max(0,(LowCorner[2]-m_LowCorner[2])/m_dCellLength-1);

	trucIdX2=min((HighCorner[0]-m_LowCorner[0])/m_dCellLength+1,m_iCellNumInX);
	trucIdY2=min((HighCorner[1]-m_LowCorner[1])/m_dCellLength+1,m_iCellNumInY);
	trucIdZ2=min((HighCorner[2]-m_LowCorner[2])/m_dCellLength+1,m_iCellNumInZ);

	std::cout<<"trucId1:"<<trucIdX1<<","<<trucIdY1<<","<<trucIdZ1<<std::endl;
	std::cout<<"trucId2:"<<trucIdX2<<","<<trucIdY2<<","<<trucIdZ2<<std::endl;
	/*
	int trucVId1=trucIdZ1+(trucIdY1+trucIdX1*m_iCellNumInY)*m_iCellNumInZ;
	Wml::Vector3d trucPt1=m_VoxelVolume[trucVId1]->GetLowCorner();
	int trucVId2=trucIdZ2+(trucIdY2+trucIdX2*m_iCellNumInY)*m_iCellNumInZ;
	Wml::Vector3d trucPt2=m_VoxelVolume[trucVId2]->GetLowCorner();
	std::cout<<"trucPt1: "<<trucPt1[0]<<","<<trucPt1[1]<<","<<trucPt1[2]<<std::endl;
	std::cout<<"trucPt2: "<<trucPt2[0]<<","<<trucPt2[1]<<","<<trucPt2[2]<<std::endl;
	*/
	//Init Labels for each voxel in Volume;        //此处可能有问题！（可能）将球周围的元素赋值未背景
	int vId=0;
	for(int xId=0;xId<m_iCellNumInX;xId++)
	{
		for(int yId=0;yId<m_iCellNumInY;yId++)
		{
			//vId=yId+xId*m_iCellNumInY;
			for(int zId=0;zId<m_iCellNumInZ;zId++)
			{
				//vId=zId+vId*m_iCellNumInZ;
				 vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
				 if(xId>=trucIdX1&&xId<=trucIdX2&&yId>=trucIdY1&&yId<=trucIdY2&&zId>=trucIdZ1&&zId<=trucIdZ2)
				 {
					 double dist=GetDistanceFromCenter(m_VoxelVolume[vId],m_avCenterVoxel);
					 if(dist<=dRduDist)
						 m_VoxelVolume[vId]->flag=GC3D_FGD;
					 else
						 m_VoxelVolume[vId]->flag=GC3D_PR_FGD;
				 }
				 else
					  m_VoxelVolume[vId]->flag=GC3D_BGD;
			}
		}
	}

}



std::string AVisualHull::GetVisualHullFileName()
{
	 return m_pMotion->m_sVisualHullDir +m_sFrameName+std::string(".ply");
}



/*如果存在Voxel投影在Mask之外，则把这个Voxel标记为物体之外
否则，如果Voxel都投影到Mask之内则标记为物体之内。
最后如果一个Voxel的投影或者在Mask之内，或者跨Mask边界*/
VoxelType  AVisualHull::CheckVoxel(AVoxel* curVoxel)
{
	bool isAllinMask = false;
	//in fact,just deal with single frame once
	//for(int iFrame = m_iStartFrame; iFrame <= m_iEndFrame; iFrame++)
	//{
	   int iFrame=m_iCurFrame;
		for(int camId= 0; camId< CamNum; camId++)
		{
			LSVideoFrame* pFrame = m_pMotion->GetFrameAt(camId, iFrame);
			VoxelType voxelType;
			voxelType = IsVoxelInMaskFaceSamp(curVoxel, pFrame);   //Bug may here
			if(voxelType == VOXEL_OUTSIDE)
				return VOXEL_OUTSIDE;
			if(voxelType == VOXEL_CROSS)
			    return VOXEL_CROSS;
		}
		 return VOXEL_INSIDE;
	//}
}


//如果存在n(2)个以上Voxel投影在Mask之内，则视为前景voxel，否则视为背景voxel/
VoxelType  AVisualHull::CheckVoxel1(AVoxel* curVoxel)     //此方法似乎并不work，看来三维投2维的策略上有问题
{
	//bool isAllinMask = false;
	//in fact,just deal with single frame once
//	for(int iFrame = m_iStartFrame; iFrame <= m_iEndFrame; iFrame++)
//	{
		int iFrame=m_iCurFrame;
		int castInNum=0;
		for(int camId= 0; camId< CamNum; camId++)
		{
			LSVideoFrame* pFrame = m_pMotion->GetFrameAt(camId, iFrame);
			VoxelType voxelType;
			voxelType = IsVoxelInMaskFaceSamp(curVoxel, pFrame);  
			if(voxelType==VOXEL_INSIDE)
				castInNum++;
		}
		if(castInNum>=15)
		return VOXEL_INSIDE;
		else
		return VOXEL_OUTSIDE;
	//}
}





VoxelType AVisualHull::IsVoxelInMaskFaceSamp(AVoxel* curVoxel, LSVideoFrame* curVideoFrame)
{
	// std::cout<<"curVoxel:"<<curVoxel->m_iLowX<<","<<curVoxel->m_iLowY<<","<<curVoxel->m_iLowZ<<","<<curVoxel->m_iSize<<std::endl;
	// std::cout<<"m_pVoxelBase:"<<curVoxel->m_pVoxelBase->m_dSmallestCellLength<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[0]<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[1]<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[2]<<std::endl;
	// std::cout<<"curLevel:"<<curLevel<<std::endl;
	//int steps = 1 << ( m_iDetailLevel - curLevel );  //base voxel numbers of the current voxel contains
	int steps = 1;
	int inCNT = 0;
	int outCnt = 0;
	Wml::Vector3d  lowCorner = curVoxel->GetLowCorner();

	//对于非最后一层的每一层，只要遍历该Voxel的六个面，证明六个面在Object内(那么该voxel的八个顶点也在Obect内），即可证明该Voxel在Object内。
	//针对voxel在Object边缘和在Obect外的情况与在Object内类似。
	int x, y, z;
	x= 0;
	//遍历六个面，垂直于X轴的两个面
	for(y = 0; y<=steps; y++)
	{
		for(z = 0; z<=steps; z++)
		{
			Wml::Vector3d  offset((double)x,(double)y,(double)z);
			Wml::Vector3d curCorner = lowCorner + (offset * m_dCellLength);
			bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
			if(isIn) 
				inCNT++;
			else 
				outCnt++;

			if(inCNT>0 && outCnt>0)		
				return VOXEL_CROSS;//cross
		}
	}

	x= steps;
	//遍历六个面，垂直于X轴的两个面
	//if(curVideoFrame->GetFaceVis(1))	
	for(y = 0;y<=steps;y++){
		for(z = 0;z<=steps;z++){
			Wml::Vector3d  offset((double)x,(double)y,(double)z);
			Wml::Vector3d curCorner = lowCorner + (offset * m_dCellLength);
			//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
			bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
			if(isIn) 
				inCNT++;
			else 
				outCnt++;

			if(inCNT>0 && outCnt>0)		
				return VOXEL_CROSS;//cross
		}
	}
	//如果是最后一层则，上面的计算已经将所有的面上的顶点（共8个）遍历完
    
	//按理说针对只有一层的voxel，上面已经将其8个顶点遍历完成，下面的代码是多余的

	//垂直于Y轴的两个面
		y = 0;
		//if(curVideoFrame->GetFaceVis(2))	
		for(x = 0;x<=steps;x++)
		{
			for(z = 0;z<=steps;z++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}

		y = steps;
		//if(curVideoFrame->GetFaceVis(3))	
		for(x = 0;x<=steps;x++)
		{
			for(z = 0;z<=steps;z++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}


		z = 0;
		//if(curVideoFrame->GetFaceVis(4))
		for(y = 0;y<=steps;y++)
		{		
			for(x = 0;x<=steps;x++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}

		z = steps;
		//if(curVideoFrame->GetFaceVis(5))
		for(y = 0;y<=steps;y++)
		{		
			for(x = 0;x<=steps;x++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}
     
	//std::cout<<"IsVoxelInMaskFaceSamp...func"<<std::endl;
	if(inCNT==0)		
		return VOXEL_OUTSIDE;//outside
	//if(outCnt==0)		return VOXEL_INSIDE;//inside	
	else return VOXEL_INSIDE;//it is only for erease the warning and is the same with the line above
   
}

bool AVisualHull::TmpGenerateFunc(/*LSStructureMotion* pMotion, int iStart, int iEnd*/)
{
	m_sFrameName=m_pMotion->GetFrameAt(0,m_iCurFrame)->m_sName;
	std::cout<<"m_sFrameName:"<<m_sFrameName<<std::endl;

	for(int camId=0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
		
		//color image
		curFrame->m_mColorImg=cv::imread(curFrame->m_sColorImgPathName);

		//cast image
		curFrame->m_mColorImg.copyTo(curFrame->m_mCastImg);
		std::string castImgName='c'+curFrame->m_sName;
		curFrame->m_sCastImgPathName=FileNameParser::findFileNameDir(curFrame->m_sColorImgPathName)+castImgName+std::string(".bmp");

		//backup (background subtraction) image
		std::string bgdSubImgName="bs"+curFrame->m_sName;
		curFrame->m_sBgdSubImgPathName=FileNameParser::findFileNameDir(curFrame->m_sMaskImgPathName)+bgdSubImgName+std::string(".bmp");
		curFrame->m_mBgdSubImg=cv::imread(curFrame->m_sMaskImgPathName);
		cv::imwrite(curFrame->m_sBgdSubImgPathName,curFrame->m_mBgdSubImg);
	}

	int cnt=0;
	// Cast Seed Points and Generate Mask on Image
	cv::Vec3d castColor(0,0,255);   //Red color
	cv::Vec3d maskColor(255,255,255); //White color
	//cast seedVoxels from 3D to 2D
	for(int camId=0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
		
		//reset mask image(set zero to all elem)
		curFrame->m_mMaskImg=cv::imread(curFrame->m_sMaskImgPathName);
		curFrame->m_mMaskImg.setTo(cv::Scalar(0));

		//reset cast image(copy color image)
		if(!curFrame->m_mColorImg.empty())
			curFrame->m_mColorImg.copyTo(curFrame->m_mCastImg);
		else
		{
			std::cout<<"m_mColorImg is empty!"<<std::endl;
			return false;
		}
		for(int idx=0;idx<m_VoxelVolume.size();idx++)
		{
			if(m_VoxelVolume[idx]->flag==GC3D_FGD)   
			{
				cnt++;
				Wml::Vector3d wCoord=m_VoxelVolume[idx]->GetCenterCoord();  //此处应该投射voxel的八个顶点，而非中间的一个点

				double u,v,dsp;
				curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,wCoord);

				int tmpu=(int)u;
				int tmpv=(int)v;
				if( tmpu<0  ||  tmpv<0  || (tmpu + 1>=curFrame->GetImgWidth())  || (tmpv + 1>=curFrame->GetImgHeight()) )
				{
					//std::cout<<"cast out beyond edge of image"<<std::endl;
					continue;
				}
				else
				{
					curFrame->m_mMaskImg.at<cv::Vec3b>(tmpv,tmpu)=maskColor; //set value for(casted) mask

					//set color for cast image
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[0]=(castColor[0]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[0]*0.6;
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[1]=(castColor[1]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[1]*0.6;
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[2]=(castColor[2]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[2]*0.6;
				}
			}
		}
		cv::imwrite(curFrame->m_sCastImgPathName,curFrame->m_mCastImg); //write in cast image
		cv::imwrite(curFrame->m_sMaskImgPathName,curFrame->m_mMaskImg); //write in mask image
		std::cout<<"cnt:"<<cnt<<std::endl;
	}
	return true;
}

bool AVisualHull::CastVisualHulltoMask()
{

	int cnt=0;
	cv::Vec3d castColor(0,0,255);   //Red color
	cv::Vec3d maskColor(255,255,255); //White color
	
	//cast seedVoxels from 3D to 2D
	for(int camId=0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);

		//reset mask image(set zero to all elem)
		curFrame->m_mMaskImg=cv::imread(curFrame->m_sMaskImgPathName);
		curFrame->m_mMaskImg.setTo(cv::Scalar(0));                      //此处置0似乎没用

		//reset cast image(copy color image)
		if(!curFrame->m_mColorImg.empty())
		curFrame->m_mColorImg.copyTo(curFrame->m_mCastImg);
		else
		{
			std::cout<<"m_mColorImg is empty!"<<std::endl;
            return false;
		}

		for(int idx=0;idx<m_VoxelVolume.size();idx++)
		{
			if(m_VoxelVolume[idx]->flag==GC3D_FGD||m_VoxelVolume[idx]->flag==GC3D_PR_FGD)   
			{
				m_VoxelVolume[idx]->flag=GC3D_FGD;   //new add
				cnt++;
				Wml::Vector3d wCoord=m_VoxelVolume[idx]->GetCenterCoord();
				double u,v,dsp;
				curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,wCoord);

				int tmpu=(int)u;
				int tmpv=(int)v;
				if( tmpu<0  ||  tmpv<0  || (tmpu + 1>=curFrame->GetImgWidth())  || (tmpv + 1>=curFrame->GetImgHeight()) )
				{
					//std::cout<<"cast out beyond edge of image"<<std::endl;
					continue;
				}
				else
				{
					curFrame->m_mMaskImg.at<cv::Vec3b>(tmpv,tmpu)=maskColor; //set value for(casted) mask

					//set color for cast image
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[0]=(castColor[0]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[0]*0.6;
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[1]=(castColor[1]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[1]*0.6;
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[2]=(castColor[2]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[2]*0.6;
				}
			}
		}
		std::cout<<"cnt:"<<cnt<<std::endl;
		cv::imwrite(curFrame->m_sMaskImgPathName,curFrame->m_mMaskImg); //write in mask image
		cv::imwrite(curFrame->m_sCastImgPathName,curFrame->m_mCastImg); //write in cast image
		
	}
    std::cout<<"Cast VisualHull to Mask..."<<std::endl;
	return true;
}


//Paper method
void AVisualHull::Execute3DGraphCut(int iterCount,bool isConverged )
{
	Wml::Vector3d minXYZ, maxXYZ;
	std::vector<AVoxel*>& volumeVoxels=m_VoxelVolume; //image

	minXYZ=this->m_LowCorner; //rect
	maxXYZ=this->m_HighCorner; //rect

	//assert empty and no iterator
	if(volumeVoxels.empty())
	{
		std::cout<<"VolumeVoxels is empty!"<<std::endl;
		return;
	}
	else if(iterCount<0)
	{
		std::cout<<"iterCount for 3D-GraphCut is smaller than 0!";
		return;
	}
       else if(isConverged==true)
	{
		std::cout<<"VisualHull has converged!"<<std::endl;
		return;
	}
	// Check Mask
	for(int i=0;i<volumeVoxels.size();i++)
	{
		if(!(volumeVoxels[i]->flag==GC3D_PR_FGD||volumeVoxels[i]->flag==GC3D_FGD||volumeVoxels[i]->flag==GC3D_PR_BGD||volumeVoxels[i]->flag==GC3D_BGD))
		{
			std::cout<<i<< "ith voxel not assign labels! "<<std::endl;
			return;
		}
   	}

	//参数后面可能需要调整
	//const double gamma = 50;
	//const double lambda = 9*gamma;
	const double gamma = 50;
	const double lambda = 6*gamma;

	int cnt=0;
    //const double beta = CalcBeta(volumeVoxels,edgeNumbers);
	std::vector<double> leftW, upW, frontW;
	CalcNWeights(volumeVoxels,leftW,upW,frontW,gamma,edgeNumbers);

  
	for( int i = 0; i < iterCount; i++ )
	{
		GCGraph<double> graph;
		ConstructGCGraph(volumeVoxels,lambda, leftW,upW, frontW, graph );
	    EstimateSegmentation( graph, volumeVoxels);
	}

 
	cv::Vec3d castColor(0,0,255);   //Red color
	cv::Vec3d maskColor(255,255,255); //White color

	//cast seedVoxels from 3D to 2D
	for(int camId=0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);

		//reset mask image(set zero to all elem)
		curFrame->m_mMaskImg=cv::imread(curFrame->m_sMaskImgPathName);
		curFrame->m_mMaskImg.setTo(cv::Scalar(0));                      //此处置0似乎没用

		//reset cast image(copy color image)
		if(!curFrame->m_mColorImg.empty())
			curFrame->m_mColorImg.copyTo(curFrame->m_mCastImg);
		else
		{
			std::cout<<"m_mColorImg is empty!"<<std::endl;
			return ;
		}

		for(int idx=0;idx<m_VoxelVolume.size();idx++)
		{
			if(m_VoxelVolume[idx]->flag==GC3D_FGD||m_VoxelVolume[idx]->flag==GC3D_PR_FGD)   
			{
				//m_VoxelVolume[idx]->flag=GC3D_FGD;   //new add
				cnt++;
				Wml::Vector3d wCoord=m_VoxelVolume[idx]->GetCenterCoord();
				double u,v,dsp;
				curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,wCoord);

				int tmpu=(int)u;
				int tmpv=(int)v;
				if( tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight()) )
				{
					//std::cout<<"cast out beyond edge of image"<<std::endl;
					continue;
				}
				else
				{
					curFrame->m_mMaskImg.at<cv::Vec3b>(tmpv,tmpu)=maskColor; //set value for(casted) mask

					//set color for cast image
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[0]=(castColor[0]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[0]*0.6;
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[1]=(castColor[1]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[1]*0.6;
					curFrame->m_mCastImg.at<cv::Vec3b>(tmpv,tmpu)[2]=(castColor[2]*0.4)+curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu)[2]*0.6;
				}
			}
		}
		cv::imwrite(curFrame->m_sMaskImgPathName,curFrame->m_mMaskImg); //write in mask image
		cv::imwrite(curFrame->m_sCastImgPathName,curFrame->m_mCastImg); //write in cast image
		std::cout<<"cnt:"<<cnt<<std::endl;
	}
   //colorModelVoxels暂未用到（即mask未用到）
	std::cout<<"Finish Execute3DGraphCut func..."<<std::endl;
}





void AVisualHull::Execute3DGraphCut1( cv::Mat _bgdModel,cv::Mat _fgdModel,int iterCount,bool isConverged )
{
	bool initialized=false;
	
	std::vector<AVoxel*>colorModelVoxels; //
	Wml::Vector3d minXYZ, maxXYZ;

	std::vector<AVoxel*>& volumeVoxels=m_VoxelVolume; //image
	colorModelVoxels=m_SeedVoxels;//mask 
	minXYZ=this->m_LowCorner; //rect
	maxXYZ=this->m_HighCorner; //rect

	cv::Mat& bgdModel=_bgdModel;  //bgdModel
	cv::Mat& fgdModel=_fgdModel;  //fgdModel

	//assert empty and no iterator
	if(volumeVoxels.empty())
	{
		std::cout<<"VolumeVoxels is empty!"<<std::endl;
		return;
	}
	/*
	else if (colorModelVoxels.empty())    //
	{
		std::cout<<"colorModelVoxels is empty!"<<std::endl;
		return;
	}
	*/
	else if(iterCount<0)
	{
		std::cout<<"iterCount for 3D-GraphCut is smaller than 0!";
		return;
	}
       else if(isConverged==true)
	{
		std::cout<<"VisualHull has converged!"<<std::endl;
		return;
	}

	GMM3D bgdGMM(bgdModel),fgdGMM(fgdModel);

	// Check Mask
	for(int i=0;i<volumeVoxels.size();i++)
	{
		if(!(volumeVoxels[i]->flag==GC3D_PR_FGD||volumeVoxels[i]->flag==GC_FGD||volumeVoxels[i]->flag==GC3D_PR_BGD||volumeVoxels[i]->flag==GC3D_BGD))
		{
			std::cout<<i<< "ith voxel not assign labels! "<<std::endl;
			return;
		}
   	}


	if(!initialized)
     InitGMMs(volumeVoxels,colorModelVoxels,bgdGMM,fgdGMM);


	//参数后面可能需要调整
	const double gamma = 50;
	const double lambda = 9*gamma;
	const double beta = CalcBeta(volumeVoxels,edgeNumbers);
	std::vector<double> leftW, upW, frontW;
	CalcNWeights(volumeVoxels,leftW,upW,frontW,beta,gamma);


	for( int i = 0; i < iterCount; i++ )
	{
		GCGraph<double> graph;
		AssignGMMsComponents(volumeVoxels,colorModelVoxels,bgdGMM,fgdGMM);
       
		LearnGMMs( volumeVoxels, colorModelVoxels, bgdGMM, fgdGMM );
		
		ConstructGCGraph(volumeVoxels, colorModelVoxels, bgdGMM, fgdGMM, lambda, leftW,upW, frontW, graph );
	
		EstimateSegmentation( graph, volumeVoxels);
	}
   //colorModelVoxels暂未用到（即mask未用到）
	std::cout<<"Finish Execute3DGraphCut func..."<<std::endl;

}

void AVisualHull::InitGMMs(const std::vector<AVoxel*>& volumeVoxels, const std::vector<AVoxel*>& maskVoxels,GMM3D& bgdGMM, GMM3D& fgdGMM )
{
	  cv::Mat bgdLabels, fgdLabels;
	  vector<cv::Vec3f> bgdSamples, fgdSamples;
	 for(int camId=0;camId<CamNum;camId++)
	 {
		 LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
		 curFrame->LoadColorImg();
	 }
	 
	 //sample points according to mask  //从各帧图像上采样颜色平均作为voxel的颜色
	 for(int i=0;i<volumeVoxels.size();i++)
	 {
		 Wml::Vector3d pt=volumeVoxels[i]->GetCenterCoord();
		 int count=0;
		 cv::Vec3f objColor;
		 for(int camId=0;camId<CamNum;camId++)
		 {
			 LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
			 double u,v,dsp;
			 curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,pt);
			// std::cout<<"u:"<<u<<",v:"<<v<<std::endl;
			 int tmpu=(int)u;
			 int tmpv=(int)v;
			 if( tmpu<0  ||  tmpv<0  || (tmpu + 1>=curFrame->GetImgWidth())  || (tmpv + 1>=curFrame->GetImgHeight()) )
			 {
				 //std::cout<<"cast out beyond edge of image"<<std::endl;
				;
			 }
			 else
			 {
				 Wml::Vector3f curColor;
				 curFrame->GetColorAt((int)u,(int)v,curColor);
				 // curFrame->GetColorAt((float)u,(float)v,curColor);   //此函数有bug!
				 // std::cout<<curColor[0]<<","<<curColor[1]<<","<<curColor[2]<<std::endl;
				 // if(curColor[0]<0||curColor[1]<0||curColor[2]<0)
				 //	std::cout<<camId<<std::endl;
				 count++;
				 (volumeVoxels[i]->color[0])+=curColor[0];
				 (volumeVoxels[i]->color[1])+=curColor[1];
				 (volumeVoxels[i]->color[2])+=curColor[2];
			 }
		 }
		 if(count>0)
		 {
			objColor[0]=volumeVoxels[i]->color[0]=(volumeVoxels[i]->color[0])/count;
			objColor[1]=volumeVoxels[i]->color[1]=(volumeVoxels[i]->color[1])/count;
			objColor[2]=volumeVoxels[i]->color[2]=(volumeVoxels[i]->color[2])/count;
		 }
		 else
		 {
			objColor[0]=volumeVoxels[i]->color[0]=0;
			objColor[1]=volumeVoxels[i]->color[1]=0;
			objColor[2]=volumeVoxels[i]->color[2]=0;
		 }

		// std::cout<<volumeVoxels[i]->color[0]<<","<<volumeVoxels[i]->color[1]<<","<<volumeVoxels[i]->color[2]<<std::endl;

		
		 //此处对应index,将VolumeVoxels中voxel顺序与fgdsample中color(fgd和bgd)对应起来。
		 if(volumeVoxels[i]->flag==GC3D_PR_FGD||volumeVoxels[i]->flag==GC3D_FGD)
		 {
			 fgdSamples.push_back((cv::Vec3f)(objColor));
			 //volumeVoxels[i]->smplIdx=fgdSamples.size()-1;// 新加，不一定有用
		 }

		 else if(volumeVoxels[i]->flag==GC3D_PR_BGD||volumeVoxels[i]->flag==GC3D_BGD)
		 {
			 bgdSamples.push_back((cv::Vec3f)(objColor));
		     //volumeVoxels[i]->smplIdx=fgdSamples.size()-1;  //新加，不一定有用
		 }
	 }
	 
	 std::cout<<"fgdSamples.size():"<<fgdSamples.size()<<",bgdSamples.size():"<<bgdSamples.size()<<std::endl;

	 const int kMeansItCount = 10;
	 const int kMeansType = KMEANS_PP_CENTERS;
	 //using k-means cluster
	 CV_Assert( !bgdSamples.empty() && !fgdSamples.empty() );
	 Mat _bgdSamples( (int)bgdSamples.size(), 3, CV_32FC1, &bgdSamples[0][0] );
	 kmeans( _bgdSamples, GMM::componentsCount, bgdLabels,
		 TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );

	 Mat _fgdSamples( (int)fgdSamples.size(), 3, CV_32FC1, &fgdSamples[0][0] );
	 kmeans( _fgdSamples, GMM::componentsCount, fgdLabels,
		 TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );
	


	 //在这个过程中需要将Label（前景或者背景）与voxel对应起来；即每一个voxel都有一个componetId
	 bgdGMM.InitLearning(); //set all zero
	 for( int i = 0; i < (int)bgdSamples.size(); i++ )
		 bgdGMM.AddSample( bgdLabels.at<int>(i,0), bgdSamples[i] );
	  bgdGMM.EndLearning();


	 fgdGMM.InitLearning();
	 for( int i = 0; i < (int)fgdSamples.size(); i++ )
		 fgdGMM.AddSample( fgdLabels.at<int>(i,0), fgdSamples[i] );
	 fgdGMM.EndLearning();

       std::cout<<"InitGMM Model"<<std::endl;
}



 double AVisualHull::CalcBeta( const std::vector<AVoxel*>& volumeVoxels,int& edgeNum )
{
    double beta = 0;
	int vId=0;    //current voxel index
	int vNbyId=0; //nearby voxel index
	int cnt1=0;
	for(int xId=0;xId<m_iCellNumInX;xId++)
	{
		for(int yId=0;yId<m_iCellNumInY;yId++)
		{
			for(int zId=0;zId<m_iCellNumInZ;zId++)
			{
				vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
				cv::Vec3d color=(cv::Vec3d)volumeVoxels[vId]->color;
				
				if( xId>0 ) // left
				{
					vNbyId=zId+(yId+(xId-1)*m_iCellNumInY)*m_iCellNumInZ;
					//Vec3d diff = color - (Vec3d)img.at<Vec3b>(y,x-1);
					Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
					beta += diff.dot(diff);
					cnt1++;
				}
				if( yId>0 ) // up
				{
					vNbyId=zId+((yId-1)+xId*m_iCellNumInY)*m_iCellNumInZ;
					//Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x-1);
					Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
					beta += diff.dot(diff);
					cnt1++;
				}
				if( zId>0 ) //front
				{
					vNbyId=(zId-1)+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
					//Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x);
					Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
					beta += diff.dot(diff);
					cnt1++;
				}

			}
		}
	}
	//cnt2=(3*m_iCellNumInX*m_iCellNumInY*m_iCellNumInZ-(m_iCellNumInY*m_iCellNumInZ+m_iCellNumInZ*m_iCellNumInX+m_iCellNumInX*m_iCellNumInY)+m_iCellNumInX+m_iCellNumInY+m_iCellNumInZ);

	if(beta<=std::numeric_limits<double>::epsilon())
		beta=0;
	else
	{
		//beta = 1.f / (2 * beta/(4*img.cols*img.rows - 3*img.cols - 3*img.rows + 2) );
		//beta = 1.f/(2*beta/(3*m_iCellNumInX*m_iCellNumInY*m_iCellNumInZ-(m_iCellNumInY*m_iCellNumInZ+m_iCellNumInZ*m_iCellNumInX+m_iCellNumInX*m_iCellNumInY)+m_iCellNumInX+m_iCellNumInY+m_iCellNumInZ));//约等于
		//double beta1=1.f/(2*beta/cnt1);
		//double beta2=1.f/(2*beta/cnt2);
		 beta=1.f/(2*beta/cnt1);
		//std::cout<<"beta1:"<<beta1<<",beta2:"<<beta2<<std::endl;
	 }
	edgeNum=cnt1;
	//std::cout<<"cnt1:"<<cnt1<<std::endl;
	//std::cout<<"cnt2:"<<cnt2<<std::endl;
    return beta;
}

 //Paper mehod
 void AVisualHull::CalcNWeights( const std::vector<AVoxel*>& volumeVoxels, std::vector<double>& leftW, std::vector<double>& upW, std::vector<double>& frontW,double gamma,int &edgeNum)
 {
	 gamma=1;
	 leftW.resize(volumeVoxels.size());
	 upW.resize(volumeVoxels.size());
	 frontW.resize(volumeVoxels.size());
	 int edgeCnt=0;
	 int vId=0;
	 int vNbyId=0;
	 for(int xId=0;xId<m_iCellNumInX;xId++)
	 {
		 for(int yId=0;yId<m_iCellNumInY;yId++)
		 {
			 for(int zId=0;zId<m_iCellNumInZ;zId++)
			 {
				 vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
				
				  /*
				 if( xId>0 ) // left
				 {
					 vNbyId=zId+(yId+(xId-1)*m_iCellNumInY)*m_iCellNumInZ;
					 //Vec3d diff = color - (Vec3d)img.at<Vec3b>(y,x-1);
					 Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
					 //leftW.at<double>(y,x) = gamma * exp(-beta*diff.dot(diff));
					 leftW[vId]=gamma*exp(-beta*diff.dot(diff));
				 }
				 else
					 leftW[vId]=0;
                 */
				 if( xId>0 ) // left
				 {
					 double aslDiff=0.0; //max color difference square 
					 int mCamId=0;  // camId when taken max color difference
					 vNbyId=zId+(yId+(xId-1)*m_iCellNumInY)*m_iCellNumInZ;
					 for(int camId=0;camId<CamNum;camId++)
					 {
						 LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
						 cv::Vec3d curColor(0,0,0);
						 Wml::Vector3d wCurCoord=m_VoxelVolume[vId]->GetCenterCoord();
						 double u,v,dsp;
						 curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,wCurCoord);
						 int tmpu=(int)u;
						 int tmpv=(int)v;
						 if( tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight()) )
						 {
							 //std::cout<<"cast out beyond edge of image"<<std::endl;
							 continue;
						 }
						 else
						 {
							 curColor=curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu);
						 }

						 cv::Vec3d nbyColor(0,0,0);
						 Wml::Vector3d wNbrCoord=m_VoxelVolume[vNbyId]->GetCenterCoord();
						 double u1,v1,dsp1;
						 curFrame->GetImgCoordFrmWorldCoord(u1,v1,dsp1,wNbrCoord);
						 int tmpu1=(int)u1;
						 int tmpv1=(int)v1;
						 if( tmpu1<0  ||  tmpv1<0  || (tmpu1>=curFrame->GetImgWidth())  || (tmpv1>=curFrame->GetImgHeight()) )
						 {
							// std::cout<<"cast out beyond edge of image"<<std::endl;
							 continue;
						 }
						 else
						 {
							 nbyColor=curFrame->m_mColorImg.at<cv::Vec3b>(tmpv1,tmpu1);
						 }

						 cv::Vec3d diff=0.0;
						 if(tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight())||
							 tmpu1<0  ||  tmpv1<0  || (tmpu1>=curFrame->GetImgWidth())  || (tmpv1>=curFrame->GetImgHeight()))
						 {
							 diff=0.0;	    //对于投出边界的相邻voxel的N-weight计算可能不准确
						 }
						 else
						 {
						     diff=curColor-nbyColor;       //此处可以尝试用灰度图像作差。
						 }

						 if(aslDiff<diff.dot(diff))      
						 {
							 aslDiff=diff.dot(diff);
							 mCamId=camId;
						 }
					 }
					 //leftW[vId]=gamma*exp(-beta*diff.dot(diff));
					leftW[vId]=gamma*exp(-m_vBetas[mCamId]*aslDiff);

					edgeCnt++;
				 }
				 else
					 leftW[vId]=0;


				 /*
				 if( yId>0 ) // up
				 {
				 vNbyId=zId+((yId-1)+xId*m_iCellNumInY)*m_iCellNumInZ;
				 //Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x-1);
				 Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
				 upW[vId]=gamma*exp(-beta*diff.dot(diff));

				 }
				 else
				 upW[vId]=0;
				 */

				 if( yId>0 ) // up
				 {
					 double aslDiff=0.0; //max color difference square 
					 int mCamId=0;  // camId when taken max color difference
					 vNbyId=zId+((yId-1)+xId*m_iCellNumInY)*m_iCellNumInZ;

					 for(int camId=0;camId<CamNum;camId++)
					 {
						 LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
						 cv::Vec3d curColor(0,0,0);
						 Wml::Vector3d wCurCoord=m_VoxelVolume[vId]->GetCenterCoord();
						 double u,v,dsp;
						 curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,wCurCoord);
						 int tmpu=(int)u;
						 int tmpv=(int)v;
						 if( tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight()) )
						 {
							 //std::cout<<"cast out beyond edge of image"<<std::endl;
							 continue;
						 }
						 else
						 {
							 curColor=curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu);
						 }

						 cv::Vec3d nbyColor(0,0,0);
						 Wml::Vector3d wNbrCoord=m_VoxelVolume[vNbyId]->GetCenterCoord();
						 double u1,v1,dsp1;
						 curFrame->GetImgCoordFrmWorldCoord(u1,v1,dsp1,wNbrCoord);
						 int tmpu1=(int)u1;
						 int tmpv1=(int)v1;
						 if( tmpu1<0  ||  tmpv1<0  || (tmpu1>=curFrame->GetImgWidth())  || (tmpv1>=curFrame->GetImgHeight()) )
						 {
							 // std::cout<<"cast out beyond edge of image"<<std::endl;
							 continue;
						 }
						 else
						 {
							 nbyColor=curFrame->m_mColorImg.at<cv::Vec3b>(tmpv1,tmpu1);
						 }

						 cv::Vec3d diff=0.0;
						 if(tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight())||
							 tmpu1<0  ||  tmpv1<0  || (tmpu1>=curFrame->GetImgWidth())  || (tmpv1>=curFrame->GetImgHeight()))
						 {
							 diff=0.0;	    //对于投出边界的相邻voxel的N-weight计算可能不准确
						 }
						 else
						 {
							 diff=curColor-nbyColor;
						 }

						 if(aslDiff<diff.dot(diff))
						 {
							 aslDiff=diff.dot(diff);
							 mCamId=camId;
						 }
					 }
					 //upW[vId]=gamma*exp(-beta*diff.dot(diff));
					 upW[vId]=gamma*exp(-m_vBetas[mCamId]*aslDiff);

					 edgeCnt++;
				 }
				 else
					 upW[vId]=0;
                
				 /*
				 if( zId>0 ) //front
				 {
					 vNbyId=(zId-1)+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
					 //Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x);
					 Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
					 frontW[vId]=gamma*exp(-beta*diff.dot(diff));
				 }
				 else
					 frontW[vId]=0;
				 */

				 if( zId>0 ) // front
				 {
					 double aslDiff=0.0; //max color difference square 
					 int mCamId=0;  // camId when taken max color difference
					  vNbyId=(zId-1)+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;

					 for(int camId=0;camId<CamNum;camId++)
					 {
						 LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
						 cv::Vec3d curColor(0,0,0);
						 Wml::Vector3d wCurCoord=m_VoxelVolume[vId]->GetCenterCoord();
						 double u,v,dsp;
						 curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,wCurCoord);
						 int tmpu=(int)u;
						 int tmpv=(int)v;
						 if( tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight()) )
						 {
							 //std::cout<<"cast out beyond edge of image"<<std::endl;
							 continue;
						 }
						 else
						 {
							 curColor=curFrame->m_mColorImg.at<cv::Vec3b>(tmpv,tmpu);
						 }

						 cv::Vec3d nbyColor(0,0,0);
						 Wml::Vector3d wNbrCoord=m_VoxelVolume[vNbyId]->GetCenterCoord();
						 double u1,v1,dsp1;
						 curFrame->GetImgCoordFrmWorldCoord(u1,v1,dsp1,wNbrCoord);
						 int tmpu1=(int)u1;
						 int tmpv1=(int)v1;
						 if( tmpu1<0  ||  tmpv1<0  || (tmpu1>=curFrame->GetImgWidth())  || (tmpv1>=curFrame->GetImgHeight()) )
						 {
							 // std::cout<<"cast out beyond edge of image"<<std::endl;
							 continue;
						 }
						 else
						 {
							 nbyColor=curFrame->m_mColorImg.at<cv::Vec3b>(tmpv1,tmpu1);
						 }

						 cv::Vec3d diff=0.0;
						 if(tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight())||
							 tmpu1<0  ||  tmpv1<0  || (tmpu1>=curFrame->GetImgWidth())  || (tmpv1>=curFrame->GetImgHeight()))
						 {
							 diff=0.0;	    //对于投出边界的相邻voxel的N-weight计算可能不准确
						 }
						 else
						 {
							 diff=curColor-nbyColor;
						 }

						 if(aslDiff<diff.dot(diff))
						 {
							 aslDiff=diff.dot(diff);
							 mCamId=camId;
						 }
					 }
					 //frontW[vId]=gamma*exp(-beta*diff.dot(diff));
					 frontW[vId]=gamma*exp(-m_vBetas[mCamId]*aslDiff);

					 edgeCnt++;
				 }
				 else
					 frontW[vId]=0;

			 }
		 }
	 }
	 int cnt2=3*m_iCellNumInX*m_iCellNumInY*m_iCellNumInZ-m_iCellNumInX*m_iCellNumInY-m_iCellNumInY*m_iCellNumInZ-m_iCellNumInX*m_iCellNumInZ;
	 edgeNum=edgeCnt;
	 std::cout<<"edgeNum:"<<edgeNum<<std::endl;
	 std::cout<<"edgeNum2:"<<cnt2<<std::endl;
	 std::cout<<"Finish calculate N weights.."<<std::endl;
 }


 // Average color method
 void AVisualHull::CalcNWeights( const std::vector<AVoxel*>& volumeVoxels, std::vector<double>& leftW, std::vector<double>& upW, std::vector<double>& frontW, double beta, double gamma )
 {
	 //const double gammaDivSqrt2 = gamma / std::sqrt(2.0f);
	 leftW.resize(volumeVoxels.size());
	 upW.resize(volumeVoxels.size());
	 frontW.resize(volumeVoxels.size());

	 int vId=0;
	 int vNbyId=0;
	 for(int xId=0;xId<m_iCellNumInX;xId++)
	 {
		 for(int yId=0;yId<m_iCellNumInY;yId++)
		 {
			 for(int zId=0;zId<m_iCellNumInZ;zId++)
			 {
				 vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
				 cv::Vec3d color=(cv::Vec3d)volumeVoxels[vId]->color;

				 if( xId>0 ) // left
				 {
					 vNbyId=zId+(yId+(xId-1)*m_iCellNumInY)*m_iCellNumInZ;
					 //Vec3d diff = color - (Vec3d)img.at<Vec3b>(y,x-1);
					 Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
					 //leftW.at<double>(y,x) = gamma * exp(-beta*diff.dot(diff));
					 leftW[vId]=gamma*exp(-beta*diff.dot(diff));
				 }
				 else
					 leftW[vId]=0;

				 if( yId>0 ) // up
				 {
					 vNbyId=zId+((yId-1)+xId*m_iCellNumInY)*m_iCellNumInZ;
					 //Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x-1);
					 Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
					 upW[vId]=gamma*exp(-beta*diff.dot(diff));
					
				 }
				 else
					 upW[vId]=0;

				 if( zId>0 ) //front
				 {
					 vNbyId=(zId-1)+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
					 //Vec3d diff = color - (Vec3d)img.at<Vec3b>(y-1,x);
					 Vec3d diff = color - (Vec3d)volumeVoxels[vNbyId]->color;
					 frontW[vId]=gamma*exp(-beta*diff.dot(diff));
				 }
				 else
					 frontW[vId]=0;

			 }
		 }
	 }
	 std::cout<<"Finish calculate N weights.."<<std::endl;
 }

 void AVisualHull::AssignGMMsComponents( const std::vector<AVoxel*>& volumeVoxels, const std::vector<AVoxel*>& maskVoxels, const GMM3D& bgdGMM, const GMM3D& fgdGMM/*, Mat& compIdxs */)
 {
	 int vId=0;
	 for(int xId=0;xId<m_iCellNumInX;xId++)
	 {
		 for(int yId=0;yId<m_iCellNumInY;yId++)
		 {
			 for(int zId=0;zId<m_iCellNumInZ;zId++)
			 {
				 vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
				 cv::Vec3d color=(cv::Vec3d)volumeVoxels[vId]->color;
				 volumeVoxels[vId]->compId=volumeVoxels[vId]->flag==GC_BGD||volumeVoxels[vId]->flag==GC_PR_BGD?
					 bgdGMM.WhichComponent(color):fgdGMM.WhichComponent(color);					 
			 }
		 }
	 }
 }

 void AVisualHull::LearnGMMs( const std::vector<AVoxel*>&volumeVoxels, const std::vector<AVoxel*>& maskVoxels,GMM3D& bgdGMM, GMM3D& fgdGMM)
 {
    bgdGMM.InitLearning();
    fgdGMM.InitLearning();
	
  for( int ci = 0; ci < GMM::componentsCount; ci++ )
  {
	int vId=0;
	for(int xId=0;xId<m_iCellNumInX;xId++)
	{
		for(int yId=0;yId<m_iCellNumInY;yId++)
		{
			for(int zId=0;zId<m_iCellNumInZ;zId++)
			{
				vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
				cv::Vec3d color=(cv::Vec3d)volumeVoxels[vId]->color;

				if(volumeVoxels[vId]->compId==ci)
				{
				  if(volumeVoxels[vId]->flag==GC_BGD||volumeVoxels[vId]->flag==GC_PR_BGD)
					bgdGMM.AddSample(ci,color);
				  else
				    fgdGMM.AddSample(ci,color);
				}
			}
		}
	}
  }
    bgdGMM.EndLearning();
    fgdGMM.EndLearning();
}

 //M2:Paper Method
 void AVisualHull::ConstructGCGraph( const std::vector<AVoxel*>&volumeVoxels,double lambda,
	 const std::vector<double>& leftW, const std::vector<double>& upW, const std::vector<double>& frontW,GCGraph<double>& graph )
 {
	 int vtxCount = volumeVoxels.size();  //231694  
	 int edgeCount =edgeNumbers;          //682789

	 std::cout<<"vtxCount: "<<vtxCount<<std::endl;
	 std::cout<<"edgeCount: "<<edgeCount<<std::endl;

	 graph.create(vtxCount, edgeCount);

	 int vId=0,vNbyId=0;
	
	 for(int xId=0;xId<m_iCellNumInX;xId++)
	 {
		 for(int yId=0;yId<m_iCellNumInY;yId++)
		 {
			 for(int zId=0;zId<m_iCellNumInZ;zId++)
			 {
				
				 vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
				
		
				 ////===================================================================================
				 ////Add Voxels which cast in mask as Fgd voxel
				 // bool castInMask=false;  // weather voxel are casted in the mask
				 //if( volumeVoxels[vId]->flag == GC3D_PR_BGD || volumeVoxels[vId]->flag == GC3D_PR_FGD )
				 //{
					// for(int camId=0;camId<CamNum;camId++)
					// {
					//	 LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
					//	 curFrame->LoadMaskImg();   //需要判断是否加载最新的mask
					//	 Wml::Vector3d wCurCoord=m_VoxelVolume[vId]->GetCenterCoord();
					//	 double u,v,dsp;
					//	 curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,wCurCoord);
					//	 int tmpu=(int)u;
					//	 int tmpv=(int)v;
					//	 if( tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight()) )
					//	 {
					//		 //std::cout<<"cast out beyond edge of image"<<std::endl;
					//		 continue;                                                   //投出图像边缘的vovel暂未处理，可能有问题
					//	 }
					//	 else
					//	 {
					//		 cv::Point p(tmpu,tmpv);
					//		 if(curFrame->IsInMask(tmpu,tmpv))
					//		 castInMask=true;
					//		
					//	 }

					// }

					// if(castInMask==true)
					//	 volumeVoxels[vId]->flag=GC3D_FGD;	
				 //}
     //           //===================================================================================
				

				 // add node
				 int vtxIdx = graph.addVtx();

				 // set t-weights
				 double fromSource, toSink;
				 if( volumeVoxels[vId]->flag == GC3D_PR_BGD || volumeVoxels[vId]->flag == GC3D_PR_FGD )
				 {
					 double meanSource=0.0;
					 int cnt=0;
					 double threshold=0.87; // 0.8~0.9 is range

					 for(int camId=0;camId<CamNum;camId++)
					{
						 LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
						// curFrame->LoadMaskImg();   //需要判断是否加载最新的mask
						 Wml::Vector3d wCurCoord=volumeVoxels[vId]->GetCenterCoord();
						 double u,v,dsp;
						 curFrame->GetImgCoordFrmWorldCoord(u,v,dsp,wCurCoord);
						 int tmpu=(int)u;
						 int tmpv=(int)v;
						 if( tmpu<0  ||  tmpv<0  || (tmpu>=curFrame->GetImgWidth())  || (tmpv>=curFrame->GetImgHeight()) )
						 {
							 //std::cout<<"cast out beyond edge of image"<<std::endl;
							 continue;                                                   //投出图像边缘的vovel暂未处理，可能有问题
						 }
						 else
						 {
							 cv::Point p(tmpu,tmpv);
							// if(curFrame->IsInMask(tmpu,tmpv))
							// m_vProbFgdImgs[camId].at<float>(p)*=10;
							 meanSource+=m_vProbFgdImgs[camId].at<float>(p);
							 cnt++;
						 }

					  }
					 
					 if(cnt>0)
					 {
						  fromSource =meanSource/cnt;
						 //toSink = 1-fromSource;    //需要验证：对于prb_fgd和prb_bgd,fromSource+toSink是否等于1！

						  //fromSource=1-(fromSource-threshold);
						  //toSink=1+(fromSource-threshold);
						  fromSource=1+(fromSource-threshold);
						  toSink=1-(fromSource-threshold);
						 //std::cout<<"fromSource:"<<fromSource<<std::endl;					     //std::cout<<"toSink:"<<toSink<<std::endl;
					 }
					 else  //改voxel在各帧上全部投出去的情况，尚不确定这样计算是否正确！
					 {
					      fromSource = 0;
					      toSink = lambda;
					 }
					 
					 //std::cout<<"fromSource: "<<fromSource<<std::endl;
					 //std::cout<<"toSink: "<<toSink<<std::endl;
				 }
				 else if( volumeVoxels[vId]->flag== GC3D_BGD )
				 {
					 fromSource = 0;
					 toSink = lambda;
					
				 }
				 else // GC_FGD
				 {
					 fromSource = lambda;
					 toSink = 0;
				 }
				 graph.addTermWeights( vtxIdx, fromSource, toSink );
				

				 // set n-weights
				 if( xId>0 )
				 {
					 double w = leftW.at(vId);
					 vNbyId=zId+(yId+(xId-1)*m_iCellNumInY)*m_iCellNumInZ;
					 graph.addEdges( vtxIdx, vNbyId, w, w );
				 }
				 if( yId>0 )
				 {
					 double w = upW.at(vId);
					 vNbyId=zId+((yId-1)+xId*m_iCellNumInY)*m_iCellNumInZ;
					 graph.addEdges( vtxIdx, vNbyId, w, w );
				 }
				 if( zId>0 )
				 {
					 double w = frontW.at(vId);
					 vNbyId=(zId-1)+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
					 graph.addEdges( vtxIdx, vNbyId, w, w );
				 }

			 }
		 }
	 }

	 std::cout<<"finish construct 3D graph..."<<std::endl;

 }


//Average color method
 void AVisualHull::ConstructGCGraph( const std::vector<AVoxel*>&volumeVoxels, const std::vector<AVoxel*>& maskVoxels, const GMM3D& bgdGMM, const GMM3D& fgdGMM, double lambda,
	 const std::vector<double>& leftW, const std::vector<double>& upW, const std::vector<double>& frontW,GCGraph<double>& graph )
 {
	 int vtxCount = volumeVoxels.size();  //231694  
	 int edgeCount =edgeNumbers;          //682789
	 graph.create(vtxCount, edgeCount);

	 int vId=0,vNbyId=0;
	 for(int xId=0;xId<m_iCellNumInX;xId++)
	 {
		 for(int yId=0;yId<m_iCellNumInY;yId++)
		 {
			 for(int zId=0;zId<m_iCellNumInZ;zId++)
			 {
				 vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;

				 // add node
				 int vtxIdx = graph.addVtx();
				 cv::Vec3d color=(cv::Vec3d)volumeVoxels[vId]->color;

				 // set t-weights
				 double fromSource, toSink;
				 if( volumeVoxels[vId]->flag == GC_PR_BGD || volumeVoxels[vId]->flag == GC_PR_FGD )
				 {
					 fromSource = -log( bgdGMM(color) );
					 toSink = -log( fgdGMM(color) );
					 //std::cout<<"fromSource: "<<fromSource<<std::endl;
					 //std::cout<<"toSink: "<<toSink<<std::endl;
				 }
				 else if( volumeVoxels[vId]->flag== GC_BGD )
				 {
					 fromSource = 0;
					 toSink = lambda;
				 }
				 else // GC_FGD
				 {
					 fromSource = lambda;
					 toSink = 0;
				 }
				 graph.addTermWeights( vtxIdx, fromSource, toSink );

				 // set n-weights
				 if( xId>0 )
				 {
					 double w = leftW.at(vId);
					 vNbyId=zId+(yId+(xId-1)*m_iCellNumInY)*m_iCellNumInZ;
					 graph.addEdges( vtxIdx, vNbyId, w, w );
				 }
				 if( yId>0 )
				 {
					 double w = upW.at(vId);
					 vNbyId=zId+((yId-1)+xId*m_iCellNumInY)*m_iCellNumInZ;
					 graph.addEdges( vtxIdx, vNbyId, w, w );
				 }
				 if( zId>0 )
				 {
					 double w = frontW.at(vId);
					 vNbyId=(zId-1)+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;
					 graph.addEdges( vtxIdx, vNbyId, w, w );
				 }
			 }
		 }
	 }
	
 }



 void AVisualHull::EstimateSegmentation(GCGraph<double>& graph,const std::vector<AVoxel*>&volumeVoxels)
 {
	 graph.maxFlow();
	
	 int vId=0;
	 for(int xId=0;xId<m_iCellNumInX;xId++)
	 {
		 for(int yId=0;yId<m_iCellNumInY;yId++)
		 {
			 for(int zId=0;zId<m_iCellNumInZ;zId++)
			 {
				 vId=zId+(yId+xId*m_iCellNumInY)*m_iCellNumInZ;

				 if( volumeVoxels[vId]->flag == GC3D_PR_BGD || volumeVoxels[vId]->flag == GC3D_PR_FGD )
				 {
					 if( graph.inSourceSegment( vId /*vertex index*/ ) )
						 volumeVoxels[vId]->flag = GC3D_PR_FGD;
					 else
						 volumeVoxels[vId]->flag = GC3D_PR_BGD;
				 }

			 }
		 }
	 }
 }


  void AVisualHull::Execute2DGrabCut()
 {	 
	//std::vector<Mat> vProbFgdImgs;
	 if(!m_vProbFgdImgs.empty())
	 {
		 for(int i=0;i<m_vProbFgdImgs.size();i++)
		 {
			 if(!m_vProbFgdImgs[i].empty())
				 m_vProbFgdImgs[i].release();
		 }
		 m_vProbFgdImgs.clear();
	 }

	 if(!m_vBetas.empty())
	 {
		 m_vBetas.clear();
	 }

	//std::cout<<"m_vProbFgdImgs.size():"<<m_vProbFgdImgs.size()<<std::endl;
	// std::cout<<"m_vBetas.size():"<<m_vBetas.size()<<std::endl;
	
	//从三维点数目7929递减到二维的5629估计不是程序的问题，而是有些不同的三维点投影到二维点，再经过Int强制转换投射到同一二维点上了。
	for(int camId=0;camId<CamNum;camId++)
	{
		    LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
			cv::Mat image,mask;  //此mask为RGB三通道mask.
			Mat bgdModel;
			Mat fgdModel;

			//color image
			if(curFrame->m_mColorImg.empty())
			{
				std::cout<<"colorImg is empty!"<<std::endl;
				return ;
			}
			else
			{
				curFrame->m_mColorImg.copyTo(image);
			}

			if( image.type() != CV_8UC3 )
				CV_Error( CV_StsBadArg, "image must have CV_8UC3 type" );

		    //因为mask在不断更新中，因此不同于image(只需读取一次）
			mask=cv::imread(curFrame->m_sMaskImgPathName);
			if(mask.empty())
			{
				std::cout<<"mask image is empty!"<<std::endl;
				return ;
			}

			cv::Mat maskImg;    //此maskImg为Grabcut中参数和保存结果的mask（单通道）
			maskImg.create( image.size(), CV_8UC1);

			//imgWidth& imgHeight
			int imgWidth=curFrame->GetImgWidth();
			int imgHeight=curFrame->GetImgHeight();
			//rect
			int minX = imgWidth;
			int minY = imgHeight;
			int maxX = 0;
			int maxY = 0;
			curFrame->GetMaskRectangle(minX,minY,maxX,maxY); // may bug!
			cv::Rect rect(minX,minY,maxX-minX,maxY-minY);

			//Set rect in mask (init labels with rect)
			assert( !maskImg.empty() );
			maskImg.setTo( GC_BGD );
			rect.x = max(0, rect.x);
			rect.y = max(0, rect.y);
			rect.width = min(rect.width, image.cols-rect.x);
			rect.height = min(rect.height, image.rows-rect.y);
			(maskImg(rect)).setTo( Scalar(GC_PR_FGD) );

			//assign to mask (init labels with mask, mainly refers to FgdPixels)
			for( int y=minY;y<maxY;y++)
			{
				for(int x=minX;x<maxX;x++)
				{
					Vec3d color = mask.at<Vec3b>(y,x);  
					double val=(color[0]+color[1]+color[2])/3.0;
					if(val>0)
					{
						maskImg.at<uchar>(y,x)=GC_FGD;
					}
				}
			}

			GMM bgdGMM(bgdModel),fgdGMM(fgdModel); //initialize model param(covariance,mean,ci etc.)
			Mat compIdxs( image.size(), CV_32SC1 );
			Mat probFgdImg(image.size(),CV_32FC1);

			GMM::CheckMask( image, maskImg );// GC_INIT_WITH_MASK
			std::cout<<"Init GMM "<<camId<<" Frame..."<<std::endl;
		    GMM::InitGMMs( image, maskImg, bgdGMM, fgdGMM );//calculate model param(covariance,coefs, prods etc.)

			const double gamma = 50;
			const double lambda = 9*gamma;
			const double beta = GMM::CalcBeta( image );
			m_vBetas.push_back(beta);

			cv::Mat leftW, upleftW, upW, uprightW;
			GMM::CalcNWeights( image, leftW, upleftW, upW, uprightW, beta, gamma );

			for( int i = 0; i < 1; i++ )
			{
				GCGraph<double> graph;
				GMM::AssignGMMsComponents( image, maskImg, bgdGMM, fgdGMM, compIdxs );  
				GMM::LearnGMMs( image, maskImg, compIdxs, bgdGMM, fgdGMM );
				
				
				//输出可能的前景/背景概率
				cv::Point p;
				for( p.y = 0; p.y < image.rows; p.y++ )
				{
					for( p.x = 0; p.x < image.cols; p.x++)
					{
						// add node
						Vec3b color = image.at<Vec3b>(p);

						// set t-weights
						double fromSource, toSink;
						double probFgd=0.0;
						if( maskImg.at<uchar>(p) == GC_PR_BGD || maskImg.at<uchar>(p) == GC_PR_FGD)
						{
							fromSource = -log( bgdGMM(color) );
							toSink = -log( fgdGMM(color) );
	                      //  std::cout<<"fromSource:"<<fromSource<<std::endl;
						  //  std::cout<<"toSink:"<<toSink<<std::endl;
						  //可以尝试如果 fromSource> toSink.然后将相应的点投到maskImg上看结果。
						}
						else if( maskImg.at<uchar>(p) == GC_BGD )
						{
							fromSource = 0;
							toSink = lambda;
						}
						else // GC_FGD
						{
							fromSource = lambda;
							toSink = 0;
						}
						probFgd=fromSource/(fromSource+toSink);
						//std::cout<<"fgdProb: "<<probFgd<<std::endl;
						probFgdImg.at<float>(p)=probFgd;
						//std::cout<<probFgdImg.at<float>(p)<<std::endl;
					}
				}

			
			
				
				//GMM::ConstructGCGraph(image, maskImg, bgdGMM, fgdGMM, lambda, leftW, upleftW, upW, uprightW, graph );
				//GMM::EstimateSegmentation( graph, maskImg );
			}

			/*
			//Show image and write mask in disk
			Mat res;
			Mat binMask;
			//if image have been reset, then copy it
			//else copy the processed image
			if( maskImg.empty() || maskImg.type()!=CV_8UC1 )
				CV_Error( CV_StsBadArg, "maskImg is empty or has incorrect type (not CV_8UC1)" );
			if( binMask.empty() || binMask.rows!=maskImg.rows || binMask.cols!=maskImg.cols )
				binMask.create( maskImg.size(), CV_8UC1 );
			binMask = maskImg & 1;
			image.copyTo( res, binMask );

			//cv::imshow(winName,res);

			for(int y=0;y<imgHeight;y++)
			{
				for(int x=0;x<imgWidth;x++)
				{
					cv::Point p;
					p.x=x;
					p.y=y;
					maskImg.at<uchar>(y,x)=(double)maskImg.at<uchar>(y,x)*(255.0/3.0);
					//probFgdImg.at<float>(p)=probFgdImg.at<float>(p)*255;
						
				}
			}
			cv::imwrite(curFrame->m_sMaskImgPathName,maskImg);
		    //cv::imwrite(curFrame->m_sMaskImgPathName,probFgdImg);
		//}
         */
           m_vProbFgdImgs.push_back(probFgdImg);
	}
	 std::cout<<"Execute2DGrabCut..."<<std::endl;
 }


 void AVisualHull::Execute2DGrabCut1()
 {
	//从三维点数目7929递减到二维的5629估计不是程序的问题，而是有些不同的三维点投影到二维点，再经过Int强制转换投射到同一二维点上了。
	for(int camId=0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame=m_pMotion->GetFrameAt(camId,m_iCurFrame);
		if(camId==0)    
		{
			cv::Mat image,mask;  //此mask为RGB三通道mask.
			
			Mat bgdModel, fgdModel;  
			//color image
			if(curFrame->m_mColorImg.empty())
			{
				std::cout<<"colorImg is empty!"<<std::endl;
				return ;
			}
			else
			{
				curFrame->m_mColorImg.copyTo(image);
			}

			if( image.type() != CV_8UC3 )
				CV_Error( CV_StsBadArg, "image must have CV_8UC3 type" );

			//mask image
			if(curFrame->m_mMaskImg.empty())
			{
				std::cout<<"mask image is empty!"<<std::endl;
				return ;
			}
			else
			{
				curFrame->m_mMaskImg.copyTo(mask);
			}

			cv::Mat maskImg;    //此maskImg为Grabcut和保存结果的mask
			maskImg.create( image.size(), CV_8UC1);

			//imgWidth& imgHeight
			int imgWidth=curFrame->GetImgWidth();
			int imgHeight=curFrame->GetImgHeight();
			//rect
			int minX = imgWidth;
			int minY = imgHeight;
			int maxX = 0;
			int maxY = 0;
			curFrame->GetMaskRectangle(minX,minY,maxX,maxY);
			cv::Rect rect(minX,minY,maxX-minX,maxY-minY);

			//define and show windows
			//const string winName = "image";
			//cv::namedWindow( winName, cv::WINDOW_AUTOSIZE );

			//Set rect in mask (init labels with rect)
			assert( !maskImg.empty() );
			maskImg.setTo( GC_BGD );
			rect.x = max(0, rect.x);
			rect.y = max(0, rect.y);
			rect.width = min(rect.width, image.cols-rect.x);
			rect.height = min(rect.height, image.rows-rect.y);
			(maskImg(rect)).setTo( Scalar(GC_PR_FGD) );

			//assign to mask (init labels with mask, mainly refers to FgdPixels)
			for( int y=minY;y<maxY;y++)
			{
				for(int x=minX;x<maxX;x++)
				{
					Vec3d color = mask.at<Vec3b>(y,x);  
					double val=(color[0]+color[1]+color[2])/3.0;
					if(val>0)
					{
						maskImg.at<uchar>(y,x)=GC_FGD;
					}
				}
			}

			GMM bgdGMM(bgdModel),fgdGMM(fgdModel); //initialize model param(covariance,mean,ci etc.)
			Mat compIdxs( image.size(), CV_32SC1 );
			Mat probFgdImg(image.size(),CV_32FC1);

			GMM::CheckMask( image, maskImg );// GC_INIT_WITH_MASK
		    GMM::InitGMMs( image, maskImg, bgdGMM, fgdGMM );//calculate model param(covariance,coefs, prods etc.)

			const double gamma = 50;
			const double lambda = 9*gamma;
			const double beta = GMM::CalcBeta( image );

			cv::Mat leftW, upleftW, upW, uprightW;
			GMM::CalcNWeights( image, leftW, upleftW, upW, uprightW, beta, gamma );

			for( int i = 0; i < 1; i++ )
			{
				GCGraph<double> graph;
				GMM::AssignGMMsComponents( image, maskImg, bgdGMM, fgdGMM, compIdxs );  //may have bug
				GMM::LearnGMMs( image, maskImg, compIdxs, bgdGMM, fgdGMM );

				/*
				//输出可能的前景/背景概率
				cv::Point p;
				for( p.y = 0; p.y < image.rows; p.y++ )
				{
					for( p.x = 0; p.x < image.cols; p.x++)
					{
						// add node
						Vec3b color = image.at<Vec3b>(p);

						// set t-weights
						double fromSource, toSink;
						double probFgd=0.0;
						if( maskImg.at<uchar>(p) == GC_PR_BGD || maskImg.at<uchar>(p) == GC_PR_FGD)
						{
							fromSource = -log( bgdGMM(color) );
							toSink = -log( fgdGMM(color) );
	
						  //可以尝试如果 fromSource> toSink.然后将相应的点投到maskImg上看结果。
						}
						else if( mask.at<uchar>(p) == GC_BGD )
						{
							fromSource = 0;
							toSink = lambda;
						}
						else // GC_FGD
						{
							fromSource = lambda;
							toSink = 0;
						}
						probFgd=fromSource/(fromSource+toSink);
						//std::cout<<"fgdProb: "<<probFgd<<std::endl;
						probFgdImg.at<float>(p)=probFgd;
						//std::cout<<probFgdImg.at<float>(p)<<std::endl;
					}
				}
				*/
				GMM::ConstructGCGraph(image, maskImg, bgdGMM, fgdGMM, lambda, leftW, upleftW, upW, uprightW, graph );
				GMM::EstimateSegmentation( graph, maskImg );
			}

			//Show image and write mask in disk
			Mat res;
			Mat binMask;
			//if image have been reset, then copy it
			//else copy the processed image
			if( maskImg.empty() || maskImg.type()!=CV_8UC1 )
				CV_Error( CV_StsBadArg, "maskImg is empty or has incorrect type (not CV_8UC1)" );
			if( binMask.empty() || binMask.rows!=maskImg.rows || binMask.cols!=maskImg.cols )
				binMask.create( maskImg.size(), CV_8UC1 );
			binMask = maskImg & 1;
			image.copyTo( res, binMask );

			//cv::imshow(winName,res);

			for(int y=0;y<imgHeight;y++)
			{
				for(int x=0;x<imgWidth;x++)
				{
					maskImg.at<uchar>(y,x)=(double)maskImg.at<uchar>(y,x)*(255.0/3.0);
				}
			}
			cv::imwrite(curFrame->m_sMaskImgPathName,maskImg);

		}
	}

	 std::cout<<"Execute2DGrabCut1..."<<std::endl;
 }