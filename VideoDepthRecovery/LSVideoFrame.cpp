#include "LSVideoFrame.h"
#include "LSModelIO.h"
#include "LSparawidget.h"
#include "LSRunConfigDlg.h"
#include <algorithm>
#include <stack>
#include <map>

//初始化：静态成员必须在cpp中初始化，否则报错
int LSVideoFrame::m_iWidth=-1;  
int LSVideoFrame::m_iHeight=-1;
std::string LSVideoFrame::s_DataDir[CamNum];
std::string LSVideoFrame::s_TempDir[CamNum];
std::string LSVideoFrame::s_RawDir[CamNum];
std::string LSVideoFrame::s_SegDir[CamNum];
std::string LSVideoFrame::s_OptFlwDir[CamNum];  //new add_ for optical flow

LSVideoFrame::LSVideoFrame(void)
{
	m_pCameraFrame=0;
	m_pMaskMap=0;              //NEW-ADD

	m_pLsCameraParameter=new LSCameraParameters;  //point object must create
	m_pColorImg=NULL;
	m_pMaskImg=NULL;
	
	m_mCastImg=NULL;
	m_mMaskImg=NULL;
	m_mColorImg=NULL;
	m_mBgdSubImg=NULL;

	m_pMinDspImg = NULL;
	m_pMaxDspImg = NULL;

	m_pDepthMax=NULL;  //new add
	m_pDepthMin=NULL;  //new add

	m_pVarianceMap=NULL; //new add

	m_pMinLabelImg = NULL;
	m_pMaxLabelImg = NULL;

	m_pDspImg=NULL;

	m_pForeOptImg=NULL;
	m_pBackOptImg=NULL;

	//initialize
	 m_iMaskMinX=-1;
	 m_iMaskMinY=-1;
	 m_iMaskMaxX=-1;
	 m_iMaskMaxY=-1;

	 LEASTFRONT=2;   //阈值可调，为什么设置为2  （判断是否属于mask中像素）
}


LSVideoFrame::~LSVideoFrame(void)
{
	if(m_pMaskMap)  	delete m_pMaskMap;  	m_pMaskMap = 0; //NEW ADD

	delete m_pLsCameraParameter;
	Clear();
}

//make all sort of images point to Null
void LSVideoFrame::Clear()
{
	if(m_pColorImg!=NULL)
	{
	   delete m_pColorImg;
	   m_pColorImg=NULL;
	}

	if(m_pMaskImg!=NULL)
	{
		delete m_pMaskImg;
		m_pMaskImg=NULL;
	}

	if(	m_pDspImg != NULL){
		delete m_pDspImg;
		m_pDspImg = NULL;
	}

	if(m_pMaxDspImg != NULL){
		delete m_pMaxDspImg;
		m_pMaxDspImg = NULL;
	}
	if(m_pMinDspImg != NULL){
		delete m_pMinDspImg;
		m_pMinDspImg = NULL;
	}

	// new add
	if(m_pDepthMax!=NULL)
	{
		delete m_pDepthMax;
		m_pDepthMax=NULL;
	}
	//new add
	if(m_pDepthMin!=NULL)
	{
		delete m_pDepthMin;
		m_pDepthMin=NULL;
	}

	//new add
	if(m_pVarianceMap!=NULL)
	{
		delete m_pVarianceMap;
		m_pVarianceMap=NULL;
	}



	if(m_pMaxLabelImg != NULL){
		delete m_pMaxLabelImg;
		m_pMaxLabelImg = NULL;
	}
	if(m_pMinLabelImg != NULL){
		delete m_pMinLabelImg;
		m_pMinLabelImg = NULL;
	}


	if(m_pForeOptImg != NULL){
		delete m_pForeOptImg;
		m_pForeOptImg = NULL;
	}


	if(m_pBackOptImg != NULL){
		delete m_pBackOptImg;
		m_pBackOptImg = NULL;
	}


	if(!m_mCastImg.empty())
	{  
		m_mCastImg.release();
	}

	if(!m_mMaskImg.empty())
	{
		m_mMaskImg.release();
	}
	
	if(!m_mColorImg.empty())
	{
		m_mColorImg.release();
	}

	if(!m_mBgdSubImg.empty())
	{
		m_mBgdSubImg.release();
	}

}


void LSVideoFrame::GetColorAt(int x, int y, Wml::Vector3f& out_Color ) 
{
	out_Color[0] = m_pColorImg->at(x, y, 0);         //(
	out_Color[1] = m_pColorImg->at(x, y, 1);
	out_Color[2] = m_pColorImg->at(x, y, 2);
}


//此函数有问题！！！//还没有发现问题。
void LSVideoFrame::GetColorAt(float x, float y, Wml::Vector3f& out_Color )
{
	// floor 向下取整   ceil 向上取整

	int ix0 = floor(x)<0 ? 0:floor(x);
    int ix1=ix0+1;
	if(ix1>=GetImgWidth())
	{
		ix1=GetImgWidth()-1;
		ix0=ix1-1;
	}
	
	//int ix1 =ceil(x) >= GetImgWidth()? (GetImgWidth()-1): ceil(x);

	int iy0 = floor(y)<0 ? 0:floor(y);
	int iy1=iy0+1;
	if(iy1>=GetImgHeight())
	{
		iy1=GetImgHeight()-1;
		iy0=iy1-1;
	}


	//int iy1 = ceil(y) >= GetImgHeight()? (GetImgHeight()-1): ceil(y);


	float dx = x-ix0, dy = y-iy0;
	
	/*if(ix0<0 || ix0>s_iWidth-1 || iy0<0 || iy0>s_iHeight-1)
		printf("x:%f,y:%f\n",x,y);*/
	//std::cout<<"ix0: "<<ix0<<",iy0: "<<iy0<<std::endl;
	//std::cout<<"ix1: "<<ix1<<",iy1:" <<iy1<<std::endl;
	float r00 = m_pColorImg->at(ix0, iy0, 0);
	float r01 = m_pColorImg->at(ix0, iy1, 0);
	float r10 = m_pColorImg->at(ix1, iy0 ,0);
	float r11 = m_pColorImg->at(ix1, iy1, 0);

	
	float g00 = m_pColorImg->at(ix0, iy0, 1);
	float g01 = m_pColorImg->at(ix0, iy1, 1);
	float g10 = m_pColorImg->at(ix1, iy0, 1);
	float g11 = m_pColorImg->at(ix1, iy1, 1);

	float b00 = m_pColorImg->at(ix0, iy0, 2);
	float b01 = m_pColorImg->at(ix0, iy1, 2);
	float b10 = m_pColorImg->at(ix1, iy0, 2);
	float b11 = m_pColorImg->at(ix1, iy1, 2);

	//二元线性插值
	out_Color[0] = (r00 * (1.0-dx) + r10 * dx) * (1.0-dy) + 
		(r01 * (1.0-dx) + r11 * dx) * dy;

	out_Color[1] = (g00 * (1.0-dx) + g10 * dx) * (1.0-dy) + 
		(g01 * (1.0-dx) + g11 * dx) * dy;

	out_Color[2] = (b00 * (1.0-dx) + b10 * dx) * (1.0-dy) + 
		(b01 * (1.0-dx) + b11 * dx) * dy;
}


void LSVideoFrame::GetForeOptAt(double x, double y, Wml::Vector2d& out_ForeOpt )
{
	// floor 向下取整   ceil 向上取整

	int ix0 = floor(x), ix1 = ceil(x) >= GetImgWidth()? ix0: ceil(x);
	int iy0 = floor(y), iy1 = ceil(y) >= GetImgHeight()? iy0: ceil(y);


	double dx = x-ix0, dy = y-iy0;
	
	/*if(ix0<0 || ix0>s_iWidth-1 || iy0<0 || iy0>s_iHeight-1)
		printf("x:%f,y:%f\n",x,y);*/

	double u00 = m_pForeOptImg->at(ix0, iy0, 0);
	double u01 = m_pForeOptImg->at(ix0, iy1, 0);
	double u10 = m_pForeOptImg->at(ix1, iy0 ,0);
	double u11 = m_pForeOptImg->at(ix1, iy1, 0);

	double v00 = m_pForeOptImg->at(ix0, iy0, 1);
	double v01 = m_pForeOptImg->at(ix0, iy1, 1);
	double v10 = m_pForeOptImg->at(ix1, iy0, 1);
	double v11 = m_pForeOptImg->at(ix1, iy1, 1);


	//二元线性插值
	out_ForeOpt[0] = (v00 * (1.0-dx) + v10 * dx) * (1.0-dy) + 
		(v01 * (1.0-dx) + v11 * dx) * dy;

	out_ForeOpt[1] = (u00 * (1.0-dx) + u10 * dx) * (1.0-dy) + 
		(u01 * (1.0-dx) + u11 * dx) * dy;
}




void LSVideoFrame::GetBackOptAt(double x, double y, Wml::Vector2d& out_BackOpt )
{
	// floor 向下取整   ceil 向上取整

	int ix0 = floor(x), ix1 = ceil(x) >= GetImgWidth()? ix0: ceil(x);
	int iy0 = floor(y), iy1 = ceil(y) >= GetImgHeight()? iy0: ceil(y);


	double dx = x-ix0, dy = y-iy0;
	
	/*if(ix0<0 || ix0>s_iWidth-1 || iy0<0 || iy0>s_iHeight-1)
		printf("x:%f,y:%f\n",x,y);*/

	double u00 = m_pBackOptImg->at(ix0, iy0, 0);
	double u01 = m_pBackOptImg->at(ix0, iy1, 0);
	double u10 = m_pBackOptImg->at(ix1, iy0 ,0);
	double u11 = m_pBackOptImg->at(ix1, iy1, 0);

	double v00 = m_pBackOptImg->at(ix0, iy0, 1);
	double v01 = m_pBackOptImg->at(ix0, iy1, 1);
	double v10 = m_pBackOptImg->at(ix1, iy0, 1);
	double v11 = m_pBackOptImg->at(ix1, iy1, 1);


	//二元线性插值
	out_BackOpt[0] = (v00 * (1.0-dx) + v10 * dx) * (1.0-dy) + 
		(v01 * (1.0-dx) + v11 * dx) * dy;

	out_BackOpt[1] = (u00 * (1.0-dx) + u10 * dx) * (1.0-dy) + 
		(u01 * (1.0-dx) + u11 * dx) * dy;
}




bool LSVideoFrame::GenerateMaskImgFromBGS()
{
	double colorDiffThreshold=LSParaWidget::GetInstance()->GetColorDiffThreshold();
	//get background color image path
	std::string bgdImgName='b'+m_sName;
	

	std::string m_sBgdImagePathName=FileNameParser::findFileNameDir(m_sColorImgPathName)+bgdImgName+std::string(".png");
	cv::Mat backgroundImage;
	cv::Mat currentImage;
	//cv::Mat object; //the object to track
	backgroundImage=cv::imread(m_sBgdImagePathName);
	currentImage=cv::imread(m_sColorImgPathName);
	if(!backgroundImage.data)
	{
		cerr << "Unable to open background image frame: " << m_sBgdImagePathName << endl;
		exit(EXIT_FAILURE);
	}
	if(!currentImage.data)
	{
		cerr << "Unable to open current image frame: " <<m_sColorImgPathName<< endl;
		exit(EXIT_FAILURE);
	}
  //Method1: OpenCV Function
	////cv::Mat frame; //current frame 
	// cv::Mat fgMaskMOG; //fg mask generated by MOG method
	// cv::Ptr<cv::BackgroundSubtractor> pMOG; //MOG Background subtracter
	// pMOG= new cv::BackgroundSubtractorMOG(); //MOG approach
	// pMOG->operator()(backgroundImage,fgMaskMOG);
	// pMOG->operator()(currentImage,fgMaskMOG);
	// if(fgMaskMOG.empty())
	//	 std::cout<<"fgMaskMOG is empty!"<<std::endl;
	// cv::imwrite(m_sMaskImgPathName,fgMaskMOG);
	// //cv::imshow("backgroundImage", backgroundImage);
	// //cv::imshow("currentImage", currentImage);
	// //cv::imshow("FG Mask MOG", fgMaskMOG);
	// // cvWaitKey(0);

//Method2: GrayImage Function
	cv::Mat background_bw;
	cv::Mat current_bw;
	cvtColor(backgroundImage, background_bw, CV_RGB2GRAY);
	cvtColor(currentImage, current_bw, CV_RGB2GRAY);

	cv::Mat newObject(background_bw.rows, background_bw.cols, CV_8UC1);
	double grayDiff;
	for (int y = 0; y < newObject.rows; y++)
	{
		for (int x = 0; x < newObject.cols; x++)
		{
			// Subtract the two images
			newObject.at<uchar>(y, x)=grayDiff=(double)(current_bw.at<uchar>(y, x)-background_bw.at<uchar>(y, x));
			if(abs(grayDiff)>colorDiffThreshold)
				newObject.at<uchar>(y,x)=255;
			else
				newObject.at<uchar>(y,x)=0;
		}
	}
	cv::imwrite(m_sMaskImgPathName,newObject);
	return true;
}


bool LSVideoFrame::GenerateMaskImgFromGMMBGS()
{
	//image && mask
	cv::Mat image,mask;  
	cv::Mat bgdModel;
	cv::Mat fgdModel;
    image=cv::imread(m_sColorImgPathName);
	if( image.type() != CV_8UC3 )
		CV_Error( CV_StsBadArg, "image must have CV_8UC3 type" );
    mask.create(image.size(),CV_8UC1);

	//imgWidth& imgHeight
	int imgWidth=image.cols;
    int imgHeight=image.rows;

 // //中心点的方法可以在有合适数据时再使用
 //   int iRidus=min(imgWidth,imgHeight)/25;
 //   int midX=imgWidth/1.8;
	//int midY=imgHeight/2;

 //rect body in every images(different cams)...
    int minX=(1/4.0)*imgWidth;
	int maxX=(4/5.0)*imgWidth;
	int minY=(1/25.0)*imgHeight;
	int maxY=(24/25.0)*imgHeight;

	cv::Rect rect(minX,minY,maxX-minX,maxY-minY);
	//cv::rectangle(image,rect,cvScalar(0,0,255),2);
	//cv::imshow("image",image);

	//Set rect in mask (init labels with rect)
	assert( !mask.empty() );
	mask.setTo( GC_BGD );
	rect.x = max(0, rect.x);
	rect.y = max(0, rect.y);
	rect.width = min(rect.width, image.cols-rect.x);
	rect.height = min(rect.height, image.rows-rect.y);
	(mask(rect)).setTo( Scalar(GC_PR_FGD) );

	//中心点的方法可以在合适数据时再使用
	//Init Mask with cirle for the first frame;
	//cv::Vec3d castColor(0,255,0);
	//for(int y=0;y<imgHeight;y++)
	//{
	//	for(int x=0;x<imgWidth;x++)
	//	{
	//	      if((x-midX)*(x-midX)+(y-midY)*(y-midY)<=(iRidus*iRidus))
	//		  {
	//			 mask.at<uchar>(y,x)=GC_FGD;
	//			 //image.at<cv::Vec3b>(y,x)[0]=(castColor[0]*0.4)+image.at<cv::Vec3b>(y,x)[0]*0.6;
	//			 //image.at<cv::Vec3b>(y,x)[1]=(castColor[1]*0.4)+image.at<cv::Vec3b>(y,x)[1]*0.6;
	//			 //image.at<cv::Vec3b>(y,x)[2]=(castColor[2]*0.4)+image.at<cv::Vec3b>(y,x)[2]*0.6;
	//		  }
	//		  //mask.at<uchar>(y,x)*=(255.0/3.0);
	//	}
	//}


	GMM bgdGMM(bgdModel),fgdGMM(fgdModel); //initialize model param(covariance,mean,ci etc.)

	Mat compIdxs(image.size(), CV_32SC1 );
	//Mat probFgdImg(image.size(),CV_32FC1);

	GMM::CheckMask( image, mask);// GC_INIT_WITH_MASK

	GMM::InitGMMs( image, mask, bgdGMM, fgdGMM );//calculate model param(covariance,coefs, prods etc.)

	//const double beta = GMM::CalcBeta( image );  //Beta是根据各图像自适应的，可以考虑用来设置参数

	//std::cout<<"beta: "<<beta<<std::endl;

	//using pixels out of the rect as the candidate background pixels.
	for( int i = 0; i < 1; i++ )
	{
		GMM::AssignGMMsComponents( image, mask, bgdGMM, fgdGMM, compIdxs );  
		GMM::LearnGMMs( image, mask, compIdxs, bgdGMM, fgdGMM );
	     
		 cv::Point p;
	     double minSource=10,maxSource=-1;

               //Find minSink and maxSink
				for( p.y = minY; p.y < maxY; p.y++ )
				{
					for( p.x = minX; p.x < maxX; p.x++)
					{
						Vec3b color = image.at<Vec3b>(p);
						double fromSource=bgdGMM(color);
						if(fromSource<minSource)
							minSource=fromSource;
						if(fromSource>maxSource)
							maxSource=fromSource;

					}
				}
				std::cout<<"minSource:"<<minSource<<std::endl;
				std::cout<<"maxSource:"<<maxSource<<std::endl;

				//Normalize and calculate for every pixel
				for( p.y = minY; p.y < maxY; p.y++ )
				{
					for( p.x = minX; p.x < maxX; p.x++)
					{
						Vec3b color = image.at<Vec3b>(p);

						// calcluate probablity
						    double fromSource;
						    fromSource =  bgdGMM(color);
							double prbFgd=0;
							prbFgd=(fromSource-minSource)/(maxSource-minSource);
					      
						   if(prbFgd<0.00000001)  //0.000010
								mask.at<uchar>(p)=GC_PR_FGD;
							else
								mask.at<uchar>(p)=GC_PR_BGD;
					}
				}
	}

	
	//利用初始化的前背景采样点进行迭代学习新的GMM
	for( int i = 0; i < 1; i++ )
	{
		GMM::AssignGMMsComponents( image, mask, bgdGMM, fgdGMM, compIdxs );  
		GMM::LearnGMMs( image, mask, compIdxs, bgdGMM, fgdGMM );

		cv::Point p;
		for( p.y = minY; p.y < maxY; p.y++ )
		{
			for( p.x = minX; p.x < maxX; p.x++)
			{
				// add node
				Vec3b color = image.at<Vec3b>(p);

				// set t-weights
				double fromSource, toSink;

				fromSource = -log( bgdGMM(color) );
				toSink = -log( fgdGMM(color) );

				if(fromSource>toSink)
					mask.at<uchar>(p)=GC_PR_FGD;
				else
					mask.at<uchar>(p)=GC_PR_BGD;
			}
		}
	}
	

	Mat res;
	Mat binMask;
	//if image have been reset, then copy it; else copy the processed image
	if( mask.empty() || mask.type()!=CV_8UC1 )
		CV_Error( CV_StsBadArg, "maskImg is empty or has incorrect type (not CV_8UC1)" );
	if( binMask.empty() || binMask.rows!=mask.rows || binMask.cols!=mask.cols )
		binMask.create( mask.size(), CV_8UC1 );
	binMask = mask& 1;
	image.copyTo( res, binMask );

  //  imshow("image",res);
//	cvWaitKey(0);

	cv::Mat castImage;
	image.copyTo(castImage);
	cv::Vec3d cColor(0,255,0);

	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
		   //mask.at<uchar>(y,x)=mask.at<uchar>(y,x)/3*255;  //distinguish foreground and background
			if(mask.at<uchar>(y,x)==GC_PR_FGD)
			{
				mask.at<uchar>(y,x)=1;
				//castImage.at<cv::Vec3b>(y,x)[0]=castImage.at<cv::Vec3b>(y,x)[0]*0.4+cColor[0]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[1]=castImage.at<cv::Vec3b>(y,x)[1]*0.4+cColor[1]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[2]=castImage.at<cv::Vec3b>(y,x)[2]*0.4+cColor[2]*0.6;
			}
			else 
				mask.at<uchar>(y,x)=0;
			 //mask.at<uchar>(y,x)*=255;
		}
	}
	//cv::imwrite(m_sMaskImgPathName,mask);
	
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(9,9),Point(4,4));
	 morphologyEx(mask, mask, MORPH_OPEN, element);
     element.release();

	//imshow("mask",mask);

//test cast result
	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
			if(mask.at<uchar>(y,x)==1)
			{
				//castImage.at<cv::Vec3b>(y,x)[0]=castImage.at<cv::Vec3b>(y,x)[0]*0.4+cColor[0]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[1]=castImage.at<cv::Vec3b>(y,x)[1]*0.4+cColor[1]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[2]=castImage.at<cv::Vec3b>(y,x)[2]*0.4+cColor[2]*0.6;
				//mask.at<uchar>(y,x)*=255;
			}
		}
	}

	//imshow("mask",mask);
	//imwrite(m_sMaskImgPathName,mask);

 //求最大连通区域    //4邻接方法
	//void Seed_Filling(const cv::Mat& binImg, cv::Mat& lableImg)   //种子填充法
	cv::Mat binImg;
	mask.copyTo(binImg);
	cv::Mat lableImg;
	if (binImg.empty()||
		binImg.type() != CV_8UC1)
	{
		std::cout<<"binImg.type() != CV_8UC1"<<std::endl;
		return false;
	}

	lableImg.release();
	binImg.convertTo(lableImg, CV_32SC1);

	int label = 1; 
	//int labelCount[500]={0};   //参数20为人为指定，可以利用数组改成动态生成。
	std::vector<int> labelCount;
	labelCount.reserve(200);
	int rows = binImg.rows - 1;  
	int cols = binImg.cols - 1;
	

	for(int i=1;i<rows-1;i++)
	{
		int * data=lableImg.ptr<int>(i);
		for(int j=1;j<cols-1;j++)
		{
			if(data[j]==1)
			{
				std::stack<std::pair<int,int>> neighborPixels;
				neighborPixels.push(std::pair<int,int>(i,j));  //像素位置：<i,j>
				++label;  //没有重复的团，开始新的标签
				while(!neighborPixels.empty())
				{
					std::pair<int,int>curPixel=neighborPixels.top(); //如果与上一行中一个团有重合区域，则将上一行的那个团的标号赋给它
					int curX=curPixel.first;
				    int curY=curPixel.second;
					lableImg.at<int>(curX,curY)=label;
					
					//动态增加labelCount的size;
					if(labelCount.size()>=label+1)
					labelCount[label]++;                
					else
					{ 
						labelCount.resize(label+1);
						labelCount[label]++;
					}
					neighborPixels.pop();
					
					if(curY-1>=0&&curY-1<imgHeight&&curX>=0&&curX<imgWidth)
						if(lableImg.at<int>(curX,curY-1)==1)
						{//左边
							neighborPixels.push(std::pair<int,int>(curX,curY-1));
						}
					if(curY+1>=0&&curY+1<imgHeight&&curX>=0&&curX<imgWidth)
						if(lableImg.at<int>(curX,curY+1)==1)
						{//右边
							neighborPixels.push(std::pair<int,int>(curX,curY+1));
						}
					if(curX-1>=0&&curX-1<imgWidth&&curY>=0&&curY<imgHeight)
						if(lableImg.at<int>(curX-1,curY)==1)
						{//上边
							neighborPixels.push(std::pair<int,int>(curX-1,curY));
						}
					if(curX+1>=0&&curX+1<imgWidth&&curY>=0&&curY<imgHeight)
						if(lableImg.at<int>(curX+1,curY)==1)
						{//下边
							neighborPixels.push(std::pair<int,int>(curX+1,curY));
						}
				}
			}
		}
		
	}
	std::cout<<label<<std::endl;


	//求最大团的label值
	int maxLabelCnt=0;
	int finalLabel=1;
	for(int i=0;i<labelCount.size();i++)
	{
		if(labelCount[i]>maxLabelCnt)
		{
			maxLabelCnt=labelCount[i];
			finalLabel=i;
		}
	}

	std::cout<<maxLabelCnt<<" , "<<finalLabel<<std::endl;

	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
			// mask.at<uchar>(y,x)=mask.at<uchar>(y,x)*(255.0/3);
			//mask.at<uchar>(y,x)=mask.at<uchar>(y,x)/3*255;
			if(lableImg.at<int>(y,x)==finalLabel)
			{
				mask.at<uchar>(y,x)=1;
				castImage.at<cv::Vec3b>(y,x)[0]=castImage.at<cv::Vec3b>(y,x)[0]*0.4+cColor[0]*0.6;
				castImage.at<cv::Vec3b>(y,x)[1]=castImage.at<cv::Vec3b>(y,x)[1]*0.4+cColor[1]*0.6;
				castImage.at<cv::Vec3b>(y,x)[2]=castImage.at<cv::Vec3b>(y,x)[2]*0.4+cColor[2]*0.6;
			}
			else 
				mask.at<uchar>(y,x)=0;

			 mask.at<uchar>(y,x)*=255;

		}
	}

	//imshow("finalLabel",mask);
	//imshow("castImg",castImage);
	imwrite(m_sMaskImgPathName,mask);
	return true;
}

//Init the 0th cam in certain frame
bool LSVideoFrame::InitMaskImgFromGMM(GMM &bgdGMM,GMM &fgdGMM)
{
	//image && mask
	cv::Mat image,mask;  
	image=cv::imread(m_sColorImgPathName);
	if( image.type() != CV_8UC3 )
		CV_Error( CV_StsBadArg, "image must have CV_8UC3 type" );
	mask.create(image.size(),CV_8UC1);

	//imgWidth && imgHeight
	int imgWidth=image.cols;
	int imgHeight=image.rows;

	//rect body in first camera image...   //hog features could not work well in some situations
	int minX=(1/4.0)*imgWidth;
	int maxX=(4/5.0)*imgWidth;
	int minY=(1/25.0)*imgHeight;
	int maxY=(24/25.0)*imgHeight;

	cv::Rect rect(minX,minY,maxX-minX,maxY-minY);
	//cv::rectangle(image,rect,cvScalar(0,0,255),2);
	//cv::imshow("image",image);
	

	//Set rect in mask (init labels with rect)
	assert( !mask.empty() );
	mask.setTo( GC_BGD );
	rect.x = max(0, rect.x);
	rect.y = max(0, rect.y);
	rect.width = min(rect.width, image.cols-rect.x);
	rect.height = min(rect.height, image.rows-rect.y);
	(mask(rect)).setTo( Scalar(GC_PR_FGD) );
	
	Mat compIdxs(image.size(), CV_32SC1 );
	GMM::CheckMask( image, mask);
	GMM::InitGMMs( image, mask, bgdGMM, fgdGMM );//calculate model param(covariance,coefs, prods etc.)  //Just using the background GMM model

	//using pixels out of the rect as the candidate background pixels.
	for( int i = 0; i < 1; i++ )
	{
		GMM::AssignGMMsComponents( image, mask, bgdGMM, fgdGMM, compIdxs );  
		GMM::LearnGMMs( image, mask, compIdxs, bgdGMM, fgdGMM );
		
		cv::Point p;
		double minSource=10,maxSource=-1;
		//Find minSink and maxSink
		for( p.y = minY; p.y < maxY; p.y++ )
		{
			for( p.x = minX; p.x < maxX; p.x++)
			{
				Vec3b color = image.at<Vec3b>(p);
				double fromSource=bgdGMM(color);
				if(fromSource<minSource)
					minSource=fromSource;
				if(fromSource>maxSource)
					maxSource=fromSource;

			}
		}
		//std::cout<<"minSource:"<<minSource<<std::endl;
		//std::cout<<"maxSource:"<<maxSource<<std::endl;


		//Normalize and calculate for every pixel（Init Segment Image)
		for( p.y = minY; p.y < maxY; p.y++ )
		{
			for( p.x = minX; p.x < maxX; p.x++)
			{
				Vec3b color = image.at<Vec3b>(p);
				// calculate probability
				double fromSource;
				fromSource =  bgdGMM(color);
				double prbFgd=0;
				prbFgd=(fromSource-minSource)/(maxSource-minSource);

				if(prbFgd<0.00000001)  //0.000010
					mask.at<uchar>(p)=GC_PR_FGD;
				else
					mask.at<uchar>(p)=GC_PR_BGD;
			}
		}

	}

	//iterate and refine image
	for( int i = 0; i < 1; i++ )
	{
		GMM::AssignGMMsComponents( image, mask, bgdGMM, fgdGMM, compIdxs );  
		GMM::LearnGMMs( image, mask, compIdxs, bgdGMM, fgdGMM );
		cv::Point p;
		for( p.y = minY; p.y < maxY; p.y++ )
		{
			for( p.x = minX; p.x < maxX; p.x++)
			{
				// add node
				Vec3b color = image.at<Vec3b>(p);

				// set t-weights
				double fromSource, toSink;

				fromSource = -log( bgdGMM(color) );
				toSink = -log( fgdGMM(color) );

				if(fromSource>toSink)
					mask.at<uchar>(p)=GC_PR_FGD;
				else
					mask.at<uchar>(p)=GC_PR_BGD;
			}
		}
	}
   
	//Copy the pixel value in color image according to the mask
	Mat res;
	Mat binMask;
	//if image have been reset, then copy it; else copy the processed image
	if( mask.empty() || mask.type()!=CV_8UC1 )
		CV_Error( CV_StsBadArg, "maskImg is empty or has incorrect type (not CV_8UC1)" );
	if( binMask.empty() || binMask.rows!=mask.rows || binMask.cols!=mask.cols )
		binMask.create( mask.size(), CV_8UC1 );
	binMask = mask& 1;
	image.copyTo( res, binMask );
	//imshow("image",res);
    //cvWaitKey(0);
	


	cv::Mat castImage;
	image.copyTo(castImage);
	cv::Vec3d cColor(0,255,0);
	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
			if(mask.at<uchar>(y,x)==GC_PR_FGD)
			{
				mask.at<uchar>(y,x)=1;
				//castImage.at<cv::Vec3b>(y,x)[0]=castImage.at<cv::Vec3b>(y,x)[0]*0.4+cColor[0]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[1]=castImage.at<cv::Vec3b>(y,x)[1]*0.4+cColor[1]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[2]=castImage.at<cv::Vec3b>(y,x)[2]*0.4+cColor[2]*0.6;
			}
			else 
				mask.at<uchar>(y,x)=0;
			   // mask.at<uchar>(y,x)*=255;
		}
	}




	//erode and expansion 
	//Mat element=getStructuringElement(MORPH_ELLIPSE,Size(9,9),Point(4,4));
	//morphologyEx(mask, mask, MORPH_OPEN, element);
	//element.release();

 
	//针对粗糙的mask结果求最大连通区域    //4邻接方法
	//void Seed_Filling(const cv::Mat& binImg, cv::Mat& lableImg)   //Seed fill method
	cv::Mat binImg;
	mask.copyTo(binImg);
	cv::Mat lableImg;
	if (binImg.empty()||
		binImg.type() != CV_8UC1)
	{
		std::cout<<"binImg.type() != CV_8UC1"<<std::endl;
		return false;
	}
	lableImg.release();
	binImg.convertTo(lableImg, CV_32SC1);

	int label = 1; 
	std::vector<int> labelCount;
	labelCount.reserve(200);
	int rows = binImg.rows - 1;  
	int cols = binImg.cols - 1;

	for(int i=1;i<rows-1;i++)
	{
		int * data=lableImg.ptr<int>(i);
		for(int j=1;j<cols-1;j++)
		{
			if(data[j]==1)
			{
				std::stack<std::pair<int,int>> neighborPixels;
				neighborPixels.push(std::pair<int,int>(i,j));  //像素位置：<i,j>
				++label;  //没有重复的团，开始新的标签
				while(!neighborPixels.empty())
				{
					std::pair<int,int>curPixel=neighborPixels.top(); //如果与上一行中一个团有重合区域，则将上一行的那个团的标号赋给它
					int curX=curPixel.first;
					int curY=curPixel.second;
					lableImg.at<int>(curX,curY)=label;

					//动态增加labelCount的size;
					if(labelCount.size()>=label+1)
						labelCount[label]++;                
					else
					{ 
						labelCount.resize(label+1);
						labelCount[label]++;
					}
					neighborPixels.pop();

						if(lableImg.at<int>(curX,curY-1)==1)
						{//左边
							neighborPixels.push(std::pair<int,int>(curX,curY-1));
						}
							if(lableImg.at<int>(curX,curY+1)==1)
							{//右边
								neighborPixels.push(std::pair<int,int>(curX,curY+1));
							}
								if(lableImg.at<int>(curX-1,curY)==1)
								{//上边
									neighborPixels.push(std::pair<int,int>(curX-1,curY));
								}
									if(lableImg.at<int>(curX+1,curY)==1)
									{//下边
										neighborPixels.push(std::pair<int,int>(curX+1,curY));
									}
				}
			}
		}

	}
	//std::cout<<label<<std::endl;

	//求最大团的label值
	int maxLabelCnt=0;
	int finalLabel=1;
	for(int i=0;i<labelCount.size();i++)
	{
		if(labelCount[i]>maxLabelCnt)
		{
			maxLabelCnt=labelCount[i];
			finalLabel=i;
		}
	}

	//std::cout<<maxLabelCnt<<" , "<<finalLabel<<std::endl;

	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
			// mask.at<uchar>(y,x)=mask.at<uchar>(y,x)*(255.0/3);
			//mask.at<uchar>(y,x)=mask.at<uchar>(y,x)/3*255;
			if(lableImg.at<int>(y,x)==finalLabel)
			{
				mask.at<uchar>(y,x)=1;
				//castImage.at<cv::Vec3b>(y,x)[0]=castImage.at<cv::Vec3b>(y,x)[0]*0.4+cColor[0]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[1]=castImage.at<cv::Vec3b>(y,x)[1]*0.4+cColor[1]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[2]=castImage.at<cv::Vec3b>(y,x)[2]*0.4+cColor[2]*0.6;
			}
			else 
				mask.at<uchar>(y,x)=0;

			//mask.at<uchar>(y,x)*=255;

		}
	}

	//在得到最大连通区域，剔除噪点后，再更新一次前背景GMM。
	for( int i = 0; i < 1; i++ )
	{
		GMM::AssignGMMsComponents( image, mask, bgdGMM, fgdGMM, compIdxs );  
		GMM::LearnGMMs( image, mask, compIdxs, bgdGMM, fgdGMM );
	}


	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
			// mask.at<uchar>(y,x)=mask.at<uchar>(y,x)*(255.0/3);
			//mask.at<uchar>(y,x)=mask.at<uchar>(y,x)/3*255;
			if(mask.at<uchar>(y,x)==1)
			{
				castImage.at<cv::Vec3b>(y,x)[0]=castImage.at<cv::Vec3b>(y,x)[0]*0.4+cColor[0]*0.6;
				castImage.at<cv::Vec3b>(y,x)[1]=castImage.at<cv::Vec3b>(y,x)[1]*0.4+cColor[1]*0.6;
				castImage.at<cv::Vec3b>(y,x)[2]=castImage.at<cv::Vec3b>(y,x)[2]*0.4+cColor[2]*0.6;
				mask.at<uchar>(y,x)*=255;
			}
			else 
				mask.at<uchar>(y,x)=0;

		}
	}
	
	//cv::imshow("mask",mask);
	//cv::imshow("castImg",castImage);
	cv::imwrite(m_sMaskImgPathName,mask);
    //cvWaitKey(0);
   //std::cout<<"Init GMM with the first frame..."<<std::endl;

	return true;
}


//依据Cam 0计算所得GMM学习和估计其他Cam的分割结果
bool LSVideoFrame::GetMaskImgFromInitGMM(GMM &bgdGMM,GMM &fgdGMM)
{
	//image && mask
	cv::Mat image,mask;  
	image=cv::imread(m_sColorImgPathName);
	if( image.type() != CV_8UC3 )
		CV_Error( CV_StsBadArg, "image must have CV_8UC3 type" );
	mask.create(image.size(),CV_8UC1);

	//imgWidth& imgHeight
	int imgWidth=image.cols;
	int imgHeight=image.rows;
	Mat compIdxs(image.size(), CV_32SC1 );


	//不需要InitGMM,因为通过第一帧已经得到前背景的GMM,只需要针对当前图像更新前背景GMM
	//较为正确的做法是先将框外的背景元素“加进来”更新背景GMM，然后分别利用前景和背景GMM学习并更新

	cv::Point p;                                         //也可以考虑将开始的框加进来，这样不确定的前背景元素就相对较少
	for( p.y = 0; p.y < imgHeight; p.y++ )
	{
		for( p.x = 0; p.x < imgWidth; p.x++)
		{
			// add node
			Vec3b color = image.at<Vec3b>(p);
			// set t-weights
			double fromSource, toSink;
			fromSource = -log( bgdGMM(color) );
			toSink = -log( fgdGMM(color) );
			//fromSource =  bgdGMM(color) ;
			//toSink =  fgdGMM(color) ;
			//std::cout<<"fromSource:"<<fromSource<<std::endl;
			//std::cout<<"toSink:"<<toSink<<std::endl;
			if(fromSource>toSink)
				mask.at<uchar>(p)=GC_PR_FGD;
			else
				mask.at<uchar>(p)=GC_PR_BGD;
		}
	}

  /* 暂注
	Mat res;
	Mat binMask;
	//if image have been reset, then copy it; else copy the processed image
	if( mask.empty() || mask.type()!=CV_8UC1 )
		CV_Error( CV_StsBadArg, "maskImg is empty or has incorrect type (not CV_8UC1)" );
	if( binMask.empty() || binMask.rows!=mask.rows || binMask.cols!=mask.cols )
		binMask.create( mask.size(), CV_8UC1 );
	binMask = mask& 1;
	image.copyTo( res, binMask );

	imshow("image",res);
    imwrite(m_sMaskImgPathName,res);
	cvWaitKey(0);
   */

	cv::Mat castImage;
	image.copyTo(castImage);
	cv::Vec3d cColor(0,255,0);

	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
			if(mask.at<uchar>(y,x)==GC_PR_FGD)
			{
				mask.at<uchar>(y,x)=1;
				
			}
			else 
				mask.at<uchar>(y,x)=0;
		}
	}
	
	//erode and expansion 
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(9,9),Point(4,4));
	morphologyEx(mask, mask, MORPH_OPEN, element);
	element.release();


	//针对粗糙的mask结果求最大连通区域    //4邻接方法
	//void Seed_Filling(const cv::Mat& binImg, cv::Mat& lableImg)   //种子填充法
	cv::Mat binImg;
	mask.copyTo(binImg);
	cv::Mat lableImg;
	if (binImg.empty()||
		binImg.type() != CV_8UC1)
	{
		std::cout<<"binImg.type() != CV_8UC1"<<std::endl;
		return false;
	}
	lableImg.release();
	binImg.convertTo(lableImg, CV_32SC1);

	
	int label = 1; 
	std::vector<int> labelCount;
	labelCount.reserve(200);
	int rows = binImg.rows - 1;  
	int cols = binImg.cols - 1;

	for(int i=1;i<rows-1;i++)
	{
		int * data=lableImg.ptr<int>(i);
		for(int j=1;j<cols-1;j++)
		{
			if(data[j]==1)
			{
				std::stack<std::pair<int,int>> neighborPixels;
				neighborPixels.push(std::pair<int,int>(i,j));  //像素位置：<i,j>
				++label;  //没有重复的团，开始新的标签
				while(!neighborPixels.empty())
				{
					std::pair<int,int>curPixel=neighborPixels.top(); //如果与上一行中一个团有重合区域，则将上一行的那个团的标号赋给它
					int curX=curPixel.first;
					int curY=curPixel.second;
					lableImg.at<int>(curX,curY)=label;

					//动态增加labelCount的size;
					if(labelCount.size()>=label+1)
						labelCount[label]++;                
					else
					{ 
						labelCount.resize(label+1);
						labelCount[label]++;
					}
					neighborPixels.pop();

					if(lableImg.at<int>(curX,curY-1)==1)
					{//左边
						neighborPixels.push(std::pair<int,int>(curX,curY-1));
					}
					if(lableImg.at<int>(curX,curY+1)==1)
					{//右边
						neighborPixels.push(std::pair<int,int>(curX,curY+1));
					}
					if(lableImg.at<int>(curX-1,curY)==1)
					{//上边
						neighborPixels.push(std::pair<int,int>(curX-1,curY));
					}
					if(lableImg.at<int>(curX+1,curY)==1)
					{//下边
						neighborPixels.push(std::pair<int,int>(curX+1,curY));
					}
				}
			}
		}

	}
	//std::cout<<label<<std::endl;


	//求最大团的label值
	int maxLabelCnt=0;
	int finalLabel=1;
	for(int i=0;i<labelCount.size();i++)
	{
		if(labelCount[i]>maxLabelCnt)
		{
			maxLabelCnt=labelCount[i];
			finalLabel=i;
		}
	}
	//std::cout<<maxLabelCnt<<" , "<<finalLabel<<std::endl;

	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
			if(lableImg.at<int>(y,x)==finalLabel)
			{
				mask.at<uchar>(y,x)=1;
				//castImage.at<cv::Vec3b>(y,x)[0]=castImage.at<cv::Vec3b>(y,x)[0]*0.4+cColor[0]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[1]=castImage.at<cv::Vec3b>(y,x)[1]*0.4+cColor[1]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[2]=castImage.at<cv::Vec3b>(y,x)[2]*0.4+cColor[2]*0.6;
			}
			else 
				mask.at<uchar>(y,x)=0;

			//mask.at<uchar>(y,x)*=255;

		}
	}

   /* 在多帧上计算会有bug
	//在得到最大连通区域，剔除噪点后，再更新一次前背景GMM。
	for( int i = 0; i < 1; i++ )
	{
		GMM::AssignGMMsComponents( image, mask, bgdGMM, fgdGMM, compIdxs );  
		GMM::LearnGMMs( image, mask, compIdxs, bgdGMM, fgdGMM );             //Bug may here
	}
	*/

	for( int y = 0; y < imgHeight; y++ )
	{
		for( int x = 0; x < imgWidth; x++)
		{
			if(mask.at<uchar>(y,x)==1)
			{
				//castImage.at<cv::Vec3b>(y,x)[0]=castImage.at<cv::Vec3b>(y,x)[0]*0.4+cColor[0]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[1]=castImage.at<cv::Vec3b>(y,x)[1]*0.4+cColor[1]*0.6;
				//castImage.at<cv::Vec3b>(y,x)[2]=castImage.at<cv::Vec3b>(y,x)[2]*0.4+cColor[2]*0.6;
				mask.at<uchar>(y,x)*=255;
			}
			else 
				mask.at<uchar>(y,x)=0;

		}
	}
	//cv::imshow("mask",mask);
	//cv::imshow("castImg",castImage);
	cv::imwrite(m_sMaskImgPathName,mask);
	//std::cout<<"GetMaskImgFromInitGMM..."<<std::endl;
	
	return true;
}





bool LSVideoFrame::GenerateRectFromHogDetect()
{
	cv::Mat currentImage;
	currentImage=cv::imread(m_sColorImgPathName);
	if(currentImage.empty())
	{
		cerr << "current image is empty: " <<m_sColorImgPathName<< endl;
		exit(EXIT_FAILURE);
	}
	
	// 1. 定义HOG对象  
    cv::HOGDescriptor hog; //采用默认参数
   
	// 2.设置SVM分类器
	hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());   //There is a bug!

	//3 在测试图像上检测行人区域
	std::vector<cv::Rect> regions;
	hog.detectMultiScale(currentImage,regions,0,cv::Size(8,8),cv::Size(32,32),1.05,1);
	
	for(size_t i=0;i<regions.size();i++)
	{
		cv::rectangle(currentImage,regions[i],cv::Scalar(0,0,255),2);
	}

	cv::imshow("hog",currentImage);
	cv::waitKey(0);
	
	std::cout<<"Hog Detect...."<<std::endl;
	return true;
}



//NEW ADD
bool LSVideoFrame::LoadMaskImg()
{
	//std::cout<<"Load mask image"<<std::endl;
	if(m_pMaskImg != NULL){
		return true;
	}
	CxImage image;
	//std::cout<<"m_sMaskImgPathName: "<<m_sMaskImgPathName<<std::endl;
	bool LoadSuc;
	if(image.Load(m_sMaskImgPathName.c_str()) == false){
		std::cout << "Can not find mask file: " << m_sMaskImgPathName << std::endl;
		return false;
	}
    const int imgWidth = image.GetWidth();
	const int imgHeight = image.GetHeight();
	m_pMaskImg= new ZByteImage(imgWidth, imgHeight);

	for(int y = 0;y<imgHeight;++y)
		for(int x = 0;x<imgWidth;++x)
		{
			RGBQUAD  color = image.GetPixelColor(x,imgHeight - 1 - y,true);
			//here may be a bug for the chanle is not 4
			int tmp = ((int)color.rgbRed  +  (int)color.rgbGreen  +  (int)color.rgbBlue );
			m_pMaskImg->at(x,y) = tmp / 3;

		}
	//CxImageToZImage(image, *m_pMaskImg);
	return true;
}





/*ORG
bool LSVideoFrame::LoadMaskImg(int corrode)
{
	//std::cout<<"Load mask image"<<std::endl;
   
	if(m_pMaskImg != NULL){
		return true;
	}
	
	CxImage image;

	std::cout<<"m_sMaskImgPathName: "<<m_sMaskImgPathName<<std::endl;

	if(image.Load(m_sMaskImgPathName.c_str()) == false){
		std::cout << "Can not find mask file: " << m_sMaskImgPathName << std::endl;
		return false;
	}
	m_pMaskImg= new ZByteImage(image.GetWidth(), image.GetHeight());
	CxImageToZImage(image, *m_pMaskImg);
	if(corrode == 0)
		return true;

	int width = image.GetWidth();
	int height = image.GetHeight();

     //all eight directions
	const int dir[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
	for(int ci = 0; ci<corrode; ci++){    //corrode' meaning is not so clearly		
		std::vector<int> points;
		for(int y = 0; y< height; y++){
			for(int x = 0; x < width; x++){
				if(m_pMaskImg->GetPixel(x, y, 0) <= LEASTFRONT)   //LEASTFRONT=2
					continue;
				bool ok = true;
				for(int i= 0; i < 8 && ok; i++){
					int xi = x + dir[i][0];
					int yi = y + dir[i][1];
					if(xi < 0 || xi >= width || yi < 0 || yi >= height)
						continue;
					if(m_pMaskImg->GetPixel(xi, yi, 0) <= LEASTFRONT)
						ok = false;
				}
				if(ok == false){
					points.push_back(x);
					points.push_back(y);
				}
			}
		}
		for(int i = 0; i<points.size(); i+= 2){
			m_pMaskImg->at(points[i], points[i+1]) = 0;
		}
	}
	return true;
}
*/

bool LSVideoFrame::LoadColorImg()
{
	if(m_pColorImg != NULL){
		delete m_pColorImg;
		m_pColorImg = NULL;
	}

	CxImage image;
	//image.Load(m_sImgPathName.c_str(), CXIMAGE_FORMAT_PNG);
	//image.Load(m_sImgPathName.c_str(), CXIMAGE_FORMAT_PNM);
	if(image.Load(m_sColorImgPathName.c_str()) == false){
		std::cout << "Can not find color image file: " << m_sColorImgPathName << std::endl;
		return false;
	}
	m_pColorImg = new ZByteImage(image.GetWidth(), image.GetHeight(), 3);
	CxImageToZImage(image, *m_pColorImg);
	return true;
}

//NEW ADD

bool LSVideoFrame::LoadMinLabelImg()
{
	if(m_pMinLabelImg != NULL){
		delete m_pMinLabelImg;
		m_pMinLabelImg = NULL;
	}

	std::string m_sMinLabelImg;
	std::string str =  m_sName + std::string(".png"); 

	m_sMinLabelImg= s_TempDir[m_iCameraId] + "MinDsp_" + str;

	//std::cout<<"m_sMinLabelImg: "<<m_sMinLabelImg<<std::endl;
	CxImage image;
	//image.Load(m_sImgPathName.c_str(), CXIMAGE_FORMAT_PNG);
	//image.Load(m_sImgPathName.c_str(), CXIMAGE_FORMAT_PNM);
//	RGBQUAD color;
//	color=image.GetPixelColor(5,7,true );
	
	if(image.Load(m_sMinLabelImg.c_str()) == false){
		std::cout << "Can not find MinLabel image file: " << m_sMinLabelImg << std::endl;
		return false;
	}
	m_pMinLabelImg = new ZIntImage(image.GetWidth(), image.GetHeight(), 1);
	CxImageToZImage(image, *m_pMinLabelImg,1);  //此行程序需要修改
	return true;
}

//NEW ADD
bool LSVideoFrame::LoadMaxLabelImg()
{
	if(m_pMaxLabelImg != NULL){
		delete m_pMaxLabelImg;
		m_pMaxLabelImg = NULL;
	}

	std::string m_sMaxLabelImg;
	std::string str =  m_sName + std::string(".png"); 

	m_sMaxLabelImg= s_TempDir[m_iCameraId] + "MaxDsp_" + str;

	//std::cout<<"m_sMinLabelImg: "<<m_sMinLabelImg<<std::endl;
	CxImage image;
	//image.Load(m_sImgPathName.c_str(), CXIMAGE_FORMAT_PNG);
	//image.Load(m_sImgPathName.c_str(), CXIMAGE_FORMAT_PNM);

	if(image.Load(m_sMaxLabelImg.c_str()) == false){
		std::cout << "Can not find MaxLabel image file: " << m_sMaxLabelImg << std::endl;
		return false;
	}
	m_pMaxLabelImg = new ZIntImage(image.GetWidth(), image.GetHeight(), 1);
	CxImageToZImage(image, *m_pMaxLabelImg,1);
	return true;
}

//NEW ADD   //不确定 zByteImage和ZFloatImage是否取值相同
void LSVideoFrame::GenerateVariance(ZByteImage& Img,ZFloatImage& varianceImg,int halfWin /* = 2 */)
{
	const int channels = Img.GetChannel();
	const int width = Img.GetWidth();
	const int height = Img.GetHeight();
	const double  invPixelNum = 1.0 /(  (halfWin * 2 + 1) * (halfWin * 2 + 1)  );

	//以7*7窗口大小取平均值作为当前像素值
	ZFloatImage  avgImg(width,height,channels);
	for(int y = halfWin;y<height - halfWin;++y)
		for(int x = halfWin;x<width - halfWin;++x)
		{
			for(int iChannel = 0;iChannel<channels;++iChannel)
			{
				float sum = 0.0;
				for(int offsetY = -halfWin;offsetY<=halfWin;++offsetY)
					for(int offsetX = -halfWin;offsetX<=halfWin;++offsetX)
						sum += Img.at(x + offsetX,y + offsetY,iChannel);
				sum *=invPixelNum;
				avgImg.at(x,y,iChannel) = sum;
			}
		}
		varianceImg.MakeZero();

		for(int y = halfWin;y<height - halfWin;++y)
			for(int x = halfWin;x<width - halfWin;++x)
			{
				float sum = 0.0;
				for(int iChannel = 0;iChannel<channels;++iChannel)
				{
					for(int offsetY = -halfWin;offsetY<=halfWin;++offsetY)
						for(int offsetX = -halfWin;offsetX<=halfWin;++offsetX)
						{
							float tmp = Img.at(x + offsetX,y + offsetY,iChannel) - 
								avgImg.at(x + offsetX,y + offsetY,iChannel);
							sum += tmp * tmp;
						}
				}

				sum *=invPixelNum;
				varianceImg.at(x,y) = sum/channels;
			}

}





void LSVideoFrame::LoadDepthImg()
{
	if(m_pDspImg != NULL){
		delete m_pDspImg;
		m_pDspImg = NULL;
	}

	TCHAR path[100];
	FILE * fp;

	m_pDspImg = new ZFloatImage(GetImgWidth(), GetImgHeight(), 1);

	sprintf(path, "%s_depth%d.raw", s_DataDir[m_iCameraId].c_str(), m_iFrameId);
	fp = fopen(path, "rb");
	if(fp){
		fread(m_pDspImg->GetMap(), sizeof(BYTE), m_pDspImg->GetSize(), fp);
		fclose(fp);
	}
	else{
		std::cout << "ERROR:Can not load dspImg:"<< path << std::endl;
	}
}

/*
//未xie完
void LSVideoFrame::LoadOptFlwImgU(const std::string & foreFrame,const std::string & backFrame)
{
	TCHAR path[100];
	FILE * fp;
	sprintf(path, "%sopticalFlow_u_%s_%s.raw", s_OptFlwDir[m_iCameraId].c_str(),foreFrame.c_str(),backFrame.c_str());
    std::cout<<_T(path)<<std::endl;


}

void LSVideoFrame::LoadOptFlwImgV(const std::string & foreFrame,const std::string & backFrame)
{

}

*/



void LSVideoFrame::Clear_depth(){
	if(	m_pDspImg != NULL){
		delete m_pDspImg;
		m_pDspImg = NULL;
	}
}

void LSVideoFrame::GetCameraCoordFrmWorldCoord(Wml::Vector3d& c_pt3d, Wml::Vector3d& w_pt3d){
	m_pLsCameraParameter->GetCameraCoordFrmWorldCoord(c_pt3d, w_pt3d);
}


void LSVideoFrame::GetWorldCoordFrmImgCoord( double u, double v, double dsp, Wml::Vector3d& w_pt3d )
{
	m_pLsCameraParameter->GetWorldCoordFrmImgCoord(u, v, dsp, w_pt3d);
}


void LSVideoFrame::GetImgCoordFrmWorldCoord( double& out_u, double& out_v, double& out_dsp, Wml::Vector3d& w_pt3d )
{
	m_pLsCameraParameter->GetImgCoordFrmWorldCoord(out_u, out_v, out_dsp, w_pt3d);
}



// for VisualHull （不清楚此函数的含义）
void LSVideoFrame::SetFaceVis( Wml::Vector3d* faceNormal )
{
	const double EPSIRON = 1.0e-5f;
	Wml::Matrix4d ExtrinsicMat;
	ExtrinsicMat(3,0) = 0.0;
	ExtrinsicMat(3,1) = 0.0;
	ExtrinsicMat(3,2) = 0.0;
	ExtrinsicMat(3,3) = 1.0;

	for(int j = 0; j<3; j++)
	{
		for(int i = 0; i<3; i++)
		{	
			ExtrinsicMat(j,i)=m_pLsCameraParameter->m_R(j,i);    // (camera  parameter pointer just relate to single frame)
		}
		ExtrinsicMat(j,3)=m_pLsCameraParameter->m_T[j];
	}

	//liql2007 可能有问题
	//Wml::Matrix4d  InvExtrinsicMat_T = ( ExtrinsicMat.Inverse() ).Transpose();
	Wml::Matrix4d  InvExtrinsicMat_T = ExtrinsicMat.Inverse();
	InvExtrinsicMat_T = InvExtrinsicMat_T.Transpose();	


//Test 
/*
	  for(int j=0;j<4;j++)
	  {
		  for(int i=0;i<4;i++)
		  {
			  std::cout<< InvExtrinsicMat_T[i][j]<<" ";
		  }
		  std::cout<<std::endl;
	  }
*/
	//不清楚以下代码含义：
	for(int i = 0;i<6;i++)
	{
		Wml::Vector4d  faceNormal_1 = Wml::Vector4d(faceNormal[i][0],faceNormal[i][1],faceNormal[i][2],1.0f);
		faceNormal_1  = InvExtrinsicMat_T * faceNormal_1;
		double t = faceNormal_1[2]/faceNormal_1[3];
		if(t<EPSIRON)
			m_FaceVis[i] = true;
		else m_FaceVis[i] = false;
	}
  	//std::cout<<"SetFaceVis..."<<std::endl;
}

bool LSVideoFrame::IsInMask( Wml::Vector3d& w_pt3d, int Apron)
{
	double u, v, dsp;
	m_pLsCameraParameter->GetImgCoordFrmWorldCoord(u, v, dsp, w_pt3d);

/*
	std::cout<<"M_K:"<<std::endl;
	for (int i=0;i<3;i++)
		for(int j=0;j<3;j++)
     std::cout<<m_pLsCameraParameter->m_K[i][j]<<" ";
	std::cout<<std::endl;
	
	std::cout<<"M_R:"<<std::endl;
	for (int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			std::cout<<m_pLsCameraParameter->m_R[i][j]<<" ";
	std::cout<<std::endl;
	
	std::cout<<"M_T:"<<std::endl;
	for (int i=0;i<3;i++)
			std::cout<<m_pLsCameraParameter->m_T[i]<<" ";
	std::cout<<std::endl;



	std::cout<<"w_pt3d:"<<w_pt3d[0]<<w_pt3d[1]<<w_pt3d[2]<<std::endl;
	std::cout<<"Apron:"<<Apron<<std::endl;

	int tmpu = (int) u;
	int tmpv = (int) v;

	std::cout<<"tmpu:"<<tmpu<<std::endl;
	std::cout<<"tmpv:"<<tmpv<<std::endl;
*/

	int tmpu = (int) u;
	int tmpv = (int) v;

	const int maxApron = 300;
	if( tmpu<=0  ||  tmpv<=0  || (tmpu + 1 >= m_iWidth)  || (tmpv + 1 >= m_iHeight) ){
		if(Apron<=0)		
			return false;	

		if( Apron>maxApron )
			Apron = maxApron;

		if(tmpu>=-Apron  && tmpu<=0)
			tmpu = 0;
		else if(tmpu <=(m_iWidth + Apron)  && tmpu+1>=m_iWidth) 
			tmpu = m_iWidth - 1;
		else if( (tmpu >(m_iWidth + Apron)) || (tmpu<-Apron) )
			return false;


		if(tmpv>=-Apron  && tmpv<=0)
			tmpv = 0;
		else if(tmpv <=(m_iHeight + Apron)  && tmpv+1>=m_iHeight) 
			tmpv = m_iHeight - 1;
		else if( (tmpv >(m_iHeight + Apron)) || (tmpv<-Apron) )
			return false;
		return IsInMask(tmpu, tmpv);
	}
	return  ( IsInMask(tmpu, tmpv)|| IsInMask(tmpu+1, tmpv)|| IsInMask(tmpu, tmpv+1)|| IsInMask(tmpu+1, tmpv+1) );
	//return  ( IsInMask(tmpu, tmpv)|| IsInMask(tmpu+1, tmpv)|| IsInMask(tmpu+1, tmpv+1)|| IsInMask(tmpu+1, tmpv+1) );
}

bool LSVideoFrame::IsInMask(int u, int v){
	if(m_pMaskImg == NULL){
		std::cout << "ERROR: please load maskImg first!"<<std::endl;
		system("pause");
	}
	return m_pMaskImg->GetPixel(u, v, 0) > LEASTFRONT;
}


//NEW-ADD
bool LSVideoFrame::inMask(float x,float y,float z,int Apron )
{
	double u, v, dsp;
	m_pLsCameraParameter->GetImgCoordFrmWorldCoord(u, v, dsp,  Wml::Vector3d(x,y,z));
//	std::cout<<"M_K:"<<std::endl;
//	for (int i=0;i<3;i++)
//		for(int j=0;j<3;j++)
//			std::cout<<mCameraFrame->m_K[i][j]<<" ";
//	std::cout<<std::endl;
//
//	std::cout<<"M_R:"<<std::endl;
//	for (int i=0;i<3;i++)
//		for(int j=0;j<3;j++)
//			std::cout<<mCameraFrame->m_objAbsTransformMG(i,j)<<" ";
//	std::cout<<std::endl;
//
//	std::cout<<"M_T:"<<std::endl;
//	for (int i=0;i<3;i++)
//		std::cout<<mCameraFrame->m_objAbsTransformMG(i,3)<<" ";
//	std::cout<<std::endl;
//
//
//	std::cout<<"w_pt3d:"<<x<<y<<z<<std::endl;
//	std::cout<<"Apron:"<<Apron<<std::endl;
//
//	int tmpu = (int) uv.X();
//	int tmpv = (int) uv.Y();
//
//	std::cout<<"tmpu:"<<tmpu<<std::endl;
//	std::cout<<"tmpv:"<<tmpv<<std::endl;
//
	int tmpu = (int) u;
	int tmpv = (int) v;


   //enlarge the range a voxel cast on the image( through enlarge Apron from image edge)
	const int maxApron = 300;
	if( tmpu<=0  ||  tmpv<=0  || (tmpu + 1>=m_iWidth)  || (tmpv + 1>=m_iHeight) )
	{
		if(Apron<=0)		
			return false;	
		else
		{
			if( Apron>maxApron ) Apron = maxApron;

			if(tmpu>=-Apron  && tmpu<=0)
				tmpu = 0;
			else if(tmpu <=(m_iWidth + Apron)  && tmpu+1>=m_iWidth) 
				tmpu = m_iWidth - 1;
			else if( (tmpu >(m_iWidth + Apron)) || (tmpu<-Apron) )
				return false;
			

			if(tmpv>=-Apron  && tmpv<=0)
				tmpv = 0;
			else if(tmpv <=(m_iHeight + Apron)  && tmpv+1>=m_iHeight) 
				tmpv = m_iHeight - 1;
			else if( (tmpv >(m_iHeight + Apron)) || (tmpv<-Apron) )
				return false;
			bool *p = m_pMaskMap + tmpu + tmpv * m_iWidth;
			return *p;
		}
	}
	bool *p = m_pMaskMap + tmpu + tmpv * m_iWidth;
	return  (  *p || *(p+1) || *(p + m_iWidth) || *(p + m_iWidth + 1) );  //为什么返回的以p为左上角的四个元素（而不是p为中心的4连通或者8连通）
}

//NEW ADD
void LSVideoFrame::setup(LSVideoFrame* cameraFrame)
{

	ZByteImage& alphaImage = *cameraFrame->m_pMaskImg;
	m_iWidth = alphaImage.GetWidth();
	m_iHeight = alphaImage.GetHeight();
	m_pMaskMap = new bool [m_iWidth * m_iHeight ];
	for(int py = 0;py  < m_iHeight;py++)
		for(int px = 0;px < m_iWidth;px++)
		{
			unsigned char tmp = alphaImage.GetPixel( px,py,0 );  
			m_pMaskMap[ px +py * m_iWidth ] = ( tmp>LEASTFRONT );  //按遍历行方式存储
		}
	//	m_pCameraFrame= cameraFrame;
#define   kxldebug
#ifdef kxldebug
	//	std::cout<<"Load Image's Mask"<<cameraFrame->m_sColorImgPathName<<"\n";	
#endif
		
}

//NEW- Change VisualHull Constraints
void LSVideoFrame::InitDspConstrain( std::vector<IdxTraingle>& TriangleList, std::vector<Wml::Vector3f>& VertexList ){

	//get the maxdepth,mindepth in this view
	m_fMaxDsp*=1.0001f;   //与KXL代码不同（这里未用到depth范围表示，而是用disparity范围），因此KXL的depth扩大等同于这里的disparity缩小
	m_fMinDsp*=0.9999f;
	float depthMin=1/m_fMaxDsp;
	float depthMax=1/m_fMinDsp;
 /*
	//NEW-ADD	
	float* mDepthMax;//tmp  var
	float* mDepthMin;//tmp  var
	mDepthMax = mDepthMin = 0;

	mDepthMax = new float [m_iWidth * m_iHeight ];
	mDepthMin = new float [m_iWidth* m_iHeight];


	//CO printf("set mindepth :%.6f\n",depthMin);
	//CO printf("set maxdepth :%.6f\n",depthMax);
//此步骤之前无大问题，DepthMin和DepthMax值差异在0.0001范围内
	for(int y=0;y<m_iHeight;y++)
	{
		for(int x=0;x<m_iWidth;x++)
		{
			int idx = x + y * m_iWidth;
			mDepthMin[idx]=depthMax;
			mDepthMax[idx]=depthMin;
		}
	}
	//SoftRasterize softRasterize( this );
	SoftRasterize  softRasterize( mDepthMin,mDepthMax,m_iWidth,m_iHeight,this);
	softRasterize.start( TriangleList,VertexList);

	for(int y = 0;y<m_iHeight;y++)
		for(int x = 0;x<m_iWidth;x++)
		{
			int idx = x + y * m_iWidth;
			if( mDepthMin[idx]<depthMin )
				mDepthMin[idx] =depthMin;
			if( depthMax<mDepthMax[idx] )
				mDepthMax[idx] = depthMax;
		}

		if(m_pMaskImg == NULL)
			LoadMaskImg();

		float tmpMin = 1e6f;
		float tmpMax = 1e-6f;
		//get a more tight bound min max dsp
		//const float maxZ = 1.0 / curMinDsp;
		//const float minZ = 1.0 / curMaxDsp;
		for(int y=0; y<m_iHeight; ++y)
			for(int x=0; x<m_iWidth; ++x)
				if(IsInMask(x,y) )
				{
					int idx = x + y * m_iWidth;
					//if( maxZ<mDepthMax[ idx ] )
					//	mDepthMax[ idx ] = maxZ;
					//if( minZ>mDepthMin[ idx ] )
					//	mDepthMin[ idx ] = minZ;
					float smallDsp = 1.0 / mDepthMax[ idx ];
					float bigDsp = 1.0 / mDepthMin[idx];
					tmpMax = max(bigDsp,tmpMax);
					tmpMin = min(smallDsp,tmpMin);
				}

				m_fMaxDsp = min(m_fMaxDsp, tmpMax) * 1.05F;     
				m_fMinDsp = max(m_fMinDsp, tmpMin) * 0.95F;    
			    std::cout<<"_m_fMinDsp: "<<m_fMinDsp<<",_m_fMaxDsp: "<<m_fMaxDsp<<std::endl;

				//pRefFrame->minDsp = curMinDsp;
				//pRefFrame->maxDsp = curMaxDsp;
 */

	InitMinDepthImg(depthMax);
	InitMaxDepthImg(depthMin);
	//	std::cout<<"m_fMinDsp: "<<m_fMinDsp<<" , "<<"m_fMaxDsp: "<<m_fMaxDsp<<std::endl;
	//从这个步骤开始，中间执行过程可能与KXL有细微不同
	SoftRasterize softRasterize( this );
	softRasterize.start( TriangleList, VertexList);   //问题出在TriangleList, VertexList中。（形状应该是不同）

	//和KXL程序一样，是用depth表达的
	for(int y = 0;y<m_iHeight;y++)
	{       
		for(int x = 0;x<m_iWidth;x++)
		{
			if( m_pDepthMin->at(x, y) < depthMin )    
			{
				m_pDepthMin->at(x, y) = depthMin;
			}

			if( m_pDepthMax->at(x, y) > depthMax)
			{
				m_pDepthMax->at(x, y) = depthMax;
			}

		}
	}

	//与KXL程序相比，缺少debug: 存储m_pMaxDspImg和m_pMinDspImg.
	// _CalDepthConstrain over!
	if(m_pMaskImg == NULL)
		LoadMaskImg();

	float tmpMin = 1e6f; 
	float tmpMax = 1e-6f;
	//get a more tight bound min max dsp
	//const float maxZ = 1.0 / curMinDsp;
	//const float minZ = 1.0 / curMaxDsp;
	for(int y = 0;y<m_iHeight;y++){
		for(int x = 0;x<m_iWidth;x++){
			if(IsInMask(x, y) == false){
				continue;
			}

			float smallDsp = 1.0 / m_pDepthMax->at(x,y);
			float bigDsp = 1.0 / m_pDepthMin->at(x,y);
			tmpMax = max(bigDsp,tmpMax);
			tmpMin = min(smallDsp,tmpMin);
		}
	}
	
	//在这个过程中m_pMinDspImg和m_pMaxDspImg发生了改变(应该是visuall约束的两行代码引起的），导致后两个值改变
	m_fMaxDsp = min(m_fMaxDsp, tmpMax) * 1.05F;     
	m_fMinDsp = max(m_fMinDsp, tmpMin) * 0.95F;    
    std::cout<<"_m_fMinDsp: "<<m_fMinDsp<<",_m_fMaxDsp: "<<m_fMaxDsp<<std::endl;

	/*
	//在这步骤输出disparity范围，大部分和KXL的程序一致，但有30%disparity 范围比KXL程序小
	std::cout << "Set mindepth:" << 1.0F / m_fMaxDsp << "(1/" << m_fMaxDsp << ")"<<std::endl;
	std::cout << "Set maxdepth:" << 1.0F / m_fMinDsp << "(1/" << m_fMinDsp << ")"<<std::endl;

	std::cout<<"InitDspConstrain over..."<<std::endl;
	*/
	//if(CSystemParameters::GetInstance()->GetIfGenerateTempResult()){
	//	ZFloatImage  minDspImg,maxDspImg;
	//	minDspImg.Create( m_iWidth,m_iHeight );
	//	maxDspImg.Create( m_iWidth,m_iHeight );

	//	for(int y = 0; y < m_iHeight; y++){
	//		for(int x = 0; x < m_iWidth; x++){	
	//			//minDepthImg.at(x, y) = pFrame->m_pMinDepthImg->at(x, y) - depthMin;
	//			//maxDepthImg.at(x, y) = pFrame->m_pMaxDepthImg->at(x, y) - depthMin;
	//			if(m_pMinDspImg->at(x, y) >= m_fMaxDsp)
	//				minDspImg.at(x, y) = 0;
	//			else
	//				minDspImg.at(x, y) = m_pMinDspImg->at(x, y) - m_fMinDsp;

	//			if(m_pMaxDspImg->at(x, y) <= m_fMinDsp)
	//				maxDspImg.at(x, y) = 0;
	//			else
	//				maxDspImg.at(x, y) = m_pMaxDspImg->at(x, y) - m_fMinDsp;
	//		}

	//	}
	//	CxImage image;
	//	ZImageToCxImage(minDspImg, image, 255.0/( m_fMaxDsp - m_fMinDsp) );
	//	//ZImageToCxImage(minDepthImg, image, 255.0F /( depthMax - depthMin) );
	//	std::tstring str0 = s_TempDir[m_iCameraId] + _T("minDsp_") + m_sName + _T(".png");
	//	image.Save(str0.c_str(), CxImageFuns::GetImageFileType(str0));

	//	ZImageToCxImage(maxDspImg, image, 255.0/( m_fMaxDsp - m_fMinDsp ) );
	//	//ZImageToCxImage(maxDepthImg, image, 255.0F /( depthMax - depthMin) );
	//	str0 = s_TempDir[m_iCameraId] + _T("maxDsp_") + m_sName + _T(".png");
	//	image.Save(str0.c_str(), CxImageFuns::GetImageFileType(str0));
	//}
	
}



/*
// ORG 2014/12/09
void LSVideoFrame::InitDspConstrain( std::vector<IdxTraingle>& TriangleList, std::vector<Wml::Vector3f>& VertexList ){

	//get the maxdepth,mindepth in this view
	// SetDspRange(m_fMinDsp*1.1,m_fMaxDsp*0.9);

	/////////////////////////////////////////////////////////////////////////
	InitMinDspImg(m_fMaxDsp);    //init min dsp image  0.054863989
	InitMaxDspImg(m_fMinDsp);    //init max dsp image  0.033917766

	std::cout<<"m_fMinDsp: "<<m_fMinDsp<<" , "<<"m_fMaxDsp: "<<m_fMaxDsp<<std::endl;

	SoftRasterize softRasterize( this );
	softRasterize.start( TriangleList, VertexList);
	//Visuall hull 约束的过程不太懂！！！

	for(int y = 0;y<m_iHeight;y++){
		for(int x = 0;x<m_iWidth;x++){
			if( m_pMinDspImg->at(x, y) < m_fMinDsp )    // m_pMinDspImg init is -1
			{
				m_pMinDspImg->at(x, y) = m_fMinDsp;
				//std::cout<<"x: "<<x<<",y: "<<y<<":"<<m_pMinDspImg->at(x,y)<<std::endl;
			}

			if( m_pMaxDspImg->at(x, y) > m_fMaxDsp)
			{
				m_pMaxDspImg->at(x, y) = m_fMaxDsp;
				// std::cout<<"x: "<<x<<",y: "<<y<<":"<<m_pMaxDspImg->at(x,y)<<std::endl;
			}

		}
	}
	if(m_pMaskImg == NULL)
		LoadMaskImg();

	float fMinDsp = m_fMaxDsp;  //0.054863989
	float fMaxDsp = m_fMinDsp; //0.033917766
	for(int y = 0;y<m_iHeight;y++){
		for(int x = 0;x<m_iWidth;x++){
			if(IsInMask(x, y) == false){
				continue;
			}
			//m_pMinDspImg->at(x, y) *= 0.9;
			//m_pMaxDspImg->at(x, y) *= 1.1;
			if( m_pMinDspImg->at(x, y) < fMinDsp )
			{
				fMinDsp = m_pMinDspImg->at(x, y);
				//std::cout<<"x: "<<x<<",y: "<<y<<",m_pMinDspImg->at(x,y): "<<m_pMinDspImg->at(x,y)<<std::endl;;
			}

			if( m_pMaxDspImg->at(x, y) > fMaxDsp)
			{
				fMaxDsp = m_pMaxDspImg->at(x, y);
				//	std::cout<<"x: "<<x<<",y: "<<y<<",m_pMaxDspImg->at(x,y): "<<m_pMaxDspImg->at(x,y)<<std::endl;
			}
		}
	}
	//在这个过程中m_pMinDspImg和m_pMaxDspImg发生了改变(应该是visuall约束的两行代码引起的），导致后两个值改变
	m_fMaxDsp = min(m_fMaxDsp, fMaxDsp) * 1.05F;    // why? 0.054863989      0.050336245  
	m_fMinDsp = max(m_fMinDsp, fMinDsp) * 0.95F;    //      0.033917766      0.035765123

	//m_fMaxDsp = min(m_fMaxDsp, fMaxDsp) ;    // smaller the dsp range
	//m_fMinDsp = max(m_fMinDsp, fMinDsp) ;

	std::cout << "Set mindepth:" << 1.0F / m_fMaxDsp << "(1/" << m_fMaxDsp << ")"<<std::endl;
	std::cout << "Set maxdepth:" << 1.0F / m_fMinDsp << "(1/" << m_fMinDsp << ")"<<std::endl;

	std::cout<<"InitDspConstrain over..."<<std::endl;

	//if(CSystemParameters::GetInstance()->GetIfGenerateTempResult()){
	//	ZFloatImage  minDspImg,maxDspImg;
	//	minDspImg.Create( m_iWidth,m_iHeight );
	//	maxDspImg.Create( m_iWidth,m_iHeight );

	//	for(int y = 0; y < m_iHeight; y++){
	//		for(int x = 0; x < m_iWidth; x++){	
	//			//minDepthImg.at(x, y) = pFrame->m_pMinDepthImg->at(x, y) - depthMin;
	//			//maxDepthImg.at(x, y) = pFrame->m_pMaxDepthImg->at(x, y) - depthMin;
	//			if(m_pMinDspImg->at(x, y) >= m_fMaxDsp)
	//				minDspImg.at(x, y) = 0;
	//			else
	//				minDspImg.at(x, y) = m_pMinDspImg->at(x, y) - m_fMinDsp;

	//			if(m_pMaxDspImg->at(x, y) <= m_fMinDsp)
	//				maxDspImg.at(x, y) = 0;
	//			else
	//				maxDspImg.at(x, y) = m_pMaxDspImg->at(x, y) - m_fMinDsp;
	//		}

	//	}
	//	CxImage image;
	//	ZImageToCxImage(minDspImg, image, 255.0/( m_fMaxDsp - m_fMinDsp) );
	//	//ZImageToCxImage(minDepthImg, image, 255.0F /( depthMax - depthMin) );
	//	std::tstring str0 = s_TempDir[m_iCameraId] + _T("minDsp_") + m_sName + _T(".png");
	//	image.Save(str0.c_str(), CxImageFuns::GetImageFileType(str0));

	//	ZImageToCxImage(maxDspImg, image, 255.0/( m_fMaxDsp - m_fMinDsp ) );
	//	//ZImageToCxImage(maxDepthImg, image, 255.0F /( depthMax - depthMin) );
	//	str0 = s_TempDir[m_iCameraId] + _T("maxDsp_") + m_sName + _T(".png");
	//	image.Save(str0.c_str(), CxImageFuns::GetImageFileType(str0));
	//}
}
*/

void LSVideoFrame::InitMinDspImg( float value ){
	if(m_pMinDspImg == NULL)
		m_pMinDspImg = new ZFloatImage;
	m_pMinDspImg->CreateAndInit(m_iWidth, m_iHeight, 1, value);
}

void LSVideoFrame::InitMaxDspImg( float value ){
	if(m_pMaxDspImg == NULL)
		m_pMaxDspImg = new ZFloatImage;
	m_pMaxDspImg->CreateAndInit(m_iWidth, m_iHeight, 1, value);
}

//new add
void LSVideoFrame::InitMinDepthImg( float value ){
	if(m_pDepthMin == NULL)
		m_pDepthMin = new ZFloatImage;
	m_pDepthMin->CreateAndInit(m_iWidth, m_iHeight, 1, value);
}
//new add
void LSVideoFrame::InitMaxDepthImg( float value ){
	if(m_pDepthMax == NULL)
		m_pDepthMax = new ZFloatImage;
	m_pDepthMax->CreateAndInit(m_iWidth, m_iHeight, 1, value);
}



// NEW ADD- Change for VisualHull Constraint
//此函数可能有问题，因为对于非Mask区域，minLabel值往往大于maxLabel值（应该是在某一步初始化时出错的）
void LSVideoFrame::InitLabelRangeByDspRange( int layerCount, float  VisualHullConstrianSigma, bool SaveImg){
	if(m_pMinLabelImg == NULL)
		m_pMinLabelImg = new ZIntImage;
	if(m_pMaxLabelImg == NULL)
		m_pMaxLabelImg = new ZIntImage;
	if(m_pMaskImg == NULL)
		LoadMaskImg();

	m_pMinLabelImg->CreateAndInit( m_iWidth, m_iHeight, 1, -1);
	m_pMaxLabelImg->CreateAndInit( m_iWidth, m_iHeight, 1, -1);
	const float dspSeg = ( m_fMaxDsp - m_fMinDsp ) / layerCount;  //dsp interval

    //std::cout<<"m_fMaxDsp: "<<m_fMaxDsp<<"m_fMinDsp:"<<m_fMinDsp<<std::endl;

	for(int y = m_iMaskMinY; y <= m_iMaskMaxY; ++y)
	{
		for(int x = m_iMaskMinX; x <= m_iMaskMaxX; ++x)
		{

			//may have bug!  m_pMinDspImg中的值要大于m_pMaxDspImg中的值
			float fminDsp = 1.0/m_pDepthMax->GetPixel(x,y,0);
			float fmaxDsp = 1.0/m_pDepthMin->GetPixel(x,y,0);
		  //fminDsp = fminDsp + (fmaxDsp - fminDsp) * VisualHullConstrianSigma;   //将minDspImage向上扩展
			  
			int minLabel = ( fminDsp- m_fMinDsp ) /dspSeg - 1 ;
			minLabel = max(0, minLabel);
			minLabel = min(minLabel, layerCount);

			int maxLabel = ( fmaxDsp- m_fMinDsp ) / dspSeg + 1;
			maxLabel = min(maxLabel, layerCount);
			maxLabel = max(0, maxLabel);	

			//if(IsInMask(x,y)==true)
			//{
			//	if(m_pMaxDspImg->GetPixel(x, y, 0) < m_pMinDspImg->GetPixel(x, y, 0))
			//	{
			//		minLabel = 100;		
			//	    maxLabel = 99;
			//		std::cout<<"x: "<<x<<" ,y: "<<y<<"  maxLabel: "<<maxLabel<<" ,minLabel: "<<minLabel<<std::endl;
			//	}
			//}

			  //直接判断两种情况不太确定是否正确。
			  if( IsInMask(x, y) == false || m_pDepthMax->GetPixel(x, y, 0) < m_pDepthMin->GetPixel(x, y, 0)){
					minLabel = 1;		
					maxLabel = 0;
				}

			if(minLabel <= maxLabel && maxLabel - minLabel<5)
			{
				maxLabel = layerCount;
				minLabel = 0;
			}
			m_pMinLabelImg->at( x,y ) = minLabel ;
			m_pMaxLabelImg->at( x,y ) = maxLabel ;

			//		  std::cout<<"x:"<<x<<",y:"<<y<<",minLabel:"<<minLabel<<",maxLabel:"<<maxLabel<<std::endl;
		}
	}
	if(SaveImg)
	{
		SaveLabelImg(*m_pMinLabelImg, MINDSP, layerCount);
		SaveLabelImg(*m_pMaxLabelImg, MAXDSP, layerCount);
	}

}


/*
// ORG  2014/12/09
//此函数可能有问题，因为对于非Mask区域，minLabel值往往大于maxLabel值（应该是在某一步初始化时出错的）
void LSVideoFrame::InitLabelRangeByDspRange( int layerCount, float  VisualHullConstrianSigma, bool SaveImg){
	if(m_pMinLabelImg == NULL)
		m_pMinLabelImg = new ZIntImage;
	if(m_pMaxLabelImg == NULL)
		m_pMaxLabelImg = new ZIntImage;
	if(m_pMaskImg == NULL)
		LoadMaskImg();

	m_pMinLabelImg->CreateAndInit( m_iWidth, m_iHeight, 1, -1);
	m_pMaxLabelImg->CreateAndInit( m_iWidth, m_iHeight, 1, -1);
	const float dspSeg = ( m_fMaxDsp - m_fMinDsp ) / layerCount;  //dsp interval

	std::cout<<"m_fMaxDsp: "<<m_fMaxDsp<<"m_fMinDsp:"<<m_fMinDsp<<std::endl;

	for(int y = m_iMaskMinY; y <= m_iMaskMaxY; ++y)
	{
		for(int x = m_iMaskMinX; x <= m_iMaskMaxX; ++x)
		{
		 //may have bug!  m_pMinDspImg中的值要大于m_pMaxDspImg中的值
			 float fminDsp = m_pMinDspImg->GetPixel(x, y, 0);
			 float fmaxDsp = m_pMaxDspImg->GetPixel(x, y, 0);
		
			 // 胖瘦的核心问题应该出在这里！！！！

		    fminDsp = fminDsp + (fmaxDsp - fminDsp) * VisualHullConstrianSigma;   //将minDspImage向上扩展

			int minLabel = (fminDsp - m_fMinDsp ) /dspSeg - 1 ;
			minLabel = max(0, minLabel);
			minLabel = min(minLabel, layerCount);

			fmaxDsp=fmaxDsp+(fmaxDsp-fminDsp)*0.15;       //new add
			int maxLabel = (fmaxDsp - m_fMinDsp ) / dspSeg + 1;
			maxLabel = min(maxLabel, layerCount);
			maxLabel = max(0, maxLabel);	

			if( IsInMask(x, y) == false || m_pMaxDspImg->GetPixel(x, y, 0) < m_pMinDspImg->GetPixel(x, y, 0)){
				minLabel = layerCount+1;		
				maxLabel = layerCount;
			}

			if(minLabel <= maxLabel && maxLabel - minLabel<5)
			{	
				maxLabel = min(maxLabel+1, layerCount);
				minLabel = max(minLabel-1, 0);
				//maxLabel =0;
				//minLabel =0;
			}
			m_pMinLabelImg->at( x,y ) = minLabel ;
			m_pMaxLabelImg->at( x,y ) = maxLabel ;

	//	std::cout<<"minLabel: "<<minLabel<<" ,maxLabel: "<<maxLabel<<std::endl;
		}
	}

	if(SaveImg)
	{
		SaveLabelImg(*m_pMinLabelImg, MINDSP, layerCount);
		SaveLabelImg(*m_pMaxLabelImg, MAXDSP, layerCount);
	}

}
*/



void LSVideoFrame::SaveLabelImg(ZIntImage& labelImg, LABEL_IMG_TYPE type, int labelCount, bool showMask)
{
	//LSParaWidget* funs = LSParaWidget::GetInstance();   //not used

	std::string goalPath;
	std::string str =  m_sName + std::string(".png"); 
   
	char path[100];
	FILE * fp;
	sprintf(path, "%sDynSeg%d.png", s_TempDir[m_iCameraId].c_str(), m_iFrameId);

	// switch which type label image to save
	switch (type){
	case INIT: goalPath = s_TempDir[m_iCameraId] + "1_Init_" + str; break;
	case BO: goalPath = s_TempDir[m_iCameraId] + "2_BO_" + str; break;
	case DE:	goalPath = s_TempDir[m_iCameraId] + "3_DE_" + str; break;
	case DATACOST_Init: goalPath = s_TempDir[m_iCameraId] + "1_DA_Init" + str; break;
	case DATACOST_BO:goalPath = s_TempDir[m_iCameraId] + "2_DA_BO" + str; break;
    case DATACOST_OPT:goalPath = s_TempDir[m_iCameraId] + "3_DA_OPT" + str; break;
	case MINDSP: goalPath = s_TempDir[m_iCameraId] + "MinDsp_" + str; break;
	case MAXDSP: goalPath = s_TempDir[m_iCameraId] + "MaxDsp_" + str; break;
	case BO_DE: goalPath = s_TempDir[m_iCameraId] + "2_BO_DE_" + str; break;
	case INIT_DE: goalPath = s_TempDir[m_iCameraId] + "1_Init_DE_" + str; break;
	case INIT_SEG:goalPath = s_TempDir[m_iCameraId] + "1_Init_Seg_" + str; break;
    case OPT:goalPath = s_TempDir[m_iCameraId] + "4_Opt_Init" + str; break;
	case OPT_DE:goalPath = s_TempDir[m_iCameraId] + "4_Opt_DE_" + str; break;
	default:std::cout<<"ERROR:Can not find save labelImg because the label type is unkown!"<<std::endl;

	}

	CxImage res ;
	ZImageToCxImage(labelImg, res, 255.0F/(labelCount-1));
	if(showMask == true){
		int width = labelImg.GetWidth();
		int height = labelImg.GetHeight();
		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				if(IsInMask(x, y) == true)
					continue;
				RGBQUAD color = res.GetPixelColor(x, height - 1 - y);
				//color.rgbRed = min((int)color.rgbRed + 125, 255);
				//color.rgbGreen = min((int)color.rgbGreen + 125, 255);
				color.rgbBlue = min((int)color.rgbBlue + 125, 255);   //区分Mask区域外面颜色的
				//color.rgbBlue = 255;
				//color.rgbGreen = 0;
				//color.rgbRed = 0;
				res.SetPixelColor(x, height - 1 - y, color);
			}
		}
	}
	res.Save(goalPath.c_str(), CXIMAGE_FORMAT_PNG);
   
}


void LSVideoFrame::SetDspImg( ZIntImage& labelImg, int trueX1, int trueY1, int trueX2, int trueY2, int dspLevelCount){
	/*
	m_pMinDspImg = new ZFloatImage;
	m_pMinDspImg->CreateAndInit(m_iWidth, m_iHeight, 1, value);
	*/

	if(m_pDspImg == NULL)
	//	m_pDspImg=new ZFloatImage;
	//m_pDspImg->CreateAndInit(labelImg.GetWidth(),labelImg.GetHeight(),1,0);

	  m_pDspImg = new ZFloatImage(labelImg.GetWidth(), labelImg.GetHeight(), 1);
	     
	if(m_pDspImg==NULL)
		std::cout<<"m_pDspImg is Null"<<std::endl;

 //   std::cout<<"labelImg.GetWidth(): " <<labelImg.GetWidth()<<" , "<<"labelImg.GetHeight(): "<<labelImg.GetHeight()<<std::endl;

	float layercount = dspLevelCount - 1;

	int dspLeveli;
	float dsp;

	for(int y=trueY1; y<=trueY2; y++){
		for(int x=trueX1; x<=trueX2; x++){
		    // std::cout<<"x: "<<x<<" , "<<" y: "<<y<<std::endl;    //348,140
			dspLeveli = labelImg.GetPixel(x, y, 0);
			if(dspLeveli == -1)
				dsp = -1.0F;
			else
				dsp = m_fMinDsp * (layercount-dspLeveli)/layercount + m_fMaxDsp * dspLeveli/layercount;
			m_pDspImg->SetPixel(x,  y, 0, dsp);        // bug occurred
		  //std::cout<<"dsp: "<<dsp<<std::endl;

		}
	}

	//dsp image 出现问题，只计算了右边一半
	//SaveLabelImg(m_pDspImg,LSVideoFrame::INIT_DE,100);
}


// get segm result save path
std::string LSVideoFrame::GetSegmSavePath(LSVideoFrame* pFrm )
{
	std::string frameName=pFrm->m_sName; 
	std::string dirPath=pFrm->s_SegDir[pFrm->m_iCameraId];
	std::string segSavePath=dirPath+frameName+ std::string(".jpg"); 
	std::cout<<"segSavePath:"<<segSavePath<<std::endl;
	return segSavePath;
}


/*
std::string PointCloudSampling::GetPtCldFileName( LSVideoFrame* pFrm )
{
	std::string frameName=pFrm->m_sName;
	std::string dirPath=pFrm->s_RawDir[pFrm->m_iCameraId];
	//std::cout<<"dirPath: "<<dirPath<<std::endl;
	return dirPath+frameName + "_PtCld" + ".raw";
}
*/



void LSVideoFrame::SaveModel(bool considerMask)
{
	if(m_pDspImg == NULL){
		std::cout<<"ERROR! No ObjModel is saved!!"<<std::endl;
		return;
	}
	char path[MAX_PATH];
	sprintf(path, "%s_model%d.obj", s_DataDir[m_iCameraId].c_str(), m_iFrameId);
	std::cout<< path<<std::endl;

	LSModelIO ouput(m_fMinDsp, m_fMaxDsp, 10);


	ouput.SaveModel(*m_pDspImg, considerMask ? m_sMaskImgPathName : "", path,  *m_pLsCameraParameter);
}

void LSVideoFrame::InitLabelImgByDspImg( ZIntImage& labelImg, int dspLevelCount )
{
	if(m_pDspImg == NULL){
		std::cout<<"ERROR: Can not init labelImg, because DspImg is null!"<<std::endl;
		return;
	}

	int width = m_pDspImg->GetWidth();
	int height = m_pDspImg->GetHeight();
	labelImg.Create(width, height);

	int dspLeveli;
	float dsp;
	LSParaWidget* funs =  LSParaWidget::GetInstance();

	for(int y=0; y<height; y++){
		for(int x=0; x<width; x++){
			dsp = m_pDspImg->GetPixel(x, y, 0);
			if(dsp < 0.0F){
				dspLeveli = -1;
			}
			else
				dspLeveli =  (dspLevelCount - 1) * (dsp - m_fMinDsp) / (m_fMaxDsp - m_fMinDsp);
			/*if(dspLeveli < 0 )
				std::tcout<<_T("dspLeveli = -1");*/
			labelImg.SetPixel(x, y, 0, dspLeveli);
		}
	}
}

float LSVideoFrame::GetDspAt(int x,int y)
{
	return m_pDspImg->GetPixel(x,y,0);
}

void LSVideoFrame::SetDspAt( int x, int y, float dspValue )
{
	m_pDspImg->at(x, y, 0) = dspValue;
}


// return dsp value nearby (xt,yt) according wheather it in mask
float LSVideoFrame::GetDspAt( double x, double y, bool considerMask )
{
	int xt = (int) x;  /* coordinates of top-left corner */
	int yt = (int) y;
	float ax = x - xt; //decimal
	float ay = y - yt;
	
	// 二元线性插值
	if(considerMask == false)
		return ( (1-ax) * (1-ay) * m_pDspImg->at(xt, yt, 0) +
		ax * (1-ay) * m_pDspImg->at(xt+1, yt, 0) +
		(1-ax) * ay   * m_pDspImg->at(xt, yt+1, 0) +
		ax * ay * m_pDspImg->at(xt+1, yt+1, 0) );

// calculate according weight
	float res = 0;
	float weight = 0;
	if(IsInMask(xt, yt)){
		res += (1-ax) * (1-ay) * m_pDspImg->at(xt, yt, 0);
		weight += (1-ax) * (1-ay);
	}
	if(IsInMask(xt+1, yt)){
		res += ax * (1-ay) * m_pDspImg->at(xt+1, yt, 0);
		weight += ax * (1-ay);
	}
	if(IsInMask(xt, yt+1)){
		res += (1-ax) * ay   * m_pDspImg->at(xt, yt+1, 0);
		weight += (1-ax) * ay;
	}
	if(IsInMask(xt, yt+1)){
		res += ax * ay * m_pDspImg->at(xt+1, yt+1, 0);
		weight += ax * ay;
	}
	return res / weight;
}


void LSVideoFrame::SaveDspImg()
{
	if(m_pDspImg == NULL){
		std::cout<<"ERROR! No dspImg is saved!!"<<std::endl;
		return;
	}
	//TCHAR path[100];
	//_stprintf(path, _T("%s_depth%d.raw"), s_DataDir[m_iCameraId].c_str(), m_iFrameId);
	char path[100];
	sprintf(path, "%s_depth%d.raw", s_DataDir[m_iCameraId].c_str(), m_iFrameId);
	FILE * fp;
	fp = fopen(path, "wb");
	if(!fp){
		std::cout<< "ERROR: Can not create depth file: "<< path <<std::endl;
		system("pause");
		return;
	}
	fwrite(m_pDspImg->GetMap(), sizeof(BYTE), m_pDspImg->GetSize(), fp);
	fclose(fp);

	//std::tofstream outStream(path, std::tofstream::binary);
	//if(!outStream.good()){
	//	std::tcout<< _T("ERROR: Can not create depth file: ")<< path <<std::endl;
	//	system("pause");
	//	return;
	//}
	//outStream.write(m_pDspImg->GetMap(), m_pDspImg->GetSize());
}


//load fore optical flow image
 void  LSVideoFrame::LoadForeOptFlwImg()
{
	int startFrameId=LSRunConfigDlg::GetInstance()->GetStartFrame();
    int endFrameId=LSRunConfigDlg::GetInstance()->GetEndFrame();


	if(m_iFrameId<startFrameId||m_iFrameId>=endFrameId)
	{
	    std::cout<<"FrameId is out of range or is the last frame! m_iFrameId:"<<m_iFrameId<<std::endl;
		system("pause");
	}
	else
	{
		if(m_pForeOptImg != NULL)
		{
			delete m_pForeOptImg;
			m_pForeOptImg = NULL;
		}

		TCHAR path1[100],path2[100];

		FILE * fp1,* fp2;

		std::string foreFrameName=LSStructureMotion::GetInstance()->GetFrameAt(m_iCameraId,m_iFrameId)->m_sName;
		std::string backFrameName=LSStructureMotion::GetInstance()->GetFrameAt(m_iCameraId,m_iFrameId+1)->m_sName;

		sprintf(path1, "%sopticalFlow_u_%s_%s.raw",LSVideoFrame::s_OptFlwDir[m_iCameraId].c_str(),foreFrameName.c_str(),backFrameName.c_str());
		sprintf(path2, "%sopticalFlow_v_%s_%s.raw",LSVideoFrame::s_OptFlwDir[m_iCameraId].c_str(),foreFrameName.c_str(),backFrameName.c_str());

		int iWidth=LSVideoFrame::GetImgWidth();
		int iHeight=LSVideoFrame::GetImgHeight();

		std::vector<float> U(iWidth*iHeight);
		std::vector<float> V(iWidth*iHeight);

		fp1 = fopen(path1, "rb");
		fp2 = fopen(path2, "rb");

		m_pForeOptImg = new ZFloatImage(iWidth, iHeight, 2);
		//ZFloatImage OptFlowMap(iWidth, iHeight, 2);

		if(fp1&&fp2)
		{
			fread(&U[0],iWidth*iHeight,sizeof(float),fp1);
			fclose(fp1);

			fread(&V[0],iWidth*iHeight,sizeof(float),fp2);
			fclose(fp2);


			for(int y = 0, p = 0; y < iHeight; y++)
			{
				for(int x = 0; x < iWidth; x++, p++)
				{
					m_pForeOptImg->at(x, iHeight - 1 - y, 0) = U[p];
				    m_pForeOptImg->at(x, iHeight - 1 - y, 1) = V[p];

					// std::cout<<"U[p]:"<<U[p]<<",V[p]:"<<V[p]<<std::endl;
				}
			}

		}
		else
		{
			std::cout << "ERROR:Can not OptFlwImgU:"<< "path1:" <<path1<< std::endl;
			std::cout << "ERROR:Can not OptFlwImgV:"<< "path2:" <<path2<< std::endl;
			system("pause");
		}

    }
}


 //load back optical flow image( t, t-1)
 void  LSVideoFrame::LoadBackOptFlwImg()
 {
	 int startFrameId=LSRunConfigDlg::GetInstance()->GetStartFrame();
	 int endFrameId=LSRunConfigDlg::GetInstance()->GetEndFrame();


	 if(m_iFrameId<=startFrameId||m_iFrameId>endFrameId)
	 {
		 std::cout<<"FrameId is out of range or is the first frame! m_iFrameId:"<<m_iFrameId<<std::endl;
		 system("pause");
	 }
	 else
	 {
		 if(m_pBackOptImg != NULL)
		 {
			 delete m_pBackOptImg;
			 m_pBackOptImg = NULL;
		 }

		 TCHAR path1[100],path2[100];

		 FILE * fp1,* fp2;

		 std::string foreFrameName=LSStructureMotion::GetInstance()->GetFrameAt(m_iCameraId,m_iFrameId)->m_sName;
		 std::string backFrameName=LSStructureMotion::GetInstance()->GetFrameAt(m_iCameraId,m_iFrameId-1)->m_sName;

		 sprintf(path1, "%sopticalFlow_u_%s_%s.raw",LSVideoFrame::s_OptFlwDir[m_iCameraId].c_str(),foreFrameName.c_str(),backFrameName.c_str());
		 sprintf(path2, "%sopticalFlow_v_%s_%s.raw",LSVideoFrame::s_OptFlwDir[m_iCameraId].c_str(),foreFrameName.c_str(),backFrameName.c_str());

		 int iWidth=LSVideoFrame::GetImgWidth();
		 int iHeight=LSVideoFrame::GetImgHeight();

		 std::vector<float> U(iWidth*iHeight);
		 std::vector<float> V(iWidth*iHeight);

		 fp1 = fopen(path1, "rb");
		 fp2 = fopen(path2, "rb");

		 m_pBackOptImg = new ZFloatImage(iWidth, iHeight, 2);
		 //ZFloatImage OptFlowMap(iWidth, iHeight, 2);

		 if(fp1&&fp2)
		 {
			 fread(&U[0],iWidth*iHeight,sizeof(float),fp1);
			 fclose(fp1);

			 fread(&V[0],iWidth*iHeight,sizeof(float),fp2);
			 fclose(fp2);
			 for(int y = 0, p = 0; y < iHeight; y++)
			 {
				 for(int x = 0; x < iWidth; x++, p++)
				 {
					 m_pBackOptImg->at(x, iHeight - 1 - y, 0) = U[p];
					 m_pBackOptImg->at(x, iHeight - 1 - y, 1) = V[p];

					 // std::cout<<"U[p]:"<<U[p]<<",V[p]:"<<V[p]<<std::endl;
				 }
			 }
		 }
		 else
		 {
			 std::cout << "ERROR:Can not OptFlwImgU:"<< "path1:" <<path1<< std::endl;
			 std::cout << "ERROR:Can not OptFlwImgV:"<< "path2:" <<path2<< std::endl;
			 system("pause");
		 }
	 }
 }
