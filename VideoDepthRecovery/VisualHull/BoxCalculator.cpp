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
     
//�˲�֮ǰ����֤������

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

  
	//����֡����Ϣ����(�������˽�����׶ƽ���ཻ���Ĵ��¹��̺�˼��--->�����ģ�
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


		//min x0 - x1,Ҳ����max x1 - x0
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

		//min y0 - y1,Ҳ����max y1 - y0
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

		//min z0 - z1,Ҳ���� max z1 - z0
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

			//�˲�֮ǰ����֤������

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


	//����֡����Ϣ����(�������˽�����׶ƽ���ཻ���Ĵ��¹��̺�˼��--->�����ģ�
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


		//min x0 - x1,Ҳ����max x1 - x0
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

		//min y0 - y1,Ҳ����max y1 - y0
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

		//min z0 - z1,Ҳ���� max z1 - z0
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
				curFrame->GenerateRectFromHogDetect();       //����֤�˷�����work(���ڲ�ͬ��ͼ���ϣ�
            */
			std::cout<<"Init "<<camId<<" GMMS..."<<std::endl;
			//BackGround Subtraction Method
			//if(camId==11)
		    curFrame->GenerateMaskImgFromGMMBGS();           //Ҫ����վ�����м��Լ���е�̫ǿ�������廪�����ݲ��������ӵ���չ�ķ�����		
           
		 // 	curFrame->GenerateMaskImgFromBGS();   //NEW ADD by ly in 3/16/2015    //�д�����ͼ��
			
		  
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
			maxY = min(height - 1 ,maxY + m_iColorApron);  //����ͷ���������ԣ��ر���ϵ�50
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
// ����һȦǰ�����ֱ�ֻ��һ��GMM
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
				//���ݵ�һ֡(Cam 0)��õ�ǰ����ģ�͹��ƺ͸���GMM���õ���֡��mask.
			}
        
	     
		 // 	curFrame->GenerateMaskImgFromBGS();   //NEW ADD by ly in 3/16/2015    //�д�����ͼ��
			curFrame->GetMaskRectangle(minX, minY, maxX, maxY);
			if(minX != -1 && minY != -1 && maxX != -1 && maxY != -1)
				return true;
		  //std::cout<< minX<<":"<<minY<<":"<<maxX<<":"<<maxY<<std::endl;
	
		//load ����õ�mask
			if(curFrame->LoadMaskImg() == false)
				return false;
			
			ZByteImage&  maskImg = *(curFrame->m_pMaskImg);

			const int width = maskImg.GetWidth();
			const int height = maskImg.GetHeight();
            
		 //����mask������õ���סmask��rect.
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
	if(InitMaskRectWithGMM()==false)        //new add_����һȦ���ֻ��һ��ǰ����GMM
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
��������Ҫ����ת�������Թ滮�ı�׼��ʽ
1>���б����Ǹ��������� x =  x0 - x1,y = y0 - y1,z = z0 - z1,���еȺ��ұߵ���ȫ����0.
�����滮�����ɣ�
max  p = x0 - x1
st.   xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 + wi <=0
2>���еĲ���ʽ���ߵ�ʽ���ұߵ�ֵ���� 0 �����Ҫ����wi��ֵ�ж�
3>M1��ʾ����ʽ�� С�ں�ʽ�Ӷ��ٸ���M2���ںţ�M3���ںţ��������2>�ж�

//���ھ������⣬ �����еĲ���ʽ�е�0�滻�� ESP.
///////////////////////////////////////////////////
�����ǹ������
���Ƚ����еĲ���ʽ�м����ɳڱ��� qi��i<Լ�������ĸ��� M = M1 + M2 + M3��

����С�ںŵ�ʽ��Ҳ����wiС��0��
xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1  <= -wi   ---->

xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 + qi = -wi -->
�����˹�����
pi  = - wi - xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1 -qi  ---<1>
@@@@@@@@@@@@@@@@@@@@@@@@

���ڴ��ںŵ�ʽ��Ҳ����wi����0��Ҳ����ԭ������ʽ��ʽ���߳��� -1
-xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1  >= wi   ---->

-xi * x0 + xi * x1 - yi * y0  + yi * y1- zi * z0 + zi * z1 - qi= wi -->
�����˹�����
pi  = wi + xi * x0 - xi * x1 + yi * y0  - yi * y1+ zi * z0 - zi * z1 +qi    --<2>

/////////////////////////////////////////////////////
�����µ�Ŀ�꺯�� newp = sum( -pi ),����pi����0��
��������˹�����ǰ�󲻿��ܵȼۣ���������ϣ�����ǵȼ۵�.
�����̷������׶Σ�
1�����µ�Ŀ�꺯�� �����ʼ������������
2����ԭ����Ŀ�꺯�����
//////////////////////////////////////////////////////
���ڹ���������
���������Ŀ�꺯��ֵp,�Լ�pi��newp�������ܹ�����M + 2��.
��������ǹ������pi��ʽ�еĳ�����ԭ���ı���x0,x1,y0,y1,z0,z1
�����ǲ��ù��˹���������Ϊ������⺯����Ҫ���ȷ������е�С�ڵ��ڣ�Ȼ��������еĴ��ڵ��ڣ�
��С�ڵ���ʽ���е�qiϵ��һ����-1�����ڵ���ʽ���е�qi��ϵ����1��,�����ܹ���N + 1�У�����������1.

�������е�piʽ�ӽ��������ϡ�
//////////////////////////////////////////////////////////////////////

*/

//ϵ���������ȷ�С�ڵ��ڵ�ʽ��
void CBoxCalculator::prepareMat(std::vector<Wml::Vector4d >& planeSet,
	double*  coffMatA,double* B,int& M1)
{
	const int  NVar = 6;//��������ֻ��6������

	M1 = 0;
	for(int iPlane = 0;iPlane<planeSet.size();iPlane++)
	{
		Wml::Vector4d&  plane = planeSet[ iPlane ];
		const double xi = plane[ 0 ];
		const double yi = plane[ 1 ];
		const double zi = plane[ 2 ]; 
		const double wi = plane[ 3 ];

		if( wi<=0 )//С�ں�ʽ��
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

		if( wi<=0 )//С�ں�ʽ��
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
