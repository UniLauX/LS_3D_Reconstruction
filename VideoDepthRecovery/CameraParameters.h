#pragma once

#include "WmlMatrix3.h"
#include "WmlMatrix4.h"
  ////*** ����ͼ�����ꡢ������ꡢ��������֮����໥ת��***///
#include <iostream>
class CameraParameters
{
public:
	//�������캯������ʼ���ڲ�
	 CameraParameters(const Wml::Matrix3d &K, const Wml::Matrix3d &Scaled_K):m_K(K),m_Scaled_K(Scaled_K){}
	
	//��������
	~CameraParameters(void);

	 //void GetWorldCoordFromImageCoord(int u,int v,double dsp,Wml::Vector3d & w_pt3d,bool scaled);
	 void GetWorldCoordFromImageCoord(double u,double v,double dsp,Wml::Vector3d & w_pt3d,bool scaled);
	 void GetImageCoordFromWorldCoord(double &out_u,double &out_v,double &out_dsp,Wml::Vector3d &w_pt3d,bool scaled);


	 void GetCameraCoordFromImageCoord(double u,double v,double dsp,Wml::Vector3d &c_pt3d,bool scaled);
	 void GetCameraCoordFromWorldCoord(Wml::Vector3d &c_pt3d,Wml::Vector3d &w_pt3d);

	 //����ξ���Rת�ô洢������ξ���T�д洢�������λ�ã�-R_tanspose*T)
	 void InverseExternalParameters();


	 void GetKXR( Wml::Matrix3d& KXR );

	 //�õ��ڲξ���
	 const Wml::Matrix3d *GetK(bool scaled)const
	 {
		 return scaled? &m_Scaled_K: &m_K;
	 }

	 //�õ����λ��
	
		 Wml::Vector3d GetCameraPostion()const
	 {
		 return -m_R*m_T;                    //m_R��Ҫת�ã���Ϊ��ȷ�������λ�ñ��ʽΪ-R_transpose*T
	 }


//for Ls
	  Wml::Vector3d GetCameraPos()const
	  {
		 return -(m_R.Transpose()*m_T);                    //m_R��Ҫת�ã���Ϊ��ȷ�������λ�ñ��ʽΪ-R_transpose*T
	  }

public:

	//���
	Wml::Matrix3d m_R;       // ��ת����R(Rotation Matrix)
	Wml::Vector3d m_T;       // ƽ�ƾ���T(Translation Matrix)
	


	//lightstage�ڲ�
    Wml::Matrix3d Lm_K;
	 
	//����ڲ�
    const	Wml::Matrix3d &m_K;      // �ڲξ���Intrinsic Matrix)
	const   Wml::Matrix3d &m_Scaled_K; //�ڲη�������Intrinsic Scaled Matrix)

};


//��ͼ������(x,y,1)ת������������(X,Y,Z)
inline void CameraParameters::GetWorldCoordFromImageCoord(double u,double v,double dsp,Wml::Vector3d & w_pt3d,bool scaled)
{
	/*
	Wml::Vector3d c_pt3d;
	GetCameraCoordFromImageCoord(u,v,dsp,c_pt3d,scaled);
	
	//(�������ת�����������꣩tranfer the camera coordinate to world coordinate
	
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

//ͼ�����굽������꣬��ά����ά����x,y,1)-->(Xcam,Ycam,f)
inline void CameraParameters::GetCameraCoordFromImageCoord(double u,double v,double dsp,Wml::Vector3d &c_pt3d,bool scaled)
{
	/*
	//u,v�ֱ�Ϊͼ���϶�ά�����x,y
	//c_pt3d[0],c_pt3d[1],c_pt3d[2]��ΪXcam, Ycam��f;
	c_pt3d[0]=u;
	c_pt3d[1]=v;
	c_pt3d[2]=1.0/dsp;        // z=lamda*f=1*f=1/d;         (fΪ���࣬�����disparity��Ϊ������

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

//����������(X,Y,Z)ת����ͼ������(x,y,1)
inline void CameraParameters::GetImageCoordFromWorldCoord(double &out_u,double &out_v,double &out_dsp,Wml::Vector3d &w_pt3d,bool scaled)
{
    Wml::Vector3d c_pt3d;
	GetCameraCoordFromWorldCoord(c_pt3d,w_pt3d);
    
	//��������꣨Xcam,Ycam,f)ת����ͼ������(x,y,1)
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

//����������(X,Y,Z)ת����������꣨Xcam,Ycam,f)
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
    
	//�����R��ת�þ���
    for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			m_R_transpose(i,j)=m_R(j,i);
		}
	}
	//���������ȫ�ȼ��� m_R_Transpose=m_R.TimesTranspose();
    

	/*   �������λ�ã�����������ϵ�У�
	Xcam=R*Xworld+T;
	0=R*Xcenter+T;
	Xcenter=-R_tanspose*T
	*/

	m_T=-m_R_transpose*m_T;

	//����ξ���Rת�ú�洢
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