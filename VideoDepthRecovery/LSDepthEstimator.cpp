#include "LSDepthEstimator.h"

#include <numeric>
#include "ImagePartition/LSImgPartition.h"
#include "LSStereoWorkUnit.h"
#include "LSBeliefPropagation.h"
#include "LSSuperRefineWorkUnit.h"

#include "ImgPartiton.h"
#include <utility>

#include "LSRunConfigDlg.h"

#include "LSStructureMotion.h"


LSDepthEstimator::LSDepthEstimator(void)
{
	  m_pOptFlwImg=NULL;
}


LSDepthEstimator::~LSDepthEstimator(void)
{
	
	if(	m_pOptFlwImg != NULL){
		delete m_pOptFlwImg;
		m_pOptFlwImg = NULL;
	}
	
}


/************************************************************/
          /*** Init Depth ***/
/***********************************************************/
void LSDepthEstimator::EstimateDepth(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, DataCost& DataCost)
{
	//parameter instance
	LSParaWidget* paras = LSParaWidget::GetInstance();

	//image width and height
	int ImgWidth = pCurrentFrame->GetImgWidth();     //1024
	int ImgHeight = pCurrentFrame->GetImgHeight();   //1024
	
	//mask size (refer to single frame)
	int minX, minY, maxX, maxY;
	pCurrentFrame->GetMaskRectangle(minX, minY, maxX, maxY); //348,140,647,732
	int MaskWidth = maxX - minX + 1;   //300 (647-348+1)
	int MaskHeight = maxY - minY + 1;  //593 (732-140+1)
	LSImgPartition::GetInstance()->SetImgSize(MaskWidth, MaskHeight); 

	
//	std::cout<<"ImgWidth: "<<ImgWidth<<", ImgHeight:"<<ImgHeight<<std::endl;
//	std::cout<<"MaskWidth: "<<MaskWidth<<", MaskHeight: "<<MaskHeight<<std::endl;
//	std::cout<<"minX:"<<minX<<",maxX:"<<maxX<<std::endl;
//	std::cout<<"minY:"<<minY<<",maxY:"<<maxY<<std::endl;
	
	//create and init
	ZIntImage colorLabelImg, bpLabelImg, maskLabelImg;
	colorLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	bpLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	maskLabelImg.CreateAndInit(MaskWidth, MaskHeight, 1, -1); //width and height are shorter

	//ZIntImage segmLabelImg;                                 //seg add...
	//segmLabelImg.CreateAndInit(ImgWidth,ImgHeight,1,-1);    //seg add...

	//load colorImage
	pCurrentFrame->LoadColorImg();
	for(int i=0; i<FwFrames.size(); i++)
		FwFrames[i]->LoadColorImg();
	for(int i=0; i<BwFrames.size(); i++)
		BwFrames[i]->LoadColorImg();

	//dsparity parameters
    int dspLevel=LSParaWidget::GetInstance()->GetDspLevel();  
	int layerCount = dspLevel - 1;
	float curMaxDsp, curMinDsp;
	float dspStep;
	pCurrentFrame->GetDspRange(curMinDsp, curMaxDsp);
    std::cout<<"curMinDsp: "<<curMinDsp<<";"<<"curMaxDsp: "<<curMaxDsp<<std::endl;
 
	const float dspSeg = ( curMaxDsp - curMinDsp ) / layerCount;
	std::vector<float> dspV(layerCount + 1);
	for(int i = 0; i <= layerCount; i++)
		dspV[i] = curMinDsp * (layerCount - i) / layerCount + curMaxDsp * i / layerCount;
//	MeanShiftSeg meanShiftSegm;                                  //seg add
//	int sigmaS = LSParaWidget::GetInstance()->m_iSegSpatial;     //seg add
//	float sigmaR = LSParaWidget::GetInstance()->m_fSegColor;     //seg add
//	int minSize = LSParaWidget::GetInstance()->m_iSegMinsize;    //seg add
	//get segment images and save
//	meanShiftSegm.Segment(*pCurrentFrame->m_pColorImg, sigmaS, sigmaR, minSize,  pCurrentFrame->GetSegmSavePath(pCurrentFrame));   //seg add

	std::vector<ZFloatImage*> matchCostCube;
	matchCostCube.resize(dspLevel);

	// compute according block
	int blockCountX = LSImgPartition::GetInstance()->GetBlockCountX();  //2
	int blockCountY = LSImgPartition::GetInstance()->GetBlockCountY();  //1
	for(int blockIdy = 0; blockIdy < blockCountY; blockIdy++)
	{
		for(int blockIdX = 0; blockIdX < blockCountX; blockIdX++)
		{
			int offsetX, offsetY;  // mask start position in the whole image
			int blockWidth, blockHeight;
			int trueX1, trueY1, trueX2, trueY2;  
			LSImgPartition::GetInstance()->SetBlockId(blockIdX, blockIdy);
			
			//--  get info for each block --//
			///(trueX1, trueY1, trueX2, trueY2 represent the start-leftTop and end-rightBottom coord of the block coord in the whole image)
			LSImgPartition::GetInstance()->GetBlockInfoSimple(trueX1, trueY1, trueX2, trueY2, offsetX, offsetY, blockWidth, blockHeight);
          

			std::cout<<"trueX1:"<<trueX1<<",trueY1:"<<trueY1<<",trueX2:"<<trueX2<<",trueY2:"<<trueY2<<std::endl;
		//	std::cout<<"offsetX: "<<offsetX<<",offsetY: "<<offsetY<<std::endl;
		    std::cout<<"blockWidth:"<<blockWidth<<",blockHeight"<<blockHeight<<std::endl;
		//	std::cout<<"---------------------------------------------------------------"<<std::endl;
			SetBlockState(trueX1 + minX, trueY1 + minY, trueX2 + minX, trueY2 + minY, offsetX + minX, offsetY + minY, blockWidth, blockHeight, blockIdX, blockIdy);
		  
		     std::cout<<"offsetX: "<<offsetX<<",offsetY: "<<offsetY<<std::endl;
			std::cout<<"minX:"<<minX<<",minY:"<<minY<<std::endl;
		//	std::cout<<"offsetX+minX:"<<offsetX+minX<<",offsetY+minY"<<offsetY+minY<<std::endl;


			PrintBlockState();

			DataCost.Create(m_iBlockWidth,m_iBlockHeight,dspLevel);  //Instead
           
			
			GetInitialDataCost(FwFrames, BwFrames, pCurrentFrame, DataCost, dspV, colorLabelImg);    // get dataCost and colorLabelImage
			// output init datacost result
			//  ORG
			//if(paras->GetIfGenerateTempResult() == true)
			//	pCurrentFrame->SaveLabelImg(colorLabelImg, LSVideoFrame::DATACOST_Init, dspLevel);
           
   

			if(paras->GetIfGenerateTempResult() == true){
				for(int y = trueY1; y <= trueY2; y++){
					for(int x = trueX1; x <= trueX2; x++){
						if(pCurrentFrame->IsInMask(x + minX, y + minY) == false)
							colorLabelImg.at(x + minX, y + minY) = 0;
					}
				}
				pCurrentFrame->SaveLabelImg(colorLabelImg, LSVideoFrame::DATACOST_Init, dspLevel);
			}
			
			//ZIntImage testImg;
			//testImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
			//CBoxFilter boxFilter(blockWidth, blockHeight, minX + trueX1, minY + trueY1, minX + trueX2, minY + trueY2, 
			//	minX + offsetX, minY + offsetY, CBoxFilter::eBorderReplicate, CSystemParameters::GetInstance()->GetCpuThreads()); //CSystemParameters::GetInstance()->GetCpuThreads());
			//boxFilter.BoxFilter(testImg, DataCost, 1, true);
			//pCurrentFrame->SaveLabelImg(testImg, CBDMVideoFrame::INIT, dspLevel, true);

			//pCurrentFrame->SaveDataCost(DataCost, m_iBlockWidth * m_iBlockHeight * dspLevel * sizeof(float));
			//pCurrentFrame->ReadDataCost(DataCost, labelImg, m_iOffsetX, m_iOffsetY, m_iBlockWidth, m_iBlockHeight );
			///if(paras->GetIfGenerateTempResult() == true)
			///	pCurrentFrame->SaveLabelImg(colorLabelImg, CBDMVideoFrame::DATACOST, dspLevel);

			std::cout<<std::endl;


//NEW ADD
	//KXL-Version BP
			std::cout<<"Init By BP:"<<std::endl;
		   int apronedTileStartX,apronedTileStartY,apronedTileEndX,apronedTileEndY;
			apronedTileStartX=minX+trueX1;
			apronedTileStartY=minY+trueY1;
			apronedTileEndX=minX+trueX2;
			apronedTileEndY=minY+trueY2;

	    //	std::cout<<"apronedTileStartX:"<<apronedTileStartX<<",apronedTileStartY:"<<apronedTileStartY<<"apronedTileEndX:"<<apronedTileEndX<<"apronedTileEndY:"<<apronedTileEndY<<std::endl

//首先想办法将DataCost中存储的值复制到MatchCostCube（<vector>ZFloatIamge形式),以便直接调用KXL系统BP中的很多模块代码。
//另外将DataCost中的值移植以后，就可以释放掉其内存了。(DataCost值暂时未释放）
			
		  for(int i=0; i<matchCostCube.size(); ++i)
				matchCostCube[i] = new ZFloatImage(m_iBlockWidth,m_iBlockHeight);


		   CopyValueFromDataCostToMatchCostCube(DataCost,matchCostCube);

			const bool gaussianFilter = true;
			ZIntImage tmpLabelImg;
			TileOptimizeGeneral(apronedTileStartX,apronedTileStartY,apronedTileEndX,apronedTileEndY,
				matchCostCube,tmpLabelImg,gaussianFilter);
        

			for(int y = trueY1; y <= trueY2; y++)
			{
				for(int x = trueX1; x <= trueX2; x++)
				{
					// if(pCurrentFrame->IsInMask(x + minX, y + minY) == true) //之前未注销(此行代码严重影响对遮挡的处理）
				     bpLabelImg.at(x + minX, y + minY) = tmpLabelImg.GetPixel(x, y, 0);//虽然每帧图像为1024*1024，计算时仅用mask的size大小区域计算
					//std::cout<<maskLabelImg.GetPixel(x,y,0)<<std::endl;
				}
				//std::cout<<std::endl;
			}
			//std::cout<<"m_iTrueX1: "<< m_iTrueX1<<" , "<<"m_iTrueY1: "<<m_iTrueY1<<" , "<<"m_iTrueX2: "<< m_iTrueX2<<" , "<<"m_iTrueY2: "<<m_iTrueY2<<std::endl;
		  pCurrentFrame->SetDspImg(bpLabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);
		  
		  tmpLabelImg.Clear();
		  
			}
	}
    
		  if(LSParaWidget::GetInstance()->GetIfGenerateTempResult() == true)
		  {
			  pCurrentFrame->SaveLabelImg(bpLabelImg, LSVideoFrame::INIT, dspLevel);
			  //pCurrentFrame->SaveLabelImg(segmLabelImg,LSVideoFrame::INIT_SEG,dspLevel);
		  }

		  pCurrentFrame->SaveModel(true);
		  pCurrentFrame->SaveDspImg();

/*ORG 暂注
// LQL-Version BP  
			std::cout<<"Init By BP:"<<std::endl;
			RefineDspByBP(DataCost, maskLabelImg, true);
			//copy pixel in maskLabelImg to bpLabelImg;
			for(int y = trueY1; y <= trueY2; y++)
			{
				for(int x = trueX1; x <= trueX2; x++)
				{
					if(pCurrentFrame->IsInMask(x + minX, y + minY) == true)
						bpLabelImg.at(x + minX, y + minY) = maskLabelImg.GetPixel(x, y, 0);//虽然每帧图像为1024*1024，计算时仅用mask的size大小区域计算
   
				//	    segmLabelImg.at(x+minX,y+minY)=maskLabelImg.GetPixel(x,y,0);   // seg add
					//std::cout<<maskLabelImg.GetPixel(x,y,0)<<std::endl;
				}
				//std::cout<<std::endl;
			}

            //may have bug
		    std::cout<<"m_iTrueX1: "<< m_iTrueX1<<" , "<<"m_iTrueY1: "<<m_iTrueY1<<" , "<<"m_iTrueX2: "<< m_iTrueX2<<" , "<<"m_iTrueY2: "<<m_iTrueY2<<std::endl;
	        pCurrentFrame->SetDspImg(bpLabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);
			//instance the block range
		//	Block block(m_iTrueX1,m_iTrueY1,blockWidth,blockHeight);   //吃内存！！！

	//1.3  refine dsp by mean shift segement
		//	RefineDspBySegm(block,meanShiftSegm,pCurrentFrame,segmLabelImg,DataCost);     //吃内存（regions)!!!			
		//	pCurrentFrame->SetDspImg(segmLabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);   //new add(test result)

		}
	}
	


	if(LSParaWidget::GetInstance()->GetIfGenerateTempResult() == true)
	{
		
		pCurrentFrame->SaveLabelImg(bpLabelImg, LSVideoFrame::INIT, dspLevel);
	   //pCurrentFrame->SaveLabelImg(segmLabelImg,LSVideoFrame::INIT_SEG,dspLevel);
		
	}



// save model 和Init DE都只有一半，bug应该出在block上
     pCurrentFrame->SaveModel(true);

	 pCurrentFrame->SaveDspImg();
	//注新 _SuperRefine(FwFrames, BwFrames, pCurrentFrame, 1, LSVideoFrame::INIT_DE);

	//pCurrentFrame->SaveDspImg();
	//for(int i=0; i<FwFrames.size(); i++)
	//	FwFrames[i]->Clear();
	//for(int i=0; i<BwFrames.size(); i++)
	//	BwFrames[i]->Clear();
	//pCurrentFrame->Clear();

*/
	colorLabelImg.Clear();
	bpLabelImg.Clear();
	maskLabelImg.Clear();
//	segmLabelImg.Clear();  //seg add
}

void LSDepthEstimator::BundleOptimization( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames,LSVideoFrame* pCurrentFrame, DataCost& DataCost ){ 
	//Test
	//pCurrentFrame->LoadDepthImg();
	//pCurrentFrame->SaveModel(true);
	//pCurrentFrame->Clear();
	//system("pause");
	//return;

	    // BO Steps coding by Yu Liu, 2014-11-25
	    //1.Set parameters (image width & height, mask size and coordinate, create and initiate images, disparity level, min and max disparity).
		//2.Load images (current, forward and backward frames).
		//3.Get BO dataCost.
		//4.Refine disparity by BP. ( save result as BO)
		//5._SuperRefine. ( equal to DE)

	// image width and height
	LSParaWidget* paras = LSParaWidget::GetInstance();
	int ImgWidth = pCurrentFrame->GetImgWidth();
	int ImgHeight = pCurrentFrame->GetImgHeight();
	
	// mask info( size and coord)
	int minX, minY, maxX, maxY;
	pCurrentFrame->GetMaskRectangle(minX, minY, maxX, maxY);
	int MaskWidth = maxX - minX + 1;
	int MaskHeight = maxY - minY + 1;
	LSImgPartition::GetInstance()->SetImgSize(MaskWidth, MaskHeight);


	//create and init
	ZIntImage colorLabelImg, bpLabelImg, maskLabelImg;
	colorLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	bpLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	maskLabelImg.CreateAndInit(MaskWidth, MaskHeight, 1, -1);  

	//put forward frames and backward frames together.
	std::vector<LSVideoFrame*> nearFrames(FwFrames.begin(), FwFrames.end());
	nearFrames.insert(nearFrames.end(), BwFrames.begin(), BwFrames.end());

	//load images(color,depth,mask)
	pCurrentFrame->LoadColorImg();
	pCurrentFrame->LoadDepthImg();
	pCurrentFrame->LoadMaskImg();

	for(int i=0; i<FwFrames.size(); i++)
	{
		FwFrames[i]->LoadColorImg();
		FwFrames[i]->LoadDepthImg();
		FwFrames[i]->LoadMaskImg();
	}
	for(int i=0; i<BwFrames.size(); i++){
		BwFrames[i]->LoadColorImg();
		BwFrames[i]->LoadDepthImg();
		BwFrames[i]->LoadMaskImg();
	}

    // disparity set up( different levels)  
//	int dspLevel = DataCost.GetDspLevels();   //Old

	int dspLevel=LSParaWidget::GetInstance()->GetDspLevel();

	int layerCount = dspLevel - 1;
	float curMaxDsp, curMinDsp;
	float dspStep;
	pCurrentFrame->GetDspRange(curMinDsp, curMaxDsp);
   
	std::cout<<"curMaxDsp1: "<<curMaxDsp<<",curMinDsp1: "<<curMinDsp<<std::endl;

	const float dspSeg = ( curMaxDsp - curMinDsp ) / layerCount;

	std::vector<float> dspV(layerCount + 1);
	for(int i = 0; i <= layerCount; i++)
		dspV[i] = curMinDsp * (layerCount - i) / layerCount + curMaxDsp * i / layerCount;

	//NEW ADD: assign label according to the depth image from init.
 
	for(int y = 0;y<ImgHeight;y++)
		for(int x = 0;x<ImgWidth;x++)
		{
			float dsp = pCurrentFrame->GetDspAt(x,y);
			int label = (dsp - curMinDsp) / (curMaxDsp - curMinDsp) * layerCount;
			label = min( label, layerCount);
			label = max( label,0 );
			bpLabelImg.at(x,y) = label;
		}
   

	//debug
	//pCurrentFrame->InitLabelImgByDspImg(colorLabelImg, dspLevel);
	//pCurrentFrame->SaveLabelImg(colorLabelImg, CBDMVideoFrame::INIT, dspLevel);


	std::vector<ZFloatImage*> matchCostCube;
	matchCostCube.resize(dspLevel);


	int blockCountX = LSImgPartition::GetInstance()->GetBlockCountX();
	int blockCountY = LSImgPartition::GetInstance()->GetBlockCountY();
	for(int blockIdy = 0; blockIdy < blockCountY; blockIdy++){
		for(int blockIdX = 0; blockIdX < blockCountX; blockIdX++){
			int offsetX, offsetY;
			int blockWidth, blockHeight;
			int trueX1, trueY1, trueX2, trueY2;
		    LSImgPartition::GetInstance()->SetBlockId(blockIdX, blockIdy);
			LSImgPartition::GetInstance()->GetBlockInfoSimple(trueX1, trueY1, trueX2, trueY2, offsetX, offsetY, blockWidth, blockHeight);
		//trueX1, trueY1, trueX2, trueY2 is the Block true coord not contain offset 
		
			SetBlockState(trueX1 + minX, trueY1 + minY, trueX2 + minX, trueY2 + minY, offsetX + minX, offsetY + minY, blockWidth, blockHeight, blockIdX, blockIdy);

		//	clock_t tempTime = clock();
			printf("BO Part:Init DataCost:\n");
			PrintBlockState();

			DataCost.Create(m_iBlockWidth,m_iBlockHeight,dspLevel);       //Instead

			//ZDoubleImage dtCstLabelImg;
			//dtCstLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, 0);
			GetBODataCost(FwFrames, BwFrames, pCurrentFrame, DataCost, dspV, colorLabelImg);
			//pCurrentFrame->SaveDataCost(DataCost, m_iBlockWidth * m_iBlockHeight * dspLevel * sizeof(float));
			//pCurrentFrame->ReadDataCost(DataCost, labelImg, m_iOffsetX, m_iOffsetY, m_iBlockWidth, m_iBlockHeight );
			if(paras->GetIfGenerateTempResult() == true){
				for(int y = trueY1; y <= trueY2; y++){
					for(int x = trueX1; x <= trueX2; x++){
						if(pCurrentFrame->IsInMask(x + minX, y + minY) == false)
							colorLabelImg.at(x + minX, y + minY) = -1;
					}
				}
				pCurrentFrame->SaveLabelImg(colorLabelImg, LSVideoFrame::DATACOST_BO, dspLevel);
			}

		//	printf("\nTime:%.3f s\n",(double)(clock()-tempTime)/CLOCKS_PER_SEC);
		//	tempTime = clock();


  
			//NEW ADD
			//KXL-Version BP
			std::cout<<"BO Part: Init By BP:"<<std::endl;
			int apronedTileStartX,apronedTileStartY,apronedTileEndX,apronedTileEndY;
			apronedTileStartX=minX+trueX1;
			apronedTileStartY=minY+trueY1;
			apronedTileEndX=minX+trueX2;
			apronedTileEndY=minY+trueY2;
		//	std::cout<<"apronedTileStartX:"<<apronedTileStartX<<",apronedTileStartY:"<<apronedTileStartY<<"apronedTileEndX:"<<apronedTileEndX<<"apronedTileEndY:"<<apronedTileEndY<<std::endl
			//首先想办法将DataCost中存储的值复制到MatchCostCube（<vector>ZFloatIamge形式),以便直接调用KXL系统BP中的很多模块代码。
			//另外将DataCost中的值移植以后，就可以释放掉其内存了。(DataCost值暂时未释放）

			for(int i=0; i<matchCostCube.size(); ++i)
				matchCostCube[i] = new ZFloatImage(m_iBlockWidth,m_iBlockHeight);


			CopyValueFromDataCostToMatchCostCube(DataCost,matchCostCube);

			const bool gaussianFilter = true;
			ZIntImage tmpLabelImg;
			TileOptimizeGeneral(apronedTileStartX,apronedTileStartY,apronedTileEndX,apronedTileEndY,
				matchCostCube,tmpLabelImg,gaussianFilter);


			for(int y = trueY1; y <= trueY2; y++)
			{
				for(int x = trueX1; x <= trueX2; x++)
				{
					// if(pCurrentFrame->IsInMask(x + minX, y + minY) == true) //之前未注销
					bpLabelImg.at(x + minX, y + minY) = tmpLabelImg.GetPixel(x, y, 0);//虽然每帧图像为1024*1024，计算时仅用mask的size大小区域计算
					//std::cout<<maskLabelImg.GetPixel(x,y,0)<<std::endl;
				}
				//std::cout<<std::endl;
			}
			//std::cout<<"m_iTrueX1: "<< m_iTrueX1<<" , "<<"m_iTrueY1: "<<m_iTrueY1<<" , "<<"m_iTrueX2: "<< m_iTrueX2<<" , "<<"m_iTrueY2: "<<m_iTrueY2<<std::endl;
			pCurrentFrame->SetDspImg(bpLabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);

			tmpLabelImg.Clear();

/*
         //ORG 暂注
		 //LQL-Version BP
			printf("Init By BP:\n");
			RefineDspByBP(DataCost, maskLabelImg, true);
			for(int y = trueY1; y <= trueY2; y++){
				for(int x = trueX1; x <= trueX2; x++){
					//if(pCurrentFrame->IsInMask(x + minX, y + minY) == true)   //之前未注释，对遮挡影响较大
						bpLabelImg.at(x + minX, y + minY) = maskLabelImg.GetPixel(x, y, 0);
				}
			}
			if(paras->GetIfGenerateTempResult() == true)
				pCurrentFrame->SaveLabelImg(bpLabelImg, LSVideoFrame::BO, dspLevel);

		//	printf("Time:%.3f s\n",(double)(clock()-tempTime)/CLOCKS_PER_SEC);
		//	tempTime = clock();

			pCurrentFrame->SetDspImg(bpLabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);
*/	

		}
	}

	if(paras->GetIfGenerateTempResult() == true)
		pCurrentFrame->SaveLabelImg(bpLabelImg, LSVideoFrame::BO, dspLevel);

	pCurrentFrame->SaveModel(true);
	pCurrentFrame->SaveDspImg();
   
	_SuperRefine(FwFrames, BwFrames, pCurrentFrame, 1, LSVideoFrame::BO_DE,bpLabelImg);   //之前为迭代2次

 	//pCurrentFrame->SaveDspImg();
	//for(int i=0; i<nearFrames.size(); i++)
	//nearFrames[i]->Clear();
	//pCurrentFrame->Clear();
}


// add in--optical flow calculation for temporal consistence
void LSDepthEstimator::OpticalFlowOptimization( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames,LSVideoFrame* pCurrentFrame, DataCost& DataCost ){


	// image width and height
	LSParaWidget* paras = LSParaWidget::GetInstance();
	int ImgWidth = pCurrentFrame->GetImgWidth();
	int ImgHeight = pCurrentFrame->GetImgHeight();

	// mask info( size and coord)
	int minX, minY, maxX, maxY;
	pCurrentFrame->GetMaskRectangle(minX, minY, maxX, maxY);
	int MaskWidth = maxX - minX + 1;
	int MaskHeight = maxY - minY + 1;
	LSImgPartition::GetInstance()->SetImgSize(MaskWidth, MaskHeight);


	//create and init
	ZIntImage colorLabelImg, bpLabelImg, maskLabelImg;
	colorLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	bpLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	maskLabelImg.CreateAndInit(MaskWidth, MaskHeight, 1, -1);  // not used until then

	std::vector<LSVideoFrame*> nearFrames(FwFrames.begin(), FwFrames.end());
	nearFrames.insert(nearFrames.end(), BwFrames.begin(), BwFrames.end());

	//load
	pCurrentFrame->LoadColorImg();
	pCurrentFrame->LoadDepthImg();
	pCurrentFrame->LoadMaskImg();


    int startFrameId=LSRunConfigDlg::GetInstance()->GetStartFrame();
	int endFrameId=LSRunConfigDlg::GetInstance()->GetEndFrame();

   // LSStructureMotion::GetInstance()->GetFrameAt(0,1)->LoadBackOptFlwImg(); //new add

	if((pCurrentFrame->m_iFrameId)>=startFrameId&&(pCurrentFrame->m_iFrameId)<endFrameId)
    pCurrentFrame->LoadForeOptFlwImg();   //new add-for optical flow
    
	if((pCurrentFrame->m_iFrameId)>startFrameId&&(pCurrentFrame->m_iFrameId)<=endFrameId)
	pCurrentFrame->LoadBackOptFlwImg();



	//pCurrentFrame->LoadOptFlwImg();// 加载optical flow image 需要至少连续两帧方可

	for(int i=0; i<FwFrames.size(); i++)
	{
		FwFrames[i]->LoadColorImg();
		FwFrames[i]->LoadDepthImg();
		FwFrames[i]->LoadMaskImg();

		if((FwFrames[i]->m_iFrameId)>=startFrameId&&(FwFrames[i]->m_iFrameId)<endFrameId)
		{
			FwFrames[i]->LoadForeOptFlwImg();
	      // std::cout<<"FW: "<<FwFrames[i]->m_iCameraId<<","<<FwFrames[i]->m_iFrameId<<std::endl;
		}

		if((FwFrames[i]->m_iFrameId)>startFrameId&&(FwFrames[i]->m_iFrameId)<=endFrameId)
		{
			FwFrames[i]->LoadBackOptFlwImg();
			//std::cout<<"FW: "<<FwFrames[i]->m_iCameraId<<","<<FwFrames[i]->m_iFrameId<<std::endl;
		} 
	}

	for(int i=0; i<BwFrames.size(); i++){
		BwFrames[i]->LoadColorImg();
		BwFrames[i]->LoadDepthImg();
		BwFrames[i]->LoadMaskImg();

		if((BwFrames[i]->m_iFrameId)>=startFrameId&&(BwFrames[i]->m_iFrameId)<endFrameId)
		{
			BwFrames[i]->LoadForeOptFlwImg();
			//std::cout<<"BW: "<<BwFrames[i]->m_iCameraId<<","<<BwFrames[i]->m_iFrameId<<std::endl;
		}
    
		if((BwFrames[i]->m_iFrameId)>startFrameId&&(BwFrames[i]->m_iFrameId)<=endFrameId)
		{
			BwFrames[i]->LoadBackOptFlwImg();
		    //std::cout<<"BW: "<<BwFrames[i]->m_iCameraId<<","<<BwFrames[i]->m_iFrameId<<std::endl;
		} 
	}


	// disparity set up( different levels)  //
	int dspLevel = DataCost.GetDspLevels();   //101
	int layerCount = dspLevel - 1;
	float curMaxDsp, curMinDsp;
	float dspStep;
	pCurrentFrame->GetDspRange(curMinDsp, curMaxDsp);
	const float dspSeg = ( curMaxDsp - curMinDsp ) / layerCount;

	std::vector<float> dspV(layerCount + 1);
	for(int i = 0; i <= layerCount; i++)
		dspV[i] = curMinDsp * (layerCount - i) / layerCount + curMaxDsp * i / layerCount;

	//debug
	//pCurrentFrame->InitLabelImgByDspImg(colorLabelImg, dspLevel);
	//pCurrentFrame->SaveLabelImg(colorLabelImg, CBDMVideoFrame::INIT, dspLevel);

	int blockCountX = LSImgPartition::GetInstance()->GetBlockCountX();
	int blockCountY = LSImgPartition::GetInstance()->GetBlockCountY();
	for(int blockIdy = 0; blockIdy < blockCountY; blockIdy++)
	{
		for(int blockIdX = 0; blockIdX < blockCountX; blockIdX++)
		{
			int offsetX, offsetY;
			int blockWidth, blockHeight;
			int trueX1, trueY1, trueX2, trueY2;
			LSImgPartition::GetInstance()->SetBlockId(blockIdX, blockIdy);
			LSImgPartition::GetInstance()->GetBlockInfoSimple(trueX1, trueY1, trueX2, trueY2, offsetX, offsetY, blockWidth, blockHeight);
			//trueX1, trueY1, trueX2, trueY2 is the Block true coord not contain offset 

			SetBlockState(trueX1 + minX, trueY1 + minY, trueX2 + minX, trueY2 + minY, offsetX + minX, offsetY + minY, blockWidth, blockHeight, blockIdX, blockIdy);

			//	clock_t tempTime = clock();
			std::cout<<"Init OptDataCost:"<<std::endl;

			PrintBlockState();


			GetOptDataCost(FwFrames, BwFrames, pCurrentFrame, DataCost, dspV, colorLabelImg);


			//pCurrentFrame->SaveDataCost(DataCost, m_iBlockWidth * m_iBlockHeight * dspLevel * sizeof(float));
			//pCurrentFrame->ReadDataCost(DataCost, labelImg, m_iOffsetX, m_iOffsetY, m_iBlockWidth, m_iBlockHeight );
			if(paras->GetIfGenerateTempResult() == true)
			{
				for(int y = trueY1; y <= trueY2; y++)
				{
					for(int x = trueX1; x <= trueX2; x++)
					{
						if(pCurrentFrame->IsInMask(x + minX, y + minY) == false)
							colorLabelImg.at(x + minX, y + minY) = -1;
					}
				}
				pCurrentFrame->SaveLabelImg(colorLabelImg, LSVideoFrame::DATACOST_OPT, dspLevel);
			}

			//	printf("\nTime:%.3f s\n",(double)(clock()-tempTime)/CLOCKS_PER_SEC);
			//	tempTime = clock();

			std::cout<<"Init By Opt:"<<std::endl;

			RefineDspByBP(DataCost, maskLabelImg, true);

			for(int y = trueY1; y <= trueY2; y++){
				for(int x = trueX1; x <= trueX2; x++){
					if(pCurrentFrame->IsInMask(x + minX, y + minY) == true)
						bpLabelImg.at(x + minX, y + minY) = maskLabelImg.GetPixel(x, y, 0);
				}
			}
			if(paras->GetIfGenerateTempResult() == true)
				pCurrentFrame->SaveLabelImg(bpLabelImg, LSVideoFrame::OPT, dspLevel);
			//	printf("Time:%.3f s\n",(double)(clock()-tempTime)/CLOCKS_PER_SEC);
			//	tempTime = clock();

			pCurrentFrame->SetDspImg(bpLabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);

		
		}
	}

	pCurrentFrame->SaveDspImg();
  


	//pCurrentFrame->SaveModel(false);

	//暂注_SuperRefine(FwFrames, BwFrames, pCurrentFrame, 1, LSVideoFrame::OPT_DE);

	//if(CSystemParameters::GetInstance()->GetIfGenerateTempResult() == true){
	//	//pCurrentFrame->InitLabelImgByDspImg(labelImg, dspLevel);
	//	pCurrentFrame->SaveLabelImg(bpLabelImg, CBDMVideoFrame::BO, dspLevel);
	//}

	//pCurrentFrame->SaveDspImg();
	//for(int i=0; i<nearFrames.size(); i++)
	//	nearFrames[i]->Clear();
	//pCurrentFrame->Clear();

}



void LSDepthEstimator::SuperRefine( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame ){

	std::vector<LSVideoFrame*> referenceFrames(FwFrames.begin(), FwFrames.end());
	referenceFrames.insert(referenceFrames.end(), BwFrames.begin(), BwFrames.end());

	//load
	pCurrentFrame->LoadColorImg();
	pCurrentFrame->LoadDepthImg();
	pCurrentFrame->LoadMaskImg();
	for(int i=0; i<FwFrames.size(); i++)
	{
		FwFrames[i]->LoadColorImg();
		FwFrames[i]->LoadDepthImg();
		FwFrames[i]->LoadMaskImg();
	}
	for(int i=0; i<BwFrames.size(); i++){
		BwFrames[i]->LoadColorImg();
		BwFrames[i]->LoadDepthImg();
		BwFrames[i]->LoadMaskImg();
	}

//暂注	_SuperRefine(FwFrames, BwFrames, pCurrentFrame, LSParaWidget::GetInstance()->GetSubIter(), LSVideoFrame::DE);
}


// 按照KXL代码改写的InitDataCostAt 函数
void LSDepthEstimator::GetInitialDataCostAt(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, 
	LSVideoFrame* pCurrentFrame, int x, int y, DataCostUnit& dataCosti, int& bestLabel, std::vector<float>& dspV, bool ConsiderDspRang ){
		LSParaWidget* paras = LSParaWidget::GetInstance();

		//float fColorSigma = paras->GetColorSigma();
		float fColorSigma = 5;  //KXL代码中colorSigma在Init和BO中是不同的。
		float fColorMissPenalty = paras->GetMissPenalty();
		float dataCostWeight = paras->GetTrueDataCostWeight();
		float fDiffTruncValue=paras->GetDiffTrunc();

		int dspLevelCount = dspV.size();

		bestLabel = 0;

		int minLabel = 0, maxLabel = dspLevelCount - 1;
		
		if(ConsiderDspRang == true){
			minLabel = pCurrentFrame->m_pMinLabelImg->at(x, y);   //x=0,y=0;   
			maxLabel = pCurrentFrame->m_pMaxLabelImg->at(x,y); 
		  //	 std::cout<<"minLabel:"<<minLabel<<" ,maxLabel: "<<maxLabel<<std::endl;	            //200   有bug
		}

		//std::cout<<"x:" <<x<<",y:"<<y<<std::endl;
		//std::cout<<"minLabel:"<<minLabel<<",maxLabel:"<<maxLabel<<std::endl;
        //std::cout<<"fColorSigma:"<<fColorSigma<<",fColorMissPenalty:"<<fColorMissPenalty<<",fDiffTruncValue:"<<fDiffTruncValue<<std::endl;
		//add reference frames in
		std::vector<LSVideoFrame*> NearFrames(FwFrames.begin(), FwFrames.end());
		NearFrames.insert(NearFrames.end(), BwFrames.begin(), BwFrames.end());
		int refFrameCount = NearFrames.size();

		Wml::Vector3d ptWorldCoord;
		Wml::Vector3f CurrentColor, CorrespondingColor;
		double u2, v2, dsp;

		pCurrentFrame->GetColorAt(x, y, CurrentColor);
	
		//assign value for two sides
		for(int iLayerIndex = 0; iLayerIndex < minLabel; iLayerIndex++)
			dataCosti[iLayerIndex] = fColorSigma/(fDiffTruncValue+fColorSigma);
		for(int iLayerIndex = maxLabel+1; iLayerIndex < dspLevelCount; iLayerIndex++) 
		{
			dataCosti[iLayerIndex] = fColorSigma/(fDiffTruncValue+fColorSigma);

		}

		float MaxLikelihood = 1.0e-3f;
		std::vector<float>  matchCostCandinate(refFrameCount);
		int halfNeighberSize = refFrameCount / 2;
		halfNeighberSize = min( max(halfNeighberSize,2), refFrameCount);

	//	std::cout<<dspLevelCount<<std::endl;
	//	std::cout<<",color[0]:"<<CurrentColor[0]<<",color[1]:"<<CurrentColor[1]<<",color[2]:"<<CorrespondingColor[2]<<std::endl;
	//	for(int i=0;i<dspV.size();i++)
	//		std::cout<<dataCosti[i]<<" ";
	//	std::cout<<std::endl;
	//	std::cout<<"halfNeighberSize:"<<halfNeighberSize<<std::endl;

		for(int depthLeveli = minLabel; depthLeveli <=min((int)maxLabel,(int)dspV.size()-1) ;(int)depthLeveli++)
		{
			dataCosti[depthLeveli]=0;

			pCurrentFrame->GetWorldCoordFrmImgCoord(x, y, dspV[depthLeveli], ptWorldCoord);

			//z = 1.0/dspV[depthLeveli];
			for(int Framei=0; Framei<refFrameCount; Framei++)
			{
				NearFrames[Framei]->GetImgCoordFrmWorldCoord(u2, v2, dsp, ptWorldCoord);

			//	std::cout<<"u2:"<<u2<<",v2:"<<v2<<",z2:"<<1/dsp<<std::endl;
			//	std::cout<<"neighberIdx:"<<Framei<<std::endl;
				//out of range
				float evl=1e-4f;
//may try this if(u2<0 || v2<0 || u2> NearFrames[Framei]->GetImgWidth()-1 || v2 > NearFrames[Framei]->GetImgHeight()-1||NearFrames[Framei]->IsInMask(u2, v2) == false) 
				if(u2<0 || v2<0 || u2>=NearFrames[Framei]->GetImgWidth()-1 || v2 >=NearFrames[Framei]->GetImgHeight()-1)
					evl = fColorSigma / (fColorSigma + fColorMissPenalty);

				else
				{
					NearFrames[Framei]->GetColorAt((float)u2, (float)v2, CorrespondingColor);

					float colordist = (fabs(CurrentColor[0] - CorrespondingColor[0]) 
						+ fabs(CurrentColor[1] - CorrespondingColor[1]) 
						+ fabs(CurrentColor[2] - CorrespondingColor[2])) / 3.0;

					colordist=min(fDiffTruncValue,colordist);
					evl = fColorSigma / (fColorSigma + colordist);
				}
				matchCostCandinate[Framei]=evl;

				// std::cout<<"matchCostCandinate[neighberIdx]:"<<matchCostCandinate[Framei]<<" ";
			}
			//std::cout<<std::endl;

		
			//dataCosti[depthLeveli] = GetValue(matchCostCandinate, FwFrames.size(), BwFrames.size());

			//if(dataCosti[depthLeveli] > MaxLikelihood)
			//{
			//	MaxLikelihood = dataCosti[depthLeveli];

			//	//bestLabel = depthLeveli; // ORG

			//}

		
	    if(matchCostCandinate.size()>halfNeighberSize)
				std::nth_element(matchCostCandinate.begin(),
				matchCostCandinate.begin() + halfNeighberSize,
				matchCostCandinate.end(),std::greater<float>() );  //按从大到小递减排序
			float sum = 0.0;
			for(int iCandidate = 0;iCandidate<halfNeighberSize;++iCandidate)
				sum+=matchCostCandinate.at(iCandidate);
			dataCosti[depthLeveli] = sum;

			//取最大相似度  ORG
			//if(dataCosti[depthLeveli] > MaxLikelihood)
			//{
			//	MaxLikelihood = dataCosti[depthLeveli];
			//	//bestLabel = depthLeveli;   // ORG
			//	// std::cout<<"bestLabel: "<<bestLabel<<std::endl;	

			//}
		//	std::cout<<"matchCost[iLayerIndex]:"<<dataCosti[depthLeveli]<<std::endl;
			

		}

		const float smallVariance = 16;
		if( pCurrentFrame->m_pVarianceMap->at(x,y)<smallVariance )
			for(int iLayerIndex = maxLabel;iLayerIndex>=max( minLabel,maxLabel - 3);iLayerIndex--)
				dataCosti[ iLayerIndex ] *= 1.04f;


	/*	std::cout<<"maxLabel:"<<maxLabel<<",minLabel:"<<minLabel<<std::endl;

		for(int i=0;i<dspV.size();i++)
		{
			std::cout<<dataCosti[i]<<" ";
		}

		std::cout<<std::endl;*/


//NEW ADD
/*
	const float minVal = 1e-3f;
	//取得最大的相似度
	float normScale = minVal;
	for(int iLayerIndex=0; iLayerIndex<dspLevelCount; ++iLayerIndex){	
        
		normScale =max(normScale,dataCosti[iLayerIndex]);
		//if(matchCost[iLayerIndex]>normScale)
		//{
		//	normScale=matchCost[iLayerIndex];
		//	bestLabel=iLayerIndex;
		//}
	}

	//std::cout<<"normScale: "<<normScale<<std::endl;    
	bestLabel=(int)(100*(normScale/2.5));
	//计算不相似度，并记录最大的不相似度
	float maxCost = minVal;
	const float invNormScale = 1.0 / normScale;
	for(int iLayerIndex=0; iLayerIndex<dspLevelCount;iLayerIndex++)
	{
		const float tmp = 1.0 - dataCosti[iLayerIndex] * invNormScale;
		dataCosti[iLayerIndex] = 		tmp;
		maxCost = max(tmp,maxCost);
	}
*/

// ORG
		//取最大相似度
		for(int depthLeveli=0;depthLeveli<dspLevelCount;depthLeveli++)
		{
			MaxLikelihood=max(MaxLikelihood,dataCosti[depthLeveli]);
		}
		
		  // std::cout<<"MaxLikelihood:"<<MaxLikelihood<<std::endl;
		//std::cout<<"bestLabel: "<<bestLabel<<std::endl;
		bestLabel=(int)(100*(MaxLikelihood/2.5));

		//Normalize (归一化）
		float maxCost = 1e-3f;
		for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++){
			const float tmp=  1.0 - dataCosti[depthLeveli] / MaxLikelihood;
		    dataCosti[depthLeveli]=tmp;
			maxCost = max(maxCost,tmp);
		}
//

		for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++){
			dataCosti[depthLeveli] /= maxCost;
			//std::cout<<"dataCosti[depthLeveli]: "<<dataCosti[depthLeveli]<<std::endl;
			//dataCosti[depthLeveli] *= dataCostWeight;
		}
	//	std::cout<<"MatchCost: "<<std::endl;

		float  conf = pCurrentFrame->m_pVarianceMap->at(x,y);
		//std::cout<<"conf:"<<conf<<std::endl;

		conf = conf / ( conf + 10 );
		for(int iLayerIndex = 0;iLayerIndex<dspV.size();iLayerIndex++)
		{
			dataCosti[ iLayerIndex ] *= conf;
		//	std::cout<<dataCosti[iLayerIndex]<<" ";
			//dataCosti[iLayerIndex] *= dataCostWeight; //KXL-version不需要加
		}
		//std::cout<<std::endl;
	//	std::cout<<"Test DataCost!"<<std::endl;
}




/*
//ORG
void LSDepthEstimator::GetInitialDataCostAt(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, 
	LSVideoFrame* pCurrentFrame, int x, int y, DataCostUnit& dataCosti, int& bestLabel, std::vector<float>& dspV, bool ConsiderDspRang ){
		LSParaWidget* paras = LSParaWidget::GetInstance();

		float fColorSigma = paras->GetColorSigma();
		float fColorMissPenalty = paras->GetMissPenalty();
		float dataCostWeight = paras->GetTrueDataCostWeight();

		int dspLevelCount = dspV.size();

		int minLabel = 0, maxLabel = dspLevelCount - 1;


		if(ConsiderDspRang == true){
			minLabel = pCurrentFrame->m_pMinLabelImg->at(x, y);   //x=0,y=0;   
			maxLabel = pCurrentFrame->m_pMaxLabelImg->at(x,y);
		}
		//std::cout<<"x: "<<x<<" , "<<"y: "<<y<<std::endl;
		//std::cout<<"minLabel: "<<minLabel<<" , "<<"maxLabel: "<<maxLabel<<std::endl;
		
		//assign value for two sides
		for(int iLayerIndex = 0; iLayerIndex <= minLabel; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.00001F;
		for(int iLayerIndex = maxLabel; iLayerIndex < dspLevelCount; iLayerIndex++)
		{
			dataCosti[iLayerIndex] = 0.00001F;

		}
		float MaxLikelihood = 1.0e-6F;
		bestLabel = 0;

		//add reference frames in
		std::vector<LSVideoFrame*> NearFrames(FwFrames.begin(), FwFrames.end());
		NearFrames.insert(NearFrames.end(), BwFrames.begin(), BwFrames.end());
		int refFrameCount = NearFrames.size();

		Wml::Vector3d ptWorldCoord;
		Wml::Vector3f CurrentColor, CorrespondingColor;


		double u2, v2, dsp;
		//double z, u2, v2, z2, dsp;
		//vector< vector<double> > r (NearFrames.size(), vector<double>(3)); 
		//vector< vector<double> > b (NearFrames.size(), vector<double>(3));
		//double fx = (u - pCurrentFrame->GetInternalParaAt(0,2))/pCurrentFrame->GetInternalParaAt(0,0);
		//double fy = (v - pCurrentFrame->GetInternalParaAt(1,2))/pCurrentFrame->GetInternalParaAt(1,1);
		//for(int i=0; i<NearFrames.size(); i++){
		//	NearFrames[i]->Calculate(&r[i][0], &b[i][0], fx, fy, PList[i]);	
		//}

	 // std::cout<<"x:  "<<x<<" , "<<"y: "<<y<<std::endl;
	  
		  // Have bug!!!
 		pCurrentFrame->GetColorAt(x, y, CurrentColor);
		std::vector<float>  matchCostCandinate(refFrameCount);
		int halfNeighberSize = refFrameCount / 2;
		halfNeighberSize = min( max(halfNeighberSize,2), refFrameCount);

		for(int depthLeveli = minLabel; depthLeveli <= maxLabel; depthLeveli++){

			pCurrentFrame->GetWorldCoordFrmImgCoord(x, y, dspV[depthLeveli], ptWorldCoord);
			//z = 1.0/dspV[depthLeveli];
			for(int Framei=0; Framei<refFrameCount; Framei++){
				NearFrames[Framei]->GetImgCoordFrmWorldCoord(u2, v2, dsp, ptWorldCoord);
				//z2 = r[i][2]*z + b[i][2];
				//u2 = (r[i][0]*z + b[i][0]) / z2;
				//v2 = (r[i][1]*z + b[i][1]) / z2;	

			    //out of range
				if(u2<0 || v2<0 || u2> NearFrames[Framei]->GetImgWidth()-1 || v2 > NearFrames[Framei]->GetImgHeight()-1)
					matchCostCandinate[Framei] = fColorSigma / (fColorSigma + fColorMissPenalty * fColorMissPenalty);

				else
				{
					NearFrames[Framei]->GetColorAt((float)u2, (float)v2, CorrespondingColor);

					float colordist = (fabs(CurrentColor[0] - CorrespondingColor[0]) 
						+ fabs(CurrentColor[1] - CorrespondingColor[1]) 
						+ fabs(CurrentColor[2] - CorrespondingColor[2])) / 3.0;

					colordist *= colordist;
					matchCostCandinate[Framei] = fColorSigma / (fColorSigma + colordist);
				}
			}
			////std::nth_element(matchCostCandinate.begin(), matchCostCandinate.begin() + halfNeighberSize,	matchCostCandinate.end(), std::greater<float>() );
			//float sum = 0.0F;
			//for(int iCandidate = 0;iCandidate<halfNeighberSize;++iCandidate)
			//for(int iCandidate = 0;iCandidate<matchCostCandinate.size();++iCandidate)
			//	sum += matchCostCandinate[iCandidate];

			//dataCosti[depthLeveli] = sum / matchCostCandinate.size(); //halfNeighberSize;

			dataCosti[depthLeveli] = GetValue(matchCostCandinate, FwFrames.size(), BwFrames.size());

			if(dataCosti[depthLeveli] > MaxLikelihood)
			{
				MaxLikelihood = dataCosti[depthLeveli];

				//bestLabel = depthLeveli; // ORG
				
			}

		}

		//std::cout<<"bestLabel: "<<bestLabel<<std::endl;
		bestLabel=(int)(100*(MaxLikelihood/1.0));

		//Normalize (归一化）
		float maxCost = 1e-6F;
		for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++){
			dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;
			maxCost = max(maxCost,dataCosti[depthLeveli]);
		}

		for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++){
			dataCosti[depthLeveli] /= maxCost;
			//std::cout<<"dataCosti[depthLeveli]: "<<dataCosti[depthLeveli]<<std::endl;
			dataCosti[depthLeveli] *= dataCostWeight;
		}

}
*/

//对于此函数的具体处理过程及原因尚不清楚。。
float LSDepthEstimator::GetValue( std::vector<float>& matchCostCandinate, int fwFrameCount, int bwFrameCount )
{
	//以下三种处理的具体原理尚不清楚。。
	if(fwFrameCount == 0 || bwFrameCount==0)
		return std::accumulate(matchCostCandinate.begin(), matchCostCandinate.end(), 0.0F) / matchCostCandinate.size();

	if(fwFrameCount > bwFrameCount)
	{
		if(bwFrameCount <= fwFrameCount/2)
			return std::accumulate(matchCostCandinate.begin(), matchCostCandinate.end(), 0.0F) / matchCostCandinate.size();
		return std::accumulate(matchCostCandinate.begin(), matchCostCandinate.begin() + fwFrameCount + fwFrameCount/2, 0.0F) / (fwFrameCount + fwFrameCount/2);
	}

	if(bwFrameCount > fwFrameCount)
	{
		if(fwFrameCount <= bwFrameCount/2)
			return std::accumulate(matchCostCandinate.begin(), matchCostCandinate.end(), 0.0F) / matchCostCandinate.size();
		return std::accumulate(matchCostCandinate.begin(), matchCostCandinate.begin() + bwFrameCount/2, 0.0F) + 
			std::accumulate(matchCostCandinate.begin() + fwFrameCount, matchCostCandinate.end(), 0.0F) / (bwFrameCount + bwFrameCount/2);;
	}

	float value[2] = {0};
	int len = matchCostCandinate.size()/2;
	int halfLen = (len / 2 > 0 ? len / 2 : 1);

	float sumFw = 0;
	for(int i= 0; i<len; i++){
		if(i < halfLen)
			value[0] += matchCostCandinate[i];
		sumFw += matchCostCandinate[i];
	}
	float sumBw = 0;
	for(int i= 0; i<len; i++){
		if(i < halfLen)
			value[1] += matchCostCandinate[i + len];
		sumBw += matchCostCandinate[i + len];
	}

	if(sumFw > sumBw)
		return (sumFw + value[1]) / (len + halfLen);
	return (sumBw + value[0]) / (len + halfLen);
}

void LSDepthEstimator::GetInitialDataCost(std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, DataCost& outDataCost, std::vector<float>& dspV, ZIntImage& labelImg ){
	//CParallelManager pm( CSystemParameters::GetInstance()->GetCpuThreads() );
	
	//std::cout<<"m_iBlockWidth:"<<m_iBlockWidth<<",m_iBlockHeight:"<<m_iBlockHeight<<std::endl; //(just calculate for single frame)
	//std::cout<<"m_iOffsetX:"<<m_iOffsetX<<",m_iOffsetY:"<<m_iOffsetY<<std::endl;

	for(int j=0; j<m_iBlockHeight; ++j){ //（按每行计算）
		LSStereoWorkUnit* pWorkUnit = new LSStereoWorkUnit(pCurrentFrame, FwFrames, BwFrames, labelImg, outDataCost, dspV, j, m_iBlockWidth, LSStereoWorkUnit::INIT);//,dspV,depthEst,labelImg,j);
		pWorkUnit->Execute();
		//pm.EnQueue(pWorkUnit);
	}
	//pm.Run();
}


void LSDepthEstimator::GetBODataCost( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, DataCost& outDataCost, std::vector<float>& dspV, ZIntImage& labelImg ){
	//CParallelManager pm( CSystemParameters::GetInstance()->GetCpuThreads() );
	for(int j=0; j<m_iBlockHeight; ++j){
		LSStereoWorkUnit* pWorkUnit = new LSStereoWorkUnit(pCurrentFrame, FwFrames, BwFrames, labelImg, outDataCost, dspV, j, m_iBlockWidth, LSStereoWorkUnit::BO);
		pWorkUnit->Execute();
		//pm.EnQueue(pWorkUnit);
	}
	//pm.Run();
}


//new add- optical flow calculation-for temporal consistence
void LSDepthEstimator::GetOptDataCost( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, DataCost& optDataCost, std::vector<float>& dspV, ZIntImage& labelImg ){
	//CParallelManager pm( CSystemParameters::GetInstance()->GetCpuThreads() );
	for(int j=0; j<m_iBlockHeight; ++j)
	{
		LSStereoWorkUnit* pWorkUnit = new LSStereoWorkUnit(pCurrentFrame, FwFrames, BwFrames, labelImg, optDataCost, dspV, j, m_iBlockWidth, LSStereoWorkUnit::OPT);
		pWorkUnit->Execute();
		//pm.EnQueue(pWorkUnit);
	}
	//pm.Run();
}


void LSDepthEstimator::RefineDspByBP( DataCost& DataCost, ZIntImage& labelImg, bool addEdgeInfo, ZIntImage* offsetImg /*= NULL*/ )
{
	
	LSBeliefPropagation bp(
		*( LSImgPartition::GetInstance() ),
		LSParaWidget::GetInstance()->GetTrueDiscK(),			 //disc_k   10
		DataCost.GetDspLevels() ,  //    101
		0.1F,					 //sigma for gauss smooth
		offsetImg != NULL ? 10 : 5,				//nIter    5
		offsetImg != NULL ? 1 : 5				//nlevels  1
		);
	/*  NEW ADD
	LSBeliefPropagation bp(
	*( LSImgPartition::GetInstance() ),
	LSParaWidget::GetInstance()->GetTrueDiscK(),			 //disc_k   10
	DataCost.GetDspLevels() ,  //    101
	0.1F,					 //sigma for gauss smooth
	5			//nIter (10 is too large and can't compute) 	//nlevels  1
	);
	*/
	bp.printPara();
 
	bp.run(DataCost, labelImg, addEdgeInfo, offsetImg);
 
}

//KXL-Version BP-NEW ADD by LY 2014/12/18
void LSDepthEstimator::TileOptimizeGeneral(
	const int  apronedTileStartX,
	const int apronedTileStartY, 
	const int apronedTileEndX,
	const int apronedTileEndY,
	std::vector<ZFloatImage*>& matchCostCube,
	ZIntImage& tmpLabelImg,bool useGaussianFilter /*= false*/)
{

   std::cout<<"TileOptimizeGeneral Function!"<<std::endl;

   const int  apronedTileWidth = apronedTileEndX - apronedTileStartX + 1;
   const int apronedTileHeight = apronedTileEndY - apronedTileStartY + 1;

   //4) Optimization   
   NumericalAlgorithm::CExWRegularGridBP stereoBP;	 //Create BP object
   ZCubeFloatImage tmpCubeCost;		//make sure the image width is multiple of 4.
   tmpCubeCost.Create(apronedTileWidth,apronedTileHeight,matchCostCube.size()); 

   ZIntImage offsetImg;
   offsetImg.CreateAndInit(apronedTileWidth,apronedTileHeight);
   tmpLabelImg.Clear();
   tmpLabelImg.CreateAndInit(apronedTileWidth,apronedTileHeight);


   //set DataCost weight and truncValue
   const int maxLayers = tmpCubeCost.GetChannel();
  

   double smoothWeight=LSParaWidget::GetInstance()->GetDataCostWeight()/ 100.0 * 
	   (maxLayers - 1);

   double truncValue = 
	   LSParaWidget::GetInstance()->GetDiscK()/ 100.0 * 
	   (maxLayers - 1);

   std::cout<<"maxLayers:"<<maxLayers<<std::endl;
   printf("smooth:%f, tranc:%f\n",smoothWeight,truncValue);

   if(useGaussianFilter)
   {
	   const int gaussianWin = 3;
	   ZFloatImage filterSlice( apronedTileWidth,apronedTileHeight );
	   for(int c=0; c<maxLayers; ++c)
	   {
		   IMGAlgorithm::BoxFilter( *(matchCostCube[c]),filterSlice,gaussianWin,gaussianWin,
			   IMGAlgorithm::eBorderReplicate,0,true);
		   for(int y=0; y<apronedTileHeight; ++y)
			   for(int x=0; x<apronedTileWidth; ++x)
				   matchCostCube[c]->at(x,y) = filterSlice.at(x,y);
	   }
   }

   for(int j=0; j<apronedTileHeight; ++j)
	   for(int i=0; i<apronedTileWidth; ++i)
	   {
		   offsetImg.at(i,j) = 0;
		   for(int c=0; c<tmpCubeCost.GetChannel(); ++c)
		   {
			   tmpCubeCost.at(i,j,c) = smoothWeight * matchCostCube[c + offsetImg.at(i,j)]->at(i,j);
		   }			
	   }


	   for(int i=0; i<matchCostCube.size(); ++i)
		   delete matchCostCube[i];


	   stereoBP.VALUES = tmpCubeCost.GetChannel();

	   stereoBP.m_iMaxIter = 10;
	   int parallThreadNum=0;    //临时添加，之后并行时可以考虑
	   stereoBP.m_iThreadCount = max(1,parallThreadNum);

	   ZFloatImage weightMap, truncMap;
	   weightMap.CreateAndInit(apronedTileWidth,apronedTileHeight,4,1);	
	   truncMap.CreateAndInit(apronedTileWidth,apronedTileHeight,4,truncValue);

	   stereoBP.Solve(tmpCubeCost,tmpLabelImg,offsetImg,weightMap,truncMap);

}


//KXL version-DE'BP
void LSDepthEstimator::TileSubRefineOptimize(
	const int initLayer,
	const int subSample,
	const int  apronedTileStartX,const int apronedTileStartY, 
	const int apronedTileEndX,const int apronedTileEndY,
	ZCubeFloatImage& subDataCost,
	ZIntImage& offsetImg,//just a block,please setup it
	ZIntImage& tmpLabelImg//ans
	)
{
	const int  apronedTileWidth = apronedTileEndX - apronedTileStartX + 1;
	const int apronedTileHeight = apronedTileEndY - apronedTileStartY + 1;

	//4) Optimization
	NumericalAlgorithm::CExWRegularGridBP stereoBP;

	double smoothWeight = 
		LSParaWidget::GetInstance()->GetDataCostWeight() / 100.0f * initLayer;
	double truncValue = 
		LSParaWidget::GetInstance()->GetDiscK() / 100 * initLayer;

	printf("smooth:%f, tranc:%f\n",smoothWeight,truncValue);

	for(int j=0; j<apronedTileHeight; ++j)
		for(int i=0; i<apronedTileWidth; ++i)
		{
			for(int c=0; c<subDataCost.GetChannel(); ++c)
			{
				subDataCost.at(i,j,c) *=smoothWeight / 100.0 * subSample;
			}			
		}

		stereoBP.VALUES = subDataCost.GetChannel();

		stereoBP.m_iMaxIter = 10;
		int parallThreadNum=0;    //临时添加，之后并行时可以考虑
		stereoBP.m_iThreadCount = max(1,parallThreadNum);

		ZFloatImage weightMap, truncMap;
		weightMap.CreateAndInit(apronedTileWidth,apronedTileHeight,4,1);	
		truncMap.CreateAndInit(apronedTileWidth,apronedTileHeight,4,truncValue * subSample);
		tmpLabelImg.CreateAndInit( apronedTileWidth,apronedTileHeight);

		stereoBP.Solve(subDataCost,tmpLabelImg,offsetImg,weightMap,truncMap);	
}






//此函数目前会影响程序的执行效率，如果重写或加速需要完全替换前面的DataCost
bool LSDepthEstimator::CopyValueFromDataCostToMatchCostCube(DataCost& dataCost,std::vector<ZFloatImage*>& matchCostCube)
{
	for(int iLevel=0;iLevel<dataCost.GetDspLevels();iLevel++)
	{
	     for(int iHeight=0;iHeight<matchCostCube[iLevel]->GetHeight();iHeight++)
		 {
			 for(int iWidth=0;iWidth<matchCostCube[iLevel]->GetWidth();iWidth++)
			 {
				 matchCostCube[iLevel]->at(iWidth,iHeight)=dataCost.At(iWidth,iHeight,iLevel);
			 }

		 }
	}
	std::cout<<"Copy Value from DataCost to MatchCostCube"<<std::endl;
	return true;
}







void LSDepthEstimator::RefineDspBySegm(const Block &block,const MeanShiftSeg &meanShiftSegm, LSVideoFrame* currentFrame, ZIntImage& labelImg, DataCost& DataCost )
{
	class cmp
	{
	public:
		bool operator()(const std::vector<blk::Point> &v1, const std::vector<blk::Point> &v2)
		{
			return v1.size() > v2.size();
		}
	};
	std::vector<std::vector<blk::Point> > regions;
	
	meanShiftSegm.GetRegions(regions,block);
   // std::cout<<"get regions over!"<<std::endl;
	std::sort(regions.begin(), regions.end(), cmp());
   
//	std::cout<<"regions.size():"<<regions.size()<<std::endl;

	//int threadCount = DepthParaWidget::GetInstance()->m_iThreadsCount;
	int planfittingSize = LSParaWidget::GetInstance()->m_iPlaneFittingSize;   //300 could be adjusted

	//CParallelManager pm(threadCount);
	for(auto iSeg=0U; iSeg<regions.size(); ++iSeg)
	{
	//	std::cout<<regions[iSeg].size()<<std::endl;
		if(regions[iSeg].size() >= planfittingSize)
		{
			PlanFittingWorkUnit* pWorkUnit = new PlanFittingWorkUnit(block, meanShiftSegm, regions[iSeg], currentFrame, labelImg, DataCost);
			//pm.EnQueue(pWorkUnit);
			pWorkUnit->LsExecute();
		}
	}
	


  // regions.clear();
   
	//pm.Run();

	
}

//在单个单元上应用Planfitting
void LSDepthEstimator::PlanfittingOneSegm(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, LSVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost)
{
	//std::cout<<"PlanfittingOneSegm..."<<std::endl;

	//四邻域
	static const int NeighborCount = 4;
	static const int Neighbor[NeighborCount][2] = { {0,-1}, {1,0}, {0,1}, {-1,0} };

	int dspLevel = dataCost.GetDspLevels();
	float disck = LSParaWidget::GetInstance()->GetTrueDiscK();
	std::vector<double> layerEnergy(dspLevel, 0);  //创建dspLevel个对象，每个初值赋值为0


	//Find best layer (求出能量值最小的层数）
	double minSegEnergy = 1e20;
	int iSegBestLayer=0;
	for(int iLayer=0; iLayer<dspLevel; ++iLayer)
	{
		for(std::vector<blk::Point>::const_iterator it = SegmPoints.begin(); it != SegmPoints.end(); ++it)
		{
			int x = it->X;
			int y = it->Y;
			//layerEnergy[iLayer] += dataCost[((offsetX + x) + (offsetY + y) * m_iBlockWidth) * dspLevel + iLayer];	
				

			layerEnergy[iLayer] += dataCost.GetValueAt(x - block.m_X,  y - block.m_Y, iLayer);  //may have bug
		
			//DataCostUnit dataCosti;
			//dataCost.GetDataCostUnit(x-block.m_X, y-block.m_Y, dataCosti);     //may have problem
			//std::cout<<" dataCost.GetValueAt(x,  y, iLayer): "<< dataCost.GetValueAt(x,  y, iLayer)<<std::endl;
			//std::cout<<" dataCost.GetValueAt(x - block.m_X,  y - block.m_Y, iLayer): "<< dataCost.GetValueAt(x - block.m_X,  y - block.m_Y, iLayer)<<std::endl; 
			//std::cout<<"dataCost.GetDataCostUnit(x-block.m_X, y-block.m_Y, dataCosti):"<<dataCosti[iLayer]<<std::endl;


			//Neighbor
			int x1,y1;
			for(int neighbori = 0; neighbori< NeighborCount; neighbori++)
			{
				x1 = x + Neighbor[neighbori][0];
				y1 = y + Neighbor[neighbori][1];

				if(block.IsInBlock(x1, y1) && meanShiftSegm.GetSegmMapAt(x,y) != meanShiftSegm.GetSegmMapAt(x1,y1) && labelImg.at(x1,y1) != -1)
				{
					layerEnergy[iLayer] += std::min<double>(disck, abs( iLayer - labelImg.at(x1, y1)));   //参看论文公式4.1
				}
			}
		}
		if(minSegEnergy > layerEnergy[iLayer])
		{
			minSegEnergy = layerEnergy[iLayer];
			iSegBestLayer = iLayer;
		}
	}

	/************************************************************************/
	/* Init LevenMar                                                        */
	/************************************************************************/


	int segPointCount = SegmPoints.size();

	//构造函数（初始化）
	LevenMarOptimizer LMOptimizer(SegmPoints, 25,  dspLevel, disck);
	double nonSegEnergy = 0;
	double nonSegSmoothCost = 0;
	DataCostUnit dataCostUnit;

	for(int index = 0; index < segPointCount; ++index)
	{
		int x = SegmPoints[index].X;
		int y = SegmPoints[index].Y;

		int iBestLayer = labelImg.at( x, y);
		dataCost.GetDataCostUnit(x - block.m_X, y - block.m_Y, dataCostUnit);

		// 设置某个Index上（不同layer上）的dataCostUnit值
		LMOptimizer.SetDataCostAt(index, dataCostUnit);
		nonSegEnergy += dataCost.GetValueAt(x - block.m_X, y - block.m_Y, iBestLayer);



		int x1, y1;
		for(int neighbori = 0; neighbori< NeighborCount; neighbori++){
			x1 = x + Neighbor[neighbori][0];
			y1 = y + Neighbor[neighbori][1];
			//判断是否在块内，不在块内则程序继续（忽略掉）
			if(block.IsInBlock(x1, y1) == false)
				continue;

			//对下面的代码不是很理解
			if(meanShiftSegm.GetSegmMapAt(x,y) != meanShiftSegm.GetSegmMapAt(x1,y1) ){
				LMOptimizer.AddEdgeNeighborInfo(index, labelImg.at( x1,  y1));
				nonSegEnergy += std::min<double>(disck, abs( iBestLayer - labelImg.at(x1, y1)));
			}
			else
				nonSegSmoothCost += std::min<double>(disck, abs( iBestLayer - labelImg.at(x1, y1)));

		}

	}
	nonSegEnergy += nonSegSmoothCost/2.0;


	double MinValue;


	Wml::GVectord optA(3);

	//先固定a_i,b_i两个元素，求c_i
	optA[0] = 0.0; optA[1] = 0.0; optA[2] = iSegBestLayer;

	//利用levenMar求解a_i,b_i(c_i）
	LMOptimizer.Optimize(optA, MinValue);
	double a,b,c;
	a = optA[0]; b = optA[1];	c = optA[2];

	minSegEnergy = LMOptimizer.GetAbsValue(optA);

	//std::cout<<"minSegEnergy:"<<minSegEnergy<<"   nonSegEnergy: "<<nonSegEnergy<<std::endl;

	double segErrRateThreshold =LSParaWidget::GetInstance()->GetTruetSegErrRateThreshold();

	//比较并更新dis
	if(minSegEnergy < segErrRateThreshold * nonSegEnergy)
	{
		for(std::vector<blk::Point>::const_iterator ptIter = SegmPoints.begin(); ptIter != SegmPoints.end(); ++ptIter)
		{
			int x = ptIter->X;
			int y = ptIter->Y;
			float bestD = a * x + b * y + c;
			pCurrentFrame->SetDspAt(x, y, GetDspAtLeveli(pCurrentFrame,bestD));  // may have bug!!
			labelImg.at(x, y) = std::min<int>( dspLevel-1, max<int>(0, bestD+0.5) );
		}
	}
	
}

float LSDepthEstimator::GetDspAtLeveli(LSVideoFrame* pCurrentFrame, float leveli )
{
	//may have bug!!
	float dspMin;
	float dspMax;
     pCurrentFrame->GetDspRange(dspMin,dspMax);
	
	int dspLevel = LSParaWidget::GetInstance()->m_iDspLevel;
	return dspMin * (dspLevel - 1 - leveli)/(dspLevel - 1)  + dspMax * leveli/(dspLevel - 1) ;
}


// NEW ADD - KXL Version-DE
void LSDepthEstimator::_SuperRefine( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int subIterCount, LSVideoFrame::LABEL_IMG_TYPE type,ZIntImage& LabelImg){

	std::cout << "Depth expansion..."<<std::endl;
	LSParaWidget* paras = LSParaWidget::GetInstance();
	int dspLevel = paras->GetDspLevel();
	int subSample = paras->GetSubSample();

	int minX, minY, maxX, maxY;
	pCurrentFrame->GetMaskRectangle(minX, minY, maxX, maxY);
	int MaskWidth = maxX - minX + 1;
	int MaskHeight = maxY - minY + 1;
	LSImgPartition::GetInstance()->SetImgSize(MaskWidth, MaskHeight);

	int ImgWidth = pCurrentFrame->GetImgWidth();
	int ImgHeight = pCurrentFrame->GetImgHeight();

	ZIntImage dataCostlabelImg, resLabelImg, maskLabelImg;
	dataCostlabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	resLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	ZIntImage offsetImg(MaskWidth, MaskHeight);
	//原注 ZIntImage LabelImg;


	//coding by Yu Liu on Mar 22, 2014
	//原注 pCurrentFrame->InitLabelImgByDspImg(LabelImg, dspLevel);
	//pCurrentFrame->SaveLabelImg(LabelImg, CBDMVideoFrame::INIT, dspLevel);

	//此处可加验证代码！
	//std::cout<<"LabelImg.at(359,40):"<<LabelImg.at(359,40)<<std::endl;
	//std::cout<<"LabelImg.at(359,42):"<<LabelImg.at(359,42)<<std::endl;
	//std::cout<<"LabelImg.at(593,720):"<<LabelImg.at(593,720)<<std::endl;
	//std::cout<<"LabelImg.at(590,720):"<<LabelImg.at(590,720)<<std::endl;

	int subDsplevel = 2 * subSample + 1;
	int LayerCount = dspLevel - 1;
	//============backup==================
	float backupDspSigma = paras->GetDspSigma();        //0.03
	float backupDataCostWeight = paras->GetDataCostWeight();  //20
	float backupDiscK = paras->GetDiscK();    //3

	
	float dspSigma = backupDspSigma;
	float dataCostWeight = backupDataCostWeight;
	float discK = backupDiscK;

	
	/*
	 dspSigma=0.03;
	 paras->SetDspSigma(dspSigma);
	 */


	//dspSigma = max(0.001,  dspSigma / 1.5);
	/*
    //ORG
	dspSigma = max(0.001,  dspSigma / 2);
	paras->SetDspSigma(dspSigma);

	dataCostWeight *= (float)subSample / LayerCount; //(float)(m_iDspLvlNm - 1) / subDsplevel; //(float)m_iSubSample /(m_iDspLvlNm - 1); //(subDeplevel - 1.0F) / (m_iDspLevel - 1);
	paras->SetDataCostWeight(dataCostWeight);
	discK *= subSample; //(subDeplevel - 1.0F) / (m_iDspLevel - 1);
	paras->SetDiscK(discK);
	*/	
	
	int blockCountX = LSImgPartition::GetInstance()->GetBlockCountX();
	int blockCountY = LSImgPartition::GetInstance()->GetBlockCountY();

	for(int iPass = 0; iPass < subIterCount; iPass++){
		std::cout << "DepthExpansion iter:" << iPass << "/" << subIterCount<< std::endl;

		LayerCount *= subSample;
		std::cout << "Layer count:" << LayerCount << ", subDspLevel:" << subDsplevel << ", DataCost weight:"<< dataCostWeight << std::endl;
		 
	    //bug is here  
		// layerCount =0;  subDsplevel =1;  dataCostWeight=0;
		//coding by Yu Liu, Mar 28, 2014


		maskLabelImg.CreateAndInit(MaskWidth, MaskHeight, 1, -1);

		for(int y = minY; y <= maxY; y++){
			for(int x = minX; x <= maxX; x++){
				offsetImg.at(x - minX, y - minY) = min( LayerCount - (subDsplevel - 1), max(0, (LabelImg.at(x, y) - 1) * subSample) );
			}
		}
		//pCurrentFrame->SaveLabelImg(offsetImg, CBDMVideoFrame::INIT, LayerCount + 1);

		for(int blockIdy = 0; blockIdy < blockCountY; blockIdy++){
			for(int blockIdX = 0; blockIdX < blockCountX; blockIdX++){

				int offsetX, offsetY;
				int blockWidth, blockHeight;
				int trueX1, trueY1, trueX2, trueY2;
			    LSImgPartition::GetInstance()->SetBlockId(blockIdX, blockIdy);
				LSImgPartition::GetInstance()->GetBlockInfoSimple(trueX1, trueY1, trueX2, trueY2, offsetX, offsetY, blockWidth, blockHeight);
				SetBlockState(trueX1 + minX, trueY1 + minY, trueX2 + minX, trueY2 + minY, offsetX + minX, offsetY + minY, blockWidth, blockHeight, blockIdX, blockIdy);

			//	clock_t tempTime = clock();
				//printf("Init DataCost:\n");
				std::cout<<"DE Part:Init DataCost:"<<std::endl;
				PrintBlockState();

				DataCost subDataCost(m_iBlockWidth, m_iBlockHeight, subDsplevel);
			
				GetSupreRefineDataCost(FwFrames, BwFrames, pCurrentFrame, subDataCost, offsetImg, LayerCount + 1, subSample, dataCostlabelImg, type);
				//pCurrentFrame->SaveDataCost(DataCost, m_iBlockWidth * m_iBlockHeight * dspLevel * sizeof(float));
				//pCurrentFrame->ReadDataCost(DataCost, labelImg, m_iOffsetX, m_iOffsetY, m_iBlockWidth, m_iBlockHeight );
		
				if(paras->GetIfGenerateTempResult() == true){
					for(int y = trueY1; y <= trueY2; y++){
						for(int x = trueX1; x <= trueX2; x++){
							/*
							if(pCurrentFrame->IsInMask(x + minX, y + minY) == false)
								resLabelImg.at(x + minX, y + minY) = -1;
							else
						   */
								resLabelImg.at(x + minX, y + minY) = offsetImg.at(x, y);// + dataCostlabelImg.at(x + minX, y + minY);
						}
					}
			//		pCurrentFrame->SaveLabelImg(resLabelImg, LSVideoFrame::BO_DE, dspLevel);
				}



        //NEW ADD- KXL-Version BP-DE
				printf("DE:Init By BP:\n");
				int apronedTileStartX,apronedTileStartY,apronedTileEndX,apronedTileEndY;
				apronedTileStartX=minX+trueX1;
				apronedTileStartY=minY+trueY1;
				apronedTileEndX=minX+trueX2;
				apronedTileEndY=minY+trueY2;
				const int  apronedTileWidth = apronedTileEndX - apronedTileStartX + 1;
				const int apronedTileHeight = apronedTileEndY - apronedTileStartY + 1;
				
	//	CopyValueFromDataCostToMatchCostCube;
				ZCubeFloatImage matchCostCube;
				matchCostCube.CreateAndInit(apronedTileWidth,apronedTileHeight,subDsplevel,0.0f);
				for(int iLevel=0; iLevel<subDsplevel; ++iLevel)
				{
					for(int iHeight=0;iHeight<matchCostCube.GetHeight();iHeight++)
					{
						for(int iWidth=0;iWidth<matchCostCube.GetWidth();iWidth++)
						{
							matchCostCube.at(iWidth,iHeight,iLevel)=subDataCost.At(iWidth,iHeight,iLevel);
						}

					}

				}
					


		//此处开始有问题
				ZIntImage  tmpLabelMap,tmpOffsetImg;
				tmpLabelMap.CreateAndInit(apronedTileWidth,apronedTileHeight);
				tmpOffsetImg.CreateAndInit(apronedTileWidth,apronedTileHeight);
				for(int y=apronedTileStartY; y<=apronedTileEndY; ++y)
					for(int x=apronedTileStartX; x<=apronedTileEndX; ++x)
						tmpOffsetImg.at(x - apronedTileStartX,y - apronedTileStartY) = offsetImg.at(x-apronedTileStartX,y-apronedTileStartY);

         
				
				TileSubRefineOptimize(dspLevel,subSample,apronedTileStartX,apronedTileStartY,apronedTileEndX,apronedTileEndY,
					matchCostCube,tmpOffsetImg,tmpLabelMap);

				//std::cout<<"trueX1:"<<trueX1<<",trueY1:"<<trueY1<<",trueX2:"<<trueX2<<",trueY2:"<<trueY2<<std::endl;
				//std::cout<<"minX:"<<minX<<",minY:"<<minY<<",maxX:"<<maxX<<",maxY:"<<maxY<<std::endl;
				//std::cout<<"apronedTileStartX:"<<apronedTileStartX<<",apronedTileStartY:"<<apronedTileStartY<<",apronedTileEndX:"<<apronedTileEndX<<",apronedTileEndY:"<<apronedTileEndY<<std::endl;


			//	RefineDspByBP(subDataCost, maskLabelImg, true, &offsetImg);

				//New ADD-将mask改为tile,是要对遮挡进行处理
				for(int y = trueY1; y <= trueY2; y++){
					for(int x = trueX1; x <= trueX2; x++){
						

						int tmp = tmpLabelMap.at(x ,y)+offsetImg.at(x, y);
						tmp = min(tmp,LayerCount);
						tmp = max(tmp,0);

						//std::cout<<"tmpLabelMap.at(x ,y):"<<tmpLabelMap.at(x,y)<<" ,offsetImg.at(x,y):"<<offsetImg.at(x,y)<<std::endl;
						//std::cout<<"tmpOffsetImg.at(x,y):"<<tmpOffsetImg.at(x,y)<<std::endl;
						//std::cout<<"Test Result"<<std::endl;
						LabelImg.at(x + minX, y + minY) = tmp;
					}
				}

				//if(paras->GetIfGenerateTempResult() == true)
				//	pCurrentFrame->SaveLabelImg(resLabelImg, CBDMVideoFrame::BO, LayerCount+1);
				//	printf("Time:%.3f s\n",(double)(clock()-tempTime)/CLOCKS_PER_SEC);
				//	tempTime = clock();
			   // pCurrentFrame->SetDspImg(LabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);   //bug may here
				pCurrentFrame->SetDspImg(LabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, LayerCount+1);

				tmpLabelMap.Clear();
				tmpOffsetImg.Clear();

       /*
	//ORG-LQL-Version BP-DE
				printf("Init By BP:\n");
				RefineDspByBP(subDataCost, maskLabelImg, true, &offsetImg);

				/// ORG
				//for(int y = trueY1; y <= trueY2; y++){
				//	for(int x = trueX1; x <= trueX2; x++){
				//		if(pCurrentFrame->IsInMask(x + minX, y + minY) == false)
				//			resLabelImg.at(x + minX, y + minY) = -1;
				//		else
				//			resLabelImg.at(x + minX, y + minY) = offsetImg.at(x, y) + maskLabelImg.at(x, y);

				//		LabelImg.at(x + minX, y + minY) = resLabelImg.at(x + minX, y + minY);
				//	}
				//}

				//New ADD-将mask改为tile,是要对遮挡进行处理
				for(int y = trueY1; y <= trueY2; y++){
					for(int x = trueX1; x <= trueX2; x++){

						
						std::cout<<<<" offsetImg.at(x,y):"<<offsetImg.at(x,y)<<std::endl;
						std::cout<<"maskLabelImg.at(x,y):"<<maskLabelImg.at(x,y)<<std::endl;
						//std::cout<<"Test Result"<<std::endl;
						
						resLabelImg.at(x + minX, y + minY) = offsetImg.at(x, y) + maskLabelImg.at(x, y);

						LabelImg.at(x + minX, y + minY) = resLabelImg.at(x + minX, y + minY);
					}
				}

				//if(paras->GetIfGenerateTempResult() == true)
				//	pCurrentFrame->SaveLabelImg(resLabelImg, CBDMVideoFrame::BO, LayerCount+1);
			//	printf("Time:%.3f s\n",(double)(clock()-tempTime)/CLOCKS_PER_SEC);
			//	tempTime = clock();
            //pCurrentFrame->SetDspImg(bpLabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);
 */
			}
			
		}

	}



	paras->SetDspSigma(backupDspSigma);
	paras->SetDataCostWeight(backupDataCostWeight);
	paras->SetDiscK(backupDiscK);

	//printf("layerCount %d\n", LayerCount + 1);
//	pCurrentFrame->SetDspImg(LabelImg, minX, minY, maxX, maxY, LayerCount + 1);




	if(LSParaWidget::GetInstance()->GetIfGenerateTempResult() == true)
	{
		//pCurrentFrame->InitLabelImgByDspImg(labelImg, dspLevel);
		pCurrentFrame->SaveLabelImg(LabelImg, type, LayerCount);
	}


	//std::cout<<"LabelImg.at(359,40):"<<LabelImg.at(359,40)<<std::endl;
	//std::cout<<"LabelImg.at(359,42):"<<LabelImg.at(359,42)<<std::endl;
	//std::cout<<"LabelImg.at(593,720):"<<LabelImg.at(593,720)<<std::endl;
	//std::cout<<"LabelImg.at(590,720):"<<LabelImg.at(590,720)<<std::endl;


	pCurrentFrame->SaveModel(true);

	pCurrentFrame->SaveDspImg();         //保存depth图像数据


	

	//pCurrentFrame->SaveModel(false);

	//pCurrentFrame->InitLabelImgByDspImg(LabelImg, LayerCount + 1);
	//pCurrentFrame->SaveLabelImg(LabelImg, LSVideoFrame::DE, LayerCount + 1);


//

	for(int i=0; i<FwFrames.size(); i++)
		FwFrames[i]->Clear();
	for(int i=0; i<BwFrames.size(); i++)
		BwFrames[i]->Clear();
	pCurrentFrame->Clear(); 

	std::cout<<"DE Over!"<<std::endl;
	}

/* ORG - LQL Version-DE
void LSDepthEstimator::_SuperRefine( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int subIterCount, LSVideoFrame::LABEL_IMG_TYPE type){

	std::cout << "Depth expansion..."<<std::endl;
	LSParaWidget* paras = LSParaWidget::GetInstance();

	int dspLevel = paras->GetDspLevel();
	int subSample = paras->GetSubSample();

	int minX, minY, maxX, maxY;
	pCurrentFrame->GetMaskRectangle(minX, minY, maxX, maxY);
	int MaskWidth = maxX - minX + 1;
	int MaskHeight = maxY - minY + 1;
	LSImgPartition::GetInstance()->SetImgSize(MaskWidth, MaskHeight);

	int ImgWidth = pCurrentFrame->GetImgWidth();
	int ImgHeight = pCurrentFrame->GetImgHeight();

	ZIntImage dataCostlabelImg, resLabelImg, maskLabelImg;
	dataCostlabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	resLabelImg.CreateAndInit( ImgWidth, ImgHeight, 1, -1);
	ZIntImage offsetImg(MaskWidth, MaskHeight);
	ZIntImage LabelImg;


	//coding by Yu Liu on Mar 22, 2014


	pCurrentFrame->InitLabelImgByDspImg(LabelImg, dspLevel);
	//pCurrentFrame->SaveLabelImg(LabelImg, CBDMVideoFrame::INIT, dspLevel);


	int subDsplevel = 2 * subSample + 1;
	int LayerCount = dspLevel - 1;
	//============backup==================
	float backupDspSigma = paras->GetDspSigma();
	float backupDataCostWeight = paras->GetDataCostWeight();
	float backupDiscK = paras->GetDiscK();

	float dspSigma = backupDspSigma;
	float dataCostWeight = backupDataCostWeight;
	float discK = backupDiscK;
    //ORG
	dspSigma = max(0.001,  dspSigma / 2);
	paras->SetDspSigma(dspSigma);

	dataCostWeight *= (float)subSample / LayerCount; //(float)(m_iDspLvlNm - 1) / subDsplevel; //(float)m_iSubSample /(m_iDspLvlNm - 1); //(subDeplevel - 1.0F) / (m_iDspLevel - 1);
	paras->SetDataCostWeight(dataCostWeight);
	discK *= subSample; //(subDeplevel - 1.0F) / (m_iDspLevel - 1);
	paras->SetDiscK(discK);
	
	
	////NEW ADD
	//dataCostWeight = (float)dataCostWeight/100.0*LayerCount; //(float)(m_iDspLvlNm - 1) / subDsplevel; //(float)m_iSubSample /(m_iDspLvlNm - 1); //(subDeplevel - 1.0F) / (m_iDspLevel - 1);
	//paras->SetDataCostWeight(dataCostWeight);
	//discK = discK/100.0*LayerCount; //(subDeplevel - 1.0F) / (m_iDspLevel - 1);
	//paras->SetDiscK(discK);



	//paras->setdsp

	int blockCountX = LSImgPartition::GetInstance()->GetBlockCountX();
	int blockCountY = LSImgPartition::GetInstance()->GetBlockCountY();

	for(int iPass = 0; iPass < subIterCount; iPass++){
		std::cout << "DepthExpansion iter:" << iPass << "/" << subIterCount<< std::endl;

		LayerCount *= subSample;
		std::cout << "Layer count:" << LayerCount << ", subDspLevel:" << subDsplevel << ", DataCost weight:"<< dataCostWeight << std::endl;
		 
	    //bug is here  
		// layerCount =0;  subDsplevel =1;  dataCostWeight=0;
		//coding by Yu Liu, Mar 28, 2014


		maskLabelImg.CreateAndInit(MaskWidth, MaskHeight, 1, -1);

		for(int y = minY; y <= maxY; y++){
			for(int x = minX; x <= maxX; x++){
				offsetImg.at(x - minX, y - minY) = min( LayerCount - (subDsplevel - 1), max(0, (LabelImg.at(x, y) - 1) * subSample) );
			}
		}
		//pCurrentFrame->SaveLabelImg(offsetImg, CBDMVideoFrame::INIT, LayerCount + 1);

		for(int blockIdy = 0; blockIdy < blockCountY; blockIdy++){
			for(int blockIdX = 0; blockIdX < blockCountX; blockIdX++){

				int offsetX, offsetY;
				int blockWidth, blockHeight;
				int trueX1, trueY1, trueX2, trueY2;
			    LSImgPartition::GetInstance()->SetBlockId(blockIdX, blockIdy);
				LSImgPartition::GetInstance()->GetBlockInfoSimple(trueX1, trueY1, trueX2, trueY2, offsetX, offsetY, blockWidth, blockHeight);
				SetBlockState(trueX1 + minX, trueY1 + minY, trueX2 + minX, trueY2 + minY, offsetX + minX, offsetY + minY, blockWidth, blockHeight, blockIdX, blockIdy);

			//	clock_t tempTime = clock();
				//printf("Init DataCost:\n");
				std::cout<<"Init DataCost:"<<std::endl;
				PrintBlockState();

				DataCost subDataCost(m_iBlockWidth, m_iBlockHeight, subDsplevel);

			
				GetSupreRefineDataCost(FwFrames, BwFrames, pCurrentFrame, subDataCost, offsetImg, LayerCount + 1, subSample, dataCostlabelImg, type);
				//pCurrentFrame->SaveDataCost(DataCost, m_iBlockWidth * m_iBlockHeight * dspLevel * sizeof(float));
				//pCurrentFrame->ReadDataCost(DataCost, labelImg, m_iOffsetX, m_iOffsetY, m_iBlockWidth, m_iBlockHeight );
		
				
			
				if(paras->GetIfGenerateTempResult() == true){
					for(int y = trueY1; y <= trueY2; y++){
						for(int x = trueX1; x <= trueX2; x++){
							if(pCurrentFrame->IsInMask(x + minX, y + minY) == false)
								resLabelImg.at(x + minX, y + minY) = -1;
							else
								resLabelImg.at(x + minX, y + minY) = offsetImg.at(x, y);// + dataCostlabelImg.at(x + minX, y + minY);
						}
					}
			//		pCurrentFrame->SaveLabelImg(resLabelImg, LSVideoFrame::BO_DE, dspLevel);
				}

				printf("Init By BP:\n");
				RefineDspByBP(subDataCost, maskLabelImg, true, &offsetImg);

				/// ORG
				//for(int y = trueY1; y <= trueY2; y++){
				//	for(int x = trueX1; x <= trueX2; x++){
				//		if(pCurrentFrame->IsInMask(x + minX, y + minY) == false)
				//			resLabelImg.at(x + minX, y + minY) = -1;
				//		else
				//			resLabelImg.at(x + minX, y + minY) = offsetImg.at(x, y) + maskLabelImg.at(x, y);

				//		LabelImg.at(x + minX, y + minY) = resLabelImg.at(x + minX, y + minY);
				//	}
				//}

				//New ADD-将mask改为tile,是要对遮挡进行处理
				for(int y = trueY1; y <= trueY2; y++){
					for(int x = trueX1; x <= trueX2; x++){
							resLabelImg.at(x + minX, y + minY) = offsetImg.at(x, y) + maskLabelImg.at(x, y);

						LabelImg.at(x + minX, y + minY) = resLabelImg.at(x + minX, y + minY);
					}
				}

				//if(paras->GetIfGenerateTempResult() == true)
				//	pCurrentFrame->SaveLabelImg(resLabelImg, CBDMVideoFrame::BO, LayerCount+1);
			//	printf("Time:%.3f s\n",(double)(clock()-tempTime)/CLOCKS_PER_SEC);
			//	tempTime = clock();
            //pCurrentFrame->SetDspImg(bpLabelImg, m_iTrueX1, m_iTrueY1, m_iTrueX2, m_iTrueY2, dspLevel);
			}
			
		}

	    dspSigma = max(0.001,  2 * dspSigma / subSample);
	    paras->SetDspSigma(dspSigma);
	}



	paras->SetDspSigma(backupDspSigma);
	paras->SetDataCostWeight(backupDataCostWeight);
	paras->SetDiscK(backupDiscK);

	//printf("layerCount %d\n", LayerCount + 1);
	pCurrentFrame->SetDspImg(LabelImg, minX, minY, maxX, maxY, LayerCount + 1);

	pCurrentFrame->SaveModel(false);

	if(LSParaWidget::GetInstance()->GetIfGenerateTempResult() == true){
		//pCurrentFrame->InitLabelImgByDspImg(labelImg, dspLevel);
		pCurrentFrame->SaveLabelImg(LabelImg, type, LayerCount);
	}

	//pCurrentFrame->SaveModel(false);

	//pCurrentFrame->InitLabelImgByDspImg(LabelImg, LayerCount + 1);
	//pCurrentFrame->SaveLabelImg(LabelImg, LSVideoFrame::DE, LayerCount + 1);

	pCurrentFrame->SaveDspImg();         //保存depth图像数据

//
	for(int i=0; i<FwFrames.size(); i++)
		FwFrames[i]->Clear();
	for(int i=0; i<BwFrames.size(); i++)
		BwFrames[i]->Clear();
	pCurrentFrame->Clear(); 
}
*/




void LSDepthEstimator::GetSupreRefineDataCost( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, 
	DataCost& outDataCost, ZIntImage& offsetImg, int totalDspLevel, int subSample, ZIntImage& labelImg, LSVideoFrame::LABEL_IMG_TYPE type )
{
	//CParallelManager pm( CSystemParameters::GetInstance()->GetCpuThreads() );
	for(int j=0; j<m_iBlockHeight; ++j){
		LSSuperRefineWorkUnit* pWorkUnit = new LSSuperRefineWorkUnit(pCurrentFrame, FwFrames, BwFrames, labelImg, offsetImg, outDataCost, j, m_iBlockWidth, totalDspLevel, subSample, type);
		pWorkUnit->Execute();
		//pm.EnQueue(pWorkUnit);
	}
	//pm.Run();
}



/*
// 原函数
// need to be added optical flow infomation
void LSDepthEstimator::GetBODataCostAt( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int x, int y, DataCostUnit& dataCosti, 
	int& bestLabel, std::vector<float>& dspV, bool ConsiderDspRang )
{

        //parameters
		LSParaWidget* paras = LSParaWidget::GetInstance();
		float fDspMin, fDspMax;
		pCurrentFrame->GetDspRange(fDspMin, fDspMax);
        //std::cout<<"BO DspRange: fDspMin: "<<fDspMin<< "  fDspMax"<<fDspMax<<std::endl;

        //sigma_d
		float fDspSigma = paras->GetDspSigma() * (fDspMax - fDspMin); // dspSigma=0.03
		float dspSigma2 = fDspSigma * fDspSigma;

		//sigma_c
		float fColorSigma = paras->GetColorSigma();            //5
		float fColorSigma2 = fColorSigma * fColorSigma;

		//datacost weight and penalty
		float fColorMissPenalty = paras->GetMissPenalty();      //10
		float dataCostWeight = paras->GetTrueDataCostWeight();  //20

		//default penalty value
		float defaulValue = fColorSigma / (fColorSigma + fColorMissPenalty * fColorMissPenalty * 9) * 0.01;  //??

		//reference frames
		std::vector<LSVideoFrame*> NearFrames(FwFrames.begin(), FwFrames.end());
		NearFrames.insert(NearFrames.end(), BwFrames.begin(), BwFrames.end());
		int refFrameCount = NearFrames.size();

		int dspLevelCount = dspV.size();
		int minLabel = 0, maxLabel = dspLevelCount - 1;
		if(ConsiderDspRang == true)
		{
			minLabel = pCurrentFrame->m_pMinLabelImg->at(x, y);
			maxLabel = pCurrentFrame->m_pMaxLabelImg->at(x,y);
		}
		
		//assign two sides value
		for(int iLayerIndex = 0; iLayerIndex <= minLabel; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.0001F;
		for(int iLayerIndex = maxLabel; iLayerIndex < dspLevelCount; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.0001F;

//==========================================================================================/
		// need be changed, add optical flow calculation /
//==========================================================================================/


		double MaxLikelihood = 1.0e-6F;
		bestLabel = 0;

		Wml::Vector3d ptWorldCoord;
		Wml::Vector3f CurrentColor, CorrespondingColor;
		double u2, v2, dsp;


		pCurrentFrame->GetColorAt(x, y, CurrentColor);
		std::vector<float>  matchCostCandinate(refFrameCount);
		int halfNeighberSize = refFrameCount / 2;
		halfNeighberSize = min( max(halfNeighberSize, 2), refFrameCount); //make sure that refFrameCount not small than 2

		for(int depthLeveli = minLabel; depthLeveli <= maxLabel; depthLeveli++)
		{

			pCurrentFrame->GetWorldCoordFrmImgCoord(x, y, dspV[depthLeveli], ptWorldCoord);
			//z = 1.0/dspV[depthLeveli];
			for(int Framei=0; Framei< refFrameCount; Framei++)
			{
				NearFrames[Framei]->GetImgCoordFrmWorldCoord(u2, v2, dsp, ptWorldCoord);
				

				// out of range
				if(u2<0 || v2<0 || u2> NearFrames[Framei]->GetImgWidth() -1 || v2 > NearFrames[Framei]->GetImgHeight()-1 || NearFrames[Framei]->IsInMask(u2, v2) == false)
					matchCostCandinate[Framei] = defaulValue;
				


				else{
					//比较双线性插值和直接Int近似值像素位置，得到参考帧上（u2,v2)上的dsp值。
                    // return dsp value nearby (xt,yt) according wheather it in mask (m_pDspImg->at(xt+1, yt+1, 0);)
					float d2 = NearFrames[Framei]->GetDspAt(u2, v2, true);   // 二元线性插值方法计算得到dsparity

					if(d2 < 0){
						matchCostCandinate[Framei] = defaulValue;
						continue;
					}
					if( u2+0.5F < NearFrames[Framei]->GetImgWidth() &&  v2+0.5F < NearFrames[Framei]->GetImgHeight() )
					{
						float d2_INT = NearFrames[Framei]->GetDspAt((int)(u2+0.5F), (int)(v2+0.5F));   //四舍五入方法计算得到disparity

						//dsp = 1.0/z2;
						if(  d2_INT > 0 && fabs(d2_INT - dsp) < fabs(d2 - dsp) )
						{
							d2 = d2_INT;
						}
					}

					// dsp is calculate by (x,y) through casting, while d2 is itself disparity(from m_dspImg)
					float dspDiff = fabs(d2 - dsp);

					float wd = dspSigma2 / (dspSigma2 + dspDiff*dspDiff*2);    //different with the paper

					NearFrames[Framei]->GetColorAt((float)u2, (float)v2, CorrespondingColor);




					float colordist = (fabs(CurrentColor[0] - CorrespondingColor[0]) 
						+ fabs(CurrentColor[1] - CorrespondingColor[1]) 
						+ fabs(CurrentColor[2] - CorrespondingColor[2])) / 3.0F;

					 colordist *= colordist;
					//dataCosti[depthLeveli] += fColorSigma / (fColorSigma + colordist);
					float wc = fColorSigma2/(fColorSigma2 + colordist*colordist);

					matchCostCandinate[Framei] = max(1e-6, wc * wd);
				}
			}

			// get average matchCost
			dataCosti[depthLeveli] = GetValue(matchCostCandinate, FwFrames.size(), BwFrames.size());

			if(dataCosti[depthLeveli] > MaxLikelihood)
			{
				MaxLikelihood = dataCosti[depthLeveli];
				bestLabel = depthLeveli;
			}
		}

		//============================================================================================/
		//==== need be changed, add optical flow calculation =====/
		//=== deal with occulusion    ===/
		//============================================================================================/

		//Normalize
		float maxCost = 1e-6F;
		for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
		{
			dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;
			maxCost = max(maxCost,dataCosti[depthLeveli]);
		}

		for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++)
		{
			dataCosti[depthLeveli] /= maxCost;
			dataCosti[depthLeveli] *= dataCostWeight;
		}

		//归一化（wc*wd越大，对应的DataCost越小）

}
*/


//按照孔相澧BO改正
void LSDepthEstimator::GetBODataCostAt( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int x, int y, DataCostUnit& dataCosti, 
	int& bestLabel, std::vector<float>& dspV, bool ConsiderDspRang )
{

	//parameters
	LSParaWidget* paras = LSParaWidget::GetInstance();
	float fDspMin, fDspMax;
	pCurrentFrame->GetDspRange(fDspMin, fDspMax);
	//std::cout<<"BO DspRange: fDspMin: "<<fDspMin<< "  fDspMax"<<fDspMax<<std::endl;

	//sigma_d
	float fDspSigma = paras->GetDspSigma() * (fDspMax - fDspMin); // dspSigma=0.03
	float dspSigma2 = fDspSigma * fDspSigma;

	//sigma_c
	float fColorSigma = paras->GetColorSigma();            //10

	//datacost weight and penalty
	float fColorMissPenalty = paras->GetMissPenalty();      //20
	float dataCostWeight = paras->GetTrueDataCostWeight();  //20

	//default penalty value
	//float defaulValue = fColorSigma / (fColorSigma + fColorMissPenalty * fColorMissPenalty * 9) * 0.01;  //??

	//reference frames
	std::vector<LSVideoFrame*> NearFrames(FwFrames.begin(), FwFrames.end());
	NearFrames.insert(NearFrames.end(), BwFrames.begin(), BwFrames.end());
	int refFrameCount = NearFrames.size();

	int dspLevelCount = dspV.size();
//	int minLabel = 0, maxLabel = dspLevelCount - 1;
/* 去掉VisualHull约束
	if(ConsiderDspRang == true)
	{
		minLabel = pCurrentFrame->m_pMinLabelImg->at(x, y);
		maxLabel = pCurrentFrame->m_pMaxLabelImg->at(x,y);
	}

	//assign two sides value
	for(int iLayerIndex = 0; iLayerIndex <= minLabel; iLayerIndex++)
		dataCosti[iLayerIndex] = 0.0001F;
	for(int iLayerIndex = maxLabel; iLayerIndex < dspLevelCount; iLayerIndex++)
		dataCosti[iLayerIndex] = 0.0001F;
*/
	//==========================================================================================/
	// need be changed, add optical flow calculation /
	//==========================================================================================/


	double MaxLikelihood = 1.0e-4F;
	bestLabel = 0;

	Wml::Vector3d ptWorldCoord;
	Wml::Vector3f CurrentColor, CorrespondingColor;
	double u2, v2, dsp;


	pCurrentFrame->GetColorAt(x, y, CurrentColor);
	std::vector<float>  matchCostCandinate(refFrameCount);
	int halfNeighberSize = refFrameCount / 2;
	halfNeighberSize = min( max(halfNeighberSize, 2), refFrameCount); //make sure that refFrameCount not small than 2

	for(int depthLeveli = 0; depthLeveli <dspV.size(); depthLeveli++)
	{

		pCurrentFrame->GetWorldCoordFrmImgCoord(x, y, dspV[depthLeveli], ptWorldCoord);
		//z = 1.0/dspV[depthLeveli];
		for(int Framei=0; Framei< refFrameCount; Framei++)
		{
			NearFrames[Framei]->GetImgCoordFrmWorldCoord(u2, v2, dsp, ptWorldCoord);


			// out of range
			//if(u2<0 || v2<0 || u2> NearFrames[Framei]->GetImgWidth() -1 || v2 > NearFrames[Framei]->GetImgHeight()-1 || NearFrames[Framei]->IsInMask(u2, v2) == false)
			if(u2<0 || v2<0 || u2>= NearFrames[Framei]->GetImgWidth() -1 || v2 >= NearFrames[Framei]->GetImgHeight()-1)
			{
				const float visVal = 1e-2f;
				matchCostCandinate[Framei] = fColorSigma / (fColorMissPenalty + fColorSigma) * visVal;
			}

			else{

				NearFrames[Framei]->GetColorAt((float)u2, (float)v2, CorrespondingColor);
				//use SAD for difference
				float val = 0;
				for(int i=0; i<3; ++i){
					val += min<float>(20,fabs(CurrentColor[i] - CorrespondingColor[i]));
				}
				const double costVal =val/3;
                

				// return dsp value nearby (xt,yt) according wheather it in mask (m_pDspImg->at(xt+1, yt+1, 0);)
				//float d2 = NearFrames[Framei]->GetDspAt(u2, v2, true);   // 二元线性插值方法计算得到dsparity
				const double neighDsp =NearFrames[Framei]->m_pDspImg->at((int)(u2 + 0.49f),(int)(v2 + 0.49f));    //需要验证是否取到正确的dsp值
				const double dif = neighDsp - dsp;
				const double visWeight = dspSigma2 / (dspSigma2 + dif * dif *2);
				const double colorWeight = fColorSigma / (costVal + fColorSigma);

				matchCostCandinate[Framei]= max(1e-4,visWeight * colorWeight);	
			 
			}
		}

		/*  DataCost  这一步暂时不求解BestLabel.
		// get average matchCost
		dataCosti[depthLeveli] = GetValue(matchCostCandinate, FwFrames.size(), BwFrames.size());

		if(dataCosti[depthLeveli] > MaxLikelihood)
		{
			MaxLikelihood = dataCosti[depthLeveli];
			bestLabel = depthLeveli;
		}
		*/

		if(matchCostCandinate.size()>halfNeighberSize)
			std::nth_element(matchCostCandinate.begin(),
			matchCostCandinate.begin() + halfNeighberSize,
			matchCostCandinate.end(),std::greater<float>() );  //按从大到小递减排序
		float sum = 0.0;
		for(int iCandidate = 0;iCandidate<halfNeighberSize;++iCandidate)
			sum+=matchCostCandinate.at(iCandidate);
		dataCosti[depthLeveli] = sum;


		  //取最大相似度
		//if(dataCosti[depthLeveli] > MaxLikelihood)
		//{
		//	MaxLikelihood = dataCosti[depthLeveli];
	 //      //bestLabel = depthLeveli;   // ORG
	 //     // std::cout<<"bestLabel: "<<bestLabel<<std::endl;	
		//
		//}
	}



/* ORG
	 for(int depthLeveli=0;depthLeveli<dspV.size();depthLeveli++)
	 {
		 MaxLikelihood=max(MaxLikelihood,dataCosti[depthLeveli]);
	 }

	   bestLabel=(int)(100*(MaxLikelihood/2.5));
   //	    std::cout<<"bestLabel: "<<bestLabel<<std::endl;
	//============================================================================================/
	//==== need be changed, add optical flow calculation =====/
	//=== deal with occulusion    ===/
	//============================================================================================/

	//Normalize
	float maxCost = 1e-4F;
	for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
	{
		const float tmp =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;
		dataCosti[depthLeveli]=tmp;
		maxCost = max(maxCost,tmp);
	}
*/
//NEW ADD
	const float minVal = 1e-4f;
	//NormalizeResult( minVal,matchCost );  //ORG
	
	//取得最大的相似度
	float normScale = minVal;
	for(int iLayerIndex=0; iLayerIndex<dspLevelCount; ++iLayerIndex){		
		normScale = max(normScale,dataCosti[iLayerIndex]);
	}

	//std::cout<<"normScale: "<<normScale<<std::endl;
	bestLabel=(int)(100*(normScale/2.5));


	//计算不相似度，并记录最大的不相似度
	float maxCost = minVal;
	const float invNormScale = 1.0 / normScale;
	for(int iLayerIndex=0; iLayerIndex<dspLevelCount;iLayerIndex++)
	{
		const float tmp = 1.0 - dataCosti[iLayerIndex] * invNormScale;
		dataCosti[iLayerIndex] = 		tmp;
		maxCost = max(tmp,maxCost);
	}




	for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++)
	{
		dataCosti[depthLeveli] /= maxCost;
	//	dataCosti[depthLeveli] *= dataCostWeight;  //orignal 可能要加上
	}
	
	//归一化（wc*wd越大，对应的DataCost越小）
}


/*
//之前新修改函数
// need to be added optical flow infomation
void LSDepthEstimator::GetBODataCostAt( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int x, int y, DataCostUnit& dataCosti, 
	int& bestLabel, std::vector<float>& dspV, bool ConsiderDspRang )
{
		//parameters
		LSParaWidget* paras = LSParaWidget::GetInstance();
		float fDspMin, fDspMax;
		pCurrentFrame->GetDspRange(fDspMin, fDspMax);
		//std::cout<<"BO DspRange: fDspMin: "<<fDspMin<< "  fDspMax"<<fDspMax<<std::endl;

		//sigma_d
		float fDspSigma = paras->GetDspSigma() * (fDspMax - fDspMin); // dspSigma=0.03
		float dspSigma2 = fDspSigma * fDspSigma;

		//sigma_c
		float fColorSigma = paras->GetColorSigma();            //5
		float fColorSigma2 = fColorSigma * fColorSigma;

		//datacost weight and penalty
		float fColorMissPenalty = paras->GetMissPenalty();      //10
		float dataCostWeight = paras->GetTrueDataCostWeight();  //20

		//default penalty value
		float defaulValue = fColorSigma / (fColorSigma + fColorMissPenalty * fColorMissPenalty * 9) * 0.01;  //??

		//reference frames
		std::vector<LSVideoFrame*> NearFrames(FwFrames.begin(), FwFrames.end());
		NearFrames.insert(NearFrames.end(), BwFrames.begin(), BwFrames.end());
		int refFrameCount = NearFrames.size();

		int dspLevelCount = dspV.size();
		int minLabel = 0, maxLabel = dspLevelCount - 1;
		if(ConsiderDspRang == true)
		{
			minLabel = pCurrentFrame->m_pMinLabelImg->at(x, y);
			maxLabel = pCurrentFrame->m_pMaxLabelImg->at(x,y);
		}

		//assign two sides value
		for(int iLayerIndex = 0; iLayerIndex <= minLabel; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.0001F;
		for(int iLayerIndex = maxLabel; iLayerIndex < dspLevelCount; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.0001F;

		//==========================================================================================//
		//=== need be changed, add optical flow calculation ===/
		//===========================================================================================//

		double MaxLikelihood = 1.0e-6F;
		bestLabel = 0;

		Wml::Vector3d ptWorldCoord;
		Wml::Vector3f CurrentColor, CorrespondingColor;
		double u2, v2, dsp;


		//--------Add----------//
		Wml::Vector3d ptWorldCoordx;
		Wml::Vector3f CorrespondingColorx;
		double u2x, v2x, dspx;


		pCurrentFrame->GetColorAt(x, y, CurrentColor);
		std::vector<float>  matchCostCandinate(refFrameCount);

		int halfNeighberSize = refFrameCount / 2;
		halfNeighberSize = min( max(halfNeighberSize, 2), refFrameCount); //make sure that refFrameCount not small than 2

		for(int depthLeveli = minLabel; depthLeveli <= maxLabel; depthLeveli++)
		{

			pCurrentFrame->GetWorldCoordFrmImgCoord(x, y, dspV[depthLeveli], ptWorldCoord);
			//z = 1.0/dspV[depthLeveli];
			for(int Framei=0; Framei< refFrameCount; Framei++)
			{   //iFrame start
				NearFrames[Framei]->GetImgCoordFrmWorldCoord(u2, v2, dsp, ptWorldCoord);

				//right image out of range
				if(u2<0 || v2<0 || u2> NearFrames[Framei]->GetImgWidth() -1 || v2 > NearFrames[Framei]->GetImgHeight()-1 || NearFrames[Framei]->IsInMask(u2, v2) == false)
					matchCostCandinate[Framei] = defaulValue;

				else
				{    //else1 start

					// return dsp value nearby (xt,yt) according wheather it in mask (m_pDspImg->at(xt+1, yt+1, 0);)
					float d2 = NearFrames[Framei]->GetDspAt(u2, v2, true);

					if(d2 < 0)
					{
						matchCostCandinate[Framei] = defaulValue;
						continue;
					}

					if( u2+0.5F < NearFrames[Framei]->GetImgWidth() &&  v2+0.5F < NearFrames[Framei]->GetImgHeight() )
					{  // if1 start
						 
						float d2_INT = NearFrames[Framei]->GetDspAt((int)(u2+0.5F), (int)(v2+0.5F));
						//dsp = 1.0/z2;
						if(  d2_INT > 0 && fabs(d2_INT - dsp) < fabs(d2 - dsp) )
						{ // if2 start
							d2 = d2_INT;

							NearFrames[Framei]->GetWorldCoordFrmImgCoord(u2,v2,d2,ptWorldCoordx);
							pCurrentFrame->GetImgCoordFrmWorldCoord(u2x,v2x,dspx,ptWorldCoordx);

							if(u2x<0 || v2x<0 || u2x> pCurrentFrame->GetImgWidth() -1 || v2x > pCurrentFrame->GetImgHeight()-1 || pCurrentFrame->IsInMask(u2x, v2x) == false)
								matchCostCandinate[Framei] = defaulValue;
							else
							{   //else2 start
								float d2x=pCurrentFrame->GetDspAt(u2x,v2x,true);
						        if(d2x<0)
								{
									matchCostCandinate[Framei] = defaulValue;
									continue;
								}

								if(u2x+0.5F < pCurrentFrame->GetImgWidth() &&  v2x+0.5F < pCurrentFrame->GetImgHeight())

								{
									float d2x_INT = pCurrentFrame->GetDspAt((int)(u2x+0.5F), (int)(v2x+0.5F));
									//dsp = 1.0/z2;
									if(  d2x_INT > 0 && fabs(d2x_INT - dspV[depthLeveli]) < fabs(d2x - dspV[depthLeveli]) )
										d2x=d2x_INT;
								}

								float dspDiff = fabs(d2x - dspV[depthLeveli]);
								float wd = dspSigma2 / (dspSigma2 + dspDiff*dspDiff*2);    //different with the paper
								pCurrentFrame->GetColorAt((float)u2x, (float)v2x, CorrespondingColorx);
								float colordist = (fabs(CurrentColor[0] - CorrespondingColorx[0]) 
									+ fabs(CurrentColor[1] - CorrespondingColorx[1]) 
									+ fabs(CurrentColor[2] - CorrespondingColorx[2])) / 3.0F;

								colordist *= colordist;
								float wc = fColorSigma2/(fColorSigma2 + colordist*colordist);

								matchCostCandinate[Framei] = max(1e-6, wc * wd);
						  		
					
								////get wc(color term)
								//float colordist2 = ((CurrentColor[0] - CorrespondingColor[0])* (CurrentColor[0] - CorrespondingColor[0])
								//	+ (CurrentColor[1] - CorrespondingColor[1])*(CurrentColor[1] -CorrespondingColor[1]) 
								//	+ (CurrentColor[2] - CorrespondingColor[2])*(CurrentColor[2] - CorrespondingColor[2])) / 3.0f;
								//float colordist=sqrt(colordist2);

								//float wc=fColorSigma/(fColorSigma+colordist2);

								//// disparity term
								//float dspdiff = fabs(d2x - dspV[depthLeveli]);
								//float wd=exp(-(dspdiff/(2*dspSigma2)));
								//
								//// distance term
								//float disdiff2=(u2x-x)*(u2x-x)+(v2x-y)*(v2x-y);
								//float disdiff=sqrt(disdiff2);
								//float wx=exp(-disdiff/(2*fColorSigma2));

							 //   matchCostCandinate[Framei] = max(1e-6, wc * wd * wx);
						

							  }  //else2 end

							}  //if2 end

						}   // if1 end
					

			   }// else1 end

			}//iFrame end


			// get average matchCost
			dataCosti[depthLeveli] = GetValue(matchCostCandinate, FwFrames.size(), BwFrames.size());

			if(dataCosti[depthLeveli] > MaxLikelihood)
			{
				MaxLikelihood = dataCosti[depthLeveli];
				bestLabel = depthLeveli;
			}
		

		}// dspLeveli



		//========================================================================================/
		// need be changed, add optical flow calculation //
		// deal with occulusion    //
		//=========================================================================================//

		//Normalize
		float maxCost = 1e-6F;
		for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
		{
			dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;
			maxCost = max(maxCost,dataCosti[depthLeveli]);
		}

		for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++)
		{
			dataCosti[depthLeveli] /= maxCost;
			dataCosti[depthLeveli] *= dataCostWeight;
		}

		//归一化（wc*wd越大，对应的DataCost越小）

	
}
*/

// 原函数
//new add---Optical flow- for temporal consistence
void LSDepthEstimator::GetOptDataCostAt( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int x, int y, DataCostUnit& dataCosti, 
	int& bestLabel, std::vector<float>& dspV, bool ConsiderDspRang )
{

		//parameters
		LSParaWidget* paras = LSParaWidget::GetInstance();
		float fDspMin, fDspMax;
		pCurrentFrame->GetDspRange(fDspMin, fDspMax);
	//	std::cout<<"Opt DspRange: fDspMin: "<<fDspMin<< "  fDspMax"<<fDspMax<<std::endl;

		//sigma_d
		float fDspSigma = paras->GetDspSigma() * (fDspMax - fDspMin); // dspSigma=0.03
		float dspSigma2 = fDspSigma * fDspSigma;

		//sigma_c
		float fColorSigma = paras->GetColorSigma();            //5
		float fColorSigma2 = fColorSigma * fColorSigma;

		//datacost weight and penalty
		float fColorMissPenalty = paras->GetMissPenalty();      //10
		float dataCostWeight = paras->GetTrueDataCostWeight();  //20

		//default penalty value
		float defaulValue = fColorSigma / (fColorSigma + fColorMissPenalty * fColorMissPenalty * 9) * 0.01;  //  default datacost penalty

		//reference frames
		std::vector<LSVideoFrame*> NearFrames(FwFrames.begin(), FwFrames.end());
		NearFrames.insert(NearFrames.end(), BwFrames.begin(), BwFrames.end());
		//int refFrameCount = NearFrames.size();

//================================================================================================================//

		//reference frames parameters ( for different cams in the same frames)
		int camRefStart =paras->GetCamRefStart();
		int camRefStep = paras->GetCamRefStep(); 
		int camRefCount =paras->GetCamRefCount();    


		//reference frames parameters ( for different frames in the same cam)
		int frmRefStart = paras->GetFrmRefStart();
		int frmRefStep = paras->GetFrmRefStep(); 
		int frmRefCount = paras->GetFrmRefCount();   
		     
		
		int StartFrameIndex=LSRunConfigDlg::GetInstance()->GetStartFrame();
		int EndFrameIndex=LSRunConfigDlg::GetInstance()->GetEndFrame();

		//std::cout<<"StartFrameIndex:"<<StartFrameIndex<<" ";
		//std::cout<<"EndFrameIndex:"<<EndFrameIndex<<std::endl;

		//std::cout<<"pCurrentFrame->m_iCameraId:"<<pCurrentFrame->m_iCameraId<<",pCurrentFrame->m_iFrameId:"<<pCurrentFrame->m_iFrameId<<std::endl;


		std::map<std::pair<int,int>,LSVideoFrame*> mOpt;

		for(int index=0;index<NearFrames.size();index++)
		{
			mOpt.insert(std::make_pair(std::make_pair(NearFrames[index]->m_iCameraId,NearFrames[index]->m_iFrameId),NearFrames[index]));
		}

		
		//for(int index=0;index<NearFrames.size();index++)
		//{
		//	std::map<std::pair<int,int>,LSVideoFrame*>::iterator iter = mOpt.find(std::make_pair(NearFrames[index]->m_iCameraId,NearFrames[index]->m_iFrameId));
			//std::cout<<"camId:"<<iter->second->m_iCameraId<<" "<<"frmId:"<<iter->second->m_iFrameId<<std::endl;
		//}
	

	    // 这两个数据结构尚未释放存储空间，可能导致内存泄露

		std::pair<int,int> refPair;
		std::vector<std::pair<int,int>> refPairVec;


		int iCamCount=0;
		int iFrmCount=0;

		// cam forward
		for(int camIndex = pCurrentFrame->m_iCameraId + camRefStart; iCamCount <camRefCount; camIndex += camRefStep)
		{  
			if(camIndex>=CamNum)camIndex=camIndex%CamNum;  //form a circle(cams)

			//frame forward
			iFrmCount=0;
			for(int frmIndex = pCurrentFrame->m_iFrameId + frmRefStart; frmIndex <= EndFrameIndex && iFrmCount < frmRefCount; frmIndex+= frmRefStep)
			{
			
                refPair.first=camIndex;
				refPair.second=frmIndex;
				refPairVec.push_back(refPair);
				
				iFrmCount++;
			}

			//frame backward
			iFrmCount=0;
			//for(int frmIndex = pCurrentFrame->m_iFrameId -frmRefStart; frmIndex >= StartFrameIndex && iFrmCount < frmRefCount; frmIndex-= frmRefCount)
			for(int frmIndex = pCurrentFrame->m_iFrameId -frmRefStart; frmIndex >= StartFrameIndex && iFrmCount < frmRefCount; frmIndex-= frmRefStep)
			{

				//FwFrameIndex.push_back(std::pair<int, int>(camIndex, frmIndex));
				refPair.first=camIndex;
				refPair.second=frmIndex;
				refPairVec.push_back(refPair);

				iFrmCount++;
				//iCount++;
			}

			iCamCount++;
		}



		//get reference frames backward
		iCamCount=0;
		iFrmCount=0;

		//cam backward
		for(int camIndex = pCurrentFrame->m_iCameraId - camRefStart; iCamCount < camRefCount; camIndex -= camRefStep)
		{ 
			if(camIndex<0)camIndex=(camIndex+CamNum)%CamNum;   //form a circle

			//frame forward
			iFrmCount=0;
			for(int frmIndex = pCurrentFrame->m_iFrameId+ frmRefStart; frmIndex <= EndFrameIndex && iFrmCount < frmRefCount; frmIndex+= frmRefStep)
			{

			
                refPair.first=camIndex;
				refPair.second=frmIndex;
				refPairVec.push_back(refPair);
			    iFrmCount++;
			}


			//frame backward
			iFrmCount=0;
			for(int frmIndex = pCurrentFrame->m_iFrameId -frmRefStart; frmIndex >= StartFrameIndex && iFrmCount < frmRefCount; frmIndex-= frmRefStep)
			{

                refPair.first=camIndex;
				refPair.second=frmIndex;
				refPairVec.push_back(refPair);
				iFrmCount++;
			}

			iCamCount++;
		}
		
		int refFrameCount = refPairVec.size();

		int iWidth=LSVideoFrame::GetImgWidth();
		int iHeight=LSVideoFrame::GetImgHeight();

		/*
		for(int i=0;i<refFrameCount;i++)
		{
			std::cout<<refPairVec[i].first<<","<<refPairVec[i].second<<std::endl;
		}
		*/

	// std::cout<<"x:"<<x<<",y"<<y<<",optValue:"<<optValue<<std::endl;

//=========================================================================================================//

		//dsp level count
		int dspLevelCount = dspV.size();

		//set min label and max label
		int minLabel = 0, maxLabel = dspLevelCount - 1;
		if(ConsiderDspRang == true)
		{
			minLabel = pCurrentFrame->m_pMinLabelImg->at(x,y);
			maxLabel = pCurrentFrame->m_pMaxLabelImg->at(x,y);
		}

		//assign two sides value for datacosti
		for(int iLayerIndex = 0; iLayerIndex <= minLabel; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.0001F;
		for(int iLayerIndex = maxLabel; iLayerIndex < dspLevelCount; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.0001F;

		//==========================================================================================//
		//=== optical flow calculation,for temporal consistence ===/
		//===========================================================================================//

		double MaxLikelihood = 1.0e-6F;
		bestLabel = 0;

		Wml::Vector3d ptWorldCoord;
		Wml::Vector3f CurrentColor, CorrespondingColor;
	    Wml::Vector3f CurrentColorxy2;

		double x1, y1, dsp1;   //for iter1

	    double x2, y2;// dsp2;   //for iter2

		double x2t,y2t,dsp2t; //casting by iter3

		double x3,y3;//dsp3;     //for iter3


		//Int for save coordinate
		//int x1_int,y1_int;
		//int x2_int,y2_int;
	  // int x3_int,y3_int;

		Wml::Vector2d foreOptVal1,foreOptVal2,backOptVal1,backOptVal2;

		//--------Add----------//
		Wml::Vector3d ptWorldCoordx;
		Wml::Vector3f CorrespondingColorx;
		//double u2x, v2x, dspx;

		pCurrentFrame->GetColorAt(x, y, CurrentColor);

		std::vector<float>  matchCostCandinate(refFrameCount);

		int halfNeighberSize = refFrameCount / 2;
		halfNeighberSize = min( max(halfNeighberSize, 2), refFrameCount); //make sure that half refFrameCount not small than 2


		for(int depthLeveli = minLabel; depthLeveli <= maxLabel; depthLeveli++)
		{

			pCurrentFrame->GetWorldCoordFrmImgCoord(x, y, dspV[depthLeveli], ptWorldCoord);
			//z = 1.0/dspV[depthLeveli];
			
			std::map<std::pair<int,int>,LSVideoFrame*>::iterator iter1,iter2,iter3;    //是否需要释放内存空间 

			for(int Framei=0; Framei< refFrameCount; Framei++)
			{   //iFrame start

		        //reference camId, current frameId
				iter1=mOpt.find(std::make_pair(refPairVec[Framei].first,pCurrentFrame->m_iFrameId));

				iter1->second->GetImgCoordFrmWorldCoord(x1, y1, dsp1, ptWorldCoord);


				//right image out of range	
				if(x1<0 || y1<0 || x1> iter1->second->GetImgWidth() -1 || y1 > iter1->second->GetImgHeight()-1 || iter1->second->IsInMask(x1, y1) == false)
				matchCostCandinate[Framei] = defaulValue;

               
				else
				{    //else1 start
                 
					// current camId, reference frameId
					iter2=mOpt.find(std::make_pair(pCurrentFrame->m_iCameraId,refPairVec[Framei].second));
					
					//reference camId, reference frameId
					iter3=mOpt.find(std::make_pair(refPairVec[Framei].first,refPairVec[Framei].second));

					
				   // int interval=(pCurrentFrame->m_iFrameId)-(iter2->second->m_iFrameId);   //原代码
			        int interval=(iter2->second->m_iFrameId)-(pCurrentFrame->m_iFrameId);   // bug is here          
				    
					x2=x;
					y2=y;

					x3=x1;
					y3=y1;

				 //std::cout<<"x1: "<<x1<<"，y1:"<<y1<<std::endl;
                   // 这两个坐标应该由单线性插值获得(已确认为double类型）


		           
				    if(interval>0)   //if-else start
					{

						   double xTmp1, yTmp1; //track from current to iter2
						
						   double xTmp2, yTmp2; //track from iter1 to iter3
						   
						   
						    for(int frameId=(pCurrentFrame->m_iFrameId);frameId<(iter2->second->m_iFrameId);frameId++)
							{
							 
					          // xTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pForeOptImg->at(x2,y2,0);
							   
							 //  yTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pForeOptImg->at(x2,y2,1);
								
								LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->GetForeOptAt(x2,y2,foreOptVal1);
								xTmp1=foreOptVal1[0];
								yTmp1=foreOptVal1[1];


							   x2+=xTmp1;
							   y2+=yTmp1;

							//   xTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pForeOptImg->at(x3,y3,0);
							//   yTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pForeOptImg->at(x3,y3,1);
							     
							    LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->GetForeOptAt(x3,y3,foreOptVal2);
								xTmp2=foreOptVal2[0];
								yTmp2=foreOptVal2[1];

							   x3+=xTmp2;
							   y3+=yTmp2;

							}

					   }
					   else  //(interval<0)
					   {
						   double xTmp1, yTmp1; //track from current to iter2

						   double xTmp2, yTmp2; //track from iter1 to iter3


						   for(int frameId=(pCurrentFrame->m_iFrameId);frameId>(iter2->second->m_iFrameId);frameId--)
						   {
					
							  // xTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pBackOptImg->at(x2,y2,0);

							 //  yTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pBackOptImg->at(x2,y2,1);

							   LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->GetBackOptAt(x2,y2,backOptVal1);
							   xTmp1=backOptVal1[0];
							   yTmp1=backOptVal1[1];

							   x2+=xTmp1;
							   y2+=yTmp1;

							  // xTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pBackOptImg->at(x3,y3,0);
							  // yTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pBackOptImg->at(x3,y3,1);
							   LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->GetBackOptAt(x3,y3,backOptVal2);
							   xTmp2=backOptVal2[0];
							   yTmp2=backOptVal2[1];

							   x3+=xTmp2;
							   y3+=yTmp2;

						   }


					   }  //if-else over
					 
				

					   
					//it2 image coord out of range
					if(x2<0 || y2<0 || x2> iter2->second->GetImgWidth() -1 || y2 > iter2->second->GetImgHeight()-1 || iter2->second->IsInMask(x2, y2) == false)
						matchCostCandinate[Framei] = defaulValue;
					
					//it3 image coord out of range
					else if(x3<0 || y3<0 || x3> iter3->second->GetImgWidth() -1 || y3 > iter3->second->GetImgHeight()-1 || iter3->second->IsInMask(x3, y3) == false)
						matchCostCandinate[Framei] = defaulValue;
					
					else
					{

						float dsp3=iter3->second->GetDspAt(x3,y3,true);
						if(dsp3 < 0)
						{
							matchCostCandinate[Framei] = defaulValue;
							continue;
						}
						if( x3+0.5F < iter3->second->GetImgWidth() &&  y3+0.5F <iter3->second->GetImgHeight() )
						{  // if1 start

							float d3_INT = iter3->second->GetDspAt((int)(x3+0.5F), (int)(y3+0.5F));     //四舍五入与双线性插值的比较（可要可不要）

							if(d3_INT>0&&fabs(d3_INT-dsp1)<fabs(dsp3-dsp1))
							{
								dsp3=d3_INT;

							    iter3->second->GetWorldCoordFrmImgCoord(x3,y3,dsp3,ptWorldCoordx);

								iter2->second->GetImgCoordFrmWorldCoord(x2t,y2t,dsp2t,ptWorldCoordx);

								if(x2t<0 || y2t<0 || x2t> iter2->second->GetImgWidth() -1 || y2t > iter2->second->GetImgHeight()-1 || iter2->second->IsInMask(x2t, y2t) == false)
									matchCostCandinate[Framei] = defaulValue;

								else
								{
                                    float dsp2=iter2->second->GetDspAt(x2t,y2t,true);  // by casting

                                    float dxy2=iter2->second->GetDspAt(x2,y2,true);    // by tracking

									if(dsp2<0||dxy2<0)
									{
										matchCostCandinate[Framei] = defaulValue;
										continue;
									}

									if(x2t+0.5F < iter2->second->GetImgWidth() && y2t+0.5F <iter2->second->GetImgHeight())

									{
										float d2_INT = iter2->second->GetDspAt((int)(x2t+0.5F), (int)(y2t+0.5F));
										//dsp = 1.0/z2;
										if(  d2_INT > 0 && fabs(d2_INT - dxy2) < fabs(dsp2 - dxy2) )
											dsp2=d2_INT;
									}



									float dspDiff = fabs(dsp2 - dxy2);
									float wd = dspSigma2 / (dspSigma2 + dspDiff*dspDiff*2);    //different with the paper
									

									iter2->second->GetColorAt((float)x2,(float)y2,CurrentColorxy2);
									iter2->second->GetColorAt((float)x2t, (float)y2t, CorrespondingColorx);

									float colordist = (fabs(CurrentColorxy2[0] - CorrespondingColorx[0]) 
										+ fabs(CurrentColorxy2[1] - CorrespondingColorx[1]) 
										+ fabs(CurrentColorxy2[2] - CorrespondingColorx[2])) / 3.0F;

									colordist *= colordist;

									float wc = fColorSigma2/(fColorSigma2 + colordist*colordist);

									matchCostCandinate[Framei] = max<float>(1e-6, wc * wd);


								}

							}



						}
                   

						//iter2->GetImgCoordFrmWorldCoord(u2x,v2x,dspx,ptWorldCoordx);

					}
					

			   }// else1 end
               
			  

			}//iFrame end


			// get average matchCost
		//	dataCosti[depthLeveli] = GetValue(matchCostCandinate, FwFrames.size(), BwFrames.size());  //May have problem!
		
			float sumMatchCost=0.0;
			for(int i=0;i<matchCostCandinate.size();i++)
			{
				sumMatchCost+=matchCostCandinate[i];

			}
			dataCosti[depthLeveli]=sumMatchCost/matchCostCandinate.size();
		


			if(dataCosti[depthLeveli] > MaxLikelihood)
			{
				MaxLikelihood = dataCosti[depthLeveli];
				bestLabel = depthLeveli;
			}
		

		}// dspLeveli

		//===========================================================================================//
		// need be changed, add optical flow calculation //
		// deal with occulusion    //
		//===========================================================================================//

		//Normalize
		float maxCost = 1e-6F;
		for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
		{
			dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;
			maxCost = max(maxCost,dataCosti[depthLeveli]);
		}

		for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++)
		{
			dataCosti[depthLeveli] /= maxCost;
			dataCosti[depthLeveli] *= dataCostWeight;
		}

		//归一化（wc*wd越大，对应的DataCost越小）
}


/* 新修改函数
void LSDepthEstimator::GetOptDataCostAt( std::vector<LSVideoFrame*>& FwFrames,std::vector<LSVideoFrame*>& BwFrames, LSVideoFrame* pCurrentFrame, int x, int y, DataCostUnit& dataCosti, 
	int& bestLabel, std::vector<float>& dspV, bool ConsiderDspRang )
{

		//parameters
		LSParaWidget* paras = LSParaWidget::GetInstance();
		float fDspMin, fDspMax;
		pCurrentFrame->GetDspRange(fDspMin, fDspMax);
		//std::cout<<"Opt DspRange: fDspMin: "<<fDspMin<< "  fDspMax"<<fDspMax<<std::endl;

		//sigma_d
		float fDspSigma = paras->GetDspSigma() * (fDspMax - fDspMin); // dspSigma=0.03
		float dspSigma2 = fDspSigma * fDspSigma;

		//sigma_c
		float fColorSigma = paras->GetColorSigma();            //5
		float fColorSigma2 = fColorSigma * fColorSigma;        

		//datacost weight and penalty
		float fColorMissPenalty = paras->GetMissPenalty();      //10
		float dataCostWeight = paras->GetTrueDataCostWeight();  //20

		//default penalty value
		float defaulValue = fColorSigma / (fColorSigma + fColorMissPenalty * fColorMissPenalty * 9) * 0.01;  //  default datacost penalty

		//reference frames
		std::vector<LSVideoFrame*> NearFrames(FwFrames.begin(), FwFrames.end());
		NearFrames.insert(NearFrames.end(), BwFrames.begin(), BwFrames.end());
		//int refFrameCount = NearFrames.size();

//================================================================================================================//
		//要对 NearFrames里面的相机号和帧号索引进行分类，用新的数据结构，如Map等。

		//reference frames parameters ( for different cams in the same frames)
		int camRefStart =paras->GetCamRefStart();
		int camRefStep = paras->GetCamRefStep(); 
		int camRefCount =paras->GetCamRefCount();    


		//reference frames parameters ( for different frames in the same cam)
		int frmRefStart = paras->GetFrmRefStart();
		int frmRefStep = paras->GetFrmRefStep(); 
		int frmRefCount = paras->GetFrmRefCount();   
		     
		
		int StartFrameIndex=LSRunConfigDlg::GetInstance()->GetStartFrame();
		int EndFrameIndex=LSRunConfigDlg::GetInstance()->GetEndFrame();

		//std::cout<<"StartFrameIndex:"<<StartFrameIndex<<" ";
		//std::cout<<"EndFrameIndex:"<<EndFrameIndex<<std::endl;

		//std::cout<<"pCurrentFrame->m_iCameraId:"<<pCurrentFrame->m_iCameraId<<",pCurrentFrame->m_iFrameId:"<<pCurrentFrame->m_iFrameId<<std::endl;

		std::map<std::pair<int,int>,LSVideoFrame*> mOpt;

		for(int index=0;index<NearFrames.size();index++)
		{
			mOpt.insert(std::make_pair(std::make_pair(NearFrames[index]->m_iCameraId,NearFrames[index]->m_iFrameId),NearFrames[index]));
		}


		std::pair<int,int> refPair;
		std::vector<std::pair<int,int>> refPairVec;


		int iCamCount=0;
		int iFrmCount=0;

		// cam forward
		for(int camIndex = pCurrentFrame->m_iCameraId + camRefStart; iCamCount <camRefCount; camIndex += camRefStep)
		{  
			if(camIndex>=CamNum)camIndex=camIndex%CamNum;  //form a circle(cams)

			//frame forward
			iFrmCount=0;
			for(int frmIndex = pCurrentFrame->m_iFrameId + frmRefStart; frmIndex <= EndFrameIndex && iFrmCount < frmRefCount; frmIndex+= frmRefStep)
			{
			
                refPair.first=camIndex;
				refPair.second=frmIndex;
				refPairVec.push_back(refPair);
				
				iFrmCount++;
			}

			//frame backward
			iFrmCount=0;
			for(int frmIndex = pCurrentFrame->m_iFrameId -frmRefStart; frmIndex >= StartFrameIndex && iFrmCount < frmRefCount; frmIndex-= frmRefCount)
			{

				//FwFrameIndex.push_back(std::pair<int, int>(camIndex, frmIndex));

				
				refPair.first=camIndex;
				refPair.second=frmIndex;
				refPairVec.push_back(refPair);

				iFrmCount++;
				//iCount++;
			}

			iCamCount++;
		}



		//get reference frames backward
		iCamCount=0;
		iFrmCount=0;

		//cam backward
		for(int camIndex = pCurrentFrame->m_iCameraId - camRefStart; iCamCount < camRefCount; camIndex -= camRefStep)
		{ 
			if(camIndex<0)camIndex=(camIndex+CamNum)%CamNum;   //form a circle

			//frame forward
			iFrmCount=0;
			for(int frmIndex = pCurrentFrame->m_iFrameId+ frmRefStart; frmIndex <= EndFrameIndex && iFrmCount < frmRefCount; frmIndex+= frmRefStep)
			{

			
                refPair.first=camIndex;
				refPair.second=frmIndex;
				refPairVec.push_back(refPair);
			    iFrmCount++;
			}


			//frame backward
			iFrmCount=0;
			for(int frmIndex = pCurrentFrame->m_iFrameId -frmRefStart; frmIndex >= StartFrameIndex && iFrmCount < frmRefCount; frmIndex-= frmRefStep)
			{

				
                refPair.first=camIndex;
				refPair.second=frmIndex;
				refPairVec.push_back(refPair);
				iFrmCount++;
			}

			iCamCount++;
		}

		
		int refFrameCount = refPairVec.size();

		int iWidth=LSVideoFrame::GetImgWidth();
		int iHeight=LSVideoFrame::GetImgHeight();

//=========================================================================================================//


		//dsp level count
		int dspLevelCount = dspV.size();

		//set min label and max label
		int minLabel = 0, maxLabel = dspLevelCount - 1;
		if(ConsiderDspRang == true)
		{
			minLabel = pCurrentFrame->m_pMinLabelImg->at(x,y);
			maxLabel = pCurrentFrame->m_pMaxLabelImg->at(x,y);
		}
	 
		//assign two sides value for datacosti
		for(int iLayerIndex = 0; iLayerIndex <= minLabel; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.0001F;
		for(int iLayerIndex = maxLabel; iLayerIndex < dspLevelCount; iLayerIndex++)
			dataCosti[iLayerIndex] = 0.0001F;

		//==========================================================================================//
		//=== optical flow calculation,for temporal consistence ===/
		//===========================================================================================//

		double MaxLikelihood = 1.0e-6F;
		bestLabel = 0;

		Wml::Vector3d ptWorldCoord;    //between cur and itr1
		Wml::Vector3f CurrentColor, CorrespondingColor; //between cur and itr1
	    Wml::Vector3f CurrentColorxy2;

		double x1, y1, dsp1;   //for iter1 (differ cam, cur frame)

	    double x2, y2;// dsp2;   //for iter2 ( cur cam, differ frame)

		double x2t,y2t,dsp2t; //casting  back by iter3

		double x3,y3;//dsp3;     //for iter3(differ cam, differ frame)


		//--------Add----------//
		Wml::Vector3d ptWorldCoordx;
		Wml::Vector3f CorrespondingColorx;
		//double u2x, v2x, dspx;

		pCurrentFrame->GetColorAt(x, y, CurrentColor);

		std::vector<float>  matchCostCandinate(refFrameCount);

		int halfNeighberSize = refFrameCount / 2;
		halfNeighberSize = min( max(halfNeighberSize, 2), refFrameCount); //make sure that  refFrameCount not small than 2


		for(int depthLeveli = minLabel; depthLeveli <= maxLabel; depthLeveli++)
		{

			pCurrentFrame->GetWorldCoordFrmImgCoord(x, y, dspV[depthLeveli], ptWorldCoord);
			//z = 1.0/dspV[depthLeveli];
			
			
			std::map<std::pair<int,int>,LSVideoFrame*>::iterator iter1,iter2,iter3; 

			for(int Framei=0; Framei< refFrameCount; Framei++)
			{   //iFrame start

				
		        //reference camId, current frameId
				iter1=mOpt.find(std::make_pair(refPairVec[Framei].first,pCurrentFrame->m_iFrameId));

				iter1->second->GetImgCoordFrmWorldCoord(x1, y1, dsp1, ptWorldCoord);


				//right image out of range	
				if(x1<0 || y1<0 || x1> iter1->second->GetImgWidth() -1 || y1 > iter1->second->GetImgHeight()-1 || iter1->second->IsInMask(x1, y1) == false)
				matchCostCandinate[Framei] = defaulValue;

               
				else
				{    //else1 star
					
					// current camId, reference frameId
					iter2=mOpt.find(std::make_pair(pCurrentFrame->m_iCameraId,refPairVec[Framei].second));
					
					//reference camId, reference frameId
					iter3=mOpt.find(std::make_pair(refPairVec[Framei].first,refPairVec[Framei].second));
					

			        int interval=(pCurrentFrame->m_iFrameId)-(iter2->second->m_iFrameId);   // bug is here
			    
					//x2=x;
					//y2=y;

					//x3=x1;
					//y3=y1;
					//

     //               //存在较大误差		           
				 //   if(interval>0)   //if-else start
					//{

					//	   double xTmp1, yTmp1; //track from current to iter2
					//	
					//	   double xTmp2, yTmp2; //track from iter1 to iter3
					//	   
					//	   
					//	    for(int frameId=(pCurrentFrame->m_iFrameId);frameId<(iter2->second->m_iFrameId);frameId++)
					//		{
					//           xTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pForeOptImg->at(x2,y2,0);
					//		   
					//		   yTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pForeOptImg->at(x2,y2,1);
					//		   x2+=xTmp1;
					//		   y2+=yTmp1;


					//		   xTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pForeOptImg->at(x3,y3,0);
					//		   yTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pForeOptImg->at(x3,y3,1);

					//		   x3+=xTmp2;
					//		   y3+=yTmp2;

					//		}

					//   }
					//   else  //(interval<0)
					//   {
					//	   double xTmp1, yTmp1; //track from current to iter2

					//	   double xTmp2, yTmp2; //track from iter1 to iter3


					//	   for(int frameId=(pCurrentFrame->m_iFrameId);frameId>(iter2->second->m_iFrameId);frameId--)
					//	   {

					//		   xTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pBackOptImg->at(x2,y2,0);

					//		   yTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pBackOptImg->at(x2,y2,1);
					//		   x2+=xTmp1;
					//		   y2+=yTmp1;

					//		   xTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pBackOptImg->at(x3,y3,0);
					//		   yTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pBackOptImg->at(x3,y3,1);

					//		   x3+=xTmp2;
					//		   y3+=yTmp2;

					//	   }


					//   }  //if-else over
					

					x2=x;
					y2=y;

					x3=x1;
					y3=y1;
              
				int x2_int=int(x+0.5);
				int y2_int=int(y+0.5);

				int x3_int=int(x1+0.5);
				int y3_int=int(y1+0.5);

				//iter1->second->GetColorAt(x1,y1,CorrespondingColor);
               
					//存在较大误差, 下面代码需要应用双线性插值方法进行缩小误差		           
					if(interval>0)   //if-else start
					{

						double xTmp1, yTmp1; //track from current to iter2

						double xTmp2, yTmp2; //track from iter1 to iter3

						//四舍五入
						for(int frameId=(pCurrentFrame->m_iFrameId);frameId<(iter2->second->m_iFrameId);frameId++)
						{
							xTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pForeOptImg->at(int(x2+0.5),int(y2+0.5),0);

							yTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pForeOptImg->at(int(x2+0.5),int(y2+0.5),1);
							x2+=xTmp1;
							y2+=yTmp1;


							xTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pForeOptImg->at(int(x3+0.5),int(y3+0.5),0);
							yTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pForeOptImg->at(int(x3+0.5),int(y3+0.5),1);

							x3+=xTmp2;
							y3+=yTmp2;

						}

					}
					else  //(interval<0)
					{
						double xTmp1, yTmp1; //track from current to iter2

						double xTmp2, yTmp2; //track from iter1 to iter3


						for(int frameId=(pCurrentFrame->m_iFrameId);frameId>(iter2->second->m_iFrameId);frameId--)
						{

							xTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pBackOptImg->at(x2,y2,0);

							yTmp1=LSStructureMotion::GetInstance()->GetFrameAt(pCurrentFrame->m_iCameraId,frameId)->m_pBackOptImg->at(x2,y2,1);
							x2+=xTmp1;
							y2+=yTmp1;

							xTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pBackOptImg->at(x3,y3,0);
							yTmp2=LSStructureMotion::GetInstance()->GetFrameAt(iter1->second->m_iCameraId,frameId)->m_pBackOptImg->at(x3,y3,1);

							x3+=xTmp2;
							y3+=yTmp2;

						}


					}  //if-else over
				

					   
					//it2 image coord out of range
					if(x2<0 || y2<0 || x2> iter2->second->GetImgWidth() -1 || y2 > iter2->second->GetImgHeight()-1 || iter2->second->IsInMask(x2, y2) == false)
						matchCostCandinate[Framei] = defaulValue;
					
					//it3 image coord out of range
					else if(x3<0 || y3<0 || x3> iter3->second->GetImgWidth() -1 || y3 > iter3->second->GetImgHeight()-1 || iter3->second->IsInMask(x3, y3) == false)
						matchCostCandinate[Framei] = defaulValue;
					
					else
					{

                      
						float dsp3=iter3->second->GetDspAt(x3,y3,true);
						if(dsp3 < 0)
						{
							matchCostCandinate[Framei] = defaulValue;
							continue;
						}
						if( x3+0.5F < iter3->second->GetImgWidth() &&  y3+0.5F <iter3->second->GetImgHeight() )
						{  // if1 start

							float d3_INT = iter3->second->GetDspAt((int)(x3+0.5F), (int)(y3+0.5F));

							if(d3_INT>0&&fabs(d3_INT-dsp1)<fabs(dsp3-dsp1))
							{
								dsp3=d3_INT;

							    iter3->second->GetWorldCoordFrmImgCoord(x3,y3,dsp3,ptWorldCoordx);

								iter2->second->GetImgCoordFrmWorldCoord(x2t,y2t,dsp2t,ptWorldCoordx);

								if(x2t<0 || y2t<0 || x2t> iter2->second->GetImgWidth() -1 || y2t > iter2->second->GetImgHeight()-1 || iter2->second->IsInMask(x2t, y2t) == false)
									matchCostCandinate[Framei] = defaulValue;

								else
								{
                                    float dsp2=iter2->second->GetDspAt(x2t,y2t,true);

                                    float dxy2=iter2->second->GetDspAt(x2,y2,true);

									if(dsp2<0||dxy2<0)
									{
										matchCostCandinate[Framei] = defaulValue;
										continue;
									}

									if(x2t+0.5F < iter2->second->GetImgWidth() && y2t+0.5F <iter2->second->GetImgHeight())

									{
										float d2_INT = iter2->second->GetDspAt((int)(x2t+0.5F), (int)(y2t+0.5F));
										//dsp = 1.0/z2;
										if(  d2_INT > 0 && fabs(d2_INT - dxy2) < fabs(dsp2 - dxy2) )
											dsp2=d2_INT;
									}



									float dspDiff = fabs(dsp2 - dxy2);
									float wd = dspSigma2 / (dspSigma2 + dspDiff*dspDiff*2);    //different with the paper
									

									iter2->second->GetColorAt((float)x2,(float)y2,CurrentColorxy2);
									iter2->second->GetColorAt((float)x2t, (float)y2t, CorrespondingColorx);
									float colordist = (fabs(CurrentColorxy2[0] - CorrespondingColorx[0]) 
										+ fabs(CurrentColorxy2[1] - CorrespondingColorx[1]) 
										+ fabs(CurrentColorxy2[2] - CorrespondingColorx[2])) / 3.0F;

									colordist *= colordist;

									float wc = fColorSigma2/(fColorSigma2 + colordist*colordist);



									matchCostCandinate[Framei] = max(1e-6, wc * wd);



								}

							}



						}
                   

						//iter2->GetImgCoordFrmWorldCoord(u2x,v2x,dspx,ptWorldCoordx);

					

					}
					

			   }// else1 end
               
			  

			}//iFrame end


			// get average matchCost
		//	dataCosti[depthLeveli] = GetValue(matchCostCandinate, FwFrames.size(), BwFrames.size());  //May have problem!
		
			float sumMatchCost=0.0;
			for(int i=0;i<matchCostCandinate.size();i++)
			{
				sumMatchCost+=matchCostCandinate[i];

			}
			dataCosti[depthLeveli]=sumMatchCost/matchCostCandinate.size();
		


			if(dataCosti[depthLeveli] > MaxLikelihood)
			{
				MaxLikelihood = dataCosti[depthLeveli];
				bestLabel = depthLeveli;
			}
		

		}// dspLeveli

		//===========================================================================================//
		// need be changed, add optical flow calculation //
		// deal with occulusion    //
		//===========================================================================================//

		//Normalize
		float maxCost = 1e-6F;
		for(int depthLeveli = 0; depthLeveli < dspLevelCount; depthLeveli++)
		{
			dataCosti[depthLeveli] =  1.0 - dataCosti[depthLeveli] / MaxLikelihood;
			maxCost = max(maxCost,dataCosti[depthLeveli]);
		}

		for(int depthLeveli=0; depthLeveli<dspLevelCount; depthLeveli++)
		{
			dataCosti[depthLeveli] /= maxCost;
			dataCosti[depthLeveli] *= dataCostWeight;
		}

		//归一化（wc*wd越大，对应的DataCost越小）
	  
	

}
*/
