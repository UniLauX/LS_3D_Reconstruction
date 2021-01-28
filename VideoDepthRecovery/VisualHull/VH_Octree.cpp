#include <iostream>
#include <string>
#include "VisualHull.h"
#include "LSStructureMotion.h"

void CVisualHull::OctVH()
{
	if(m_iDetailLevel < 7)
	{
		std::cout << "There is no need to use this method"<<std::endl;
	}
	//std::cout<<"m_SurFaceVoxel.size():"<<m_SurFaceVoxel.size()<<std::endl;
	//std::cout<<"m_SurefaceVoxelCells.size():"<<m_SurefaceVoxelCells.size()<<std::endl;
	m_SurFaceVoxel.clear();   
	m_SurefaceVoxelCells.clear();

	CVoxel  rootVoxel;
	rootVoxel.setup(0, 0, 0, m_iEdgeCells, m_VoxelBase);

//	std::cout<<"m_iEdgeCells:"<<m_iEdgeCells<<std::endl;
	CVoxel* eightChild[8];
	rootVoxel.divide(eightChild);
	for(int iSub = 0;iSub<8;iSub++)
	{
		m_SurFaceVoxel.push_back(eightChild[iSub]);
	   // std::cout<<eightChild[iSub]->m_iLowX<<" ";
	}  
    //std::cout<<std::endl;
	//bool isWholeInOne = true;//是否是整个物体在某一个Voxel，如果不是则可以优化
	//ORG size_t curLevelCNT = m_SurFaceVoxel.size();
	size_t curLevelCNT = 8; //voxel numbers in current level
	int iLevel;             //ith level
	const int LEFTLEVELS = 2;

//NEW ADD
	//经过循环之后，送出的是Level + 1的Voxel，但并不知道他是否属于表面
	for(iLevel = 1; iLevel <= m_iDetailLevel - LEFTLEVELS; iLevel++)
	{   //此循环中可能有bug
		std::cout<<"level:"<<iLevel<<"  ";
		std::vector<CVoxel* > nextLevelVoxel;  //vector for storing voxels which need to be carved in next level 
		for(size_t iCurLevel = 0; iCurLevel < curLevelCNT; iCurLevel++)
		{
			CVoxel* curVoxel = m_SurFaceVoxel.at(iCurLevel);
			// std::cout<<"iLevel:"<<iLevel<<std::endl;
			// std::cout<<"curVoxel:"<<curVoxel->m_iLowX<<","<<curVoxel->m_iLowY<<","<<curVoxel->m_iLowZ<<","<<curVoxel->m_iSize<<std::endl;
		    // std::cout<<"m_pVoxelBase:"<<curVoxel->m_pVoxelBase->m_dSmallestCellLength<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[0]<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[1]<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[2]<<std::endl;
			VoxelType voxelType = CheckVoxel(curVoxel, iLevel);   //decide which type of current voxel belongs to //问题可能出在此函数上
			if(voxelType==VOXEL_CROSS)
			{
				curVoxel->divide(eightChild);
				for(int iSub = 0;iSub<8;iSub++)
					nextLevelVoxel.push_back(eightChild[iSub]);
			}
			delete curVoxel;

		}  //end for iCurLevel

	   //如果发现至少有一个Voxel被分割
		//if(isWholeInOne ||  (nextLevelVoxel.size()>8) )
		//isWholeInOne = false;

		curLevelCNT = nextLevelVoxel.size();
		m_SurFaceVoxel.swap(nextLevelVoxel);

		std::cout<<"cnt:"<<m_SurFaceVoxel.size()<<std::endl;
	  //ORG std::cout << "Level:"<< iLevel << "voxel:" << m_SurFaceVoxel.size() << std::endl;
	}//end for iLevel

	//for the last level one or two level
	if(LEFTLEVELS == 1)
		LastLevel();
	else		
		LastTwoLevels();
 
	m_SurFaceVoxel.clear();
   std::cout<<"octVH..."<<std::endl;
}



/*如果存在Voxel投影在Mask之外，则把这个Voxel标记为物体之外
否则，如果Voxel都投影到Mask之内则标记为物体之内。
最后如果一个Voxel的投影或者在Mask之内，或者跨Mask边界*/
VoxelType  CVisualHull::CheckVoxel(CVoxel* curVoxel, int curLevel)
{
	bool isSomeCross = false;
	//in fact,just deal with single frame once
	for(int iFrame = m_iStartFrame; iFrame <= m_iEndFrame; iFrame++)
	{
		for(int camId= 0; camId< CamNum; camId++)
		{
			LSVideoFrame* pFrame = m_pMotion->GetFrameAt(camId, iFrame);
			VoxelType voxelType;
	        voxelType = IsVoxelInMaskFaceSamp(curVoxel, pFrame, curLevel);   //Bug may here

			if(voxelType == VOXEL_OUTSIDE)
				return VOXEL_OUTSIDE;
			if(voxelType == VOXEL_CROSS)
				isSomeCross = true;
		}
		if(isSomeCross)
			return VOXEL_CROSS;
		else return VOXEL_INSIDE;
	}
}

//NEW-ADD
VoxelType CVisualHull::IsVoxelInMaskFaceSamp(CVoxel* curVoxel, LSVideoFrame* curVideoFrame, int curLevel){

	// std::cout<<"curVoxel:"<<curVoxel->m_iLowX<<","<<curVoxel->m_iLowY<<","<<curVoxel->m_iLowZ<<","<<curVoxel->m_iSize<<std::endl;
	// std::cout<<"m_pVoxelBase:"<<curVoxel->m_pVoxelBase->m_dSmallestCellLength<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[0]<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[1]<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[2]<<std::endl;
	// std::cout<<"curLevel:"<<curLevel<<std::endl;
	int steps = 1 << ( m_iDetailLevel - curLevel );  //base voxel numbers of the current voxel contains
	int inCNT = 0;
	int outCnt = 0;
	Wml::Vector3d  lowCorner = curVoxel->getLowCorner();
	
//对于非最后一层的每一层，只要遍历该Voxel的六个面，证明六个面在Object内(那么该voxel的八个顶点也在Obect内），即可证明该Voxel在Object内。
//针对voxel在Object边缘和在Obect外的情况与在Object内类似。
	int x, y, z;
	x= 0;
	//遍历六个面，垂直于X轴的两个面
	//if(curVideoFrame->GetFaceVis(0))	
	for(y = 0; y<=steps; y++){
		for(z = 0; z<=steps; z++){
			Wml::Vector3d  offset((double)x,(double)y,(double)z);
			Wml::Vector3d curCorner = lowCorner + (offset * m_dCellLength);
			//bool isIn = curVideoFrame->IsInMask(curCorner, m_iImgApron);
			bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
			if(isIn) 
				inCNT++;
			else 
				outCnt++;

			if(inCNT>0 && outCnt>0)		
				return VOXEL_CROSS;//cross
		}
	}

	x= steps;
	//遍历六个面，垂直于X轴的两个面
	//if(curVideoFrame->GetFaceVis(1))	
	for(y = 0;y<=steps;y++){
		for(z = 0;z<=steps;z++){
			Wml::Vector3d  offset((double)x,(double)y,(double)z);
			Wml::Vector3d curCorner = lowCorner + (offset * m_dCellLength);
			//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
			bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
			if(isIn) 
				inCNT++;
			else 
				outCnt++;

			if(inCNT>0 && outCnt>0)		
				return VOXEL_CROSS;//cross
		}
	}

	//如果是最后一层则，上面的计算已经将所有的面上的顶点（共8个）遍历完
	if(curLevel < m_iDetailLevel){
		//垂直于Y轴的两个面
		y = 0;
		//if(curVideoFrame->GetFaceVis(2))	
		for(x = 0;x<=steps;x++){
			for(z = 0;z<=steps;z++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}

		y = steps;
		//if(curVideoFrame->GetFaceVis(3))	
		for(x = 0;x<=steps;x++){
			for(z = 0;z<=steps;z++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}


		z = 0;
		//if(curVideoFrame->GetFaceVis(4))
		for(y = 0;y<=steps;y++){		
			for(x = 0;x<=steps;x++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}

		z = steps;
		//if(curVideoFrame->GetFaceVis(5))
		for(y = 0;y<=steps;y++){		
			for(x = 0;x<=steps;x++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				//bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				bool isIn = curVideoFrame->inMask(curCorner[0],curCorner[1],curCorner[2],m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}
	}

	//std::cout<<"IsVoxelInMaskFaceSamp...func"<<std::endl;

	if(inCNT==0)		
		return VOXEL_OUTSIDE;//outside

	//if(outCnt==0)		return VOXEL_INSIDE;//inside	
	else return VOXEL_INSIDE;//it is only for erease the warning and is the same with the line above
}


/*  ORG
VoxelType CVisualHull::IsVoxelInMaskFaceSamp(CVoxel* curVoxel, LSVideoFrame* curVideoFrame, int curLevel){

  // std::cout<<"curVoxel:"<<curVoxel->m_iLowX<<","<<curVoxel->m_iLowY<<","<<curVoxel->m_iLowZ<<","<<curVoxel->m_iSize<<std::endl;
  // std::cout<<"m_pVoxelBase:"<<curVoxel->m_pVoxelBase->m_dSmallestCellLength<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[0]<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[1]<<","<<curVoxel->m_pVoxelBase->m_RootLowCorner[2]<<std::endl;
  // std::cout<<"curLevel:"<<curLevel<<std::endl;

	int steps = 1 << ( m_iDetailLevel - curLevel );

	int inCNT = 0;
	int outCnt = 0;
	Wml::Vector3d  lowCorner = curVoxel->getLowCorner();
	int x, y, z;
	x= 0;


	//遍历六个面，垂直于X轴的两个面
	//if(curVideoFrame->GetFaceVis(0))	
	for(y = 0; y<=steps; y++){
		for(z = 0; z<=steps; z++){
			Wml::Vector3d  offset((double)x,(double)y,(double)z);
			Wml::Vector3d curCorner = lowCorner + (offset * m_dCellLength);
			bool isIn = curVideoFrame->IsInMask(curCorner, m_iImgApron);
			if(isIn) 
				inCNT++;
			else 
				outCnt++;

			if(inCNT>0 && outCnt>0)		
				return VOXEL_CROSS;//cross
		}
	}

	x= steps;
	//遍历六个面，垂直于X轴的两个面
	//if(curVideoFrame->GetFaceVis(1))	
	for(y = 0;y<=steps;y++){
		for(z = 0;z<=steps;z++){
			Wml::Vector3d  offset((double)x,(double)y,(double)z);
			Wml::Vector3d curCorner = lowCorner + (offset * m_dCellLength);
			bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
			if(isIn) 
				inCNT++;
			else 
				outCnt++;

			if(inCNT>0 && outCnt>0)		
				return VOXEL_CROSS;//cross
		}
	}

	//如果是最后一层则，上面的计算已经将所有的面上的顶点（共8个）遍历完
	if(curLevel < m_iDetailLevel){
		//垂直于Y轴的两个面
		y = 0;
		//if(curVideoFrame->GetFaceVis(2))	
		for(x = 0;x<=steps;x++){
			for(z = 0;z<=steps;z++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}

		y = steps;
		//if(curVideoFrame->GetFaceVis(3))	
		for(x = 0;x<=steps;x++){
			for(z = 0;z<=steps;z++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}


		z = 0;
		//if(curVideoFrame->GetFaceVis(4))
		for(y = 0;y<=steps;y++){		
			for(x = 0;x<=steps;x++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}

		z = steps;
		//if(curVideoFrame->GetFaceVis(5))
		for(y = 0;y<=steps;y++){		
			for(x = 0;x<=steps;x++)
			{
				Wml::Vector3d  offset((double)x,(double)y,(double)z);
				Wml::Vector3d curCorner = lowCorner + (offset *m_dCellLength);
				bool isIn = curVideoFrame->IsInMask(curCorner,m_iImgApron);
				if(isIn) inCNT++;
				else outCnt++;

				if(inCNT>0 && outCnt>0)		
					return VOXEL_CROSS;//cross
			}
		}
	}

    //std::cout<<"IsVoxelInMaskFaceSamp...func"<<std::endl;
	
	if(inCNT==0)		
		return VOXEL_OUTSIDE;//outside

	//if(outCnt==0)		return VOXEL_INSIDE;//inside	
	else return VOXEL_INSIDE;//it is only for erease the warning and is the same with the line above
}
*/



void CVisualHull::LastLevel(){
	int lastFailedFrame = 0;
	char  voxel1X1X1[8];//Voxel它包含8个点。
	for(int iVoxel = 0; iVoxel<(int)m_SurFaceVoxel.size(); iVoxel++){
		CVoxel*  curVoxel = m_SurFaceVoxel[iVoxel];
		Wml::Vector3d  lowCorner = curVoxel->getLowCorner();
		int sum = 0;

		for(int z = 0;z<=1;z++){
			for(int y = 0;y<=1;y++){
				for(int x = 0;x<=1;x++){
					Wml::Vector3d  offset((double)x,(double)y,(double)z);
					Wml::Vector3d curCorner = lowCorner + (offset * m_dCellLength);
					lastFailedFrame = PointInMasks(curCorner,lastFailedFrame);

					if(lastFailedFrame<0){	//inside
						sum+= INSIDE;
						voxel1X1X1[z*4 + y*2 +x] = INSIDE;
						lastFailedFrame = 0;
					}
					else{
						sum += OUTSIDE;//outside
						voxel1X1X1[z*4 + y*2 +x]= OUTSIDE;
					}
				}
			}
		}
		if(sum>0  && sum< 8 ){
			CVoxel* newVoxel = new CVoxel;
			newVoxel->setup(curVoxel->m_iLowX, curVoxel->m_iLowY, curVoxel->m_iLowZ, 1, m_VoxelBase);
			CVoxelCell* newVoxelCell = new CVoxelCell(newVoxel);
			float* eightIsoValues = newVoxelCell->m_IsoValues;
			eightIsoValues[0] = (float)voxel1X1X1[0];
			eightIsoValues[1] = (float)voxel1X1X1[1];
			eightIsoValues[2] = (float)voxel1X1X1[3];//note the order is the same with the order in voxel.h file
			eightIsoValues[3] = (float)voxel1X1X1[2];
			eightIsoValues[4] = (float)voxel1X1X1[4];
			eightIsoValues[5] = (float)voxel1X1X1[5];//note the order is the same with the order in voxel.h file
			eightIsoValues[6] = (float)voxel1X1X1[7];
			eightIsoValues[7] = (float)voxel1X1X1[6]; 

			m_SurefaceVoxelCells.push_back(newVoxelCell);
		}
	}//end for each surface voxels

	for(size_t iVoxelIndex = 0; iVoxelIndex < m_SurFaceVoxel.size(); iVoxelIndex++){
		delete m_SurFaceVoxel[iVoxelIndex];
		m_SurFaceVoxel[iVoxelIndex] = NULL;
	}

	std::cout << "Level:"<< m_iDetailLevel <<" voxelcell:" << m_SurefaceVoxelCells.size() << std::endl << std::endl;
}

//此函数的逻辑可能存在Bug（原因是BDM项目中计算是以连续帧为一个计算单位，而这里是以某帧的多台相机为一个计算单位）
int CVisualHull::PointInMasks( Wml::Vector3d& w_pt3d, int lastFailedFrame ){
	int FrameCount = m_iEndFrame - m_iStartFrame + 1;
	int lastFailedM = lastFailedFrame / FrameCount;
	int lastFailedFrameIndex = lastFailedFrame % FrameCount + m_iStartFrame;

	LSVideoFrame* pFrame = m_pMotion->GetFrameAt(lastFailedM, lastFailedFrameIndex);
	//bool isObj = pFrame->IsInMask(w_pt3d ,m_iImgApron);
	bool isObj = pFrame->inMask(w_pt3d[0],w_pt3d[1],w_pt3d[2],m_iImgApron);

	if(!isObj) 
		return  lastFailedFrame;//  now -1 is inside and lastFrame is outside

	for(int iFrame = m_iStartFrame; iFrame <= m_iEndFrame; iFrame++){
	//for(int iFrame = m_iStartFrame; iFrame <1; iFrame++){
		for(int camId= 0; camId < CamNum; camId++){
			if(iFrame == lastFailedFrameIndex && camId == lastFailedM)
				continue;
			pFrame = m_pMotion->GetFrameAt(camId, iFrame);
		//	bool isObj = pFrame->IsInMask(w_pt3d ,m_iImgApron);
			bool isObj = pFrame->inMask(w_pt3d[0],w_pt3d[1],w_pt3d[2],m_iImgApron);
			if(!isObj) { 
			  lastFailedFrame = camId * (m_iEndFrame - m_iStartFrame) + (iFrame - m_iStartFrame); 
				//lastFailedFrame = camId * (0 - m_iStartFrame) + (iFrame - m_iStartFrame);  //改 
				return lastFailedFrame;
			}
		}
	}
	//in all mask
	return -1;
}

void CVisualHull::LastTwoLevels(){
	char  voxel2X2X2[3][3][3];//一个2X2X2的Voxel它包含27个点。

	int lastFailedFrame = 0;
	for(int iVoxel = 0;iVoxel<(int)m_SurFaceVoxel.size();iVoxel++){
		CVoxel*  curVoxel = m_SurFaceVoxel[iVoxel];
		Wml::Vector3d  lowCorner = curVoxel->getLowCorner();
		for(int z = 0;z<=2;z++){
			for(int y = 0;y<=2;y++){
				for(int x = 0;x<=2;x++){
					Wml::Vector3d  offset((double)x,(double)y,(double)z);
					Wml::Vector3d curCorner = lowCorner + (offset * m_dCellLength);
					lastFailedFrame = PointInMasks(curCorner, lastFailedFrame);

					if(lastFailedFrame<0){	//inside					
						voxel2X2X2[z][y][x] = INSIDE;
						lastFailedFrame = 0;
					}
					else				
						voxel2X2X2[z][y][x] = OUTSIDE;//outside
				}
			}
		}

		for(int z = 0;z<=1;z++){
			for(int y = 0;y<=1;y++){
				for(int x = 0;x<=1;x++){
					int sum = voxel2X2X2[z][y][x] + voxel2X2X2[z][y][x + 1] + voxel2X2X2[z][y + 1][x + 1] + voxel2X2X2[z][y + 1][x] + 
						voxel2X2X2[z + 1][y][x] + voxel2X2X2[z + 1][y][x + 1] + voxel2X2X2[z +1][y + 1][x + 1] + voxel2X2X2[z + 1][y + 1][x]; 
					if(sum>0  && sum< 8 )
					{
						CVoxel* newVoxel = new CVoxel;
						newVoxel->setup(x+ curVoxel->m_iLowX, y + curVoxel->m_iLowY, z + curVoxel->m_iLowZ, 1, m_VoxelBase);
						CVoxelCell* newVoxelCell = new CVoxelCell(newVoxel);
						float* eightIsoValues = newVoxelCell->m_IsoValues;
						eightIsoValues[0] = (float)voxel2X2X2[z][y][x];
						eightIsoValues[1] = (float)voxel2X2X2[z][y][x + 1];
						eightIsoValues[2] = (float)voxel2X2X2[z][y + 1][x + 1];
						eightIsoValues[3] = (float)voxel2X2X2[z][y + 1][x];
						eightIsoValues[4] = (float)voxel2X2X2[z + 1][y][x];
						eightIsoValues[5] = (float)voxel2X2X2[z + 1][y][x + 1];
						eightIsoValues[6] = (float)voxel2X2X2[z +1][y + 1][x + 1];
						eightIsoValues[7] = (float)voxel2X2X2[z + 1][y + 1][x]; 

					     
						m_SurefaceVoxelCells.push_back(newVoxelCell);  //此处不太一致！ may have bug
					}//end if check cross

				}//end for eight sub voxels in the iVoxel
			}
		}
	}//end for each surface voxels

	for(size_t iVoxelIndex = 0; iVoxelIndex < m_SurFaceVoxel.size(); iVoxelIndex++){
		delete m_SurFaceVoxel[iVoxelIndex];
		m_SurFaceVoxel[iVoxelIndex] = NULL;
	}
	std::cout <<"Level:"<< m_iDetailLevel - 1 <<","<< m_iDetailLevel <<" voxelcell:" << m_SurefaceVoxelCells.size() << std::endl << std::endl;

}
