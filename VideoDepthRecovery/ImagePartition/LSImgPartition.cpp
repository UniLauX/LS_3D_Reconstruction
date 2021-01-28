#include "LSImgPartition.h"
#include <iostream>


LSImgPartition::LSImgPartition(void)
{
	//m_fOverlap=8/100.0F;  //程序中暂时未用到块之间的叠加功能
}


LSImgPartition::~LSImgPartition(void)
{
}

// get the max width and height of max block
void LSImgPartition::GetMaxBlockSize( int& width, int& height )
{
	int trueX1, trueY1, trueX2, trueY2, x, y;

	int blockIdX_backup = m_iBlockIdX;
	int blockIdY_backup = m_iBlockIdY;

	SetBlockId(m_iBlockCountX / 2, m_iBlockCountY / 2);  //1，0
    GetBlockInfoSimple(trueX1, trueY1, trueX2, trueY2, x, y, width, height);
	SetBlockId(blockIdX_backup, blockIdY_backup);
	
	std::cout<<"GetMaxBlockSize..."<<std::endl;
}

//get output(offset)& true block coord infomation
bool LSImgPartition::GetBlockInfoSimple(int& TrueX1, int& TrueY1, int& TrueX2, int& TrueY2, int& out_X, int& out_Y, int& out_width, int& out_height)
{
	if(m_iBlockIdX >= m_iBlockCountX || m_iBlockIdY >= m_iBlockCountY){
		std::cout<<"ERROR: Block Index: ("<<m_iBlockIdX<<","<<m_iBlockIdY<<") is out of bounds!"<<std::endl;
		return false;
	}



	float blockWidth = (float)m_iImgWidth / m_iBlockCountX;  //block width
	float blockHeight = (float)m_iImgHeight / m_iBlockCountY; //block height

	//output(offset) start x,y coord on each block
	out_X = m_iBlockIdX != 0 ? blockWidth * m_iBlockIdX  - blockWidth * m_fOverlap + 0.5F : 0;
	out_Y = m_iBlockIdY != 0 ? blockHeight * m_iBlockIdY - blockHeight * m_fOverlap + 0.5F : 0;

	//true start x,y coord on each block
	TrueX1 = m_iBlockIdX != 0 ? blockWidth * m_iBlockIdX + 0.5F : 0;
	TrueY1 = m_iBlockIdY != 0 ? blockHeight * m_iBlockIdY + 0.5F : 0;

	//output(block) right bottom coord of each block
	int rightbottomX = m_iBlockIdX != m_iBlockCountX-1 ?  (blockWidth * (m_iBlockIdX + 1) - 1.0F) + blockWidth * m_fOverlap + 0.5F : m_iImgWidth-1;
	int rightbottomY = m_iBlockIdY != m_iBlockCountY-1 ?  (blockHeight *(m_iBlockIdY + 1) - 1.0F) + blockHeight * m_fOverlap + 0.5F : m_iImgHeight-1;

	//std::cout<<"rightbottomX:"<<rightbottomX<<" ,rightbottomY:"<<rightbottomY<<std::endl;

    //ture end x,y coord on each block
	TrueX2 = m_iBlockIdX != m_iBlockCountX-1 ?  blockWidth * (m_iBlockIdX + 1) - 1.0F + 0.5F : m_iImgWidth-1;
	TrueY2 = m_iBlockIdY != m_iBlockCountY-1 ?  blockHeight *(m_iBlockIdY + 1) - 1.0F + 0.5F : m_iImgHeight-1;


	//output (block) width and height
	out_width = rightbottomX - out_X + 1;
	out_height = rightbottomY - out_Y + 1;

    //std::cout<<"out_width:"<<out_width<<",out_height:"<<out_height<<std::endl;
	return true;
}


/*************************************************************************/
         /**** Get block edgePoints coord(x,y) ***/
/*************************************************************************/
bool LSImgPartition::GetBlockInfoFull( int& out_X, int& out_Y, int& out_width, int& out_height, std::vector<int>& EdgePoints, int& TrueX1, int& TrueY1, int& TrueX2, int& TrueY2)
{
	if(m_iBlockIdX >= m_iBlockCountX || m_iBlockIdY >= m_iBlockCountY){
		std::cout<<"ERROR: Block Index: ("<<m_iBlockIdX<<","<<m_iBlockIdY<<") is out of bounds!"<<std::endl;
		return false;
	}
	//float blockWidth = (float)m_iImgWidth / m_iBlockCountX;
	//float blockHeight = (float)m_iImgHeight / m_iBlockCountY;
 
	GetBlockInfoSimple(TrueX1, TrueY1, TrueX2, TrueY2, out_X, out_Y, out_width, out_height);

	int EdgePointCount = out_width*2 + out_height*2 - 4;
	EdgePoints.resize(EdgePointCount * 2);
	int px, py, pointIndex = 0;

	
	for(int y = 0; y < out_height; y++){
	  //left y
		px = out_X + 0;
		py = out_Y + y;
		EdgePoints[pointIndex++] = px;
		EdgePoints[pointIndex++] = py;

      //right y
		px = out_X + out_width - 1;
		py = out_Y + y;
		EdgePoints[pointIndex++] = px;
		EdgePoints[pointIndex++] = py;
	}

	for(int x=1; x<out_width-1; x++)
	{
		//top x
		px = out_X + x;
		py = out_Y + 0;
		EdgePoints[pointIndex++] = px;
		EdgePoints[pointIndex++] = py;

		//bottom x
		px = out_X + x;
		py = out_Y + out_height - 1;
		EdgePoints[pointIndex++] = px;
		EdgePoints[pointIndex++] = py;
	}

	if(pointIndex != EdgePointCount * 2){
		std::cout<<"ERROR: The Count of Edge Points is NOT right!"<<std::endl;
		system("pause");
	}
  
	return true;
}