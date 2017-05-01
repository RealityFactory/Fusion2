/****************************************************************************************/
/*  ChildFrm.h                                                                          */
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
/*  Genesis3D Version 1.1 released November 15, 1999                                 */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/
#ifndef CHILDFRM_H
#define CHILDFRM_H

#pragma once


#include "FlatSplitterWnd.h"



// CFixedSplitterWnd fixes WM_MOUSEWHEEL crash in MFC 0x0421
class CFixedSplitterWnd : public CSplitterWnd
{
	//{{AFX_MSG(CFixedSplitterWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// child frame class
class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual ~CChildFrame();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	bool	MaximizeUpperLeftPane();
	CSize	GetUpperLeftPaneSize();

	bool	MaximizeUpperRightPane();
	CSize	GetUpperRightPaneSize();

	bool	MaximizeLowerLeftPane();
	CSize	GetLowerLeftPaneSize();

	bool	MaximizeLowerRightPane();
	CSize	GetLowerRightPaneSize();

	bool	RestoreAllPanes();

	CFlatSplitterWnd	*GetSplitterWnd();





// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG

//	CFixedSplitterWnd m_wndSplitter;

	CFlatSplitterWnd m_wndSplitter;


	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	BOOL	InitDone;

	int			m_iOldX0;
	int			m_iOldX1;
	int			m_iOldY0;
	int			m_iOldY1;

	bool		m_bResizingNow;
	bool		m_bResizerInitilaized;

	bool		m_bResizePanes;					// Set to true once the splitter bars are done loading.
	int			m_clientSizeX;
	int			m_clientSizeY;
};

/////////////////////////////////////////////////////////////////////////////
#endif
