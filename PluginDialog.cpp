/****************************************************************************************/
/*  PluginDialog.cpp																	*/
/*                                                                                      */
/*  Author:       Tom Morris															*/
/*  Description:  Plugin Demonstration													*/
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


#include "stdafx.h"
#include "GEditPro.h"
#include "PluginDialog.h"
#include "Ram.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPluginDialog dialog


CPluginDialog::CPluginDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPluginDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPluginDialog)
	m_pluginEditBox = _T("");
	Initialized = false;

	//}}AFX_DATA_INIT


}


void CPluginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPluginDialog)
	DDX_Control(pDX, IDC_PLUGIN_STOPBTN, m_plugin1StopBTN);
	DDX_Control(pDX, IDC_PLUGIN1_WAV1, m_plugin1Wav1);
	DDX_Control(pDX, IDC_PLUGIN1_PLAYBTN, m_plugin1PlayBTN1);
	DDX_Control(pDX, IDC_PLUGIN1_BRSBTN, m_plugin1BrowseBTN);
	DDX_Text(pDX, IDC_EDIT1, m_pluginEditBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPluginDialog, CDialog)
	//{{AFX_MSG_MAP(CPluginDialog)
	ON_BN_CLICKED(IDC_PLUGIN1_BRSBTN, OnPlugin1Brsbtn)
	ON_BN_CLICKED(IDC_PLUGIN1_PLAYBTN, OnPlugin1Playbtn)
	ON_BN_CLICKED(IDC_PLUGIN_STOPBTN, OnPluginStopbtn)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPluginDialog message handlers

BOOL CPluginDialog::OnInitDialog() 
{

	CDialog::OnInitDialog();
	
	m_brush.CreateSolidBrush(BLACK);

	char * PluginText;

	PluginText = CGlobals::LoadText(IDR_PLUGINTEXT );
	if( PluginText != NULL )
	{
		m_pluginEditBox = PluginText;
		geRam_Free( PluginText );
		UpdateData( false );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	although I use a class called mp3Mgr, it sets up a filter graph for almost
//	any popular media file type., including wav, and avi files.

void CPluginDialog::OnPlugin1Brsbtn() 
{
	int					nItem = 0;
	int					itemtext = 0;
	LPCTSTR				wav = NULL;
	CFileDialog			m_backgrndwavFile
		(
		TRUE,
		wav,
		NULL,
		OFN_EXTENSIONDIFFERENT	| OFN_FILEMUSTEXIST ,
		_T("media files(*.avi; *.mp3; *.wav)|*.avi;*.mp3;*.wav||"));


				//	get the current dir
	char	currDir[MAX_PATH + 1];
	DWORD	dwcNameSize = MAX_PATH + 1;
	GetCurrentDirectory(dwcNameSize, currDir);
				//	se current dir as default
	m_backgrndwavFile.m_ofn.lpstrInitialDir = currDir;

				//	set the title for the fileOpen dialogbox
	static char szTitle[] = _T("ProtoGame File Opener");
	m_backgrndwavFile.m_ofn.lpstrTitle = szTitle;

				//	open the openFile dialog	
	if(IDOK !=m_backgrndwavFile.DoModal())
		return;
	m_plugin1Wav1.DeleteItem(nItem);	//	clear the list to make room for new entry
	
	POSITION pos = m_backgrndwavFile.GetStartPosition();
	while (pos)
	m_plugin1Wav1.InsertItem(m_plugin1Wav1.GetItemCount(), m_backgrndwavFile.GetNextPathName(pos));
	m_wavfilepath = m_plugin1Wav1.GetItemText( itemtext, NULL );
	
	if ((m_wavfilepath.Find("avi") != -1) || (m_wavfilepath.Find("mp3") != -1) ||
		(m_wavfilepath.Find("wav") != -1) 
		)
	{
		m_plugin1StopBTN.EnableWindow(TRUE);
		m_plugin1PlayBTN1.EnableWindow(TRUE);
	}
}



void CPluginDialog::OnPlugin1Playbtn() 
{
	/*const char**/ tempMp3file = (LPCTSTR)m_wavfilepath;

	DoBackgrndMp3(this->m_hWnd, (char*)tempMp3file, 0);
}

void CPluginDialog::OnPluginStopbtn() 
{
	if (Initialized)
	StopBackgrndMp3(this->m_hWnd);	
}


//	 not called at the moment.
bool CPluginDialog::UpdatePlugin1Dialog()
{
	if (Initialized)
	{
		if (tempMp3file != NULL)
			PlayMp3(0);
	}

	return true;
}



void CPluginDialog::OnOK() 
{
	if (Initialized)
		StopBackgrndMp3(this->m_hWnd);		// TODO: Add extra validation here

	if (m_brush.m_hObject != NULL)
		m_brush.DeleteObject();
	
	this->DestroyWindow();
	//	CDialog::OnOK();
}



HBRUSH CPluginDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
//	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		LOGBRUSH logbrush;
	switch (nCtlColor)
	{
	case CTLCOLOR_EDIT:
	case CTLCOLOR_STATIC:
	case CTLCOLOR_LISTBOX:
	case CTLCOLOR_SCROLLBAR:
	case CTLCOLOR_BTN:
	case CTLCOLOR_MSGBOX:
	case CTLCOLOR_DLG:
		{
			m_brush.GetLogBrush(&logbrush);
			pDC->SetTextColor(DKGRAY);
			pDC->SetBkColor(logbrush.lbColor);
			return m_brush;
		}
	default:
		return m_brush;
	}

//	return hbr;
//	return m_brush;
}


void CPluginDialog::OnClose() 
{
	OnOK();
	CDialog::OnClose();
}



//////////////////////////////////////////////////////////////////////
//	Initmp3Mgr
/*	This function gets the Windows Media Player and makes its functionality
	available for us. We will use mp3Mgr to access this tool.	*/ 
//////////////////////////////////////////////////////////////////////
BOOL CPluginDialog::Initmp3Mgr(HWND hWnd)
{
    // Filter interface initialize?
    if( SUCCEEDED( CoInitialize( NULL )))
		return TRUE;
	
    return FALSE;
}

//////////////////////////////////////////////////////////////////////
//	UnInitmp3Mgr
//	Unloads mp3Mgr
//////////////////////////////////////////////////////////////////////
void CPluginDialog::UnInitmp3Mgr(HWND hWnd)
{
    CoUninitialize( );

}

//////////////////////////////////////////////////////////////////////
//	mp3Mgr
//
/*	This funcion initializez mp3Mgr and connects it to the main window.
	This is necessary so that streaming audio calls will deliver audio
	to the main game window.	*/
//////////////////////////////////////////////////////////////////////
void CPluginDialog::mp3Mgr_Create(HWND hWnd)
{	
	
	// Initialise COM and the application
    Initmp3Mgr(hWnd);
	
	InitMedia(hWnd);
	
	Initialized = true;

}	// mp3Mgr


//////////////////////////////////////////////////////////////////////
// CanPlay
//
// Return true if we can go to a playing state from our current state
//////////////////////////////////////////////////////////////////////
BOOL CPluginDialog::CanPlay()
{
    return (state == Stopped || state == Paused);
}


//////////////////////////////////////////////////////////////////////
// CanStop
//
// Return true if we can go to a stopped state from our current state
//////////////////////////////////////////////////////////////////////
BOOL CPluginDialog::CanStop()
{
    return (state == Playing || state == Paused);
}


//////////////////////////////////////////////////////////////////////
// IsInitialized
//
// Return true if we have loaded and initialized a multimedia file
//////////////////////////////////////////////////////////////////////
BOOL CPluginDialog::IsInitialized()
{
    return (state != Uninitialized);
}


//////////////////////////////////////////////////////////////////////
// ChangeStateTo
//////////////////////////////////////////////////////////////////////
void CPluginDialog::ChangeStateTo( State newState )
{
    state = newState;
}

//////////////////////////////////////////////////////////////////////
// InitMedia
//
// Initialization
//////////////////////////////////////////////////////////////////////
BOOL CPluginDialog::InitMedia(HWND hWnd)
{
    ChangeStateTo( Uninitialized );
	
    hGraphNotifyEvent = NULL;
    pGraph = NULL;
	
    return TRUE;
}



//////////////////////////////////////////////////////////////////////
// CreateFilterGraph
//////////////////////////////////////////////////////////////////////
BOOL CPluginDialog::CreateMP3FilterGraph(HWND hWnd)
{
  //  IMediaEventEx *pME;					//	Event Pointer
    HRESULT hr;							//	for communicating with the filter graph
	
    pGraph = NULL;						//	make sure we start clean
	
    hr = CoCreateInstance(CLSID_FilterGraph,           // CLSID of object
		NULL,							// Outer unknown
//		CLSCTX_INPROC_SERVER,			// Type of server
		CLSCTX_INPROC,
		IID_IGraphBuilder,				// Interface wanted
		(void **) &pGraph);				// Returned object
    if (FAILED(hr))
	{
		pGraph = NULL;
		return FALSE;
    }
	
    // We use this to find out events sent by the filtergraph
	
    hr = pGraph->QueryInterface(IID_IMediaEventEx, 
		(void **) &pME);				//	open lines of communication
    if (FAILED(hr)) 
	{
		DeleteContentsMp3();
		return FALSE;
    }
										//	establish event handle
    hr = pME->GetEventHandle((OAEVENT*) &hGraphNotifyEvent);

    if (FAILED(hr)) 
	{
	  AfxMessageBox("Failed to get Event Handle in PluginDlg");

		DeleteContentsMp3();
		return FALSE;
    }

    pME->Release();
	
    return TRUE;
	
} // CreateFilterGraph


//////////////////////////////////////////////////////////////////////
// DeleteContents
//////////////////////////////////////////////////////////////////////
void CPluginDialog::DeleteContentsMp3()
{
    if (pGraph != NULL) 
	{
		pGraph->Release( );
		pGraph = NULL;
    }
	
    // this event is owned by the filter graph and is thus invalid
    hGraphNotifyEvent = NULL;
	
    ChangeStateTo( Uninitialized );
	
}	//	Delete Contents

//////////////////////////////////////////////////////////////////////
// RenderFile
// Process the file through the appropriate filter path. This function is called by
// OpenMediaFile()
//////////////////////////////////////////////////////////////////////
BOOL CPluginDialog::RenderFile( LPSTR szFileName, HWND hWnd )
{
    HRESULT hr = S_OK;
    WCHAR wPath[MAX_PATH];
	
    DeleteContentsMp3();
	
    if ( !CreateMP3FilterGraph(hWnd) ) 
	{
		return FALSE; 
    } 
	
    MultiByteToWideChar( CP_ACP, 0, szFileName, -1, wPath, MAX_PATH );
	
    hr = pGraph->RenderFile(wPath, NULL);
	
    if (FAILED( hr )) 
	{
		return FALSE;
    } 

	hr = pGraph->QueryInterface(IID_IMediaEventEx, 
		(void **) &pME);				//	open lines of communication
    if (FAILED(hr)) 
	{
			AfxMessageBox("Failed to set Query Interface pME in PluginDlg");
			return FALSE;
    }

 	pME->Release();

    return TRUE;
	
} // RenderFile


//////////////////////////////////////////////////////////////////////
// OpenMediaFile
// This function opens and renders the specified media file.
// File..Open has been selected
//////////////////////////////////////////////////////////////////////
void CPluginDialog::OpenMediaFile( HWND hWnd, LPSTR szFile )
{
    static char szFileName[ _MAX_PATH ];
	
    if( szFile != NULL && RenderFile( szFile, hWnd))	//	this calls the filter graph
    {
		LPSTR szTitle;
		
		// Work out the full path name and the file name from the
		// specified file
		GetFullPathName( szFile, _MAX_PATH, szFileName, &szTitle );
		
		// Set the main window title and update the state
		ChangeStateTo( Stopped );
		
    } 
} // OpenMediaFile


//////////////////////////////////////////////////////////////////////
// PlayMp3
//////////////////////////////////////////////////////////////////////

void CPluginDialog::PlayMp3(long volume)
{
	
    if( CanPlay() )
	{
		HRESULT hr;
		//		static IMediaControl *pMC;
		//		static IBasicAudio		*pMA;
		//		static IMediaPosition	*pMP;
		
		// Obtain the interface to our filter graph
		hr = pGraph->QueryInterface(IID_IMediaControl, 
			(void **) &pMC);
		
		if( SUCCEEDED(hr) )
		{
		/*	In order to loop sounds, we will check with Media
		Player to see when the sound is almost over. When
		it's within 0.05 sec of ending, we'll stop the sound,
		rewind it and declare that the sound is ready to play
		again. The next time around your game's main loop,
		PlayMp3 will start it up again. And so it goes...
			*/
			
			//	Set volume level
			hr = pGraph->QueryInterface(
				IID_IBasicAudio,
				(void**) &pMA);
			
			if (SUCCEEDED(hr)) 
			{							/*	Set volume. 
				-10000 is silence,	0 is full volume*/
				hr = pMA->put_Volume(volume);
				pMA->Release();	//	release the interface
			}			
			
			// Ask the filter graph to play and release the interface
			hr = pMC->Run();
			pMC->Release();
			
			if( SUCCEEDED(hr) )
			{
				return;
			}	
		}
		
	}
	
} // PlayMp3

    


//////////////////////////////////////////////////////////////////////
// StopMp3
//////////////////////////////////////////////////////////////////////
void CPluginDialog::StopMp3()
{
	HRESULT hr;
//	IMediaControl *pMC;
	
	// Obtain the interface to our filter graph
	hr = pGraph->QueryInterface(IID_IMediaControl, 
		(void **) &pMC);
	if( SUCCEEDED(hr) )
	{
		pMC->Stop(  );	//	stop it!
		pMC->Release(  );
		ChangeStateTo( Stopped );
	}
	
	return ;
} //StopMp3


//////////////////////////////////////////////////////////////////////
// GetGraphEventHandle
//
// We use this to check for graph events
//////////////////////////////////////////////////////////////////////
HANDLE CPluginDialog::GetGraphEventHandle()
{
    return hGraphNotifyEvent;
	
} // GetGraphEventHandle



//////////////////////////////////////////////////////////////////////
//	MP3PLAYING
//	This function checks to see if the current mp3 file is still playing.
//////////////////////////////////////////////////////////////////////
geBoolean CPluginDialog::Mp3Playing()
{
	HRESULT				hr;
//	IMediaPosition		*pMP;
	//	query the interface
	hr = pGraph->QueryInterface(
		IID_IMediaPosition,
		(void**) &pMP);
				if (SUCCEEDED(hr)) 
				{
					REFTIME tCurrent, tLength;		//	find the max playtime
					hr = pMP->get_Duration(&tLength);
					if (SUCCEEDED(hr)) 
					{								//	where are we now?
						hr = pMP->get_CurrentPosition(&tCurrent);
						if (SUCCEEDED(hr)) 
						{
							
							//	Test to see if there is any time left
							while ((tLength - tCurrent) > 0)
								return GE_TRUE;	// if so, still playing, buddy.
						}
						
					}

				}				//	when done playing...
				pMP->Release();	//	release our access to the interface
				return GE_FALSE;//	mp3 file is all done playing.				
}	//Mp3Playing



///////////////////////////////////////////////////////////////////////////////
//	StopBackgroundMp3
///////////////////////////////////////////////////////////////////////////////
void CPluginDialog::StopBackgrndMp3(HWND m_hWnd)
{
	if (Mp3Playing())
	{
		StopMp3();						//	kill the mp3 and free some memory
	}
		DeleteContentsMp3();				//	Delete the contents from memory
		UnInitmp3Mgr(m_hWnd);	//	Uninitialize Mp3Mgr. It's over, Man.
		Initialized = false;										//	mp3 is over
}


///////////////////////////////////////////////////////////////////////////////
//	DoBackgroundMp3
///////////////////////////////////////////////////////////////////////////////
void CPluginDialog::DoBackgrndMp3(HWND m_hWnd, char *backgrndmp3, long volume)
{
	
	if (Initialized == false)
	{
		mp3Mgr_Create(m_hWnd);	//	Initialize Mp3Mgr
	}

	OpenMediaFile(m_hWnd, backgrndmp3);	// spec your own *.mp3
	
											//	NOTE:	-10000 is silence, 0 is full volume.
											//	FALSE plays once
											//	TRUE loops continuously
	//	   vol  loop		
	PlayMp3(volume);

	return;									
}

