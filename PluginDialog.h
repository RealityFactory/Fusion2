/****************************************************************************************/
/*  PluginDialog.h																		*/
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


//{{AFX_INCLUDES()

//}}AFX_INCLUDES
#if !defined(AFX_PLUGINDIALOG_H__2B128520_C0B3_11D4_9554_444553540000__INCLUDED_)
#define AFX_PLUGINDIALOG_H__2B128520_C0B3_11D4_9554_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PluginDialog.h : header file
#include "StdAfx.h"
#include <dshow.h>

//#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CPluginDialog dialog

class CPluginDialog : public CDialog
{
// Construction
public:
	CPluginDialog(CWnd* pParent = NULL);   // standard constructor

	bool UpdatePlugin1Dialog();

// Dialog Data
	//{{AFX_DATA(CPluginDialog)
	enum { IDD = IDD_PLUGIN1_DIALOG };
	CButton	m_plugin1StopBTN;
	CListCtrl	m_plugin1Wav1;
	CButton	m_plugin1PlayBTN1;
	CButton	m_plugin1BrowseBTN;
	CString	m_pluginEditBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluginDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	const char* tempMp3file;
	CString		m_wavfilepath;
	CBrush		m_brush;
	// Generated message map functions
	//{{AFX_MSG(CPluginDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPlugin1Brsbtn();
	afx_msg void OnPlugin1Playbtn();
	afx_msg void OnPluginStopbtn();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//	DEFINES	AND DECLARATIONS ============================================

typedef enum tagState {Uninitialized, Stopped, Paused, Playing } State;

State				state;
IGraphBuilder		*pGraph;
HANDLE				hGraphNotifyEvent;
HWND				m_hWnd;
bool				Initialized;

	IMediaSeeking	*pMS;
	IMediaControl	*pMC;
	IBasicAudio		*pMA;
	IMediaPosition	*pMP;
	IMediaEventEx	*pME;
	IBasicVideo		*pBV;




//	FUNCTION PROTOTYPES	=================================================


BOOL				Initmp3Mgr(HWND hWnd);
void				UnInitmp3Mgr(HWND hWnd);
void				mp3Mgr_Create(HWND hWnd);
BOOL				InitMedia(HWND hWnd);
void				OpenMediaFile(HWND hWnd, LPSTR szFile );
HANDLE				GetGraphEventHandle( void );

BOOL				CreateMP3FilterGraph(HWND hWnd);
BOOL				RenderFile( LPSTR szFileName, HWND hWnd);

BOOL				CanPlay();
BOOL				CanStop();
BOOL				CanPause();
BOOL				IsInitialized();
void				DeleteContentsMp3();

// Event handlers
void				PlayMp3(long volume);
void				OnMediaPause();
void				StopMp3();
void				OnMediaAbortStop();
BOOL				Mp3Playing();
void				ChangeStateTo( State newState );
//void				StopBackgrndMp3();
void				StopBackgrndMp3(HWND m_hWnd);
void				DoBackgrndMp3(HWND m_hWnd, char *backgrndmp3, long volume);



};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINDIALOG_H__2B128520_C0B3_11D4_9554_444553540000__INCLUDED_)
