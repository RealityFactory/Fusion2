#if !defined(AFX_EXPORT3DSDIALOG_H__B7780D04_359F_11D8_BEB2_000374890932__INCLUDED_)
#define AFX_EXPORT3DSDIALOG_H__B7780D04_359F_11D8_BEB2_000374890932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Export3dsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExport3dsDialog dialog

class CExport3dsDialog : public CDialog
{
// Construction
public:
	CExport3dsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExport3dsDialog)
	enum { IDD = IDD_EXPORT3DSDIALOG };
	int		m_ExportAll;
	BOOL	m_ExportLights;
	BOOL	m_GroupFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExport3dsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExport3dsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORT3DSDIALOG_H__B7780D04_359F_11D8_BEB2_000374890932__INCLUDED_)
