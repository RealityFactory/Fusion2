#if !defined(AFX_SCALEDIALOG_H__38189854_AADC_11D4_A52D_006008BFCEBD__INCLUDED_)
#define AFX_SCALEDIALOG_H__38189854_AADC_11D4_A52D_006008BFCEBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScaleDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScaleDialog dialog

class CScaleDialog : public CDialog
{
// Construction
public:
	CScaleDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScaleDialog)
	enum { IDD = IDD_SCALEDIALOG };
	float	m_X;
	float	m_Y;
	float	m_Z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleDialog)
	public:
	virtual int DoModal(geVec3d *PassedScaleVector);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScaleDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
private:
	geVec3d *ScaleVector;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCALEDIALOG_H__38189854_AADC_11D4_A52D_006008BFCEBD__INCLUDED_)
