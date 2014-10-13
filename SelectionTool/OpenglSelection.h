#pragma once

#include "..\opengl\OpenGLDC.h"

#include "../trimesh/include/Vec.h"
class COpenglSelection
{
public:
	#define M(row,col) m[col * 4 + row]

public:
	static vec GetMousePoint3D(COpenGLDC *pDC,CPoint mouseposition);//��ȡ�������ά��

	static void PointToScreen(COpenGLDC *pDC, const vec& pt, vec2 &p);//��ȡ��ά������Ļ��λ��
	static void Transform_Point(double out[4], const double m[16], const double in[4]);


	
    static float GetPointBuffer(COpenGLDC *pDC,CPoint pt);//��ȡ��������ֵ
	static vec GetBufferPoint(COpenGLDC *pDC,CPoint pt,float zbuffer);//��ȡ���㣬ָ����ȵ���ά����



	/*BOOL Is_MatrixUpdate();
	GLdouble		m_ModelMatrix[16];
	GLdouble		m_ProjMatrix[16];
	GLint			m_Viewport[4];*/



};