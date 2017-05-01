/****************************************************************************************/
/*  BrushAttributesDialog.h                                                             */
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
/*  Copyright (C) 1996-1999 Eclipse Entertainment, L.L.C. All Rights Reserved           */
/*                                                                                      */
/****************************************************************************************/
#ifndef BRUSHATTRIBUTESDIALOG_H
#define BRUSHATTRIBUTESDIALOG_H

#include "brush.h"
#include "resource.h"

class CMainFrame;

class CFusionDoc;

enum BrushTypeOption
{
	IDRB_SOLID,
	IDRB_EMPTY,
	IDRB_CLIP,
	IDRB_WINDOW,
	IDRB_HINT,
	IDRB_SUBTRACT
};

class CBrushAttributesDialog : public CDialog
{
public:
	void UpdateBrushFocus ();
//	CBrushAttributesDialog(CFusionDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	CBrushAttributesDialog(CMainFrame* pMainFrame, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CBrushAttributesDialog)
	enum { IDD = IDD_BRUSHDIALOG };
	CCheckListBox	m_ContentsList;
	CString	m_Name;
	BOOL	m_Wavy;
	BOOL	m_Area;
//	BOOL	m_Translucent;
	int		m_BrushType;
	BOOL	m_Detail;
	BOOL	m_Flocking;
	BOOL	m_Sheet;
	CString m_NumBrushes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushAttributesDialog)
	protected:
	virtual	BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	uint16	m_OldFlags ;

	// Generated message map functions
	//{{AFX_MSG(CBrushAttributesDialog)
	afx_msg void OnBrushsolid();
	afx_msg void OnBrushclip();
	afx_msg void OnBrushwindow();
	afx_msg void OnBrushhint();
	afx_msg void OnBrushsubtract();
	afx_msg void OnBrushempty();
	afx_msg void OnBrushflocking();
	afx_msg void OnBrushsheet();
	afx_msg void OnClose();
	afx_msg void OnBrushdetail();
	afx_msg void OnBrusharea();
	afx_msg void OnBrushwavy();
	afx_msg void OnKillfocusName();
	afx_msg void OnSelchangeContentslist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int BrushColor;
//	CFusionDoc	*m_pDoc;
	CMainFrame *m_pMainFrame;

	void OnRadioButton( BrushTypeOption NewValue ) ;
//	void EnableTranslucency (void);
//	void EnableHullsize (void);
	void SetDialogFields (void);
	int	 BrushFlagsToIndex(  Brush *pBrush ) ;
	void BrushOptionsFromType( void ) ;
	void SetFlagsToBrushType( void ) ;
	void AssignCurrentToValues();
	void AssignCurrentToViews();
};

#endif
