#include "SelectPoint.h"
#include "ZNCCConfEstimator.h"

SelectPoint::SelectPoint(LSStructureMotion* pMotion,int startFrame,int endFrame)
	:mMotion(pMotion),mStartFrame(startFrame),mEndFrame(endFrame)
{

}


SelectPoint::~SelectPoint(void)
{
}

void SelectPoint::SetParam(SelectPointParam& param)
{
	SPParam = param;
	SPParam.Dump();
}

void SelectPoint::doStart(std::string& FileName)
{
	std::vector<Wml::Vector3f> SelectedVerts;
	std::vector<Wml::Vector3f> SelectedNoraml;


	Start(SelectedVerts,SelectedNoraml);	


	WritePts(SelectedVerts,SelectedNoraml,FileName);
	
}


void SelectPoint::Start(std::vector<Wml::Vector3f>& SelectedVerts,
										std::vector<Wml::Vector3f>& SelectedNoraml)
{

////////////////////////////////////////////////////////////////////////////////////////////
 //Part1:  

	LoadDepthAndMask();


	std::vector<float* > ConfMapList;
	for(int camId = 0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame = mMotion->GetFrameAt(camId,mStartFrame);		
		float* tmp = new float[curFrame->GetImgWidth() * curFrame->GetImgHeight() ];
		ConfMapList.push_back( tmp );
	}

	EstConffidence(ConfMapList);    //此步骤之前存在错误(直接由置信度图像看出），需要debug


 //Through Verified!!
//////////////////////////////////////////////////////////////////////////////////////////////



//Part2: 
	//////////////////////////////////////////////////////////////////////////
	std::vector<int* > OccupyMapList;
	for(int camId = 0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame = mMotion->GetFrameAt(camId,mStartFrame);
		const int width = curFrame->GetImgWidth();
		const int height = curFrame->GetImgHeight();

		int* tmp = new int[ width* height ];
		int idx = 0;
		//ZByteImage& alphaImg = curFrame->m_AlphaMap;
		for(int py = 0;py<height;py++)
			for(int px = 0;px<width;px++)
				if( curFrame->IsInMask(px,py) )
				{
					tmp[idx] = 0;
					idx++;
				}
				else
				{
					tmp[idx] = 255;
					idx++;
				}

				OccupyMapList.push_back(tmp);
	}

	std::vector<float> CoarseDisp;
	float delta = fabs(SPParam.mDspMax - SPParam.mDspMin) / (float)(SPParam.mCoarseInteval - 1);
	for(int iseg = 0;iseg<SPParam.mCoarseInteval;iseg++)
	{
		float tmp = iseg * delta + SPParam.mDspMin;
		CoarseDisp.push_back(tmp);
	}



	//////////////////////////////////////////////////////////////////////////
	
	//下面是选点，选择的过程是首先选择高优先级的点，
	
	std::vector<ThreeDPoint* > SelectedVertex;

	ProjSelect(SelectedVertex,CoarseDisp,OccupyMapList,ConfMapList);
	//////////////////////////////////////////////////////////////////////////	



	int VChoose = 0;
	for(int iVertex = 0;iVertex< SelectedVertex.size();iVertex++)
	{
		ThreeDPoint* CurVertex = SelectedVertex[iVertex];
		bool suc = true;
		suc = VisualhullCheck(CurVertex->Vertex3D);
		if(suc)
		{
			
		}		

		CurVertex->isValid = suc;
	}


	//free the memory
	for(int iVertex = 0;iVertex< SelectedVertex.size();iVertex++)
	{
		ThreeDPoint* CurVertex = SelectedVertex[iVertex];
		if(CurVertex->isValid)
		{
			Wml::Vector3f v = CurVertex->Vertex3D;
			Wml::Vector3f n = CurVertex->Normal;
			float newCof = CurVertex->nccConf ;
			n = n * newCof;
			SelectedVerts.push_back(v);
			SelectedNoraml.push_back(n);
			VChoose++;
		}

		delete CurVertex;
		SelectedVertex[iVertex] = 0;
	}

	printf("%d pts,final choose pts%d\n",SelectedVertex.size(),VChoose);
	//////////////////////////////////////////////////////////////////////////
	//release
	for(int camId = 0;camId<(int)OccupyMapList.size();camId++)
	{
		delete OccupyMapList[camId];
		delete ConfMapList[camId];
	}

	ReleaseDepthAndMask();
  
}


void SelectPoint::LoadDepthAndMask()
{
	for(int camId = 0;camId<CamNum;camId++)
	{
	 LSVideoFrame* pFrame=mMotion->GetFrameAt(camId,mStartFrame);

		
	 pFrame->LoadDepthImg();
	 pFrame->LoadMaskImg();
	// pFrame->SaveModel(true);
	}
	//std::cout<<"load mask and depth over!"<<std::endl;
}

//Select_Conf
void  SelectPoint::PrepareImg(LSStructureMotion*  pMOtion,
	std::vector<int >&  frameNoSet,
	std::vector<ZByteImage* >&  loadImg)
{
	for(int i = 0;i<(int)frameNoSet.size();i++)
	{
		int frameNo = frameNoSet[ i ];
		LSVideoFrame* curFrame =  pMOtion->GetFrameAt(frameNo,mStartFrame);

		/*
		if(curFrame->m_pColorImg != NULL){
			delete curFrame->m_pColorImg;
			curFrame->m_pColorImg = NULL;
		}
		*/
	/*	
		CxImage image;
		//image.Load(m_sImgPathName.c_str(), CXIMAGE_FORMAT_PNG);
		//image.Load(m_sImgPathName.c_str(), CXIMAGE_FORMAT_PNM);

		if(image.Load(curFrame->m_sColorImgPathName.c_str()) == false){
			std::cout << "Can not find color image file: " << curFrame->m_sColorImgPathName << std::endl;
			//return false;
		}
		curFrame->m_pColorImg = new ZByteImage(image.GetWidth(), image.GetHeight(), 3);
		CxImageToZImage(image, *curFrame->m_pColorImg);

		loadImg.push_back(curFrame->m_pColorImg);
		//return true;
	*/	
		CxImage image;
		//CString  imageName = curFrame->ImgFileName().c_str();
		std::string  imageName = curFrame->m_sColorImgPathName;
		bool loadSuc = image.Load( imageName.c_str() );
		if( !loadSuc )
			std::cout<<"failed when loading image\n";
		ZByteImage* pColorImg = new ZByteImage(image.GetWidth(),image.GetHeight(),3);
		CxImageToZImage(image,*pColorImg);
		loadImg.push_back(pColorImg );
		

	}
}


//Select_Conf
void SelectPoint::EstConffidence(std::vector<float* >& ConfMapList)
{
	const int MaxNeigh =max(  min( ( CamNum-1) / 13,4) , 1 ) ;
	printf("Total Neighber Num:%d\n",MaxNeigh * 2);

	for(int camId=0;camId<CamNum;camId++)
	{
		std::cout<<"Est"<<camId<<"'s confidence\n";
		LSVideoFrame*  curFrame = mMotion->GetFrameAt(camId,mStartFrame);
		//ZByteImage&  alphaImg = curFrame->m_AlphaMap;
		curFrame->LoadMaskImg();
		const int width = curFrame->GetImgWidth();
		const int height = curFrame->GetImgHeight();
		//
		CxImage qw;
		qw.Create(width,height,32);

		std::vector<int >  frameNoSet;
		frameNoSet.push_back( camId);
		for(int stride = -MaxNeigh;stride<=MaxNeigh;stride++)
		{
			if(stride==0) continue;

			int tmpsize = CamNum;
			//std::cout<<"tmpsize："<<tmpsize<<std::endl;

			int NeighIdx = ( stride  + camId + tmpsize) % tmpsize;

		//	std::cout<<"NeighIdx:"<<NeighIdx<<" ";
			frameNoSet.push_back( NeighIdx );   // Bug与此有关

		}
     //  std::cout<<std::endl;

     std::vector<ZByteImage* >  loadImgSet;
     
	/*
	 //Prepare Current and Neighbor Reference Images
	 for(int i = 0;i<(int)frameNoSet.size();i++)
	 {
		 int frameNo = frameNoSet[ i ];
		 LSVideoFrame* curFrame =  mMotion->GetFrameAt( frameNo,mStartFrame);
		 curFrame->LoadColorImg();
		 loadImgSet.push_back(curFrame->m_pColorImg);
	 }
	 */
 	PrepareImg(mMotion,frameNoSet,loadImgSet);


	 ZByteImage *refImage = loadImgSet[ 0 ];  
	 
	 std::vector<LSVideoFrame* >  cmpFrameSet;
	 std::vector<ZByteImage* >  cmpImageSet;
	 for(int i = 1;i<frameNoSet.size();i++)
	 {
		 int frameNo = frameNoSet[ i ];
		 LSVideoFrame* cmpFrame = mMotion->GetFrameAt( frameNo,mStartFrame);
		 cmpFrameSet.push_back( cmpFrame );        //neighbor frame set
		 cmpImageSet.push_back( loadImgSet[ i ] ); //neighbor image set
	 }

#define   useZNCC
#ifdef useZNCC 
	 ZNCCConfEstimator  worker( curFrame,cmpFrameSet,refImage,cmpImageSet );
#else
	 NormalConfEstimator  worker( curFrame,cmpFrameSet,refImage,cmpImageSet, 
		 SPParam.mBackProjImgSigma);
#endif

	 float*  curConfMap = ConfMapList[camId];
	 for(int py = 0;py < height;py++)
		 for(int px = 0;px < width;px++)
		 {
			 if(!curFrame->IsInMask(px,py))
				 continue;
#if defined  useZNCC
			 float ZNCC = worker.estPixelConf( px,py,3 ) * 0.91f + 0.1f;
			 if(ZNCC>1.0) ZNCC = 1.0;
			 if( ZNCC<0.0 ) ZNCC = 0.0f;
			 curConfMap [px + py * width]  = ZNCC;
#else

			 curConfMap [px + py * width]  = worker.estPixelConf(px,py);
			 //EstPixelConf(px,py,frameNoSet,loadImgSet,
			 //	curConfMap [px + py * width] );
#endif

			 RGBQUAD rgb;
			 rgb.rgbRed = curConfMap [px + py * width] * 255.0f;
			 rgb.rgbGreen = rgb.rgbRed;
			 rgb.rgbBlue = rgb.rgbRed;
			 qw.SetPixelColor(px,height - 1 - py,rgb);
		 }



			static char  tmpChar[8];
			sprintf(tmpChar,"%d",camId);
			std::string file (tmpChar);
			file =mMotion->m_sMeshDir+ file + ".jpg";
			qw.Save(file.c_str(),CXIMAGE_FORMAT_JPG);

			for(std::vector<ZByteImage* >::iterator  iter = loadImgSet.begin();
				iter!=loadImgSet.end();++iter)
				delete  (*iter);
				
	}
}

//Select_Priority
bool  ConfGreater(const ThreeDPoint* x,const ThreeDPoint* y)
{
	return x->Conf > y->Conf;
}

//Select_Priority
void SelectPoint::ProjSelect(std::vector<ThreeDPoint* >&SelectedVertex,std::vector<float>& CoarseDisp, 
	std::vector<int* >& OccupyMapList,std::vector<float* >& ConfMapList)
{
	const int MaxPass = 10;
	const  float ConfThreshold = 0.84f;
	float segconf = (ConfThreshold - SPParam.mLeastConf) / (float)(MaxPass - 1);
	for(int iPass = 0;iPass<=MaxPass;iPass++)
	{
		float confStart = ConfThreshold - iPass * segconf;
		float  confEnd = ConfThreshold - iPass * segconf  + segconf;
		if(iPass==0)
			confEnd = 1.0f;
		for(int iSeg = (int)CoarseDisp.size() - 2; iSeg>=0;iSeg--)
		{
			std::vector<ThreeDPoint* > CadidateV;
			float segStartDisp = CoarseDisp[iSeg];
			float segEndDisp = CoarseDisp[iSeg + 1];
			for(int camId= 0;camId<CamNum;camId++)
			{
				SelectCadidateV(CadidateV,segStartDisp,segEndDisp,ConfMapList[camId],
					camId,OccupyMapList[camId],
					confStart,confEnd );
			}

		
			// sort acooding to the confidence
			std::sort(CadidateV.begin(),CadidateV.end(),ConfGreater );
			std::cout<<iPass<<":cadidate  "<<CadidateV.size()<<"\n";

			ProjOccupy(SelectedVertex, CadidateV,OccupyMapList);
			std::cout<<iPass<<":selected  "<<SelectedVertex.size()<<"\n";

			DebugDesity(OccupyMapList);
		
		}

	}

}





//Select_Priority
//从这一帧里面选择置信度在某个区间的点，并满足密度，以及mask的要求
void SelectPoint::SelectCadidateV(std::vector<ThreeDPoint* >&CandidateV,
	float segstart,float segend,float* ConfMap,
	int CurCamNo,int* OccupyMap,float confStart,float confEnd)
{
	const int Margin = 3;
	LSVideoFrame*  curFrame = mMotion->GetFrameAt( CurCamNo, mStartFrame);
//	ZByteImage& alphaImg = curFrame->m_AlphaMap;

	//ZFloatImage& depthImg = curFrame->m_DepthImg;
	ZFloatImage& depthImg = *curFrame->m_pDspImg;

	//Wml::Vector3f OptiCenter = curFrame->GetOptiCenter();
    Wml::Vector3d OptiCenter=curFrame->GetCameraPos(); //若存在bug，需检查此行


	const int width = curFrame->GetImgWidth();
	const int height = curFrame->GetImgHeight();


	calNormalFn svdFn = SVDPtsSetNormal;


	for(int py = Margin;py<height- Margin;py++)
		for(int px = Margin;px<width - Margin;px++)

		{

			int idx = px + py * width;
			if( !curFrame->IsInMask(px,py) || OccupyMap[idx]>=SPParam.mDensity  || 
				ConfMap[idx]<confStart  || ConfMap[idx]>confEnd )
				continue;			

			double InvZ = depthImg.at( px,py );
			if(InvZ>=segstart && InvZ <segend )
			{
				//Wml::Vector3f XYZ= curFrame->ProjUVZtoWorldXYZf((double)px,(double)py,1.0 / InvZ );
				Wml::Vector3d XYZ=Reproject(curFrame,Wml::Vector2d((double)px,(double)py),1.0/InvZ);
				//////////////////////////////////////////////////////////////////////////

			   Wml::Vector3f fXYZ=CvtV3dToV3f(XYZ);

				bool suc = VisualhullCheck(fXYZ);
				if(!suc) continue;

			 
				Wml::Vector3f Normal;
				float cosangle ;
				{
					Wml::Vector3d OptiView = OptiCenter - XYZ;
					Wml::Vector3f fOptiView=CvtV3dToV3f(OptiView);
					bool suc = false;
					suc = 
						FitNormal(svdFn,curFrame,px,py,Normal,OptiView);
                 

					if(!suc)  continue;
					Normal.Normalize();
					OptiView.Normalize();
					cosangle = Normal.Dot( fOptiView ) ;
				
				}
				//////////////////////////////////////////////////////////////////////////

			   // Wml::Vector3f fXYZ=CvtV3dToV3f(XYZ);
				ThreeDPoint* tmpPoint = new ThreeDPoint;
				tmpPoint->Vertex3D = fXYZ;
				//tmpPoint->MainFrameRGB = CurVFrame->GetPixelColor(px,py);
				tmpPoint->Normal = Normal;
				//???may need notice or change  ,tmpPoint->MainFrameIdx = CurVFrame->FrameNo
				tmpPoint->MainFrameIdx = CurCamNo;
				tmpPoint->x = px;
				tmpPoint->y = py;

				tmpPoint->Conf = ConfMap[idx] * InvZ * cosangle;
				//sqrt(cosangle) ;
				tmpPoint->nccConf = ConfMap[idx] ;
				//ConfMap[idx] * 0.9f + InvZ * 0.05f + cosangle * 0.05f;
				//ConfMap[idx] * InvZ * sqrt(cosangle);//ConfMap[idx] * 0.75f + InvZ * 0.1f + cosangle * 0.15f;
				CandidateV.push_back(tmpPoint);
				
			}

		}

}


//Select_Priority
void SelectPoint::ProjOccupy(std::vector<ThreeDPoint* >&SelectedV,std::vector<ThreeDPoint* >&CandidateV,
							 std::vector<int* >& OccupyMapList )
{
	std::cout<<"enter ProjOccupy Func \n"<<"Candidates Set Size:"<<CandidateV.size()<<"\n";
	/*
	这里candidate中的点一定是在除去margin的区域，而且这些一定在对应图片的Visualhull内
	首先检测每个点投影的MainFrameIdx  Frame上的点是否已经达到一定的密度，达到一定密度就将这个点删除
	如果没有达到一定密度则将其加入最终的所选点中，并且将其投射到其他帧上，覆盖投射点附近的像素。原来帧像
	素周围的像素也被覆盖一次
	*/


	int DebugCnt = 0;
	for(int iVertex = 0;iVertex<(int) CandidateV.size();iVertex++)
	{
		ThreeDPoint* CurVertex = CandidateV[iVertex];
		const  int mainFrameNo = CurVertex->MainFrameIdx;


		//std::cout<<"mainFrameNo: "<<mainFrameNo<<std::endl;
		LSVideoFrame* curFrame = mMotion->GetFrameAt( mainFrameNo,mStartFrame );
		const int width = curFrame->GetImgWidth();
		const int height = curFrame->GetImgHeight();
		const int centerX = CurVertex->x;
		const int centerY = CurVertex->y;
		int* CurOccupyMap = OccupyMapList[mainFrameNo-0];

		
		if(CurOccupyMap[centerX +  centerY * width]<SPParam.mDensity) 
			///fill the pixels neighber to this vertex
		{
			//选中
			SelectedV.push_back(CurVertex);

			//fill the frame the vertex in 
			const int MaxOffset = SPParam.mHalfWinSizeForDensity;			
			for(int y = centerY - MaxOffset;y<=centerY + MaxOffset;y++)
				for(int x = centerX - MaxOffset;x<=centerX + MaxOffset;x++)
					if(x>=0 && y>=0 && x < width && y < height)
					{
						int& tmp = CurOccupyMap[x + y * width ];
						tmp = min( tmp + 1, SPParam.mDensity );
					}

					//fill in the proj frame
					for(int camId = 0;camId<CamNum;camId++)
					{
						if(camId== CurVertex->MainFrameIdx)
							continue;
						
						ProjCoccupyIthFrame(CurVertex,OccupyMapList[ camId - 0 ],
							mMotion->GetFrameAt(camId,mStartFrame) );
						
					}

		}
		else 
		{
			DebugCnt++;
			delete CurVertex;
			CandidateV[iVertex] = 0;
		}

	}
	std::cout<<"discard because of desity,"<<DebugCnt<<"\n"; 
}

//Select_Priority
void SelectPoint::ProjCoccupyIthFrame(ThreeDPoint* CurVertex,int* OccupyMap,LSVideoFrame* otherFrame)
{
     double guassInvZ;
	double CurU,CurV;
	Wml::Vector3f Pt  =CurVertex->Vertex3D;
	Wml::Vector3d dPt=CvtV3fToV3d(Pt);

	Wml::Vector3f  projUVZ;
	
	//otherFrame->ProjXYZtoUVZf( Pt,projUVZ );
	//CurU = projUVZ.X();	        CurV = projUVZ.Y();
	//guassInvZ = 1.0 / ( projUVZ.Z() );
	
	otherFrame->GetImgCoordFrmWorldCoord(CurU, CurV, guassInvZ, dPt); //double u, v, dsp;


	if( CurU>0&&CurU<(otherFrame->GetImgWidth())&&CurV>0&&CurV<(otherFrame->GetImgHeight()))
	{
		//Wml::Vector3f OptiView = otherFrame->GetOptiCenter();
		Wml::Vector3d OptiView = otherFrame->GetCameraPos();
		Wml::Vector3f fOptiView=CvtV3dToV3f(OptiView);

		fOptiView -= CurVertex->Vertex3D;
		fOptiView.Normalize();

		int centerX = (int) (CurU + 0.5f);
		int centerY = (int) (CurV + 0.5f);

		float cosangle = fOptiView.Dot(CurVertex->Normal);
		//ZFloatImage& otherDepthImg = otherFrame->m_DepthImg;
		ZFloatImage& otherDepthImg = *otherFrame->m_pDspImg;


		float CurInvZ = otherDepthImg.at(centerX,centerY);
		float Delta = fabs( SPParam.mDspMax -  SPParam.mDspMin)*0.002f; 

		//ZByteImage&  otherAlphaImg = otherFrame->m_AlphaMap;
		if(  otherFrame->IsInMask( centerX,centerY ) &&
			cosangle>CosAngleFilter && 
			/*guassInvZ>(CurInvZ - Delta)*/ fabs( guassInvZ - CurInvZ) < Delta  )
		{
			const int width = otherFrame->GetImgWidth();
			const int height = otherFrame->GetImgHeight();
			const int MaxOffset = SPParam.mHalfWinSizeForDensity;
			for(int py = max(centerY - MaxOffset,0);py<=min(centerY + MaxOffset,height - 1);py++)
				for(int px = max(centerX - MaxOffset,0);px<=min(centerX + MaxOffset,width - 1);px++)
				{
					int& tmp = OccupyMap[px + py * width];
					tmp = min(tmp + 1,SPParam.mDensity);
				}

		}//end if

	}//end if 

}

//Select_VisualHull
void SelectPoint::DebugDesity(std::vector<int*>& OccupyMapList)
{
	for(int camId= 0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame = mMotion->GetFrameAt( camId,mStartFrame);
		int* OccupyMap = OccupyMapList[camId - 0];
		CxImage tmp;
		const int width = curFrame->GetImgWidth();
		const int height = curFrame->GetImgHeight();
		tmp.Create(width,height,32);
		for(int py = 0;py<height;py++)
			for(int px = 0;px<width;px++)
			{
				int idx = px + py * width;
				RGBQUAD rgba;
				if(OccupyMap[idx]>(SPParam.mDensity*2)  ||  (OccupyMap[idx]==0) )
				{
					rgba.rgbGreen = 0;
					rgba.rgbRed = 0;
					rgba.rgbBlue = 0;
				}	
				else
				{
					rgba.rgbRed = (OccupyMap[idx] + 7) * 15;
					rgba.rgbGreen = (OccupyMap[idx] + 7) * 15;
					rgba.rgbBlue = (OccupyMap[idx] + 7) * 15;
				}

				tmp.SetPixelColor(px,height - 1 - py,rgba);
			}

			static char tmpChar[8];
			sprintf(tmpChar,"%d",camId);

			std::string file( tmpChar );
			//file  = getMeshConstructManager().getOccupyMapDir + file + ".jpg";
			file  = mMotion->m_sMeshDir + file + ".jpg";          //可根据此处代码，验证Part2的代码是否正确！！！
			tmp.Save(file.c_str(),CXIMAGE_FORMAT_JPG);
	}
}


//Select_VisualHull
void SelectPoint::ReleaseDepthAndMask()
{
	for(int camId = 0;camId<CamNum;camId++)
	{
		LSVideoFrame* curFrame = mMotion->GetFrameAt( camId,mStartFrame );
		curFrame->Clear();
		//curFrame->ClearData( SWAP_ALL );
	}
}


//Select_VisualHull
void SelectPoint::WritePts(std::vector<Wml::Vector3f>& PointCloud,
	std::vector<Wml::Vector3f>& NormalCloud,std::string& FileName)
{
	std::cout<<"begin write pts\n";
	FILE* fp = fopen(FileName.c_str(),"w");

	assert(PointCloud.size()==NormalCloud.size() );

	for(int i = 0;i<(int)PointCloud.size();i++)
	{
		fprintf(fp,"%f %f %f %f %f %f\n",PointCloud[i][0],PointCloud[i][1],PointCloud[i][2],
			NormalCloud[i][0],NormalCloud[i][1],NormalCloud[i][2] );
		//fprintf(fp,"%f %f %f\n",PointCloud[i][0],PointCloud[i][1],PointCloud[i][2]);
	}

	fclose( fp );
	std::cout<<"save pts success\n";
}







//LY Add Funciton
Wml::Vector3d SelectPoint::Reproject(LSVideoFrame* pVFrm, Wml::Vector2d p, double z)
{
	Wml::Vector3d P;

	pVFrm->GetWorldCoordFrmImgCoord(p.X(), p.Y(), 1.0/z, P);
	return P;
}

Wml::Vector3d SelectPoint::CvtV3fToV3d(Wml::Vector3f& v3fNum)

{
	Wml::Vector3d cngNum;
	cngNum[0]=(double)v3fNum[0];
	cngNum[1]=(double)v3fNum[1];
	cngNum[2]=(double)v3fNum[2];
	return cngNum;
}

Wml::Vector3f SelectPoint::CvtV3dToV3f(Wml::Vector3d& v3dNum)
{
	Wml::Vector3f cngNum;
	cngNum[0]=(float)v3dNum[0];
	cngNum[1]=(float)v3dNum[1];
	cngNum[2]=(float)v3dNum[2];
	return cngNum;
}




//Select_VisualHull
bool SelectPoint::VisualhullCheck( Wml::Vector3f& ThreeDV )
{
	/*
	由于这里的边界是hard边界，所以这里当一个点只是在某一张图片中没有投影到Visualhull
	时并不认为它是outlier，
	另外如果一个点投影到任何的图片中都在图片边界之外时也认为这个点是outlier,至少在3张图片的Visualhull中
	*/
	int inVHCnt = 0;
	int outVHCnt = 0;

	Wml::Vector3d dThreeDV=CvtV3fToV3d(ThreeDV);
		//Temp
		//for(int m = 0; m < 2; m++){
		for(int camId = 0; camId< CamNum; camId++)
		{
			LSVideoFrame*  curFrame = mMotion->GetFrameAt(camId, mStartFrame);

			double NeighU, NeighV, out_dsp;
			curFrame->GetImgCoordFrmWorldCoord(NeighU, NeighV, out_dsp, dThreeDV);

			const int width = curFrame->GetImgWidth();
			const int height = curFrame->GetImgHeight();
			if(NeighU>0.5 && NeighV>0.5 && NeighU + 1.5<width && NeighV + 1.5<height )
			{
				int tmpu = (int) (NeighU + 0.5f);
				int tmpv = (int) (NeighV + 0.5f);
				bool inMask = (  curFrame->IsInMask(tmpu - 1,tmpv)  ||
					curFrame->IsInMask(tmpu,tmpv)    ||
					curFrame->IsInMask(tmpu + 1,tmpv)  ||
					curFrame->IsInMask(tmpu,tmpv - 1) || 
					curFrame->IsInMask(tmpu,tmpv + 1)
					);

				if( inMask== false)
					outVHCnt++;
				else inVHCnt++;

			}
			else 
				outVHCnt++;
			if(outVHCnt>2) 
				break;
		}
	


	if(outVHCnt>2 ||  inVHCnt <=1)//inVHCnt  is at least one
		return false;
  
	return true;
}



//Select_VisualHull
bool SelectPoint::FitNormal(calNormalFn  fn,LSVideoFrame* CurVFrame,int x,int y,
	Wml::Vector3f& Noraml,Wml::Vector3d& OptiView)
{
	const int HalfWSize = min(SPParam.mHalfWinSizeForPlaneFit,5);
	const int LeastPT = 5;

	OptiView.Normalize();

	std::vector<Wml::Vector3f> XYZSet;
	//ZByteImage& alphaImg = CurVFrame->m_AlphaMap;
	//ZFloatImage& depthImg = CurVFrame->m_DepthImg;
	ZFloatImage& depthImg = *CurVFrame->m_pDspImg;

	XYZSet.reserve(HalfWSize * HalfWSize * 2);
	for(int offsety = -HalfWSize;offsety<=HalfWSize;offsety++)
		for(int offsetx = -HalfWSize;offsetx<=HalfWSize;offsetx++)
		{
			int px = offsetx + x;
			int py = offsety + y;
			//if(  CurVFrame->InImg(px,py) && CurVFrame->IsInMask(px,py) )
			if(px>0&&px<(CurVFrame->GetImgWidth())&&py>0&&py<(CurVFrame->GetImgHeight()) && (CurVFrame->IsInMask(px,py)))
			{
				double invZ = depthImg.at(px,py);
				//Wml::Vector3d XYZ = CurVFrame->ProjUVZtoWorldXYZf( (double)px,(double)py,1.0 / invZ);
				Wml::Vector3d XYZ = Reproject(CurVFrame,Wml::Vector2d((double)px,(double)py),1.0 / invZ);

				Wml::Vector3f fXYZ=CvtV3dToV3f(XYZ);
				XYZSet.push_back(fXYZ);
			}

		}
		
		Wml::Vector3f fOptiView=CvtV3dToV3f(OptiView);
		
		if((int)XYZSet.size()<LeastPT)
			return false;
		else
		{
			bool suc = fn(XYZSet,Noraml);
			if(suc ==false)
				return false;

			float cosangle = fOptiView.Dot(Noraml);
			if(cosangle<0.0)
			{
				Noraml = -Noraml;
				cosangle = -cosangle;
			}
			if(cosangle<CosAngleFilter)
				return false;
			else
				return true;
		}
		
}