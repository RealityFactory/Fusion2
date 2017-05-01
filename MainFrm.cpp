/****************************************************************************************/
/*  MainFrm.cpp                                                                         */
/*                                                                                      */
/*  Author:       Tom Morris									                        */
/*	Description:  MFC frame stuff with a bit of other misc ui stuff                     */
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
/*	Genesis3D Version 1.1 released November 15, 1999                            */
/*  Copyright (C) 1999 Eclipse, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/

#include "stdafx.h"
#include "Globals.h"
#include "GEditProView.h"
#include "units.h"
#include <assert.h>
#include "GEditPro.h"		
#include "ChildFrm.h"
#include "TextureDlg.h"
#include "EntityTable.h"
#include "TemplatesDlg.h"
#include "FilePath.h"
#include "Splash.h"
#include "mainfrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAX_TAB_NAME_LENGTH (32)
#define	TOOLBAR_HEIGHT 33
#define TOOLBAR_HEIGHT_NOLABELS 28
#define	TEXT_HEIGHT	16				//	for toolbar button labels
#define TEXT_HEIGHT_NOLABELS 16


const static UINT tabNames[] =
{
	IDS_TAB_TEMPLATES,
	IDS_TAB_GROUP,
	IDS_TAB_TEXTURES,
	IDS_TAB_MODELS 
} ;


const CString strBarState = _T("MainFrameBarState");



static UINT indicators[] =
{
	ID_SEPARATOR,				// Text Area. Also holds progress control
	ID_INDICATOR_SLOCK,			// Lock pane
	ID_INDICATOR_SELINFO,		// Selection Info pane
	ID_INDICATOR_WORLDPOS,		// World Coordinates of cursor
	ID_INDICATOR_CURSORINFO,	// Cursor Info (What would select if you click)
	ID_INDICATOR_GRID,			// Grid Info 
	ID_INDICATOR_SNAP			// Snap Info
};



typedef enum eINDICATORS
{
	ID_SEPARATOR_PANE = 1,
	ID_SLOCK_PANE,
	ID_SELINFO_PANE,
	ID_WORLDPOS_PANE,
	ID_CURSORINFO_PANE,
	ID_GRIDINFO_PANE,
	ID_SNAPINFO_PANE
} ;




/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_INITMENU()
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND(ID_WINDOW_NEW, OnWindowNew)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SLOCK, OnUpdateSLock)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SELINFO, OnUpdateSelInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_WORLDPOS, OnUpdateWorldPos)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CURSORINFO, OnUpdateCursorInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GRID, OnUpdateGridInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SNAP, OnUpdateSnapInfo)
	ON_COMMAND(ID_GROUPS_MAKENEWGROUP, OnBrushGroupsMakenewgroup)
	ON_COMMAND(IDM_MODEBAR, OnModebar)
	ON_UPDATE_COMMAND_UI(IDM_MODEBAR, OnUpdateModebar)
	ON_COMMAND(IDM_GROUPBAR, OnGroupbar)
	ON_UPDATE_COMMAND_UI(IDM_GROUPBAR, OnUpdateGroupbar)
	ON_COMMAND(IDM_COMMAND_PANEL, OnCommandPanel)
	ON_UPDATE_COMMAND_UI(IDM_COMMAND_PANEL, OnUpdateCommandPanel)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEWLEAKFINDER, OnViewLeakFinder)
	ON_UPDATE_COMMAND_UI(ID_VIEWLEAKFINDER, OnUpdateViewLeakFinder)
	ON_CBN_SELCHANGE(IDC_GROUPBAR_COMBO, OnSelchangeGroupList)
	ON_COMMAND(IDM_PROPERTIES_PANEL, OnPropertiesPanel)
	ON_UPDATE_COMMAND_UI(IDM_PROPERTIES_PANEL, OnUpdatePropertiesPanel)
	ON_COMMAND(IDM_CONSOLE, OnConsole)
	ON_BN_CLICKED(IDC_CONSOLE_COPY_BTN, OnConsoleCopyBtn)
	ON_UPDATE_COMMAND_UI(IDM_CONSOLE, OnUpdateConsole)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_CLOSE_CONSOLEBTN, OnCloseConsolebtn)
	ON_COMMAND(IDM_PREVIEW, OnPreview)
	ON_UPDATE_COMMAND_UI(IDM_PREVIEW, OnUpdatePreview)
	ON_BN_CLICKED(IDC_CONSOLE_QUICKVIEW_BTN, OnConsoleQuickviewBtn)
	ON_COMMAND(ID_VIEW_VIEWMGR, OnViewViewmgr)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VIEWMGR, OnUpdateViewViewmgr)
	ON_COMMAND(ID_FILE_PRINT_TEDIT_VIEW, OnFilePrinttEditView)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW_T_EDIT_VIEW, OnFilePrintPreviewtEditView)
	//}}AFX_MSG_MAP
	
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame(void) : IsStartingApp(1), IsDestroyingApp(0)
{
			m_pDoc = NULL;
			m_dPropBrushAttributesDialog = NULL;
			m_dPropFaceAttributesDialog = NULL;
			m_dPropEntitiesDialog = NULL;
			m_pCurrentBrush = NULL;
			m_pPreviousBrush = NULL;
			m_pCurrentFace = NULL;
			m_pPreviousFace = NULL;
			m_CommandTabControl = NULL;
			m_ConsoleDialogBar = NULL;
			m_consoleEdit = NULL;
			m_consoleCopyButton = NULL;
}



CMainFrame::~CMainFrame()
{

}


//==========================================================================================
// CMainFrame::OnCreate
// Called by: 
// Calls: 
// Parameters:LPCREATESTRUCT lpCreateStruct
// Returns:	int
// Purpose: Main window builder. instructs for the construction of toolbars
//==========================================================================================
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	//	Set the titlebar title
	CFrameWnd::SetTitle("RFEditPro");

	//	enable docking toolbars in the main frame client area	
	EnableDocking(CBRS_ALIGN_ANY | CBRS_FLOAT_MULTI);	

	if (!SetControlBarFont())
		{TRACE0("Failed to create ControlBarFont\n");return -1;}
	
	if (!CreateMainToolBar())
		{TRACE0("Failed to create Main toolbar\n");return -1;}

	if( !CreateBrushToolBar() )
		{TRACE0("Failed to create brush toolbar\n");return -1;}

	if( !CreateGroupBar() )
		{TRACE0("Failed to create group toolbar\n");return -1;}

	if( !CreateViewMgrDialog())
		{TRACE0("Failed to create group toolbar\n");return -1;}
	
	if (!InitCommandDialogBar())	
		{TRACE0("Failed to InitCommandDialogBar() \n");return -1;}

	if	(!CreatePropertiesPanel())
		{TRACE0("Failed to CreatePropertiesPanel() \n");return -1;}

	if (!CreateConsoleDialogBar())
		{TRACE0("Failed to CreateConsoleDialogBar() \n");return -1;}

	if (!CreateStatusBar())
		{TRACE0("Failed to create status bar\n");return -1;}
	DockControlBarLeftOf(&m_wndToolBar, &m_wndBrushToolBar);	//	post 0.57
	DockControlBarLeftOf(&m_wndBrushToolBar, &m_wndGroupBar);

//	LoadBarState( "DESKTOP" ) ;		//THIS DON'T WORK

	//	zero out pointers for these dynamic dialog boxes
	m_dPropEntitiesDialog = NULL;
	m_pCurrentEntity = -1;
	m_pPreviousEntity = -1;

	m_dPropBrushAttributesDialog = NULL;
	m_pCurrentBrush = NULL;
	m_pPreviousBrush = NULL;

	m_dPropFaceAttributesDialog = NULL;
	m_pCurrentFace = NULL;
	m_pPreviousFace = NULL;

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);

	//	set time interval for autosave feature -- not implemented
	SetTimer(ID_MAINFRAME_TIMER, 100, NULL); 

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
//	OnTimer
//	
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	//	Enumerate through the child windows
	//	If the splitter window is being resized, then reset the ViewMgr buttons
	if (nIDEvent == ID_MAINFRAME_TIMER)
	{
		CChildFrame *pChildFrame = NULL;
		
		pChildFrame = (CChildFrame*)this->MDIGetActive();

		static bool	bTracking;
		if (pChildFrame)
		{
			ASSERT( pChildFrame->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) );
			if (pChildFrame->GetSplitterWnd()->IsTracking())
			{
				bTracking = true;
			}
		}
		//	if tracking is FINISHED
		if (bTracking && !pChildFrame->GetSplitterWnd()->GetCapture())
		{
			ResetViewMgrDialog();
			bTracking = false;
		}

		//	Enable or Disable the buttons on the ViewMgr Dialog
		//	depending if a Doc is open or not
		static	BOOL	bDocIsOpen;
		BOOL	bTemp;
		CGEditProDoc *pDoc = NULL;
		pDoc = CGlobals::GetActiveDocument();
		if (pDoc)
			bTemp = TRUE;
		else
			bTemp = FALSE;
		if (bTemp != bDocIsOpen)
		{
			CViewMgrDlg	*pViewMgrDlg = NULL;
			pViewMgrDlg = GetViewMgrDialog();
			if (pViewMgrDlg)
			{
				pViewMgrDlg->EnableButtons(bTemp);
				bDocIsOpen = bTemp;
			}
		}
	}
	
	CMDIFrameWnd::OnTimer(nIDEvent);
}


bool	CMainFrame::ResetViewMgrDialog()
{
	//	reset the ViewMgr Dialog
		CViewMgrDlg	*pViewMgrDlg = NULL;
		pViewMgrDlg = GetViewMgrDialog();
		if (pViewMgrDlg)
			pViewMgrDlg->ResetButtons();
	return true;
}




//==========================================================================================
// CMainFrame::
// Called by: 
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics



//==========================================================================================
// CMainFrame::
// Called by: 
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}



//==========================================================================================
// CMainFrame::
// Called by: 
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


//	***********************************************************************************************
//	BEGIN TOOL BAR CODE

//=====================================================================================================================
// CMainFrame::SetControlBarFont
// Called by: OnCreate()
// Calls: 
// Parameters: void
// Returns:	BOOL
// Purpose: creates a font. makes it available for controlbars and status bar
//			Sets it as a static global variable
//=====================================================================================================================
BOOL CMainFrame::SetControlBarFont()
{
							//  Prepare LOGFONT structure
	CClientDC dc( this );	//	need a DC for GetDeviceCaps() below

	LOGFONT lf;				// logical font structure
	::ZeroMemory( &lf, sizeof( lf ) );

							//	designate desired font by name
	TCHAR myfont[LF_FACESIZE] = "Tahoma"; 

							//	8 point Tahoma
	lf.lfHeight = - MulDiv( 8, dc.GetDeviceCaps( LOGPIXELSX ), 72 );
	lf.lfWidth			= 0;
	lf.lfEscapement		= 0;
	lf.lfOrientation	= 0;
	lf.lfWeight			= FW_NORMAL;
	lf.lfItalic			= FALSE;
	lf.lfUnderline		= FALSE;
	lf.lfStrikeOut		= FALSE;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfOutPrecision	= OUT_TT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfQuality		= DEFAULT_QUALITY;
	lf.lfPitchAndFamily	= VARIABLE_PITCH | TMPF_TRUETYPE;
	strcpy(lf.lfFaceName, myfont);

					//	Create font and assign it to our CFont static Global var
	if( !CGlobals::m_ControlBarFont.CreateFontIndirect( &lf ) )
	{
		return FALSE;
	}
	return TRUE;
}


//=====================================================================================================================
// CMainFrame::CreateMainToolBar
// Called by: OnCreate()
// Calls: 
// Parameters: void
// Returns:	BOOL
// Purpose: 
//=====================================================================================================================
BOOL CMainFrame::CreateMainToolBar()
{
	char		szBuffer[MAX_TAB_NAME_LENGTH] ;
	HINSTANCE	hResources ;

	hResources = AfxGetResourceHandle() ;

	//	Create the main toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;     
	}
	//	set the font using global font var
	m_wndToolBar.SetFont(&CGlobals::m_ControlBarFont);

	// Assign a title to bar
	::LoadString( hResources, IDS_TOOLBAR_GENERAL, szBuffer, sizeof szBuffer ) ;
	m_wndToolBar.SetWindowText( szBuffer ) ;

	//	make it dockabale
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return TRUE;
}


//==========================================================================================
// CMainFrame::CreateBrushToolBar
// Called by: OnCreate()
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
BOOL CMainFrame::CreateBrushToolBar()
{
	char		szBuffer[MAX_TAB_NAME_LENGTH] ;
	HINSTANCE	hResources = NULL;

	hResources = AfxGetResourceHandle() ;

	if (!m_wndBrushToolBar.CreateEx(this,TBSTYLE_FLAT, WS_CHILD|WS_VISIBLE|CBRS_TOP|
		CBRS_GRIPPER | CBRS_TOOLTIPS|CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndBrushToolBar.LoadToolBar(IDR_BRUSHTOOLS) )
	{
		TRACE0("Failed to create stylebar\n");
		return FALSE;       // fail to create
	}
	//	set the font using global font var
	m_wndBrushToolBar.SetFont(&CGlobals::m_ControlBarFont);

	// Assign a title to bar
	::LoadString( hResources, IDS_TOOLBAR_BRUSH, szBuffer, sizeof szBuffer ) ;
	m_wndBrushToolBar.SetWindowText( szBuffer ) ;

	//	make it dockabale
	m_wndBrushToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndBrushToolBar);	

	return TRUE;
}


//==========================================================================================
// CMainFrame::DockControlBarLeftOf
// Called by: OnCreate()
// Calls: 
// Parameters:
// Returns:	
// Purpose: helps organize the initial layout of the toolbars
//==========================================================================================
void CMainFrame::DockControlBarLeftOf(CToolBar* LeftOf,CToolBar* Bar)
{
	CRect rect;
	DWORD dw;
	UINT n;

	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout();
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line.  By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}




///////////////////////////////////////////////////////////////////////////////
//	CreateViewToolBar
//	
///////////////////////////////////////////////////////////////////////////////
bool CMainFrame::CreateViewMgrDialog()
{
	m_dlgViewMgr.Create(IDD_VIEWMGR_DLG, this);
	m_dlgViewMgr.ShowWindow(SW_SHOW);

	return true;

}



void CMainFrame::OnViewViewmgr() 
{
	if (!m_dlgViewMgr.IsWindowVisible())
	{
		m_dlgViewMgr.ShowWindow(SW_RESTORE);
	}
	else
	{
		m_dlgViewMgr.ShowWindow(SW_HIDE);
	}
}



void CMainFrame::OnUpdateViewViewmgr(CCmdUI* pCmdUI) 
{
	if (m_dlgViewMgr.IsWindowVisible())
	{
		pCmdUI->SetCheck(1);
	}

	else
	{
		pCmdUI->SetCheck(0);
	}
	
}


CViewMgrDlg		*CMainFrame::GetViewMgrDialog()
{
	return &m_dlgViewMgr;
}



//	BEGIN BRUSHGROUP CODE

//==========================================================================================
// CMainFrame::CreateGroupBar
// Called by: OnCreate()
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
BOOL CMainFrame::CreateGroupBar()
{
	char		szBuffer[MAX_TAB_NAME_LENGTH] ;
	HINSTANCE	hResources = NULL;
	CRect		rect;						//	for combobox

	hResources = AfxGetResourceHandle() ;

	if (!m_wndGroupBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD|WS_VISIBLE|CBRS_TOP|
		CBRS_GRIPPER | CBRS_TOOLTIPS|CBRS_FLYBY| CBRS_SIZE_DYNAMIC) ||
		!m_wndGroupBar.LoadToolBar(IDR_GROUPBAR))
	{
		TRACE0("Failed to create groupbar\n");
		return FALSE;       // fail to create
	}
	//	set the font using global font var
	m_wndGroupBar.SetFont(&CGlobals::m_ControlBarFont);

	// Assign a title to bar
	::LoadString( hResources, IDS_TOOLBAR_GROUP, szBuffer, sizeof szBuffer ) ;
	m_wndGroupBar.SetWindowText( szBuffer ) ;
	m_wndGroupBar.SetButtonInfo(7, IDC_GROUPBAR_COMBO, TBBS_SEPARATOR, 110);

	//	Set the dimensions for the length of the dropdown menu
	m_wndGroupBar.GetItemRect(7, &rect);
	rect.top = 3;
	rect.bottom = rect.top + 200;

	//	at position 6 (IDC_GROUPBAR_COMBO).

	if (!m_wndGroupBar.m_comboBox.Create( WS_CHILD |
		CBS_DROPDOWNLIST|CBS_SORT|WS_VISIBLE|WS_TABSTOP,
		rect, &m_wndGroupBar, IDC_GROUPBAR_COMBO))
	{
		TRACE0("Failed to create combo-box\n");
		return FALSE;
	}

	m_wndGroupBar.m_comboBox.SetFont(&CGlobals::m_ControlBarFont);

	//	make it dockabale	
	m_wndGroupBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndGroupBar);

	return TRUE;
}


//==========================================================================================
// CMainFrame::OnGroupbar
// Called by: Menu
// Calls: 
// Parameters:
// Returns:	
// Purpose: shows or hides group bar
//==========================================================================================
void CMainFrame::OnGroupbar() 
{
	CControlBar* pBar = NULL;
	pBar = (CControlBar*)&m_wndGroupBar ;

	if( pBar )
	{
		ShowControlBar( pBar, (pBar->GetStyle() & WS_VISIBLE) == 0, FALSE ) ;
	}
}


//==========================================================================================
// CMainFrame::OnSelchangeGroupList
// Called by: MFC Messaging sys
// Calls: 
// Parameters:
// Returns:	
// Purpose: Set current group to the one selected from the groupbar combo box
//==========================================================================================
void CMainFrame::OnSelchangeGroupList ()
{
	int CurSel;

	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		CurSel = m_wndGroupBar.m_comboBox.GetCurSel ();
		if (CurSel != LB_ERR)
		{
			m_GroupsTab.SelectNone();	// deslect all from current group
			pDoc->m_iCurrentGroup = m_wndGroupBar.m_comboBox.GetItemData (CurSel);
			UpdateMainControls();
			m_GroupsTab.SelectNone();	// deslect all from group we just switched to.	

		}
		pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
	}
}


//==========================================================================================
// CMainFrame::OnUpdateGroupbar
// Called by: Menu messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: places or removes check mark next to menu item
//==========================================================================================
void CMainFrame::OnUpdateGroupbar(CCmdUI* pCmdUI) 
{
	CControlBar* pBar = NULL;
	pBar = (CControlBar*)&m_wndGroupBar ;
	if (pBar)
    {
		pCmdUI->SetCheck((pBar->GetStyle() & WS_VISIBLE) != 0);
	}
}


//==========================================================================================
// CMainFrame::OnBrushGroupsMakenewgroup
// Called by: Menu Messaging (why is it HERE?)
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::OnBrushGroupsMakenewgroup() 
{
	/*
	  Things to do here:
	    1) Get the current document
		2) Create a new group in that document's list
		3) Update the combo box with the group info
		4) Set the document's current group to the newly created group
	*/

	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		if (pDoc->MakeNewBrushGroup ( this ))
		{
			UpdateMainControls();		
		}
	}

}/* CMainFrame::OnBrushGroupsMakenewgroup */

//	END BRUSHGROUP CODE



//	END TOOLBAR CODE
//	****************************************************************************************
//	BEGIN PROPERTIES PANEL CODE



//=====================================================================================================================
// CMainFrame::CreatePropertiesDailogPanel
// Called by: OnCreate()
// Calls: 
// Parameters: void
// Returns:	BOOL
// Purpose: creates a CDialogBar, then places a CDialog onto that bar for handling
//			Properties controls				
//=====================================================================================================================
BOOL CMainFrame::CreatePropertiesPanel()
{
	//	create the Properties dialog. This will hold
	//	the properties controls for objects.
	if (!m_PropertiesPanel.Create(this, IDD_PROPERTIES_PANEL, CBRS_GRIPPER |
		CBRS_RIGHT|CBRS_TOOLTIPS|CBRS_FLYBY | CBRS_SIZE_DYNAMIC, IDD_PROPERTIES_PANEL))
	{
		TRACE0("Failed to create DlgBar\n");
		return FALSE;	
	}
	//	make it dockabale
	m_PropertiesPanel.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT );
	DockControlBar(&m_PropertiesPanel);

	return TRUE;
}


//=====================================================================================================================
// CMainFrame::OnPropertiesPanel
// Called by: Menu
// Calls: 
// Parameters: 
// Returns:	void
// Purpose: Menu command to show the panel if it isn't visible
//=====================================================================================================================
void CMainFrame::OnPropertiesPanel() 
{
		CDialogBar* pBar = NULL;
		pBar = (CDialogBar*)&m_PropertiesPanel ;

	if( pBar )
	{
		ShowControlBar( pBar, (pBar->GetStyle() & WS_VISIBLE) == 0, FALSE ) ;
	}
}


//=====================================================================================================================
// CMainFrame::HideAllPropDialogsBut
// Called by: the Create template dialogs, Entity, brush and Face dialogs, and Update(here)
// Calls: 
// Parameters: 
// Returns:	bool
// Purpose: prepares the property panel for showing the appropriate dialog
//=====================================================================================================================
bool CMainFrame::HideAllPropDialogsBut(CWnd *pWnd)
{
	if ((m_dPropBrushAttributesDialog) && (m_dPropBrushAttributesDialog != pWnd))
	{
		m_dPropBrushAttributesDialog->DestroyWindow();
		delete m_dPropBrushAttributesDialog;		//	post 0.57
		m_dPropBrushAttributesDialog = NULL;
	}

	if ((m_dPropFaceAttributesDialog) && (m_dPropFaceAttributesDialog != pWnd))
	{
		m_dPropFaceAttributesDialog->DestroyWindow();
		delete m_dPropFaceAttributesDialog;
		m_dPropFaceAttributesDialog = NULL;
	}

	if ((m_dPropEntitiesDialog) && (m_dPropEntitiesDialog != pWnd))
	{
		m_dPropEntitiesDialog->DestroyWindow();
		delete m_dPropEntitiesDialog;
		m_dPropEntitiesDialog = NULL;
	}

	if (&m_createArchDialog != NULL && &m_createArchDialog != pWnd)
	{
		m_createArchDialog.DestroyWindow();
		m_createArchDialog.m_hWnd = NULL;	//	post 0.57
	}

	if (&m_createBoxDialog != NULL && &m_createBoxDialog != pWnd)
	{
		m_createBoxDialog.DestroyWindow();
		m_createBoxDialog.m_hWnd = NULL;	//	post 0.57
	}
	if (&m_createConeDialog != NULL && &m_createConeDialog != pWnd)
	{
		m_createConeDialog.DestroyWindow();
		m_createConeDialog.m_hWnd = NULL;	//	post 0.57
	}

	if (&m_createConeDialog!= NULL && &m_createCylDialog != pWnd)
	{
		m_createCylDialog.DestroyWindow();
		m_createCylDialog.m_hWnd = NULL;	//	post 0.57
	}

	if (&m_createSpheroidDialog != NULL && &m_createSpheroidDialog != pWnd)
	{
		m_createSpheroidDialog.DestroyWindow();
		m_createSpheroidDialog.m_hWnd = NULL;	//	post 0.57
	}

	if (&m_createStaircaseDialog != NULL && &m_createStaircaseDialog != pWnd)
	{
		m_createStaircaseDialog.DestroyWindow();
		m_createStaircaseDialog.m_hWnd = NULL;	//	post 0.57
	}

	return true;
}




//=====================================================================================================================
// CMainFrame::OnUpdatePropertiesPanel
// Called by: Menu
// Calls: 
// Parameters: 
// Returns:	
// Purpose: places or removes check mark on the View menu
//=====================================================================================================================
void CMainFrame::OnUpdatePropertiesPanel(CCmdUI* pCmdUI) 
{
	CControlBar* pBar = NULL;
	pBar = (CControlBar*)&m_PropertiesPanel ;
	if (pBar != NULL)
    {
		pCmdUI->SetCheck((pBar->GetStyle() & WS_VISIBLE) != 0);
	}
}



//	END PROPERTIES PANEL CODE
//	****************************************************************************************
//	BEGIN CONSOLE PANEL CODE



//=====================================================================================================================
// CMainFrame::CreateConsoleDialogBar
// Called by: OnCreate()
// Calls: 
// Parameters: 
// Returns:	
// Purpose: Creates console bar and places CEdit control on the Console bar
//=====================================================================================================================
BOOL	CMainFrame::CreateConsoleDialogBar()
{
	m_ConsoleDialogBar = NULL;				//	this is a pointer so set it to NULL
	m_ConsoleDialogBar = new CDialogBar;	//	new instance and point to it

											//	 create the console dialogbar
	if (!m_ConsoleDialogBar->Create(this, IDD_CONSOLE_DIALOG, CBRS_GRIPPER |
		CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, IDD_CONSOLE_DIALOG))
	{
		TRACE0("Failed to create Console DialogBar\n");
		return FALSE;
	}
										//	assign a var to the template's CEdit control
	m_consoleEdit = (CEdit*)m_ConsoleDialogBar->GetDlgItem(IDC_CONSOLE_EDIT);
										//	set the font using global font var
	m_consoleEdit->SetFont(&CGlobals::m_ControlBarFont);
										//	put in a test string for fun
	m_consoleEdit->SetWindowText(_T("Test"));

	m_consoleCopyButton = (CButton*)m_ConsoleDialogBar->GetDlgItem(IDC_CONSOLE_COPY_BTN);
										//	 make it dockable
	m_ConsoleDialogBar->EnableDocking(CBRS_ALIGN_BOTTOM);
										//	hide it until we need it
	m_ConsoleDialogBar->ShowWindow(SW_HIDE);
	
 return TRUE;
}


//=====================================================================================================================
// CMainFrame::ShowConsoleBar
// Called by: CCompiler(3) and CGEditProDoc(1)
// Calls: 
// Parameters: 
// Returns:	
// Purpose: Bring Console bar outta hiding. Put it where it belongs
//=====================================================================================================================
void	CMainFrame::ShowConsoleBar()
{
	if( m_ConsoleDialogBar )
	{
		DockControlBar(m_ConsoleDialogBar);
		ShowControlBar( m_ConsoleDialogBar, TRUE, FALSE ) ;
	}
}


//=====================================================================================================================
// CMainFrame::TranslateString
// Called by: ConPrintf (just below)
// Calls: 
// Parameters: 
// Returns:	
// Purpose: organize char buffer into comprehensible strings of chars - for Console output
//=====================================================================================================================
char *CMainFrame::TranslateString(char *buf)
{
	static	char	buf2[32768];
	char			*out = NULL;
	unsigned int	i;

	for(i=0, out=buf2;i < strlen(buf);i++)
	{
		if(buf[i]=='\n')
		{
			*out++	='\r';
			*out++	='\n';
		}
		else
		{
			*out++	=buf[i];
		}
	}
	*out++	=0;

	return	buf2;
}


//=====================================================================================================================
// CMainFrame::ConClear
// Called by: CGEditProDoc::OnCompile(), and OnMaxtextConedit(below)
// Calls: 
// Parameters: 
// Returns:	
// Purpose: clear out the console window and get it ready for new messages
//=====================================================================================================================
void CMainFrame::ConClear()
{
	char	text[4];
	text[0]	=0;

	m_consoleEdit = (CEdit*)m_ConsoleDialogBar->GetDlgItem(IDC_CONSOLE_EDIT);

	if (m_consoleEdit == NULL)
		{AfxMessageBox("Failed to get m_ConsoleEdit"); return;}

	m_consoleEdit->SetWindowText(text);
}



//=====================================================================================================================
// CMainFrame::ConPrintf
// Called by:	Brush_SealFaces(), Brush_SnapShearNewest()
//				CGEditProDoc::CopySelectedBrushes(), LoadLeakFile(), OnBrushSelectedCopytocurrent()
//				CGEditProView::OnCompileMessage()
//				Compiler_RunPreview(), Face_SetPlaneFromFace(), fdocValidateBrush,
//				fdocValidateEntity(), Render_AllocViewVars(), Render_BlitViewDIB()
// Calls: 
// Parameters: 
// Returns:	
// Purpose: blast console messages to the console edit window
//=====================================================================================================================
void CMainFrame::ConPrintf(char *text, ...)
{
	char	buf[32768];		//this is ... cautious
	char	*out = NULL;
	CString	Listing;
	va_list argptr;

	va_start(argptr, text);
	vsprintf(buf, text, argptr);
	va_end(argptr);

	out	=TranslateString(buf);

	m_consoleEdit = (CEdit*)m_ConsoleDialogBar->GetDlgItem(IDC_CONSOLE_EDIT);

	if (m_consoleEdit == NULL)
		{AfxMessageBox("Failed to get m_ConsoleEdit"); return;}

	m_consoleEdit->ReplaceSel(out);
}



//=====================================================================================================================
// CMainFrame::ConError
// Called by: tEditView::OnCompileError()
// Calls: 
// Parameters: 
// Returns:	
// Purpose: announces a compiler error -- puts message on console
//=====================================================================================================================
void CMainFrame::ConError(char *text, ...)
{
	char	buf[32768];		
	char	*out = NULL;
	va_list argptr;

	va_start(argptr, text);
	vsprintf(buf, text, argptr);
	va_end(argptr);

	out	=TranslateString(buf);

	m_consoleEdit = (CEdit*)m_ConsoleDialogBar->GetDlgItem(IDC_CONSOLE_EDIT);

	if (m_consoleEdit == NULL)
		{AfxMessageBox("Failed to get m_ConsoleEdit"); return;}

	m_consoleEdit->ReplaceSel(out);
  }



/*
//=====================================================================================================================
// CMainFrame::OnMaxtextConedit		OBSOLETE
// Called by: 
// Calls: 
// Parameters: 
// Returns:	
// Purpose: 
//=====================================================================================================================
void CMainFrame::OnMaxtextConedit() 
{
	ConClear();
}
*/

//=====================================================================================================================
// CMainFrame::OnConsole
// Called by: Menu
// Calls: 
// Parameters: 
// Returns:	
// Purpose: Displays or hides console bar
//=====================================================================================================================
void CMainFrame::OnConsole() 
{
	if( m_ConsoleDialogBar )
	{
		DockControlBar(m_ConsoleDialogBar);
		ShowControlBar( m_ConsoleDialogBar, (m_ConsoleDialogBar->GetStyle() & WS_VISIBLE) == 0, FALSE ) ;
	}
}


//=====================================================================================================================
// CMainFrame::OnUpdateConsole
// Called by: Messaging, keeps menu current
// Calls: 
// Parameters: 
// Returns:	
// Purpose: places or removes check mark beside Console spot on View menu
//=====================================================================================================================
void CMainFrame::OnUpdateConsole(CCmdUI* pCmdUI) 
{
	if (m_ConsoleDialogBar != NULL)
    {
		pCmdUI->SetCheck((m_ConsoleDialogBar->GetStyle() & WS_VISIBLE) != 0);
	}	
}



//=====================================================================================================================
// CMainFrame::OnConsoleCopyBtn() 
// Called by: messaging framework - copy to notepad button
// Calls: 
// Parameters: 
// Returns:	
// Purpose: Selects and copies console contents to clipboard, opens up Notepad
//=====================================================================================================================
void CMainFrame::OnConsoleCopyBtn() 
{
	// make sure we have a legit edit box
	m_consoleEdit = (CEdit*)m_ConsoleDialogBar->GetDlgItem(IDC_CONSOLE_EDIT);
	if (m_consoleEdit == NULL)
	{AfxMessageBox("Failed to get m_ConsoleEdit"); return;}

	m_consoleEdit->SetSel(0, -1);	// select all text on the console
	m_consoleEdit->Copy();			// copy it to the clipboard as text

	//	prepare to fire up Notepad.exe
	STARTUPINFO			si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	//	setup error string if can't find Notepad
	CString notepadError = _T("No NotePad on your system, sorry.\n\n");
	notepadError += _T("However, you now may paste console text\n");
	notepadError += _T("into an editor of your choice.");		

	// launch Notepad... if present
	if(!::CreateProcess(NULL, _T("Notepad"), NULL, NULL, 
		FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{								//	if not present, let 'em know they can
		//	paste into any text editor of their choice
		AfxMessageBox(notepadError);
	}
	else
	{								//	if Notepad successfull, sever relations
		//	it's on its own now.
		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);
	}
}



void CMainFrame::OnConsoleQuickviewBtn() 
{
	OnPreview();	
}




//=====================================================================================================================
// CMainFrame::OnCloseConsolebtn() 
// Called by: messaging framework - Close console button
// Calls: 
// Parameters: 
// Returns:	
// Purpose: Manually closes the console dialog bar.
//=====================================================================================================================
void CMainFrame::OnCloseConsolebtn() 
{
	if( m_ConsoleDialogBar )
	{
		ShowControlBar( m_ConsoleDialogBar, FALSE, FALSE ) ;
	}
}



//	END CONSOLE PANEL CODE
//	****************************************************************************************
//	BEGIN COMMAND PANEL / TAB CODE

//==========================================================================================
// CMainFrame::InitCommandPanel
// Called by: OnCreate()
// Calls: 
// Parameters: void
// Returns:	bool
// Purpose: setup the main command panel. This sits on the left side of the view ports
//==========================================================================================
bool CMainFrame::InitCommandDialogBar()
{
	int			i ;
	TC_ITEM		TabItem ;
	char		szBuffer[MAX_TAB_NAME_LENGTH] ;
	HINSTANCE	hResources  = AfxGetResourceHandle() ;

	//	create the command dialog. This will hold
	//	the tab control and the tab control tabs will hold the
	//	individual control dialogs.
	if (!m_CommandDialogBar.Create(this, IDD_COMMANDDIALOG, CBRS_GRIPPER |
		CBRS_ALIGN_LEFT|CBRS_TOOLTIPS|CBRS_FLYBY | CBRS_SIZE_DYNAMIC, IDC_COMMAND_TAB))
	{
		TRACE0("Failed to create DlgBar\n");
		return FALSE;	
	}

	//	make it dockabale
	m_CommandDialogBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_CommandDialogBar);

	//	Create the tab control
	m_CommandTabControl = (CTabCtrl*)m_CommandDialogBar.GetDlgItem( IDC_COMMAND_TAB ) ;
	ASSERT( m_CommandTabControl != NULL ) ;

	CWnd	*pCommandTabControlWnd;
	pCommandTabControlWnd = m_CommandTabControl->GetParent();
	ASSERT( pCommandTabControlWnd != NULL ) ;

	//	set up the tab control labels by using 
	//	an array of Identifiers for strings
	//	tabNames is defined at the top of this file.
	TabItem.mask = TCIF_TEXT ;
	for( i=0; i< sizeof(tabNames)/sizeof(*tabNames); i++ )
	{
		::LoadString( hResources, tabNames[i], szBuffer, sizeof szBuffer ) ;
		TabItem.pszText = szBuffer ;
		TabItem.cchTextMax = strlen( szBuffer ) + sizeof szBuffer[0] ;
		m_CommandTabControl->InsertItem(i, &TabItem);
	}
	//	create the dialogs for the tab bar
	m_GroupsTab.Create( IDD_GROUPDIALOG, m_CommandTabControl ) ;
	m_TexturesTab.Create(IDD_TEXTURE_DLG, m_CommandTabControl);
	m_TemplatesTab.Create(IDD_TEMPLATES_DLG, m_CommandTabControl);
	m_ModelsTab.Create(IDD_MODELKEY, m_CommandTabControl);

	// Default to TemplatesTab
	m_CommandTabControl->SetCurSel( MAINFRM_COMMANDPANEL_TEMPLATES ) ;
	m_eCurrentTab = MAINFRM_COMMANDPANEL_TEMPLATES ;	

	return TRUE;
}



//==========================================================================================
// CMainFrame::SetCommandPanelTab
// Called by: OnNotify(here), and CGEditProDoc::SelectTab()
// Calls: 
// Parameters: MAINFRM_COMMANDPANEL_TAB nTab
// Returns:	void
// Purpose: 
//==========================================================================================
void CMainFrame::SetCommandPanelTab( MAINFRM_COMMANDPANEL_TAB nTab )
{
	CDialog	*	pOldDialog = NULL;
	CDialog *	pNewDialog = NULL;

	ASSERT( nTab < MAINFRM_COMMANDPANEL_LAST ) ;

	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc != NULL)
	{
		if( m_eCurrentTab != nTab )
		{
			m_ePreviousTab = m_eCurrentTab;	//	keep the previous tab for a test below

			// Notify Tab closing
			pOldDialog = DialogFromIndex( m_eCurrentTab ) ;
			pOldDialog->ShowWindow( SW_HIDE ) ;		

			// Notify Tab opening
			pNewDialog = DialogFromIndex( nTab ) ;
			pNewDialog->ShowWindow( SW_SHOW ) ;
			m_eCurrentTab = nTab ;
		}

		//	Now set the appropriate Mode based on the tab that was just selected
		//	This sends the current tab info to the SetMode() in CGEditProDoc.
		//	If it's the Templates tab, then the Templates mode is set and
		//	the templates button is pushed.
		//	If the Textures tab is pushed while at the Templates tab, then the Templates mode will
		//	remain -- unless the Brush mode is active at the time. If the Textures tab is pushed
		//	while at any other tab, the Brush mode will be set, the Brush edit button is pushed.
		if (m_eCurrentTab == MAINFRM_COMMANDPANEL_TEMPLATES)
		{	
			//	post 0.57	//	added to eliminate brush AND template simultaneous
			//	selection upon hitting the Templates Tab
			pDoc->ResetAllSelectedEntities();	//	calls DoGeneralSelect which calls
			//	ConfigureCurrentTool(), which calls
			//	UpdateAllViews()
			pDoc->ResetAllSelectedFaces();
			pDoc->ResetAllSelectedBrushes();
			//	end post 0.57 add					

			pDoc->SetMode(m_eCurrentTab);	
			int nState = m_wndBrushToolBar.GetToolBarCtrl().GetState(ID_TOOLS_TEMPLATE);
			m_wndBrushToolBar.GetToolBarCtrl().SetState(ID_TOOLS_TEMPLATE, nState | TBSTATE_CHECKED);
		}

		else if (m_eCurrentTab == MAINFRM_COMMANDPANEL_TEXTURES && m_ePreviousTab == MAINFRM_COMMANDPANEL_TEMPLATES)
		{
			if (pDoc->m_iModeTool == ID_GENERALSELECT)
			{
				pDoc->SetMode(m_eCurrentTab);
				int nState = m_wndBrushToolBar.GetToolBarCtrl().GetState(ID_GENERALSELECT);
				m_wndBrushToolBar.GetToolBarCtrl().SetState(ID_GENERALSELECT, nState | TBSTATE_CHECKED);
			}
			else
			{
				pDoc->SetMode(m_ePreviousTab);
				int nState = m_wndBrushToolBar.GetToolBarCtrl().GetState(ID_TOOLS_TEMPLATE);
				m_wndBrushToolBar.GetToolBarCtrl().SetState(ID_TOOLS_TEMPLATE, nState | TBSTATE_CHECKED);
			}
		}
		else
		{
			pDoc->SetMode(m_eCurrentTab);
			int nState = m_wndBrushToolBar.GetToolBarCtrl().GetState(ID_GENERALSELECT);
			m_wndBrushToolBar.GetToolBarCtrl().SetState(ID_GENERALSELECT, nState | TBSTATE_CHECKED);
		}
	}

	else
	{
		// still need to keep track of clicked tab -- even if no docs open
		m_eCurrentTab = nTab ;
	}

}// SetCommandPanelTab




//==========================================================================================
// CMainFrame::DialogFromIndex
// Called by: SetCommandPanelTab()
// Calls: 
// Parameters: MAINFRM_COMMANDPANEL_TAB nTab
// Returns:	CDialog
// Purpose: 
//==========================================================================================
CDialog * CMainFrame::DialogFromIndex(MAINFRM_COMMANDPANEL_TAB nTab)
{
	ASSERT( nTab < MAINFRM_COMMANDPANEL_LAST ) ;

	switch( nTab )
	{
		case MAINFRM_COMMANDPANEL_TEXTURES:		return &m_TexturesTab;
		case MAINFRM_COMMANDPANEL_TEMPLATES:	return &m_TemplatesTab;
		case MAINFRM_COMMANDPANEL_MODELS :		return &m_ModelsTab;
		case MAINFRM_COMMANDPANEL_GROUPS :		return &m_GroupsTab ;
		default :
			ASSERT( 0 ) ;
			break ;
	}
	return &m_GroupsTab;
}// DialogFromIndex


//==========================================================================================
// CMainFrame::HideAllTabsBut
// Called by: UpdateMainControls()
// Calls: all tabbed dialog boxes on the Command Panel
// Parameters:
// Returns:	
// Purpose: Hides all dialogboxes associated with the CommandPanel BUT ONE
//==========================================================================================
void CMainFrame::HideAllTabsBut(int nTab)
{
	if (m_TexturesTab)
	{
		if (nTab == MAINFRM_COMMANDPANEL_TEXTURES)
			m_TexturesTab.ShowWindow(SW_SHOW);
		else 
			m_TexturesTab.ShowWindow(SW_HIDE);
	}

	if (m_TemplatesTab)
	{
		if (nTab == MAINFRM_COMMANDPANEL_TEMPLATES)
		{
			m_TemplatesTab.ShowWindow(SW_SHOW);
		}	
		else
			m_TemplatesTab.ShowWindow(SW_HIDE);
	}

	if (m_ModelsTab)
	{
		if (nTab == MAINFRM_COMMANDPANEL_MODELS)
			m_ModelsTab.ShowWindow(SW_SHOW);
	else
			m_ModelsTab.ShowWindow(SW_HIDE);
	}

	if (m_GroupsTab)
	{
		if (nTab == MAINFRM_COMMANDPANEL_GROUPS)
			m_GroupsTab.ShowWindow(SW_SHOW);
		else
			m_GroupsTab.ShowWindow(SW_HIDE);
	}
}


//==========================================================================================
// CMainFrame::UpdateMainControls
// Called by:	CBrushAttributes::OnApply(),
//				CGEditProApp::InitInstance(), OnFileNew(), OnIdle(), OnOpenRecenFile()
//				CGEditProDoc::CopySelectedBrushes(), DeleteSelectedBrushes(), DoGeneralSelect(),
//					OnBrushAddToWorld(), OnLevelOptions(), OnNewLibObject(), SetMode(),
//					UpdateSelected()
//				CGEditProView::OnActivateView(), OnBrushGroupsAddtogroup(), 
//					OnBrushRemoveselectedfromgroup(), OnToolsTemplate()
//				CGroupsDlg::OnCreateNewGroup()
//				CMainFrame::OnBrushGroupsMakenewgroup(), OnNotify(), OnSelchangeGroupList,
//					UpdateTextures()
// Calls: 
// Parameters:
// Returns:	
// Purpose: This handles main control updates. This is the main function that is called
//			by several classes for the purpose of updating toolbar and tab bar controls.
//			This has replaced the previous version's style of updating toolbar and dialog
//			bar properties by calling those classes directly. Now, nearly every control
//			update goes through here.  SHOULD TO BE CALLED AFTER DOCUMENT CREATION!
//==========================================================================================	
void CMainFrame::UpdateMainControls()
{	
	int			UpdateTexture		= 1;
	//	Clear the slate.
	//	still needed here even though
	//	CGEditProDoc::OnCloseDocument()
	//	does this before closing a document
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();	
	if ( pDoc == NULL )
	{
		int			NO_TABS_DUDE		= -1;
		HideAllPropDialogsBut(this);
		HideAllTabsBut(NO_TABS_DUDE);
		//	get the latest address for the tab control
		m_CommandTabControl = (CTabCtrl*)m_CommandDialogBar.GetDlgItem( IDC_COMMAND_TAB ) ;
		// Default to TemplatesTab
		m_CommandTabControl->SetCurSel( MAINFRM_COMMANDPANEL_TEMPLATES ) ;
		m_eCurrentTab = MAINFRM_COMMANDPANEL_TEMPLATES ;	
		m_wndGroupBar.m_comboBox.ResetContent(); // clear contents of GroupBar combo box
		return;     
	}


	//	this asks that if any entities are selected by the mouse, then call the pDoc function
	//	to display the entitiy editor onto the properties panel -- showing this entity's
	//	attributes.

	//	if an entity is selected and if the Mode is the GENERALSELECT Mode
	if ((pDoc->GetSelState() & ANYENTITY) && pDoc->m_iModeTool == ID_GENERALSELECT)
	{
		if (m_dPropEntitiesDialog)
		{
			//	see if it still on the same old entity
			m_pCurrentEntity = pDoc->m_iCurrentEntity;
			// if not, then update the entity editor
			if (m_pCurrentEntity != m_pPreviousEntity)
			{
				CEntityArray *Entities;
				Entities = pDoc->m_pLevelMgr->GetEntities (pDoc->GetLevel());
				m_dPropEntitiesDialog->EditEntity(*Entities, m_pCurrentEntity, pDoc);
				m_pPreviousEntity = m_pCurrentEntity;
			}
		}
		//	otherwise just make a new one
		else if (!m_dPropEntitiesDialog)
		{
			m_dPropEntitiesDialog = new CEntitiesDialog(&m_PropertiesPanel);
		}
		//	in any case, update
		m_dPropEntitiesDialog->UpdateEntities(pDoc);
	}

	//	if the current mode is the GENERALSELECT Mode and select mode is Brush
	if ((pDoc->m_iModeTool == ID_GENERALSELECT) && (pDoc->m_AdjustMode == ADJUST_MODE_BRUSH)
		&& (SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) > 0))
	{
		//	if at least one brush is selected, and select mode is GENERALSELECT, and
		//	brush attributes are null, then show the brush attributes dialog on the
		//	properties panel (this initializes the attributes for the brush)

		//	if there's already a brush attributes dialog box	
		if (m_dPropBrushAttributesDialog)
		{
			//	see if it still on the same old brush
			m_pCurrentBrush = SelBrushList_GetBrush(pDoc->DocVarsGetPointer()->m_pSelBrushes, 0);
			// if not, then replace the dialog box with a new one
			if (m_pCurrentBrush != m_pPreviousBrush)
			{
				m_dPropBrushAttributesDialog->DestroyWindow();
				delete m_dPropBrushAttributesDialog;
				m_dPropBrushAttributesDialog = NULL;
				m_dPropBrushAttributesDialog = new CBrushAttributesDialog(&m_PropertiesPanel);
				m_pPreviousBrush = m_pCurrentBrush;
			}
		}
		//	otherwise just make a new one
		else if (!m_dPropBrushAttributesDialog)
		{
			m_dPropBrushAttributesDialog = new CBrushAttributesDialog(&m_PropertiesPanel);				
			m_pCurrentBrush = SelBrushList_GetBrush(pDoc->DocVarsGetPointer()->m_pSelBrushes, 0);
			m_pPreviousBrush = m_pCurrentBrush;
		}

		//	 in either case, update
		m_dPropBrushAttributesDialog->UpdateBrushAttributes(pDoc);
	}

	//	if the current mode is the GENERALSELECT Mode and select mode is Brush
	//	AND NEITHER BRUSHES NOR ENTITIES ARE SELECTED
	if ((pDoc->m_iModeTool == ID_GENERALSELECT) && (pDoc->m_AdjustMode == ADJUST_MODE_BRUSH)
		&& (SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) == 0) && (pDoc->GetSelState() & NOENTITIES))
	{
		HideAllPropDialogsBut(this);		//	clear the properties panel
	}

	//	if the current mode is the GENERALSELECT Mode and select mode is Face
	if ((pDoc->m_iModeTool == ID_GENERALSELECT) && (pDoc->m_AdjustMode == ADJUST_MODE_FACE) 
		&& (SelFaceList_GetSize (pDoc->DocVarsGetPointer()->m_pSelFaces) > 0))
	{
		//	if at least one Face is selected, and select mode is GENERALSELECT, and
		//	Face attributes are null, then show the Face attributes dialog on the
		//	properties panel (this initializes the attributes for the Face)

		//	if there's already a face attributes dialog box
		if (m_dPropFaceAttributesDialog)
		{					//	see if it still on the same old face
			m_pCurrentFace = SelFaceList_GetFace (pDoc->DocVarsGetPointer()->m_pSelFaces, 0);
			// if not, then replace the dialog box with a new one
			if (m_pCurrentFace != m_pPreviousFace)
			{
				m_dPropFaceAttributesDialog->DestroyWindow();
				delete m_dPropFaceAttributesDialog;	//	post 0,57
				m_dPropFaceAttributesDialog = NULL;
				m_dPropFaceAttributesDialog = new CFaceAttributesDialog(&m_PropertiesPanel);
				m_pPreviousFace = m_pCurrentFace;
			}
		}
		//	otherwise just make a new one
		if (!m_dPropFaceAttributesDialog)
		{
			m_dPropFaceAttributesDialog = new CFaceAttributesDialog(&m_PropertiesPanel);
			m_pCurrentFace = SelFaceList_GetFace (pDoc->DocVarsGetPointer()->m_pSelFaces, 0);
			m_pPreviousFace = m_pCurrentFace;
		}
		//	 in either case, update
		m_dPropFaceAttributesDialog->UpdateFaceAttributes(pDoc, m_pCurrentFace);
	}

	//	post 0.5 release
	//	if no Faces are selected, and select mode is GENERALSELECT, and
	//	adjust mode is ADJUST_MODE_FACE and the
	//	Face attributes dialog box is showing, destroy the face attributes
	//	dialog on the properties panel.
	if ((pDoc->m_iModeTool == ID_GENERALSELECT) && (pDoc->m_AdjustMode == ADJUST_MODE_FACE) 
		&& (SelFaceList_GetSize (pDoc->DocVarsGetPointer()->m_pSelFaces) <= 0))
	{
		if (m_dPropFaceAttributesDialog)
		{
			m_dPropFaceAttributesDialog->DestroyWindow();
			delete m_dPropFaceAttributesDialog;	//	post 0.57
			m_dPropFaceAttributesDialog = NULL;
		}
	}
	//	end post 0.5 release

	//	update the Create Template dialogs
	if (m_createArchDialog.m_hWnd !=NULL)	//	post 0.55	//	added m_hWnd...
		m_createArchDialog.UpdateCreateArchDlg(pDoc);
	if (m_createBoxDialog.m_hWnd !=NULL)
		m_createBoxDialog.UpdateCreateBoxDlg(pDoc);
	if (m_createConeDialog.m_hWnd !=NULL)
		m_createConeDialog.UpdateCreateConeDlg(pDoc);
	if (m_createCylDialog.m_hWnd !=NULL)
		m_createCylDialog.UpdateCreateCylDlg(pDoc);
	if (m_createSpheroidDialog.m_hWnd !=NULL)
		m_createSpheroidDialog.UpdateCreateSpheroidDlg(pDoc);
	if (m_createStaircaseDialog.m_hWnd !=NULL)
		m_createStaircaseDialog.UpdateCreateStaircaseDlg(pDoc);


	//	Update all tabs...
	if (!m_ModelsTab.UpdateModels(pDoc, pDoc->m_pLevelMgr->GetModelInfo (pDoc->GetLevel())))
		AfxMessageBox("Failed to update Models tab");

	if (!m_TemplatesTab.UpdateTemplates(pDoc))
		AfxMessageBox("Failed to update templates tab");

	if (!m_GroupsTab.UpdateGroups(pDoc))
		AfxMessageBox("Failed to update GroupsTab");

	//	NOTE: UpdateTextures is also called independently from
	//	CTextureListBox:: in order to avoid loss-of-focus problems.
	//	CTextureListBox:: does NOT call UpdateMainControls()
	if (!m_TexturesTab.UpdateTextures( pDoc ))
		AfxMessageBox("Failed to update textures tab");

	//	Which tab do we show..?
	switch( m_eCurrentTab )
	{
	case MAINFRM_COMMANDPANEL_TEXTURES:
		{
			if( UpdateTexture )
			{
				HideAllTabsBut(MAINFRM_COMMANDPANEL_TEXTURES);
			}
		}
		break;
	case MAINFRM_COMMANDPANEL_TEMPLATES:
		{
			HideAllTabsBut(MAINFRM_COMMANDPANEL_TEMPLATES);
		}
		break;

	case MAINFRM_COMMANDPANEL_MODELS:
		{
			HideAllTabsBut(MAINFRM_COMMANDPANEL_MODELS);
		}
		break ;

	case MAINFRM_COMMANDPANEL_GROUPS:
		{
			HideAllTabsBut(MAINFRM_COMMANDPANEL_GROUPS);
		}
		break;
	}
}


//==========================================================================================
// CMainFrame::OnNotify
// Called by: Messaging -- when command panel tab is selected with mouse
// Calls: 
// Parameters:
// Returns:	
// Purpose: Handles tab selection messages from CommandPanel
//==========================================================================================
BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPNMHDR		pHeader;
	int			idCtrl, idTab ;
	pHeader = (LPNMHDR)lParam;
					//	this code handles CommandPanel tab selections
    idCtrl = (int)wParam ;

	if( IDC_COMMAND_TAB == idCtrl && TCN_SELCHANGE == pHeader->code )
	{
		idTab = (MAINFRM_COMMANDPANEL_TAB)TabCtrl_GetCurSel( pHeader->hwndFrom);

		SetCommandPanelTab((MAINFRM_COMMANDPANEL_TAB)idTab);
	}

			//	check to see if the user selected the templates tab. This is needed
			//	since the templates tab gets disabled when the user picks the
			//	GENERALSELECT button. This little routine enables the templates tab
			//	when the user selects it and changes the mode back to Templates mode.
	 if (idCtrl == IDC_COMMAND_TAB && pHeader->code != TCN_SELCHANGING)
		{
			idTab = (MAINFRM_COMMANDPANEL_TAB)TabCtrl_GetCurSel( pHeader->hwndFrom);

			if (idTab == MAINFRM_COMMANDPANEL_TEMPLATES)
			{
				SetCommandPanelTab((MAINFRM_COMMANDPANEL_TAB)idTab);
			}
		}

	if (pHeader != NULL)
	{
				// child frame sends a WM_SETFOCUS notification when it gets the focus
		if (pHeader->code == WM_SETFOCUS)
		{
			;
		//	UpdateMainControls();	//	causes endless set focus loop when Properties panel
									//	is closed or undocked
		}
	}
	return CMDIFrameWnd::OnNotify (wParam, lParam, pResult);
}


//==========================================================================================
// CMainFrame::OnCommandPanel
// Called by: Menu
// Calls: 
// Parameters:
// Returns:	
// Purpose: shows or hides command panel
//==========================================================================================
void CMainFrame::OnCommandPanel() 
{
	CControlBar* pBar = NULL;
	pBar = (CControlBar*)&m_CommandDialogBar ;

	if(pBar)
	{
		ShowControlBar( pBar, (pBar->GetStyle() & WS_VISIBLE) == 0, FALSE ) ;
	}
}


//==========================================================================================
// CMainFrame::OnUpdateCommandPanel
// Called by: Menu
// Calls: 
// Parameters:
// Returns:	
// Purpose: places or removes check mark on menu item
//==========================================================================================
void CMainFrame::OnUpdateCommandPanel(CCmdUI* pCmdUI) 
{
	CControlBar* pBar = NULL;
	pBar = (CControlBar*)&m_CommandDialogBar ;
	if (pBar)
    {
		pCmdUI->SetCheck((pBar->GetStyle() & WS_VISIBLE) != 0);
	}
}



//	END COMMAND PANEL / TAB CODE
//	****************************************************************************************
//	BEGIN TEXTURES CODE


//==========================================================================================
// CMainFrame::UpdateTextures
// Called by: CGEditProDoc::OnLeveloptions() only after txl source change
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::UpdateTextures (void)
{
	m_TexturesTab.m_TxlibChanged = true;
	if (m_eCurrentTab == MAINFRM_COMMANDPANEL_TEXTURES)
	{
		UpdateMainControls ();
	}
}

//	END TEXTURES CODE
//	***************************************************************************************
//	START MODELS DIALOG CODE

//==========================================================================================
// CMainFrame::UpdateSelectedModel
// Called by: 
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::UpdateSelectedModel( int MoveRotate, geVec3d const *pVecDelta)
{

	if (&m_ModelsTab != NULL)
	{
		m_ModelsTab.UpdateSelectedModel( MoveRotate, pVecDelta );
	}
}


//	END MODELS DIALOG CODE
//	************************************************************************************************
//	START STATUSBAR CODE

//=====================================================================================================================
// CMainFrame::CreateStatusBar
// Called by: CMainFrame::OnCreate
// Calls: 
// Parameters:void
// Returns:	BOOL
// Purpose: 
//=====================================================================================================================
BOOL CMainFrame::CreateStatusBar()
{
	int		SProgressPane, SLockPane, SelInfoPane, WorldPosPane, CursorInfoPane, GridPane, 
				SnapPane, pane_width;
	UINT	SProgressFlags, SLockFlags, SelInfoFlags, WorldPosFlags, CursorInfoFlags, 
				GridFlags, SnapFlags, pane_id, pane_style;
	CStatusBar	*pStatusBar = NULL;

				//	create the status bar. Use default Creat() params, get no messages
	if (!m_wndStatusBar.Create(this)  ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return FALSE;      
	}
								//	setup a pointer to the statusbar
	pStatusBar = &m_wndStatusBar;

	if (pStatusBar)
	{
							//	assign indicators to panes
		SProgressPane = pStatusBar->CommandToIndex(ID_SEPARATOR);
		SLockPane = pStatusBar->CommandToIndex(ID_INDICATOR_SLOCK);
		SelInfoPane = pStatusBar->CommandToIndex(ID_INDICATOR_SELINFO);
		WorldPosPane = pStatusBar->CommandToIndex(ID_INDICATOR_WORLDPOS);
		CursorInfoPane = pStatusBar->CommandToIndex(ID_INDICATOR_CURSORINFO);
		GridPane = pStatusBar->CommandToIndex(ID_INDICATOR_GRID);
		SnapPane = pStatusBar->CommandToIndex(ID_INDICATOR_SNAP);
		
								//	set pane styles
				// put the progress control at the message pane 
		SProgressFlags = pStatusBar->GetPaneStyle(SProgressPane);	
		SProgressFlags |= SBPS_NOBORDERS;		//	add to default styles flags
		pStatusBar->SetPaneStyle(SProgressPane, SProgressFlags);
		pStatusBar->GetPaneInfo( ID_SEPARATOR_PANE,	pane_id, pane_style, pane_width );
		pStatusBar->SetPaneInfo( ID_SEPARATOR_PANE,	pane_id, pane_style, 110);	

		SLockFlags = pStatusBar->GetPaneStyle(SLockPane);
		SLockFlags |= SBPS_NOBORDERS;		//	add to default styles flags
		pStatusBar->SetPaneStyle(SLockPane, SLockFlags);
		pStatusBar->GetPaneInfo( ID_SLOCK_PANE,	pane_id, pane_style, pane_width );
		pStatusBar->SetPaneInfo( ID_SLOCK_PANE,	pane_id, pane_style, 110);	//	set width
		
		SelInfoFlags = pStatusBar->GetPaneStyle(SelInfoPane);
		SelInfoFlags |= SBPS_NOBORDERS    ;
		pStatusBar->SetPaneStyle(SelInfoPane, SelInfoFlags);
		pStatusBar->GetPaneInfo( ID_SELINFO_PANE,	pane_id, pane_style, pane_width );
		pStatusBar->SetPaneInfo( ID_SELINFO_PANE,	pane_id, pane_style, 110);
		
		WorldPosFlags = pStatusBar->GetPaneStyle(WorldPosPane);
		WorldPosFlags |= SBPS_NOBORDERS;
		pStatusBar->SetPaneStyle(WorldPosPane, WorldPosFlags);
		pStatusBar->GetPaneInfo( ID_WORLDPOS_PANE,	pane_id, pane_style, pane_width );
		pStatusBar->SetPaneInfo( ID_WORLDPOS_PANE,	pane_id, pane_style, 110);
		
		CursorInfoFlags = pStatusBar->GetPaneStyle(CursorInfoPane);
		CursorInfoFlags |= SBPS_NOBORDERS;
		pStatusBar->SetPaneStyle(CursorInfoPane, CursorInfoFlags);
		pStatusBar->GetPaneInfo( ID_CURSORINFO_PANE, pane_id, pane_style, pane_width );
		pStatusBar->SetPaneInfo( ID_CURSORINFO_PANE, pane_id, pane_style, 110);
		
		GridFlags = pStatusBar->GetPaneStyle(GridPane);
		GridFlags |= SBPS_NOBORDERS;
		pStatusBar->SetPaneStyle(GridPane, GridFlags);
		
		SnapFlags = pStatusBar->GetPaneStyle(SnapPane);
		SnapFlags |= SBPS_NOBORDERS;
		pStatusBar->SetPaneStyle(SnapPane, SnapFlags);
		
								//	set font
		pStatusBar->SetFont(&CGlobals::m_ControlBarFont);
	}
	
							//	bring to top so controlbars don't block it
	m_wndStatusBar.BringWindowToTop();
	
	return TRUE;
}

//==========================================================================================
// CMainFrame::OnUpdateSLock
// Called by: messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: updates status bar
//==========================================================================================
#pragma warning (disable:4100)
void CMainFrame::OnUpdateSLock(CCmdUI *pCmdUI)
{
	CGEditProDoc *pDoc = NULL;
	pDoc = (CGEditProDoc*)GetCurrentDoc();
	if (pDoc)
	{
		if (pDoc->IsSelectionLocked()) 
		{
			m_wndStatusBar.SetPaneText(ID_SLOCK_PANE, "SLOCK");
		}
		else 
		{
			m_wndStatusBar.SetPaneText(ID_SLOCK_PANE, "");
		}
		return;
	}
	m_wndStatusBar.SetPaneText(ID_SLOCK_PANE, "");
}
#pragma warning (default:4100)


//==========================================================================================
// CMainFrame::OnUpdateSelInfo
// Called by: messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: updates status bar
//==========================================================================================
void CMainFrame::OnUpdateSelInfo(CCmdUI *pCmdUI)
{
	
	CString Text = "0 items selected";

	pCmdUI->Enable();


	CGEditProDoc *pDoc = NULL;
	pDoc = (CGEditProDoc*)GetCurrentDoc();
	if(pDoc)
	{
		CString etxt, btxt, ftxt;
		int NumSelBrushes;
		int NumSelFaces;
		SelBrushList *pSelBrushes = NULL;
		SelFaceList *pSelFaces = NULL;
		pSelBrushes = (SelBrushList *)pDoc->DocVarsGetPointer()->m_pSelBrushes;
		pSelFaces = (SelFaceList *)pDoc->DocVarsGetPointer()->m_pSelFaces;
	
		if (pSelBrushes)
			NumSelBrushes = SelBrushList_GetSize (pSelBrushes);
		if (pSelFaces)
            NumSelFaces = SelFaceList_GetSize (pSelFaces);

		etxt.Format("Ent: %d", pDoc->m_iNumSelEntities);
		btxt.Format(" Brsh: %d", NumSelBrushes);
		ftxt.Format(" Face: %d", NumSelFaces);
		Text.Format("%s%s%s",
			pDoc->m_iNumSelEntities ? etxt.GetBuffer(64) : "",
			NumSelBrushes ? btxt.GetBuffer(64) : "",
			NumSelFaces ? ftxt.GetBuffer(64) : "");

		switch (pDoc->m_AdjustMode)
		{
			case ADJUST_MODE_BRUSH :
				Text = "BRUSHMODE:"+Text;
				break;
			case ADJUST_MODE_FACE :
				Text = "FACEMODE:"+Text;
				break;
			default :
				assert (0);	// can't happen??
//				pCmdUI->SetText( Text);
				break;
		}
	}
	pCmdUI->SetText (Text) ;
}


//==========================================================================================
// CMainFrame::OnUpdateCursorInfo
// Called by: messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: updates status bar
//==========================================================================================
void CMainFrame::OnUpdateCursorInfo(CCmdUI *pCmdUI)
{
	char info[256];

	pCmdUI->Enable();

	CGEditProDoc *pDoc = NULL;
	pDoc = (CGEditProDoc*)GetCurrentDoc();
	if (pDoc)
	{
		pDoc->GetCursorInfo(info, 15);
		pCmdUI->SetText( info );
		return;
	}
	pCmdUI->SetText( "" );
}


//==========================================================================================
// CMainFrame::OnUpdateWorldPos
// Called by: messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: updates status bar
//==========================================================================================
void CMainFrame::OnUpdateWorldPos(CCmdUI *pCmdUI)
{
	CPoint		CursorPos;
	char		stuff[100];
	CString		strWorldPos = _T("");
	CGEditProView * pView = NULL ;
	CWnd		* pWnd = NULL; 
	geVec3d		wp ;

	pCmdUI->Enable() ;
	GetCursorPos(&CursorPos);
	pWnd = WindowFromPoint( CursorPos );

	if (pWnd)
	{
		if( pWnd->IsKindOf( RUNTIME_CLASS (CGEditProView)) )
		{
			pView = (CGEditProView *)pWnd ;
			if (pView)
			{
				if( pView->mViewType == ID_VIEW_TOPVIEW ||
					pView->mViewType == ID_VIEW_FRONTVIEW ||
					pView->mViewType == ID_VIEW_SIDEVIEW )
				{
					pView->ScreenToClient( &CursorPos ) ; 
					Render_ViewToWorld( pView->VCam, CursorPos.x, CursorPos.y, &wp ) ;

					CGEditProDoc *pDoc = NULL;
					pDoc = (CGEditProDoc*)GetCurrentDoc();

					if(pDoc)
					{
						if (pDoc->m_pLevelMgr->GetGridType (pDoc->GetLevel()) == GridMetric)
						{
							geVec3d_Scale (&wp, Units_EngineToCentimeters (1.0f), &wp);
						}
					}
//					sprintf( stuff, "x%4d  y%4d  z%4d", (int)wp.X, (int)wp.Y, (int)wp.Z );
					strWorldPos.Format( "x%4d  y%4d  z%4d", (int)wp.X, (int)wp.Y, (int)wp.Z );
					pCmdUI->SetText( strWorldPos ) ;
					return ;
				}
			}
		}
	}

	m_wndStatusBar.SetPaneText( ID_WORLDPOS_PANE, "" );
}


//==========================================================================================
// CMainFrame::OnUpdateGridInfo
// Called by: messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: updates status bar
//==========================================================================================
void CMainFrame::OnUpdateGridInfo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetText( szGridString );
}


//==========================================================================================
// CMainFrame::OnUpdateSnapInfo
// Called by: messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: updates status bar
//==========================================================================================
void CMainFrame::OnUpdateSnapInfo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetText( szSnapString ) ;
}


//	END STATUS BAR CODE
//	****************************************************************************************

//==========================================================================================
// CMainFrame::CreateNewGameViewFrame
// Called by: MakeNewView()
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
CFrameWnd* CMainFrame::CreateNewGameViewFrame(CRuntimeClass* pViewClass,  CDocTemplate* pTemplate, CDocument* pDoc, CFrameWnd* pOther)
{
				// make sure we have a doc
	if (pDoc) 
	{
		ASSERT_VALID(pDoc);
	}

				// create a frame wired to the specified document
	CCreateContext context;
	context.m_pCurrentFrame = pOther;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = pViewClass;
	context.m_pNewDocTemplate = pTemplate;

				// make the object
	CFrameWnd* pFrame = (CFrameWnd*)(TEMPLATE_CHILD_FRAME_CLASS->CreateObject());
	if (pFrame == NULL)
	{
		TRACE1("Warning: Dynamic create of frame %hs failed.\n",
			TEMPLATE_CHILD_FRAME_CLASS->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

				// create new from resource
	if (!pFrame->LoadFrame(TEMPLATE_RESOURCE_ID,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
			NULL, &context))
	{
		TRACE0("Warning: CDocTemplate couldn't create a frame.\n");
				// frame will be deleted in PostNcDestroy cleanup
		return NULL;
	}
	
	return pFrame;
}



//==========================================================================================
// CMainFrame::MakeNewView
// Called by: Nothing?
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::MakeNewView( CRuntimeClass* pViewRuntimeClass)
{
	CDocument* pDocument = NULL;
	CChildFrame *pActiveChild = NULL;

	pActiveChild =(CChildFrame *)this->MDIGetActive();

	pDocument = this->GetCurrentDoc ();

	// otherwise we have a new frame !
	CDocTemplate* pTemplate = pDocument->GetDocTemplate();
	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = CreateNewGameViewFrame(pViewRuntimeClass, pTemplate, pDocument, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame.\n");
		return;     // command failed
	}

	pTemplate->InitialUpdateFrame(pFrame, pDocument);
}



//==========================================================================================
//			GetMag10	
// Called by: UpdateGridSize(below)
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
static int GetMag10
	(
	  int Num
	)
/*
  Returns the number of the multiple of 10 that is
  less than or equal to the number.  That is,
  0 thru 9 returns 0
  10 thru 99 returns 1
  100 thru 999 returns 2
  etc.

  This will work for negative numbers, although the
  return value is positive.  (i.e. -10 thru -99 returns 1)
*/
{
	int Mag;
	int MyNum;

	Mag = 0;
	MyNum = Num/10;
	while (MyNum != 0)
	{
		++Mag;
		MyNum /= 10;		
	}
	return Mag;
}


//==========================================================================================
// CMainFrame::UpdateGridSize
// Called by: CGEditProDoc::UpdateGridInformation
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
// update the size of the grid in the pane
void CMainFrame::UpdateGridSize(geFloat GridSize, int SnapOn, int snapto, int gunits, int snapunits)
{
	int	GridIndex;
	static const char *GridUnits[] =
		{"CENTI", "DECI", "METER", "10MTR", "100METER", "KILO" };

	assert (GridSize > 0);
	assert (GridSize < 1000000);
	assert (snapto > 0);
	assert (snapto < 1000000);

	if(!gunits)
	{
					//use plus one to compensate for fp error
		GridIndex = GetMag10((int)Units_EngineToCentimeters(GridSize)+1);

					// build grid units into string
		sprintf( szGridString, "Grid: %s", GridUnits[GridIndex]);
	}
	else
	{
					// build grid units into string
		sprintf( szGridString, "Grid: %d Texel", (int)GridSize);
	}

					// append the snap information.
	if(SnapOn)
	{
		int SnapIndex;

					// build snap units and append to grid string
		if(!snapunits)
		{
			SnapIndex = GetMag10 (snapto);
			sprintf( szSnapString, "Snap: %s", GridUnits[SnapIndex]);
		}
		else
		{
			sprintf( szSnapString, "Snap: %d Texel", snapto);
		}
	}
	else
	{
		strcpy( szSnapString, "Snap: Off" ) ;
	}
}



//==========================================================================================
// CMainFrame::OnWindowNew
// Called by: Messaging?
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::OnWindowNew() 
{
	CDocument *pDoc = NULL;

	pDoc = this->GetCurrentDoc ();
	if (pDoc != NULL)
	{
		CChildFrame* pActiveChild;
	
		pActiveChild =(CChildFrame *)this->MDIGetActive();

					// Create a new frame !
		CDocTemplate* pTemplate = pDoc->GetDocTemplate();
		ASSERT_VALID(pTemplate);

		CFrameWnd* pFrame = NULL;
		pFrame = pTemplate->CreateNewFrame(pDoc, pActiveChild);
		if (pFrame == NULL)
		{
			TRACE0("Warning: failed to create new frame.\n");
			return;     // command failed
		}

		pTemplate->InitialUpdateFrame(pFrame, pDoc);
	}
}


//==========================================================================================
// CMainFrame::GetCurrentDoc
// Called by: many CMainFrame Functions
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
CGEditProDoc *CMainFrame::GetCurrentDoc( )
{
	CChildFrame* pActiveChild = NULL;

	
	CGEditProDoc *pDoc = NULL;

	pActiveChild =(CChildFrame *)this->MDIGetActive();

	if (pActiveChild != NULL)
	{
		pDoc =(CGEditProDoc*)pActiveChild->GetActiveDocument();
	}
	return pDoc;
}


//==========================================================================================
// CMainFrame::GetActiveDocument
// Called by: the framework
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
CDocument* CMainFrame::GetActiveDocument() 
{
	
	return CMDIFrameWnd::GetActiveDocument();
}



//==========================================================================================
// CMainFrame::OnModebar
// Called by: Menu messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::OnModebar() 
{

	CControlBar* pBar = NULL;
	pBar = (CControlBar*)&m_wndBrushToolBar ;

	if( pBar )
	{
		ShowControlBar( pBar, (pBar->GetStyle() & WS_VISIBLE) == 0, FALSE ) ;
	}
}


//==========================================================================================
// CMainFrame::OnUpdateModebar
// Called by: Menu messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::OnUpdateModebar(CCmdUI* pCmdUI) 
{
	CControlBar* pBar = NULL;
	pBar = (CControlBar*)&m_wndBrushToolBar ;
	if (pBar != NULL)
    {
		pCmdUI->SetCheck((pBar->GetStyle() & WS_VISIBLE) != 0);
	}
}


//==========================================================================================
// CMainFrame::OnClose
// Called by: menu
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::OnClose() 
{
	SaveBarState( "DESKTOP" ) ;	

	//	create an OleDataObject for getting clipboard data	
	COleDataObject	obj;
	//	try to connect to the clipboard
	if (obj.AttachClipboard())
	{
		//	check to see if the clipboard's contents match this app's format
		if (obj.IsDataAvailable(((CGEditProApp*)AfxGetApp())->GetClipboardFormat()))
		{
			obj.Release();

			if ( !OpenClipboard() )
			{
				AfxMessageBox( "Cannot open the Clipboard" );
			}
			// Remove the current Clipboard contents  
			if( !EmptyClipboard() )
			{
				AfxMessageBox( "Cannot empty the Clipboard" );
			}

			if (!CloseClipboard())
			{
				AfxMessageBox( "Cannot close the Clipboard" );
			}

		}
	}

	CMDIFrameWnd::OnClose();
}




//==========================================================================================
// CMainFrame::OnViewLeakFinder
// Called by: menu messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::OnViewLeakFinder() 
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		pDoc->SetShowLeakFinder(!pDoc->bShowLeakFinder());
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
	}
}



//==========================================================================================
// CMainFrame::OnUpdateViewLeakFinder
// Called by: menu messaging
// Calls: 
// Parameters:
// Returns:	
// Purpose: 
//==========================================================================================
void CMainFrame::OnUpdateViewLeakFinder(CCmdUI* pCmdUI) 
{
	geBoolean	bEnable	=GE_FALSE;
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();

	if(pDoc)
	{
		bEnable	=pDoc->IsLeakFileLoaded();
	}	
	pCmdUI->Enable(bEnable) ;

	//should this be enabled even if greyed?
	//so you know that it will display when loaded?
	pCmdUI->SetCheck(pDoc->bShowLeakFinder());
}





void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
}


///////////////////////////////////////////////////////////
// CreateProgressBar

BOOL CMainFrame::CreateProgressBar(	CProgressCtrl* pProgressCtrl,
							CStatusBar* pStatusBar,
							LPCTSTR szMessage,
							int nPaneIndex,
							int cxMargin,
							int cxMaxWidth,
							UINT nIDControl )
{
	ASSERT_VALID( pProgressCtrl );
	ASSERT_VALID( pStatusBar );

	// Calculate destination rectangle for progress control
    CRect rc;
    pStatusBar->GetItemRect( nPaneIndex, &rc );

	// Define progress bar horizontal offset
	if( szMessage != NULL )
	{
		// Compute message text extent
		CClientDC dc( pStatusBar );
		CFont* pFont = NULL;
		pFont = pStatusBar->GetFont();
		if (pFont)
		{
			CFont* pOldFont = NULL;
			pOldFont = dc.SelectObject( pFont );
			if (pOldFont)
			{
				CSize sizeText = dc.GetTextExtent( szMessage );
				dc.SelectObject( pOldFont );
				rc.left += sizeText.cx + cxMargin;
			}
		}
	}

	// Compute progress bar width
	if( cxMaxWidth != -1 )
	{
		rc.right = rc.left + min( cxMaxWidth, rc.Width() );
	}

	// Display message text
	pStatusBar->SetPaneText( nPaneIndex, szMessage );
	pStatusBar->RedrawWindow();

	// Create progress control
    return pProgressCtrl->Create(	WS_CHILD | WS_VISIBLE,
									rc, pStatusBar, nIDControl );
}



//	open up QuickViewer
void CMainFrame::OnPreview() 
{
	//	check the toolbar button before ddisplaying the modal dialog box
	m_wndToolBar.GetToolBarCtrl().CheckButton(IDM_PREVIEW);

	//	bring up the preview dialog box
	previewDlg.DoModal();
}

void CMainFrame::OnUpdatePreview(CCmdUI* pCmdUI) 
{
	if (previewDlg.m_hWnd)	// if the preview dialog box is showing
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}


void CMainFrame::OnInitMenu(CMenu* pMenu)
{
   CMDIFrameWnd::OnInitMenu(pMenu);
  
	// CG: This block added by 'Tip of the Day' component.
	{
		// TODO: This code adds the "Tip of the Day" menu item
		// on the fly.  It may be removed after adding the menu
		// item to all applicable menu items using the resource
		// editor.

		// Add Tip of the Day menu item on the fly!
		static CMenu* pSubMenu = NULL;

		CString strHelp; strHelp.LoadString(CG_IDS_TIPOFTHEDAYHELP);
		CString strMenu;
		int nMenuCount = pMenu->GetMenuItemCount();
		BOOL bFound = FALSE;
		for (int i=0; i < nMenuCount; i++) 
		{
			pMenu->GetMenuString(i, strMenu, MF_BYPOSITION);
			if (strMenu == strHelp)
			{ 
				pSubMenu = pMenu->GetSubMenu(i);
				bFound = TRUE;
				ASSERT(pSubMenu != NULL);
			}
		}

		CString strTipMenu;
		strTipMenu.LoadString(CG_IDS_TIPOFTHEDAYMENU);
		if (!bFound)
		{
			// Help menu is not available. Please add it!
			if (pSubMenu == NULL) 
			{
				// The same pop-up menu is shared between mainfrm and frame 
				// with the doc.
				static CMenu popUpMenu;
				pSubMenu = &popUpMenu;
				pSubMenu->CreatePopupMenu();
				pSubMenu->InsertMenu(0, MF_STRING|MF_BYPOSITION, 
					CG_IDS_TIPOFTHEDAY, strTipMenu);
			} 
			pMenu->AppendMenu(MF_STRING|MF_BYPOSITION|MF_ENABLED|MF_POPUP, 
				(UINT)pSubMenu->m_hMenu, strHelp);
			DrawMenuBar();
		} 
		else
		{      
			// Check to see if the Tip of the Day menu has already been added.
			pSubMenu->GetMenuString(0, strMenu, MF_BYPOSITION);

			if (strMenu != strTipMenu) 
			{
				// Tip of the Day submenu has not been added to the 
				// first position, so add it.
				pSubMenu->InsertMenu(0, MF_BYPOSITION);  // Separator
				pSubMenu->InsertMenu(0, MF_STRING|MF_BYPOSITION, 
					CG_IDS_TIPOFTHEDAY, strTipMenu);
			}
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
//	OnSetPreviewMode
//	
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState) 
{
	CString	thisFunctionName = "CMainFrame::OnSetPreviewMode";
	
	//	make certain that we don't try to get back to any previous Views
	//	Otherwise it's all crazy. NULL is best
	pState->pViewActiveOld = NULL;
	
	CMDIFrameWnd::OnSetPreviewMode(bPreview, pState);

	if (!bPreview)
	{
		CGEditProDoc *pDoc = NULL;
		pDoc = CGlobals::GetActiveDocument();
		
		if (pDoc)
		{
			CWnd *pWnd = NULL;
			CGEditProView* pView = NULL;
			pView = (CGEditProView*)GetActiveFrame()->GetActiveView();

			if (printViewDlg.m_btnTopView)
			{
				pView = (CGEditProView*)GetActiveFrame()->GetActiveView();
				CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
				
				int row = TOP_VIEW_ROW;
				int col = TOP_VIEW_COL;
				
				pWnd = pParent->GetPane(row,col);
				//	if the active pane is not the pane for this view
				if (pParent->GetActivePane(&row, &col) != pWnd)
				{
					//	make the pane for this view the active pane
					pParent->SetActivePane(row,col, pWnd);
//					pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				}
				
				pParent->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW);
				printViewDlg.m_btnTopView = FALSE;
				return;
			}
			
			if (printViewDlg.m_btnFrontView)
			{
				pView = (CGEditProView*)GetActiveFrame()->GetActiveView();
				CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
				
				int row = FRONT_VIEW_ROW;
				int col = FRONT_VIEW_COL;
				
				pWnd = pParent->GetPane(row,col);
				//	if the active pane is not the pane for this view
				if (pParent->GetActivePane(&row, &col) != pWnd)
				{
					//	make the pane for this view the active pane
					pParent->SetActivePane(row,col, pWnd);
//					pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				}
				pParent->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW);
				printViewDlg.m_btnFrontView = FALSE;
				return;
			}
			
			if (printViewDlg.m_btnSideView)
			{
				pView = (CGEditProView*)GetActiveFrame()->GetActiveView();
				CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
				
				int row = SIDE_VIEW_ROW;
				int col = SIDE_VIEW_COL;
				
				pWnd = pParent->GetPane(row,col);
				//	if the active pane is not the pane for this view
				if (pParent->GetActivePane(&row, &col) != pWnd)
				{
					//	make the pane for this view the active pane
					pParent->SetActivePane(row,col, pWnd);
//					pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				}
				pParent->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW);
				printViewDlg.m_btnSideView = FALSE;
				return;
			}
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
//	OnFilePrintPreviewtEditView
//	
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFilePrintPreviewtEditView() 
{
	CString	thisFunctionName = "CMainFrame::OnFilePrintPreviewtEditView";
	
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	
	if (pDoc)
	{
		if (printViewDlg.DoModal() ==1)	
		{
			CWnd *pWnd = NULL;
			CGEditProView* pView = NULL;
			pView = (CGEditProView*)GetActiveFrame()->GetActiveView();
			
			if (printViewDlg.m_btnTopView)
			{
				pView = (CGEditProView*)GetActiveFrame()->GetActiveView();
				CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
				
				int row = TOP_VIEW_ROW;
				int col = TOP_VIEW_COL;
				
				pWnd = pParent->GetPane(row,col);
				//	if the active pane is not the pane for this view
				if (pParent->GetActivePane(&row, &col) != pWnd)
				{
					//	make the pane for this view the active pane
					pParent->SetActivePane(row,col, pWnd);
//					pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				}
				pParent->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW);
				printViewDlg.m_btnTopView = FALSE;
				return;
			}
			
			if (printViewDlg.m_btnFrontView)
			{
				pView = (CGEditProView*)GetActiveFrame()->GetActiveView();
				CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
				
				int row = FRONT_VIEW_ROW;
				int col = FRONT_VIEW_COL;
				
				pWnd = pParent->GetPane(row,col);
				//	if the active pane is not the pane for this view
				if (pParent->GetActivePane(&row, &col) != pWnd)
				{
					//	make the pane for this view the active pane
					pParent->SetActivePane(row,col, pWnd);
//					pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				}
				pParent->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW);
				printViewDlg.m_btnFrontView = FALSE;
				return;
			}
			
			if (printViewDlg.m_btnSideView)
			{
				pView = (CGEditProView*)GetActiveFrame()->GetActiveView();
				CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
				
				int row = SIDE_VIEW_ROW;
				int col = SIDE_VIEW_COL;
				
				pWnd = pParent->GetPane(row,col);
				//	if the active pane is not the pane for this view
				if (pParent->GetActivePane(&row, &col) != pWnd)
				{
					//	make the pane for this view the active pane
					pParent->SetActivePane(row,col, pWnd);
//					pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				}
				pParent->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW);
				printViewDlg.m_btnSideView = FALSE;
				return;
			}
		}
	}
}





///////////////////////////////////////////////////////////////////////////////
//	OnFilePrinttEditView
//	
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFilePrinttEditView() 
{
	CString	thisFunctionName = "CMainFrame::OnFilePrinttEditView";

	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();

	CDialogPrintView	printViewDlg;

	if (printViewDlg.DoModal() ==1)	
	{
		CWnd *pWnd = NULL;
		CGEditProView* pView = NULL;
		pView = (CGEditProView*)GetActiveFrame()->GetActiveView();
		if (printViewDlg.m_btnTopView)
		{
			CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
			
			int row = TOP_VIEW_ROW;
			int col = TOP_VIEW_COL;
			
			pView = (CGEditProView*)pParent->GetPane(row,col);

				//	if the active pane is not the pane for this view
			if (pParent->GetActivePane(&row, &col) != pView)
			{
					//	make the pane for this view the active pane
				pParent->SetActivePane(row,col, pView);
//				pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				pView->SendMessage(WM_COMMAND, ID_FILE_PRINT);
			}
		}

		if (printViewDlg.m_btnFrontView)
		{
			CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
			
			int row = FRONT_VIEW_ROW;
			int col = FRONT_VIEW_COL;
			
			pView = (CGEditProView*)pParent->GetPane(row,col);			

				//	if the active pane is not the pane for this view
			if (pParent->GetActivePane(&row, &col) != pView)
			{
					//	make the pane for this view the active pane
				pParent->SetActivePane(row,col, pView);
//				pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				pView->SendMessage(WM_COMMAND, ID_FILE_PRINT);

			}
		}
		if (printViewDlg.m_btnSideView)
		{
			CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
			
			int row = SIDE_VIEW_ROW;
			int col = SIDE_VIEW_COL;
			
			pView = (CGEditProView*)pParent->GetPane(row,col);			

			//	if the active pane is not the pane for this view
			if (pParent->GetActivePane(&row, &col) != pView)
			{
					//	make the pane for this view the active pane
				pParent->SetActivePane(row,col, pView);
//				pDoc->UpdateAllViews(UAV_ALLVIEWS, NULL);
				pView->SendMessage(WM_COMMAND, ID_FILE_PRINT);
			}
		}

		if (printViewDlg.m_btnTexturedView)
			MessageBeep(0);
	}
}


void CMainFrame::OnEditUndo()
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		pDoc->DocVarsUndo();
	}
	
}

void CMainFrame::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		if (pDoc->GetUndoAvailability())
			pCmdUI->Enable();
		else
			pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnEditRedo()
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		pDoc->DocVarsRedo();
	}
}

void CMainFrame::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		if (pDoc->GetRedoAvailability())
			pCmdUI->Enable();
		else
			pCmdUI->Enable(FALSE);
	}
}
