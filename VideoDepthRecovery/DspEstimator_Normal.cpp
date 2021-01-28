#include "DspEstimator_Normal.h"
#include <vector>
#include <memory>
#include "VDRVideoFrame.h"

#include <iostream>

//���캯��
DspEstimator_Normal::DspEstimator_Normal(void)
{
}

//�õ�һ����̬ʵ���������ⲿ����
DspEstimator_Normal *DspEstimator_Normal::GetInstance()
{
	static DspEstimator_Normal instance;
	return &instance;
}

//��������
DspEstimator_Normal::~DspEstimator_Normal(void)
{
}

//����Init����ʵ��
void DspEstimator_Normal::Run_Init(int start,int end)
{
	
	//����Ƿ�����ͼ������,�Լ��Ƿ񣨳ɹ���ѡ��ο�֡�ͽ���ͼ��ֿ�
	if(check()==false)
		 return;

	//�õ�structionMotion���͵�����
	VDRStructureMotion &structMotion=*VDRStructureMotion::GetInstance();

	//������disparity�Ĳ�����(m+1=101)
	int iDspLevel=DepthParaWidget::GetInstance()->m_iDspLevel;

    //��������Ⱥ͸߶ȣ��Դ˺�����ⲻ������Ҫ�ٲ鿴��
    int maxBlockWidth,maxBlockHeight; 
	m_pImgPartition->GetMaxBlockSize(VDRVideoFrame::GetImageWidth(),VDRVideoFrame::GetImageHeight(),maxBlockWidth,maxBlockHeight);

	//��ʼ��������洢�ռ䣬��Ԫ��ȫ����0��
	DataCost DataCost(maxBlockWidth,maxBlockHeight,iDspLevel);

	//����һ����������ֵDataCost��(PixelCostComputer_Init)ʵ��
	m_pPixelCostComputer=PixelCostComputer_Init::GetInstance();
	
	//�õ����һ֡֡��
	if(end==-1)
	{
		end=structMotion.GetFrameCount()-1;
	   // std::cout<<"execute end==-1"<<std::endl;
		std::cout<<"end= "<<end<<std::endl;
	}

	for(int index=start;index<=end;index++)
	{
	//	clock_t beginTime=clock();
		std::cout<<"Estimate Frame: "<<index<<"("<<start<<"~"<<end<<")"<<std::endl;
		//(504,540),ò��������ʱ���������
	    std::cout<<"Max Block Size: "<<maxBlockWidth*maxBlockHeight<<"("<<maxBlockWidth<<","<<maxBlockHeight<<")"<<std::endl; 
		m_pImgPartition->print();

		//�õ���ǰ֡
		VDRVideoFrame *pCurrentFrame=structMotion.GetFrameAt(index);
	    

		//�õ��ο�֡������
		std::vector<int> nearIndex=m_pFrameSelector->GetReferenceFrames(*pCurrentFrame,structMotion.GetFrameCount());

	  //  std::cout<<"nearIndex.size()"<<nearIndex.size()<<std::endl;

		//�����洢���ο�֡���Ŀռ�
		std::vector<VDRVideoFrame *> nearFrames(nearIndex.size());

		for(auto i=0U; i<nearIndex.size(); i++)
		{
		   nearFrames[i] = structMotion.GetFrameAt(nearIndex[i]);
          // std::cout<<"nearIndex[i]"<<nearIndex[i]<<std::endl;
		}


		Run_Init_At(nearFrames,pCurrentFrame,DataCost);
	   // if(Run_Init_At(nearFrames,pCurrentFrame,DataCost)==false)
		//	return;



	}

	std::cout<<"implement Run_Init function"<<std::endl;
}


//����BO����ʵ��
void DspEstimator_Normal::Run_BO(int start,int end,int pass)
{
	//����Ƿ�����ͼ������,�Լ��Ƿ񣨳ɹ���ѡ��ο�֡�ͽ���ͼ��ֿ�
	if(check() == false)
		return;

	//����SFM�������ڶ�ȡ����ε���Ϣ������˶��켣�����ٵ㣩
	VDRStructureMotion &structMotion = *VDRStructureMotion::GetInstance();

   //������disparity�Ĳ�����(m+1=101)
	int DspLevel = DepthParaWidget::GetInstance()->m_iDspLevel;
	
	//��������Ⱥ͸߶ȣ��Դ˺�����ⲻ������Ҫ�ٲ鿴��
	int MaxBlockWidth, MaxBlockHeight;
	m_pImgPartition->GetMaxBlockSize(VDRVideoFrame::GetImageWidth(), VDRVideoFrame::GetImageHeight(), MaxBlockWidth, MaxBlockHeight); //504,540
	
	//��ʼ��������洢�ռ䣬��Ԫ��ȫ����0��
	DataCost DataCost(MaxBlockWidth, MaxBlockHeight, DspLevel);

	//����һ����������ֵDataCost��(PixelCostComputor_Refine)ʵ��
	 m_pPixelCostComputer = PixelCostComputor_Refine::GetInstance();   


	 //======for DE ================
	 //int expanLevels = DepthParaWidget::GetInstance()->m_iSubLevels;
	 //DataCost subDataCost;
	 //if(m_bRunDEWhenBO == true){
	  // int subDspLevel = expanLevels * 2 + 1;
	  //	 subDataCost.Create(MaxBlockWidth, MaxBlockHeight, subDspLevel);
	 //}

	 //�õ����һ֡֡��
	 if(end == -1)
	 {
		 end = structMotion.GetFrameCount() - 1;
		 std::cout<<"end= "<<end<<std::endl;
	 }
	 
	   //ִ������
	 for(int passi = 0; passi < pass; ++passi)
	 {
		 //��������֡
		 for(int index = start; index <= end; index++)
		 {
			 //��ʱ��
			// clock_t beginTime = clock();
           
			 //�������ӡ��֡�ͳ��������Ϣ
			 std::cout<<"Pass:"<<passi+1<<"/"<<pass<<std::endl;
			 std::cout<<"Refine Frame: "<<index<<"/("<<start<<"~"<<end<<")"<<std::endl;

			 
			 //�������ӡ���飨�ָ��Ϣ
			 std::cout << "Max. Block Size:" << MaxBlockWidth * MaxBlockHeight 
				 << "(" << MaxBlockWidth << "*" << MaxBlockHeight << ")" <<std::endl; 
			 m_pImgPartition->print();

			 //��ȡ��ǰ֡�Ͳο�֡
			 VDRVideoFrame * pCurrentFrame = structMotion.GetFrameAt(index);
			 std::vector<int> nearIndex = m_pFrameSelector->GetReferenceFrames(*pCurrentFrame, structMotion.GetFrameCount());
			 std::vector<VDRVideoFrame *> nearFrames(nearIndex.size());
			 for(auto i=0U; i<nearIndex.size(); i++)
			 {
				 nearFrames[i] = structMotion.GetFrameAt(nearIndex[i]);
				// std::cout<<"nearIndex[i]: "<<nearIndex[i]<<std::endl;
			 }
			 
			 if(Run_BO_At(nearFrames, pCurrentFrame, DataCost) == false)
				 return;
			
        //     std::cout<<"===>Total Time:"<<(double)(clock()-beginTime)/CLOCKS_PER_SEC<<" s"<<std::endl;
		 }
	 }

   std::cout<<"implement Run_BO funciton"<<std::endl;
}

//����DE����ʵ��
void DspEstimator_Normal::Run_DE(int start,int end,int pass)
{
	//�����Ƿ��������ع��̣�ѡ��ο�֡�ͷֿ�
	if(check()==false)
		return;

    //����SFM����õ��������ٵ㣬�������ε���Ϣ
	VDRStructureMotion &structMotion = *VDRStructureMotion::GetInstance();

	//���DE����
	int iters = DepthParaWidget::GetInstance()->m_iSubIter;       //����������� m_iSubIter=2
	int expanLevels = DepthParaWidget::GetInstance()->m_iSubLevels; //m_iSubLevels=10
	int DspLevel = expanLevels * 2 + 1;              //disparity�Ĳ���

	//�������ĳ��ȺͿ�ȣ�504*540��
	int MaxBlockWidth, MaxBlockHeight;
	m_pImgPartition->GetMaxBlockSize(VDRVideoFrame::GetImageWidth(), VDRVideoFrame::GetImageHeight(), MaxBlockWidth, MaxBlockHeight);
	
	//��ʼ��������洢�ռ䣬��Ԫ��ȫ����0��
	DataCost DataCost(MaxBlockWidth, MaxBlockHeight, DspLevel);

	//����һ����������ֵDataCost��(PixelCostComputor_Refine)ʵ��
	m_pPixelCostComputer = PixelCostComputor_Refine::GetInstance();

	if(end == -1)
		end = structMotion.GetFrameCount() - 1;   //end=140
	for(int passi = 0; passi < pass; ++passi){    //pass=2  ִ������
		for(int index = start; index <= end; index++){

		//	clock_t beginTime = clock();

		    std::cout<<"Pass: "<<passi+1<<"/"<<pass<<std::endl;
			std::cout<<"DE Frame: "<<index<<"/("<<start<<"~"<<end<<")"<<std::endl;

			std::cout << "Max. Block Size:" << MaxBlockWidth * MaxBlockHeight << "(" << MaxBlockWidth << "*" << MaxBlockHeight << ")" << std::endl; 
			m_pImgPartition->print();

           //��ȡ��ǰ֡�Ͳο�֡
			VDRVideoFrame *pCurrentFrame = structMotion.GetFrameAt(index);
		    std::vector<int> nearIndex = m_pFrameSelector->GetReferenceFrames(*pCurrentFrame, structMotion.GetFrameCount());   //��òο�֡��֡��
			std::vector<VDRVideoFrame *> nearFrames(nearIndex.size());
			for(auto i=0U; i<nearIndex.size(); i++)
			{
				nearFrames[i] = structMotion.GetFrameAt(nearIndex[i]);
			    std::cout<<nearIndex[i]<<" "; 
			}
			
			if(Run_DE_At(nearFrames, pCurrentFrame, DataCost, iters, expanLevels) == false)
		   	return;
		
	   //    std::cout<<"===>Total Time:"<<(double)(clock()-beginTime)/CLOCKS_PER_SEC;  

		}
	}
	std::cout<<"implement Run_DE funciton"<<std::endl;
}


//�ڵ�֡��ִ��Init��disparity��������Run_Init����
bool DspEstimator_Normal::Run_Init_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost)
{
	int imageWidth=VDRVideoFrame::GetImageWidth();
	int imageHeight=VDRVideoFrame::GetImageHeight();

	ZIntImage  labelImgInit,labelImgBP,labelImgSegm;
	labelImgInit.CreateAndInit(imageWidth,imageHeight,1,-1);      //����imgageWidth*imageHeight��С�ĵ�ͨ��ͼ�񣬳�ֵ��Ϊ-1��
	labelImgBP.CreateAndInit(imageWidth, imageHeight, 1, -1);
	labelImgSegm.CreateAndInit(imageWidth, imageHeight, 1, -1);

    int dspLevel=DataCost.GetDspLevels();  //��ȡ disparity����
    m_pPixelCostComputer->SetFrames(pCurrentFrame,&nearFrames); //���ü���֡
	
	double disck=DepthParaWidget::GetInstance()->GetTrueDisck(); //�õ�ƽ���ض���

	double dspMin=DepthParaWidget::GetInstance()->m_dDspMin;    //disparity��Сֵ
	double dspMax=DepthParaWidget::GetInstance()->m_dDspMax;    //disparity���ֵ

	//double dspMax=0.160021;      //��DspMax������

	/******************************************************
		disparity����ΪdspLevel��m+1)��,
	    ���ڵ�k�㣬��disparityֵd.k=(m-k)/m*d.min+k/m*d.max
		k=0,������m.
	 ********************************************************/
/*
	std::vector<double> dspV(dspLevel);
	for(auto k=0;k<dspLevel;k++)
	{
		dspV[k]=dspMin*(dspLevel-1-k)/(dspLevel-1)+dspMax*k/(dspLevel-1);      
	}

	std::cout<<"dspMax: "<<dspMax<<" dspMin: "<<dspMin<<" dspLevel�� "<<dspLevel<<std::endl;
*/
	std::vector<double> dspV(dspLevel);
	for(auto i=0;i<dspLevel;i++)
		dspV[i] = dspMin * (dspLevel - 1 - i)/(dspLevel - 1)  + dspMax * i/(dspLevel - 1) ;

	std::cout << "DspMin:" << dspMin << " DspMax:" << dspMax << " DspLevel:" << dspLevel <<std::endl;
    //Load
	     //����Դ����ɫ��ͼ��
	  pCurrentFrame->LoadColorImage();
	  
	  
	     //����disparityͼ��
      pCurrentFrame->CreateDspImage();

	  //���زο�֡��Դ����ɫ��ͼ��

	//  std::cout<<"nearFrames.size()"<<nearFrames.size()<<std::endl;

	  for(auto i=0U;i<nearFrames.size();i++)
	  {
		  nearFrames[i]->LoadColorImage();
	  }

	  //���� meanShiftSegment����
	  MeanShiftSeg meanShiftSegm;
	  

	  //MeanShift�㷨����
	  int sigmaS=DepthParaWidget::GetInstance()->m_iSegSpatial;
	  int sigmaR=DepthParaWidget::GetInstance()->m_fSegColor;
	  int minSize=DepthParaWidget::GetInstance()->m_iSegMinSize;
     
	  //std::cout<<"GetImageWidth:"<<pCurrentFrame->GetImageWidth()<<std::endl;

	  //�ж����루��ɫ��ͼ���Ƿ�Ϊ��
	  if(pCurrentFrame->m_pColorImage==NULL)
	  {
		  std::cout<<"color Image is Null"<<std::endl;
	  }
	  
	  //����MeanShift�㷨��ͼ����зָ�
	  meanShiftSegm.Segment(*pCurrentFrame->m_pColorImage,sigmaS,sigmaR,minSize,pCurrentFrame->GetSegmSavePath());
	  
	  //ˮƽ�ʹ�ֱ����ֿ���
	  int blockCountX=m_pImgPartition->GetBlockCountX();
	  int blockCountY=m_pImgPartition->GetBlockCountY();
	  std::cout<<" blockCountX: "<<blockCountX<<std::endl;
	  std::cout<<" blockCountY: "<<blockCountY<<std::endl;

	  bool canceled=false;
	  for(int blockIdy=0;(canceled=isCanceled())==false&&blockIdy<blockCountY;blockIdy++)
	  {
		  for(int blockIdx=0;(canceled=isCanceled())==false&&blockIdx<blockCountX;blockIdx++)
		  {
			  Block  block=m_pImgPartition->GetBlockAt(blockIdx,blockIdy,imageWidth,imageHeight);

			 // clock_t tempTime=clock();
	
			  std::cout<<"Step1_1.Init DataCost: "<<std::endl;
			 

			  GetDataCost(block,DataCost,labelImgInit,dspV,nullptr);

			  //std::cout<<std::endl;
			  //�洢LabelImage.
   			  pCurrentFrame->SaveLabelImage(labelImgInit,dspLevel,"1.1_Init_");
			 
			  
			  //std::cout<<"dspLevel�� "<<dspLevel<<std::endl;
			
			  std::cout<<std::endl;
              std::cout<<"Step1_2.BP:"<<std::endl; 
			  RefineDspByBP(disck,block,DataCost,labelImgBP,true);
			 pCurrentFrame->SetDspImg(labelImgBP,*block.m_pInnerBlock,dspMin,dspMax,dspLevel);
			  pCurrentFrame->SaveLabelImage(labelImgBP,dspLevel,"1.2_BP_");

			 std::cout<<std::endl;
			 std::cout<<"PlanFitting����"<<std::endl;
			 for(int y = block.m_pInnerBlock->m_Y; y < block.m_pInnerBlock->m_Y + block.m_pInnerBlock->m_iHeight; ++y)
			 {
				 for(int x = block.m_pInnerBlock->m_X; x < block.m_pInnerBlock->m_X + block.m_pInnerBlock->m_iWidth; ++x)
					 labelImgSegm.at(x, y) = labelImgBP.at(x, y);
			 }
			 //���meanShift�ָ�ṹ�Ż�disparity
			 RefineDspBySegm(block,meanShiftSegm,pCurrentFrame,labelImgSegm,DataCost);
			 pCurrentFrame->SaveLabelImage(labelImgSegm,dspLevel,"1.3_SegRefine_");
		  }
	  }
	 
	  
	  //��Init����δ��;ȡ����������raw�ļ���������
	  if(canceled == true)
		  std::cout << "Depth Recovering is canceled..." << std::endl;
	  else
		   pCurrentFrame->SaveDspImg();

	  //�ͷ�Ϊ��ǰ֡�Ͳο�֡����Ĵ洢�ռ�
      for(auto i=0U; i<nearFrames.size(); i++)
			  nearFrames[i]->Clear();
	  pCurrentFrame->Clear();

	  pCurrentFrame->SaveLabelImage(labelImgSegm,dspLevel,"1_Init_final_");
	  return canceled == false;
}


//�ڵ�ǰ֡��Ӧ��BO���Ż�Init����disparity�Ľ������Run_BO����
bool DspEstimator_Normal::Run_BO_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost)
{
	int ImgWidth = VDRVideoFrame::GetImageWidth();
	int ImgHeight = VDRVideoFrame::GetImageHeight();

	int dspLevel = DataCost.GetDspLevels();
	double dspMin = DepthParaWidget::GetInstance()->m_dDspMin;
	double dspMax = DepthParaWidget::GetInstance()->m_dDspMax;

	ZIntImage labelImgInt, labelImgBP;
	labelImgInt.CreateAndInit(ImgWidth, ImgHeight, 1, -1);
	labelImgBP.CreateAndInit(ImgWidth, ImgHeight, 1, -1);


	//���ص�ǰ֡�Ͳο�֡�Ĳ�ɫͼ���Լ���Init������òο�֡�����ͼ��
	pCurrentFrame->LoadColorImage();

	pCurrentFrame->CreateDspImage();
	for(auto i=0U; i<nearFrames.size(); i++)
	{
		nearFrames[i]->LoadColorImage();
		
		if(nearFrames[i]->LoadDepthImg() == false)
	    return false;
	}

	//***���ڵ�����, 
	m_pPixelCostComputer->SetFrames(pCurrentFrame,&nearFrames);

	double disck = DepthParaWidget::GetInstance()->GetTrueDisck();   //�õ�ƽ���ض���
   //���㲻ͬ������disparityֵ
	std::vector<double> dspV(dspLevel);
	for(auto i=0;i<dspLevel;i++)
		dspV[i] = dspMin * (dspLevel - 1 - i)/(dspLevel - 1)  + dspMax * i/(dspLevel - 1) ;

	std::cout << "DspMin:" << dspMin << " DspMax:" << dspMax << " DspLevel:" << dspLevel <<std::endl;


	//�õ�ˮƽ�ʹ�ֱ����ֿ����
	int blockCountX = m_pImgPartition->GetBlockCountX();
	int blockCountY = m_pImgPartition->GetBlockCountY();

	bool canceled = false;
	for(int blockIdy = 0; (canceled = isCanceled()) == false && blockIdy < blockCountY; blockIdy++)
	{
		for(int blockIdX = 0; (canceled = isCanceled()) == false && blockIdX < blockCountX; blockIdX++)
		{

			//����õ��鲢���м���
			Block block = m_pImgPartition->GetBlockAt(blockIdX, blockIdy, ImgWidth, ImgHeight);
			
           // clock_t tempTime = clock();
			std::cout<<"Step2_1.Init DataCost: "<<std::endl;
			GetDataCost(block,DataCost,labelImgInt,dspV,nullptr);

		    pCurrentFrame->SaveLabelImage(labelImgInt,dspLevel,"2.1_Init");
		
		     std::cout<<"Step2_2 BP: "<<std::endl;
			 RefineDspByBP(disck, block, DataCost, labelImgBP, true);
			
		     pCurrentFrame->SaveLabelImage(labelImgBP,dspLevel,"2.2_BP");
		
			 pCurrentFrame->SetDspImg(labelImgBP, *block.m_pInnerBlock, dspMin, dspMax, dspLevel);		

		}
	}


	//�ͷŵ�ǰ֡�Ͳο�֡�Ĵ洢�ռ�
	if(canceled == true)
	{
		std::cout << "Depth Recovering is canceled..." << std::endl;
		for(auto i=0U; i<nearFrames.size(); i++)
			nearFrames[i]->Clear();
		pCurrentFrame->Clear();
		return false;
	}

	//�洢BO����������
	pCurrentFrame->SaveLabelImage(labelImgBP, dspLevel,"2_BO_final_");

	return true;

}

bool DspEstimator_Normal::Run_DE_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost, int iters, int expanLevls)
{
	int ImgWidth = VDRVideoFrame::GetImageWidth();   //ͼ����
	int ImgHeight =VDRVideoFrame::GetImageHeight();  //ͼ��߶�

	//disparity����
	int dspLevel = DepthParaWidget::GetInstance()->m_iDspLevel;   //101
	double dspMin = DepthParaWidget::GetInstance()->m_dDspMin;   //��Сdisparityֵ
	double dspMax = DepthParaWidget::GetInstance()->m_dDspMax;   //���disparityֵ
    

	//��ǰ֡���ز�ɫͼ������ͼ��
	pCurrentFrame->LoadColorImage();
	if(pCurrentFrame->LoadDepthImg() == false)
	{
		std::cout << "ERROR:Can not load DspImg" << std::endl;
		return false;
	}
	
	//�ο�֡���ز�ɫͼ������ͼ��
	for(auto i=0U; i<nearFrames.size(); i++)
	{
		nearFrames[i]->LoadColorImage();
		if( nearFrames[i]->LoadDepthImg() == false)
		{
			std::cout << "ERROR:Can not load DspImg" << std::endl;
			return false;
		}
	}

	ZIntImage labelImgInit, labelImgBP, offsetImg;
	labelImgInit.CreateAndInit(ImgWidth, ImgHeight, 1, -1);
	pCurrentFrame->InitLabelImgByDspImg(labelImgBP, dspLevel, dspMin, dspMax);   //����BO������õ�disparityͼ���ʼ��labelImgBP
	
	offsetImg.Create(ImgWidth, ImgHeight);     //ΪoffsetImage����洢�ռ�
	m_pPixelCostComputer->SetFrames(pCurrentFrame, &nearFrames);  //���ü���֡

	int subDsplevel = 2 * expanLevls + 1;           //subDspLevel=21
	int layerCount = dspLevel - 1;                  //layerCount=100
	//std::cout<<"subDsplevel: "<<subDsplevel<<std::endl;
   // std::cout<<"layerCount: "<<layerCount<<std::endl;

	//��������ʱ����
	//============backup=======================================================
	double backupDspSigma = m_pPixelCostComputer->GetDspSigma();             //sigma_d
	double backupDataCostWeight = m_pPixelCostComputer->GetDataCostWeight(); //DataCost��Ȩ��ֵ
	double backupDiscK = DepthParaWidget::GetInstance()->GetTrueDisck();     //ƽ���ض���,�μ�formula[4.1]
	//=========================================================================

	double dspSigma = backupDspSigma;
	double dataCostWeight = backupDataCostWeight * expanLevls /(dspLevel - 1);  //����
	m_pPixelCostComputer->SetDataCostWeight(dataCostWeight);                    //����������Ȩ��                
	double discK = backupDiscK * expanLevls;                                    //? ?

	std::cout << "Depth expansion..." <<std::endl;

	//��ã�ͼ��ģ��ֿ�����
	int blockCountX = m_pImgPartition->GetBlockCountX();
	int blockCountY = m_pImgPartition->GetBlockCountY();
	
	std::vector<double> dspV;
	bool canceled = false;

	//ѭ������Ϊ�������Σ�����δ���Cancle��ť
	for(int iPass = 0; (canceled = isCanceled()) == false && iPass < iters; iPass++)
	{
		layerCount *= expanLevls;	    //layerCount=1000����һ�Σ���layerCount=10000(�ڶ��Σ�
		//std::cout<<"layerCount: "<<layerCount<<std::endl;
		for(int y = 0; y < ImgHeight; y++)
		{
			for(int x = 0; x < ImgWidth; x++)
			{
				offsetImg.at(x, y) = std::min( layerCount - (subDsplevel - 1),std::max(0, (labelImgBP.at(x, y) - 1) * expanLevls) );   //??
				// std::cout<<"offsetImg.at(x,y): "<<offsetImg.at(x,y)<<std::endl;
			}
		}

		//disparity����ÿ������10�������³�ʼ��Labelͼ��ͷ���dsparity�洢�����Ŀռ�
		labelImgBP.Init(-1);
		dspV.resize(layerCount + 1);

		//��ͬdisparity������disparity�ľ���ֵ[d_k=(m-k)/m*d_min+k/m*d_max,k=0...m;
		for(int leveli = 0; leveli <= layerCount; ++leveli)
			dspV[leveli] = dspMin * (layerCount - leveli)/layerCount + dspMax * leveli/layerCount;

		for(int blockIdy = 0; (canceled = isCanceled()) == false && blockIdy < blockCountY; blockIdy++)
		{
			for(int blockIdX = 0; (canceled = isCanceled()) == false && blockIdX < blockCountX; blockIdX++)
			{
				//�õ�Ҫ����ģ�ͼ��֣���
				Block block = m_pImgPartition->GetBlockAt(blockIdX, blockIdy, ImgWidth, ImgHeight);
			//	clock_t tempTime = clock();
				std::cout<<"Init DataCost..."<<std::endl;
				GetDataCost(block, DataCost, labelImgInit, dspV, &offsetImg);   //�Կ��ʼ��DataCost���õ�labelImage

				  
				const Block &innerBlock = *block.m_pInnerBlock;    //�ֿ��ڲ�����

				  for(int y = innerBlock.m_Y; y - innerBlock.m_Y < innerBlock.m_iHeight; ++y)
				  {
				 	for(int x = innerBlock.m_X; x - innerBlock.m_X < innerBlock.m_iWidth; ++x)
				  		labelImgInit.at(x, y) += offsetImg.at(x, y); 
				  }
				  pCurrentFrame->SaveLabelImage(labelImgInit,layerCount+1,"3.1_Init_");  


				  RefineDspByBP(discK, block, DataCost, labelImgBP, true, &offsetImg);
				  for(int y = innerBlock.m_Y; y - innerBlock.m_Y < innerBlock.m_iHeight; ++y)
				  {
					  for(int x = innerBlock.m_X; x - innerBlock.m_X < innerBlock.m_iWidth; ++x)
						  labelImgBP.at(x, y) += offsetImg.at(x, y); 
				  }
				  pCurrentFrame->SaveLabelImage(labelImgBP,layerCount + 1,"3.2_BP_");

			}
		}

		dspSigma = std::max(0.0001 * (dspMax - dspMin), 2 * dspSigma / expanLevls);     //���㲢����sigma_d
		m_pPixelCostComputer->SetDspSigma(dspSigma);   //���õ������ؼ��㵥λ�ϵ�sigma_d

		//���ݿ��ϵ�label����ͼ���disparityֵ
	    pCurrentFrame->SetDspImg(labelImgBP, Block(0, 0, ImgWidth, ImgHeight, std::shared_ptr<Block>(nullptr)), dspMin, dspMax, layerCount + 1);
		
	}

	 // recovery
      m_pPixelCostComputer->SetDspSigma(backupDspSigma);           //����(�ָ���sigma_d
	  m_pPixelCostComputer->SetDataCostWeight(backupDataCostWeight);  //���ã��ָ���DataCostȨ��ϵ��
	
	  if(canceled == true)
		  std::cout << "Depth Recovering is canceled..." << std::endl;
	  else
		  pCurrentFrame->SaveDspImg();     //�洢disparityͼ��Ϊraw�ļ�


	  //�ͷ��ڴ�
	  for(auto i=0U; i<nearFrames.size(); i++)
		  nearFrames[i]->Clear();
	  pCurrentFrame->Clear();

	  pCurrentFrame->SaveLabelImage(labelImgBP,layerCount + 1,"3_DE_final");     //�洢DE��Ľ��
	  return canceled == false;
}

	//�����Ƿ��������ع��̣�ѡ��ο�֡�ͷֿ�
bool DspEstimator_Normal::check()
{
	//�����Ƿ��Ѿ����ع���
    if(VDRStructureMotion::GetInstance()->GetFrameCount()==0)
	{
		std::cout<<"Load project first,please!"<<std::endl;
        return false;
	}
    //�����Ƿ񣨳ɹ���ѡ��ο�֡���Ƿ񣨳ɹ����ֿ�
	if(m_pFrameSelector == false || m_pImgPartition == false)
	{
		std::cerr << "Please Initialize Frame Selector or Image Partition First!" << std::endl;
		system("pause");
		return false;
	}
    return  true;
}


//Step1_1.�Կ��ʼ��DataCost���õ�labelImage
void DspEstimator_Normal::GetDataCost(const Block &block,DataCost &outDataCost,ZIntImage & lableImg,const std::vector<double> &dspV,ZIntImage *pOffsetImg)
{
	//���߳�
	for(int y=block.m_Y;y<block.m_Y+block.m_iHeight;++y)
	{
		DataCostWorkUnit *pWorkUnit=new DataCostWorkUnit(y,block,dspV,lableImg,outDataCost,*m_pPixelCostComputer,pOffsetImg);
		pWorkUnit->Execute();
	}

}

//Step1_2.��BP�Ż�disparity
void DspEstimator_Normal::RefineDspByBP(float disck, const Block &block, DataCost &DataCost, ZIntImage &labelImg, bool addEdgeInfo, ZIntImage* offsetImg /* = NULL */)
{
	BeliefPropagation bp(
		disck,						//disc_k
		DataCost.GetDspLevels(),		//max_d
		0.1F,						//sigma for gauss smooth
		offsetImg != NULL ? 10 : 5,	   //nIter
		offsetImg != NULL ? 1 : 5  /*,	//nlevels
		cpuThreadCount	*/			//nThreads
		);

   //��ӡ�������������Ϣ
	bp.printPara();

	//����bp
	bp.run(block, DataCost, labelImg, offsetImg);

}


//Step1_3 ��meanShift����ָ�����Disparity�����Ż�
void DspEstimator_Normal::RefineDspBySegm( const Block &block, const MeanShiftSeg &meanShiftSegm, VDRVideoFrame* currentFrame, ZIntImage& labelImg, DataCost& DataCost )
{

	
	class cmp
	{
	public:

		//�Ƚ����������Ĵ�С
		bool operator()(const std::vector<blk::Point> &v1, const std::vector<blk::Point> &v2)
		{
			return v1.size() > v2.size();
		}
	
		
	};

	std::vector<std::vector<blk::Point>> regions;
	meanShiftSegm.GetRegions(regions, *block.m_pInnerBlock);
	std::sort(regions.begin(), regions.end(), cmp());           //�����е���������С�����˳������

	//int threadCount = DepthParaWidget::GetInstance()->m_iThreadsCount;
	int planfittingSize = DepthParaWidget::GetInstance()->m_iPlaneFittingSize;


	for(auto iSeg=0U; iSeg<regions.size(); ++iSeg)
	{
		if(regions[iSeg].size() >= planfittingSize)
		{
			PlanFittingWorkUnit* pWorkUnit = new PlanFittingWorkUnit(block, meanShiftSegm, regions[iSeg], currentFrame, labelImg, DataCost);
			//pm.EnQueue(pWorkUnit);
			pWorkUnit->Execute();
		}

	}
	
}

//�ڵ�����Ԫ��Ӧ��Planfitting
void DspEstimator_Normal::PlanfittingOneSegm(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, VDRVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost)
{
	//������
	static const int NeighborCount = 4;
	static const int Neighbor[NeighborCount][2] = { {0,-1}, {1,0}, {0,1}, {-1,0} };
	
	int dspLevel = dataCost.GetDspLevels();
	float disck = DepthParaWidget::GetInstance()->GetTrueDisck();
	std::vector<double> layerEnergy(dspLevel, 0);  //����dspLevel������ÿ����ֵ��ֵΪ0


	//Find best layer (�������ֵ��С�Ĳ�����
	double minSegEnergy = 1e20;
	int iSegBestLayer=0;
	for(int iLayer=0; iLayer<dspLevel; ++iLayer)
	{
		for(std::vector<blk::Point>::const_iterator it = SegmPoints.begin(); it != SegmPoints.end(); ++it)
		{
			int x = it->X;
			int y = it->Y;
			//layerEnergy[iLayer] += dataCost[((offsetX + x) + (offsetY + y) * m_iBlockWidth) * dspLevel + iLayer];	
			layerEnergy[iLayer] += dataCost.GetValueAt(x - block.m_X,  y - block.m_Y, iLayer);

			//Neighbor
			int x1,y1;
			for(int neighbori = 0; neighbori< NeighborCount; neighbori++)
			{
				x1 = x + Neighbor[neighbori][0];
				y1 = y + Neighbor[neighbori][1];
				if(block.m_pInnerBlock->IsInBlock(x1, y1) && meanShiftSegm.GetSegmMapAt(x,y) != meanShiftSegm.GetSegmMapAt(x1,y1) && labelImg.at(x1,y1) != -1)
				{
					layerEnergy[iLayer] += std::min<double>(disck, abs( iLayer - labelImg.at(x1, y1)));   //�ο����Ĺ�ʽ4.1
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

	//���캯������ʼ����
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
		
		// ����ĳ��Index�ϣ���ͬlayer�ϣ���dataCostUnitֵ
		LMOptimizer.SetDataCostAt(index, dataCostUnit);
		nonSegEnergy += dataCost.GetValueAt(x - block.m_X, y - block.m_Y, iBestLayer);
      


		int x1, y1;
		for(int neighbori = 0; neighbori< NeighborCount; neighbori++){
			x1 = x + Neighbor[neighbori][0];
			y1 = y + Neighbor[neighbori][1];
		  //�ж��Ƿ��ڿ��ڣ����ڿ����������������Ե���
			if(block.m_pInnerBlock->IsInBlock(x1, y1) == false)
				continue;

	//������Ĵ��벻�Ǻ����
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

   //�ȹ̶�a_i,b_i����Ԫ�أ���c_i
   optA[0] = 0.0; optA[1] = 0.0; optA[2] = iSegBestLayer;

   //����levenMar���a_i,b_i(c_i��
   LMOptimizer.Optimize(optA, MinValue);
   double a,b,c;
   a = optA[0]; b = optA[1];	c = optA[2];

   	minSegEnergy = LMOptimizer.GetAbsValue(optA);
   
	//std::cout<<"minSegEnergy:"<<minSegEnergy<<"   nonSegEnergy: "<<nonSegEnergy<<std::endl;

	double segErrRateThreshold = DepthParaWidget::GetInstance()->GetTruetSegErrRateThreshold();

	//�Ƚϲ�����dis
	if(minSegEnergy < segErrRateThreshold * nonSegEnergy){
		for(std::vector<blk::Point>::const_iterator ptIter = SegmPoints.begin(); ptIter != SegmPoints.end(); ++ptIter){
			int x = ptIter->X;
			int y = ptIter->Y;
			float bestD = a * x + b * y + c;
	        pCurrentFrame->SetDspAt(x, y, GetDspAtLeveli(bestD));
			labelImg.at(x, y) = std::min((int)dspLevel-1,std::max((int)0, (int)(bestD+0.5)) );
			
		}
	}

}

//���leveli���ϵ�disparityֵ
float DspEstimator_Normal::GetDspAtLeveli( float leveli )
{
	double dspMin = DepthParaWidget::GetInstance()->m_dDspMin;
	double dspMax = DepthParaWidget::GetInstance()->m_dDspMax;
	int dspLevel = DepthParaWidget::GetInstance()->m_iDspLevel;
	return dspMin * (dspLevel - 1 - leveli)/(dspLevel - 1)  + dspMax * leveli/(dspLevel - 1) ;
}

