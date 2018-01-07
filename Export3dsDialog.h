// added QD 12/03
#if !defined(AFX_EXPORT3DSDIALOG_H__B1E9B1E1_33A6_11D8_BEB2_000374890932__INCLUDED_)
#define AFX_EXPORT3DSDIALOG_H__B1E9B1E1_33A6_11D8_BEB2_000374890932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Export3dsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Export3dsDialog dialog

class Export3dsDialog : public CDialog
{
// Construction
public:
	Export3dsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Export3dsDialog)
	enum { IDD = IDD_EXPORT3DSDIALOG };
	int		m_ExportAll;
	BOOL	m_ExportLights;
	BOOL	m_GroupFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Export3dsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Export3dsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORT3DSDIALOG_H__B1E9B1E1_33A6_11D8_BEB2_000374890932__INCLUDED_)
