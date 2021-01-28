#pragma once

#include "WmlMatrix3.h"

class LSCameraParameters
{
public:
	LSCameraParameters(void);
	~LSCameraParameters(void);

	//get intrinsic parameters（K) multiple extrinsic parameters (rotation para R）
	void GetKXR(Wml::Matrix3d& KXR);


	//not's so clearly!!!!!  camPose*R+T=(0,0,0) (R为正交矩阵，其转置矩阵和逆矩阵相等）
	void InitCameraPos(){
		Wml::Vector3d tmp  = - ( m_R.Transpose() * m_T) ;
		m_CameraPos[0] = (float)tmp[0];
		m_CameraPos[1] = (float)tmp[1];
		m_CameraPos[2] = (float)tmp[2];
	}

	// get camera position
	Wml::Vector3d GetCameraPos(){
		return m_CameraPos;
	}
	
	//world coord to camera coord
	void GetCameraCoordFrmWorldCoord(Wml::Vector3d& c_pt3d, Wml::Vector3d& w_pt3d);

	//image coord to world coord
	void GetWorldCoordFrmImgCoord(double u, double v, double dsp, Wml::Vector3d& w_pt3d);

	//world coord to image coord
	void GetImgCoordFrmWorldCoord(double& out_u, double& out_v, double& out_dsp, Wml::Vector3d& w_pt3d);

	//image coord to camera coord
	void GetCameraCoordFrmImgCoord(int u, int v, double desp, Wml::Vector3d& c_pt3d);



public:
	Wml::Matrix3d m_K;      //Intrinsic Matrix (3*3)

	Wml::Matrix3d m_R;     //Rotation Maxtrix (3*3)
	Wml::Vector3d m_T;     //Translation Matrix (3*1)

	Wml::Vector3d  m_CameraPos;  //position of cameras
};

