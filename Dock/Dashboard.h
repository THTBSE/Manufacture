#pragma once
#include "afxwin.h"


// CDashboard dialog

class CDashboard : public CDialogEx
{
	DECLARE_DYNAMIC(CDashboard)
	const int STANDARDGETTER = 1;
	const int BRUTEFORCEGETTER = 2;
public:
	CDashboard(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDashboard();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnInitDialog();

private:
	CToolTipCtrl m_ttctrl;
	int extractStyle;
	void InitializeToolTipCtrl();
	bool meshIsBind;
public:
	afx_msg void OnDetectContour();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSetstandardplane();
	afx_msg void OnBnClickedDumpplane();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedBindmesh();
};
