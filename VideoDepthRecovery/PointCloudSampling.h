#pragma once
#include <iostream>
#include <string>
#include "LSStructureMotion.h"


#include <queue>
#include <iostream>

class PointCloudSampling
{
public:
	static PointCloudSampling * GetInstance()
	{
		static PointCloudSampling instance;
		return &instance;
	}

    // calculate point cloud sampling
	void PointCloudSampling::fPointCloudSampling(int iSampRds = 1);

	//get world coor from image coord
    Wml::Vector3d Reproject(LSVideoFrame* pVFrm, Wml::Vector2d p, double z);

	//visual hull checking
	bool VisualhullCheck( Wml::Vector3d& ThreeDV );
	
	//get Normal vector of PtMp
    Wml::Vector3d Normal(Wml::Vector2<int> P, const Wml::GMatrix<Wml::Vector3d>& PtMp, LSVideoFrame* pFrm);

	//get Point cloud file name path
	std::string GetPtCldFileName(LSVideoFrame* pFrm);

	//set directory
	void SetDir(const std::string& dir);



 LSStructureMotion *mMotion;
 int mStartFrame;     //start frame
 int mEndFrame;       //end frame




 std::string m_dir;       //project system dir
 std::string m_MdlFlPth;  //point Cloud temp path
 std::vector<std::string>m_NptsFlPthArr; //Npt file path
 std::vector<std::string>m_PtNormArr;    //model(obj file) path



public:
	PointCloudSampling(void);
	~PointCloudSampling(void);
};

