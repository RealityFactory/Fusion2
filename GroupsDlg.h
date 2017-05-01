/****************************************************************************************/
/*  GroupsDlg.h																			*/
/*                                                                                      */
/*  Author:       Tom Morris															*/
/*  Description:  Group Control															*/
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
/*	Genesis3D Version 1.1 released November 15, 1999				                    */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved							*/
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/


#if !defined(AFX_GROUPSDLG_H__0143E404_A1EB_11D4_9554_444553540000__INCLUDED_)
#define AFX_GROUPSDLG_H__0143E404_A1EB_11D4_9554_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupsDlg.h : header file

#include "ColorSelector\colorbutton.h"
#include "brush.h"
#include "flat_combo/XTFlatComboBox.h"



/////////////////////////////////////////////////////////////////////////////
// CGroupsDlg dialog

class CGroupsDlg : public CDialog
{
// Construction
public:
	CGroupsDlg(CWnd* pParent = NULL);   // standard constructor




	bool	SetupGroupDialog(CGEditProDoc	*pDoc);
	bool	UpdateGroups(CGEditProDoc* NewDoc);
	void	LoadComboBox();
	void	UpdateGroupSelection() ;
	void	SelectNone();


// Dialog Data
	//{{AFX_DATA(CGroupsDlg)
	enum { IDD = IDD_GROUPDIALOG };
	CButton	m_bDeselctBrushesBtn;
	CXTFlatComboBox	m_cGroupCombo;
	CButton	m_bSelectAllBrushesBtn;
	CButton	m_bRemoveGroupBtn;
	CButton	m_bCreateNewGroupBtn;
	CButton	m_bRemoveFromCurrentBtn;
	CButton			m_bAddtoCurrentBtn;
	CButton			m_BrushLock;
	CButton			m_Visible;
	CListBox		m_BrushList;
	CColorButton	m_ColorButton;
//	CColorBtn	m_ColorButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGroupsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelectbrushes();
	afx_msg void OnDeselectbrushes();
	virtual void OnCancel();
	afx_msg void OnCreateNewGroup();
	afx_msg void OnAddToCurrent();
	afx_msg void OnSelChangeGroupCombo();
	afx_msg void OnVisible();
	afx_msg void OnRemovefromcurrent();
	afx_msg void OnBrushlock();
	afx_msg void OnSelchangeBrushlist();
	afx_msg void OnRemovegroup();
	afx_msg LONG OnChangeColor(UINT lParam, LONG wParam);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
//	CGEditProDoc			*pDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPSDLG_H__0143E404_A1EB_11D4_9554_444553540000__INCLUDED_)
