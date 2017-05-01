#if !defined(AFX_NAMEWINDOW_H__BB9F18C6_1D42_11D4_BE85_B09D4EC13800__INCLUDED_)
#define AFX_NAMEWINDOW_H__BB9F18C6_1D42_11D4_BE85_B09D4EC13800__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NameWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNameWindow window

class CNameWindow : public CWnd
{
// Construction
public:
	CNameWindow();
	
	void Show(const char* pstr);


	void Hide() { ShowWindow(SW_HIDE); }
	CFont m_Font;

// Attributes
public:

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameWindow)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNameWindow();

	// Generated message map functions
protected:
	CString m_strName;

	//{{AFX_MSG(CNameWindow)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAMEWINDOW_H__BB9F18C6_1D42_11D4_BE85_B09D4EC13800__INCLUDED_)
