#include "BoxCalculator.h"
#include "SimplxWarp.h"
//#include "SystemParameters.h"
#include <iostream>
#include "CxImageFunction.h"
#include "ZImage.h"
#include "LSparawidget.h"

double maxObjFunc( double*  coffMatA,double* B,
	double*  objFunc,const int MEqua,const int NVar,const int M1,const int M2,int& iCase);

CBoxCalculator CBoxCalculator::m_Instance;

CBoxCalculator::CBoxCalculator(){
	m_iObjectThreshold = 5;   
	m_iColorApron = 30;    //Org=30
	m_dSpaceApronSigma =0.01;
}

CBoxCalculator::~CBoxCalculator(){
}

bool CBoxCalculator::Calculate(std::vector<Wml::Vector3d>&  minXYZ, std::vector<Wml::Vector3d>&  maxXYZ)
{
	//first get the four plane for each LSVideoFrame
	 std::vector<std::vector<Wml::Vector4d >> planeSet;
	 int frameNum=minXYZ.size();
	 planeSet.resize(frameNum);
	  for(int iFrame = m_iStartFrame; iFrame <=m_iEndFrame; iFrame++)
	  { 
		
		 for(int camId = 0; camId <m_iCamNum; camId++)
		 {
			LSVideoFrame* curFrame = m_pMotion->GetFrameAt(camId, iFrame);

			Wml::Vector3d   pixelPosSet[4];
			
			int startX, startY, endX, endY;

			curFrame->GetMaskRectangle(startX, startY, endX, endY);
           // std::cout<<startX<<":"<<startY<<":"<<endX<<":"<<endY<<std::endl;

			pixelPosSet[ 0 ] = Wml::Vector3d(startX, startY, 1.0);
			pixelPosSet[ 1 ] = Wml::Vector3d(endX, startY, 1.0);
			pixelPosSet[ 2 ] = Wml::Vector3d(endX, endY, 1.0);
			pixelPosSet[ 3 ] = Wml::Vector3d(startX, endY, 1.0);
			Wml::Vector3d checkPixel = ( pixelPosSet[0] + pixelPosSet[2] ) * 0.5;
		   //  std::cout<<"cam: "<<camId<<" ith: "<<iFrame<<std::endl;
			Wml::Matrix3d  Inv_KXR;
			curFrame->GetKXR(Inv_KXR);
		   // std::cout<<"Inv_KXR[0][0]:"<<Inv_KXR[0][0]<<",Inv_KXR[2][2]:"<<Inv_KXR[2][2]<<std::endl;
     
//此步之前已验证，无误！

			Inv_KXR = Inv_KXR.Inverse();   //R-K-
			Wml::Vector3d   rayDirSet[4];
			for(int iPixel = 0;iPixel<4;++iPixel)
			{
				Wml::Vector3d RayDir = Inv_KXR * pixelPosSet[iPixel];
				RayDir.Normalize();
				rayDirSet[ iPixel ] = RayDir;
			}
		   
			Wml::Vector3d  camPos = curFrame->GetCameraPos();

		   // std::cout<<"camPos: "<<camPos[0]<<","<<camPos[1]<<","<<camPos[2]<<std::endl;
	
			Wml::Vector3d  checkPos;
			curFrame->GetWorldCoordFrmImgCoord(checkPixel.X(), checkPixel.Y(), 1.0/checkPixel.Z(), checkPos);

			//std::cout<<"checkPos:"<<checkPos[0]<<","<<checkPos[1]<<","<<checkPos[2]<<std::endl;


			for(int iPixel = 0;iPixel<4;++iPixel)
			{
				int kPixel = (iPixel + 1) %4;
				Wml::Vector3d  normal = rayDirSet[iPixel].Cross( rayDirSet[kPixel] );
				normal.Normalize();
				double D = -normal.Dot( camPos );

				double checkValue  = D + checkPos.Dot( normal );
				if( checkValue>0 ){ //we suppose the inner is negative
					D = -D;
					normal = -normal;
				}
				Wml::Vector4d plane( normal[0] ,normal[1],normal[2],D );
				planeSet[iFrame].push_back( plane );
				//planeSet[camId].push_back( plane );
			}
			
		}

	}

  
	//将各帧等信息加入(首先需了解四棱锥平面相交求解的大致过程和思想--->看论文）
	for(int frameId=m_iStartFrame;frameId<=m_iEndFrame;frameId++)
	{
	  
		const int NVar = 6;
		const int MEqua = planeSet[frameId].size();
		//const int MEqua = planeSet[camId].size();
		double*  coffMatA = new double [ MEqua * NVar ];
		double* B = new double [ MEqua ];
		double* objFunc = new double [ NVar + 1];
		int M1 = 0;
		int M2 = 0;
		prepareMat(planeSet[frameId],coffMatA,B,M1);
		//prepareMat(planeSet[camId],coffMatA,B,M1);
		M2 = MEqua - M1;
		int iCase = 0;

		//max  x0 - x1
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[ 1 ] = 1.0;
		objFunc[ 2 ] = -1.0;
		double maxX = maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase1: "<<iCase<<std::endl;
			return false;
		}


		//min x0 - x1,也就是max x1 - x0
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  1 ] = -1.0;
		objFunc[ 2 ] = 1.0;
		double minX = -maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase2: "<<iCase<<std::endl;
			return false;
		}

		//max y0 - y1
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  3 ] = 1.0;
		objFunc[ 4 ] = -1.0;
		double maxY = maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase3: "<<iCase<<std::endl;
			return false;
		}

		//min y0 - y1,也就是max y1 - y0
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  3 ] = -1.0;
		objFunc[ 4 ] = 1.0;
		double minY = -maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase4: "<<iCase<<std::endl;
			return false;
		}
		//max z0 - z1
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  5 ] = 1.0;
		objFunc[ 6 ] = -1.0;
		double maxZ = maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase5: "<<iCase<<std::endl;
			return false;
		}

		//min z0 - z1,也就是 max z1 - z0
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  5 ] = -1.0;
		objFunc[ 6 ] = 1.0;
		double minZ = -maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase6: "<<iCase<<std::endl;
			return false;
		}

		minXYZ[frameId] = Wml::Vector3d(minX,minY,minZ);
		maxXYZ[frameId] = Wml::Vector3d(maxX,maxY,maxZ);
		delete  [] objFunc;
		delete  [] B;
		delete  [] coffMatA;
	   
	    std::cout<<"calculate: "<<frameId<<" ith frame BoundingBox over..."<<std::endl;
	}
	planeSet.clear();  //release space allocated

   
	return true;
}


//for seedball based method: enlarge boundingbox
bool CBoxCalculator::CalculateEnlarge(std::vector<Wml::Vector3d>&  minXYZ, std::vector<Wml::Vector3d>&  maxXYZ)
{
	//first get the four plane for each LSVideoFrame
	std::vector<std::vector<Wml::Vector4d >> planeSet;
	int frameNum=minXYZ.size();
	planeSet.resize(frameNum);
	for(int iFrame = m_iStartFrame; iFrame <=m_iEndFrame; iFrame++)
	{ 
		//  std::cout<<"calculate: "<<iFrame<<" ith frame"<<std::endl;
		for(int camId = 0; camId < CamNum; camId++)
		{
			LSVideoFrame* curFrame = m_pMotion->GetFrameAt(camId, iFrame);

			Wml::Vector3d   pixelPosSet[4];

			int startX, startY, endX, endY;

			//curFrame->GetMaskRectangle(startX, startY, endX, endY);
			curFrame->GetEnlargeMaskRectangle(startX, startY, endX, endY,30);
		   // std::cout<<startX<<":"<<startY<<":"<<endX<<":"<<endY<<std::endl;

	
		// =======================================================================
			//Draw Mask Rect
			//cv::Mat drawRectImg=cv::imread(curFrame->m_sColorImgPathName);
			std::string sRectImg="r"+curFrame->m_sName;
			sRectImg=FileNameParser::findFileNameDir(curFrame->m_sColorImgPathName)+sRectImg+std::string(".bmp");
			cv::Mat drawRectImg=cv::imread(sRectImg);
			cv::Rect rect(startX,startY,endX-startX,endY-startY);
			cv::rectangle(drawRectImg,rect,cv::Scalar(0,0,255),2);
			cv::imwrite(sRectImg,drawRectImg);
			//curFrame->SetMaskRectangle(startX, startY, endX, endY);

        //=============================================================================
		
			pixelPosSet[ 0 ] = Wml::Vector3d(startX, startY, 1.0);
			pixelPosSet[ 1 ] = Wml::Vector3d(endX, startY, 1.0);
			pixelPosSet[ 2 ] = Wml::Vector3d(endX, endY, 1.0);
			pixelPosSet[ 3 ] = Wml::Vector3d(startX, endY, 1.0);
			Wml::Vector3d checkPixel = ( pixelPosSet[0] + pixelPosSet[2] ) * 0.5;
			//  std::cout<<"cam: "<<camId<<" ith: "<<iFrame<<std::endl;
			Wml::Matrix3d  Inv_KXR;
			curFrame->GetKXR(Inv_KXR);
			// std::cout<<"Inv_KXR[0][0]:"<<Inv_KXR[0][0]<<",Inv_KXR[2][2]:"<<Inv_KXR[2][2]<<std::endl;

			//此步之前已验证，无误！

			Inv_KXR = Inv_KXR.Inverse();   //R-K-
			Wml::Vector3d   rayDirSet[4];
			for(int iPixel = 0;iPixel<4;++iPixel)
			{
				Wml::Vector3d RayDir = Inv_KXR * pixelPosSet[iPixel];
				RayDir.Normalize();
				rayDirSet[ iPixel ] = RayDir;
			}

			Wml::Vector3d  camPos = curFrame->GetCameraPos();

			std::cout<<"camPos: "<<camPos[0]<<","<<camPos[1]<<","<<camPos[2]<<std::endl;

			Wml::Vector3d  checkPos;
			curFrame->GetWorldCoordFrmImgCoord(checkPixel.X(), checkPixel.Y(), 1.0/checkPixel.Z(), checkPos);

			std::cout<<"checkPos:"<<checkPos[0]<<","<<checkPos[1]<<","<<checkPos[2]<<std::endl;


			std::cout<<"check camera pos para!"<<std::endl;


			for(int iPixel = 0;iPixel<4;++iPixel)
			{
				int kPixel = (iPixel + 1) %4;
				Wml::Vector3d  normal = rayDirSet[iPixel].Cross( rayDirSet[kPixel] );
				normal.Normalize();
				double D = -normal.Dot( camPos );

				double checkValue  = D + checkPos.Dot( normal );
				if( checkValue>0 ){ //we suppose the inner is negative
					D = -D;
					normal = -normal;
				}
				Wml::Vector4d plane( normal[0] ,normal[1],normal[2],D );
				planeSet[iFrame].push_back( plane );
				//planeSet[camId].push_back( plane );
			}

		}

	}


	//将各帧等信息加入(首先需了解四棱锥平面相交求解的大致过程和思想--->看论文）
	for(int frameId=0;frameId<frameNum;frameId++)
	{
		const int NVar = 6;
		const int MEqua = planeSet[frameId].size();
		//const int MEqua = planeSet[camId].size();
		double*  coffMatA = new double [ MEqua * NVar ];
		double* B = new double [ MEqua ];
		double* objFunc = new double [ NVar + 1];
		int M1 = 0;
		int M2 = 0;
		prepareMat(planeSet[frameId],coffMatA,B,M1);
		//prepareMat(planeSet[camId],coffMatA,B,M1);
		M2 = MEqua - M1;
		int iCase = 0;

		//max  x0 - x1
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[ 1 ] = 1.0;
		objFunc[ 2 ] = -1.0;
		double maxX = maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase1: "<<iCase<<std::endl;
			return false;
		}


		//min x0 - x1,也就是max x1 - x0
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  1 ] = -1.0;
		objFunc[ 2 ] = 1.0;
		double minX = -maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase2: "<<iCase<<std::endl;
			return false;
		}

		//max y0 - y1
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  3 ] = 1.0;
		objFunc[ 4 ] = -1.0;
		double maxY = maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase3: "<<iCase<<std::endl;
			return false;
		}

		//min y0 - y1,也就是max y1 - y0
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  3 ] = -1.0;
		objFunc[ 4 ] = 1.0;
		double minY = -maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase4: "<<iCase<<std::endl;
			return false;
		}
		//max z0 - z1
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  5 ] = 1.0;
		objFunc[ 6 ] = -1.0;
		double maxZ = maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase5: "<<iCase<<std::endl;
			return false;
		}

		//min z0 - z1,也就是 max z1 - z0
		for(int i = 0;i<NVar+1;i++)
			objFunc[ i ] = 0.0;
		objFunc[  5 ] = -1.0;
		objFunc[ 6 ] = 1.0;
		double minZ = -maxObjFunc( coffMatA,B,objFunc,MEqua,NVar,M1,M2,iCase);
		if(iCase!=0)
		{
			std::cout<<"iCase6: "<<iCase<<std::endl;
			return false;
		}

		minXYZ[frameId] = Wml::Vector3d(minX,minY,minZ);
		maxXYZ[frameId] = Wml::Vector3d(maxX,maxY,maxZ);
		delete  [] objFunc;
		delete  [] B;
		delete  [] coffMatA;
	}
	planeSet.clear();  //release space allocated
	std::cout<<"Calculate over..."<<std::endl;
	return true;
}


  
// Load mask for every frame and get the mask boundary(equal to CalMaskBoudingBox)
bool CBoxCalculator::InitMaskRectangle()
{
	int minX, minY, maxX, maxY;

	for(int iFrame = m_iStartFrame; iFrame <= m_iEndFrame; ++iFrame)
	{	
		for(int camId= 0;camId < CamNum; camId++)
		{
			LSVideoFrame* curFrame = m_pMotion->GetFrameAt(camId, iFrame); // get frame in camId camera ith Frame.
			/*
			//if(camId==0)
				curFrame->GenerateRectFromHogDetect();       //经验证此方法不work(放在不同的图像上）
            */
			std::cout<<"Init "<<camId<<" GMMS..."<<std::endl;
			//BackGround Subtraction Method
			//if(camId==11)
		    curFrame->GenerateMaskImgFromGMMBGS();           //要求人站在正中间的约束有点太强（至少清华的数据不能用种子点扩展的方法）		
           
		 // 	curFrame->GenerateMaskImgFromBGS();   //NEW ADD by ly in 3/16/2015    //有纯背景图像
			
		  
			curFrame->GetMaskRectangle(minX, minY, maxX, maxY);

			if(minX != -1 && minY != -1 && maxX != -1 && maxY != -1)
				return true;
		  //std::cout<< minX<<":"<<minY<<":"<<maxX<<":"<<maxY<<std::endl;
		
			if(curFrame->LoadMaskImg() == false)
				return false;
			
			ZByteImage&  maskImg = *(curFrame->m_pMaskImg);

			const int width = maskImg.GetWidth();
			const int height = maskImg.GetHeight();
            
		   //initializtion
			minX = width;
			minY = height;
			maxX = 0;
			maxY = 0;

			for(int y=0; y<height; ++y)
			{
				for(int x=0; x<width; ++x)
				{
					if( maskImg.at(x,y) > m_iObjectThreshold )
					{
						minX = min(minX,x);
						maxX = max(maxX,x);
						minY = min(minY,y);
						maxY = max(maxY,y);
					}
				}
			}

			curFrame->Clear();  //release the memory that allocate to the color image and mask image
			
			//apron boundary, for the detect mask boundary, enlarge 30 pixel
			minX = max(0, minX - m_iColorApron);
			minY = max(0, minY - m_iColorApron-50);  
			maxX = min(width - 1, maxX + m_iColorApron);
			maxY = min(height - 1 ,maxY + m_iColorApron);  //考虑头部的特殊性，特别加上的50
		    std::cout<<"minX: "<<minX<<",minY: "<<minY<<",maxX: "<<maxX<<",maxY: "<<maxY<<std::endl;
         
		   //Draw Mask Rect
		   cv::Mat drawRectImg=cv::imread(curFrame->m_sColorImgPathName);
		   cv::Rect rect(minX,minY,maxX-minX,maxY-minY);
		   cv::rectangle(drawRectImg,rect,cv::Scalar(255,0,0),2);
		   std::string sRectImg="r"+curFrame->m_sName;
		   sRectImg=FileNameParser::findFileNameDir(curFrame->m_sColorImgPathName)+sRectImg+std::string(".bmp");
		   cv::imwrite(sRectImg,drawRectImg);
		   curFrame->SetMaskRectangle(minX, minY, maxX, maxY);
		 
		}
	}
	std::cout<<"Init Mask Rectangle successfully"<<std::endl;
	return true;
}


// Load mask for every frame and get the mask boundary(equal to CalMaskBoundingBox)
// 整个一圈前背景分别只用一个GMM
bool CBoxCalculator::InitMaskRectWithGMM()
{
	m_iCamNum=LSParaWidget::GetInstance()->GetCameraNumber();
	int minX, minY, maxX, maxY;
	for(int iFrame = m_iStartFrame; iFrame <= m_iEndFrame; ++iFrame)
	{	
		cv::Mat bgdModel,fgdModel;
		GMM bgdGMM(bgdModel),fgdGMM(fgdModel); 
		for(int camId= 0;camId <m_iCamNum; camId++)
		{
			LSVideoFrame* curFrame = m_pMotion->GetFrameAt(camId, iFrame); // get frame in camId camera iFrame Frame.

			if(camId==0)
			{
				 std::cout<<"Init Frame:"<<iFrame<<",Cam:"<<camId<<" GMMS..."<<std::endl;
				 curFrame->InitMaskImgFromGMM(bgdGMM,fgdGMM);  
			}

		
			else if(camId>=1)
			{
				 std::cout<<"Init Frame:"<<iFrame<<",Cam:"<<camId<<" GMMS..."<<std::endl;
				 curFrame->GetMaskImgFromInitGMM(bgdGMM,fgdGMM);    
				//根据第一帧(Cam 0)求得的前背景模型估计和更新GMM，得到各帧的mask.
			}
        
	     
		 // 	curFrame->GenerateMaskImgFromBGS();   //NEW ADD by ly in 3/16/2015    //有纯背景图像
			curFrame->GetMaskRectangle(minX, minY, maxX, maxY);
			if(minX != -1 && minY != -1 && maxX != -1 && maxY != -1)
				return true;
		  //std::cout<< minX<<":"<<minY<<":"<<maxX<<":"<<maxY<<std::endl;
	
		//load 新求得的mask
			if(curFrame->LoadMaskImg() == false)
				return false;
			
			ZByteImage&  maskImg = *(curFrame->m_pMaskImg);

			const int width = maskImg.GetWidth();
			const int height = maskImg.GetHeight();
            
		 //根据mask结果，得到包住mask的rect.
			minX = width;
			minY = height;
			maxX = 0;
			maxY = 0;

			for(int y=0; y<height; ++y)
			{
				for(int x=0; x<width; ++x)
				{
					if( maskImg.at(x,y) > m_iObjectThreshold )
					{
						minX = min(minX,x);
						maxX = max(maxX,x);
						minY = min(minY,y);
						maxY = max(maxY,y);
					}
				}
			}

			curFrame->Clear();  //release the memory that allocate to the color image and mask image

			//apron boundary, for the detect mask boundary, enlarge 30 pixel
			minX = max(0, minX - m_iColorApron);
			minY = max(0, minY - m_iColorApron);  
			maxX = min(width - 1, maxX + m_iColorApron);
			maxY = min(height - 1 ,maxY + m_iColorApron); 
			//std::cout<<"minX: "<<minX<<",minY: "<<minY<<",maxX: "<<maxX<<",maxY: "<<maxY<<std::endl;

		   //Draw Mask Rect
		   cv::Mat drawRectImg=cv::imread(curFrame->m_sColorImgPathName);
		   cv::Rect rect(minX,minY,maxX-minX,maxY-minY);
		   cv::rectangle(drawRectImg,rect,cv::Scalar(255,0,0),2);
		   std::string sRectImg="r"+curFrame->m_sName;
		   sRectImg=FileNameParser::findFileNameDir(curFrame->m_sMaskImgPathName)+sRectImg+"."+FileNameParser::findFileNameExt(curFrame->m_sColorImgPathName);  
		   cv::imwrite(sRectImg,drawRectImg);
		   curFrame->SetMaskRectangle(minX, minY, maxX, maxY);
		  
		}
	}
	//std::cout<<"Init Mask Rect with GMM"<<std::endl;
	return true;
}



void CBoxCalculator::InitDspRange( std::vector<double>& minDsp,std::vector<double>& maxDsp,std::vector<Wml::Vector3d>& minXYZ,std::vector<Wml::Vector3d>& maxXYZ )
{
	for(int iFrame=m_iStartFrame;iFrame<=m_iEndFrame;iFrame++)
	{
		//enlarge the area of BoundingBox
		Wml::Vector3d  delta = (maxXYZ[iFrame] - minXYZ[iFrame])  * m_dSpaceApronSigma;
		minXYZ[iFrame] -= delta;
		maxXYZ[iFrame] += delta;

		// eight corners for the BoundingBox
		const int eightNum = 8;
		Wml::Vector3d  eightPt[ eightNum ];
		eightPt[ 0 ] = Wml::Vector3d( minXYZ[iFrame].X(),minXYZ[iFrame].Y(),minXYZ[iFrame].Z() );
		eightPt[ 1 ] = Wml::Vector3d( maxXYZ[iFrame].X(),minXYZ[iFrame].Y(),minXYZ[iFrame].Z() );
		eightPt[ 2 ] = Wml::Vector3d( maxXYZ[iFrame].X(),maxXYZ[iFrame].Y(),minXYZ[iFrame].Z() );
		eightPt[ 3 ] = Wml::Vector3d( minXYZ[iFrame].X(),maxXYZ[iFrame].Y(),minXYZ[iFrame].Z() );
		eightPt[ 4 ] = Wml::Vector3d( minXYZ[iFrame].X(),minXYZ[iFrame].Y(),maxXYZ[iFrame].Z() );
		eightPt[ 5 ] = Wml::Vector3d( maxXYZ[iFrame].X(),minXYZ[iFrame].Y(),maxXYZ[iFrame].Z() );
		eightPt[ 6 ] = Wml::Vector3d( maxXYZ[iFrame].X(),maxXYZ[iFrame].Y(),maxXYZ[iFrame].Z() );
		eightPt[ 7 ] = Wml::Vector3d( minXYZ[iFrame].X(),maxXYZ[iFrame].Y(),maxXYZ[iFrame].Z() );

		minDsp[iFrame] = 1e10F;
		maxDsp[iFrame] = 1e-10F;

		for(int camId = 0; camId <m_iCamNum; camId++)  //CamNum
		{
			double curFrameMinDsp = 1e10F;
			double curFrameMaxDsp = 1e-10F;

			LSVideoFrame* curFrame = m_pMotion->GetFrameAt(camId, iFrame );
			Wml::Vector3d  UVInvZ;

			for(int i = 0;i<eightNum;i++)
			{
				curFrame->GetCameraCoordFrmWorldCoord(UVInvZ, eightPt[ i ]);
				double dsp = 1.0 / UVInvZ.Z();
				curFrameMinDsp = min( dsp, curFrameMinDsp );
				curFrameMaxDsp = max( dsp, curFrameMaxDsp );

			}
			//bug fix  kxl 2010 6-26 - 16:29
			curFrame->SetDspRange(curFrameMinDsp, curFrameMaxDsp);

			minDsp[iFrame] = min( minDsp[iFrame], curFrameMinDsp );
			maxDsp[iFrame] = max( maxDsp[iFrame], curFrameMaxDsp );

			std::cout<<"minDsp: "<<minDsp[iFrame]<<";"<<"maxDsp: "<<maxDsp[iFrame]<<std::endl;
		}
		//std::cout<<"LSStructureMotion::GetInstance()->m_fDspMin.size():"<<LSStructureMotion::GetInstance()->m_fDspMin.size()<<std::endl;
		//std::cout<<"m_iStartFrame:"<<m_iStartFrame<<",m_iEndFrame:"<<m_iEndFrame<<std::endl;
	   LSStructureMotion::GetInstance()->m_fDspMin[iFrame]=minDsp[iFrame];
	   LSStructureMotion::GetInstance()->m_fDspMax[iFrame]=maxDsp[iFrame];
     //  std::cout<<"mfDspMin:"<< LSStructureMotion::GetInstance()->m_fDspMin[iFrame]<<"mfDspMax:"<< LSStructureMotion::GetInstance()->m_fDspMax[iFrame]<<std::endl;
	}
   //   std::cout<<"Init Dsp Range over..."<<std::endl;
}

bool CBoxCalculator::CalculateBox( LSStructureMotion* pMotion, int startFrame, int endFrame )
{
	std::cout <<"Calculating the bounding box start..."<<std::endl;
	m_iStartFrame = startFrame;          
	m_iEndFrame = endFrame;              
	m_pMotion = pMotion;   
	m_iFrameNum=endFrame-startFrame+1;    //frame number
	//if(InitMaskRectangle() == false)      //Init mask rect Box on every image //ORG
	if(InitMaskRectWithGMM()==false)        //new add_所有一圈相机只建一个前背景GMM
		return false;

	  std::vector<Wml::Vector3d>  minXYZ;
	  std::vector<Wml::Vector3d>  maxXYZ;  
	  minXYZ.resize(m_iFrameNum);     
	  maxXYZ.resize(m_iFrameNum);

	 if( Calculate(minXYZ, maxXYZ) == false)  //calculate minXYZ and maxXYZ for boundingBox
	   {
		   std::cout<<"Calculate(minXYZ, maxXYZ) == false"<<std::endl;
		   return false;
	   }
   //create space for minDsp and maxDsp
	std::vector<double>minDsp; 
	std::vector<double>maxDsp;
	minDsp.resize(m_iFrameNum);
	maxDsp.resize(m_iFrameNum);
	LSStructureMotion::GetInstance()->m_fDspMin.resize(m_iFrameNum);   //NEW-ADD
	LSStructureMotion::GetInstance()->m_fDspMax.resize(m_iFrameNum);

    
	InitDspRange(minDsp, maxDsp, minXYZ, maxXYZ);  //assume the process is right, I don' konw until now
	LSParaWidget::GetInstance()->SetDspRange(minDsp,maxDsp);
	
	pMotion->m_BoxMinXYZ = minXYZ;
	pMotion->m_BoxMaxXYZ = maxXYZ;

	//std::cout <<"Finish calculating the bounding box:"<<std::endl;
	for(int iFrame=m_iStartFrame;iFrame<=m_iEndFrame;iFrame++)
	{
		std::cout << "minXYZ: " << minXYZ[iFrame].X() <<" " << minXYZ[iFrame].Y() <<" "<< minXYZ[iFrame].Z() << std::endl;
		std::cout << "maxXYZ: " << maxXYZ[iFrame].X() <<" " << maxXYZ[iFrame].Y() <<" "<< maxXYZ[iFrame].Z() << std::endl << std::endl;
	}
	minXYZ.clear();
	maxXYZ.clear();
	minDsp.clear();
	maxDsp.clear();
	return true;
}



bool CBoxCalculator::CalculateEnlargeBox( LSStructureMotion* pMotion, int startFrame, int endFrame )
{
	std::cout <<"Calculating the Enlarge bounding box..."<<std::endl;
	m_iStartFrame = startFrame;          
	m_iEndFrame = endFrame;              
	m_pMotion = pMotion;   
	m_iFrameNum=endFrame-startFrame+1;   //frame number

	//if(InitMaskRectangle() == false)      //Init mask rect Box on every image
   // return false;

	std::vector<Wml::Vector3d>  minXYZ;
	std::vector<Wml::Vector3d>  maxXYZ;

	minXYZ.resize(m_iFrameNum);     
	maxXYZ.resize(m_iFrameNum);
	//calculate minXYZ and maxXYZ for boundingBox
	if( CalculateEnlarge(minXYZ, maxXYZ) == false)
	{
		std::cout<<"Calculate(minXYZ, maxXYZ) == false"<<std::endl;
		return false;
	}
	
	pMotion->m_LgrBoxMinXYZ = minXYZ;
	pMotion->m_LgrBoxMaxXYZ = maxXYZ;

	std::cout <<"Finish calculating the bounding box:"<<std::endl;
	for(int iFrame=m_iStartFrame;iFrame<=m_iEndFrame;iFrame++)
	{
		std::cout << "EnlargeminXYZ: " << minXYZ[iFrame].X() <<" " << minXYZ[iFrame].Y() <<" "<< minXYZ[iFrame].Z() << std::endl;
		std::cout << "EnlargemaxXYZ: " << maxXYZ[iFrame].X() <<" " << maxXYZ[iFrame].Y() <<" "<< maxXYZ[iFrame].Z() << std::endl << std::endl;
	}
	minXYZ.clear();
	maxXYZ.clear();
	
	return true;
}



/*
for each plane  Pi  we can construct a linear equation like
Pi * X <=0, that is 
xi * x + yi * y + zi * z + wi <=0
these are all the constraints

we want to get the max and min x,y,z.
for example the max  x.
下面我们要把它转化成线性规划的标准形式
1>所有变量非负数。所以 x =  x0 - x1,y = y0 - y1,z = z0 - z1,其中等号右边的量全大于0.
这样规划问题变成：
max  p = x0 - x1
st.   xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 + wi <=0
2>所有的不等式或者等式的右边的值大于 0 ，这个要根据wi的值判断
3>M1表示不等式中 小于号式子多少个，M2大于号，M3等于号，这里根据2>判断

//由于精度问题， 将所有的不等式中的0替换成 ESP.
///////////////////////////////////////////////////
下面是构造矩阵：
首先将所有的不等式中加入松弛变量 qi（i<约束条件的个数 M = M1 + M2 + M3）

对于小于号等式（也就是wi小于0）
xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1  <= -wi   ---->

xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 + qi = -wi -->
加入人工变量
pi  = - wi - xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1 -qi  ---<1>
@@@@@@@@@@@@@@@@@@@@@@@@

对于大于号等式（也就是wi大于0）也就是原来不等式等式两边乘以 -1
-xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1  >= wi   ---->

-xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1 - qi= wi -->
加入人工变量
pi  = wi + xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 +qi    --<2>

/////////////////////////////////////////////////////
构造新的目标函数 newp = sum( -pi ),除非pi等于0，
否则加入人工变量前后不可能等价，而我们是希望他们等价的.
求解过程分两个阶段：
1利用新的目标函数 构造初始基本可行向量
2利用原来的目标函数求解
//////////////////////////////////////////////////////
现在构造求解矩阵
矩阵的行是目标函数值p,以及pi和newp，所以总共的有M + 2行.
矩阵的列是构造出的pi等式中的常数，原来的变量x0,x1,y0,y1,z0,z1
（我们不用管人工变量，因为他的求解函数中要求先放入所有的小于等于，然后放入所有的大于等于，
而小于等于式子中的qi系数一定是-1，大于等于式子中的qi的系数是1）,所以总共有N + 1列，变量个数加1.

根据所有的pi式子将矩阵填上。
//////////////////////////////////////////////////////////////////////

*/

//系数矩阵中先放小于等于的式子
void CBoxCalculator::prepareMat(std::vector<Wml::Vector4d >& planeSet,
	double*  coffMatA,double* B,int& M1)
{
	const int  NVar = 6;//这里我们只有6个变量

	M1 = 0;
	for(int iPlane = 0;iPlane<planeSet.size();iPlane++)
	{
		Wml::Vector4d&  plane = planeSet[ iPlane ];
		const double xi = plane[ 0 ];
		const double yi = plane[ 1 ];
		const double zi = plane[ 2 ]; 
		const double wi = plane[ 3 ];

		if( wi<=0 )//小于号式子
		{
			double* curRow = coffMatA + M1 * NVar;
			B[ M1 ] = -wi;	
			curRow[ 0 ] = xi;curRow[ 1 ] = -xi;
			curRow[ 2 ] = yi;curRow[ 3 ] = -yi;
			curRow[ 4 ] = zi;curRow[ 5 ] = -zi;
			M1++;
		}
	}

	int M2 = M1;
	for(int iPlane = 0;iPlane<planeSet.size();iPlane++)
	{
		Wml::Vector4d&  plane = planeSet[ iPlane ];
		const double xi = plane[ 0 ];
		const double yi = plane[ 1 ];
		const double zi = plane[ 2 ]; 
		const double wi = plane[ 3 ];

		if( wi<=0 )//小于号式子
			continue;

		double* curRow = coffMatA + M2 * NVar;
		B[ M2 ] = wi;	
		curRow[ 0 ] = -xi;curRow[ 1 ] = xi;
		curRow[ 2 ] = -yi;curRow[ 3 ] = yi;
		curRow[ 4 ] = -zi;curRow[ 5 ] = zi;
		M2++;
	}

	assert( M2==planeSet.size() );
}

//bool CBoxCalculator::CropImages( CBDMStructureMotion* pMotion, int startFrame, int endFrame )
//{
//	std::tcout <<_T("Crop original and mask images...\n");
//	m_iStartFrame = startFrame;
//	m_iEndFrame = endFrame;
//	m_pMotion = pMotion;
//
//	if( InitMaskRectangle() == false)
//		return false;
//
//	int minX, minY, maxX, maxY;
//	CxImage  newColorImage, origColorImage;
//	CxImage  newMaskImage, origMaskImage;
//	for(int iFrame = m_iStartFrame; iFrame <= m_iEndFrame; ++iFrame){
//		//Temp
//		//for(int m = 0; m < 2; m++){
//		for(int m = 0; m < 1; m++){
//			CBDMVideoFrame* curFrame = m_pMotion->GetFrameAt(m, iFrame);
//			curFrame->GetMaskRectangle(minX, minY, maxX, maxY);
//
//			int height = curFrame->GetImgHeight();
//			int newWidth = maxX - minX + 1;
//			int newHeight = maxY - minY + 1;
//
//			if( origColorImage.Load( curFrame->m_sColorImgPathName.c_str() ) == false)
//				return false;
//			if( origMaskImage.Load( curFrame->m_sMaskImgPathName.c_str() ) == false)
//				return false;
//
//			newColorImage.Create(newWidth, newHeight, 24);
//			newMaskImage.Create(newWidth, newHeight, 24);
//
//			for(int  y = minY; y<=maxY; y++){
//				for(int x = minX; x<=maxX; x++){
//					RGBQUAD  ColorRgb = origColorImage.GetPixelColor(x, height - 1 - y);
//					RGBQUAD  MaskRgb = origMaskImage.GetPixelColor(x, height - 1 - y);
//					const int newX = x - minX;
//					const int newY = y - minY;
//					newColorImage.SetPixelColor(newX, newHeight -1 - newY, ColorRgb);
//					newMaskImage.SetPixelColor(newX, newHeight -1 - newY, MaskRgb);
//				}
//			}
//			curFrame->Clear();
//
//			newColorImage.Save(curFrame->m_sColorImgPathName.c_str(), CxImageFuns::GetImageFileType(curFrame->m_sColorImgPathName));
//			newMaskImage.Save(curFrame->m_sMaskImgPathName.c_str(), CXIMAGE_FORMAT_BMP);
//
//			//set the camera param
//			curFrame->SetCameraParaKAt(0, 2, curFrame->GetCameraParaKAt(0, 2) - minX);
//			curFrame->SetCameraParaKAt(1, 2, curFrame->GetCameraParaKAt(1, 2) - minY);
//			curFrame->InitCameraPos();
//
//			curFrame->SetImgSize(newWidth, newHeight);
//			curFrame->SetMaskRectangle(-1, -1, -1, -1);
//			curFrame->SetDspRange(-1.0, -1.0);
//		}
//	}
//
//	pMotion->m_BoxMinXYZ = Wml::Vector3d(-1, -1, -1);
//	pMotion->m_BoxMaxXYZ = Wml::Vector3d(-1, -1, -1);
//
//	if( pMotion->SaveCamera_Format2() == false){
//		return false;
//	}
//
//	return true;
//}
