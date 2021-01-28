#include "DataCost.h"


//���캯�������ã�������ʼ����
DataCost::DataCost(void)
{
	m_iWidth=0;
	m_iHeight=0;
	m_iDspLevels=0;

	m_iSlice=0;
	m_iTotal=0;
	m_pfData=NULL;
}

//�����أ����캯�������ã�������ʼ����
DataCost::DataCost(int width,int height,int dspLevel,bool setZero/*=false */)
{
	m_iWidth=0;
	m_iHeight=0;
	m_iDspLevels=0;

	Create(width,height,dspLevel,setZero);
}

// �������������ã��ͷŴ洢�ռ䣩
DataCost::~DataCost(void)
{
	if(m_iTotal!=0)
		delete []m_pfData;    //�ͷ�ָ����ָ����ռ�ô洢�ռ�
}


//��������Levelͼ��Ԫ�ش洢�ռ䣨float����)���������ֵΪ0
void DataCost::Create(int width,int height,int dspLevel,bool setZero/* =false */ )
{
	if(m_iWidth==width&&m_iHeight==height&&m_iDspLevels==dspLevel)
	{
		if(setZero=true)
		  memset(m_pfData,0, m_iTotal*sizeof(float));    //��m_pfData�洢Ԫ��ȫ����0
		return;
	}

	m_iWidth=width;
	m_iHeight=height;
	m_iDspLevels=dspLevel;


	m_iSlice=m_iWidth*m_iHeight;
	m_iTotal=m_iSlice*m_iDspLevels;


	m_pfData=new float[m_iTotal];    //�����洢�ռ�

	if(setZero==true)
		memset(m_pfData,0,m_iTotal*sizeof(float));
}

//�õ�level_i�ϣ�x,y)����λ��Ԫ�أ�ֵ)
float DataCost::GetValueAt(int x,int y,int level_i)
{
    int index=level_i*m_iSlice+y*m_iWidth+x;
    if(index>m_iTotal)
	{
		std::cout<<"Error: the index of DataCost is out of range (x:"<<x<<",y:"<<",level_i:"<<level_i<<")"<<std::endl;
		system("pause");   //�ȴ��û����������Ȼ���˳�
		exit(0);   //�����˳�����
        return -1;
	}
	return m_pfData[index];
}


//����At(x,y,level_i)�õ�level_i�ϣ�x,y)����λ��Ԫ�أ�ֵ)
float &DataCost::At(int x,int y,int level_i)
{
	int index=level_i*m_iSlice+y*m_iWidth+x;
	if(index>m_iTotal)
	{
		std::cout<<"Error: the index of DataCost is out of range (x:"<<x<<",y:"<<",level_i:"<<level_i<<")"<<std::endl;
		system("pause");   //�ȴ��û����������Ȼ���˳�
		exit(0);   //�����˳�����
		return m_pfData[0];
	}

	return m_pfData[index];  
}


//��ã�x,y)�����ϵ�DataCostUnitֵ
void DataCost::GetDataCostUnit(int x,int y,DataCostUnit& dataCostUnit)
{
	int index=y*m_iWidth+x;
	if(index>m_iSlice)
	{
		std::cout<<"Error: the index of DataCost is out of range (x:"<<x<<",y:"<<")"<<std::endl;
		system("pause");   //�ȴ��û����������Ȼ���˳�
		exit(0);   //�����˳�����
	}
    dataCostUnit.Init(m_pfData+index,m_iSlice,m_iDspLevels);  //(��init�еĺ������Ǻܺö�Ӧ��
}



/**********************************************************************
 ����ƫ����Ϊ��offsetX,offsetY)�Ĵ�СΪ��blockWidth,blockHeight)��block,
 ����ÿ������λ����ѡ��disparityȡֵ��level��Ϊ��LabelImg����Ӧ�����ֵ��
 *********************************************************************/
void DataCost::SetLabelImg(int offsetX,int offsetY,int blockWidth,int blockHeight,ZIntImage &labelImg)
{
	 for(int j=0;j<blockHeight&&j+offsetY<labelImg.GetHeight();j++)
	 {
		 for(int i=0;i<blockWidth&&i+offsetX<labelImg.GetWidth();i++)
		 {
			 float minValue=1e20F;    //����һ����Խϴ����Сֵ����Ϊ��ʼ����
			 int minDi=0;

			 for(int lev=0;lev<m_iDspLevels;lev++)
			 {
				 if(GetValueAt(i,j,lev)<minValue)
				 {
					 minValue=GetValueAt(i,j,lev);
					 minDi=lev;                       //�ҳ���Сֵ���ڵ�disparity����
				 } 
			 }
             labelImg.at(offsetX+i,offsetY+j)=minDi;
		 }
	 }
}