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
/*  Copyright (C) 1996-1999 Eclipse Entertainment, L.L.C. All Rights Reserved           */
/*                                                                                      */
/****************************************************************************************/
#ifndef CHILDFRM_H
#define CHILDFRM_H

#include "resource.h"

// CFixedSplitterWnd fixes WM_MOUSEWHEEL crash in MFC 0x0421
class CFixedSplitterWnd : public CSplitterWnd
{
// Construction
public:
	CFixedSplitterWnd();
	void SetHighlight();
// Attributes
public:
	HWND m_hwndAct;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixedSplitterWnd)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFixedSplitterWnd();
	void RefreshSplitBars();
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg);
	// Generated message map functions
protected:

	//{{AFX_MSG(CFixedSplitterWnd)
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
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

	CFixedSplitterWnd m_wndSplitter;

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	BOOL	InitDone;
};

/////////////////////////////////////////////////////////////////////////////
#endif
