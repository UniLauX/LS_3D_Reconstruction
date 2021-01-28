#pragma once
#include "Block.h"
#include "ImgPartitionBase.h"
class ImgPartiton:public ImgPartitionBase
{
public:
	ImgPartiton(void);


public:

	//�õ�һ����̬����ʵ���������ⲿ����
	static ImgPartiton * GetInstance()
	{
		static ImgPartiton instance;
		return &instance;
	}
      
	//��������
	~ImgPartiton(void);

public:

	//�ֿ����
	void GetBlockStrategy(double oriImgWidth,double oriImgHeight,int dspLayer,double overlap,int &BlockCountX,int &BlockCountY) override;

	virtual Block GetBlockAt(int BlockIdX,int BlockIdY,int iImgWidht,int iImgHeight) override;


	//�����ӡ���˺���ִ�У���Ϣ
	virtual void print() override;
};

