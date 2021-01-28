#include "LSBeliefPropagation.h"
#include <vector>
#include <Windows.h>


const int NeighborCount = 4;   
const int Neighbor[NeighborCount][2] = { {0,-1}, {1,0}, {0,1}, {-1,0} };

//constructor
LSBeliefPropagation::LSBeliefPropagation(LSImgPartition& imgPartition,float disc_k, int max_d, 
	float sigma, int nIter,int nLevels, int* segmId) : m_ImgPartion( imgPartition )
{
	this->m_fDisck=disc_k;
	this->m_iMaxD=max_d;
	this->m_fSigma=sigma;
	this->m_iIterCount=nIter;
	this->m_iLevelsCount=nLevels;
	//this->m_iThreadsCount = iThreadsCount;
	this->m_pSegmId = segmId;	
}

LSBeliefPropagation::~LSBeliefPropagation(void)
{
}

void LSBeliefPropagation::printPara()
{
	std::cout<<"BP parameters:MaxD:"<<m_iMaxD<<", IterCount:"<<m_iIterCount<<", LevelCount:"
		<<m_iLevelsCount<<", DiscK:"<<m_fDisck<<std::endl;
}


void LSBeliefPropagation::run(DataCost& DataCost, ZIntImage& LabelImg, bool addEdgeInfo, ZIntImage* offsetImg)
{
	//image width and height
	int ImgWidth = LabelImg.GetWidth();
	int ImgHeight = LabelImg.GetHeight();

	int blockWidth, blockHeight;
	std::vector<int> EdgePoints;
	int trueX1, trueY1, trueX2, trueY2;

    //get block edge info
	m_ImgPartion.GetBlockInfoFull(m_iOffsetX, m_iOffsetY, blockWidth, blockHeight, EdgePoints, trueX1, trueY1, trueX2, trueY2);

	std::cout<<"ImgWidth:"<<ImgWidth<<",ImgHeight:"<<ImgHeight<<std::endl;
    std::cout<<"m_iOffsetX:"<<m_iOffsetX<<",m_iOffsetY:"<<m_iOffsetY<<std::endl;
    std::cout<<"blockWidth:"<<blockWidth<<",blockHeight:"<<blockHeight<<std::endl;
	std::cout<<"trueX1:"<<trueX1<<",trueY1:"<<trueY1<<",trueX2:"<<trueX2<<",trueY2:"<<trueY2<<std::endl;



	//std::tcout <<_T("Block Index:(")<< blockIDx<<_T(",")<<blockIDy<<_T("), Block Info: offsetX:")<< m_iOffsetX<<_T(", offsetY:")<< m_iOffsetY <<_T(", Width:")
	//	<< blockWidth <<_T(", Height:") << blockHeight << std::endl; 


	if(addEdgeInfo == false || (m_iBlockIdX == 0 && m_iBlockIdy == 0)){
		//m_iOffsetX = 0;
		//m_iOffsetY = 0;
		//int trueX1 = 0, trueY1 = 0, trueX2 = ImgWidth-1, trueY2 = ImgHeight-1;
		//stereoMatchingAt(DataCost, LabelImg, blockWidth, blockHeight, trueX1, trueY1, trueX2, trueY2, datacost);
		stereoMatchingAt(DataCost, LabelImg, offsetImg, blockWidth, blockHeight, trueX1, trueY1, trueX2, trueY2);
		return;
	}


	int EdgePointCount = blockWidth*2 + blockHeight*2 - 4;

	float * EdgeSmoothInfo = new float[EdgePointCount * m_iMaxD];  //m_iMaxD=101
	memset(EdgeSmoothInfo, 0, EdgePointCount * m_iMaxD * sizeof(float));

	int rightBottomX = m_iOffsetX + blockWidth - 1;
	int rightBottomY = m_iOffsetY + blockHeight - 1;

	/***  get smooth term for edge  ***/
	int px, py, x, y;
	for(int PointIndex = 0; PointIndex < EdgePointCount; PointIndex++){
		x = EdgePoints[PointIndex<<1];   //pointIndx=pointIndxe*2  0,2,4...
		y = EdgePoints[(PointIndex<<1) + 1]; // 1,3,5....
		 
		//eight neighbors
		for(int neighbori = 0; neighbori< NeighborCount; neighbori++)
		{
			px = x + Neighbor[neighbori][0];
			py = y + Neighbor[neighbori][1];

			if(px >= m_iOffsetX && px<= rightBottomX && py >= m_iOffsetY && py <= rightBottomY)
				continue;
			if(px >= 0 && px<ImgWidth && py>=0 && py<ImgHeight && LabelImg.at(px, py) != -1)
			{
				for(int di=0; di<m_iMaxD; di++){
					if(offsetImg != NULL)
						EdgeSmoothInfo[PointIndex * m_iMaxD + di] += min(abs((di + offsetImg->at(x, y)) - (LabelImg.at(px, py) 
						+ offsetImg->at(px, py))), m_fDisck);
					else
						EdgeSmoothInfo[PointIndex * m_iMaxD + di] += min(abs(di - LabelImg.at(px, py)), m_fDisck);
				}
			}
		}
	}


	//add EdgePoint's smooth cost to dataCost
	DataCostUnit DataCosti;
	for(int PointIndex = 0; PointIndex < EdgePointCount; PointIndex++){
		x = EdgePoints[PointIndex<<1] - m_iOffsetX;
		y = EdgePoints[(PointIndex<<1) + 1] - m_iOffsetY;
		//float * DataCosti = DataCost + (y * blockWidth + x) * m_iMaxD;
		DataCost.GetDataCostUnit(x, y, DataCosti);
		for(int di=0; di<m_iMaxD; di++){
			DataCosti[di] += EdgeSmoothInfo[PointIndex * m_iMaxD + di]; //smooth term
		}
	}

	//std::tcout <<_T("Block Index:(")<< blockIDx<<_T(",")<<blockIDy<<_T("), Block Info: offsetX:")<< m_iOffsetX<<_T(", offsetY:")<< m_iOffsetY <<_T(", Width:")
	//	<< blockWidth <<_T(", Height:") << blockHeight << std::endl; 

	stereoMatchingAt(DataCost, LabelImg, offsetImg, blockWidth, blockHeight, trueX1, trueY1, trueX2, trueY2);

	//recover the  dataCost
	for(int PointIndex = 0; PointIndex < EdgePointCount; PointIndex++){
		x = EdgePoints[PointIndex<<1] - m_iOffsetX;
		y = EdgePoints[(PointIndex<<1) + 1] - m_iOffsetY;
		//float * DataCosti = DataCost + (y * blockWidth + x) * m_iMaxD;
		DataCost.GetDataCostUnit(x, y, DataCosti);
		for(int di=0; di<m_iMaxD; di++){
			DataCosti[di] -= EdgeSmoothInfo[PointIndex * m_iMaxD + di];
		}
	}
	delete [] EdgeSmoothInfo;


}



void LSBeliefPropagation::stereoMatchingAt( DataCost& mDataCost, ZIntImage& LabelImg, ZIntImage* offsetImg, int blockWidth, int blockHeight, int TrueX1, int TrueY1, int TrueX2, int TrueY2)
{
	if((blockWidth>>(m_iLevelsCount-1))<=0 || (blockHeight>>(m_iLevelsCount-1))<=0)
	{
		std::cout<<"The number of levels is too large!"<<std::endl;
		return;
	}

	float **u = new float *[this->m_iLevelsCount];
	float **d = new float *[this->m_iLevelsCount];
	float **l = new float *[this->m_iLevelsCount];
	float **r = new float *[this->m_iLevelsCount];
	DataCost **datacost = new DataCost* [this->m_iLevelsCount];

	int *widths = new int[this->m_iLevelsCount];
	int *heights = new int[this->m_iLevelsCount];

	// data costs
	datacost[0] = &mDataCost;

	widths[0] = blockWidth;
	heights[0] = blockHeight;

	// data pyramid
	int preWidth = widths[0];
	int preHeight = heights[0];
	for(int i=1; i<m_iLevelsCount; i++)
	{
		int newWidth = widths[i] = (preWidth + 1) >> 1; 
		int newHeight = heights[i] = (preHeight + 1) >> 1;

		//datacost[i] = new float[newWidth * newHeight * this->m_iMaxD];
		datacost[i] = new DataCost(newWidth, newHeight, m_iMaxD, true);
		//memset(datacost[i], 0, newWidth * newHeight * this->m_iMaxD * sizeof(float));

		DataCostUnit pointerLow, pointerHigh;
		for(int y=0; y<preHeight; y++){
			for(int x=0; x<preWidth; x++){
				//float * pointerLow = datacost[i-1] + ( (preWidth * y) + x ) * this->m_iMaxD;
				//float * pointerHigh = datacost[i] + ( (newWidth * (y>>1) ) + (x>>1) ) * this->m_iMaxD ;
				datacost[i-1]->GetDataCostUnit(x, y, pointerLow);
				datacost[i]->GetDataCostUnit(x>>1, y>>1, pointerHigh);
				for(int di=0; di<m_iMaxD; di++)
					pointerHigh[di] += pointerLow[di];
			}
		}
		preWidth = newWidth;
		preHeight = newHeight;
	}

	// run CBeliefPropagation from coarse to fine
	for (int leveli = this->m_iLevelsCount-1; leveli >= 0; leveli--) 
	{
		std::cout<<"level:"<<leveli<<std::endl;
		int widthi = widths[leveli];
		int heighti = heights[leveli];

		// allocate & init memory for messages
		u[leveli] = new float [ widthi * heighti * this->m_iMaxD ];
		d[leveli] = new float [ widthi * heighti * this->m_iMaxD ];
		l[leveli] = new float [ widthi * heighti * this->m_iMaxD ];
		r[leveli] = new float [ widthi * heighti * this->m_iMaxD ];

		if (leveli == this->m_iLevelsCount-1) {
			//in the coarsest level messages are initialized to zero
			memset(u[leveli], 0 , widthi * heighti * this->m_iMaxD * sizeof(float));
			memset(d[leveli], 0 , widthi * heighti * this->m_iMaxD * sizeof(float));
			memset(l[leveli], 0 , widthi * heighti * this->m_iMaxD * sizeof(float));
			memset(r[leveli], 0 , widthi * heighti * this->m_iMaxD * sizeof(float));
		} 
		else
		{
			// initialize messages from values of previous 
			for (int y = 0; y < heighti; y++){
				for (int x = 0; x < widthi; x++){
					int offsetlow = ( (widthi * y) + x ) * this->m_iMaxD ;
					int offsethigh = ( (widths[leveli + 1] * (y>>1) ) + (x>>1) ) * this->m_iMaxD ;
					memcpy(u[leveli] + offsetlow, u[leveli+1] + offsethigh, this->m_iMaxD * sizeof(float));
					memcpy(d[leveli] + offsetlow, d[leveli+1] + offsethigh, this->m_iMaxD * sizeof(float));
					memcpy(l[leveli] + offsetlow, l[leveli+1] + offsethigh, this->m_iMaxD * sizeof(float));
					memcpy(r[leveli] + offsetlow, r[leveli+1] + offsethigh, this->m_iMaxD * sizeof(float));
				}
			}
			// delete old messages and data
			delete[] u[leveli+1];
			delete[] d[leveli+1];
			delete[] l[leveli+1];
			delete[] r[leveli+1];
			delete datacost[leveli+1];

		}
		// CBeliefPropagation
		translateMessage(u[leveli], d[leveli], l[leveli], r[leveli], datacost[leveli], widthi, heighti, offsetImg); 
	}


	GetDepth(u[0], d[0], l[0], r[0], datacost[0], LabelImg, blockWidth,  blockHeight, TrueX1, TrueY1, TrueX2, TrueY2);

	delete[] u[0]; delete[] d[0];
	delete[] l[0]; delete[] r[0];
	//delete[] datacost[0];

	delete[] u; delete[] d;
	delete[] l;	delete[] r;
	delete[] datacost;
	delete[] widths;
	delete[] heights;
 
}


// belief propagation using checkerboard update scheme
void LSBeliefPropagation::translateMessage(float* ui, float* di, float* li, float* ri, DataCost* dataCosti,int widhti, int heighti, ZIntImage* offsetImg)
{
	//if(m_iThreadsCount <=1){
		for (int t = 0; t < this->m_iIterCount; t++) {
			//clock_t start = clock();
			//printf("Iter:%d ", t);
			for (int y = 1; y < heighti-1; y++)
				for (int x = (y + t + 1) % 2 + 1; x < widhti-1; x+=2)
					translateMessageAt(x, y, ui, di, li, ri, dataCosti, widhti, offsetImg);
			//clock_t end = clock();
			//printf("-Time:%.3f s\n",(double)(end-start)/CLOCKS_PER_SEC);
		}
	//}
	/*
	else{
		for (int t = 0; t < this->m_iIterCount; t++) {
			//std::cout << "iter " << t << "\n";
			CParallelManager pm(m_iThreadsCount);
			for (int y = 1; y < heighti-1; y++) {
				CBPRefineWorkUnit* pWorkUnit = new CBPRefineWorkUnit(y, t, *this, ui, di, li, ri, dataCosti, widhti, offsetImg);
				pm.EnQueue(pWorkUnit);
			}
			pm.Run();
		}
	}
	*/
}

void LSBeliefPropagation::translateMessageAt( int x, int y, float* ui, float* di, float* li, float* ri, DataCost* dataCosti, int widhti, ZIntImage* offsetImg)
{
	int offsetLoc = ( y * widhti + x ) * this->m_iMaxD;
	DataCostUnit dataCostUnit;
	dataCosti->GetDataCostUnit(x, y, dataCostUnit);

	if( m_pSegmId == NULL){
		//msg(imRef(u, x, y+1),imRef(l, x+1, y),imRef(r, x-1, y), imRef(data, x, y), imRef(u, x, y));
		updateMessage(ui + offsetLoc + widhti * m_iMaxD, li + offsetLoc + m_iMaxD , ri + offsetLoc - m_iMaxD,
			dataCostUnit, ui + offsetLoc, offsetImg == NULL ? 0 : offsetImg->at(m_iOffsetX + x, m_iOffsetY + y-1)-offsetImg->at(m_iOffsetX + x, m_iOffsetY + y));

		//msg(imRef(d, x, y-1),imRef(l, x+1, y),imRef(r, x-1, y), imRef(data, x, y), imRef(d, x, y));
		updateMessage(di + offsetLoc - widhti * m_iMaxD, li + offsetLoc + m_iMaxD , ri + offsetLoc - m_iMaxD,
			dataCostUnit, di + offsetLoc, offsetImg == NULL ? 0 : offsetImg->at(m_iOffsetX + x, m_iOffsetY + y+1)-offsetImg->at(m_iOffsetX + x, m_iOffsetY + y));

		//msg(imRef(u, x, y+1),imRef(d, x, y-1),imRef(r, x-1, y), imRef(data, x, y), imRef(r, x, y));
		updateMessage(ui + offsetLoc + widhti * m_iMaxD, di + offsetLoc - widhti * m_iMaxD , ri + offsetLoc - m_iMaxD,
			dataCostUnit, ri + offsetLoc, offsetImg == NULL ? 0 : offsetImg->at(m_iOffsetX + x+1, m_iOffsetY + y)-offsetImg->at(m_iOffsetX + x, m_iOffsetY + y));

		//msg(imRef(u, x, y+1),imRef(d, x, y-1),imRef(l, x+1, y), imRef(data, x, y), imRef(l, x, y));
		updateMessage(ui + offsetLoc + widhti * m_iMaxD, di + offsetLoc - widhti * m_iMaxD , li + offsetLoc + m_iMaxD,
			dataCostUnit, li + offsetLoc, offsetImg == NULL ? 0 : offsetImg->at(m_iOffsetX + x-1, m_iOffsetY + y)-offsetImg->at(m_iOffsetX + x, m_iOffsetY + y));
		return;
	}

	//=========================else=============================
	std::vector<float*> neighborMsg, correspondingMsg;
	int offsetLogical = y * widhti + x;

	if(m_pSegmId[offsetLogical] == m_pSegmId[( y - 1) * widhti + x]){
		neighborMsg.push_back(di + ( ( y - 1) * widhti + x ) * this->m_iMaxD );
		correspondingMsg.push_back(ui + offsetLoc);
	}
	if(m_pSegmId[offsetLogical] == m_pSegmId[y * widhti + x + 1]){
		neighborMsg.push_back(li + ( y * widhti + x + 1 ) * this->m_iMaxD );
		correspondingMsg.push_back(ri + offsetLoc);
	}

	if(m_pSegmId[offsetLogical] == m_pSegmId[(y + 1) * widhti + x]){
		neighborMsg.push_back(ui + ( (y + 1) * widhti + x ) * this->m_iMaxD );
		correspondingMsg.push_back(di + offsetLoc);
	}
	if(m_pSegmId[offsetLogical] == m_pSegmId[y * widhti + x - 1]){
		neighborMsg.push_back(ri + ( y * widhti + x - 1 ) * this->m_iMaxD );
		correspondingMsg.push_back(li + offsetLoc);
	}

	std::vector<float> sumMsg( m_iMaxD );
	for (int di = 0; di < this->m_iMaxD; di++) {
		sumMsg[di] = dataCostUnit[di];
		for(int i=0; i<neighborMsg.size(); i++)
			sumMsg[di] += neighborMsg[i][di];
	}

	for(int dstMsgIndex = 0 ; dstMsgIndex < correspondingMsg.size(); dstMsgIndex ++){
		for (int di = 0; di < this->m_iMaxD; di++) 
			correspondingMsg[dstMsgIndex][di] = sumMsg[di] - neighborMsg[dstMsgIndex][di];
		updateMessage(correspondingMsg[dstMsgIndex]);
	}
}



void LSBeliefPropagation::updateMessage(float* srcMsg1, float* srcMsg2, float* srcMsg3, DataCostUnit& dataCost,float* dstMsg, int offset)
{
	//aggregate and find min
	float minH=1e20F;
	for (int di = 0; di < this->m_iMaxD; di++){
		dstMsg[di] = 1e20F;
	}
	for (int di = 0; di < this->m_iMaxD; di++) 
	{
		        //±®÷∏’Î¥ÌŒÛ
		float hVal = srcMsg1[di] + srcMsg2[di] + srcMsg3[di] + dataCost[di];
		int xVal = min(m_iMaxD-1, max(0, di - offset));
		hVal += abs(xVal - (di-offset));
		dstMsg[xVal] = min(dstMsg[xVal], hVal);
		if (hVal < minH)
			minH = hVal;
	}

	// dt
	for (int di = 1; di < this->m_iMaxD; di++) {
		float prev = dstMsg[di-1] + 1.0F;
		if (prev < dstMsg[di])
			dstMsg[di] = prev;
	}
	for (int di = this->m_iMaxD-2; di >= 0; di--) {
		float prev = dstMsg[di+1] + 1.0F;
		if (prev < dstMsg[di])
			dstMsg[di] = prev;
	}

	// truncate 
	minH += this->m_fDisck;
	for (int di = 0; di < this->m_iMaxD; di++)
		if (minH < dstMsg[di])
			dstMsg[di] = minH;

	// normalize
	float val = 0;
	for (int di = 0; di < this->m_iMaxD; di++) 
		val += dstMsg[di];

	val /= this->m_iMaxD;
	for (int di = 0; di < this->m_iMaxD; di++) 
		dstMsg[di] -= val;
}


void LSBeliefPropagation::updateMessage(float* dstMsg)
{
	//aggregate and find min
	float minH=1e20F;
	for (int di = 0; di < this->m_iMaxD; di++){
		if (dstMsg[di] < minH)
			minH = dstMsg[di];
	}

	// dt
	for (int di = 1; di < this->m_iMaxD; di++) {
		float prev = dstMsg[di-1] + 1.0F;
		if (prev < dstMsg[di])
			dstMsg[di] = prev;
	}
	for (int di = this->m_iMaxD-2; di >= 0; di--) {
		float prev = dstMsg[di+1] + 1.0F;
		if (prev < dstMsg[di])
			dstMsg[di] = prev;
	}

	// truncate 
	minH += this->m_fDisck;
	for (int di = 0; di < this->m_iMaxD; di++)
		if (minH < dstMsg[di])
			dstMsg[di] = minH;

	// normalize
	float val = 0;
	for (int di = 0; di < this->m_iMaxD; di++) 
		val += dstMsg[di];

	val /= this->m_iMaxD;
	for (int di = 0; di < this->m_iMaxD; di++) 
		dstMsg[di] -= val;
}


void LSBeliefPropagation::GetDepth(float* u0, float* d0, float* l0, float* r0, DataCost* dataCost, ZIntImage& DepthVisibleImg,  
	int blockWidth, int blockHeight, int TrueX1, int TrueY1, int TrueX2, int TrueY2)
{
	for (int y = 1; y < blockHeight-1; y++) {
		for (int x = 1; x < blockWidth-1; x++) {
			if(m_iOffsetX + x < TrueX1 || m_iOffsetX + x > TrueX2 || m_iOffsetY + y < TrueY1 || m_iOffsetY + y > TrueY2)
				continue;

			// keep track of best value for current pixel
			int bestD = 0;
			float bestVal = 1e20F;

			int offset = ( y * blockWidth + x ) * this->m_iMaxD;

			for (int di = 0; di < this->m_iMaxD; di++) {
				float val = 
					(u0 + offset + blockWidth * this->m_iMaxD)[di] +
					(d0 + offset - blockWidth * this->m_iMaxD)[di] +
					(l0 + offset + this->m_iMaxD)[di] +
					(r0 + offset - this->m_iMaxD)[di] +
					dataCost->GetValueAt(x, y, di); //dataCost + offset)[di];
				if (val < bestVal) {
					bestVal = val;
					bestD = di;
				}
			}
			DepthVisibleImg.SetPixel( m_iOffsetX + x, m_iOffsetY + y, 0, bestD);
		}
	}

	//Boundary
	//int u, v;
	for(int y = 0; y < blockHeight; y++){
		//if(DepthVisibleImg.at( m_iOffsetX + 0, m_iOffsetY + y) == -1)
		if(m_iOffsetX + 0 >= TrueX1 && m_iOffsetX + 0 <= TrueX2 && m_iOffsetY + y >= TrueY1 && m_iOffsetY + y <= TrueY2)
			DepthVisibleImg.at( m_iOffsetX + 0, m_iOffsetY + y) = DepthVisibleImg.at( m_iOffsetX + 1, m_iOffsetY + y);
		//if(DepthVisibleImg.at( m_iOffsetX + blockWidth-1, m_iOffsetY + y) == -1)
		if(m_iOffsetX + blockWidth-1 >= TrueX1 && m_iOffsetX + blockWidth-1 <= TrueX2 && m_iOffsetY + y >= TrueY1 && m_iOffsetY + y <= TrueY2)
			DepthVisibleImg.at( m_iOffsetX + blockWidth-1, m_iOffsetY + y) = DepthVisibleImg.at( m_iOffsetX + blockWidth-2, m_iOffsetY + y);
	}
	for(int x=0; x<blockWidth; x++){
		//if(DepthVisibleImg.at( m_iOffsetX + x, m_iOffsetY + 0) == -1)
		if(m_iOffsetX + x >= TrueX1 && m_iOffsetX + x <= TrueX2 && m_iOffsetY + 0 >= TrueY1 && m_iOffsetY + 0 <= TrueY2)
			DepthVisibleImg.at( m_iOffsetX + x, m_iOffsetY + 0) = DepthVisibleImg.at( m_iOffsetX + x, m_iOffsetY + 1);
		//if(DepthVisibleImg.at( m_iOffsetX + x, m_iOffsetY + blockHeight-1) == -1)
		if(m_iOffsetX + x >= TrueX1 && m_iOffsetX + x <= TrueX2 && m_iOffsetY + blockHeight-1 >= TrueY1 && m_iOffsetY + blockHeight-1 <= TrueY2)
			DepthVisibleImg.at( m_iOffsetX + x, m_iOffsetY + blockHeight-1) = DepthVisibleImg.at( m_iOffsetX + x, m_iOffsetY + blockHeight-2);
	}
}