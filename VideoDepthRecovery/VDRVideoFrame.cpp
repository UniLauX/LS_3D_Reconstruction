#pragma 
#include "VDRVideoFrame.h"
//#include "Wmlvector2.h"

#define OLD_DSP_NAME
//初始化：静态成员必须在cpp中初始化，否则报错
int VDRVideoFrame::s_iWidth=-1;  
int VDRVideoFrame::s_iHeight=-1;
double VDRVideoFrame::s_dScale=1.0;

std::string VDRVideoFrame::s_tempDir="";
std::string VDRVideoFrame::s_dataDir="";
std::string VDRVideoFrame::s_maskDir="";
//std::string VDRVideoFrame::s_removalDir="";


VDRVideoFrame::VDRVideoFrame(const Wml::Matrix3d &K, const Wml::Matrix3d &Scaled_K)
{
	m_pCameraParameter=new CameraParameters(K,Scaled_K);
	m_pColorImage=NULL;
	m_pDspImage=NULL;
	m_pMaskImage=NULL;


//for lightstage 


	m_iMaskMinX = -1;
	m_iMaskMinY = -1;
	m_iMaskMaxX = -1;
	m_iMaskMaxY = -1;

	LEASTFRONT = 2;

	m_fMinDsp = -1.0;
	m_fMaxDsp = -1.0;

}


//析构函数
VDRVideoFrame::~VDRVideoFrame(void)
{
  delete m_pCameraParameter;
  Clear();
}

//获取彩色图像上(x,y)坐标的三通道颜色值
void VDRVideoFrame::GetColorAt(int x,int y,Wml::Vector3d &out_color)
{
	out_color[0]=m_pColorImage->at(x,y,0);
	out_color[1]=m_pColorImage->at(x,y,1);
	out_color[2]=m_pColorImage->at(x,y,2);
}




//得到（x,y)位置的disparity值(重载）
float VDRVideoFrame::GetDspAt( int x, int y )
{
	return m_pDspImage->GetPixel(x, y, 0);
}


//得到（x,y)位置的disparity值
double VDRVideoFrame::GetDspAt(double x,double y)
{
	//floor 向下取整；  ceil 向上取整
	int ix0 = floor(x), ix1 = ceil(x) >= GetImageWidth()? ix0: ceil(x);      //确保不能越界
	int iy0 = floor(y), iy1 = ceil(y) >= GetImageHeight()? iy0: ceil(y);
	double dx = x-ix0, dy = y-iy0;     //小数部分
	
     //获得（x,y)位置四邻域的disparity值
	double r00 = m_pDspImage->at(ix0, iy0, 0);
	double r01 = m_pDspImage->at(ix0, iy1, 0);
	double r10 = m_pDspImage->at(ix1, iy0 ,0);
	double r11 = m_pDspImage->at(ix1, iy1, 0);

	return (r00 * (1.0-dx) + r10 * dx) * (1.0-dy) + 
		(r01 * (1.0-dx) + r11 * dx) * dy;           //求解思想类似m+1 level，按比例。具体含义不是很清楚

  
}



//设置（x,y)坐标的disparity值
void VDRVideoFrame::SetDspAt( int x, int y, float DspValue )
{
	m_pDspImage->at(x, y, 0) = DspValue;
}



//加载彩色图像（重载）
void VDRVideoFrame::LoadColorImage()
{
	if(m_pColorImage!=NULL)
		return;
	LoadColorImage(m_pColorImage);
}

//加载彩色图像
void VDRVideoFrame::LoadColorImage(ZByteImage * &colorImage)
{
	CxImage cImage;
	cImage.Load(m_sImagePathName.c_str());   //根据图像路径名称加载图像
	
	//判断图像是否需要重采样（放缩）
	if(GetImageWidth()!=cImage.GetWidth())
	{
		cImage.Resample(GetImageWidth(),GetImageHeight());
	}
	//将刚创建的cImage复制给colorImage
	colorImage=new ZByteImage(cImage.GetWidth(),cImage.GetHeight(),3);
	CxImageToZImage(cImage,*colorImage);

}

// 创建dispatity图像
void VDRVideoFrame::CreateDspImage()
{
	//初值非空时返回
	if(m_pDspImage!=NULL)
		return;
    m_pDspImage=new ZFloatImage(GetImageWidth(),GetImageHeight(),1);
}


//获得分割（图像）存储路径
std::string VDRVideoFrame::GetSegmSavePath()
{
	std::string str=m_sName+std::string(".jpg");
	
	std::cout<<str<<std::endl;
	return s_tempDir+std::string("Segm_")+str;

}


//存储初始化DataCost之后的labelImage
void VDRVideoFrame::SaveLabelImage(ZIntImage &labelImage,int labelCount,std::string runType)
{
	// test
	{
		CxImage res;
		std::string goalPath;
		std::string str=m_sName+std::string(".jpg");
		//goalPath=s_tempDir+std::string("1_Init_")+str;
		goalPath=s_tempDir+runType+str;
		ZImageToCxImage(labelImage,res,255.0F/(labelCount-1));
		res.Save(goalPath.c_str(),CXIMAGE_FORMAT_JPG);
	}

}

//根据块上的label设置图像的disparity值
void VDRVideoFrame::SetDspImg(ZIntImage& labelImg, const Block &block, double dspMin, double dspMax, int dspLevelCount)
{
	//块上的实际坐标（范围）
	int trueX1 = block.m_X;
	int trueY1 = block.m_Y;
	int trueX2 = block.m_X + block.m_iWidth - 1;
	int trueY2 = block.m_Y + block.m_iHeight - 1;

	if(m_pDspImage == NULL)
		 m_pDspImage= new ZFloatImage(labelImg.GetWidth(), labelImg.GetHeight(), 1);

	int layercount = dspLevelCount - 1;

	int dspLeveli;
	float dsp;
	for(int y=trueY1; y<=trueY2; y++){
		for(int x=trueX1; x<=trueX2; x++){
			dspLeveli = labelImg.GetPixel(x, y, 0);
			if(dspLeveli == -1)
				dsp = -1.0F;
			else
				dsp = dspMin * (layercount-dspLeveli)/layercount + dspMax * dspLeveli/layercount;   //参看论文[3]
			
		//  std::cout<<"x="<<x<<", y= "<<y<<", dsp="<<dsp<<std::endl;

		  if(m_pDspImage==NULL)
			//  std::cout<<"m_pDspImage is Null!"<<std::endl;
			m_pDspImage->SetPixel(x,  y, 0, dsp);
		}
	}
}

void VDRVideoFrame::Clear()
{
	if(	m_pDspImage != NULL){
		delete m_pDspImage;
		m_pDspImage = NULL;
	}
	if(m_pDspImage!= NULL){
		delete m_pColorImage;
		m_pColorImage = NULL;
	}
}

//根据scale保存disparity
void VDRVideoFrame::SaveDspImg()
{
	if(m_pDspImage == NULL){
		printf("ERROR! No dspImg is saved!!\n");
		return;
	}
	std::string path;
	
	if(s_dScale < 1.0)
		path = GetDspImgPath_samll();
	else
		path = GetDspImgPath_normal();

	SaveDspImg(m_pDspImage, path);
}

//保存disparity
void VDRVideoFrame::SaveDspImg(ZFloatImage* p, const std::string &path){
	FILE * fp = fopen(path.c_str(), "wb");
	if(!fp){
		printf("ERROR: Can not create depth file: %s", path);
		system("pause");
		return;
	}
	fwrite(p->GetMap(), sizeof(BYTE), p->GetSize(), fp);
	fclose(fp);
}

//存储disparity图像路径（缩放后）
std::string VDRVideoFrame::GetDspImgPath_samll()
{
#ifdef OLD_DSP_NAME
	char path[256];
	sprintf(path, "%s_depth_small%d.raw",s_dataDir.c_str(), m_iID);
	return std::string(path);
#else
	std::string str =  m_sName + std::string(".raw"); 
	return s_dataDir + std::string("_depth_small_") + str;
#endif
}

//存储disparity图像路径（正常尺寸）
std::string VDRVideoFrame::GetDspImgPath_normal()
{
#ifdef OLD_DSP_NAME
	char path[256];
	sprintf(path, "%s_depth%d.raw",s_dataDir.c_str(), m_iID);
	return std::string(path);
#else
	std::string str =  m_sName + std::string(".raw"); 
	return s_dataDir + std::string("_depth_") + str;
#endif
}


//加载深度图像
bool VDRVideoFrame::LoadDepthImg(){
	if(m_pDspImage!= nullptr)
		return true;
	return LoadDepthImg(m_pDspImage);
}
//(重载）加载深度图像
bool VDRVideoFrame::LoadDepthImg(ZFloatImage *&dspImg)
{
	if(s_dScale < 1.0)
		return LoadDepthImg_small(dspImg);
	else
		return LoadDepthImg_normal(dspImg);
}


//从存储路径（创建）加载（缩放后）的深度图像
bool VDRVideoFrame::LoadDepthImg_small(ZFloatImage* &p)
{
	std::string path = GetDspImgPath_samll();
	FILE * fp = fopen(path.c_str(), "rb");

	if(!fp){
		//printf("ERROR: Can not open depth file: %s", path);
		return false;
	}
	if(p != nullptr)
		p->Create(GetImgWidthSmall(), GetImgHeightSmall(), 1);
	else
		p = new ZFloatImage(GetImgWidthSmall(), GetImgHeightSmall(), 1);

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	if(size != p->GetSize()){
		fclose(fp);
		delete p;
		p = nullptr;
		return false;
	}
	fseek(fp, 0, SEEK_SET);
	fread(p->GetMap(), sizeof(BYTE), p->GetSize(), fp);
	fclose(fp);
	return true;
}

//从存储路径（创建）加载（正常尺寸）的深度图像
bool VDRVideoFrame::LoadDepthImg_normal(ZFloatImage* &p)
{
	std::string path = GetDspImgPath_normal();

	std::cout<<"DspImgPath: "<<path<<std::endl;

	FILE * fp = fopen(path.c_str(), "rb");
	if(!fp){
		//printf("ERROR: Can not open depth file: %s", path);
		return false;
	}

	if(p != nullptr)
		p->Create(s_iWidth, s_iHeight, 1);
	else
		p = new ZFloatImage(s_iWidth, s_iHeight, 1);

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	if(size != p->GetSize()){
		fclose(fp);
		delete p;
		p = nullptr;
		return false;
	}
	fseek(fp, 0, SEEK_SET);
	fread(p->GetMap(), sizeof(BYTE), p->GetSize(), fp);
	fclose(fp);
	return true;
}

//应用BO步骤求得的disparity图像初始化DE步骤的Label图像
void VDRVideoFrame::InitLabelImgByDspImg(ZIntImage& labelImg, int LevelCount, double dDspMin, double dDspMax)
{
	//判断传进来的disparity图像是否为空
	if(m_pDspImage == NULL)
	{
		std::cout<<"ERROR: Can not init labelImg, because DspImg is null!"<<std::endl;
		return;
	}
	
	//创建存储空间
	int width = m_pDspImage->GetWidth();
	int height = m_pDspImage->GetHeight();
	labelImg.Create(width, height);

	int dspLeveli;
	float dsp;
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			dsp = m_pDspImage->GetPixel(x, y, 0);   //得到（x,y）位置的disparity值
			dspLeveli = (LevelCount - 1) * (dsp - dDspMin) / (dDspMax - dDspMin) + 0.5; //得到dsp所处的disparity层数，论文中的k(其中0<=k<=m)

			//std::cout<<"dsp: "<<dsp<<std::endl;
			//std::cout<<"dDspMin: "<<dDspMin<<std::endl;
			//std::cout<<"dDspMax: "<<dDspMax<<std::endl;

		    //std::cout<<"dspLeveli: "<<dspLeveli<<std::endl;
		    //std::cout<<"LevelCount: "<<LevelCount<<std::endl;
		/*
			if(dspLeveli < 0 || dspLeveli >= LevelCount)   
			    std::cout<<"Error:dspLevel out of range!"<<std::endl;
        */
			labelImg.SetPixel(x, y, 0, dspLeveli);
		}
	}
}





/************************************Lightstage*************************************/
//VisualHull

void VDRVideoFrame::SetFaceVis( Wml::Vector3d* faceNormal )
{
	const double EPSIRON = 1.0e-6;
  /* for(int i = 0;i<6;i++)
	{
		if( viewDir.Dot(faceNormal[i])<0.0f )
			mFaceVis[i] = true;
		else mFaceVis[i] = false;
	}*/

	/*Wml::Matrix3f  R = CameraMat->RotateMat;
	Wml::Vector3f  T = CameraMat->TransVec;
	T = -T * R;
	for(int i = 0;i<6;i++)
	{
		Wml::Vector3f  R_N = R * faceNormal[i];
		float w =  T.Dot( faceNormal[i] )   + 1;
		float t = R_N[2] / w;
		if(t<EPSIRON)
			mFaceVis[i] = true;
		else mFaceVis[i] = false;
	}*/

	Wml::Matrix4d ExtrinsicMat;
	ExtrinsicMat(3,0) = 0.0;
	ExtrinsicMat(3,1) = 0.0;
	ExtrinsicMat(3,2) = 0.0;
	ExtrinsicMat(3,3) = 1.0;
	for(int j = 0; j<3; j++){
		for(int i = 0; i<3; i++){
			ExtrinsicMat(j, i) = m_pCameraParameter->m_R(j,i);
			                      
		}
		ExtrinsicMat(j, 3) = m_pCameraParameter->m_T[j];
	}
	
	//liql2007 可能有问题
	Wml::Matrix4d  InvExtrinsicMat_T = ( ExtrinsicMat.Inverse() ).Transpose();
	for(int i = 0;i<6;i++){
		Wml::Vector4d  faceNormal_1 = Wml::Vector4d(faceNormal[i][0],faceNormal[i][1],faceNormal[i][2],1.0f);
		faceNormal_1  = InvExtrinsicMat_T * faceNormal_1;
		double t = faceNormal_1[2];// / faceNormal_1[3];
		if(t<EPSIRON)
			m_FaceVis[i] = true;
		else m_FaceVis[i] = false;
	}
}




bool VDRVideoFrame::LoadMaskImg(int corrode)
{
	if(m_pMaskImage != NULL){
		return true;
	}

	CxImage image;
   
	//std::cout<<"m_sMaskImgPathName1: "<<m_sMaskImgPathName<<std::endl;

	if(image.Load(m_sMaskImgPathName.c_str()) == false){
		std::cout << "Can not find mask file: " << m_sMaskImgPathName << std::endl;
		return false;
	}

	m_pMaskImage= new ZByteImage(image.GetWidth(), image.GetHeight());
	CxImageToZImage(image, *m_pMaskImage);
	if(corrode == 0)
		return true;

	int width = image.GetWidth();
	int height = image.GetHeight();

	const int dir[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
	for(int ci = 0; ci<corrode; ci++){		
		std::vector<int> points;
		for(int y = 0; y< height; y++){
			for(int x = 0; x < width; x++){
				if(m_pMaskImage->GetPixel(x, y, 0) <= LEASTFRONT)
					continue;
				bool ok = true;
				for(int i= 0; i < 8 && ok; i++){
					int xi = x + dir[i][0];
					int yi = y + dir[i][1];
					if(xi < 0 || xi >= width || yi < 0 || yi >= height)
						continue;
					if(m_pMaskImage->GetPixel(xi, yi, 0) <= LEASTFRONT)
						ok = false;
				}
				if(ok == false){
					points.push_back(x);
					points.push_back(y);
				}
			}
		}
		for(int i = 0; i<points.size(); i+= 2){
			m_pMaskImage->at(points[i], points[i+1]) = 0;
		}
	}
	return true;

}