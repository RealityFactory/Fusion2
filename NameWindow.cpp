// NameWindow.cpp : implementation file
//

#include "stdafx.h"
#include "fusion.h"
#include "NameWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameWindow

CNameWindow::CNameWindow()
{
}

CNameWindow::~CNameWindow()
{
}


BEGIN_MESSAGE_MAP(CNameWindow, CWnd)
	//{{AFX_MSG_MAP(CNameWindow)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNameWindow message handlers

void CNameWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CFont* pOldFont = dc.SelectObject(&m_Font);
	
	CRect r;
	GetClientRect(&r);
	
	dc.SetTextColor(RGB(0,0,0));
	dc.SetBkColor(RGB(255,255,231));

	dc.FillSolidRect(&r, RGB(255,255,231) );

	dc.DrawText( m_strName, &r, DT_CENTER|DT_VCENTER);

	dc.SelectObject(pOldFont);

	// Do not call CWnd::OnPaint() for painting messages
}


void CNameWindow::Show(const char* pstr)
{

	m_strName = pstr; 
	
	if(!m_strName.IsEmpty())
	{
		
		CDC* pDC = GetDC();
		if(!pDC)
			return;

	
		CFont* pOldFont = pDC->SelectObject(&m_Font);
		CSize sz = pDC->GetTextExtent( m_strName );
		pDC->SelectObject(pOldFont);

		CPoint pt;
		GetCursorPos(&pt);

		CRect r;
		r.left = pt.x;
		r.top = pt.y;
		r.right = pt.x + sz.cx + 8;
		r.bottom = pt.y + sz.cy + 4;

		MoveWindow( &r );


		ShowWindow(SW_SHOW);

//		Invalidate(); 
//		UpdateWindow();

	}
	

}

BOOL CNameWindow::PreCreateWindow(CREATESTRUCT& cs) 
{

	m_Font.CreatePointFont(8, "MS Sans Serif");

	return CWnd::PreCreateWindow(cs);
}
