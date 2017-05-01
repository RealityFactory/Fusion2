/****************************************************************************************/
/*  CreateSpheroidDialog.h                                                              */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax                                    */
/*  Description:  Genesis world editor header file                                      */
/*                                                                                      */
/*  The contents of this file are subject to the Genesis3D Public License               */
/*  Version 1.01 (the "License"); you may not use this file except in                   */
/*  compliance with the License. You may obtain a copy of the License at                */
/*  http://www.genesis3d.com                                                            */
/*                                                                                      */
/*  Software distributed under the License is distributed on an "AS IS"                 */
/*  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See                */
/*  the License for the specific language governing rights and limitations              */
/*  under the License.                                                                  */
/*                                                                                      */
/*  The Original Code is Genesis3D, released March 25, 1999.                            */
/*  Genesis3D Version 1.1 released November 15, 1999                                 */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/
#ifndef CREATESPHEROIDDIALOG_H
#define CREATESPHEROIDDIALOG_H

//#include "resource.h"
#include "BrushTemplate.h"
#include "GEditProDoc.h"

class CCreateSpheroidDialog : public CDialog
{
// Construction
public:
////	virtual int DoModal(geBoolean ConvertToMetric, BrushTemplate_Spheroid *pTemplate);
	void ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Spheroid *pSphereTemplate, CGEditProDoc *pDoc);

	CCreateSpheroidDialog(CWnd* pParent = NULL);   // standard constructor
	~CCreateSpheroidDialog();

	
	bool UpdateCreateSpheroidDlg(CGEditProDoc *pDoc);

	bool m_customTemplate;

	const	Brush	*pSphere;
	Brush *m_recentSphere;

// Dialog Data
	//{{AFX_DATA(CCreateSpheroidDialog)
	enum { IDD = IDD_CREATE_SPHEROID };
	CButton	m_addSphere;
	CStatic	m_Picture;
	int		m_HorizontalBands;
	int		m_VerticalBands;
	float	m_YSize;
	int		m_Solid;
	float	m_Thickness;
	BOOL	m_TCut;
	BOOL	m_customizeSphere;
	CString	m_minMaxErrorString;
	CString m_wallThicknessErrorString;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateSpheroidDialog)
	public:
	virtual void Serialize(CArchive& ar);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
private:
	void		dlgFieldsToCentimeters(void);
	void		dlgFieldsToTexels(void);
	geBoolean	m_ConvertToMetric;
	BrushTemplate_Spheroid *m_pSphereTemplate;

	CGEditProDoc	*m_pDoc;
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateSpheroidDialog)
	afx_msg void OnHollow();
	afx_msg void OnSolid();
	virtual BOOL OnInitDialog();
	afx_msg void OnDefaults();
	virtual void OnOK();
	afx_msg void OnAddSphereBtn();
	afx_msg void OnCustomizeSphereBtn();
	afx_msg void OnKillfocusYsize();
	afx_msg void OnKillfocusVertstripes();
	afx_msg void OnKillfocusHorstripes();
	afx_msg void OnKillfocusThickness();
	afx_msg void OnTcut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CBitmap mSolidSphere, mHollowSphere;
};

#endif
