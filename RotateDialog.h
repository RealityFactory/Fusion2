#if !defined(AFX_ROTATEDIALOG_H__00698BBC_A9AF_11D4_A52B_006008BFCEBD__INCLUDED_)
#define AFX_ROTATEDIALOG_H__00698BBC_A9AF_11D4_A52B_006008BFCEBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RotateDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRotateDialog dialog

class CRotateDialog : public CDialog
{
// Construction
public:
	CRotateDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRotateDialog)
	enum { IDD = IDD_ROTATEDIALOG };
	int		m_DegreesType;
	int		m_Direction;
	float	m_FreeDegrees;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRotateDialog)
	public:
	virtual int DoModal(geFloat *RotateRadians);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRotateDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	geFloat *FinalRotation;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROTATEDIALOG_H__00698BBC_A9AF_11D4_A52B_006008BFCEBD__INCLUDED_)
