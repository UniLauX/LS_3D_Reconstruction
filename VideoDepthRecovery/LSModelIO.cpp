#include "LSModelIO.h"


LSModelIO::LSModelIO(float fMinDis, float fMaxDis, float fSmoothThreshold){
	m_fMaxDsp = fMaxDis;
	m_fMinDsp = fMinDis;
	m_fSmoothThreshold = fSmoothThreshold;
}


LSModelIO::~LSModelIO(void)
{
}

void LSModelIO::SaveModel( ZFloatImage& depth, std::string& maskImgPath, std::string desPath, LSCameraParameters& camerPara)
{
	int width = depth.GetWidth();
	int height = depth.GetHeight();
	    
	//std::string strPath;
	//std::tstring2string(desPath, strPath);

	FILE * file;
	file = fopen(desPath.c_str(), "w");
	if(file == NULL){
		//printf("ERROR: Can not open the object file:%s\n", desPath);
		std::cout<<"Can not open the object ffile:"<<desPath<<std::endl;
		return;
	}

	// load mask image
	CxImage maskImg;
	if(maskImgPath.size() != 0)
		maskImg.Load(maskImgPath.c_str(), 0);

	ZIntImage tabImg;
	tabImg.CreateAndInit(width,height, 1, 0);
	int tabNum = 0;
	
//get vetex coord and write in file
	Wml::Vector3d w_pt3d;
	for(int y=0; y<height; y++)
		for(int x=0; x<width; x++){
			if(InMask(maskImg, x, height - y - 1)){

				float dsp = depth.at(x, y);
				if(dsp < m_fMinDsp || dsp > m_fMaxDsp)
					continue;

				
				tabImg.at(x, y) = ++tabNum;
				camerPara.GetCameraCoordFrmImgCoord(x, y, dsp, w_pt3d);
				fprintf(file, "v %lf %lf %lf\n", w_pt3d[0], w_pt3d[1], w_pt3d[2]);
			
			}
		}

    //可能有bug,是否缺一半（左边）？

//smooth dealt(平滑处理）
		for(int y=0; y<height-1; y++){
			for(int x=0; x<width-1; x++){
				if(tabImg.at(x, y)!=0  && tabImg.at(x+1, y+1)!=0 && fabs(depth.at(x, y) - depth.at(x+1, y+1)) <= m_fSmoothThreshold * (m_fMaxDsp-m_fMinDsp)/100.0 ){
					if(tabImg.at(x, y+1) != 0
						&& fabs(depth.at(x, y) - depth.at(x, y+1)) <= m_fSmoothThreshold * (m_fMaxDsp-m_fMinDsp)/100.0
						&& fabs(depth.at(x+1, y+1) - depth.at(x, y+1)) <= m_fSmoothThreshold * (m_fMaxDsp-m_fMinDsp)/100.0){
							fprintf(file,"f %d %d %d\n", tabImg.at(x, y), tabImg.at(x, y+1), tabImg.at(x+1, y+1));
					}
					if(tabImg.at(x+1, y)!=0
						&& fabs(depth.at(x, y) - depth.at(x+1, y)) <= m_fSmoothThreshold * (m_fMaxDsp-m_fMinDsp)/100.0
						&& fabs(depth.at(x+1, y+1) - depth.at(x+1, y)) <= m_fSmoothThreshold * (m_fMaxDsp-m_fMinDsp)/100.0){
							fprintf(file,"f %d %d %d\n", tabImg.at(x, y), tabImg.at(x+1, y+1), tabImg.at(x+1,y));
					}



				}
			}
		}
		fclose(file);
	
}


