#include "MeanShiftSeg.h"


MeanShiftSeg::MeanShiftSeg(void):iProc(nullptr){
	m_iWidth = 0;
	m_iHeight = 0;
}

MeanShiftSeg::~MeanShiftSeg(void){
}

void MeanShiftSeg::Segment(CxImage &cxImage, int sigmaS, float sigmaR, int minRegion, const std::string &savePath)
{
	std::cout << "Meanshift segmentation..." << std::endl;
	m_iWidth = cxImage.GetWidth();
	m_iHeight = cxImage.GetHeight();

	BgImage bgimg;
	bgimg.Resize(m_iWidth, m_iHeight, cxImage.IsGrayScale()==false);
	CxToBgImage(cxImage, bgimg);

	iProc.reset(new msImageProcessor());
	SpeedUpLevel speedUpLeel = MED_SPEEDUP;
	iProc->SetSpeedThreshold(1.0);
	if(cxImage.IsGrayScale()==false)
		iProc->DefineImage(bgimg.im_, COLOR, m_iHeight, m_iWidth);
	else
		iProc->DefineImage(bgimg.im_, GRAYSCALE, m_iHeight, m_iWidth);
	//iProc->Filter(sigmaS, sigmaR, speedUpLeel);
	iProc->Segment(sigmaS, sigmaR, minRegion, speedUpLeel);
	//iProc->FuseRegions(sigmaR, minRegion);

	GetSegmMap(cxImage, savePath);
}


void MeanShiftSeg::Segment( ZByteImage &zImage, int sigmaS, float sigmaR, int minRegion, const std::string &savePath )
{
	std::cout << "Meanshift segmentation..." << std::endl;

	 m_iWidth = zImage.GetWidth();
	 m_iHeight = zImage.GetHeight();

	BgImage bgimg;
	bgimg.Resize(m_iWidth, m_iHeight, zImage.GetChannel() > 1);
	CxToBgImage(zImage, bgimg);

	iProc.reset(new msImageProcessor());
	SpeedUpLevel speedUpLeel = MED_SPEEDUP;
	iProc->SetSpeedThreshold(1.0);
	if(zImage.GetChannel() > 1)
		iProc->DefineImage(bgimg.im_, COLOR, m_iHeight, m_iWidth);
	else
		iProc->DefineImage(bgimg.im_, GRAYSCALE, m_iHeight, m_iWidth);
	//iProc->Filter(sigmaS, sigmaR, speedUpLeel);
	iProc->Segment(sigmaS, sigmaR, minRegion, speedUpLeel);
	//iProc->FuseRegions(sigmaR, minRegion);

	GetSegmMap(zImage, savePath);
}

void MeanShiftSeg::GetSegmMap(CxImage &cxImage, const std::string &savePath){
	if (iProc == false){
		std::cerr << "ERROR: can not get Segm Map!" << std::endl;
		return;
	}

	std::cout<<"total region count: "<<iProc->regionCount<<std::endl;

	std::vector<int> red(iProc->regionCount, 0);
	std::vector<int> green(iProc->regionCount, 0);
	std::vector<int> blue(iProc->regionCount, 0);
	std::vector<int> pixNumOfRegion(iProc->regionCount, 0);

	m_SegMap.Create(m_iWidth, m_iHeight, 1);

	for (int i=0; i<m_iWidth; ++i){
		for (int j=0; j<m_iHeight; ++j){

			int index = j*m_iWidth + i; 
			int label = iProc->labels[index];
			m_SegMap.SetPixel(i, j, 0, label);

			RGBQUAD rgb = cxImage.GetPixelColor(i, m_iHeight-1-j);
			red[label] += (int)rgb.rgbRed;
			green[label] += (int)rgb.rgbGreen;
			blue[label] += (int)rgb.rgbBlue;
			pixNumOfRegion[label] += 1;
		}
	}	

	for (int i=0; i<red.size(); ++i){
		red[i] /= pixNumOfRegion[i];
		green[i] /= pixNumOfRegion[i];
		blue[i] /= pixNumOfRegion[i];
	}

	CxImage saveImg;
	saveImg.Create(m_iWidth, m_iHeight, 24);
	//temp.AlphaCreate();
	for (int j=0; j<m_iHeight; ++j){
		for (int i=0; i<m_iWidth; ++i){

			RGBQUAD rgb;
			rgb.rgbRed = red[m_SegMap.at(i,j)];
			rgb.rgbGreen = green[m_SegMap.at(i,j)];
			rgb.rgbBlue = blue[m_SegMap.at(i,j)];

			//rgb.rgbReserved = index & 0xFF;
			//rgb.rgbRed = (index>>3) & 0xFF;
			//rgb.rgbGreen = (index>>6) & 0xFF;
			//rgb.rgbBlue = (index>>9) & 0xFF;
			saveImg.SetPixelColor(i, m_iHeight-1-j, rgb, true);
		}
	}
	saveImg.Save(savePath.c_str(), CxImageFunction::GetImageFileType(savePath));
}

void MeanShiftSeg::GetSegmMap( ZByteImage &zImage, const std::string &savePath )
{
	if (iProc == false){
		std::cerr << "ERROR: can not get Segm Map!" << std::endl;
		return;
	}

	std::cout<<"Total region count: "<<iProc->regionCount<<std::endl;

	std::vector<int> red(iProc->regionCount, 0);
	std::vector<int> green(iProc->regionCount, 0);
	std::vector<int> blue(iProc->regionCount, 0);
	std::vector<int> pixNumOfRegion(iProc->regionCount, 0);

	m_SegMap.Create(m_iWidth, m_iHeight, 1);

	for (int i=0; i<m_iWidth; ++i){
		for (int j=0; j<m_iHeight; ++j){

			int index = j*m_iWidth + i; 
			int label = iProc->labels[index];
			m_SegMap.SetPixel(i, j, 0, label);

			red[label] += zImage.GetPixel(i, j, 0);
			green[label] += zImage.GetPixel(i, j, 1);
			blue[label] += zImage.GetPixel(i, j, 2);
			pixNumOfRegion[label] += 1;
		}
	}	

	for (int i=0; i<red.size(); ++i){
		red[i] /= pixNumOfRegion[i];
		green[i] /= pixNumOfRegion[i];
		blue[i] /= pixNumOfRegion[i];
	}

	CxImage saveImg;
	saveImg.Create(m_iWidth, m_iHeight, 24);
	//temp.AlphaCreate();
	for (int j=0; j<m_iHeight; ++j){
		for (int i=0; i<m_iWidth; ++i){

			RGBQUAD rgb;
			rgb.rgbRed = red[m_SegMap.at(i,j)];
			rgb.rgbGreen = green[m_SegMap.at(i,j)];
			rgb.rgbBlue = blue[m_SegMap.at(i,j)];

			//rgb.rgbReserved = index & 0xFF;
			//rgb.rgbRed = (index>>3) & 0xFF;
			//rgb.rgbGreen = (index>>6) & 0xFF;
			//rgb.rgbBlue = (index>>9) & 0xFF;
			saveImg.SetPixelColor(i, m_iHeight-1-j, rgb, true);
		}
	}
	saveImg.Save(savePath.c_str(), CxImageFunction::GetImageFileType(savePath));
}

void MeanShiftSeg::CxToBgImage(CxImage &cxImg, BgImage &bgImg)
{
	int i,j;
	int iEffWidth = cxImg.GetEffWidth();
	unsigned char* pBgAddress, *pCxAddress;

	pBgAddress = bgImg.im_;
	pCxAddress = cxImg.GetBits();

	if(cxImg.GetBpp() == 24){
		for(j=0; j<m_iHeight; j++){	
			unsigned char* iSrc = pCxAddress + (m_iHeight-1- j)*iEffWidth;
			unsigned char* iDst  = pBgAddress + j*m_iWidth*3;

			for(i=0; i<m_iWidth; i++){
				*iDst++ = *(iSrc+2);			 
				*iDst++ = *(iSrc+1);
				*iDst++ = *iSrc;
				iSrc += 3;
			}
		}
	}
	else{
		for(j=0;j<m_iHeight;j++){	
			unsigned char* iDst  = pBgAddress + j*m_iWidth;

			for(i=0;i<m_iWidth;i++){
				RGBQUAD color = cxImg.GetPixelColor(i, m_iHeight - 1- j);
				*iDst++ = color.rgbRed;
			}
		}
	}
}

void MeanShiftSeg::CxToBgImage( ZByteImage &zImg, BgImage &bgImg )
{
	int i,j;
	int iEffWidth = zImg.GetEffectWidth();
	unsigned char* pBgAddress, *pCxAddress;

	pBgAddress = bgImg.im_;
	pCxAddress = reinterpret_cast<unsigned char*>(zImg.GetMap());

	if(zImg.GetChannel() == 3){
		for(j=0; j<m_iHeight; j++){	
			unsigned char* iSrc = pCxAddress + j * iEffWidth;
			unsigned char* iDst  = pBgAddress + j*m_iWidth*3;

			for(i=0; i<m_iWidth; i++){
				*iDst++ = *(iSrc+2);			 
				*iDst++ = *(iSrc+1);
				*iDst++ = *iSrc;
				iSrc += 3;
			}
		}
	}
	else{
		for(j=0;j<m_iHeight;j++){	
			unsigned char* iDst  = pBgAddress + j*m_iWidth;

			for(i=0;i<m_iWidth;i++){
				*iDst++ = zImg.GetPixel(i, j);
			}
		}
	}
}

void MeanShiftSeg::GetRegions(std::vector< std::vector<blk::Point> >& regions) const
{
	if (iProc == false){
		std::cerr << "ERROR: Please segment image first!" << std::endl;
		return;
	}
	regions.clear();
	regions.resize(iProc->regionCount);

	for(int j=0; j<m_iHeight; ++j){
		for(int i=0; i<m_iWidth; ++i){
			int iSeg = m_SegMap.at(i,j);
			regions[iSeg].push_back(blk::Point(i, j));
		}		
	}

	//adjacent.clear();
	//adjacent.resize(iProc->regionCount);
	//iProc->BuildRAM();
	//for (int i=0; i<iProc->regionCount; ++i)
	//{
	//	RAList* list = iProc->raList[i].next;
	//	do 
	//	{
	//		adjacent[i].push_back(list->label);
	//		list = list->next;
	//	} while (list);
	//}
}


//find different segm index and map regions( x,y coord)
void MeanShiftSeg::GetRegions(std::vector< std::vector<blk::Point> > &regions, const Block &block ) const
{
	if (iProc == false)
	{
		std::cerr << "ERROR: Please segment image first!" << std::endl;
		return;
	}
	std::map<int, int> dir;
	for(int j=block.m_Y;  j < block.m_Y + block.m_iHeight;  ++j)
	{
		for(int i=block.m_X;  i < block.m_X + block.m_iWidth; ++i)
		{
			int iSeg = m_SegMap.at(i,j);          //seg index

			std::map<int, int>::iterator it = dir.find(iSeg);
			if(it != dir.end())
				regions[it->second].push_back(blk::Point(i, j));
			else
			{
				dir.insert(std::make_pair(iSeg, regions.size()));
				std::vector<blk::Point> v(1, blk::Point(i, j));
				regions.push_back(v);
			}
		}		
	}

	//adjacent.clear();
	//adjacent.resize(iProc->regionCount);
	//iProc->BuildRAM();
	//for (int i=0; i<iProc->regionCount; ++i)
	//{
	//	RAList* list = iProc->raList[i].next;
	//	do 
	//	{
	//		adjacent[i].push_back(list->label);
	//		list = list->next;
	//	} while (list);
	//}

	//int *LocMark = new int[GetImgWidth() * GetImgHeight()];
	//memset(LocMark, 0, GetImgWidth() * GetImgHeight() * sizeof(int));
	//segList.resize( 0 );

	//for(int j=offsetY; j<offsetY + height; ++j){
	//	for(int i=offsetX; i<offsetX + width; ++i){
	//		int iSeg = m_pSegMap->at(i,j);
	//		if(LocMark[iSeg] == 0){
	//			segList.push_back(new std::vector< Wml::Vector2<int> >());
	//			LocMark[iSeg] = segList.size();
	//		}
	//		Wml::Vector2<int> p(i,j);
	//		segList[LocMark[iSeg]-1]->push_back(p);
	//	}		
	//}
	//delete [] LocMark;
}





/*
//image width and height
int ImgWidth = LabelImg.GetWidth();
int ImgHeight = LabelImg.GetHeight();

int blockWidth, blockHeight;
std::vector<int> EdgePoints;
int trueX1, trueY1, trueX2, trueY2;

//get block edge info
m_ImgPartion.GetBlockInfoFull(m_iOffsetX, m_iOffsetY, blockWidth, blockHeight, EdgePoints, trueX1, trueY1, trueX2, trueY2);
*/








int MeanShiftSeg::GetSegmMapAt( int x, int y ) const
{
	if (iProc == false){
		std::cerr << "ERROR: Please segment image first!" << std::endl;
		system("pause");
		return 0;
	}
	return m_SegMap.at(x, y);
}
