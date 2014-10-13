// OpenGLDC.cpp: implementation of the COpenGLDC class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "OpenGLDC.h"
#include "..\MainFrm.h"
#include "..\GeometryProcDoc.h"


#define GL_PI 3.1415f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLDC::COpenGLDC(HWND hWnd):m_hWnd(hWnd)
{
	m_bSelectionMode = FALSE;
	m_clrBkMaterial = RGB(0,0,0); 
}

COpenGLDC::~COpenGLDC()
{
	
}

BOOL COpenGLDC::InitDC()
{
	if (m_hWnd == NULL) return FALSE;
	
	m_Camera.init();

	m_hDC = ::GetDC(m_hWnd);			// Get the Device context

	int pixelformat;
	PIXELFORMATDESCRIPTOR pfdWnd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), // Structure size.
		1,                             // Structure version number.
		PFD_DRAW_TO_WINDOW |           // Property flags.
		PFD_SUPPORT_OPENGL |
 		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,                            // 24-bit color.
		0, 0, 0, 0, 0, 0,              // Not concerned with these.
		0, 0, 0, 0, 0, 0, 0,           // No alpha or accum buffer.
		32,                            // 32-bit depth buffer.
		0, 0,                          // No stencil or aux buffer.
		PFD_MAIN_PLANE,                // Main layer type.
		0,                             // Reserved.
		0, 0, 0                        // Unsupported.
	};

    if ( (pixelformat = ChoosePixelFormat(m_hDC, &pfdWnd)) == 0 )
	{
		AfxMessageBox(_T("ChoosePixelFormat to wnd failed"));
		return FALSE;
	}

    if (SetPixelFormat(m_hDC, pixelformat, &pfdWnd) == FALSE)
        AfxMessageBox(_T("SetPixelFormat failed"));

	m_hRC=wglCreateContext(m_hDC);

	VERIFY(wglMakeCurrent(m_hDC,m_hRC));
	GLSetupRC();
	wglMakeCurrent(NULL,NULL);
	return m_hRC!=0;
}


void COpenGLDC::GLResize(int w,int h)
{
	wglMakeCurrent(m_hDC,m_hRC);

	// Prevent a divide by zero
	if(h == 0) h = 1;
	if(w == 0) w = 1;
	m_Camera.set_screen(w,h);
}

void COpenGLDC::GLSetupRC()
{
	//initialize color and rendering
	m_bShading = TRUE;

	//bright white light - full intensity RGB values
	GLfloat lightAmbient[] = {0.6f,0.6f,0.6f,1.0f};
	GLfloat lightDiffuse[] = {0.6f,0.6f,0.6f,1.0f};
		//////////////////////////////////////////////////////////////////////////
	glShadeModel(GL_SMOOTH);  //刘斌添加
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_CCW);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//刘斌添加
	glEnable(GL_BLEND);//刘斌添加

	//glCullFace(GL_BACK);
	//////////////////////////////////////////////////////////////////////////

	glEnable(GL_DEPTH_TEST);		//Hidden surface removal
	//glEnable(GL_CULL_FACE);			//Do not calculate inside of object
	glFrontFace(GL_CCW);			//counter clock-wise polygons face out
//	glCullFace(GL_BACK);


	CreateLight();

	//Initialize Material Color to Gray
	SetBkColor(RGB(0,0,0));							//black background****************屏幕背景
	SetMaterialColor(RGB(180,100,100));		//golden material color  模型正面
	SetBkMaterialColor(RGB(255,255,255));   //模型背面
	SetColor(RGB(255,255,255));					//white frame color
	SetHighlightColor(RGB(255,0,0));				//red highlight color

	//Point Size
	glPointSize(1.0);
	//glLineWidth(1);

}


void COpenGLDC::Ready()
{
	wglMakeCurrent(m_hDC,m_hRC);
	ClearBkground();
	OnShading();
	m_Camera.projection();
}

void COpenGLDC::Finish()
{
	glFlush();
	SwapBuffers(m_hDC);
	wglMakeCurrent(m_hDC,NULL);

}
void COpenGLDC::FinshForEps()
{
	SwapBuffers(m_hDC);
}


//////////////////LIGHT && MATERIALS SETTING//////////
void COpenGLDC::ClearBkground()
{
	GLclampf	r,g,b;
	r = (GLclampf)GetRValue(m_clrBk)/255.0;
	g = (GLclampf)GetGValue(m_clrBk)/255.0;
	b = (GLclampf)GetBValue(m_clrBk)/255.0;
    
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

}

//setting model
void COpenGLDC::OnShading()
{
	if(m_bShading )
	{
		glEnable( GL_LIGHTING );
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}
	else
	{
		COLORREF bkcol;
		GetBkColor(bkcol);
		BYTE r,g,b;
		r = GetRValue(bkcol);
		g = GetGValue(bkcol);
		b = GetBValue(bkcol);
		if (r==255 && g==255 && b==255)
		{
			r=128;
			g=128;
			b=128;
		}
		COLORREF clr(RGB(255-r,255-g,255-b));
		
		SetColor(clr);

		glDisable( GL_LIGHTING );
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}
}

void COpenGLDC::Shading(BOOL bShading)
{
	m_bShading = bShading;
}

BOOL COpenGLDC::IsShading()
{
	return m_bShading;
}

void COpenGLDC::Lighting(BOOL bLighting)
{
	if(bLighting)
		glEnable( GL_LIGHTING );
	else
		glDisable( GL_LIGHTING );
}

BOOL COpenGLDC::IsLighting()
{
	GLboolean bLighting;
	glGetBooleanv(GL_LIGHTING,&bLighting);   
	return bLighting;
}

void COpenGLDC::SetLightDirection(float dx,float dy,float dz)
{
	m_vecLight[0] = dx;
	m_vecLight[1] = dy;
	m_vecLight[2] = dz;
	GLfloat lightPos[] = { dx, dy, dz, 0.0f };
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
}

void COpenGLDC::GetLightDirection(float& dx,float& dy,float& dz)
{
	dx = m_vecLight[0];
	dy = m_vecLight[1];
	dz = m_vecLight[2];
}



//rendering color
void COpenGLDC::SetMaterialColor(COLORREF clr)
{
	float mat[4]={0.5,0.5,0.6,1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat);

	m_clrMaterial = clr;
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

void COpenGLDC::GetMaterialColor(COLORREF& clr)
{
	clr = m_clrMaterial;
}

void COpenGLDC::GetBkMaterialColor(COLORREF& clr)
{
	clr = m_clrBkMaterial;
}
void COpenGLDC::SetBkMaterialColor(COLORREF clr)
{
	float mat[4];

	mat[0] = 0.5;
	mat[1] = 0.5;
	mat[2] = 0.5;
	mat[3] = 1.0;
	glMaterialfv(GL_BACK, GL_AMBIENT, mat);

	m_clrBkMaterial = clr;
	BYTE r,g,b;
	r = GetRValue(clr);
	g = GetGValue(clr);
	b = GetBValue(clr);
	//GLfloat mat_amb_diff[] = {(GLfloat)r/255,(GLfloat)g/255,(GLfloat)b/255,1.0};
	//glMaterialfv(GL_BACK,GL_AMBIENT_AND_DIFFUSE,mat_amb_diff);
	glMaterialfv(GL_BACK, GL_DIFFUSE, mat);

	glMaterialfv(GL_BACK, GL_SPECULAR, mat);
	glMaterialf(GL_BACK, GL_SHININESS, 0.25 * 128.0);
}

void COpenGLDC::SetBkColor(COLORREF clr)
{
	m_clrBk = clr;
}

void COpenGLDC::GetBkColor(COLORREF& clr)
{
	clr = m_clrBk;
}

void COpenGLDC::SetColor(COLORREF clr)
{
	m_clr = clr;
	BYTE r,g,b;
	r = GetRValue(clr);
	g = GetGValue(clr);
	b = GetBValue(clr);
	glColor3ub(r,g,b);
}

void COpenGLDC::GetColor(COLORREF& clr)
{
	clr =  m_clr;
}

void COpenGLDC::SetHighlightColor(COLORREF clr)
{
	m_clrHighlight = clr;
}

void COpenGLDC::GetHighlightColor(COLORREF& clr)
{
	clr = m_clrHighlight;
}

void COpenGLDC::Highlight(BOOL bHighlight)
{
	BYTE r,g,b;
	if(bHighlight){
		r = GetRValue(m_clrHighlight);
		g = GetGValue(m_clrHighlight);
		b = GetBValue(m_clrHighlight);
	}
	else{
		r = GetRValue(m_clrMaterial);
		g = GetGValue(m_clrMaterial);
		b = GetBValue(m_clrMaterial);
	}
	GLfloat mat_amb_diff[] = {(GLfloat)r/255,(GLfloat)g/255,(GLfloat)b/255,1.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mat_amb_diff);
}


//////////////////////// drawing functions ////////////////////////
//draw point
void COpenGLDC::DrawPoint(const CPoint3D& pt)
{
	glBegin(GL_POINTS);
		glVertex3f(pt.x,pt.y,pt.z);
	glEnd();
}
void COpenGLDC::DrawPoint(const CPoint3D& pt, COLORREF ctr)
{
	BOOL bLighting = IsLighting();
	Lighting(FALSE);
	COLORREF old_clr;
	GetColor(old_clr);
	SetColor(ctr);

	glBegin(GL_POINTS);
	glVertex3f(pt.x,pt.y,pt.z);
	glEnd();

	Lighting(bLighting);
	SetColor(old_clr);
}
void COpenGLDC::DrawLine(const CPoint3D& sp,const CPoint3D& ep)
{
	BOOL bLighting = IsLighting();
	Lighting(FALSE);
	glBegin(GL_LINES);
		glVertex3f(sp.x,sp.y,sp.z);
		glVertex3f(ep.x,ep.y,ep.z);
	glEnd();
	Lighting(bLighting);
}
void COpenGLDC::DrawLine(const CPoint3D& sp,const CPoint3D& ep,int R,int G,int B)
{
	BOOL bLighting = IsLighting();
	Lighting(FALSE);
	COLORREF old_clr;
	GetColor(old_clr);
	SetColor(RGB(R,G,B));

	glBegin(GL_LINES);
	glVertex3f(sp.x,sp.y,sp.z);
	glVertex3f(ep.x,ep.y,ep.z);
	glEnd();

	Lighting(bLighting);
	SetColor(old_clr);
}

//
void COpenGLDC::DrawJuxing(const CPoint3D& p1,const CPoint3D& p2,const CPoint3D& p3,const CPoint3D& p4)
{
	BOOL bLighting = IsLighting();
	Lighting(FALSE);

	glBegin(GL_QUADS);						// 绘制正方形
	glVertex3f( p1.x, p1.y, 0.0f);				// 左上
	glVertex3f( p2.x, p2.y, 0.0f);				// 右上
	glVertex3f( p3.x, p3.y, 0.0f);				// 左下
	glVertex3f( p4.x, p4.y, 0.0f);				// 右下
	glEnd();	

	Lighting(bLighting);
}
void COpenGLDC::DrawPolyline(const CPoint3D* pt,int size)
{
	BOOL bLighting = IsLighting();
	Lighting(FALSE);
	COLORREF old_clr;
	GetColor(old_clr);
	SetColor(RGB(255,0,0));

	glBegin(GL_LINE_STRIP);
	for(int i=0;i<size;i++)
		glVertex3f(pt[i].x,pt[i].y,pt[i].z);
	glEnd();
	Lighting(bLighting);
	SetColor(old_clr);
}
void COpenGLDC::DrawPolyline(const vector<CPoint3D> pts, int R, int G, int B)
{
	BOOL bLighting = IsLighting();
	Lighting(FALSE);
	COLORREF old_clr;
	GetColor(old_clr);
	SetColor(RGB(R,G,B));

	glBegin(GL_LINE_STRIP);
	for(int i=0;i<pts.size();i++)
		glVertex3f(pts[i].x,pts[i].y,pts[i].z);
	glEnd();
	Lighting(bLighting);
	SetColor(old_clr);
}

void COpenGLDC::DrawTriChipwithtecture(double n0,double n1,double n2,double v00,double v01,double v02, double v10,double v11,double v12,double v20,double v21,double v22)
{
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glBegin(GL_TRIANGLES);
	glNormal3d(n0,n1,n2);
	glTexCoord2f(0.0f, 0.0f); glVertex3d(v00,v01,v02);
	glTexCoord2f(1.0f, 0.0f);glVertex3d(v10,v11,v12);
	glTexCoord2f(0.5f, 1.0f); glVertex3d(v20,v21,v22);
	glEnd();

}

void COpenGLDC::DrawQuads(vector<CPoint3D> waist1, vector<CPoint3D> waist2, vector<CVector3D> norm)
{
	//glFrontFace(GL_CW);
	BOOL bLighting = IsLighting();
	Lighting(TRUE);

	/*COLORREF old;
	GetMaterialColor(old);

	SetMaterialColor(RGB(128,128,256));*/
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin( GL_TRIANGLES );
	int n_gon=waist2.size();
	for( int a = 0; a < n_gon; a++ ) 
	{
		glNormal3f( norm[a].dx, norm[a].dy, norm[a].dz );

		//glVertex3f( waist1[a].x, waist1[a].y, waist1[a].z );
		//glVertex3f( waist2[a].x, waist2[a].y, waist2[a].z );

		//glVertex3f( waist2[(a+1)%n_gon].x, waist2[(a+1)%n_gon].y, waist2[(a+1)%n_gon].z );
		//glVertex3f( waist1[(a+1)%n_gon].x, waist1[(a+1)%n_gon].y, waist1[(a+1)%n_gon].z );
		glVertex3f( waist1[a].x, waist1[a].y, waist1[a].z );
		glVertex3f( waist2[a].x, waist2[a].y, waist2[a].z );
		glVertex3f( waist2[(a+1)%n_gon].x, waist2[(a+1)%n_gon].y, waist2[(a+1)%n_gon].z );

		glVertex3f( waist1[a].x, waist1[a].y, waist1[a].z );
		glVertex3f( waist2[(a+1)%n_gon].x, waist2[(a+1)%n_gon].y, waist2[(a+1)%n_gon].z );
		glVertex3f( waist1[(a+1)%n_gon].x, waist1[(a+1)%n_gon].y, waist1[(a+1)%n_gon].z );

	}
	glEnd();

	Lighting(bLighting);
	//SetMaterialColor(old);
	//glFrontFace(GL_CCW);
}
//Draw 3D Solid
// void COpenGLDC::DrawSphere(const CPoint3D& cen,double r,const CVector3D& vec)
// {
// 	glPushMatrix();
// 	glTranslatef(cen.x,cen.y,cen.z);
// 
// 	CVector3D vecNY(0,-1,0);
// 	CVector3D axis = vecNY*vec;
// 	double ang = _AngleBetween(vecNY,vec);
// 	ang = ang*180/GL_PI;
// 	glRotatef(ang,axis.dx,axis.dy,axis.dz);
// 
// 	auxSolidSphere(r);
// 	glPopMatrix();
// }
// 
// void COpenGLDC::DrawSphere(const CPoint3D& cen,double r)
// {
// 	double width,height;
// 	m_Camera.get_view_rect(width,height);
// 	double len = min(width,height);
// 	len *= 0.01;
// 	glPushMatrix();
// 	glTranslatef(cen.x,cen.y,cen.z);
// 	auxSolidSphere(len);
// 	glPopMatrix();
// }
//克隆  改为绘制透明球体
// void COpenGLDC::Draw_Transparent_Sphere(const CPoint3D& cen,float rgba[4],float radius)
// {   
// 	//double width,height;
// 	//m_Camera.get_view_rect(width,height);
// 	//GLint viewport[4];
// 	//glGetIntegerv(GL_VIEWPORT,viewport);
// 	//width=viewport[2];
// 	//height=viewport[3];
// 	//double len = min(width,height);
// 	//len *= 0.1*radius;
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
// 	glTranslatef(cen.x,cen.y,cen.z);
// 	auxSolidSphere(radius);
// 	glPopMatrix();
// 	glDisable(GL_CULL_FACE);
// 	glDisable(GL_COLOR_MATERIAL);
// 	glDepthMask(GL_TRUE);
// }

// void COpenGLDC::DrawSphereb(const CPoint3D& cen,double r)
// {
// 	COLORREF old_clr;
// 	GetMaterialColor(old_clr);
// 	SetMaterialColor(RGB(0,0,255));
// 
// 	double width,height;
// 	m_Camera.get_view_rect(width,height);
// 	double len = min(width,height);
// 	len *= 0.01;
// 	glPushMatrix();
// 	glTranslatef(cen.x,cen.y,cen.z);
// 	auxSolidSphere(len);
// 	SetMaterialColor(old_clr);
// 	glPopMatrix();
// }
// void COpenGLDC::DrawSpherep(const CPoint3D& cen,double r)
// {
// 	COLORREF old_clr;
// 	GetMaterialColor(old_clr);
// 	SetMaterialColor(RGB(255,0,255));
// 
// 	double width,height;
// 	m_Camera.get_view_rect(width,height);
// 	double len = min(width,height);
// 	len *= 0.01;
// 	glPushMatrix();
// 	glTranslatef(cen.x,cen.y,cen.z);
// 	auxSolidSphere(len);
// 	SetMaterialColor(old_clr);
// 	glPopMatrix();
// }
// void COpenGLDC::DrawCylinder(const vector<CPoint3D> pdata, double r)
// {
// 	COLORREF old_clr;
// 	GetMaterialColor(old_clr);
// 	SetMaterialColor(RGB(255,0,255));
// 	int n=pdata.size();
// 	for (int i=1;i<n;i++)
// 	{
// 		CVector3D li=pdata[i]-pdata[i-1];
// 		DrawCylinder(pdata[i-1],r,li);
// 	}
// 	SetMaterialColor(old_clr);
// }
// void COpenGLDC::DrawCylinder(const CPoint3D& cen,double r,const CVector3D& h)
// {
// 	glPushMatrix();
// 
// 	glTranslatef(cen.x,cen.y,cen.z);
// 
// 	CVector3D vecNY(0,-1,0);
// 	CVector3D axis = vecNY*h;
// 	double ang = _AngleBetween(vecNY,h);
// 	ang = ang*180/GL_PI;
// 	glRotatef(ang,axis.dx,axis.dy,axis.dz);
// 
// 
// 	auxSolidCylinder(r,h.GetLength());
// 	glPopMatrix();
// }

// void COpenGLDC::DrawTorus(const CPoint3D& cen,const CVector3D& ax,double r_in,double r_out)
// {
// 	glPushMatrix();
// 
// 	glTranslatef(cen.x,cen.y,cen.z);
// 
// 	CVector3D vecNY(0,-1,0);
// 	CVector3D axis = vecNY*ax;
// 	double ang = _AngleBetween(vecNY,ax);
// 	ang = ang*180/GL_PI;
// 	glRotatef(ang,axis.dx,axis.dy,axis.dz);
// 
// 
// 	auxSolidTorus(r_in,r_out);
// 	glPopMatrix();
// }

// void COpenGLDC::DrawCone(const CPoint3D& cen,double r,const CVector3D& h)
// {
// 	glPushMatrix();
// 
// 	glTranslatef(cen.x,cen.y,cen.z);
// 
// 	CVector3D vecNY(0,-1,0);
// 	CVector3D axis = vecNY*h;
// 	double ang = _AngleBetween(vecNY,h);
// 	ang = ang*180/GL_PI;
// 
// 	glRotatef(ang,axis.dx,axis.dy,axis.dz);
// 
// 	GLfloat angle,x,y;
// 	glBegin(GL_TRIANGLE_FAN);
// 		glVertex3f(0,0,h.GetLength());
// 		for(angle =0.0f;angle<(2.0f*GL_PI);angle += (GL_PI/8.0f))
// 		{
// 			x = r*sin(angle);
// 			y = r*cos(angle);
// 			glVertex2f(x,y);
// 		}
// 	glEnd();
// 
// 	// Begin a new triangle fan to cover the bottom
// 	glBegin(GL_TRIANGLE_FAN);
// 		glVertex2f(0.0f,0.0f);
// 		for(angle =0.0f;angle<(2.0f*GL_PI);angle += (GL_PI/8.0f))
// 		{
// 			x = r*sin(angle);
// 			y = r*cos(angle);
// 			glVertex2f(x,y);
// 		}
// 	glEnd();
// 	glPopMatrix();
// }

////////////////////////////////////////

void COpenGLDC::DrawCoord()
{
	//const GLubyte* OpenGLVersion =glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
	//const GLubyte* gluVersion= gluGetString(GLU_VERSION); //返回当前GLU工具库版本
	//printf("OOpenGL实现的版本号：%s\n",OpenGLVersion );
	//printf("OGLU工具库版本：%s\n", gluVersion);
	//
	BOOL bLighting = IsLighting();
	Lighting(FALSE);

	double width,height;
	m_Camera.get_view_rect(width,height);
	double len = min(width,height);
	len *= 0.1;

	glRasterPos3d(1.1*len, 0, 0);
	DrawText("X");
	glRasterPos3d(0, 1.1*len, 0);
	DrawText("Y");
	glRasterPos3d(0, 0, 1.1*len);
	DrawText("Z");
	

	CPoint3D cPt,xPt,yPt,zPt;
	xPt.x = yPt.y = zPt.z = len;

	COLORREF old_clr;
	GetColor(old_clr);

	SetColor(RGB(255,0,0));

	glPushMatrix();	
	glTranslatef(0.8*len, 0.0f, 0.0f);
	glRotatef(90.0f,0.0f,1.0f,0.0f);
	glutWireCone(0.05*len,0.2*len,10,10);
	glPopMatrix();

	//axis-x: red
	DrawLine(cPt,xPt);

	//axis-y: green
	SetColor(RGB(0,255,0));

	glPushMatrix();	
	glTranslatef(0.0f, 0.8*len, 0.0f);
	glRotatef(-90.0f,1.0f,0.0f,0.0f);
	glutWireCone(0.05*len,0.2*len,10,10);
	glPopMatrix();

	DrawLine(cPt,yPt);

	//axis-z: blue
	SetColor(RGB(0,0,255));

	glPushMatrix();	
	glTranslatef(0.0f, 0.0f, 0.8*len);
	glutWireCone(0.05*len,0.2*len,10,10);
	glPopMatrix();

	DrawLine(cPt,zPt);

	Lighting(bLighting);
	SetColor(old_clr);

	//绘制平面
	/*glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	SetColor(RGB(0,255,0));*/
   /* for (int i=0;i<10;i++)
    {   
		DrawLine(CPoint3D(i*width/20.0,-8000,0),CPoint3D(i*width/20.0,8000,0));
    }*/
	// 恢复原来的矩阵
	/*glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();*/
	
	glPopMatrix();

}
//世界坐标转为屏幕坐标
void COpenGLDC::TransWorldtoScreen()
{
	wglMakeCurrent(m_hDC,m_hRC);
	
}
void COpenGLDC::EndTransWorldtoScreen()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glRenderMode(GL_RENDER);

	wglMakeCurrent(m_hDC,NULL);

}
//////////////////////////////////////////////////
void COpenGLDC::BeginSelection(int xPos,int yPos)
{
	m_bSelectionMode = TRUE;
	wglMakeCurrent(m_hDC,m_hRC);
	
	//set up selection buffer	
	glSelectBuffer(BUFFER_LENGTH,m_selectBuff);
	
	//switch to projection and save the matrix
	m_Camera.selection(xPos,yPos);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	InitNames();
}

int	COpenGLDC::EndSelection(UINT* items)
{
	m_bSelectionMode = FALSE;
	GLint hits = glRenderMode(GL_RENDER);
	for(int i=0;i<hits;i++)
	{
		items[i] = m_selectBuff[i*4+3];
	}

	wglMakeCurrent(m_hDC,NULL);
	return hits;
}

BOOL COpenGLDC::IsSelectionMode()
{
	return m_bSelectionMode;
}

void COpenGLDC::InitNames()
{
	glInitNames();
	glPushName(0);
}

void COpenGLDC::LoadName(UINT name)
{
	glLoadName(name);
}

void COpenGLDC::PushName(UINT name)
{
	glPushName(name);
}

void COpenGLDC::PopName()
{
	glPopName();
}

void COpenGLDC::CreateLight()
{
	GLfloat fGloablAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};

	GLfloat m_lightAmb0[]={0.5f,0.5f,0.5f,1.0f};
	GLfloat m_lightDif0[]={0.5f,0.5f,0.5f,1.0f};
	GLfloat m_lightSpe0[]={0.3f,0.3f,0.3f,0.5f};
	//GLfloat m_lightPos0[]={1.0f,1.0f,1.0f,0.0f};

	GLfloat m_lightAmb1[]={0.1f,0.1f,0.1f,0.5f};
	GLfloat m_lightDif1[]={0.3f,0.3f,0.3f,0.5f};
	GLfloat m_lightSpe1[]={0.3f,0.3f,0.3f,1.0f};
	//GLfloat m_lightPos1[]={-0.3f,0.0f,1.0f,0.0f};

	glLightfv(GL_LIGHT0, GL_AMBIENT, m_lightAmb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, m_lightDif0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, m_lightSpe0);
	//glLightfv(GL_LIGHT0, GL_POSITION, m_lightPos0);
	SetLightDirection(1.0, 1.0, 0.5);

	glLightfv(GL_LIGHT1, GL_AMBIENT, m_lightAmb1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, m_lightDif1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, m_lightSpe1);
	//glLightfv(GL_LIGHT1, GL_POSITION, m_lightPos1);
	SetLightDirection(-0.3f,0.5f,0.2f);

	//全局环境光设置，产生环境光
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fGloablAmbient);/////
	GLfloat local_view[] =
	{0.0};
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

	//双面光照（需要显示背面)
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);/////

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

}
//旋转视图
void COpenGLDC::RotateView(double dRotX,double dRotY,double dRotZ)
{
	if (dRotX)
	{
		m_Camera.rotate_x_view(dRotX);
	}

	if (dRotY)
	{
		m_Camera.rotate_y_view(dRotY);
	}

	if (dRotZ)
	{
		m_Camera.rotate_z_view(dRotZ);
	}
}

//绕给定矢量旋转
void COpenGLDC::RotateView(double dRot,VECTOR3D vect)
{
	m_Camera.rotate_view(dRot, vect);
}

//绕屏幕给定轴旋转
void COpenGLDC::RotateView(double dRot,short snXyz)
{
	m_Camera.rotate_view(dRot,snXyz);
}
//三维空间写文字
void COpenGLDC::DrawText(char* string,int flag)
{
	char* p = NULL;

	for (p = string; *p; p++)
	{
		if(flag==24)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
		else if(flag==18)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
		else
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
	}
}

