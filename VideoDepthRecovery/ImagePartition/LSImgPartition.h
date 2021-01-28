#pragma once

#include <vector>
#include <iostream>
class LSImgPartition
{
public:
	LSImgPartition(void);
	~LSImgPartition(void);


	// return a static obejct of the class
	static LSImgPartition * GetInstance()
	{
		static LSImgPartition instance;
		return &instance;
	}

	// set image width and height
	void SetImgSize(int width, int height){
		m_iImgWidth = width;
		m_iImgHeight = height;
	}
	
	// get width and height of max block
	void GetMaxBlockSize(int& width, int& height);

	// set block index in x ray and y ray
	void SetBlockId(int BlockIdX, int BlockIdY){
		m_iBlockIdX = BlockIdX;
		m_iBlockIdY = BlockIdY;
	}
	
	// get block numbers on x ray
	int GetBlockCountX(){
		return m_iBlockCountX;
	}
	// get block numbers on y ray
	int GetBlockCountY(){
		return m_iBlockCountY;
	}


	//set block numbers in x ray and y ray
	void SetBlocksCount(int xCount, int yCount){
		m_iBlockCountX = xCount;
		m_iBlockCountY = yCount;
	}


   
	//get output& true block coord infomation
	bool GetBlockInfoSimple(int& TrueX1, int& TrueY1, int& TrueX2, int& TrueY2, int& out_X, int& out_Y, int& out_width, int& out_height);

	//get more information about block(include edge information)
	bool GetBlockInfoFull(int& out_X, int& out_Y, int& out_width, int& out_height, std::vector<int>& EdgePoints, int& TrueX1, int& TrueY1, int& TrueX2, int& TrueY2);

private:
	float m_fOverlap;//  overlap between different blocks

	int m_iImgWidth;  // image width
	int m_iImgHeight; // image height

	int m_iBlockIdX;  //block index in x ray
	int m_iBlockIdY;  //block index in y ray

	int m_iBlockCountX;  // block numbers in x ray
	int m_iBlockCountY;  // block numbers in y ray

};

