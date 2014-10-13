
#include "OpenglSelection.h"
#include "../opengl/glew.h"


vec COpenglSelection::GetMousePoint3D(COpenGLDC *pDC,CPoint mouseposition)
{
		int hits;
		UINT items[64];
		double x,y,z;
		pDC->BeginSelection(mouseposition.x, mouseposition.y);
		hits = pDC->EndSelection(items);
		pDC->m_Camera.GetWxyz(x,y,z);
		return vec(x,y,z);
}

void COpenglSelection::PointToScreen(COpenGLDC *pDC,const vec& pt, vec2 &p)
{

	GLint			viewport[4];
	GLdouble		ModelMatrix[16];
	GLdouble		ProjMatrix[16];
	pDC->m_Camera.Getjuzhen(viewport,ModelMatrix,ProjMatrix);
	double			in_pt[4], out[4];
	in_pt[0]=pt[0];	in_pt[1]=pt[1];	in_pt[2]=pt[2];	in_pt[3]=1.0;	//转化为齐次坐标，便于矩阵相乘

	Transform_Point(out, ModelMatrix, in_pt);						//乘以模型矩阵
	Transform_Point(in_pt, ProjMatrix, out);						//乘以投影矩阵
	if(int(in_pt[3] * 100000) == 0)									//特殊情况处理
	{
		p[0]=0;
		p[1]=0;
		return;
	}
	in_pt[0]/=in_pt[3];												//归一化处理
	in_pt[1]/=in_pt[3];
	in_pt[2]/=in_pt[3];
	p[0]= (int)(viewport[0] + (1 + in_pt[0]) * viewport[2] / 2 + 0.5);
	p[1]= viewport[3]-(int)(viewport[1] + (1 + in_pt[1]) * viewport[3] / 2 + 0.5);

}
/*bool COpenglSelection::GetPointBuffer(vec pt3D,float r,float &zvalue)
{
	GLint	viewport[4];
	GLdouble		ModelMatrix[16];
	GLdouble		ProjMatrix[16];
	m_pGLDC->m_Camera.Getjuzhen(viewport,ModelMatrix,ProjMatrix);

	CPoint2D p2d;
	PointToScreen(CPoint3D(pt3D[0],pt3D[1],pt3D[2]),p2d);
	vec uppoint;

	double x,y,z;
	glReadPixels(p2d.x, viewport[3]-p2d.y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&zvalue );		
	gluUnProject(p2d.x, viewport[3]-p2d.y,zvalue, ModelMatrix, ProjMatrix, viewport,&x,&y,&z);
	uppoint=vec(x,y,z)-pt3D;
	if(len(uppoint)>r)return false;
	else return true;



}*/
float COpenglSelection::GetPointBuffer(COpenGLDC *pDC,CPoint pt)
{
	pDC->BeginSelection(pt.x, pt.y);
	float zvalue;
	GLint	viewport[4];
	GLdouble		ModelMatrix[16];
	GLdouble		ProjMatrix[16];
	pDC->m_Camera.Getjuzhen(viewport,ModelMatrix,ProjMatrix);
	glReadPixels(pt.x, viewport[3]-pt.y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&zvalue );
	UINT items[64];
	pDC->EndSelection(items);
	return zvalue;

}
vec COpenglSelection::GetBufferPoint(COpenGLDC *pDC,CPoint pt,float zbuffer)
{
	GLint	viewport[4];
	GLdouble		ModelMatrix[16];
	GLdouble		ProjMatrix[16];
	pDC->m_Camera.Getjuzhen(viewport,ModelMatrix,ProjMatrix);
    double x,y,z;
	gluUnProject(pt.x, viewport[3]-pt.y,zbuffer, ModelMatrix, ProjMatrix, viewport,&x,&y,&z);
	return vec(x,y,z);
}

void COpenglSelection::Transform_Point(double out[4], const double m[16], const double in[4])
{
	out[0] = M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
	out[1] = M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
	out[2] = M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
	out[3] = M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
}

/*BOOL COpenglSelection::Is_MatrixUpdate(COpenGLDC *pDC)
{   
	GLdouble		ModelMatrix[16];
	GLdouble		ProjMatrix[16];
	GLint			Viewport[4];
	pDC->TransWorldtoScreen();
	pDC->m_Camera.Getjuzhen(Viewport,ModelMatrix,ProjMatrix);
	bool updateflag=false;
	for (int i=0;i<16;i++)
	{
		if (m_ModelMatrix[i]!=ModelMatrix[i])
		{
			m_ModelMatrix[i]=ModelMatrix[i];
			updateflag=true;
		}
		if (m_ProjMatrix[i]!=ProjMatrix[i])
		{
			m_ProjMatrix[i]=ProjMatrix[i];
			updateflag=true;
		}
	}

	return updateflag;
}*/


/*void COpenglSelection::GetMouseRay(COpenGLDC *pDC,int xPos,int yPos,vec &diretion,vec &pt)
{  
	
	GLint	viewport[4];
	GLdouble		ModelMatrix[16];
	GLdouble		ProjMatrix[16];
	pDC->m_Camera.Getjuzhen(viewport,ModelMatrix,ProjMatrix);
	
	GLdouble x,y,z;
	gluUnProject(xPos, viewport[3]-yPos,0.1, ModelMatrix, ProjMatrix, viewport,&x,&y,&z);
	pt=vec(x,y,z);
	gluUnProject(xPos, viewport[3]-yPos,0.8, ModelMatrix, ProjMatrix, viewport,&x,&y,&z);
	diretion=vec(x,y,z)-pt;


	normalize(direction);


}*/
