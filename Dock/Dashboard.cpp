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
	pDoc->VCalcInstance.set_mesh(pDoc->meshes[pDoc->Current_mesh]);
//	pDoc->VCalcInstance.get_standard_curve(pDoc->meshes[0]);
	pDoc->VCalcInstance.detect_borderline();
	pDoc->procController.change_proc(pDoc->procController.VOLUMECALC);

}
