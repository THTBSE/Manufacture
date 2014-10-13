// OpenGLDC.h: interface for the COpenGLDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLDC_H__30E692A3_4129_11D4_B1EE_0010B539EBC0__INCLUDED_)
#define AFX_OPENGLDC_H__30E692A3_4129_11D4_B1EE_0010B539EBC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\stdafx.h"
#include "glew.h"
#include "gl.h"
#include "glu.h"
#include "glaux.h"
#include "glut.h"
#include <vector>
#include "camera.h"
#include "cadbase.h"

using namespace std;

#define BUFFER_LENGTH 64

#define ROTATE_ANGLE			-0.08726646260
#define ROTATE_ANGLE_NEG		0.08726646260

class  COpenGLDC
{
public:
	COpenGLDC(HWND hWnd);

	//COpenGLDC(){};

	virtual ~COpenGLDC();

private:
	HWND				m_hWnd;
	HGLRC				m_hRC;
	HDC					m_hDC;

	COLORREF			m_clrBk;		//Background Color
	COLORREF			m_clr;			//Polygon Color for unshading
	COLORREF			m_clrHighlight; //for highlight using
	COLORREF			m_clrMaterial;  //for normal rendering
	COLORREF			m_clrBkMaterial;  //for normal rendering
	BOOL				m_bShading;		//use material property
   //GLfloat			m_vecLight[3][3];	//lighting direction
	GLfloat				m_vecLight[3];
	BOOL				m_NormolViewMode;  //正常显示还是同时显示着色和线框模式

	//selection
	BOOL				m_bSelectionMode;
	GLuint				m_selectBuff[BUFFER_LENGTH];

	GLuint				texture[1];		// 存储一个纹理

public:
	GCamera				m_Camera;


protected:
	void					ClearBkground();
	void					OnShading();

public:
	//initialize
	BOOL					InitDC();
	void					GLResize(int cx,int cy);
	void					GLSetupRC();

	//uMode :zero for normal rendering. non-zero for selection
	void					Ready();
	void					Finish();

	/////////////////////// light & color //////////////////////
	void					Shading(BOOL bShading);
	BOOL					IsShading();

	void					Lighting(BOOL bLighting);
	BOOL					IsLighting();

	void					ViewMode();

	//Light direction
	void					SetLightDirection(float dx,float dy,float dz);
	void					GetLightDirection(float& dx,float& dy,float& dz);
			//Light direction
//	void SetLightDirection(float dx,float dy,float dz, short sn);
//	void GetLightDirection(float& dx,float& dy,float& dz, short sn);

	//material
	 void					SetMaterialColor(COLORREF clr);
	void					GetMaterialColor(COLORREF& clr);

	void					SetBkMaterialColor(COLORREF clr);
	void					GetBkMaterialColor(COLORREF& clr);
	
	//back ground
	void					SetBkColor(COLORREF rgb);
	void					GetBkColor(COLORREF& rgb);
	
	//frame material
	void					SetColor(COLORREF rgb);
	void					GetColor(COLORREF& rgb);

	//high light setting
	void					SetHighlightColor(COLORREF clr);
	void					GetHighlightColor(COLORREF& clr);
	void					Highlight(BOOL bLight = TRUE);




	/////////////////// Drawing ////////////////////
	void					DrawPoint(const CPoint3D&);
	void					DrawPoint(const CPoint3D& pt, COLORREF ctr);
	void					DrawJuxing(const CPoint3D& p1,const CPoint3D& p2,const CPoint3D& p3,const CPoint3D& p4);

	//drawing curves
	void					DrawCoord();

	void					DrawLine(const CPoint3D& sp,const CPoint3D& ep);
	void					DrawPolyline(const CPoint3D* pt,int size);
	void					DrawPolyline(const vector<CPoint3D> pts, int R, int G, int B);
	
	//drawing surface

	void					DrawTriChipwithtecture(double n0,double n1,double n2,double v00,double v01,double v02,
		double v10,double v11,double v12,double v20,double v21,double v22);


	void					DrawQuads(vector<CPoint3D> waist1, vector<CPoint3D> waist2, vector<CVector3D> norm);

	//drawing solid entities
//	void					DrawSphere(const CPoint3D& cen,double r);
	//克隆
//	void					Draw_Transparent_Sphere(const CPoint3D& cen,float rgba[4],float radius=1.0);

//	void					DrawSphere(const CPoint3D& cen,double r,const CVector3D& vec);
//	void					DrawCylinder(const CPoint3D& cen,double r,const CVector3D& h);
//	void					DrawCylinder(const vector<CPoint3D> pdata, double r);
//	void					DrawCone(const CPoint3D& cen,double r,const CVector3D& h);
//	void					DrawTorus(const CPoint3D& cen,const CVector3D& ax,double r_in,double r_out);

	//selection Mode
	void					BeginSelection(int xPos,int yPos);
	int						EndSelection(UINT* items);
	BOOL					IsSelectionMode();

	void					InitNames();
	void					LoadName(UINT name);
	void					PushName(UINT name);
	void					PopName();

	void					CreateLight();
	//旋转视图
	void					RotateView(double dRotX,double dRotY,double dRotZ);
	void					RotateView(double dRot,VECTOR3D vect);
	void					RotateView(double dRot,short snXyz);
	//
	void					TransWorldtoScreen();
	void					EndTransWorldtoScreen();
	void					DrawText(char* string, int flag=12);
	void					FinshForEps();//仅应用于Eps输出


//	void                 DrawSphereb(const CPoint3D& cen,double r);   //蓝色（blue）小球
//	void                 DrawSpherep(const CPoint3D& cen,double r);   //粉色（pink）小球
    void                 DrawLine(const CPoint3D& sp,const CPoint3D& ep,int R,int G,int B);
};

class  CGLView : public CView
{
public:
	COpenGLDC*			m_pGLDC;
protected:
	
	
	CPoint				m_pntMouseMove;
	CPoint				m_pntLBntDown;
	CPoint				m_pntMBntDown;
	CPoint				m_pntRBntDown;

	CString				m_strViewName;

	//新添加
	GLuint				texture[1];		// 存储一个纹理

protected: // create from serialization only
	CGLView();
	DECLARE_DYNCREATE(CGLView)

// Attributes
public:
	virtual void			RenderScene(COpenGLDC* pDC);
	void					OnViewType(UINT type);
	void					Orient_To_Plane(CVector3D nor);  //视图定向于某一指定的平面

	CString					GetViewName();
	void					SetViewName(LPCTSTR);
		
	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLView)
	public:
	virtual void		OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void		DrawLegend(COpenGLDC* pDC);
	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGLView();

protected:
	virtual void		OnInitialUpdate();
// Generated message map functions
protected:
	//{{AFX_MSG(CGLView)
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL			GetBox(double& x0,double& y0,double& z0,double& x1,double& y1,double& z1);
	
	void					Zoom(double dScale);
	void					ZoomAll();
	void					MoveView(double dpx,double dpy);

	//旋转视图
	void					RotateView(double dRotX,double dRotY,double dRotZ);
	void					RotateView(double dRot,VECTOR3D vect);
	void					RotateView(double dRot,short snXyz);

	//新添加
// 	AUX_RGBImageRec			*LoadBMP(CHAR *Filename);			// 载入位图图象
// 	int						LoadGLTextures(CHAR *Filename);										// 载入位图(调用上面的代码)并转换成纹理

	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL	OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:

};
#endif // !defined(AFX_OPENGLDC_H__30E692A3_4129_11D4_B1EE_0010B539EBC0__INCLUDED_)
