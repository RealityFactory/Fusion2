/****************************************************************************************/
/*  ModelsDlg.h																			*/
/*                                                                                      */
/*  Author:       Tom Morris															*/
/*  Description:  Model Control															*/
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


#if !defined(AFX_MODELSDLG_H__0143E405_A1EB_11D4_9554_444553540000__INCLUDED_)
#define AFX_MODELSDLG_H__0143E405_A1EB_11D4_9554_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelsDlg.h : header file
//
#include "flat_combo/XTFlatComboBox.h"



/////////////////////////////////////////////////////////////////////////////
// CModelsDlg dialog

class CModelsDlg : public CDialog
{
// Construction
public:
	CModelsDlg(CWnd* pParent = NULL);   // standard constructor

	bool UpdateModels (CGEditProDoc *pDoc, ModelInfo_Type *aModelInfo);
	void UpdateSelectedModel (int MoveRotate, geVec3d const *pVecDelta);
	void GetTranslation (geVec3d *pVec);


// Dialog Data
	//{{AFX_DATA(CModelsDlg)
	enum { IDD = IDD_MODELKEY };
	CButton	m_SetModelOrigin;
	CButton	m_CloneModel;
	CButton	m_cbLockOrigin;
	CButton	m_Stop;
	CButton	m_rrStart;
	CButton	m_rrFrame;
	CButton	m_Play;
	CButton	m_ffFrame;
	CButton	m_ffEnd;
	CButton	m_Select;
	CButton	m_RemoveBrushes;
	CButton	m_EditModel;
	CButton	m_EditKey;
	CButton	m_EditEvent;
	CButton	m_Deselect;
	CButton	m_DeleteModel;
	CButton	m_DeleteKey;
	CButton	m_DeleteEvent;
	CButton	m_AddModel;
	CButton	m_AddEvent;
	CButton	m_AddBrushes;
	CButton	m_cbLocked;
	CButton	m_AnimateButton;
	CListBox	m_KeysList;
	CXTFlatComboBox	m_ModelCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddmodel();
	afx_msg void OnDeletemodel();
	afx_msg void OnAddbrushes();
	afx_msg void OnRemovebrushes();
	afx_msg void OnSelectBrushes();
	afx_msg void OnDeselectBrushes();
	afx_msg void OnSelendokModelcombo();
	virtual void OnOK();

	afx_msg void OnAnimate();
	afx_msg void OnDeletekey();
	afx_msg void OnSelchangeKeyslist();
	afx_msg void OnLocked();
	afx_msg void OnClonemodel();
	afx_msg void OnSetmodelorigin();
	afx_msg void OnLockorigin();
	afx_msg void OnAddevent();
	afx_msg void OnDeleteevent();
	afx_msg void OnEditevent();
	afx_msg void OnEditkey();
	afx_msg void OnEditmodel();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CGEditProDoc			*	pDoc;
	ModelInfo_Type		*	pModelInfo;
	geXForm3d				XfmDelta;



	void PrivateUpdate (void);
	void UpdateModelsList ();
	void UpdateKeysList (Model *pModel);
	void AddKey (void);
	int GetCurrentLbKey (geFloat *pTime, int *pKeyType);
	void AddTheKey (Model *pModel, geFloat Time, geXForm3d const *pXfm);
	void ReverseDeltas (Model *pModel);
	Model *GetCurrentModel (void);
	void SetListboxKeySelection (geFloat KeyTime);
	void EnableControls (BOOL Enable, Model const *pModel);
	geBoolean SelectedBrushesInOtherModels (int ModelId);
	int GetUniqueModelName (CString const &Caption, CString &ModelName);
	geBoolean StartSettingOrigin (Model *pModel);
	void StopSettingOrigin (Model *pModel);

	geBoolean Animating;
	geBoolean SettingOrigin;
	int EntityIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELSDLG_H__0143E405_A1EB_11D4_9554_444553540000__INCLUDED_)
