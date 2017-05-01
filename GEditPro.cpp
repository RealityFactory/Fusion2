/****************************************************************************************/
/*  GEditPro.cpp                                                                          */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax, John Moore, Bruce Cooner          */
/*  Description:  Top level mfc stuff here                                              */
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
/*Genesis3D Version 1.1 released November 15, 1999                            */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/

#include "stdafx.h"
#include "Globals.h"
#include "GEditPro.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "GEditProView.h"
#include "PreferencesDialog.h"
#include "TextureDlg.h"
#include "about.h"
#include "FilePath.h"
#include <ram.h>
#include "util.h"
#include "Splash.h"
#include "TipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//#define TIME_OUT

/////////////////////////////////////////////////////////////////////////////
// CGEditProApp

BEGIN_MESSAGE_MAP(CGEditProApp, CWinApp)
	ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)
	//{{AFX_MSG_MAP(CGEditProApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	ON_COMMAND(IDM_PREFERENCES, OnPreferences)
	ON_UPDATE_COMMAND_UI(IDM_PREFERENCES, OnUpdatePreferences)
	ON_COMMAND(ID_HELP_HOWDOI, OnHelpHowdoi)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGEditProApp construction

CGEditProApp::CGEditProApp()
{
	m_pGEditProDoc = NULL;

#if _MFC_VER>0x0600 // only for MFC7.0
	EnableHtmlHelp();
#endif

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

//	Destruction...
CGEditProApp::~CGEditProApp()
{
	if( pShutdownEvent != NULL )
	{
		pShutdownEvent->SetEvent();
		
		if( pWatcher != NULL )
			::WaitForSingleObject( pWatcher->m_hThread, INFINITE );

		delete pShutdownEvent;
		pShutdownEvent = NULL;
	}

	if( pNewInstanceEvent != NULL )
	{
		delete pNewInstanceEvent;
		pNewInstanceEvent = NULL;
	}

	CloseAllDocuments( TRUE );

	if (pPrefs != NULL)
	{
		Prefs_Save (pPrefs, m_pszProfileName);
		Prefs_Destroy (&pPrefs);
	}
	if (pResolvedPrefs != NULL)
	{
		Prefs_Destroy (&pResolvedPrefs);
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGEditProApp object

CGEditProApp theApp;

#pragma warning (disable:4100)
static	LONG	__stdcall xxHandler(LPEXCEPTION_POINTERS xcpt)
{
static	int	nested;

	if	(!nested)
	{
		nested = 1;
		::MessageBeep(1);
		exit(0);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
#pragma warning (default:4100)

void CGEditProApp::ResolvePreferencesPaths (void)
{
	char AppDir[MAX_PATH];
	char WorkTxlSearch[2048];
	char Work[2048];	// hope that's big enough!!
	
	::Prefs_SetBackgroundColor (pResolvedPrefs, Prefs_GetBackgroundColor (pPrefs));
	::Prefs_SetGridColor (pResolvedPrefs, Prefs_GetGridColor (pPrefs));
	::Prefs_SetSnapGridColor (pResolvedPrefs, Prefs_GetSnapGridColor (pPrefs));
	::FilePath_GetNameAndExt (Prefs_GetTxlName (pPrefs), Work);
	::Prefs_SetTxlName (pResolvedPrefs, Work);

	WorkTxlSearch[0] = '\0';
	::GetCurrentDirectory (sizeof (WorkTxlSearch), WorkTxlSearch);
	if (::GetModuleFileName (NULL, AppDir, sizeof (AppDir)))
	{
		::FilePath_GetDriveAndDir (AppDir, AppDir);
	}
	else
	{
		AppDir[0] = '\0';
	}
	::FilePath_AppendSearchDir (WorkTxlSearch, AppDir);

	::FilePath_ResolveRelativePathList (Prefs_GetTxlSearchPath (pPrefs), Work);
	::FilePath_AppendSearchDir (WorkTxlSearch, Work);
	::Prefs_SetTxlSearchPath (pResolvedPrefs, WorkTxlSearch);

	::FilePath_ResolveRelativePath (Prefs_GetPreviewPath (pPrefs), Work);
	::Prefs_SetPreviewPath (pResolvedPrefs, Work);

	::FilePath_ResolveRelativePathList (Prefs_GetHeadersList (pPrefs), Work);
	::Prefs_SetHeadersList (pResolvedPrefs, Work);

	::FilePath_ResolveRelativePath (Prefs_GetObjectsDir (pPrefs), Work);
	::Prefs_SetObjectsDir (pResolvedPrefs, Work);
	
	::FilePath_ResolveRelativePath (Prefs_GetProjectDir (pPrefs), Work);
	::Prefs_SetProjectDir (pResolvedPrefs, Work);
}

BOOL CGEditProApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	if (!AfxOleInit())
	{
		AfxMessageBox("AfxOleInit failed");
	}


	// CG: The following block was added by the Splash Screen component.
\
	{
\
		CCommandLineInfo cmdInfo;
\
		ParseCommandLine(cmdInfo);
\

\
		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
\
	}
	char AppPath[MAX_PATH];
	
	::GetModuleFileName (NULL, AppPath, MAX_PATH);
	FilePath_GetDriveAndDir (AppPath, AppPath);

	//	set global directory path
	GetCurrentDirectory(CGlobals::m_dwNameSize, CGlobals::m_GlobalAppPath);

	SetUnhandledExceptionFilter(xxHandler);

	//	Setup global environment...

	// Initialize OLE 2.0 libraries
	/*if (!AfxOleInit())
	{
		AfxMessageBox("Cannot load OLE");
		return FALSE;
	}*/

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif


	{
		char IniFilePath[MAX_PATH] ;

		FilePath_AppendName (AppPath, GEDITPRO_INIFILE_NAME, IniFilePath);

		// Set our INI file to tEDIT.INI
		free( (void*)m_pszProfileName ) ;
		m_pszProfileName = _tcsdup( IniFilePath ) ;

	}

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// load the preferences from the INI file, and resolve any relative paths
	pPrefs = Prefs_Read (m_pszProfileName);
	pResolvedPrefs = Prefs_Create ();

	ResolvePreferencesPaths ();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	pDocTemplate = new CMultiDocTemplate(
		IDR_GEDITPROTYPE,
		RUNTIME_CLASS(CGEditProDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CGEditProView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
//	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//	Preliminary command line error checking...

	//	Are there commands?
	int index = 0;
	CString CmdLine = m_lpCmdLine;
	CString Argument;
	if( ( index = CmdLine.Find( '-' )) != -1 )
	{
		//	Is there a .3dt file in there before the first command?
		Argument = CmdLine.Left( index - 1 );
		if( Argument.IsEmpty() )
		{
			//	Nope...
			AfxMessageBox( "Unable to process command line.  No .3DT file specified.", MB_OK );
			return FALSE;
		}
	}

	//	If there is a file name in cmdInfo, does it have a .3dt extension?
	//  If not, provide one...
	Argument = cmdInfo.m_strFileName;
	Argument.MakeLower();
	if( !Argument.IsEmpty() )
	{
		if( Argument.Find( ".3dt" ) == -1 )
		{
			//	Extension not present, add it to the end of file name
			Argument = Argument + ".3dt";
			cmdInfo.m_strFileName = Argument;
		}
	}

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
	{
		pMainFrame->IsDestroyingApp = 1;
		return FALSE;
	}

	//	Check to see if this is the first instance...
	if( !IsFirstInstance() )
		return FALSE;


	//	Let's grab the active document and save it...
	CChildFrame* pActiveChild = NULL;
	pActiveChild = (CChildFrame *)pMainFrame->MDIGetActive();

	if (pActiveChild)
	{
		CGEditProDoc* pDoc = NULL;
		pDoc = (CGEditProDoc*) pActiveChild->GetActiveDocument();

		if (pDoc)
		{
			//	set our member doc to this 
			m_pGEditProDoc = pDoc;
		}
		else
		{
			TRACE0("Warning: No active document for tab update command.\n");
			AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
			return FALSE;     // command failed
		}
	
	}

	if( !ProcessCommandLine() )
	{
		return FALSE;
	}

	//	Setup user preferences...
	InitUserPreferences(pMainFrame);

	pMainFrame->UpdateMainControls();	// updates command panel

	// ROB: This is the timeout function, not needed for "release"
#ifdef TIME_OUT
	// stick in a nice date check here.  This
	// copy will expire on september 1st
	CTime t = CTime::GetCurrentTime();
	// close the application.....
	if( (t.GetMonth() >= 11) || (t.GetYear() != 1996) ) {
		return FALSE;
	}
#endif

		//	register our objects for the clipboard
	m_nClipboardFormat = ::RegisterClipboardFormat(_T("GEditPro_Object"));

	pMainFrame->IsStartingApp = 0;
						

	// CG: This line inserted by 'Tip of the Day' component.
	ShowTipAtStartup();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGEditProApp commands

BOOL CGEditProApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following line was added by the Splash Screen component.

	CSplashWnd::PreTranslateAppMessage(pMsg);
	return CWinApp::PreTranslateMessage(pMsg);
}

// App command to run the dialog
void CGEditProApp::OnAppAbout()
{
	CAboutDlg aboutDlg;

	aboutDlg.DoModal() ;
}

//	CHANGE!	03/29/97	John Moore
void CGEditProApp::InitUserPreferences(CMainFrame* pMainFrame)
{
	// Let's show the window as maximized...
	m_nCmdShow = SW_MAXIMIZE;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
}
//	End of CHANGE

BOOL CGEditProApp::OnIdle(LONG lCount) 
{
	//	Let's grab the active document and save it...
	CChildFrame* pActiveChild =(CChildFrame *)pMainFrame->MDIGetActive();
	CGEditProDoc* pDoc;
	if (pActiveChild == NULL ||
		(pDoc = (CGEditProDoc*) pActiveChild->GetActiveDocument()) == NULL)
	{
		//TRACE0("Warning: No active document for tab update command.\n");
		//AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		//return FALSE;     // command failed

		//	We need to hide all tabs from display...
		if( m_pGEditProDoc != NULL )
		{
			pMainFrame->UpdateMainControls();			
			m_pGEditProDoc = NULL;
		}
	}
	else if( m_pGEditProDoc != pDoc )
	{
		m_pGEditProDoc = pDoc;
		pMainFrame->UpdateMainControls();						
	}
	return CWinApp::OnIdle(lCount);
}



BOOL CGEditProApp::IsFirstInstance()
{
	pNewInstanceEvent = new CEvent( FALSE, FALSE, AfxGetAppName() );
	
	if( ::GetLastError() == ERROR_ALREADY_EXISTS )
	{
		pNewInstanceEvent->SetEvent();
		return FALSE;
	}

	pShutdownEvent = new CEvent;
	pWatcher = ( CActivationWatch* ) AfxBeginThread( RUNTIME_CLASS( CActivationWatch ) );

	return TRUE;
}



BOOL CGEditProApp::ProcessCommandLine()
{
	CString CmdLine = m_lpCmdLine;
	CString Argument;
	CString Parameter;
	BOOL done = CmdLine.IsEmpty();
	int index = 0;
	int length = 0;

	while( !done )
	{
		if( ( index = CmdLine.Find( ' ' )) != -1 )
		{
			Argument = CmdLine.Left( index );
			length = CmdLine.GetLength() - index - 1;
			CmdLine = CmdLine.Right( length );
		}
		else
		{
			Argument = CmdLine;
			done = TRUE;
		}

		//	Is this a command?  If so, process it and its argument(s)...
		if( Argument.GetAt( 0 ) == '-' )
		{
			//	If there's a parameter, grab it...
			if( ( index = CmdLine.Find( ' ' )) != -1 )
			{
				Parameter = CmdLine.Left( index );
				length = CmdLine.GetLength() - index - 1;
				CmdLine = CmdLine.Right( length );
			}
			else
			{
				//	Is CmdLine empty?  If not, our last argument is the parameter...
				Parameter = CmdLine;
				done = TRUE;
			}

			if( Parameter == Argument )
				Parameter.Empty();

			//	Which command?
			Argument.MakeLower();

			if( Argument == "-export" )
			{
				// MAP file export.  Parameter indicates the file name...
				CommandLineExport( &Parameter );

				//	Request satisfied, exit the application...
				return FALSE;
			}
		}
	}

	return TRUE;
}


//	Executes command line request to export a quake map file...
void CGEditProApp::CommandLineExport( CString* pMapFileName )
{
	if( !pMapFileName->IsEmpty() )
	{
		//	Does the file name have a .MAP extension?
		//  If not, provide one...
		CString Argument = *pMapFileName;
		Argument.MakeLower();
	
		if( Argument.Find( ".map" ) == -1 )
		{
			//	Extension not present, add it to the end of file name
			Argument = Argument + ".map";
			*pMapFileName = Argument;
		}

		//	Go ahead and export...
//		pGenEditDoc->ExportToQuakeMap( FALSE, pMapFileName );
	}
	else
	{
		//	Problem! no MAP file specified...
		AfxMessageBox( "Unable to export .3DT file.  No map file name specified.\n", MB_OK );
	}
}


/////////////////////////////////////////////////////////////////////////////
//	GetClipboardFormat
//	
/////////////////////////////////////////////////////////////////////////////
UINT	CGEditProApp::GetClipboardFormat()
{
	return m_nClipboardFormat;
}




// called when user creates a new file
void CGEditProApp::OnFileNew() 
{
	// first call down to CWinApp::FileNew()
	CWinApp::OnFileNew();
	this->pMainFrame->UpdateMainControls();	
}

void CGEditProApp::OnFileOpen() 
{
	// If the current document is unmodified and has no brushes, wipe it
	CGEditProDoc* pDoc = GetActiveGenEditDoc() ;
	if( pDoc && ( pDoc->IsModified() == FALSE ) && BrushList_Count( pDoc->m_pLevelMgr->GetBrushes (pDoc->GetLevel()), BRUSH_COUNT_ALL ) == 0 )
	{
		this->pMainFrame->SendMessage( WM_COMMAND, ID_FILE_CLOSE, 0 ) ;
	}
	CWinApp::OnFileOpen ();

	this->pMainFrame->UpdateMainControls();	

}

BOOL CGEditProApp::OnOpenRecentFile (UINT nID)
{
	BOOL rslt;

	// If the current document is unmodified and has no brushes, wipe it
	CGEditProDoc* pDoc = GetActiveGenEditDoc() ;
	if( pDoc && ( pDoc->IsModified() == FALSE ) && BrushList_Count( pDoc->m_pLevelMgr->GetBrushes (pDoc->GetLevel()), BRUSH_COUNT_ALL ) == 0 )
	{
		this->pMainFrame->SendMessage( WM_COMMAND, ID_FILE_CLOSE, 0 ) ;
	}

	rslt = CWinApp::OnOpenRecentFile (nID);
	if (rslt != FALSE)
	{
		this->pMainFrame->UpdateMainControls();	
	}
	return rslt;
}



CGEditProDoc * CGEditProApp::GetActiveGenEditDoc( void ) const
{
	CGEditProDoc* pDoc;
	CChildFrame* pActiveChild =(CChildFrame *)pMainFrame->MDIGetActive();

	if(	pActiveChild == NULL ||
	  (	pDoc = (CGEditProDoc*) pActiveChild->GetActiveDocument()) == NULL )
	{
		return NULL ;
	}
	return  pDoc;
}


void CGEditProApp::OnPreferences() 
{
	CPreferencesDialog PrefsDlg;

	PrefsDlg.coBackground = Prefs_GetBackgroundColor (pPrefs);
	PrefsDlg.coGrid = Prefs_GetGridColor (pPrefs);
	PrefsDlg.coSnapGrid = Prefs_GetSnapGridColor (pPrefs);

	if(	PrefsDlg.DoModal() == IDOK )
	{
		Prefs_SetBackgroundColor (pPrefs, PrefsDlg.coBackground);
		Prefs_SetGridColor (pPrefs, PrefsDlg.coGrid);
		Prefs_SetSnapGridColor (pPrefs, PrefsDlg.coSnapGrid);
#if 1
		/*
		  Something in this program has the disturbing tendency to change 
		  the current directory.  Until I can track that down, I don't want
		  to resolve the paths but once...
		*/
		Prefs_SetBackgroundColor (pResolvedPrefs, PrefsDlg.coBackground);
		Prefs_SetGridColor (pResolvedPrefs, PrefsDlg.coGrid);
		Prefs_SetSnapGridColor (pResolvedPrefs, PrefsDlg.coSnapGrid);
#else	
		ResolvePreferencesPaths ();
#endif			
		CGEditProDoc* pDoc = GetActiveGenEditDoc() ;
		if( pDoc != NULL )
		{
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );
		}
	}
}

void CGEditProApp::OnUpdatePreferences(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable() ;
}

afx_msg void CGEditProApp::OnHelp( )
{
	WinHelp (0, HELP_FINDER);
}

afx_msg void CGEditProApp::OnHelpIndex( )
{
	WinHelp ((LONG)"", HELP_PARTIALKEY);
}

void CGEditProApp::OnHelpHowdoi() 
{
	// TODO: Add your command handler code here
	
}


void CGEditProApp::ShowTipAtStartup(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_bShowSplash)
	{
		CTipDlg dlg;
		if (dlg.m_bStartup)
			dlg.DoModal();
	}
}

void CGEditProApp::ShowTipOfTheDay(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CTipDlg dlg;
	dlg.DoModal();
}
