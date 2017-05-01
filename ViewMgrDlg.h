#if !defined(AFX_VIEWMGRDLG_H__310B7DE3_BF60_11D5_9556_444553540000__INCLUDED_)
#define AFX_VIEWMGRDLG_H__310B7DE3_BF60_11D5_9556_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewMgrDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewMgrDlg dialog

class CViewMgrDlg : public CDialog
{
// Construction
public:
	CViewMgrDlg(CWnd* pParent = NULL);   // standard constructor

	bool	ResetButtons();
	bool CViewMgrDlg::EnableButtons(BOOL bWhat);
	
	// Dialog Data
	//{{AFX_DATA(CViewMgrDlg)
	enum { IDD = IDD_VIEWMGR_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewMgrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



// Implementation
protected:



	// Generated message map functions
	//{{AFX_MSG(CViewMgrDlg)
	afx_msg void OnBtnViewUL();
	afx_msg void OnBtnViewUR();
	afx_msg void OnBtnViewLL();
	afx_msg void OnBtnViewLR();
	afx_msg void OnBtnViewRestore();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWMGRDLG_H__310B7DE3_BF60_11D5_9556_444553540000__INCLUDED_)
