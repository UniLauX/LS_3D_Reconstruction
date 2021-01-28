#pragma once
#include "Block.h"
#include "ImgPartitionBase.h"
class ImgPartiton:public ImgPartitionBase
{
public:
	ImgPartiton(void);


public:

	//得到一个静态对象实例，便于外部引用
	static ImgPartiton * GetInstance()
	{
		static ImgPartiton instance;
		return &instance;
	}
      
	//析构函数
	~ImgPartiton(void);

public:

	//分块策略
	void GetBlockStrategy(double oriImgWidth,double oriImgHeight,int dspLayer,double overlap,int &BlockCountX,int &BlockCountY) override;

	virtual Block GetBlockAt(int BlockIdX,int BlockIdY,int iImgWidht,int iImgHeight) override;


	//输入打印（此函数执行）信息
	virtual void print() override;
};

