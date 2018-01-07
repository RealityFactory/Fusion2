/****************************************************************************************/
/*  Globals.cpp                                                                         */
/*                                                                                      */
/*  Author:       Tom Morris									                        */
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
/*	Genesis3D Version 1.1 released November 15, 1999                            */
/*	GEditPro 
/*  Copyright (C) 1999 Eclipse, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/


#include "stdafx.h"
#include "Globals.h"
#include "Ram.h"
#include "ChildFrm.h"
#include "Splash.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGlobals

	char	CGlobals::m_GlobalAppPath[MAX_PATH +1];
	char	CGlobals::m_currentPath[MAX_PATH + 1];
	DWORD	CGlobals::m_dwNameSize = MAX_PATH +1;
	CString CGlobals::m_recentCompiledLevel = _T("");
	int		CGlobals::g_iRebuildClickCount = 0;
	int		CGlobals::g_iUndoMAX = 20;
	int		CGlobals::g_iGroupVisibility = 0;
	CFont	CGlobals::m_ControlBarFont;  


	COLORREF	CGlobals::g_cViewGridColor ;
	COLORREF	CGlobals::g_cViewSnapGridColor;
	COLORREF	CGlobals::g_cViewBackgroundColor;
	COLORREF	CGlobals::g_cTreeTextColor;
	COLORREF	CGlobals::g_cListBoxTextColor;
	COLORREF	CGlobals::g_cListBoxBackColor;

	COLORREF	CGlobals::g_cActiveViewFrameColor = YELLOW;
	COLORREF	CGlobals::g_cActiveViewTextColor = CYAN;
	COLORREF	CGlobals::g_cInactiveViewTextColor = LTGRAY;
	COLORREF	CGlobals::g_cDragBoxColor;
	COLORREF	CGlobals::g_cSelectBoxColor;

	BOOL		CGlobals::g_bRebuildAlways = TRUE;


CGlobals::CGlobals()
{
}

CGlobals::~CGlobals()
{
}


BEGIN_MESSAGE_MAP(CGlobals, CWnd)
	//{{AFX_MSG_MAP(CGlobals)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGlobals message handlers



void CGlobals::PositionDialogUnderTabs( CDialog * pDlg )
{
	RECT			rect;
	RECT			TabRect ;
	CTabCtrl	*	pTC  = NULL;
	int				i ;
	int				nTabs ;

	pTC = (CTabCtrl*)pDlg->GetParent( ) ;

	nTabs = pTC->GetItemCount() ;
	TabRect.top = TabRect.left = 1000 ;
	TabRect.bottom = TabRect.right = 0 ;
	
	for( i=0; i< nTabs; i++ )
	{
		pTC->GetItemRect( i, &rect );
		if( rect.left < TabRect.left )
			TabRect.left = rect.left ;
		if( rect.right > TabRect.right )
			TabRect.right = rect.right ;
		if( rect.top < TabRect.top )
			TabRect.top = rect.top ;
		if( rect.bottom > TabRect.bottom )
			TabRect.bottom  = rect.bottom ;
	}

	pDlg->SetWindowPos
	( 
		NULL, 
		TabRect.left + GetSystemMetrics(SM_CXDLGFRAME),	
		TabRect.bottom + GetSystemMetrics(SM_CYDLGFRAME), 
		0, 0, 
		SWP_NOZORDER|SWP_NOSIZE
	) ;
}// PositionDialogUnderTabs



CGEditProDoc* CGlobals::GetActiveDocument()
{
	CWnd* pWnd = NULL;
	pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		BOOL	bSplash = FALSE;
		bSplash = pWnd->IsKindOf(RUNTIME_CLASS(CSplashWnd));
		if (bSplash)
			return NULL;
		CFrameWnd* pMainFrame = NULL;
		pMainFrame = static_cast< CFrameWnd* >( pWnd );

		if (pMainFrame)
		{
			CFrameWnd* pActiveFrame = NULL;
			pActiveFrame = pMainFrame->GetActiveFrame();
			if(pActiveFrame)
			{
				CGEditProDoc *pGenDoc = NULL;
				pGenDoc = (CGEditProDoc*)pActiveFrame->GetActiveDocument();

				return pGenDoc;
			}
			return NULL;
		}
		return NULL;
	}
	return NULL;
}



// Unnecessary. Use CGlobals::GetActiveDocument()->mpMainFrame;
CMainFrame* CGlobals::GetMainFrame()
{

	// 1 - Get a pointer to the application's
	// main frame window
	CWnd* pWnd = AfxGetMainWnd();
	if( pWnd == NULL )
		return NULL;

	// 2 - Make sure the pointer is valid and more
	// strongly typed
	ASSERT_VALID( pWnd );
	ASSERT_KINDOF( CFrameWnd, pWnd );
	CFrameWnd* pMainFrame = static_cast< CFrameWnd* >( pWnd );

return (CMainFrame*)pMainFrame;

}


BOOL CGlobals::IsKeyDown (int Key)
{
	int KeyState;

	KeyState = ::GetAsyncKeyState (Key);
	return (KeyState & 0x8000);
}



BOOL CGlobals::NewKeyDown(int Key)
{
	if (GetAsyncKeyState(Key) & 1)
			return TRUE;

	return FALSE;
}




char * CGlobals::LoadText( unsigned int resid )
{
	HGLOBAL hTextRes;
	HRSRC  hTextInfo;
	char * pTextRes;
	char * pTextString;
	int		ResSize;
	HINSTANCE hResources = 0;


	hTextInfo = FindResource( hResources, MAKEINTRESOURCE( resid ), "TEXT" );
	if( hTextInfo == NULL )
		return( NULL );

	hTextRes = LoadResource( hResources, hTextInfo );
	if( hTextRes  == NULL )
		return( NULL );
	pTextRes = (char*)LockResource( hTextRes );
	if( pTextRes  == NULL )
		return( NULL );
	ResSize = SizeofResource( hResources, hTextInfo );
	pTextString = (char*)geRam_Allocate( ResSize + 1 );
	if( pTextString == NULL )
		return( NULL );
	memcpy( pTextString, pTextRes, ResSize );
	pTextString[ResSize] = '\0';
	FreeResource( hTextRes );

	return( pTextString );
}






//	THIS FUNCTION ADDS AN EXTRA "\" TO THE PATH STRING	=======================
CString	CGlobals::AddSlash(CString tempstring)
{
	int			nIndex =1 ;
	CString		m_slash=  "\\" ;
	LPCTSTR		m_doubleslash = "\\";
	int			m_stringlength;

	m_stringlength = tempstring.GetLength();

	
	while (nIndex <  m_stringlength)
	{
		if (tempstring.GetAt(nIndex) == m_slash)
		{
		tempstring.Insert(nIndex, m_doubleslash);	//	insert a backslash
		m_stringlength = tempstring.GetLength();	//	the string just got longer
		nIndex = nIndex + 2;						//	step over the added \ 
		}
	else
		nIndex++;
	}
	 
	return tempstring;
}








// LoadBMPImage	- Loads a BMP file and creates a bitmap GDI object
//		  also creates logical palette for it.
// Returns	- TRUE for success
// sBMPFile	- Full path of the BMP file
// bitmap	- The bitmap object to initialize
// pPal		- Will hold the logical palette. Can be NULL
BOOL CGlobals::LoadBMPImage( LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal )
{
	CFile				file ;
	BITMAPFILEHEADER	bmfHeader ;	// Read file header
	WORD				nColors ;

	if( !file.Open( sBMPFile, CFile::modeRead) )
		return FALSE ;


	if( file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader) )
		return FALSE;	// File type should be 'BM'

	if( bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;

	// Get length of the remainder of the file and allocate memory
	DWORD nPackedDIBLen = file.GetLength() - sizeof(BITMAPFILEHEADER);

	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if( hDIB == 0 )
		return FALSE;	// Read the remainder of the bitmap file.

	if( file.Read((LPSTR)hDIB, nPackedDIBLen) != nPackedDIBLen )
	{
		::GlobalFree(hDIB);
		return FALSE;
	}
	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	nColors = (bmiHeader.biClrUsed)
		? (WORD) bmiHeader.biClrUsed 
		: (WORD)(1 << bmiHeader.biBitCount);

	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
			((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	// Create the logical palette
	if( pPal != NULL )
	{	// Create the palette
		if( nColors <= 256 )
		{
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
			pLP->palVersion = 0x300;
			pLP->palNumEntries = nColors;
			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
				pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}
			pPal->CreatePalette( pLP );
			delete[] pLP;
		}
	}

	CClientDC dc(NULL);
	CPalette* pOldPalette = NULL;
	if( pPal )
	{
		pOldPalette = dc.SelectPalette( pPal, FALSE );
		dc.RealizePalette();	
	}
	HBITMAP hBmp = CreateDIBitmap
	(
		dc.m_hDC,			// handle to device context
		&bmiHeader,			// pointer to bitmap size and format data
		CBM_INIT,			// initialization flag
		lpDIBBits,			// pointer to initialization data
		&bmInfo,			// pointer to bitmap color-format data
		DIB_RGB_COLORS		// color-data usage
	);
	bitmap.Attach( hBmp );
	if( pOldPalette )
		dc.SelectPalette( pOldPalette, FALSE );

	::GlobalFree(hDIB) ;
	return TRUE ;
}// LoadBMPImage


void CGlobals::TrimString( CString & String )
{
	String.TrimRight() ;
	String.TrimLeft() ;
}// TrimString


void CGlobals::TrimStringByPixelCount( HDC hDC, char * psz, const int nMaxPixels )
{
	int		nLen = strlen( psz ) ;
	SIZE	size ;

	do
	{
		::GetTextExtentPoint32( hDC, psz, nLen, &size ) ;
		if( size.cx > nMaxPixels )
			psz[--nLen] = 0 ;

	}while( (size.cx > nMaxPixels) && (nLen>0) ) ;
}// TrimStringByPixelCount



///////////////////////////////////////////////////////////////////////
//	DrawLine
///////////////////////////////////////////////////////////////////////
HDC CGlobals::DrawLine(HDC hDC, int style, 
						int width, COLORREF color,
						int xbegin, int ybegin, 
						int xend, int yend)
{
	HPEN pen = CreatePen(style, width, color);
	SelectObject(hDC, pen);
	MoveToEx(hDC, xbegin, ybegin, NULL);
	LineTo(hDC, xend, yend);
	DeleteObject(pen);
	return hDC;
}

///////////////////////////////////////////////////////////////////////
//	DrawCircle
///////////////////////////////////////////////////////////////////////
HDC CGlobals::DrawCircle(HDC hDC, COLORREF color,
							int left, int top,
							int right, int bottom)
{
	HPEN pen = CreatePen(PS_SOLID, 1, BLACK);
	HBRUSH brush = CreateSolidBrush(color);

	SelectObject(hDC, pen);
	SelectObject(hDC, brush);
	Ellipse(hDC, left, top, right, bottom);
	if (!DeleteObject(pen))
	{	
		Sleep(500);
		if (!DeleteObject(pen))
		{
			AfxMessageBox("Failed to delete DrawCircle pen");
			_exit(-1);
		}
	}
	if (!DeleteObject(brush))
	{
		Sleep(500);
		if (!DeleteObject(brush))
		{
			AfxMessageBox("Failed to delete DrawCircle brush");
			_exit(-1);
		}
	}

return hDC;
}

///////////////////////////////////////////////////////////////////////
//	FlashLight
///////////////////////////////////////////////////////////////////////
void CGlobals::FlashingLight(CDC *pDC, COLORREF bright_color,
							 COLORREF dark_color,
							int left, int top,
							int right, int bottom)
{
	CPen		pen;
	CBrush		bright_brush, dark_brush;
	pen.CreatePen(PS_SOLID, 1, BLACK);
	bright_brush.CreateSolidBrush(bright_color);
	dark_brush.CreateSolidBrush(dark_color);

	pDC->SelectObject(pen);
	pDC->SelectObject(bright_brush);
	pDC->Ellipse(left, top, right, bottom);
	ReleaseDC(pDC);

	Sleep(50);
	pDC = GetDC();
	bright_brush.DeleteObject();
	pDC->SelectObject(dark_brush);
	pDC->Ellipse(left, top, right, bottom);
	pen.DeleteObject();
	dark_brush.DeleteObject();
	ReleaseDC(pDC);
return;
}

