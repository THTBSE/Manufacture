#pragma once


// CDashboard dialog

class CDashboard : public CDialogEx
{
	DECLARE_DYNAMIC(CDashboard)

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
	void InitializeToolTipCtrl();
public:
	afx_msg void OnDetectContour();
	afx_msg void OnBnClickedOk();
};
