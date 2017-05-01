/****************************************************************************************/
/*  TemplatesDlg.h																		*/
/*                                                                                      */
/*  Author:       Tom Morris															*/
/*  Description:  Template selection													*/
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


#if !defined(AFX_TEMPLATESDLG_H__32E3A4C2_A4E6_11D4_9554_BC66DB8A5520__INCLUDED_)
#define AFX_TEMPLATESDLG_H__32E3A4C2_A4E6_11D4_9554_BC66DB8A5520__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplatesDlg.h : header file
//
#include "SkyDialog.h"	//	post 0.5 release
#include "flat_combo/XTFlatComboBox.h"


//#include "GEditProDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CTemplatesDlg dialog

class CTemplatesDlg : public CDialog
{
// Construction
public:
	CTemplatesDlg(CWnd* pParent = NULL);   // standard constructor

	int					m_iCurrentPrefab, m_iCurrentEntity;
	CString				m_sCurrentTemplateName;
	CSkyDialog			m_dSky;	//	post 0.5 release

	bool				InitializeEntitiesCombo(CGEditProDoc* pDoc);
	bool				InitializePrefabList(CGEditProDoc* pDoc);
	bool				UpdateTemplates( CGEditProDoc* pDoc );
	geBoolean			GetCurrentTemplateName (char *pTemplateName);
	geBoolean			GetCurrentEntityName(char *pEntityName); 
//	CString				m_currentTemplate;

	void	SetupPrefabList();

// Dialog Data
	//{{AFX_DATA(CTemplatesDlg)
	enum { IDD = IDD_TEMPLATES_DLG };
	CButton	m_templateApplyBtn;
	CButton	m_skyBtn;
	CButton	m_entitiesButton;
	CXTFlatComboBox	m_entitiesCombo;
	CStatic	m_currentPrefabName;
	CListBox	m_prefabList;
	CButton	m_sphere_template_btn;
	CButton	m_cube_template_btn;
	CButton m_cylinder_template_btn;
	CButton m_stairs_template_btn;
	CButton m_cone_template_btn;
	CButton m_arch_template_btn;
	CButton	m_prefabButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplatesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	void	UnCheckAllTemplateButtons();

	// Generated message map functions
	//{{AFX_MSG(CTemplatesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSphere();
	afx_msg void OnButtonCube();
	afx_msg void OnButtonCylinder();
	afx_msg void OnButtonStairs();
	afx_msg void OnButtonCone();
	afx_msg void OnButtonArch();
	afx_msg void OnSelchangePrefabList();
	afx_msg void OnPrefabBtn();
	afx_msg void OnEntityBtn();
	afx_msg void OnSelchangeEntitiesCombo();
	afx_msg void OnSkybutton();
	afx_msg void OnDblclkPrefabList();
	afx_msg void OnTemplateApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
		CGEditProDoc		*m_pDoc;

		bool			m_bEntitiesInitialized;
		bool			m_bPrefabsInitialized;
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnCbnCloseupEntitiesCombo();
	CString m_strCurrentItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATESDLG_H__32E3A4C2_A4E6_11D4_9554_BC66DB8A5520__INCLUDED_)
