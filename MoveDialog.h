#if !defined(AFX_MOVEDIALOG_H__6DABECD5_A906_11D4_A52A_006008BFCEBD__INCLUDED_)
#define AFX_MOVEDIALOG_H__6DABECD5_A906_11D4_A52A_006008BFCEBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoveDialog.h : header file
//

#include <assert.h>

/////////////////////////////////////////////////////////////////////////////
// CMoveDialog dialog

class CMoveDialog : public CDialog
{
// Construction
public:
	CMoveDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMoveDialog)
	enum { IDD = IDD_MOVEDIALOG };
	int		m_MoveType;
	float	m_XFinal;
	float	m_YFinal;
	float	m_ZFinal;
	float	m_X;
	float	m_Y;
	float	m_Z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoveDialog)
	public:
	virtual int DoModal(geVec3d *Position, geBoolean ShowX, geBoolean ShowY, geBoolean ShowZ);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMoveDialog)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeXedit();
	afx_msg void OnChangeYedit();
	afx_msg void OnChangeZedit();
	afx_msg void OnAbsolute();
	afx_msg void OnRelative();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	geVec3d *Position;
	geBoolean XVisible;
	geBoolean YVisible;
	geBoolean ZVisible;
	void UpdateXValue();
	void UpdateYValue();
	void UpdateZValue();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVEDIALOG_H__6DABECD5_A906_11D4_A52A_006008BFCEBD__INCLUDED_)
