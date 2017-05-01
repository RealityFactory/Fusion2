#include "stdafx.h"
#include "capview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC (CCaptionView, CView)

#define new DEBUG_NEW

#define LEFT_MARGIN 2

BEGIN_MESSAGE_MAP(CCaptionView, CView)
    //{{AFX_MSG_MAP(CCaptionView)
    ON_MESSAGE (WM_NCCALCSIZE, OnNcCalcSize)
    ON_WM_NCPAINT()
    ON_WM_NCHITTEST()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/********************************************************/
// OnActivateView - MFC virtual function called whenever
//    the view loses or gains activation.
// Force repaint of caption, and then call base class.
/********************************************************/

void CCaptionView::OnActivateView (BOOL bActivate,
    CView *pActivateView, CView *pDeactivateView)
{
    m_bActive = bActivate;
    if ((m_nCaptionHeight > 0) && (::IsWindow (m_hWnd))) 
        SendMessage (WM_NCPAINT);
    CView::OnActivateView (bActivate, pActivateView,
        pDeactivateView);
}

/********************************************************/
// OnActivateFrame - MFC virtual function called whenever
//      the MDI child frame activation state changes.
// Parameter: nState indicates the state.
// If state is inactive, send a WM_NCPAINT after setting
// m_bActive. If state is active, don't do anything,
// because you will get a OnActivateView as well for
// the active view.
/********************************************************/

void CCaptionView::OnActivateFrame (UINT nState,
    CFrameWnd* pFrameWnd)
{
    m_bActive = (nState == WA_INACTIVE) ? FALSE : TRUE;
    if(nState == WA_INACTIVE) {
        if((m_nCaptionHeight > 0) && (::IsWindow (m_hWnd))) 
            SendMessage (WM_NCPAINT);
    }
    CView::OnActivateFrame(nState, pFrameWnd);
}

/********************************************************/
// OnNcCalcSize - MFC handler for WM_NCCALCSIZE (read
// documentation on message). This message is used to
// calculate the client area of a window. Call Default
// first to get the actual client area, and then return
// an area minus the caption area.
/********************************************************/

LRESULT CCaptionView::OnNcCalcSize(WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = Default ();
    if (m_nCaptionHeight > 0) {
        NCCALCSIZE_PARAMS FAR* lpncsp =
            (NCCALCSIZE_PARAMS FAR *)lParam;
        lpncsp->rgrc [0].top += m_nCaptionHeight;
    }
    return lResult;
}

/********************************************************/
// OnNcPaint - MFC handler for WM_NCPAINT
// No parameters
// Draw the caption in the non-client area. The color
// depends on whether is active or not. Some points to be
// noted in this function: Use of GetWindowDC and using
// a coordinate system wrt top-left of non-client area
// rather than client coordinates. Calls out to virtual
// function DoDrawCaption to do the actual drawing.
/********************************************************/

void CCaptionView::OnNcPaint() 
{
    Default ();        // Draw other parts as normal.
    if (m_nCaptionHeight <= 0) // Optimize if Caption is off
        return;
    
// Get the rectangle to paint. We use GetWindowRect and
// then shift to window based (non-client) coordinates.
// Effectively, we are only using the width of the window,
// other values are set explicitly.

    RECT rect, windowRect;
    GetWindowRect (&windowRect); 
    SetRect (&rect, 0, 0, windowRect.right - windowRect.left,
                                m_nCaptionHeight + 1);

// Get the window dc. Remember we are painting the
// non-client area.
    CDC *pDC = GetWindowDC ();
    if (pDC) {
        DoDrawCaption (pDC, rect); // Call out to virtual fn.
        ReleaseDC (pDC);
    }
    else {            // Could not get DC!
        TRACE0 ("CCaptionView::OnNcPaint - "
                "GetWindowDC failed!.\n");
        ASSERT (FALSE);
    }
}

/********************************************************/
// OnNcHitTest - MFC handler for WM_NCHITTEST
// Parameters: mouse at screen coordinates.
// Call Default first. If mouse is HTNOWHERE, return
// HTCLIENT. If this is not done, clicking on the
// pseudo-caption will not activate this view.
/********************************************************/

UINT CCaptionView::OnNcHitTest (CPoint point)
{
    LRESULT lResult = Default ();
    // Optimize if Caption is off
    if ((m_nCaptionHeight > 0) && (lResult == HTNOWHERE))
        lResult = HTCLIENT;
    return LOWORD (lResult);
}

/********************************************************/
// DoDrawCaption - virtual function to draw caption.
// Parameters: Pointer to DC to draw on, rectangle to
// indicate area. 
// Point is wrt to 0,0 of window (not client). This version
// draws the caption in the active caption color, and 
// prints out a caption as well. Override this function
// to extend draw capabilities. m_bActive tells you whether
// to draw in active colors, or inactive colors.
/********************************************************/

void CCaptionView::DoDrawCaption (CDC *pDC,
        const RECT& rect)
{
    CFrameWnd *pFrame = (CFrameWnd *)GetParentFrame ();
    ASSERT (pFrame->IsKindOf (RUNTIME_CLASS (CFrameWnd)));

// Decide color on whether the view is currently active.    
    COLORREF color = (m_bActive) ?
            GetSysColor (COLOR_ACTIVECAPTION) : 
            GetSysColor (COLOR_INACTIVECAPTION);

    CPen blackPen (PS_SOLID, 1, RGB (0, 0, 0));
    CBrush blueBrush (color);

// Select pen and brush and draw a rectangle.

    CPen *pOldPen = pDC->SelectObject (&blackPen);
    CBrush *pOldBrush = pDC->SelectObject (&blueBrush);
    pDC->Rectangle (&rect);

// Now, write the caption.

    pDC->SetTextColor ((m_bActive) ?
                GetSysColor (COLOR_CAPTIONTEXT) : 
                GetSysColor (COLOR_INACTIVECAPTIONTEXT));
    pDC->SetBkMode (TRANSPARENT);
    
    RECT rectText = rect;
    rectText.left += LEFT_MARGIN; // looks better.

//    CFont *pFont = GetFont ();        
//    if (pFont)
//      pDC->SelectObject (pFont);
		
		CFont *OldFont = pDC->SelectObject(&CaptionFont);
    pDC->DrawText (m_sCaption, m_sCaption.GetLength (),
        &rectText, DT_SINGLELINE | DT_VCENTER);
		pDC->SelectObject(OldFont);

    pDC->SelectObject (pOldBrush);
    pDC->SelectObject (pOldPen);
}
