/****************************************************************************************/
/*  ChildFrm.cpp                                                                        */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird                                                */
/*  Description:  Frame and splitter window code                                        */
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
#include "ChildFrm.h"
#include "GEditProView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


	BEGIN_MESSAGE_MAP(CFixedSplitterWnd, CSplitterWnd)
#if (_MFC_VER == 0x0421)
		//{{AFX_MSG_MAP(CFixedSplitterWnd)
		//}}AFX_MSG_MAP
#endif
 	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_MDIACTIVATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	InitDone = FALSE;
	
	m_bResizingNow = false;
	m_bResizePanes = false;
	m_bResizerInitilaized = false;

	m_iOldX0 = 0;
	m_iOldX1 = 0;
	m_iOldY0 = 0;
	m_iOldY1 = 0;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//Create maximized
	cs.style |= WS_MAXIMIZE|WS_VISIBLE;
	return CMDIChildWnd::PreCreateWindow(cs);
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext)
{
	int x, y;
	RECT rect;

	if (!m_wndSplitter.CreateStatic(this, 2, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	GetClientRect(&rect);

	m_clientSizeX = rect.right;
	m_clientSizeY = rect.bottom;

	x = (int)(rect.right * .5f);
	y = (int)(rect.bottom * .5f);

	if (!m_wndSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CGEditProView), CSize(x, y), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CGEditProView), CSize(x, y), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(1, 0,
		RUNTIME_CLASS(CGEditProView), CSize(x, y), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(1, 1,
		RUNTIME_CLASS(CGEditProView), CSize(x, y), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	//set the view types
	((CGEditProView *)m_wndSplitter.GetPane(0,0))->OnViewType(ID_VIEW_TEXTUREVIEW);
	((CGEditProView *)m_wndSplitter.GetPane(0,1))->OnViewType(ID_VIEW_TOPVIEW);
	((CGEditProView *)m_wndSplitter.GetPane(1,0))->OnViewType(ID_VIEW_FRONTVIEW);
	((CGEditProView *)m_wndSplitter.GetPane(1,1))->OnViewType(ID_VIEW_SIDEVIEW);

	// activate the top left view
	SetActiveView((CView*)m_wndSplitter.GetPane(0,1));

	m_bResizePanes = true;

	InitDone=TRUE;

	SetTimer(IDT_TIMER_CHILDFRAME, 100, NULL);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
						
#pragma warning (disable: 4100)
LRESULT CChildFrame::OnSetText(WPARAM wParam, LPARAM lParam)
{
//	const char *Text = (const char *)lParam;
//	if( Text[0] == '-' ) {
		return Default();
//	}
//	return 1;
}
#pragma warning (default: 4100)





//	this handles splitter window resizing -- to keep the panes the
//	same proportion as they were before resizing started
void CChildFrame::OnTimer(UINT nIDEvent) 
{
	int		min;
	int		iCurX0, iCurX1, iCurY0, iCurY1;
	RECT	newRect;

	float	fOldX0, fOldX1, fOldY0, fOldY1;
	float	fCurY0, fCurY1;
	float	fCurX0, fCurX1;

	CGEditProDoc *pDoc = NULL;
	CMainFrame *pMainFrame = NULL;

	if ((!m_bResizerInitilaized) && (m_bResizePanes)) 
	{
		RestoreAllPanes();

		m_wndSplitter.GetRowInfo (0, iCurY0, min);
		m_wndSplitter.GetRowInfo (1, iCurY1, min);

		m_wndSplitter.GetColumnInfo (0, iCurX0, min);
		m_wndSplitter.GetColumnInfo (1, iCurX1, min);

		CMDIChildWnd::GetClientRect(&newRect);

		m_iOldX0 = iCurX0;
		m_iOldX1 = iCurX1;
		m_iOldY0 = iCurY0;
		m_iOldY1 = iCurY1;

		m_clientSizeX = newRect.right;
		m_clientSizeY = newRect.bottom;
	
		m_bResizerInitilaized = true;
	}

	if (nIDEvent == IDT_TIMER_CHILDFRAME)
	{
		if (!m_bResizingNow && (GetCapture() != this))
		{
			CMDIChildWnd::GetClientRect(&newRect);

			m_wndSplitter.GetRowInfo (0, iCurY0, min);
			m_wndSplitter.GetRowInfo (1, iCurY1, min);

			m_wndSplitter.GetColumnInfo (0, iCurX0, min);
			m_wndSplitter.GetColumnInfo (1, iCurX1, min);

			if (newRect.right != m_clientSizeX ||
				newRect.bottom != m_clientSizeY)
			{
				fCurY0 = iCurY0;
				fCurY1 = iCurY1;
				fCurX0 = iCurX0;
				fCurX1 = iCurX1;

				fOldX0 = m_iOldX0;
				fOldX1 = m_iOldX1;
				fOldY0 = m_iOldY0;
				fOldY1 = m_iOldY1;

				float	fTemp0, fTemp1;
				fTemp0 = (fCurY0+fCurY1)*(fOldY0/(fOldY0+fOldY1));
				fTemp1 = (fCurY0+fCurY1)*(fOldY1/(fOldY0+fOldY1));
				fCurY0 = fTemp0;
				fCurY1 = fTemp1;

				fTemp0 = (fCurX0+fCurX1)*(fOldX0/(fOldX0+fOldX1));
				fTemp1 = (fCurX0+fCurX1)*(fOldX1/(fOldX0+fOldX1));
				fCurX0 = fTemp0;
				fCurX1 = fTemp1;

				m_wndSplitter.SetRowInfo (0, fCurY0, 0);
				m_wndSplitter.SetRowInfo (1, fCurY1, 0);

				m_wndSplitter.SetColumnInfo (0, fCurX0, 0);
				m_wndSplitter.SetColumnInfo (1, fCurX1, 0);

				m_wndSplitter.RecalcLayout();
			}

			m_wndSplitter.GetRowInfo (0, iCurY0, min);
			m_wndSplitter.GetRowInfo (1, iCurY1, min);

			m_wndSplitter.GetColumnInfo (0, iCurX0, min);
			m_wndSplitter.GetColumnInfo (1, iCurX1, min);

			m_iOldX0 = iCurX0;
			m_iOldX1 = iCurX1;
			m_iOldY0 = iCurY0;
			m_iOldY1 = iCurY1;

			m_clientSizeX = newRect.right;
			m_clientSizeY = newRect.bottom;
		}
	}

	CMDIChildWnd::OnTimer(nIDEvent);
}


void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	//	NOTE: All adjustments to the splitterwnds are done in the OnTimer().
	
	//	The OnTimer() checks this flag. If resizing is in progress, it attempts no
	//	splitterwnd adjustment.
	//	If you try to do it ihere, you get unreliable and changing splitter pane
	//	rect info that results in annoying flicker and crazy moments of the
	//	splitter bars.
	m_bResizingNow = true;	

	// Preserve the normal window resizing call.
	CMDIChildWnd::OnSize(nType, cx, cy);

	//	Now the OnTimer() can make adjustments to the splitterwnds
	m_bResizingNow = false;

	return;	
}


void CChildFrame::OnSetFocus(CWnd* pOldWnd) 
{
	NMHDR hdr;
	CWnd *pParent = NULL;

	CMDIChildWnd::OnSetFocus(pOldWnd);

	// notify main frame window that we've been given the focus
	hdr.hwndFrom = this->m_hWnd;
	hdr.idFrom = 0;
	hdr.code = WM_SETFOCUS;
	pParent = this->GetMDIFrame ();
	if (pParent)
	{
		pParent->SendMessage (WM_NOTIFY, 0, (LPARAM)&hdr);
	}

}


CFlatSplitterWnd *CChildFrame::GetSplitterWnd()
{
	return &m_wndSplitter;
}



///////////////////////////////////////////////////////////////////////////////
//	MaximizeUpperLeftPane
//	
///////////////////////////////////////////////////////////////////////////////
bool CChildFrame::MaximizeUpperLeftPane()
{
	HWND hWnd = NULL;
	hWnd = m_wndSplitter.GetSafeHwnd();
	if (hWnd == NULL || !::IsWindow(hWnd))
		return false;

	CRect	rect;
	GetClientRect(&rect);
	CSize size = rect.Size();
	
	m_wndSplitter.SetColumnInfo(0, size.cx, 0);
	m_wndSplitter.SetColumnInfo(1, 0, 0);
	m_wndSplitter.SetRowInfo(0, size.cy, 0);
	m_wndSplitter.SetRowInfo(1, 0, 0);

	m_wndSplitter.RecalcLayout();

	return true;
}



///////////////////////////////////////////////////////////////////////////////
//	MaximizeUpperRightPane
//	
///////////////////////////////////////////////////////////////////////////////
bool CChildFrame::MaximizeUpperRightPane()
{
	HWND hWnd = NULL;
	hWnd = m_wndSplitter.GetSafeHwnd();
	if (hWnd == NULL || !::IsWindow(hWnd))
		return false;

	CRect	rect;
	GetClientRect(&rect);
	CSize size = rect.Size();
	
	m_wndSplitter.SetColumnInfo(0, 0, 0);
	m_wndSplitter.SetColumnInfo(1, size.cx, 0);
	m_wndSplitter.SetRowInfo(0, size.cy, 0);
	m_wndSplitter.SetRowInfo(1, 0, 0);

	m_wndSplitter.RecalcLayout();

	return true;
}



///////////////////////////////////////////////////////////////////////////////
//	MaximizeLowerLeftPane
//	
///////////////////////////////////////////////////////////////////////////////
bool CChildFrame::MaximizeLowerLeftPane()
{
	HWND hWnd = NULL;
	hWnd = m_wndSplitter.GetSafeHwnd();
	if (hWnd == NULL || !::IsWindow(hWnd))
		return false;

	CRect	rect;
	GetClientRect(&rect);
	CSize size = rect.Size();
	
	m_wndSplitter.SetColumnInfo(0, size.cx, 0);
	m_wndSplitter.SetColumnInfo(1, 0, 0);
	m_wndSplitter.SetRowInfo(0, 0, 0);
	m_wndSplitter.SetRowInfo(1, size.cy, 0);

	m_wndSplitter.RecalcLayout();

	return true;
}



///////////////////////////////////////////////////////////////////////////////
//	MaximizeLowerRightPane
//	
///////////////////////////////////////////////////////////////////////////////
bool CChildFrame::MaximizeLowerRightPane()
{
	HWND hWnd = NULL;
	hWnd = m_wndSplitter.GetSafeHwnd();
	if (hWnd == NULL || !::IsWindow(hWnd))
		return false;

	CRect	rect;
	GetClientRect(&rect);
	CSize size = rect.Size();
	
	m_wndSplitter.SetColumnInfo(0, 0, 0);
	m_wndSplitter.SetColumnInfo(1, size.cx, 0);
	m_wndSplitter.SetRowInfo(0, 0, 0);
	m_wndSplitter.SetRowInfo(1, size.cy, 0);

	m_wndSplitter.RecalcLayout();

	return true;
}


///////////////////////////////////////////////////////////////////////////////
//	RestoreAllPanes
//	
///////////////////////////////////////////////////////////////////////////////
bool CChildFrame::RestoreAllPanes()
{
	HWND hWnd = NULL;
	hWnd = m_wndSplitter.GetSafeHwnd();
	if (hWnd == NULL || !::IsWindow(hWnd))
		return false;

	int correctedHeight;
	CRect	propPanelRect, childFrameRect;

	GetClientRect(&childFrameRect);

	correctedHeight = childFrameRect.Height();
	
	m_wndSplitter.SetColumnInfo(0, childFrameRect.Width()/2, 0);
	m_wndSplitter.SetColumnInfo(1, childFrameRect.Width()/2, 0);
	m_wndSplitter.SetRowInfo(0, correctedHeight/2, 0);
	m_wndSplitter.SetRowInfo(1, correctedHeight/2, 0);

	m_wndSplitter.RecalcLayout();
	
	return true;
}



void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	// TODO: Add your message handler code here

	//	reset the ViewMgr Dialog when switching between open documents
	CMainFrame *pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (pMainFrame)
		pMainFrame->ResetViewMgrDialog();
}


void CChildFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

	InitDone = FALSE;
	m_bResizerInitilaized = false;
	
	CMDIChildWnd::OnClose();
}
