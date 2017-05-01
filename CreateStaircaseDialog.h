/****************************************************************************************/
/*  CreateStaircaseDialog.h                                                             */
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
#ifndef CREATESTAIRCASEDIALOG_H
#define CREATESTAIRCASEDIALOG_H

//#include "resource.h"
#include "BrushTemplate.h"
#include "GEditProDoc.h"

class CCreateStaircaseDialog : public CDialog
{
// Construction
public:
//	virtual int DoModal(geBoolean ConvertToMetric, BrushTemplate_Staircase *pStairTemplate);
void ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Staircase *pStairTemplate, CGEditProDoc *pDoc);

	CCreateStaircaseDialog(CWnd* pParent = NULL);   // standard constructor
		
	bool UpdateCreateStaircaseDlg(CGEditProDoc *pDoc);

	bool m_customTemplate;

	const Brush *pStairs;
	Brush *m_recentStairs;

// Dialog Data
	//{{AFX_DATA(CCreateStaircaseDialog)
	enum { IDD = IDD_STAIRCASEDIALOG };
	CButton	m_addStairs;
	float	m_Height;
	float	m_Length;
	float	m_Width;
	BOOL	m_TCut;
	int		m_NumberOfStairs;
	BOOL	m_MakeRamp;
	BOOL	m_customizeStairs;
	CString	m_minMaxErrorString;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateStaircaseDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
private:
	void		dlgFieldsToCentimeters(void);
	void		dlgFieldsToTexels(void);
	geBoolean	m_ConvertToMetric;
	BrushTemplate_Staircase *m_pStairTemplate;
	CGEditProDoc *m_pDoc;
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateStaircaseDialog)
	afx_msg void OnDefaults();
	virtual void OnOK();
	afx_msg void OnAddStairs();
	afx_msg void OnCustomizeStairs();
	afx_msg void OnKillfocusHeight();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnKillfocusLength();
	afx_msg void OnKillfocusNumberofstairs();
	afx_msg void OnTcut();
	afx_msg void OnDestroy();
	afx_msg void OnMakeRamp();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
