#pragma once

#include "..\opengl\OpenGLDC.h"

#include "../trimesh/include/Vec.h"
class COpenglSelection
{
public:
	#define M(row,col) m[col * 4 + row]

public:
	static vec GetMousePoint3D(COpenGLDC *pDC,CPoint mouseposition);//获取鼠标点的三维点

	static void PointToScreen(COpenGLDC *pDC, const vec& pt, vec2 &p);//获取三维点在屏幕的位置
	static void Transform_Point(double out[4], const double m[16], const double in[4]);


	
    static float GetPointBuffer(COpenGLDC *pDC,CPoint pt);//获取鼠标点的深度值
	static vec GetBufferPoint(COpenGLDC *pDC,CPoint pt,float zbuffer);//获取鼠标点，指定深度的三维坐标



	/*BOOL Is_MatrixUpdate();
	GLdouble		m_ModelMatrix[16];
	GLdouble		m_ProjMatrix[16];
	GLint			m_Viewport[4];*/



};