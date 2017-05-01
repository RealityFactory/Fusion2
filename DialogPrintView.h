#if !defined(AFX_DIALOGPRINTVIEW_H__C87788C0_5743_11D5_9555_EA8C14B00A26__INCLUDED_)
#define AFX_DIALOGPRINTVIEW_H__C87788C0_5743_11D5_9555_EA8C14B00A26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogPrintView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogPrintView dialog

class CDialogPrintView : public CDialog
{
// Construction
public:
	CDialogPrintView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogPrintView)
	enum { IDD = IDD_PRINT_VIEW_DLG };
	BOOL	m_btnTopView;
	BOOL	m_btnFrontView;
	BOOL	m_btnSideView;
	BOOL	m_btnTexturedView;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPrintView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPrintView)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnOK ();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGPRINTVIEW_H__C87788C0_5743_11D5_9555_EA8C14B00A26__INCLUDED_)
