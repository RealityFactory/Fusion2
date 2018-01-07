/****************************************************************************************/
/*  CreateArchDialog.h                                                                  */
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
/*  Genesis3D Version 1.1 released November 15, 1999                                    */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved                            */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002                          */
/****************************************************************************************/
#ifndef CREATEARCHDIALOG_H
#define	CREATEARCHDIALOG_H


#include "GEditProDoc.h"
#include "BrushTemplate.h"

class CCreateArchDialog : public CDialog
{
public:
///	virtual int DoModal(geBoolean ConvertToMetric, BrushTemplate_Arch *pArchTemplate);
	void ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Arch *pArchTemplate, CGEditProDoc *pDoc);
	CCreateArchDialog(CWnd *pParent=NULL);   // standard constructor

	bool UpdateCreateArchDlg(CGEditProDoc *pDoc);

	bool m_customTemplate;

	const	Brush	*pArch;
	Brush *m_recentArch;

// Dialog Data
	//{{AFX_DATA(CCreateArchDialog)
	enum { IDD = IDD_CREATE_ARCH };
	CButton	m_addArchBTN;
	CButton	m_customizeArchBTN;
	int		m_NumSlits;
	float	m_Thickness;
	float	m_Width;
	float	m_Radius;
	float	m_WallSize;
	int		m_Style;
	float	m_EndAngle;
	float	m_StartAngle;
	BOOL	m_TCut;
// changed QD 11/03
	int		m_Sides;
	int		m_CW;
	int		m_Shape;
	geFloat	m_Radius2;
	geFloat	m_Height;
	BOOL	m_Massive;
	BOOL	m_Steps;
// end change
	CString	m_minMaxErrorString;
	CString	m_angleErrorString;
	CString	m_archThicknessErrorString;
	CString m_wallThicknessErrorString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateArchDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
private:
	void		dlgFieldsToCentimeters(void);
	void		dlgFieldsToTexels(void);
	geBoolean	m_ConvertToMetric;
	BrushTemplate_Arch *m_pArchTemplate;
	CGEditProDoc *m_pDoc;
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateArchDialog)
	afx_msg void OnDefaults();
	afx_msg void OnSolid();
	afx_msg void OnHollow();
	afx_msg void OnRing();
	virtual void OnOK();
	afx_msg void OnAddArchBtn();
	afx_msg void OnKillfocusStartangle();
	afx_msg void OnKillfocusEndangle();
	afx_msg void OnKillfocusThickness();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnKillfocusRadius();
	afx_msg void OnKillfocusWallsize();
	afx_msg void OnKillfocusNumslits();
// changed QD 11/03
	afx_msg void OnKillfocusSides();
	afx_msg void OnKillfocusRadius2();
	afx_msg void OnKillfocusHeight();
	afx_msg void OnCCW();
	afx_msg void OnCW();
	afx_msg void OnRectangular();
	afx_msg void OnRound();
	afx_msg void OnMassive();
	afx_msg void OnSteps();
// end change
	afx_msg void OnCustomArchBtn();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
