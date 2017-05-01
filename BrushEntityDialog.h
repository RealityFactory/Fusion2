/****************************************************************************************/
/*  BrushEntityDialog.h                                                                 */
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

/////////////////////////////////////////////////////////////////////////////

#ifndef BRUSHENTITYDIALOG_H
#define BRUSHENTITYDIALOG_H

#include "basetype.h"
#include "resource.h"

class CFusionDoc;
class CFusionTabControls;
class CPreMadeObject;

// CBrushEntityDialog dialog

class CBrushEntityDialog : public CDialog
{
// Construction
public:
	void Update( CFusionDoc* pDoc );
	CBrushEntityDialog(CFusionTabControls* pParent = NULL, CFusionDoc* pDoc = NULL);   // standard constructor

	// goes through all names in the object library and adds them to the list combo
	void SetupObjectListCombo();
	void AddObjectListName( char *name);

	// enables or disables (depending on state of flag) the place object button
	void EnablePlaceObjectButton( BOOL flag = TRUE );

	geBoolean GetCurrentEntityName (char *pEntityName);
	geBoolean GetCurrentObjectName (char *pObjName);

	void PlaceOmniLight() { OnPlaceOmniLight(); };
	void PlaceSpotLight() { OnPlaceSpotLight(); };
	void PlaceSunLight() { OnPlaceSunLight(); };

// Dialog Data
	//{{AFX_DATA(CBrushEntityDialog)
	enum { IDD = IDD_BRUSH_ENTITY_DIALOG };
	CButton	m_SunLightButton;
	CButton	m_SpotLightButton;
	CButton	m_OmniLightButton;
	CStatic	m_LblBrushes;
	CStatic	m_LblEntities;
	CButton m_CubeButton;
	CButton m_CylinderButton;
	CButton m_ConeButton;
	CButton m_SpheroidButton;
	CButton m_StaircaseButton;
	CButton m_ArchButton;
	CButton m_EntityButton;
	CComboBox m_EntityCombo;
	CButton	m_PlaceObjectButton;
	CComboBox	m_ObjectListCombo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushEntityDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual	BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrushEntityDialog)
	afx_msg void OnCubePrimitive();
	afx_msg void OnCylinderPrimitive();
	afx_msg void OnSpheroidPrimitive();
	afx_msg void OnStaircasePrimitive();
	afx_msg void OnArchPrimitive();
	afx_msg void OnConePrimitive();
	afx_msg void OnPlaceObject();
	afx_msg void OnEntities();
	afx_msg void OnSelchangeEntitycombo();
	afx_msg void OnPlaceOmniLight();
	afx_msg void OnPlaceSpotLight();
	afx_msg void OnPlaceSunLight();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//	Brushes
	
	//	Entitites

	// combo box containing list of library objects

	CFusionTabControls* m_pParentCtrl;
	CFusionDoc* m_pFusionDoc;
	void SetupDialog();
	void DisplayEntityButtonBitmap ();
	void FillEntitiesCombo ();
};

#endif
