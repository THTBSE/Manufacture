// Dashboard.cpp : implementation file
//

#include "../stdafx.h"
#include "../GeometryProc.h"
#include "Dashboard.h"
#include "afxdialogex.h"
#include "../MainFrm.h"
#include "../GeometryProcDoc.h"

// CDashboard dialog

IMPLEMENT_DYNAMIC(CDashboard, CDialogEx)

CDashboard::CDashboard(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDashboard::IDD, pParent)
{
	extractStyle = 0;
	meshIsBind = false;
}

CDashboard::~CDashboard()
{
}

BOOL CDashboard::OnInitDialog()
{
	InitializeToolTipCtrl();
	return TRUE;
}



void CDashboard::InitializeToolTipCtrl()
{
	EnableToolTips(TRUE);
	m_ttctrl.Create(this);
	m_ttctrl.Activate(TRUE);
	CWnd* pWnd = GetDlgItem(IDC_ACOUNTOUR);
	m_ttctrl.AddTool(pWnd, _T("开启添加轮廓边界模式，点选轮廓线进行添加"));
	pWnd = GetDlgItem(IDC_ERASECOUNTOUR);
	m_ttctrl.AddTool(pWnd, _T("开启删除轮廓边界模式，点选轮廓线进行删除"));
	m_ttctrl.SetMaxTipWidth(100);
}

BOOL CDashboard::PreTranslateMessage(MSG* pMsg)
{
	m_ttctrl.RelayEvent(pMsg); // <- listen mouse messages!
	return CDialog::PreTranslateMessage(pMsg);
}

void CDashboard::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDashboard, CDialogEx)
	ON_BN_CLICKED(IDC_DCOUNTOUR, &CDashboard::OnDetectContour)
	ON_BN_CLICKED(IDC_SETSTANDARDPLANE, &CDashboard::OnBnClickedSetstandardplane)
	ON_BN_CLICKED(IDC_DUMPPLANE, &CDashboard::OnBnClickedDumpplane)
	ON_BN_CLICKED(IDC_RADIO1, &CDashboard::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CDashboard::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BINDMESH, &CDashboard::OnBnClickedBindmesh)
END_MESSAGE_MAP()


// CDashboard message handlers


void CDashboard::OnDetectContour()
{
	CGeometryProcDoc* pDoc = (CGeometryProcDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if (pDoc->meshes.empty())
	{
		MessageBox(_T("未加载模型！"));
		return;
	}
	if (extractStyle == 0)
	{
		MessageBox(_T("请先选取提取方式！"));
		return;
	}

	pDoc->VCalcInstance.set_edgeGetter(extractStyle);
	pDoc->VCalcInstance.detect_borderline();
	pDoc->VCalcInstance.selected_sub_mesh_area();
}


void CDashboard::OnBnClickedSetstandardplane()
{
	if (!meshIsBind)
	{
		MessageBox(_T("请先点击\'绑定模型\'！"));
		return;
	}
	const int needStandardPlane = 0;
	CGeometryProcDoc* pDoc = (CGeometryProcDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	pDoc->procController.change_proc(vfclController::SETPLANE);
	pDoc->VCalcInstance.set_plane_type(needStandardPlane);
}



void CDashboard::OnBnClickedDumpplane()
{
	if (!meshIsBind)
	{
		MessageBox(_T("请先点击\'绑定模型\'！"));
		return;
	}
	const int needDumpPlane = 2;
	CGeometryProcDoc* pDoc = (CGeometryProcDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	pDoc->procController.change_proc(vfclController::SETPLANE);
	pDoc->VCalcInstance.set_plane_type(needDumpPlane);
}


void CDashboard::OnBnClickedRadio1()
{
	if (!meshIsBind)
	{
		MessageBox(_T("请先点击\'绑定模型\'！"));
		return;
	}
	extractStyle = STANDARDGETTER;
	CGeometryProcDoc* pDoc = (CGeometryProcDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	pDoc->VCalcInstance.set_edgeGetter(extractStyle);
}


void CDashboard::OnBnClickedRadio2()
{
	if (!meshIsBind)
	{
		MessageBox(_T("请先点击\'绑定模型\'！"));
		return;
	}
	extractStyle = BRUTEFORCEGETTER;
	CGeometryProcDoc* pDoc = (CGeometryProcDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	pDoc->VCalcInstance.set_edgeGetter(extractStyle);
}


void CDashboard::OnBnClickedBindmesh()
{
	CGeometryProcDoc* pDoc = (CGeometryProcDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if (!pDoc->meshes.empty())
	{
		pDoc->VCalcInstance.set_mesh(pDoc->meshes[pDoc->Current_mesh]);
		meshIsBind = true;
	}
	else
	{
		MessageBox(_T("请先加载模型"));
		return;
	}
}
