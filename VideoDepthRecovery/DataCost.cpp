#include "DataCost.h"


//构造函数（作用：变量初始化）
DataCost::DataCost(void)
{
	m_iWidth=0;
	m_iHeight=0;
	m_iDspLevels=0;

	m_iSlice=0;
	m_iTotal=0;
	m_pfData=NULL;
}

//（重载）构造函数（作用：变量初始化）
DataCost::DataCost(int width,int height,int dspLevel,bool setZero/*=false */)
{
	m_iWidth=0;
	m_iHeight=0;
	m_iDspLevels=0;

	Create(width,height,dspLevel,setZero);
}

// 析构函数（作用：释放存储空间）
DataCost::~DataCost(void)
{
	if(m_iTotal!=0)
		delete []m_pfData;    //释放指针所指对象占用存储空间
}


//创建所有Level图像元素存储空间（float类型)，并置其初值为0
void DataCost::Create(int width,int height,int dspLevel,bool setZero/* =false */ )
{
	if(m_iWidth==width&&m_iHeight==height&&m_iDspLevels==dspLevel)
	{
		if(setZero=true)
		  memset(m_pfData,0, m_iTotal*sizeof(float));    //将m_pfData存储元素全部置0
		return;
	}

	m_iWidth=width;
	m_iHeight=height;
	m_iDspLevels=dspLevel;


	m_iSlice=m_iWidth*m_iHeight;
	m_iTotal=m_iSlice*m_iDspLevels;


	m_pfData=new float[m_iTotal];    //创建存储空间

	if(setZero==true)
		memset(m_pfData,0,m_iTotal*sizeof(float));
}

//得到level_i上（x,y)坐标位置元素（值)
float DataCost::GetValueAt(int x,int y,int level_i)
{
    int index=level_i*m_iSlice+y*m_iWidth+x;
    if(index>m_iTotal)
	{
		std::cout<<"Error: the index of DataCost is out of range (x:"<<x<<",y:"<<",level_i:"<<level_i<<")"<<std::endl;
		system("pause");   //等待用户按任意键，然后退出
		exit(0);   //正常退出程序
        return -1;
	}
	return m_pfData[index];
}


//定义At(x,y,level_i)得到level_i上（x,y)坐标位置元素（值)
float &DataCost::At(int x,int y,int level_i)
{
	int index=level_i*m_iSlice+y*m_iWidth+x;
	if(index>m_iTotal)
	{
		std::cout<<"Error: the index of DataCost is out of range (x:"<<x<<",y:"<<",level_i:"<<level_i<<")"<<std::endl;
		system("pause");   //等待用户按任意键，然后退出
		exit(0);   //正常退出程序
		return m_pfData[0];
	}

	return m_pfData[index];  
}


//获得（x,y)坐标上的DataCostUnit值
void DataCost::GetDataCostUnit(int x,int y,DataCostUnit& dataCostUnit)
{
	int index=y*m_iWidth+x;
	if(index>m_iSlice)
	{
		std::cout<<"Error: the index of DataCost is out of range (x:"<<x<<",y:"<<")"<<std::endl;
		system("pause");   //等待用户按任意键，然后退出
		exit(0);   //正常退出程序
	}
    dataCostUnit.Init(m_pfData+index,m_iSlice,m_iDspLevels);  //(和init中的函数不是很好对应）
}



/**********************************************************************
 对于偏移量为（offsetX,offsetY)的大小为（blockWidth,blockHeight)的block,
 在其每个坐标位置上选定disparity取值的level作为其LabelImg上相应坐标的值。
 *********************************************************************/
void DataCost::SetLabelImg(int offsetX,int offsetY,int blockWidth,int blockHeight,ZIntImage &labelImg)
{
	 for(int j=0;j<blockHeight&&j+offsetY<labelImg.GetHeight();j++)
	 {
		 for(int i=0;i<blockWidth&&i+offsetX<labelImg.GetWidth();i++)
		 {
			 float minValue=1e20F;    //设置一个相对较大的最小值（作为初始化）
			 int minDi=0;

			 for(int lev=0;lev<m_iDspLevels;lev++)
			 {
				 if(GetValueAt(i,j,lev)<minValue)
				 {
					 minValue=GetValueAt(i,j,lev);
					 minDi=lev;                       //找出最小值所在的disparity层数
				 } 
			 }
             labelImg.at(offsetX+i,offsetY+j)=minDi;
		 }
	 }
}