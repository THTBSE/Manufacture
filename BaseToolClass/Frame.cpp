
#include "Frame.h"
using namespace BaseClass;
CFrame::CFrame(vec e10,vec e20,vec n0)
{
	e1=e10;
	e2=e20;
	n=n0;
	
}
CFrame::CFrame(void)
{   

}

CFrame::~CFrame(void)
{
}
//以vnormal为z轴建立一个坐标系,normalizeframe为false时表示不需要单位化框架
void CFrame::GetFrame(vec vnormal,CFrame&result_frame,bool normalizeframe)
{   
	vec &e1=result_frame.e1;
	vec &e2=result_frame.e2;
	vec &n=result_frame.n ;
	int indexflag=0;
	double maxxyz=0;
	for (int i=0;i<3;i++)
	{
		if (maxxyz<fabs(vnormal[i]))
		{
			indexflag=i;
			maxxyz=fabs(vnormal[i]);
		}
	}
	if (indexflag==0||indexflag==1)
	{
		e1[0]=-vnormal[1];
		e1[1]=vnormal[0];
		e1[2]=0.0;
	}
	else if (indexflag==2)
	{
		e1[0]=-vnormal[2];
		e1[1]=0.0;
		e1[2]=vnormal[0];
	}

	e2=vnormal CROSS e1;
	n=vnormal;
	if (!normalizeframe)return ;//result_frame;
	normalize(e1);
	normalize(e2);
	normalize(n);
	//return tempframe;
}
/*CFrame CFrame::Transform_Frame(CFrame origin_Frame,CFrame target_Frame)
{
	CFrame transformframe;

}*/
//求解e1 e2
//以vnormal为z轴，v0_vneighbor旋转至与vnormal垂直的方向为e1
void CFrame::SetFrame(vec vnormal,vec v0_vneighbor,CFrame &result_frame)
{
  
	vec e1=v0_vneighbor;
	vec e2=vnormal  CROSS e1;
	e1=e2 CROSS vnormal;
	result_frame.e1=normalize(e1);
	result_frame.e2=normalize(e2);
	result_frame.n=normalize(vnormal);
	//return framev1v2;

}
//把向量转化到局部坐标系下的向量
void CFrame::Encode_To_Frame(CFrame localframe,vec global_vector,vec& local_vector)
{
	for (int i=0;i<3;i++)
	{
		local_vector[i]=localframe[i] DOT global_vector;
	}
}
//把坐标系origin_Frame转化到局部坐标系localframe下，得到相对坐标系target_Frame
void CFrame::Encode_To_Frame(CFrame localframe,CFrame origin_Frame,CFrame &target_Frame)
{
	for (int j=0;j<3;j++)
	{
		target_Frame.e1[j]=origin_Frame.e1 DOT localframe[j];
		target_Frame.e2[j]=origin_Frame.e2 DOT localframe[j];
		target_Frame.n[j]=origin_Frame.n DOT localframe[j];
	}

}
//把相对向量，从局部坐标系下解压
void  CFrame::Decode_From_Frame(CFrame localframe,vec local_vector,vec &global_vector)
{   
	global_vector=vec(0,0,0);
	for(int j=0;j<3;j++)
	{
	  global_vector=global_vector+local_vector[j]*localframe[j];
	}
}
//把相对坐标系relative_Frame 从局部坐标系localframe下解压
void CFrame::Decode_From_Frame(CFrame localframe,CFrame relative_Frame,CFrame &decode_Frame)
{
	for (int j=0;j<3;j++)
	{
		decode_Frame.e1=relative_Frame.e1[0]*localframe.e1+relative_Frame.e1[1]*localframe.e2+relative_Frame.e1[2]*localframe.n;
		decode_Frame.e2=relative_Frame.e2[0]*localframe.e1+relative_Frame.e2[1]*localframe.e2+relative_Frame.e2[2]*localframe.n;
		decode_Frame.n=relative_Frame.n[0]*localframe.e1+relative_Frame.n[1]*localframe.e2+relative_Frame.n[2]*localframe.n;
	}

}
//把v1v2向量投影到planenormal平面上，获得投影向量
vec CFrame::ProjectVector(vec planenormal,vec v1v2)
{   
	vec e1=v1v2;
	vec e2=planenormal  CROSS e1;
	e1=e2 CROSS planenormal;
	normalize(e1);
	float length0=e1 DOT v1v2;
	return length0*e1;

}
//把v1v2向量旋转到planenormal平面上，获得旋转向量
vec CFrame::RotateVector(vec planenormal,vec v1v2)
{
	vec e1=v1v2;
	vec e2=planenormal  CROSS e1;
	e1=e2 CROSS planenormal;
	normalize(e1);
	float length0=len(v1v2);
	return length0*e1;
}

