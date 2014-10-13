#include "../stdafx.h "
#include "OpenglToolFunction.h"

void COpenglToolFunction::SetMaterialColor(COLORREF clr)
{
	glDisable(GL_COLOR_MATERIAL);
	float mat[4]={0.5,0.5,0.6,1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
	BYTE r,g,b;
	r = GetRValue(clr);
	g = GetGValue(clr);
	b = GetBValue(clr);
	mat[0] = (GLfloat)r/255;
	mat[1] = (GLfloat)g/255;
	mat[2] = (GLfloat)b/255;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
	glMaterialf(GL_FRONT, GL_SHININESS,128.0);
}

// void COpenglToolFunction::Draw_Transparent_Sphere(float *v,float rgba[4],float radius)
// {   
// 	glDisable(GL_LIGHTING);
// 	glDisable(GL_DEPTH_TEST);
// 	glColorMaterial(GL_FRONT,GL_DIFFUSE);
// 	glEnable(GL_COLOR_MATERIAL);
// 	glDepthMask(GL_FALSE);
// 	glEnable (GL_BLEND);
// 	glColor4f(rgba[0],rgba[1],rgba[2],rgba[3]);
// 	//glColor4f(51.0/255,102.0/255,205.0/255,0.5);//浅蓝色
// 	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glEnable(GL_CULL_FACE);	
// 	glCullFace(GL_BACK);
// 	glPushMatrix();
// 	glTranslatef(v[0],v[1],v[2]);
// 	glHint(GL_POLYGON_SMOOTH,GL_NICEST);//告诉opengl以显示效果为重，速度不重要
// 	glEnable(GL_POLYGON_SMOOTH);
// 	auxSolidSphere(radius);
// 	glDisable(GL_POLYGON_SMOOTH);
// 	glPopMatrix();
// 	glDisable(GL_CULL_FACE);
// 	glDisable(GL_COLOR_MATERIAL);
// 	glDepthMask(GL_TRUE);
// 	glEnable(GL_DEPTH_TEST);
// 	glEnable(GL_LIGHTING);
// }
// void COpenglToolFunction::Draw_Sphere(float *centerpoint,float r,COLORREF old_clr)
// {
// 
// 	glPushMatrix();
// 	glTranslatef(centerpoint[0],centerpoint[1],centerpoint[2]);
// 	auxSolidSphere(r);
// 	glPopMatrix();
// }