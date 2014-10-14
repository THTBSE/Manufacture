
// GeometryProcView.h : CGeometryProcView interface

#pragma once
#include "opengl/OpenGLDC.h"
#include "Dock/Dashboard.h"

class CGeometryProcView : public CGLView
{
protected:
	CGeometryProcView();
	DECLARE_DYNCREATE(CGeometryProcView)

public:
	CGeometryProcDoc* GetDocument() const;

//Dialog
	CDashboard* pDashboard;

//Controller 
public:
	void				LButtonDownController(const CPoint& ScreenPoint);


public:
	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
	virtual void		RenderScene(COpenGLDC* pDC);
protected:
	virtual void		OnInitialUpdate();
	virtual BOOL		GetBox(double& x0,double& y0,double& z0,double& x1,double& y1,double& z1);

public:
	virtual ~CGeometryProcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

//Messages map 
protected:
	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void			OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void			OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL			OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCalldashboarddlg();
	afx_msg void OnRevocation();
};

#ifndef _DEBUG 
inline CGeometryProcDoc* CGeometryProcView::GetDocument() const
   { return reinterpret_cast<CGeometryProcDoc*>(m_pDocument); }
#endif

