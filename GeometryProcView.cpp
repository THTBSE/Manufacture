
// GeometryProcView.cpp : CGeometryProcView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "GeometryProc.h"
#endif

#include "GeometryProcDoc.h"
#include "GeometryProcView.h"

#include "ViewSpace/BackGround.h"
#include "ViewSpace/ShowShader.h"
#include "SelectionTool/OpenglSelection.h"
#include "SelectionTool/circleSelection.h"
#include "BaseToolClass/FCLToolkit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGeometryProcView

IMPLEMENT_DYNCREATE(CGeometryProcView, CGLView)

BEGIN_MESSAGE_MAP(CGeometryProcView, CGLView)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_CALLDASHBOARDDLG, &CGeometryProcView::OnCalldashboarddlg)
	ON_COMMAND(ID_REVOCATION, &CGeometryProcView::OnRevocation)
END_MESSAGE_MAP()

// CGeometryProcView ����/����

CGeometryProcView::CGeometryProcView()
{
	pDashboard = NULL;

}

CGeometryProcView::~CGeometryProcView()
{
}

BOOL CGeometryProcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CGeometryProcView ����


void CGeometryProcView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
// 	ClientToScreen(&point);
// 	OnContextMenu(this, point);
}

void CGeometryProcView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGeometryProcView ���

#ifdef _DEBUG
void CGeometryProcView::AssertValid() const
{
	CView::AssertValid();
}

void CGeometryProcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGeometryProcDoc* CGeometryProcView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGeometryProcDoc)));
	return (CGeometryProcDoc*)m_pDocument;
}
#endif //_DEBUG


// CGeometryProcView Messages Processing Code 
void CGeometryProcView::OnSize(UINT nType, int cx, int cy)
{
	CGLView::OnSize(nType, cx, cy);
}

void CGeometryProcView::OnMButtonDown(UINT nFlags, CPoint point)
{
	CGLView::OnMButtonDown(nFlags,point);
}

void CGeometryProcView::OnMouseMove(UINT nFlags, CPoint point)
{
	CGLView::OnMouseMove(nFlags,point);
}

BOOL CGeometryProcView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CGLView::OnMouseWheel(nFlags,zDelta,pt);
}

void CGeometryProcView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CGLView::OnLButtonDown(nFlags,point);
	LButtonDownController(point);
}

void CGeometryProcView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CGLView::OnLButtonUp(nFlags,point);
}

void CGeometryProcView::RenderScene(COpenGLDC* pDC)
{
	CGeometryProcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CBackGround::RenderBackGround(pDoc->m_BackGroundColor);
	if (!pDoc->meshes.empty())
	{ 
		float mat[4]={0.5f,0.5f,0.6f,1.0f};
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
		mat[0] = (GLfloat)192.0/255;
		mat[1] = (GLfloat)192.0/255;
		mat[2] = (GLfloat)192.0/255;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
		glMaterialf(GL_FRONT, GL_SHININESS,128.0);
		CShowShader::ShaderMesh(pDoc->meshes[pDoc->Current_mesh],pDoc->m_ShaderModel);
		CBackGround::RenderInformation(pDoc->meshes[pDoc->Current_mesh]);
	}
	pDoc->VCalcInstance.draw();
	pDC->m_Camera.BeginGetMatrix();
}

void CGeometryProcView::OnInitialUpdate()
{
	CGLView::OnInitialUpdate();
}



BOOL CGeometryProcView::GetBox(double& x0,double& y0,double& z0,double& x1,double& y1,double& z1)
{
	CGeometryProcDoc* pDoc = GetDocument();
	ASSERT(pDoc);

	if (!pDoc->meshes.empty())
	{
		x0 =pDoc->meshes[pDoc->Current_mesh]->bbox.min[0]; 
		y0 =pDoc->meshes[pDoc->Current_mesh]->bbox.min[1]; 
		z0 =pDoc->meshes[pDoc->Current_mesh]->bbox.min[2];
		x1 =pDoc->meshes[pDoc->Current_mesh]->bbox.max[0]; 
		y1 =pDoc->meshes[pDoc->Current_mesh]->bbox.max[1];   
		z1 =pDoc->meshes[pDoc->Current_mesh]->bbox.max[2];  
		return TRUE;
	}
	return FALSE;
}

void CGeometryProcView::LButtonDownController(const CPoint& ScreenPoint)
{
	CGeometryProcDoc* pDoc = GetDocument();
	assert(pDoc);

	switch(pDoc->procController.query_proc())
	{
	case vfclController::VOLUMECALC:
		{
			vector<vec2> sp;
			vector<int> borderline;
			vec p = COpenglSelection::GetMousePoint3D(m_pGLDC,ScreenPoint);
			int k = vfclToolkit::PointOnMesh(pDoc->meshes[pDoc->Current_mesh],p);
			if (k == -1 || !pDoc->VCalcInstance.set_highlight(k))
				return;
			int bdr = pDoc->VCalcInstance.get_highlight(borderline);
			circleSelection::toScreen(pDoc->meshes[pDoc->Current_mesh],borderline,sp,m_pGLDC);
			vec2 insidep = circleSelection::getPointWithinCircle(sp);
			vec seed = COpenglSelection::GetMousePoint3D(m_pGLDC,CPoint(insidep[0],insidep[1]));
			int seedIndex = vfclToolkit::PointOnMesh(pDoc->meshes[pDoc->Current_mesh],seed);
			if (seedIndex == -1)
			{
				AfxMessageBox(_T("δ�����ѡȡ�������ŰѴ�ѡȡ�߽������Լ�"));
				return;
			}
			pDoc->VCalcInstance.choose_border_seed(bdr, seedIndex);
			pDoc->VCalcInstance.selected_sub_mesh_area();
		}
	default:
		break;
	}

}

void CGeometryProcView::OnCalldashboarddlg()
{
	if (pDashboard)
	{
		pDashboard->ShowWindow(SW_SHOW);
	}
	else
	{
		pDashboard = new CDashboard;
		pDashboard->Create(IDD_DIALOG1, this);
		pDashboard->ShowWindow(SW_SHOW);
	}
}


void CGeometryProcView::OnRevocation()
{
	CGeometryProcDoc* pDoc = GetDocument();
	switch (pDoc->procController.query_proc())
	{
	case vfclController::VOLUMECALC:
		{
			pDoc->VCalcInstance.remove_a_sub_mesh_area();
		}
		break;
	default:
		break;
	}
}
