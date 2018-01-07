/****************************************************************************************/
/*  Globals.h                                                                           */
/*                                                                                      */
/*  Author:       Tom Morris                                                            */
/*	Description:  Global vars, defines and globally-useful functions                    */
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
/*  Copyright (C) 1999 Eclipse, Inc. All Rights Reserved                                */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002                          */
/****************************************************************************************/



#if !defined(AFX_GLOBALS_H__8341BC43_95F0_11D4_9554_444553540000__INCLUDED_)
#define AFX_GLOBALS_H__8341BC43_95F0_11D4_9554_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Globals.h : header file
//

#include <genesis.h>
#include "resource.h"

#include "GEditProDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CGlobals window

const COLORREF CLOUDBLUE = RGB(128, 184, 223);
const COLORREF WHITE = RGB(255, 255, 255);
const COLORREF BLACK = RGB(1, 1, 1);
const COLORREF DKGRAY = RGB(128, 128, 128);
const COLORREF LTGRAY = RGB(205, 205, 205);
const COLORREF YELLOW = RGB(255, 255, 0);
const COLORREF DKYELLOW = RGB(128, 128, 0);
const COLORREF ORANGE = RGB(255,128,77);
const COLORREF RED = RGB(255, 0, 0);
const COLORREF DKRED = RGB(128, 0, 0);
const COLORREF DEEPRED = RGB(40,0,0);
const COLORREF BLUE = RGB(0, 0, 255);
const COLORREF LTBLUE = RGB(100, 100, 255);
const COLORREF DKBLUE = RGB(0, 0, 128);
const COLORREF CYAN = RGB(0, 255, 255);
const COLORREF DKCYAN = RGB(0, 128, 128);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF DKGREEN = RGB(0, 128, 0);
const COLORREF MAGENTA = RGB(255, 0, 255);
const COLORREF DKMAGENTA = RGB(128, 0, 128);
const COLORREF BUTTONBACKGROUND = RGB(26, 95, 103);
const COLORREF DIALOGBACKGREEN = RGB(165, 175, 165);
const COLORREF DIALOGBACKBLUE = RGB(198,198,210);
const COLORREF DIALOGBACKCYAN = RGB(178,196,196);
const COLORREF DIALOGBACKMAGENTA = RGB(223,219,223);
const COLORREF DIALOGBACKRED = RGB(214,194,194);
const COLORREF DIALOGBACKYELLOW = RGB(196,196,178);
const COLORREF MAINTEXT	= RGB(204, 204, 153);
const COLORREF TEMPLATEBLUE = RGB(155, 155, 255);

const	COLORREF	BACKGROUND = DKGRAY;
const	COLORREF	GRID = LTGRAY;	
const	COLORREF	HIGHLIGHT = YELLOW; 
const	COLORREF	TEXT = BLACK;
const	COLORREF	REFLINE = LTBLUE;


#define TEXTURE_VIEW_ROW	0
#define	TEXTURE_VIEW_COL	0
#define TOP_VIEW_ROW		0
#define TOP_VIEW_COL		1
#define FRONT_VIEW_ROW		1
#define FRONT_VIEW_COL		0
#define SIDE_VIEW_ROW		1
#define SIDE_VIEW_COL		1

#define RAD(a) ((a)*0.0174524064372835128194189785163162f)


#define BRUSH_MIN			1
#define BRUSH_MAX			4000
#define WALL_THICK_MIN		1
#define WALL_THICK_MAX		32
#define ARCH_THICK_MIN		1
// changed QD 11/03
#define ARCH_THICK_MAX		4000
#define ARCH_ANGLE_MIN		0
#define	ARCH_ANGLE_MAX		360


class CGlobals : public CWnd
{
// Construction
public:
	CGlobals();

// Attributes
public:

// Operations
public:
	static CFont				m_ControlBarFont;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlobals)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGlobals();

	static	void		PositionDialogUnderTabs( CDialog * pDlg ) ;
	static	CGEditProDoc* GetActiveDocument();
	static	CMainFrame* GetMainFrame();
	static	BOOL		IsKeyDown(int Key);
	static	BOOL		NewKeyDown(int Key);
	static	BOOL		LoadBMPImage( LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal ) ;
	static	void		TrimStringByPixelCount( HDC hDC, char * psz, const int nMaxPixels ) ;
	static	void		TrimString( CString &String ) ;
	static	char		*LoadText( unsigned int resid );
	static	CString		AddSlash(CString tempstring);
		HDC			DrawLine(HDC hDC, int style, int width, COLORREF color,
							int xbegin, int ybegin, int xend, int yend);
		HDC			DrawCircle(HDC hDC, COLORREF color, int left, int top,
							int right, int bottom);
		void		FlashingLight(CDC *pDC, COLORREF bright_color, COLORREF dark_color,
							int left, int top, int right, int bottom);

	static	char	m_GlobalAppPath[MAX_PATH +1];
	static	char	m_currentPath[MAX_PATH + 1];
	static	DWORD	m_dwNameSize;
	static	CString m_recentCompiledLevel;
	static	int		g_iRebuildClickCount, g_iUndoMAX;
	static	int		g_iGroupVisibility;

	static		COLORREF	g_cViewGridColor,
							g_cViewSnapGridColor,
							g_cViewBackgroundColor,
							g_cTreeTextColor,
							g_cListBoxTextColor,
							g_cListBoxBackColor,
							
							g_cActiveViewFrameColor,
							g_cActiveViewTextColor,
							g_cInactiveViewTextColor,
							g_cDragBoxColor,
							g_cSelectBoxColor;

	static	BOOL	g_bRebuildAlways;

	// Generated message map functions
protected:
	//{{AFX_MSG(CGlobals)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBALS_H__8341BC43_95F0_11D4_9554_444553540000__INCLUDED_)
