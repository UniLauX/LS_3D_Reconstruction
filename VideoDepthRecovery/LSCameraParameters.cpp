#include "LSCameraParameters.h"

#include <iostream>

LSCameraParameters::LSCameraParameters(void)
{
}


LSCameraParameters::~LSCameraParameters(void)
{
}


void LSCameraParameters::GetKXR( Wml::Matrix3d& KXR )
{
	//std::cout<<"m_K: "<<m_K[0][0];
	//std::cout<<" "<<"m_R: "<<m_R[0][0]<<std::endl;
	KXR = m_K * m_R;
/*
	for(int r=0;r<3;r++)
	{
		for(int c=0;c<3;c++)
		{
			std::cout<<m_K[r][c]<<" ";
		}
		std::cout<<std::endl;
	}

std::cout<<"Got K*R"<<std::endl;
*/

}

//W to C
void LSCameraParameters::GetCameraCoordFrmWorldCoord( Wml::Vector3d& c_pt3d, Wml::Vector3d& w_pt3d )
{
	for(int i=0; i<3; ++i){
		c_pt3d[i] = 0;
		for(int j=0; j<3; ++j)
			c_pt3d[i] += m_R(i,j) * w_pt3d[j];
		c_pt3d[i] += m_T[i];
	}
}

//I to W
void LSCameraParameters::GetWorldCoordFrmImgCoord(double u, double v, double dsp, Wml::Vector3d& w_pt3d)
{
	Wml::Vector3d c_pt3d;
	c_pt3d[0] = u;
	c_pt3d[1] = v;
	c_pt3d[2] = 1.0/dsp;

	c_pt3d[1] = (c_pt3d[1]-m_K(1,2)) * c_pt3d[2] / m_K(1,1); 
	//c_pt3d[0] = (c_pt3d[0]-m_K(0,2)) * c_pt3d[2] / m_K(0,0); //skew is zero
	c_pt3d[0] = (c_pt3d[0]*c_pt3d[2]-m_K(0,1)*c_pt3d[1]-m_K(0,2) * c_pt3d[2]) / m_K(0,0); //skew not equal to zero

	
	////CameraCoordToWorldCoord
	Wml::Matrix3d RT = m_R.Transpose();
	for(int i=0; i<3; ++i){
		w_pt3d[i] = 0;
		for(int j=0; j<3; ++j)
			w_pt3d[i] += RT(i,j) * ( c_pt3d[j] - m_T[j]);
	}
}

//W to I
void LSCameraParameters::GetImgCoordFrmWorldCoord(double& out_u, double& out_v, double& out_dsp, Wml::Vector3d& w_pt3d)
{
	Wml::Vector3d c_pt3d;
	GetCameraCoordFrmWorldCoord(c_pt3d, w_pt3d);

	//out_u = m_K(0,0) * c_pt3d[0] / c_pt3d[2] + m_K(0,2); //skew equal to zero
	out_u = (m_K(0,0) * c_pt3d[0]+m_K(0,1)*c_pt3d[1]) / c_pt3d[2] + m_K(0,2);  //skew not equal to zero
	out_v = m_K(1,1) * c_pt3d[1] / c_pt3d[2] + m_K(1,2);

	out_dsp = 1.0 / c_pt3d[2];
}

//I to C
void LSCameraParameters::GetCameraCoordFrmImgCoord(int u, int v, double desp, Wml::Vector3d& c_pt3d)
{
	c_pt3d[0] = u;
	c_pt3d[1] = v;
	c_pt3d[2] = 1.0/desp;
	//c_pt3d[2] = 1.0 /( m_fDMin + (m_fDMax - m_fDMin) / m_iDepthLevelCount * DepthLeveli );

	//Calibrate image position, assuming skew = 0
	c_pt3d[1] = (c_pt3d[1]-m_K(1,2)) * c_pt3d[2] / m_K(1,1);

	//c_pt3d[0] = (c_pt3d[0]-m_K(0,2)) * c_pt3d[2] / m_K(0,0); //skew is equalto zero

	c_pt3d[0] = (c_pt3d[0]*c_pt3d[2]-m_K(0,1)*c_pt3d[1]-m_K(0,2) * c_pt3d[2]) / m_K(0,0); //skew not equal to zero
}