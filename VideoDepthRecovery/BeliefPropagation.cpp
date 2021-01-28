#include "BeliefPropagation.h"

/*
BeliefPropagation::BeliefPropagation(void)
{
}
*/

//���캯��������ֵ����ʼ��
BeliefPropagation::BeliefPropagation(float disc_k, int max_d, float sigma, int nIter,int nLevels/*, int iThreadsCount*/)
{
	this->m_fDisck=disc_k;
	this->m_iMaxD=max_d;
	this->m_fSigma=sigma;
	this->m_iIterCount=nIter;
	this->m_iLevelsCount=nLevels;
	//this->m_iThreadsCount = iThreadsCount;
}

//��������
BeliefPropagation::~BeliefPropagation(void)
{
}

//��ӡ�������������Ϣ
void BeliefPropagation::printPara()
{
	std::cout<<"BP parameters:MaxD:"<<m_iMaxD<<", IterCount:"<<m_iIterCount<<", LevelCount:"
		<<m_iLevelsCount<<", DiscK:"<<m_fDisck<<std::endl;
}

//����BP(�������ݽ������ȣ�
void BeliefPropagation::run(const Block &block, DataCost& dataCost, ZIntImage& LabelImg, ZIntImage* offsetImg /* = NULL */ )
{

	//�ж�m_ilLevelsCount(=5,��������˹�������Ĳ���)���Ƿ�̫��
	if((block.m_iWidth>>(m_iLevelsCount-1))<=0 || (block.m_iHeight>>(m_iLevelsCount-1))<=0){
		std::cout << "The nuber of levels is too large!" <<std::endl;
		return;
	}

	//Ϊ�����洢����������ռ䣩
	float **u = new float *[this->m_iLevelsCount];
	float **d = new float *[this->m_iLevelsCount];
	float **l = new float *[this->m_iLevelsCount];
	float **r = new float *[this->m_iLevelsCount];

	DataCost **datacost = new DataCost* [this->m_iLevelsCount];

	int *widths = new int[this->m_iLevelsCount];
	int *heights = new int[this->m_iLevelsCount];


	// data costs
	datacost[0] = &dataCost;

	widths[0] = block.m_iWidth;
	heights[0] = block.m_iHeight;

	// data pyramid���������ݸ�˹��������
	int preWidth = widths[0];
	int preHeight = heights[0];
	for(int i=1; i<m_iLevelsCount; i++)
	{
		int newWidth = widths[i] = (preWidth + 1) >> 1;    //ͼ��ĳߴ�ÿ�α�Ϊԭ�ȵ�һ��
		int newHeight = heights[i] = (preHeight + 1) >> 1;

		//datacost[i] = new float[newWidth * newHeight * this->m_iMaxD];
		datacost[i] = new DataCost(newWidth, newHeight, m_iMaxD, true);   //��ʼ���������洢�ռ�
		//memset(datacost[i], 0, newWidth * newHeight * this->m_iMaxD * sizeof(float));

		DataCostUnit pointerLow, pointerHigh;
		for(int y=0; y<preHeight; y++){
			for(int x=0; x<preWidth; x++){
				//float * pointerLow = datacost[i-1] + ( (preWidth * y) + x ) * this->m_iMaxD;
				//float * pointerHigh = datacost[i] + ( (newWidth * (y>>1) ) + (x>>1) ) * this->m_iMaxD ;
				                                                      //���㲻ͬ�������ϵ�DataCostUnit(�ڸ�һ���ϣ������ģ2���ԭ����һ�룩
				datacost[i-1]->GetDataCostUnit(x, y, pointerLow);       //�õ������ݵ�ַ���ò����ظ�����width*height),disparity������
				datacost[i]->GetDataCostUnit(x>>1, y>>1, pointerHigh);
				for(int di=0; di<m_iMaxD; di++)                      //m_iMaxD=101  ��low ָ��ײ㣬�ֱ��ʽϸߵĲ�����
					pointerHigh[di] += pointerLow[di];            //����������������ϵĲ�ͬdispariryֵ����Ӧ�����
			}
		}
		preWidth = newWidth;                    //��ֵ����preWidth��preHeight

		preHeight = newHeight;
	}

	// run BeliefPropagation from coarse to fine�� Coarse_to_fine������
	for (int leveli = this->m_iLevelsCount-1; leveli >= 0; leveli--) 
	{
		std::cout<<"level:" <<leveli<<std::endl;         //4,3,2,1,0
		int widthi = widths[leveli];
		int heighti = heights[leveli];


		 //Ϊÿ����������䣨�洢��Ϣ�ģ��ռ�Ϊwidthi*heighti*m_iMaxD
		// allocate & init memory for messages
		u[leveli] = new float [ widthi * heighti * this->m_iMaxD ];
		d[leveli] = new float [ widthi * heighti * this->m_iMaxD ];
		l[leveli] = new float [ widthi * heighti * this->m_iMaxD ];
		r[leveli] = new float [ widthi * heighti * this->m_iMaxD ];

		if (leveli == this->m_iLevelsCount-1) {

			//�ڷֱ�����͵�һ�㣨��5-1�㣬highest���ϣ���Ϣ����ʼ��Ϊ0
			//in the coarsest level messages are initialized to zero 
			memset(u[leveli], 0 , widthi * heighti * this->m_iMaxD * sizeof(float));
			memset(d[leveli], 0 , widthi * heighti * this->m_iMaxD * sizeof(float));
			memset(l[leveli], 0 , widthi * heighti * this->m_iMaxD * sizeof(float));
			memset(r[leveli], 0 , widthi * heighti * this->m_iMaxD * sizeof(float));
		} 
		else
		{
			// initialize messages from values of previous 
			//���ͷֱ��ʣ�high��)�ϵ����ݿ������߷���(low�㣩��
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
			//ɾ��������Դ���ݣ��ͷֱ�㣬high�㣩
			// delete old messages and data
			delete[] u[leveli+1];
			delete[] d[leveli+1];
			delete[] l[leveli+1];
			delete[] r[leveli+1];
			delete datacost[leveli+1];

		}
		// CBeliefPropagation
		translateMessage(u[leveli], d[leveli], l[leveli], r[leveli], datacost[leveli], widthi, heighti, offsetImg, block); 
	}
	
	GetDepth(block, u[0], d[0], l[0], r[0], datacost[0], LabelImg);
   
//�ͷ��ڴ�
	delete[] u[0]; delete[] d[0];
	delete[] l[0]; delete[] r[0];
	//delete[] datacost[0];

	delete[] u; delete[] d;
	delete[] l;	delete[] r;
	delete[] datacost;
	delete[] widths;
	delete[] heights;
}

void BeliefPropagation:: translateMessage(float* ui, float* di, float* li, float* ri, DataCost* dataCosti, int widthi, int heighti, ZIntImage* offsetImg, const Block &block)
{
	for (int t = 0; t < this->m_iIterCount; t++)     //�������������Ĭ��Ϊ5�Σ�
	{
		//clock_t start = clock();
		//printf("Iter:%d ", t);

		for (int y = 1; y < heighti-1; y++)
			for (int x = (y + t + 1) % 2 + 1; x < widthi-1; x+=2)     //�Դ˱�ﲻ���
				translateMessageAt(x, y, ui, di, li, ri, dataCosti, widthi, offsetImg, block);
		         
		//clock_t end = clock();
		//printf("-Time:%.3f s\n",(double)(end-start)/CLOCKS_PER_SEC);
	}



}

void BeliefPropagation::translateMessageAt(int x, int y, float* ui, float* di, float* li, float* ri, DataCost* dataCosti, int widhti, ZIntImage* offsetImg, const Block &block)
{
	int offsetLoc = ( y * widhti + x ) * this->m_iMaxD;
	DataCostUnit dataCostUnit;
	dataCosti->GetDataCostUnit(x, y, dataCostUnit);

	//msg(imRef(u, x, y+1),imRef(l, x+1, y),imRef(r, x-1, y), imRef(data, x, y), imRef(u, x, y));
	updateMessage(ui + offsetLoc + widhti * m_iMaxD, li + offsetLoc + m_iMaxD , ri + offsetLoc - m_iMaxD,
		dataCostUnit, ui + offsetLoc, offsetImg == NULL ? 0 : offsetImg->at(block.m_X + x, block.m_Y + y-1)-offsetImg->at(block.m_X + x, block.m_Y + y));

	//msg(imRef(d, x, y-1),imRef(l, x+1, y),imRef(r, x-1, y), imRef(data, x, y), imRef(d, x, y));
	updateMessage(di + offsetLoc - widhti * m_iMaxD, li + offsetLoc + m_iMaxD , ri + offsetLoc - m_iMaxD,
		dataCostUnit, di + offsetLoc, offsetImg == NULL ? 0 : offsetImg->at(block.m_X + x, block.m_Y + y+1)-offsetImg->at(block.m_X + x, block.m_Y + y));

	//msg(imRef(u, x, y+1),imRef(d, x, y-1),imRef(r, x-1, y), imRef(data, x, y), imRef(r, x, y));
	updateMessage(ui + offsetLoc + widhti * m_iMaxD, di + offsetLoc - widhti * m_iMaxD , ri + offsetLoc - m_iMaxD,
		dataCostUnit, ri + offsetLoc, offsetImg == NULL ? 0 : offsetImg->at(block.m_X + x+1, block.m_Y + y)-offsetImg->at(block.m_X + x, block.m_Y + y));

	//msg(imRef(u, x, y+1),imRef(d, x, y-1),imRef(l, x+1, y), imRef(data, x, y), imRef(l, x, y));
	updateMessage(ui + offsetLoc + widhti * m_iMaxD, di + offsetLoc - widhti * m_iMaxD , li + offsetLoc + m_iMaxD,
		dataCostUnit, li + offsetLoc, offsetImg == NULL ? 0 : offsetImg->at(block.m_X + x-1, block.m_Y + y)-offsetImg->at(block.m_X + x, block.m_Y + y));
	return;
}
//����BP��ĳһ���ϵ���Ϣ
void BeliefPropagation::updateMessage(float* srcMsg1, float* srcMsg2, float* srcMsg3, DataCostUnit& dataCost,float* dstMsg, int offset)
{
	//aggregate and find min
	float minH=1e20F;
	for (int di = 0; di < this->m_iMaxD; di++){
		dstMsg[di] = 1e20F;
	}
	for (int di = 0; di < this->m_iMaxD; di++) 
	{
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


void BeliefPropagation::GetDepth(const Block &block, float* u0, float* d0, float* l0, float* r0, DataCost* dataCost, ZIntImage& DepthVisibleImg)
{
	for (int y = 1; y < block.m_iHeight-1; y++) {
		for (int x = 1; x < block.m_iWidth-1; x++) {
			if(block.m_pInnerBlock->IsInBlock(block.m_X + x, block.m_Y + y) == false)
				continue;

			// keep track of best value for current pixel
			int bestD = 0;
			float bestVal = 1e20F;

			int offset = ( y * block.m_iWidth + x ) * m_iMaxD;
			for (int di = 0; di < this->m_iMaxD; di++) {
				float val = 
					(u0 + offset + block.m_iWidth * m_iMaxD)[di] +
					(d0 + offset - block.m_iWidth * m_iMaxD)[di] +
					(l0 + offset + m_iMaxD)[di] +
					(r0 + offset - m_iMaxD)[di] +
					dataCost->GetValueAt(x, y, di); //dataCost + offset)[di];
				if (val < bestVal) {
					bestVal = val;
					bestD = di;
				}
			}
			DepthVisibleImg.SetPixel( block.m_X + x, block.m_Y + y, 0, bestD);
		}
	}

	//Boundary
	//int u, v;
	for(int y = 0; y < block.m_iHeight; y++){
		if(block.m_pInnerBlock->IsInBlock(block.m_X + 0, block.m_Y + y) == true )
			DepthVisibleImg.at( block.m_X + 0, block.m_Y + y) = DepthVisibleImg.at( block.m_X + 1, block.m_Y + y);
		if(block.m_pInnerBlock->IsInBlock(block.m_X + block.m_iWidth-1, block.m_Y + y) == true )
			DepthVisibleImg.at( block.m_X + block.m_iWidth-1, block.m_Y + y) = DepthVisibleImg.at( block.m_X + block.m_iWidth-2, block.m_Y + y);
	}
	for(int x=0; x<block.m_iWidth; x++){
		//if(DepthVisibleImg.at( block.m_X + x, block.m_Y + 0) == -1)
		if(block.m_pInnerBlock->IsInBlock(block.m_X + x, block.m_Y + 0) == true)
			DepthVisibleImg.at( block.m_X + x, block.m_Y + 0) = DepthVisibleImg.at( block.m_X + x, block.m_Y + 1);
		//if(DepthVisibleImg.at( block.m_X + x, block.m_Y + block.m_iHeight-1) == -1)
		if(block.m_pInnerBlock->IsInBlock(block.m_X + x, block.m_Y + block.m_iHeight-1) == true)
			DepthVisibleImg.at( block.m_X + x, block.m_Y + block.m_iHeight-1) = DepthVisibleImg.at( block.m_X + x, block.m_Y + block.m_iHeight-2);
	}
}
