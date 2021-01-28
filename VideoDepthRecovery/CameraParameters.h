#pragma once

#include "WmlMatrix3.h"
#include "WmlMatrix4.h"
  ////*** 进行图像坐标、相机坐标、世界坐标之间的相互转换***///
#include <iostream>
class CameraParameters
{
public:
	//声明构造函数，初始化内参
	 CameraParameters(const Wml::Matrix3d &K, const Wml::Matrix3d &Scaled_K):m_K(K),m_Scaled_K(Scaled_K){}
	
	//析构函数
	~CameraParameters(void);

	 //void GetWorldCoordFromImageCoord(int u,int v,double dsp,Wml::Vector3d & w_pt3d,bool scaled);
	 void GetWorldCoordFromImageCoord(double u,double v,double dsp,Wml::Vector3d & w_pt3d,bool scaled);
	 void GetImageCoordFromWorldCoord(double &out_u,double &out_v,double &out_dsp,Wml::Vector3d &w_pt3d,bool scaled);


	 void GetCameraCoordFromImageCoord(double u,double v,double dsp,Wml::Vector3d &c_pt3d,bool scaled);
	 void GetCameraCoordFromWorldCoord(Wml::Vector3d &c_pt3d,Wml::Vector3d &w_pt3d);

	 //将外参矩阵R转置存储，并外参矩阵T中存储相机中心位置（-R_tanspose*T)
	 void InverseExternalParameters();


	 void GetKXR( Wml::Matrix3d& KXR );

	 //得到内参矩阵
	 const Wml::Matrix3d *GetK(bool scaled)const
	 {
		 return scaled? &m_Scaled_K: &m_K;
	 }

	 //得到相机位置
	
		 Wml::Vector3d GetCameraPostion()const
	 {
		 return -m_R*m_T;                    //m_R需要转置，因为正确的求相机位置表达式为-R_transpose*T
	 }


//for Ls
	  Wml::Vector3d GetCameraPos()const
	  {
		 return -(m_R.Transpose()*m_T);                    //m_R需要转置，因为正确的求相机位置表达式为-R_transpose*T
	  }

public:

	//外参
	Wml::Matrix3d m_R;       // 旋转矩阵R(Rotation Matrix)
	Wml::Vector3d m_T;       // 平移矩阵T(Translation Matrix)
	


	//lightstage内参
    Wml::Matrix3d Lm_K;
	 
	//深度内参
    const	Wml::Matrix3d &m_K;      // 内参矩阵（Intrinsic Matrix)
	const   Wml::Matrix3d &m_Scaled_K; //内参放缩矩阵（Intrinsic Scaled Matrix)

};


//从图像坐标(x,y,1)转换到世界坐标(X,Y,Z)
inline void CameraParameters::GetWorldCoordFromImageCoord(double u,double v,double dsp,Wml::Vector3d & w_pt3d,bool scaled)
{
	/*
	Wml::Vector3d c_pt3d;
	GetCameraCoordFromImageCoord(u,v,dsp,c_pt3d,scaled);
	
	//(相机坐标转换到世界坐标）tranfer the camera coordinate to world coordinate
	
	//Xcam=R*Xworld+T
	//Xworld=R_transpose*(Xcam-T)

       Wml::Matrix3d m_R_tranpose=m_R.Transpose();
	   for(int row=0;row<3;++row)
	   {
		   w_pt3d[row]=0;
		   for(int col=0;col<3;++col)
		   {
			   w_pt3d[row]+=m_R_tranpose(row,col)*(c_pt3d[col]-m_T[col]);
		   }
		   
	   }
	  */
	Wml::Vector3d c_pt3d;
	GetCameraCoordFromImageCoord(u, v, dsp, c_pt3d, scaled);
	//CameraCoordToWorldCoord
	Wml::Matrix3d RT = m_R.Transpose();
	for(int i=0; i<3; ++i){
		w_pt3d[i] = 0;
		for(int j=0; j<3; ++j)
			w_pt3d[i] += RT(i,j) * ( c_pt3d[j] - m_T[j]);

		//std::cout<<"M_T"<<m_T[i]<<std::endl;
	}
}

//图像坐标到相机坐标，二维到三维。（x,y,1)-->(Xcam,Ycam,f)
inline void CameraParameters::GetCameraCoordFromImageCoord(double u,double v,double dsp,Wml::Vector3d &c_pt3d,bool scaled)
{
	/*
	//u,v分别为图像上二维坐标点x,y
	//c_pt3d[0],c_pt3d[1],c_pt3d[2]则为Xcam, Ycam和f;
	c_pt3d[0]=u;
	c_pt3d[1]=v;
	c_pt3d[2]=1.0/dsp;        // z=lamda*f=1*f=1/d;         (f为焦距，焦距和disparity互为倒数）

	if(scaled==false)
	{
		
		//   Xcam*Kx=X-X0;
		//   Xcam=(X-X0)/Kx;
		
		//Xcam=(X-X0)/Kx=(X-X0)/(Alpha_x/f)=(X-X0)*f/Alpha_x;
		c_pt3d[0]=(c_pt3d[0]-m_K(0,2))*c_pt3d[2]/m_K(0,0);

		//Ycam=(Y-Y0)/Ky=(Y-Y0)/(Alpha_y/f)=(Y-Y0)*f/Alpha_y
		c_pt3d[1]=(c_pt3d[1]-m_K(1,2))*c_pt3d[2]/m_K(1,1);
	}
	else
	{
		c_pt3d[0]=(c_pt3d[0]-m_Scaled_K(0,2))*c_pt3d[2]/m_K(0,0);
		c_pt3d[1]=(c_pt3d[1]-m_Scaled_K(1,2))*c_pt3d[2]/m_K(1,1);
	}
	*/
	c_pt3d[0] = u;
	c_pt3d[1] = v;
	c_pt3d[2] = 1.0/dsp;

	//std::cout<<"dsp:"<<dsp<<std::endl;
	//c_pt3d[2] = 1.0 /( m_fDMin + (m_fDMax - m_fDMin) / m_iDepthLevelCount * DepthLeveli );
	//std::cout<<"M_K"<<m_K(0,0)<<","<<m_K(1,1)<<","<<m_K(0,1)<<","<<m_K(0,2)<<std::endl;
	//std::cout<<"M_K"<<m_K(0,0)<<"   ,  "<<m_K(1,1)<<std::endl;
	//Calibrate image position, assuming skew = 0
	if(scaled == false){
		c_pt3d[0] = (c_pt3d[0]-m_K(0,2)) * c_pt3d[2] / m_K(0,0);
		c_pt3d[1] = (c_pt3d[1]-m_K(1,2)) * c_pt3d[2] / m_K(1,1);
	}
	else{
		c_pt3d[0] = (c_pt3d[0]-m_Scaled_K(0,2)) * c_pt3d[2] / m_Scaled_K(0,0);
		c_pt3d[1] = (c_pt3d[1]-m_Scaled_K(1,2)) * c_pt3d[2] / m_Scaled_K(1,1);
	}
}

//从世界坐标(X,Y,Z)转换到图像坐标(x,y,1)
inline void CameraParameters::GetImageCoordFromWorldCoord(double &out_u,double &out_v,double &out_dsp,Wml::Vector3d &w_pt3d,bool scaled)
{
    Wml::Vector3d c_pt3d;
	GetCameraCoordFromWorldCoord(c_pt3d,w_pt3d);
    
	//从相机坐标（Xcam,Ycam,f)转换到图像坐标(x,y,1)
     /*
	   Kx*Xcam=X-X0;
	   X=Kx*Xcam+X0=Xcam*(Alpha_x/f)+X0;
	 */
	if(scaled==false)
	{
	  out_u=c_pt3d[0]*m_K(0,0)/c_pt3d[2]+m_K(0,2);
	  out_v=c_pt3d[1]*m_K(1,1)/c_pt3d[2]+m_K(1,2);
	}
	else
	{
		out_u=c_pt3d[0]*m_Scaled_K(0,0)/c_pt3d[2]+m_Scaled_K(0,2);
		out_v=c_pt3d[1]*m_Scaled_K(0,0)/c_pt3d[2]+m_Scaled_K(1,2);
	}

	//disparity=1/f;
	out_dsp=1.0/c_pt3d[2];
}

//从世界坐标(X,Y,Z)转换到相机坐标（Xcam,Ycam,f)
inline void CameraParameters::GetCameraCoordFromWorldCoord(Wml::Vector3d &c_pt3d,Wml::Vector3d &w_pt3d)
{
	//Xcam=R*Xworld+T
    for(int row=0;row<3;++row)
	{
		c_pt3d[row]=0;
		for(int col=0;col<3;++col)
			c_pt3d[row]+=m_R(row,col)*w_pt3d[col];

		c_pt3d[row]+=m_T[row];
	}
}

inline void CameraParameters::InverseExternalParameters()
{
	Wml:: Matrix3d m_R_transpose;
    
	//求外参R的转置矩阵
    for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			m_R_transpose(i,j)=m_R(j,i);
		}
	}
	//以上语句完全等价于 m_R_Transpose=m_R.TimesTranspose();
    

	/*   相机中心位置（在世界坐标系中）
	Xcam=R*Xworld+T;
	0=R*Xcenter+T;
	Xcenter=-R_tanspose*T
	*/

	m_T=-m_R_transpose*m_T;

	//将外参矩阵R转置后存储
    for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			m_R(i,j)=m_R_transpose(i,j);
		}
	}

}


//for lightstage
inline void CameraParameters::GetKXR( Wml::Matrix3d& KXR )
{
	//KXR = m_K* m_R;
	KXR = Lm_K* m_R;
}