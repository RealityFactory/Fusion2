/****************************************************************************************/
/*  CreateCylDialog.h                                                                   */
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
#ifndef CREATECYLDIALOG_H
#define CREATECYLDIALOG_H

//#include "resource.h"
#include "BrushTemplate.h"
#include "GEditProDoc.h"

class CCreateCylDialog : public CDialog
{
public:
////	virtual int DoModal(geBoolean ConvertToMetric, BrushTemplate_Cylinder *pCylTemplate);
	void ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Cylinder *pCylTemplate, CGEditProDoc *pDoc);

	CCreateCylDialog(CWnd* pParent = NULL);   // standard constructor

	bool UpdateCreateCylDlg(CGEditProDoc *pDoc);

	bool m_customTemplate;

	const	Brush	*pCyl;
	Brush *m_recentCyl;

// Dialog Data
	//{{AFX_DATA(CCreateCylDialog)
	enum { IDD = IDD_CREATE_CYL };
	CButton	m_applyCylBTN;
	float	m_BotXOffset;
	float	m_BotXSize;
	float	m_BotZOffset;
	float	m_BotZSize;
	int		m_Solid;
	float	m_TopXOffset;
	float	m_TopXSize;
	float	m_TopZOffset;
	float	m_TopZSize;
	float	m_YSize;
	float	m_RingLength;
	BOOL	m_TCut;
	int		m_VerticalStripes;
	float	m_Thickness;
	BOOL	m_customizeCylBtn;
	CString	m_minMaxErrorString;
	CString m_wallThicknessErrorString;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateCylDialog)
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
	BrushTemplate_Cylinder *m_pCylTemplate;
	CGEditProDoc *m_pDoc;
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateCylDialog)
	afx_msg void OnDefaults();
	virtual void OnOK();
	afx_msg void OnCustomizeCylBtn();
	afx_msg void OnApplyCylBtn();
	afx_msg void OnKillfocusTopxsize();
	afx_msg void OnKillfocusTopxoff();
	afx_msg void OnKillfocusTopzsize();
	afx_msg void OnKillfocusTopzoff();
	afx_msg void OnKillfocusBotxsize();
	afx_msg void OnKillfocusBotxoff();
	afx_msg void OnKillfocusBotzsize();
	afx_msg void OnKillfocusBotzoff();
	afx_msg void OnKillfocusYsize();
	afx_msg void OnKillfocusVertstripes();
	afx_msg void OnKillfocusThickness();
	afx_msg void OnKillfocusRinglength();
	afx_msg void OnSolid();
	afx_msg void OnHollow();
	afx_msg void OnRing();
	afx_msg void OnTcut();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
