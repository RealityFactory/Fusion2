/****************************************************************************************/
/*  MainFrm.h                                                                           */
/*                                                                                      */
/*  Author:       Tom Morris						                                    */
/*  Description:  GEditPro header file		                                    */
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

#ifndef	MAINFRM_H
#define MAINFRM_H

#pragma once

#include "Globals.h"
#include "Entity.h"


#include "brush.h"
#include "TemplatesDlg.h"
#include "TextureDlg.h"
#include "GroupsDlg.h"
#include "ModelsDlg.h"
#include "EntitiesDialog.h"
#include "BrushAttributesDialog.h"
#include "FaceAttributesDialog.h"
#include "CreateArchDialog.h"
#include "CreateBoxDialog.h"
#include "CreateConeDialog.h"
#include "CreateCylDialog.h"
#include "CreateSpheroidDialog.h"
#include "CreateStaircaseDialog.h"
#include "QuickViewDlg.h"
#include "ViewMgrDlg.h"
#include "DialogPrintView.h"
#include "flat_combo\XTFlatComboBox.h"


#define MAX_GRID_STRING	16
#define MAX_SNAP_STRING 16


// These are the tabs which contain one or more dialogs based on context
typedef enum
{
	MAINFRM_COMMANDPANEL_TEMPLATES,
	MAINFRM_COMMANDPANEL_GROUPS,
	MAINFRM_COMMANDPANEL_TEXTURES,
	MAINFRM_COMMANDPANEL_MODELS,
	MAINFRM_COMMANDPANEL_LAST
} MAINFRM_COMMANDPANEL_TAB ;


// These are the possible dialogs for TEMPLATE
typedef enum
{
} MAINFRM_TEMPLATES ;

// These are the possible dialogs for LISTS
typedef enum
{
	MAINFRM_LISTS_TYPE,	
	MAINFRM_LISTS_GROUPS,
	MAINFRM_LISTS_MODELS,
	MAINFRM_LISTS_LAST
} MAINFRM_LISTS ;

// These are all the modeless dialogs making up the command panel
typedef enum
{
	MAINFRM_PANEL_TEMPLATES,
	MAINFRM_PANEL_GROUPS,
	MAINFRM_PANEL_TEXTURES,
	MAINFRM_PANEL_MODELS,
	MAINFRM_PANEL_OBJECTS,
	MAINFRM_PANEL_LISTS,
	MAINFRM_PANEL_LAST
} MAINFRM_PANEL ;


						//	setup a little style bar class so we can put dialog boxes
						//	on some of our toolbars.
class CStyleBar : public CToolBar
{
public:
	CXTFlatComboBox		m_comboBox;
	CFont				m_font;
};


class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

	char				szGridString[MAX_GRID_STRING] ;
	char				szSnapString[MAX_SNAP_STRING] ;

public:
	CMainFrame();

	CGEditProDoc			*m_pDoc;

	CQuickViewDlg		previewDlg;

	BOOL				IsDestroyingApp;
	int					IsStartingApp;

						//	Clipboard registration format numbers...
	UINT				m_CB_tEdit_BRUSH_FORMAT;
	UINT				m_CB_tEdit_ENTITY_FORMAT;

						//	Command Panel stuff
	MAINFRM_COMMANDPANEL_TAB	m_eCurrentTab ;
	MAINFRM_COMMANDPANEL_TAB	m_ePreviousTab ;

	CWnd				*GetTabControlWindow(int nTabIndex );
	CTemplatesDlg		m_TemplatesTab;
	CModelsDlg			m_ModelsTab;
	CGroupsDlg			m_GroupsTab;
	void				SetCommandPanelTab( MAINFRM_COMMANDPANEL_TAB nTab );
	void				HideAllTabsBut(int nTab);
	void				UpdateMainControls();
	CTabCtrl			*m_CommandTabControl;
	CDialogBar			m_CommandDialogBar;

						//	texture-related stuff
	CTextureDlg			m_TexturesTab;
	const char			*GetCurrentTexture();
	void				UpdateTextures ();

						//	toolbar-related stuff
	CToolBar			m_wndBrushToolBar;
	CStyleBar			m_wndGroupBar ;
	BOOL				CreateBrushToolBar();
	void				DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf) ;
	BOOL				CreateGroupBar() ;
	BOOL				CreatePropertiesPanel();

						//	Properties Panel stuff
	CEntitiesDialog		*m_dPropEntitiesDialog;
	CBrushAttributesDialog	*m_dPropBrushAttributesDialog;
	CFaceAttributesDialog	*m_dPropFaceAttributesDialog;
	CDialogBar			m_PropertiesPanel;

	int					m_pCurrentEntity;
	int					m_pPreviousEntity;
	Brush				*m_pCurrentBrush;
	Brush				*m_pPreviousBrush;
	Face				*m_pCurrentFace;
	Face				*m_pPreviousFace;
	bool				PutDialogOnPropertiesPanel(CWnd *pParent);
	bool				HideAllPropDialogsBut(CWnd *pWnd);
	
	CCreateArchDialog		m_createArchDialog;
	CCreateBoxDialog		m_createBoxDialog;
	CCreateConeDialog		m_createConeDialog;
	CCreateCylDialog		m_createCylDialog;
	CCreateSpheroidDialog	m_createSpheroidDialog;
	CCreateStaircaseDialog	m_createStaircaseDialog;

						//	Console Panel stuff
	BOOL				CreateConsoleDialogBar();
	CDialogBar			*m_ConsoleDialogBar;
	CEdit				*m_consoleEdit;
	CButton				*m_consoleCopyButton;

	void				ShowConsoleBar();
	char				*TranslateString(char *buf);
	void				ConClear();
	void				ConPrintf(char *text, ...);
	void				ConError(char *text, ...);
	void				OnMaxtextConedit();


						//	other schtuff
	void				UpdateGridSize(geFloat GridSize, int SnapOn, int snapto, 
							int gunits, int snapunits);
	CFrameWnd			* CreateNewGameViewFrame(CRuntimeClass* pViewClass,  
							CDocTemplate* pTemplate, CDocument* pDoc, CFrameWnd* pOther);
	void				MakeNewView( CRuntimeClass* pViewRuntimeClass);
	CGEditProDoc			*GetCurrentDoc (void);
	void				UpdateModelsDialog (void);
	void				UpdateSelectedModel (int MoveRotate, geVec3d const *pVecDelta);

	BOOL				CreateProgressBar(	CProgressCtrl* pProgressCtrl,
							CStatusBar* pStatusBar,
							LPCTSTR szMessage,
							int nPaneIndex,
							int cxMargin,
							int cxMaxWidth,
							UINT nIDControl);

	CStatusBar			m_wndStatusBar;

	CViewMgrDlg			*GetViewMgrDialog();
	bool				ResetViewMgrDialog();

	bool	SetPrintPreviewState(bool showing);


private:

	CViewMgrDlg		m_dlgViewMgr;
	bool	CreateViewMgrDialog();

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
	virtual CDocument	* GetActiveDocument();
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void		AssertValid() const;
	virtual void		Dump(CDumpContext& dc) const;
#endif

protected:				//	control bar embedded members
	CToolBar			m_wndToolBar;
	CToolBar			m_wndModeToolBar;
	CStyleBar			m_wndConsoleBar;
	
	BOOL				CreateMainToolBar();
	BOOL				CreateTemplateToolBar();
	BOOL				CreateModeToolBar();
	BOOL				SetControlBarFont();

						//	Command Panel stuff
	bool				InitCommandDialogBar();
	CDialog				*DialogFromIndex(MAINFRM_COMMANDPANEL_TAB nTab);
	CDialogBar			m_wndTabBar;
	BOOL				OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult); 

						//	status bar stuff
	BOOL				CreateStatusBar();
	CStatic				m_sXPos;
	CStatic				m_sYPos;
	CStatic				m_sZPos;
	CStatic				m_sXSize;
	CStatic				m_sYSize;
	CStatic				m_sZSize;


	CDialogPrintView	printViewDlg;


// Generated message map functions
protected:
	afx_msg void OnInitMenu(CMenu* pMenu);

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowNew();
	afx_msg void OnUpdateSLock(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSelInfo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateWorldPos(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCursorInfo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridInfo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSnapInfo(CCmdUI *pCmdUI);
	afx_msg void OnBrushGroupsMakenewgroup();
	afx_msg void OnModebar();
	afx_msg void OnUpdateModebar(CCmdUI* pCmdUI);
	afx_msg void OnGroupbar();
	afx_msg void OnUpdateGroupbar(CCmdUI* pCmdUI);
	afx_msg void OnCommandPanel();
	afx_msg void OnUpdateCommandPanel(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnViewLeakFinder();
	afx_msg void OnUpdateViewLeakFinder(CCmdUI* pCmdUI);
	afx_msg void OnSelchangeGroupList ();
	afx_msg void OnPropertiesPanel();
	afx_msg void OnUpdatePropertiesPanel(CCmdUI* pCmdUI);
	afx_msg void OnConsole();
	afx_msg void OnConsoleCopyBtn();
	afx_msg void OnUpdateConsole(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnCloseConsolebtn();
	afx_msg void OnPreview();
	afx_msg void OnUpdatePreview(CCmdUI* pCmdUI);
	afx_msg void OnConsoleQuickviewBtn();
	afx_msg void OnViewViewmgr();
	afx_msg void OnUpdateViewViewmgr(CCmdUI* pCmdUI);
	afx_msg void OnFilePrinttEditView();
	afx_msg void OnFilePrintPreviewtEditView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
public:
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

#endif
