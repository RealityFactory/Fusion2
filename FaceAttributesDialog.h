/****************************************************************************************/
/*  FaceAttributesDialog.h                                                              */
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
/*  Modified by Tom Morris for tDesigner3d ver. 0.7, Nov. 2, 2002                       */
/****************************************************************************************/
#ifndef FACEATTRIBUTESDIALOG_H
#define FACEATTRIBUTESDIALOG_H

#include "brush.h"
//#include "resource.h"
#include "flat_combo/XTFlatComboBox.h"


class CGEditProDoc;

class CFaceAttributesDialog : public CDialog
{
public:
	CFaceAttributesDialog(CWnd* pParent = NULL);	//	new g3dc
	void ShowDialog();
	bool UpdateFaceAttributes(CGEditProDoc *pDoc, Face *pFace);
	void SetupDialog();
	void UpdatePolygonFocus (Face *pFace);
	void AssignCurrentToViews();
	void OnResetAll();
	void AssignFaceValues (Face *pFace);

// Dialog Data
	//{{AFX_DATA(CFaceAttributesDialog)
	enum { IDD = IDD_FACEDIALOG };
	CEdit	m_EditTranslucency;
	CEdit	m_EditReflectivity;
	CEdit	m_EditMipMapBias;
	CEdit	m_EditLightIntensity;
	CXTFlatComboBox	m_ComboYLightScale;
	CXTFlatComboBox	m_ComboXLightScale;
	CEdit	m_EditLightYScale;
	CEdit	m_EditLightXScale;
	CEdit	m_EditAngle;
	CEdit	m_EditYScale;
	CEdit	m_EditXScale;
	CEdit	m_EditYOffset;
	CEdit	m_EditXOffset;
	CXTFlatComboBox	m_ComboXOffset;
	CXTFlatComboBox	m_ComboYOffset;
	CXTFlatComboBox	m_ComboXScale;
	CXTFlatComboBox	m_ComboYScale;
	CXTFlatComboBox	m_ComboAngle;
	float m_TextureYScale;
	float m_TextureXScale;
	int m_TextureYOffset;
	int m_TextureXOffset;
	CString m_TextureName;
	float	m_TextureAngle;
	BOOL	m_FullBright;
	BOOL	m_Light;
	BOOL	m_Mirror;
	BOOL	m_Sky;
	float	m_LightXScale;
	float	m_LightYScale;
	int		m_LightIntensity;
	BOOL	m_Flat;
	BOOL	m_Gouraud;
	float	m_MipMapBias;
	BOOL	m_TextureLock;
	float	m_Reflectivity;
	float	m_Translucency;
	BOOL	m_Transparent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFaceAttributesDialog)
	public:
	virtual	BOOL OnInitDialog();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFaceAttributesDialog)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeXOffset();
	afx_msg void OnKillfocusXOffset();
	afx_msg void OnChangeYOffset();
	afx_msg void OnKillfocusYOffset();
	afx_msg void OnChangeXScale();
	afx_msg void OnKillfocusXScale();
	afx_msg void OnChangeYScale();
	afx_msg void OnKillfocusYScale();
	afx_msg void OnChangeAngle();
	afx_msg void OnKillfocusAngle();
	afx_msg void OnFlipvertical();
	afx_msg void OnFliphorizontal();
	afx_msg void OnActivate( UINT, CWnd*, BOOL );
	afx_msg void OnChangeEditxlightscale();
	afx_msg void OnKillfocusEditxlightscale();
	afx_msg void OnChangeEditylightscale();
	afx_msg void OnKillfocusEditylightscale();
	afx_msg void OnFacemirror();
	afx_msg void OnFacesky();
	afx_msg void OnFacefullbright();
	afx_msg void OnKillfocusFacelightintensity();
	afx_msg void OnFacegouraud();
	afx_msg void OnFaceflat();
	afx_msg void OnKillfocusMipmapbias();
	afx_msg void OnTexturelock();
	afx_msg void OnKillfocusReflectivity();
	afx_msg void OnKillfocusFacetranslucency();
	afx_msg void OnTransparent();
	afx_msg void OnFaceApplybtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void OnOK ();
private:
	CGEditProDoc		*m_pDoc;

	int		m_iOldTexYOffset,
			m_iOldTexXOffset;

	Face	*pFace;
	void	OnFacelight( void );
	float	DoIncrement (float v, float incr, float vMin, float vMax, int nSBCode);
	float	GetIncrement (CComboBox &combo);
	void	SetShadingChecks ();
};

#endif
