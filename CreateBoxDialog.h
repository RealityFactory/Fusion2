/****************************************************************************************/
/*  CreateBoxDialog.h                                                                   */
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
#ifndef CREATEBOXDIALOG_H
#define CREATEBOXDIALOG_H
#include "GEditProDoc.h"

//#include "resource.h"
#include "BrushTemplate.h"

class CCreateBoxDialog : public CDialog
{
// Construction
public:
////	virtual int DoModal(geBoolean ConvertToMetric, BrushTemplate_Box *pBoxTemplate); // old gedit
	void ShowDialog(geBoolean ConvertToMetric, 	BrushTemplate_Box *pBoxTemplate, CGEditProDoc *pDoc);
	CCreateBoxDialog(CWnd* pParent = NULL);   // standard constructor old gedit

	~CCreateBoxDialog();

	bool UpdateCreateBoxDlg(CGEditProDoc *pDoc);

	bool m_customTemplate;

	const	Brush	*pCube;
	Brush	*m_recentCube;
// Dialog Data
	//{{AFX_DATA(CCreateBoxDialog)
	enum { IDD = IDD_CREATEBOX };
	CButton	m_applyBoxBTN;
	CButton	m_customTemplateBTN;
	CButton	m_CutBtn;
	CButton	m_SheetBtn;
	CStatic	m_Picture;
	float	m_YSize;
	int		m_Solid;
	float	m_XSizeBot;
	float	m_XSizeTop;
	float	m_ZSizeBot;
	float	m_ZSizeTop;
	BOOL	m_TCut;
	float	m_Thickness;
	BOOL	m_TSheet;
	CString	m_minMaxErrorString;
	CString m_wallThicknessErrorString;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateBoxDialog)
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
	BrushTemplate_Box *m_pBoxTemplate;

	CGEditProDoc *m_pDoc;

protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateBoxDialog)
	afx_msg void OnSolid();
	afx_msg void OnHollow();
	virtual BOOL OnInitDialog();
	afx_msg void OnDefaults();
	afx_msg void OnTcut();
	virtual void OnOK();
	afx_msg void OnTsheet();
	afx_msg void OnKillfocusXsizetop();
	afx_msg void OnKillfocusZsizebot();
	afx_msg void OnKillfocusYsize();
	afx_msg void OnKillfocusThickness();
	afx_msg void OnApplyBoxBtn();
	afx_msg void OnKillfocusXsizebot();
	afx_msg void OnKillfocusZsizetop();
	afx_msg void OnCustomTemplateBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CBitmap mHollowBitmap, mSolidBitmap;
};


#endif
