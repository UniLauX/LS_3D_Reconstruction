#include "PointCloudSampling.h"
#include "LSVideoFrame.h"
#include "MyHeap.h"


typedef std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > >   NodeType1;

typedef std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > >   NodeType2;

 bool cmp1(const NodeType1 &a, const NodeType1 &b)
{
	return a.first>b.first;
}

bool cmp2(const NodeType2 &a, const NodeType2 &b)
{
	return a.first>b.first;
}


PointCloudSampling::PointCloudSampling(void)
{
}

PointCloudSampling::~PointCloudSampling(void)
{
}


void PointCloudSampling::fPointCloudSampling( int iSampRds)
{
	std::cout<<"execute point cloud sampling function"<<std::endl;

	int ithFrame=0;

    if(mStartFrame==mEndFrame)
		ithFrame=mStartFrame;
	else	
		std::cout<<"mStartFrame not equal to mEndFrame"<<std::endl;

	std::cout<<"mStartFrame: "<<mStartFrame<<" "<<"mEndFrame: "<<mEndFrame<<std::endl;

	
	int iFrmNum = 1;
	std::cout<<"ithFrame: "<<ithFrame<<std::endl;
	

	//get image width and image height
	int iWidth = mMotion->GetFrameAt(0, ithFrame)->GetImgWidth();
	int iHeight = mMotion->GetFrameAt(0, ithFrame)->GetImgHeight();

	// load depth image and mask image 
	for(int iFrame = ithFrame; iFrame <= ithFrame; iFrame++)  
	{
		for(int camId=0;camId<CamNum;camId++)
		{
			LSVideoFrame* pFrm = mMotion->GetFrameAt(camId, iFrame);
			pFrm->LoadDepthImg();
			pFrm->LoadMaskImg();
			pFrm->LoadColorImg();    //used for test
		}
	}


   for(int iOutFrame = ithFrame; iOutFrame<= ithFrame; iOutFrame++)
   {  
	     std::cout<<"ith Frame:"<<ithFrame<<"/"<<ithFrame<<std::endl;
		 for(int outCamId=0;outCamId<CamNum;outCamId++)
		 {
			 LSVideoFrame* pFrm = mMotion->GetFrameAt(outCamId,iOutFrame);
			 Wml::GMatrix<Wml::Vector3d> PtMp(iWidth, iHeight);
			 for(int y = 0; y < iHeight; y++)
			 {
				 for(int x = 0; x < iWidth; x++)
				 {
				 
					 if(pFrm->IsInMask(x, y) == false)
						 continue;
					 else
					 {
						 // get world coord from image coord
						 PtMp(x, y) = Reproject(pFrm, Wml::Vector2d(x, y), 1.0 / pFrm->GetDspAt(x, y));
						 /*  see the disparity value
						 std::cout<<"dsp!:"<<pFrm->GetDspAt(x, y)<<std::endl;
						  if(pFrm->GetDspAt(x, y)<0)
							   std::cout<<"x:"<<x<<",y:"<<y<<" "<<pFrm->GetDspAt(x, y)<<std::endl;
							   */
					 }
					
					
				 }
			 }


	        int totalFrmNum=iFrmNum*CamNum;
			std::vector<bool> refFrame(totalFrmNum,false);
			 //std::vector<std::pair<bool,bool>> refFrame(mEndFrame+1,CamNum);

			 Wml::Vector3d dir1 = pFrm->GetCameraDir();   //   m_pLsCameraParameter->m_R.GetRow(2);

			// iFrame和camId的目录层次关系可能有问题
			 for(int iInFrame = ithFrame; iInFrame <= ithFrame; iInFrame++)
			 {
				for(int inCamId=0;inCamId<CamNum;inCamId++)
				{
				 Wml::Vector3d dir2 = mMotion->GetFrameAt(inCamId, iInFrame)->GetCameraDir();
				 if(dir1.Dot(dir2) > 0)
					// iInFrame=totalFrmNum/CamNum;
				     
					 refFrame[inCamId] = true;
				    // refFrame[camId,iFrame]=true;
				}
			 }
        
			 
			   /*  std::priority_queue<std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > >, 
				 std::vector<std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > > >, 
				 std::greater<std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > > > > Q1;  
		    	*/
				MyHeap<NodeType1> Q1(cmp1);
			 
			 //std::multimap<float, Wml::Vector<6, float> > PtCloudMp;
			 for(int y = 0; y < iHeight; y += iSampRds)
			 {
				 for(int x = 0; x < iWidth; x += iSampRds)
				 {
					 //if(pFrm->IsInMask(x, y) == true && pFrm->GetDspAt(x, y) > fMinDsp)
					 if(VisualhullCheck( PtMp(x, y)) == false)
						 continue;

					 Wml::Vector3d& P = PtMp(x, y);
					 Wml::Vector3d Norm = Normal(Wml::Vector2<int>(x, y), PtMp, pFrm);

					 float fConf = 0;
					 int iRefFrmNum = 0;
					 for(int iInFrame= ithFrame; iInFrame <= ithFrame; iInFrame++)
					 {
						 for(int inCamId=0;inCamId<CamNum;inCamId++)
						 {
							 if(!((iOutFrame==iInFrame)&&(outCamId==inCamId)) && refFrame[inCamId] == true)
							 {
								 LSVideoFrame* pRefFrm = mMotion->GetFrameAt(inCamId, iInFrame); //lpGlobalMotion->GetCameraFrame(r);
								 //ZFloatImage& DspMap2 = *(FrmDspMap[r-mStartFrame]);

								 double u, v, dsp;
								 pRefFrm->GetImgCoordFrmWorldCoord(u, v, dsp, P);
								 //Wml::Vector4d P2 = pRefFrm->m_objAbsTransformMG * Wml::Vector4d(P.X(), P.Y(), P.Z(), 1.0);
								 //Wml::Vector3d p = pRefFrm->K() * Wml::Vector3d(P2.X(), P2.Y(), P2.Z());
								 //p /= p.Z();
								 Wml::Vector3d p(u, v, 1.0);

								 if(p.X() >= 0 && p.X() < iWidth - 1 && p.Y() >= 0 && p.Y() < iHeight - 1 && pRefFrm->IsInMask(u, v) == true)
								 {
									 //float fPrjErr = DspVl(Wml::Vector2f(p.X(), p.Y()), DspMap2) - dsp;
									 float fPrjErr = pRefFrm->GetDspAt(p.X(), p.Y(), true) - dsp;
									 fConf += fPrjErr * fPrjErr;
									 iRefFrmNum++;
								 }
							 }
						 }
						
					 }

					 if(iRefFrmNum < 2)
						 continue;
					 fConf = sqrt(fConf / iRefFrmNum);
					 std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > > Data;

					 Data.first = fConf;

					 Data.second.first[0] = P.X();
					 Data.second.first[1] = P.Y();
					 Data.second.first[2] = P.Z();
					 Data.second.first[3] = Norm.X();
					 Data.second.first[4] = Norm.Y();
					 Data.second.first[5] = Norm.Z();

					 Data.second.second[0] = x;
					 Data.second.second[1] = y;

					 //PtCloudMp.insert(make_pair(float(dPrjErr), PtNorm));
					 // Q1.push(Data);
				    Q1.add(Data);
				 }
			 }


			 //std::sort(Q1.begin(),Q1.end(),cmp1);
          
			 std::string fileName = GetPtCldFileName(pFrm);    // direcoty under the correct camId
			 std::cout<<"fileName:"<<fileName<<std::endl;
          
			 FILE* pFile = fopen(fileName.c_str(), "wb+");
			 
			 if(pFile)
			 {              
 				 int iPtNum = Q1.size();
				 fwrite(&iPtNum, sizeof(int), 1, pFile);
				 //for(std::multimap<float, Wml::Vector3f>::iterator Iter = PtCloudMp.begin(); Iter != PtCloudMp.end(); Iter++)
				 //{
				 //	fwrite(&Iter->first, sizeof(float), 1, pFile);
				 //	fwrite(&Iter->second[0], sizeof(float), 6, pFile);
				 //}
				 
				 
				//  while(!Q1.empty())
				 while(Q1.size()>0)
				 {
				     // std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > > tempData= Q1.top();
					 
					 // std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > > &Data = tempData;

                     std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > > Data = Q1.remove();
					 //std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector2<int> > > Data = Q1[idx];
					 fwrite(&Data.first, sizeof(float), 1, pFile);
					 fwrite(&Data.second.first[0], sizeof(float), 6, pFile);
					 //fwrite(&f, sizeof(int), 1, pFile);
					 fwrite(&Data.second.second[0], sizeof(int), 2, pFile);
					 //fprintf(pFile2, "%f\n", Data.first);
					// Q1.pop();
					// std::cout<<Q1.size()<<std::endl;
					// Q1.remove();     
				 }

				 fclose(pFile); 
				 //fclose(pFile2);
			 }
			

       }
	  
	}
	std::cout<<std::endl;


	for(int iFrame =ithFrame; iFrame <= ithFrame; iFrame++)
	{
		for(int camId=0;camId<CamNum;camId++)
		{
			LSVideoFrame* pFrm = mMotion->GetFrameAt(camId, iFrame);
			pFrm->Clear_depth();
		}
	}
	//FrmDspMap.clear();

	//K way merge sort
	std::vector<FILE*> ReadFileMp(iFrmNum*CamNum);
	std::vector<int> PtNumMp(iFrmNum*CamNum);
	std::vector<int> PtReadNumMp(iFrmNum*CamNum, 0);
	for(int iFrame = ithFrame; iFrame <=ithFrame; iFrame++)
	{
		for(int camId=0;camId<CamNum;camId++)
		{
			LSVideoFrame* pFrm = mMotion->GetFrameAt(camId, iFrame);
			std::string fileName = GetPtCldFileName(pFrm);     //此函数需要区分帧号和相机号
			ReadFileMp[camId] = fopen(fileName.c_str(), "rb");
			fread(&PtNumMp[camId], sizeof(int), 1, ReadFileMp[camId]);
		}
	}
	
	/*   std::priority_queue<std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > >, 
		std::vector<std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > > >, 
		std::greater<std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > > > > Q2; */
	   MyHeap<NodeType2>Q2(cmp2);

	//std::vector<std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > > > Q2;

	for(int iFrame = ithFrame; iFrame <=ithFrame; iFrame++)
	{
		for(int camId=0;camId<CamNum;camId++)
		{
			if(PtNumMp[camId] > 0)
			{
				std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > > Data;
				fread(&Data.first, sizeof(float), 1, ReadFileMp[camId]);
				fread(&Data.second.first[0], sizeof(float), 6, ReadFileMp[camId]);
				Data.second.second[0] = camId;
				fread(&Data.second.second[1], sizeof(int), 2, ReadFileMp[camId]);
				//fread(&Data.second.second[0], sizeof(int), 2, ReadFileMp[(iFrame-mStartFrame)*CamNum+camId]);

				PtReadNumMp[camId] = 1;
				//Q2.push(Data);
			    Q2.add(Data);
			}
		}
	}

	std::vector<ZIntImage*> PrjOccMap(iFrmNum*CamNum);
	for(int iFrame = ithFrame; iFrame <=ithFrame; iFrame++)
	{
	  for(int camId=0;camId<CamNum;camId++)
	  {
		  {
			  PrjOccMap[camId] = new ZIntImage();
			  PrjOccMap[camId]->CreateAndInit(iWidth, iHeight);
		  }

	  }
	}

	//CString ModelFileName = m_MdlFlPth;
	//ModelFileName.AppendFormat("PMVS.npts");

/*
	std::string PointCloudSampling::GetPtCldFileName( LSVideoFrame* pFrm )
	{
		std::string frameName=pFrm->m_sName;
		std::string dirPath=pFrm->s_RawDir[pFrm->m_iCameraId];

		std::cout<<"dirPath: "<<dirPath<<std::endl;
		return dirPath+frameName + "_PtCld" + ".raw";
	}
*/

/*
//两个路径需要扩展到不同帧上
for(int iFrame=0;iFrame<iFrmNum;iFrame++)
{
	LSVideoFrame* pFrame=mMotion->GetFrameAt(0,iFrame);
	//mMotion->m_sMeshDir +"model_"+pFrame->m_sName+std::string(".npts");
    std::string m_dir=mMotion->m_sMeshDir +"model_"+pFrame->m_sName;
    
	std::cout<<"m_dir: "<<m_dir<<std::endl;

     m_NptsFlPth = m_dir + ".npts";
	 m_PtNorm = m_dir + ".obj";
 //   m_NptsFlPth=LS
	//路径错误，尚未赋值
	FILE* pFile1 = fopen(m_NptsFlPth.c_str(), "w+");
	//ModelFileName = m_MdlFlPth;
	//ModelFileName.AppendFormat("PtNorm.obj");
	FILE* pFile2 = fopen(m_PtNorm.c_str(), "w+");
	std::cout<<"Gernerating..."<<std::endl;

}
*/

std::vector<FILE*> pFileArr1 ;
std::vector<FILE*> pFileArr2 ;
pFileArr1.resize(iFrmNum);
pFileArr2.resize(iFrmNum);
//m_NptsFlPthArr.resize(iFrmNum);
//m_PtNormArr.resize(iFrmNum);

for(int iFrame=ithFrame;iFrame<=ithFrame;iFrame++)
{
	LSVideoFrame* pFrame=mMotion->GetFrameAt(0,iFrame);
	std::string m_dir=mMotion->m_sMeshDir +"model_"+pFrame->m_sName;

	//  std::cout<<"m_dir: "<<m_dir<<std::endl;

	m_NptsFlPthArr.push_back(m_dir + ".npts") ;
	m_PtNormArr.push_back( m_dir + ".obj");


	pFileArr1[0] = fopen(m_NptsFlPthArr[ithFrame].c_str(), "w+");       //bug is here
	pFileArr2[0] = fopen(m_PtNormArr[ithFrame].c_str(), "w+");

	if(pFileArr1[0]==nullptr||pFileArr2[0]==nullptr)
		std::cout<<"file point nullptr"<<std::endl;

}

	//while(!Q2.empty())
  // std::sort(Q2.begin(),Q2.end(),cmp2);
    while(Q2.size()>0)
	{
		
		//std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > > tempData = Q2.top();
	    //std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > > TopData = tempData;
		std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > > &TopData = Q2.remove();


		

		if(PrjOccMap[TopData.second.second[0]]->at(TopData.second.second[1], TopData.second.second[2]) < 2)   //此处可能存在Bug(为什么是小于2,不是很清楚)
		{
			Wml::Vector3d P(TopData.second.first[0], TopData.second.first[1], TopData.second.first[2]); //, 1.0);

			Wml::Vector3d normali(TopData.second.first[3], TopData.second.first[4], TopData.second.first[5]);

			for(int iFrame = ithFrame; iFrame <=ithFrame; iFrame++)
			{

			   for(int camId=0;camId<CamNum;camId++)
			   {
				   LSVideoFrame* pRefFrm = mMotion->GetFrameAt(camId, iFrame);
				   //Wml::Vector4d P2 = pRefFrm->m_objAbsTransformMG * P;
				   //Wml::Vector3d p = pRefFrm->K() * Wml::Vector3d(P2.X(), P2.Y(), P2.Z());
				   //p /= p.Z();	

				   Wml::Vector3d dir2 = pRefFrm->GetCameraDir();
				   if(normali.Dot(dir2) >= 0)
					   continue;

				   double u, v, dsp;
				   pRefFrm->GetImgCoordFrmWorldCoord(u, v, dsp, P);
				   //Wml::Vector4d P2 = pRefFrm->m_objAbsTransformMG * Wml::Vector4d(P.X(), P.Y(), P.Z(), 1.0);
				   //Wml::Vector3d p = pRefFrm->K() * Wml::Vector3d(P2.X(), P2.Y(), P2.Z());
				   //p /= p.Z();
				   Wml::Vector3d p(u, v, 1.0);

				   if(p.X() > 0 && p.X() < iWidth - 1 && p.Y() > 0 && p.Y() < iHeight - 1 && pRefFrm->IsInMask(p.X(), p.Y()))
				   {

					   pRefFrm->m_pColorImg->at((int)(u+0.49),(int)(v+0.49))=255;  //test for draw






					   Wml::Vector4<int> OccRect;
					   OccRect[0] = (std::max)(int(floor(p.X() - iSampRds)), 0);
					   OccRect[1] = (std::max)(int(floor(p.Y() - iSampRds)), 0);
					   OccRect[2] = (std::min)(int(ceil(p.X() + iSampRds)), iWidth - 1);
					   OccRect[3] = (std::min)(int(ceil(p.Y() + iSampRds)), iHeight - 1);

					   for(int wy = OccRect[1]; wy <= OccRect[3]; wy++)
					   {
						   for(int wx = OccRect[0]; wx <= OccRect[2]; wx++)
						   {
							   if(Wml::Vector2f(wx - p.X(), wy - p.Y()).Length() < iSampRds)
								   PrjOccMap[camId]->at(wx, wy)++;
						   }
					   }
				   }

			   }
			  /*
			   LSVideoFrame* pFrame=mMotion->GetFrameAt(0,iFrame);
			   std::string m_dir=mMotion->m_sMeshDir +"model_"+pFrame->m_sName;

			 //  std::cout<<"m_dir: "<<m_dir<<std::endl;

			   m_NptsFlPth = m_dir + ".npts";
			   m_PtNorm = m_dir + ".obj";
			   pFile1 = fopen(m_NptsFlPth.c_str(), "w+");     //bug!!! 338 out of range
			   pFile2 = fopen(m_PtNorm.c_str(), "w+");

			   if(pFile1==nullptr||pFile2==nullptr)
				   std::cout<<"file point nullptr"<<std::endl;

			  // std::cout<<"Gernerating..."<<std::endl;
			  // std::cout<< "m_NptsFlPth: "<<m_NptsFlPth<<std::endl;
			  // std::cout<<" m_PtNorm: "<<m_PtNorm<<std::endl;
			  */
      
			  /* fprintf(pFileArr1[0], "%f %f %f %f %f %f\n", TopData.second.first[0], TopData.second.first[1],
				   TopData.second.first[2], -TopData.second.first[3], -TopData.second.first[4], -TopData.second.first[5]);*/
			   fprintf(pFileArr1[0], "%f %f %f %f %f %f\n", TopData.second.first[0], TopData.second.first[1],
				   TopData.second.first[2], TopData.second.first[3], TopData.second.first[4], TopData.second.first[5]);
               //后面三个表示ply中法向量（及其方向）,前三个表示三维点位置坐标


			   fprintf(pFileArr2[0], "vn %f %f %f\nv %f %f %f\n", TopData.second.first[3], TopData.second.first[4], 
				   TopData.second.first[5], TopData.second.first[0], TopData.second.first[1], TopData.second.first[2]);

      /*
			   //粒度太细
              //test casting postion
			   Wml::Vector3d Pword(TopData.second.first[0], TopData.second.first[1], TopData.second.first[2]);
			   for(int camId=0;camId<CamNum;camId++)
			   {
				     LSVideoFrame* pFrm = mMotion->GetFrameAt(camId, iFrame);
					 pFrm->LoadColorImg();

				     std::cout<<"Filename: "<<pFrm->m_sName<<std::endl;
					 double u, v, dsp;
					 pFrm->GetImgCoordFrmWorldCoord(u, v, dsp, Pword);
					 Wml::Vector3d p(u, v, 1.0);
					 if((int)(u+0.49)>0&&(int)(u+0.49)<iWidth&&(int)(v+0.49)>0&&(int)(v+0.49)<iHeight)
					 pFrm->m_pColorImg->at((int)(u+0.49),(int)(v+0.49))=255;
					 

			   }
*/

			  // fclose(pFile1);
			  // fclose(pFile2);

			}
		
		}
		int iFrmIdx = TopData.second.second[0];
		//std::cout<<"iFrmIdx:"<<iFrmIdx<<std::endl;
		

		//Q2.pop();
		//Q2.erase(Q2.begin());
		//Q2.remove();

       //不清楚原因，需要查看。
		if(PtReadNumMp[iFrmIdx] < PtNumMp[iFrmIdx])
		{
			std::pair<float, std::pair<Wml::Vector<6, float>, Wml::Vector3<int> > > Data;
			fread(&Data.first, sizeof(float), 1, ReadFileMp[iFrmIdx]);
			fread(&Data.second.first[0], sizeof(float), 6, ReadFileMp[iFrmIdx]);
			Data.second.second[0] = iFrmIdx;
			fread(&Data.second.second[1], sizeof(int), 2, ReadFileMp[iFrmIdx]);
			PtReadNumMp[iFrmIdx]++;
			//Q2.push(Data);
			Q2.add(Data);
		    //std::sort(Q2.begin(),Q2.end(),cmp2);
		}
		//count++;
	  //std::cout<<"count: "<<count<<std::endl;
	}

   // std::cout<<"count: "<<count<<std::endl;
	for(int iFrame = ithFrame; iFrame <=ithFrame; iFrame++)
	{
		for(int camId=0;camId<CamNum;camId++)
		{
			fclose(ReadFileMp[camId]);
			delete PrjOccMap[camId];
		}
		
	}


  for(int iFrame=ithFrame;iFrame<=ithFrame;iFrame++)
  {
	 fclose(pFileArr1[0]);
     fclose(pFileArr2[0]);

  }

  pFileArr1.shrink_to_fit();
  pFileArr2.shrink_to_fit();

	for(int iFrame = ithFrame; iFrame <= ithFrame; iFrame++)
	{
	   for(int camId=0;camId<CamNum;camId++)
	   {
		LSVideoFrame* pFrm = mMotion->GetFrameAt(camId, iFrame);
		pFrm->Clear_depth();
	   }
	}
 
	

	m_PtNormArr.shrink_to_fit();     //使得capity 适应size（也就是说 capacity = size)
	m_NptsFlPthArr.shrink_to_fit();

	std::cout<<"Point Cloud Sampling Finished!"<<std::endl;

}

// get world coord from image coord
Wml::Vector3d PointCloudSampling::Reproject(LSVideoFrame* pVFrm, Wml::Vector2d p, double z)
{
	Wml::Vector3d P;

	pVFrm->GetWorldCoordFrmImgCoord(p.X(), p.Y(), 1.0/z, P);
	return P;
}




bool PointCloudSampling::VisualhullCheck( Wml::Vector3d& ThreeDV )
{
	/*
	由于这里的边界是hard边界，所以这里当一个点只是在某一张图片中没有投影到Visualhull
	时并不认为它是outlier，
	另外如果一个点投影到任何的图片中都在图片边界之外时也认为这个点是outlier,至少在3张图片的Visualhull中
	*/
	int inVHCnt = 0;
	int outVHCnt = 0;
	for(int iFrame = mStartFrame;iFrame<=mEndFrame;iFrame++)
	{
		//Temp
		//for(int m = 0; m < 2; m++){
		for(int camId = 0; camId< CamNum; camId++)
		{
			LSVideoFrame*  curFrame = mMotion->GetFrameAt(camId, iFrame);

			double NeighU, NeighV, out_dsp;
			curFrame->GetImgCoordFrmWorldCoord(NeighU, NeighV, out_dsp, ThreeDV);

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
	}


	if(outVHCnt>2 ||  inVHCnt <=1)//inVHCnt  is at least one
		return false;
  
	return true;
}


Wml::Vector3d PointCloudSampling::Normal(Wml::Vector2<int> P, const Wml::GMatrix<Wml::Vector3d>& PtMp, LSVideoFrame* pFrm)
{
	Wml::Vector2<int> Ngh[8];
	Ngh[0] = Wml::Vector2<int>(-1, 0);
	Ngh[1] = Wml::Vector2<int>(-1, -1);
	Ngh[2] = Wml::Vector2<int>(0, -1);
	Ngh[3] = Wml::Vector2<int>(1, -1);
	Ngh[4] = Wml::Vector2<int>(1, 0);
	Ngh[5] = Wml::Vector2<int>(1, 1);
	Ngh[6] = Wml::Vector2<int>(0, 1);
	Ngh[7] = Wml::Vector2<int>(-1, 1);

	int ImgWidth = PtMp.GetRows();
	int ImgHeight = PtMp.GetColumns();

	Wml::Vector3d Nrm(0.0, 0.0, 0.0);
	for(int n = 1; n < 8; n += 2)
	{
		Wml::Vector2<int> NghLoc = P + Ngh[n];
		if(NghLoc.X() >= 0 && NghLoc.X() < ImgWidth && NghLoc.Y() >= 0 && NghLoc.Y() < ImgHeight) // && pFrm->IsInMask(NghLoc.X(), NghLoc.Y()))
		{
			int n1 = (n + 7) % 8;
			int n2 = (n + 1) % 8;
			Wml::Vector2<int> NghLoc1 = P + Ngh[n1];
			Wml::Vector2<int> NghLoc2 = P + Ngh[n2];

			Wml::Vector3d P1_P0 = PtMp(NghLoc.X(), NghLoc.Y()) - PtMp(P.X(), P.Y());
			Wml::Vector3d P2_P0 = PtMp(NghLoc1.X(), NghLoc1.Y()) - PtMp(P.X(), P.Y());
			Wml::Vector3d N = P1_P0.Cross(P2_P0);
			N.Normalize();
			Nrm += N;

			P1_P0 = PtMp(NghLoc2.X(), NghLoc2.Y()) - PtMp(P.X(), P.Y());
			P2_P0 = PtMp(NghLoc.X(), NghLoc.Y()) - PtMp(P.X(), P.Y());
			N = P1_P0.Cross(P2_P0);
			N.Normalize();
			Nrm += N;
		}
	}

	Nrm.Normalize();

	return Nrm;
}

std::string PointCloudSampling::GetPtCldFileName( LSVideoFrame* pFrm )
{
	std::string frameName=pFrm->m_sName;
	std::string dirPath=pFrm->s_RawDir[pFrm->m_iCameraId];
	//std::cout<<"dirPath: "<<dirPath<<std::endl;
	return dirPath+frameName + "_PtCld" + ".raw";
}



// 此目录可能有问题

void   PointCloudSampling::SetDir(const std::string& dir)
{

	std::cout<<"SetDir function not used!"<<std::endl;

/*
	m_dir = dir;
	m_MdlFlPth = dir + "Raw\\";

	//std::tstring path;
	//std::string2tstring(m_MdlFlPth, path);
	
	CreateDirectory(m_MdlFlPth.c_str(), 0);
	m_NptsFlPth = m_dir + "model.npts";
	m_PtNorm = m_dir + "model.obj";
	*/
}


