#include "DspEstimator_Normal.h"
#include <vector>
#include <memory>
#include "VDRVideoFrame.h"

#include <iostream>

//构造函数
DspEstimator_Normal::DspEstimator_Normal(void)
{
}

//得到一个静态实例，便于外部引用
DspEstimator_Normal *DspEstimator_Normal::GetInstance()
{
	static DspEstimator_Normal instance;
	return &instance;
}

//析构函数
DspEstimator_Normal::~DspEstimator_Normal(void)
{
}

//定义Init函数实现
void DspEstimator_Normal::Run_Init(int start,int end)
{
	
	//检测是否载入图像序列,以及是否（成功）选择参考帧和进行图像分块
	if(check()==false)
		 return;

	//得到structionMotion类型的引用
	VDRStructureMotion &structMotion=*VDRStructureMotion::GetInstance();

	//设置求disparity的层数。(m+1=101)
	int iDspLevel=DepthParaWidget::GetInstance()->m_iDspLevel;

    //获得最大块宽度和高度（对此函数理解不够，需要再查看）
    int maxBlockWidth,maxBlockHeight; 
	m_pImgPartition->GetMaxBlockSize(VDRVideoFrame::GetImageWidth(),VDRVideoFrame::GetImageHeight(),maxBlockWidth,maxBlockHeight);

	//初始化（分配存储空间，将元素全部置0）
	DataCost DataCost(maxBlockWidth,maxBlockHeight,iDspLevel);

	//计算一个定义像素值DataCost的(PixelCostComputer_Init)实例
	m_pPixelCostComputer=PixelCostComputer_Init::GetInstance();
	
	//得到最后一帧帧号
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
		//(504,540),貌似求最大块时候出现问题
	    std::cout<<"Max Block Size: "<<maxBlockWidth*maxBlockHeight<<"("<<maxBlockWidth<<","<<maxBlockHeight<<")"<<std::endl; 
		m_pImgPartition->print();

		//得到当前帧
		VDRVideoFrame *pCurrentFrame=structMotion.GetFrameAt(index);
	    

		//得到参考帧索引号
		std::vector<int> nearIndex=m_pFrameSelector->GetReferenceFrames(*pCurrentFrame,structMotion.GetFrameCount());

	  //  std::cout<<"nearIndex.size()"<<nearIndex.size()<<std::endl;

		//创建存储（参考帧）的空间
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


//定义BO函数实现
void DspEstimator_Normal::Run_BO(int start,int end,int pass)
{
	//检测是否载入图像序列,以及是否（成功）选择参考帧和进行图像分块
	if(check() == false)
		return;

	//定义SFM对象，用于读取内外参等信息（相机运动轨迹，跟踪点）
	VDRStructureMotion &structMotion = *VDRStructureMotion::GetInstance();

   //设置求disparity的层数。(m+1=101)
	int DspLevel = DepthParaWidget::GetInstance()->m_iDspLevel;
	
	//获得最大块宽度和高度（对此函数理解不够，需要再查看）
	int MaxBlockWidth, MaxBlockHeight;
	m_pImgPartition->GetMaxBlockSize(VDRVideoFrame::GetImageWidth(), VDRVideoFrame::GetImageHeight(), MaxBlockWidth, MaxBlockHeight); //504,540
	
	//初始化（分配存储空间，将元素全部置0）
	DataCost DataCost(MaxBlockWidth, MaxBlockHeight, DspLevel);

	//计算一个定义像素值DataCost的(PixelCostComputor_Refine)实例
	 m_pPixelCostComputer = PixelCostComputor_Refine::GetInstance();   


	 //======for DE ================
	 //int expanLevels = DepthParaWidget::GetInstance()->m_iSubLevels;
	 //DataCost subDataCost;
	 //if(m_bRunDEWhenBO == true){
	  // int subDspLevel = expanLevels * 2 + 1;
	  //	 subDataCost.Create(MaxBlockWidth, MaxBlockHeight, subDspLevel);
	 //}

	 //得到最后一帧帧号
	 if(end == -1)
	 {
		 end = structMotion.GetFrameCount() - 1;
		 std::cout<<"end= "<<end<<std::endl;
	 }
	 
	   //执行两遍
	 for(int passi = 0; passi < pass; ++passi)
	 {
		 //遍历所有帧
		 for(int index = start; index <= end; index++)
		 {
			 //计时器
			// clock_t beginTime = clock();
           
			 //输出（打印）帧和程序遍历信息
			 std::cout<<"Pass:"<<passi+1<<"/"<<pass<<std::endl;
			 std::cout<<"Refine Frame: "<<index<<"/("<<start<<"~"<<end<<")"<<std::endl;

			 
			 //输出（打印）块（分割）信息
			 std::cout << "Max. Block Size:" << MaxBlockWidth * MaxBlockHeight 
				 << "(" << MaxBlockWidth << "*" << MaxBlockHeight << ")" <<std::endl; 
			 m_pImgPartition->print();

			 //获取当前帧和参考帧
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

//定义DE函数实现
void DspEstimator_Normal::Run_DE(int start,int end,int pass)
{
	//检验是否正常加载工程，选择参考帧和分块
	if(check()==false)
		return;

    //定义SFM对象得到特征跟踪点，相机内外参等信息
	VDRStructureMotion &structMotion = *VDRStructureMotion::GetInstance();

	//获得DE参数
	int iters = DepthParaWidget::GetInstance()->m_iSubIter;       //计算迭代次数 m_iSubIter=2
	int expanLevels = DepthParaWidget::GetInstance()->m_iSubLevels; //m_iSubLevels=10
	int DspLevel = expanLevels * 2 + 1;              //disparity的层数

	//获得最大块的长度和宽度（504*540）
	int MaxBlockWidth, MaxBlockHeight;
	m_pImgPartition->GetMaxBlockSize(VDRVideoFrame::GetImageWidth(), VDRVideoFrame::GetImageHeight(), MaxBlockWidth, MaxBlockHeight);
	
	//初始化（分配存储空间，将元素全部置0）
	DataCost DataCost(MaxBlockWidth, MaxBlockHeight, DspLevel);

	//计算一个定义像素值DataCost的(PixelCostComputor_Refine)实例
	m_pPixelCostComputer = PixelCostComputor_Refine::GetInstance();

	if(end == -1)
		end = structMotion.GetFrameCount() - 1;   //end=140
	for(int passi = 0; passi < pass; ++passi){    //pass=2  执行两次
		for(int index = start; index <= end; index++){

		//	clock_t beginTime = clock();

		    std::cout<<"Pass: "<<passi+1<<"/"<<pass<<std::endl;
			std::cout<<"DE Frame: "<<index<<"/("<<start<<"~"<<end<<")"<<std::endl;

			std::cout << "Max. Block Size:" << MaxBlockWidth * MaxBlockHeight << "(" << MaxBlockWidth << "*" << MaxBlockHeight << ")" << std::endl; 
			m_pImgPartition->print();

           //获取当前帧和参考帧
			VDRVideoFrame *pCurrentFrame = structMotion.GetFrameAt(index);
		    std::vector<int> nearIndex = m_pFrameSelector->GetReferenceFrames(*pCurrentFrame, structMotion.GetFrameCount());   //获得参考帧的帧号
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


//在单帧上执行Init求disparity操作，被Run_Init调用
bool DspEstimator_Normal::Run_Init_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost)
{
	int imageWidth=VDRVideoFrame::GetImageWidth();
	int imageHeight=VDRVideoFrame::GetImageHeight();

	ZIntImage  labelImgInit,labelImgBP,labelImgSegm;
	labelImgInit.CreateAndInit(imageWidth,imageHeight,1,-1);      //创建imgageWidth*imageHeight大小的单通道图像，初值设为-1；
	labelImgBP.CreateAndInit(imageWidth, imageHeight, 1, -1);
	labelImgSegm.CreateAndInit(imageWidth, imageHeight, 1, -1);

    int dspLevel=DataCost.GetDspLevels();  //获取 disparity层数
    m_pPixelCostComputer->SetFrames(pCurrentFrame,&nearFrames); //设置计算帧
	
	double disck=DepthParaWidget::GetInstance()->GetTrueDisck(); //得到平滑截断项

	double dspMin=DepthParaWidget::GetInstance()->m_dDspMin;    //disparity最小值
	double dspMax=DepthParaWidget::GetInstance()->m_dDspMax;    //disparity最大值

	//double dspMax=0.160021;      //求DspMax有问题

	/******************************************************
		disparity共分为dspLevel（m+1)层,
	    对于第k层，其disparity值d.k=(m-k)/m*d.min+k/m*d.max
		k=0,……，m.
	 ********************************************************/
/*
	std::vector<double> dspV(dspLevel);
	for(auto k=0;k<dspLevel;k++)
	{
		dspV[k]=dspMin*(dspLevel-1-k)/(dspLevel-1)+dspMax*k/(dspLevel-1);      
	}

	std::cout<<"dspMax: "<<dspMax<<" dspMin: "<<dspMin<<" dspLevel； "<<dspLevel<<std::endl;
*/
	std::vector<double> dspV(dspLevel);
	for(auto i=0;i<dspLevel;i++)
		dspV[i] = dspMin * (dspLevel - 1 - i)/(dspLevel - 1)  + dspMax * i/(dspLevel - 1) ;

	std::cout << "DspMin:" << dspMin << " DspMax:" << dspMax << " DspLevel:" << dspLevel <<std::endl;
    //Load
	     //加载源（彩色）图像
	  pCurrentFrame->LoadColorImage();
	  
	  
	     //创建disparity图像
      pCurrentFrame->CreateDspImage();

	  //加载参考帧的源（彩色）图像

	//  std::cout<<"nearFrames.size()"<<nearFrames.size()<<std::endl;

	  for(auto i=0U;i<nearFrames.size();i++)
	  {
		  nearFrames[i]->LoadColorImage();
	  }

	  //定义 meanShiftSegment对象
	  MeanShiftSeg meanShiftSegm;
	  

	  //MeanShift算法参数
	  int sigmaS=DepthParaWidget::GetInstance()->m_iSegSpatial;
	  int sigmaR=DepthParaWidget::GetInstance()->m_fSegColor;
	  int minSize=DepthParaWidget::GetInstance()->m_iSegMinSize;
     
	  //std::cout<<"GetImageWidth:"<<pCurrentFrame->GetImageWidth()<<std::endl;

	  //判断载入（彩色）图像是否为空
	  if(pCurrentFrame->m_pColorImage==NULL)
	  {
		  std::cout<<"color Image is Null"<<std::endl;
	  }
	  
	  //调用MeanShift算法对图像进行分割
	  meanShiftSegm.Segment(*pCurrentFrame->m_pColorImage,sigmaS,sigmaR,minSize,pCurrentFrame->GetSegmSavePath());
	  
	  //水平和垂直方向分块数
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
			  //存储LabelImage.
   			  pCurrentFrame->SaveLabelImage(labelImgInit,dspLevel,"1.1_Init_");
			 
			  
			  //std::cout<<"dspLevel： "<<dspLevel<<std::endl;
			
			  std::cout<<std::endl;
              std::cout<<"Step1_2.BP:"<<std::endl; 
			  RefineDspByBP(disck,block,DataCost,labelImgBP,true);
			 pCurrentFrame->SetDspImg(labelImgBP,*block.m_pInnerBlock,dspMin,dspMax,dspLevel);
			  pCurrentFrame->SaveLabelImage(labelImgBP,dspLevel,"1.2_BP_");

			 std::cout<<std::endl;
			 std::cout<<"PlanFitting……"<<std::endl;
			 for(int y = block.m_pInnerBlock->m_Y; y < block.m_pInnerBlock->m_Y + block.m_pInnerBlock->m_iHeight; ++y)
			 {
				 for(int x = block.m_pInnerBlock->m_X; x < block.m_pInnerBlock->m_X + block.m_pInnerBlock->m_iWidth; ++x)
					 labelImgSegm.at(x, y) = labelImgBP.at(x, y);
			 }
			 //结合meanShift分割结构优化disparity
			 RefineDspBySegm(block,meanShiftSegm,pCurrentFrame,labelImgSegm,DataCost);
			 pCurrentFrame->SaveLabelImage(labelImgSegm,dspLevel,"1.3_SegRefine_");
		  }
	  }
	 
	  
	  //如Init操作未中途取消，则将生成raw文件保存起来
	  if(canceled == true)
		  std::cout << "Depth Recovering is canceled..." << std::endl;
	  else
		   pCurrentFrame->SaveDspImg();

	  //释放为当前帧和参考帧分配的存储空间
      for(auto i=0U; i<nearFrames.size(); i++)
			  nearFrames[i]->Clear();
	  pCurrentFrame->Clear();

	  pCurrentFrame->SaveLabelImage(labelImgSegm,dspLevel,"1_Init_final_");
	  return canceled == false;
}


//在当前帧上应用BO，优化Init所求disparity的结果，被Run_BO调用
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


	//加载当前帧和参考帧的彩色图像（以及由Init步骤求得参考帧的深度图像）
	pCurrentFrame->LoadColorImage();

	pCurrentFrame->CreateDspImage();
	for(auto i=0U; i<nearFrames.size(); i++)
	{
		nearFrames[i]->LoadColorImage();
		
		if(nearFrames[i]->LoadDepthImg() == false)
	    return false;
	}

	//***无遮挡处理, 
	m_pPixelCostComputer->SetFrames(pCurrentFrame,&nearFrames);

	double disck = DepthParaWidget::GetInstance()->GetTrueDisck();   //得到平滑截断项
   //计算不同层数上disparity值
	std::vector<double> dspV(dspLevel);
	for(auto i=0;i<dspLevel;i++)
		dspV[i] = dspMin * (dspLevel - 1 - i)/(dspLevel - 1)  + dspMax * i/(dspLevel - 1) ;

	std::cout << "DspMin:" << dspMin << " DspMax:" << dspMax << " DspLevel:" << dspLevel <<std::endl;


	//得到水平和垂直方向分块个数
	int blockCountX = m_pImgPartition->GetBlockCountX();
	int blockCountY = m_pImgPartition->GetBlockCountY();

	bool canceled = false;
	for(int blockIdy = 0; (canceled = isCanceled()) == false && blockIdy < blockCountY; blockIdy++)
	{
		for(int blockIdX = 0; (canceled = isCanceled()) == false && blockIdX < blockCountX; blockIdX++)
		{

			//逐个得到块并进行计算
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


	//释放当前帧和参考帧的存储空间
	if(canceled == true)
	{
		std::cout << "Depth Recovering is canceled..." << std::endl;
		for(auto i=0U; i<nearFrames.size(); i++)
			nearFrames[i]->Clear();
		pCurrentFrame->Clear();
		return false;
	}

	//存储BO最终所求结果
	pCurrentFrame->SaveLabelImage(labelImgBP, dspLevel,"2_BO_final_");

	return true;

}

bool DspEstimator_Normal::Run_DE_At(std::vector<VDRVideoFrame*> &nearFrames, VDRVideoFrame *pCurrentFrame, DataCost &DataCost, int iters, int expanLevls)
{
	int ImgWidth = VDRVideoFrame::GetImageWidth();   //图像宽度
	int ImgHeight =VDRVideoFrame::GetImageHeight();  //图像高度

	//disparity参数
	int dspLevel = DepthParaWidget::GetInstance()->m_iDspLevel;   //101
	double dspMin = DepthParaWidget::GetInstance()->m_dDspMin;   //最小disparity值
	double dspMax = DepthParaWidget::GetInstance()->m_dDspMax;   //最大disparity值
    

	//当前帧加载彩色图像和深度图像
	pCurrentFrame->LoadColorImage();
	if(pCurrentFrame->LoadDepthImg() == false)
	{
		std::cout << "ERROR:Can not load DspImg" << std::endl;
		return false;
	}
	
	//参考帧加载彩色图像和深度图像
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
	pCurrentFrame->InitLabelImgByDspImg(labelImgBP, dspLevel, dspMin, dspMax);   //利用BO步骤求得的disparity图像初始化labelImgBP
	
	offsetImg.Create(ImgWidth, ImgHeight);     //为offsetImage分配存储空间
	m_pPixelCostComputer->SetFrames(pCurrentFrame, &nearFrames);  //设置计算帧

	int subDsplevel = 2 * expanLevls + 1;           //subDspLevel=21
	int layerCount = dspLevel - 1;                  //layerCount=100
	//std::cout<<"subDsplevel: "<<subDsplevel<<std::endl;
   // std::cout<<"layerCount: "<<layerCount<<std::endl;

	//参数的临时备份
	//============backup=======================================================
	double backupDspSigma = m_pPixelCostComputer->GetDspSigma();             //sigma_d
	double backupDataCostWeight = m_pPixelCostComputer->GetDataCostWeight(); //DataCost的权重值
	double backupDiscK = DepthParaWidget::GetInstance()->GetTrueDisck();     //平滑截断项,参见formula[4.1]
	//=========================================================================

	double dspSigma = backupDspSigma;
	double dataCostWeight = backupDataCostWeight * expanLevls /(dspLevel - 1);  //？？
	m_pPixelCostComputer->SetDataCostWeight(dataCostWeight);                    //设置数据项权重                
	double discK = backupDiscK * expanLevls;                                    //? ?

	std::cout << "Depth expansion..." <<std::endl;

	//获得（图像的）分块索引
	int blockCountX = m_pImgPartition->GetBlockCountX();
	int blockCountY = m_pImgPartition->GetBlockCountY();
	
	std::vector<double> dspV;
	bool canceled = false;

	//循环条件为迭代两次，并且未点击Cancle按钮
	for(int iPass = 0; (canceled = isCanceled()) == false && iPass < iters; iPass++)
	{
		layerCount *= expanLevls;	    //layerCount=1000（第一次），layerCount=10000(第二次）
		//std::cout<<"layerCount: "<<layerCount<<std::endl;
		for(int y = 0; y < ImgHeight; y++)
		{
			for(int x = 0; x < ImgWidth; x++)
			{
				offsetImg.at(x, y) = std::min( layerCount - (subDsplevel - 1),std::max(0, (labelImgBP.at(x, y) - 1) * expanLevls) );   //??
				// std::cout<<"offsetImg.at(x,y): "<<offsetImg.at(x,y)<<std::endl;
			}
		}

		//disparity层数每次扩大10倍后重新初始化Label图像和分配dsparity存储向量的空间
		labelImgBP.Init(-1);
		dspV.resize(layerCount + 1);

		//不同disparity层数上disparity的具体值[d_k=(m-k)/m*d_min+k/m*d_max,k=0...m;
		for(int leveli = 0; leveli <= layerCount; ++leveli)
			dspV[leveli] = dspMin * (layerCount - leveli)/layerCount + dspMax * leveli/layerCount;

		for(int blockIdy = 0; (canceled = isCanceled()) == false && blockIdy < blockCountY; blockIdy++)
		{
			for(int blockIdX = 0; (canceled = isCanceled()) == false && blockIdX < blockCountX; blockIdX++)
			{
				//得到要处理的（图像分）块
				Block block = m_pImgPartition->GetBlockAt(blockIdX, blockIdy, ImgWidth, ImgHeight);
			//	clock_t tempTime = clock();
				std::cout<<"Init DataCost..."<<std::endl;
				GetDataCost(block, DataCost, labelImgInit, dspV, &offsetImg);   //对块初始化DataCost并得到labelImage

				  
				const Block &innerBlock = *block.m_pInnerBlock;    //分块内部区域

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

		dspSigma = std::max(0.0001 * (dspMax - dspMin), 2 * dspSigma / expanLevls);     //计算并重置sigma_d
		m_pPixelCostComputer->SetDspSigma(dspSigma);   //设置当个像素计算单位上的sigma_d

		//根据块上的label设置图像的disparity值
	    pCurrentFrame->SetDspImg(labelImgBP, Block(0, 0, ImgWidth, ImgHeight, std::shared_ptr<Block>(nullptr)), dspMin, dspMax, layerCount + 1);
		
	}

	 // recovery
      m_pPixelCostComputer->SetDspSigma(backupDspSigma);           //重置(恢复）sigma_d
	  m_pPixelCostComputer->SetDataCostWeight(backupDataCostWeight);  //重置（恢复）DataCost权重系数
	
	  if(canceled == true)
		  std::cout << "Depth Recovering is canceled..." << std::endl;
	  else
		  pCurrentFrame->SaveDspImg();     //存储disparity图像为raw文件


	  //释放内存
	  for(auto i=0U; i<nearFrames.size(); i++)
		  nearFrames[i]->Clear();
	  pCurrentFrame->Clear();

	  pCurrentFrame->SaveLabelImage(labelImgBP,layerCount + 1,"3_DE_final");     //存储DE后的结果
	  return canceled == false;
}

	//检验是否正常加载工程，选择参考帧和分块
bool DspEstimator_Normal::check()
{
	//检验是否已经加载工程
    if(VDRStructureMotion::GetInstance()->GetFrameCount()==0)
	{
		std::cout<<"Load project first,please!"<<std::endl;
        return false;
	}
    //检验是否（成功）选择参考帧和是否（成功）分块
	if(m_pFrameSelector == false || m_pImgPartition == false)
	{
		std::cerr << "Please Initialize Frame Selector or Image Partition First!" << std::endl;
		system("pause");
		return false;
	}
    return  true;
}


//Step1_1.对块初始化DataCost并得到labelImage
void DspEstimator_Normal::GetDataCost(const Block &block,DataCost &outDataCost,ZIntImage & lableImg,const std::vector<double> &dspV,ZIntImage *pOffsetImg)
{
	//单线程
	for(int y=block.m_Y;y<block.m_Y+block.m_iHeight;++y)
	{
		DataCostWorkUnit *pWorkUnit=new DataCostWorkUnit(y,block,dspV,lableImg,outDataCost,*m_pPixelCostComputer,pOffsetImg);
		pWorkUnit->Execute();
	}

}

//Step1_2.用BP优化disparity
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

   //打印（输出）参数信息
	bp.printPara();

	//运行bp
	bp.run(block, DataCost, labelImg, offsetImg);

}


//Step1_3 用meanShift所求分割结果对Disparity进行优化
void DspEstimator_Normal::RefineDspBySegm( const Block &block, const MeanShiftSeg &meanShiftSegm, VDRVideoFrame* currentFrame, ZIntImage& labelImg, DataCost& DataCost )
{

	
	class cmp
	{
	public:

		//比较两个向量的大小
		bool operator()(const std::vector<blk::Point> &v1, const std::vector<blk::Point> &v2)
		{
			return v1.size() > v2.size();
		}
	
		
	};

	std::vector<std::vector<blk::Point>> regions;
	meanShiftSegm.GetRegions(regions, *block.m_pInnerBlock);
	std::sort(regions.begin(), regions.end(), cmp());           //将块中的向量按从小到大的顺序排列

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

//在单个单元上应用Planfitting
void DspEstimator_Normal::PlanfittingOneSegm(const Block &block, const MeanShiftSeg &meanShiftSegm, const std::vector<blk::Point> &SegmPoints, VDRVideoFrame* pCurrentFrame, ZIntImage& labelImg, DataCost& dataCost)
{
	//四邻域
	static const int NeighborCount = 4;
	static const int Neighbor[NeighborCount][2] = { {0,-1}, {1,0}, {0,1}, {-1,0} };
	
	int dspLevel = dataCost.GetDspLevels();
	float disck = DepthParaWidget::GetInstance()->GetTrueDisck();
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
			layerEnergy[iLayer] += dataCost.GetValueAt(x - block.m_X,  y - block.m_Y, iLayer);

			//Neighbor
			int x1,y1;
			for(int neighbori = 0; neighbori< NeighborCount; neighbori++)
			{
				x1 = x + Neighbor[neighbori][0];
				y1 = y + Neighbor[neighbori][1];
				if(block.m_pInnerBlock->IsInBlock(x1, y1) && meanShiftSegm.GetSegmMapAt(x,y) != meanShiftSegm.GetSegmMapAt(x1,y1) && labelImg.at(x1,y1) != -1)
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
			if(block.m_pInnerBlock->IsInBlock(x1, y1) == false)
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

	double segErrRateThreshold = DepthParaWidget::GetInstance()->GetTruetSegErrRateThreshold();

	//比较并更新dis
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

//获得leveli层上的disparity值
float DspEstimator_Normal::GetDspAtLeveli( float leveli )
{
	double dspMin = DepthParaWidget::GetInstance()->m_dDspMin;
	double dspMax = DepthParaWidget::GetInstance()->m_dDspMax;
	int dspLevel = DepthParaWidget::GetInstance()->m_iDspLevel;
	return dspMin * (dspLevel - 1 - leveli)/(dspLevel - 1)  + dspMax * leveli/(dspLevel - 1) ;
}

