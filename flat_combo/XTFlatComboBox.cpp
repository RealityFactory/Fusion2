// XTFlatComboBox.cpp : implementation of the CXTFlatComboBox class.
//
// This file is a part of the Xtreme Toolkit for MFC.
// ©1998-2001 Codejock Software, All Rights Reserved.
//
// This source code can only be used under the terms and conditions 
// outlined in the accompanying license agreement.
//
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

#include "..\StdAfx.h"
#include "XTFlatComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTFlatComboBox
/////////////////////////////////////////////////////////////////////////////
CXTFlatComboBox::CXTFlatComboBox()
{
	m_bDisableAC   = FALSE;
    m_bFlatLook    = TRUE;
    m_bLBtnDown    = FALSE;
    m_bPainted     = FALSE;
    m_bHasFocus    = FALSE;
    m_bAutoComp    = FALSE;
    m_clr3DHilight = ::GetSysColor(COLOR_3DHILIGHT);
    m_clr3DShadow  = ::GetSysColor(COLOR_3DSHADOW);
    m_clr3DFace    = ::GetSysColor(COLOR_3DFACE);
    m_cxHThumb     = ::GetSystemMetrics(SM_CXHTHUMB);
}

CXTFlatComboBox::~CXTFlatComboBox()
{

}

IMPLEMENT_DYNAMIC(CXTFlatComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CXTFlatComboBox, CComboBox)
    //{{AFX_MSG_MAP(CXTFlatComboBox)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetFocus)
    ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillFocus)
    ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXTFlatComboBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (message == WM_MOUSEMOVE)
	{
		CRect rcWindow;
		
		GetWindowRect (&rcWindow);
		
		if (rcWindow.PtInRect(GetCurrentMessage()->pt))
		{
			if (m_bFlatLook)
			{
				SetTimer(1,10,NULL);
				OnTimer(1);
			}
		}
	}
	
	return CComboBox::OnSetCursor(pWnd, nHitTest, message);
}

void CXTFlatComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if (m_bFlatLook) {
        m_bLBtnDown = TRUE;
    }

    CComboBox::OnLButtonDown(nFlags, point);
}

void CXTFlatComboBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if (m_bFlatLook)
	{
        m_bLBtnDown = FALSE;
        Invalidate();
    }

    CComboBox::OnLButtonUp(nFlags, point);
}

void CXTFlatComboBox::OnTimer(UINT nIDEvent) 
{
    UNREFERENCED_PARAMETER(nIDEvent);
    
    if (m_bFlatLook)
    {
		CClientDC dc(this);

		// OnLButtonDown, show PRESSED.
        if (m_bLBtnDown==TRUE)
		{
            KillTimer (1);

            if (m_bPainted == TRUE)
			{
                DrawCombo(&dc, PRESSED, m_clr3DShadow, m_clr3DHilight);
                m_bPainted = FALSE;
            }
        }
		
		// If mouse leaves, show flat.
		else if (!PointInRect() && !m_bHasFocus)
		{
            KillTimer (1);

            if (m_bPainted == TRUE)
			{
                DrawCombo(&dc, NORMAL, m_clr3DFace, m_clr3DFace);
                m_bPainted = FALSE;
            }

        }

		// On mouse over, show RAISED.
		else
		{
            if (m_bPainted == TRUE)
			{
                return;
            }
			else
			{
                m_bPainted = TRUE;
                DrawCombo(&dc, RAISED, m_clr3DShadow, m_clr3DHilight);
            }
        }
    }
}

BOOL CXTFlatComboBox::OnEraseBkgnd(CDC* pDC) 
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

void CXTFlatComboBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	OnNoFlickerPaint(&dc);
}

void CXTFlatComboBox::OnNoFlickerPaint(CDC* pDC)
{
	ASSERT(pDC != NULL);
	
	// Get the client area to paint.
	CRect rcClient;
	GetClientRect(&rcClient);

	CDC memDC; // memory device context.

    // Create the memory DC, set Map Mode
	memDC.CreateCompatibleDC(pDC);
	memDC.SetMapMode(pDC->GetMapMode());

    // Create a bitmap big enough to hold the window's image
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());

    // Select the bitmap into the memory DC
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	// Repaint the background, this takes the place of WM_ERASEBKGND.
	memDC.FillSolidRect(rcClient, m_clr3DFace);

	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

    if (m_bFlatLook)
    {
        if (PointInRect() || m_bHasFocus) {
            DrawCombo(&memDC, RAISED, m_clr3DShadow, m_clr3DHilight);
        }
        else {
            DrawCombo(&memDC, NORMAL, m_clr3DFace, m_clr3DFace);
        }
    }

	// Blt it
	pDC->BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
		&memDC, rcClient.left, rcClient.top, SRCCOPY);            

    // Select the original bitmap back in
	memDC.SelectObject(pOldBitmap);

	// Clean up
	bitmap.DeleteObject();
	memDC.DeleteDC();
}

void CXTFlatComboBox::DrawCombo(CDC* pDC, STATE eState, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
    CRect rcItem;
    GetClientRect(&rcItem);

    // Cover up dark 3D shadow.
    pDC->Draw3dRect(rcItem, clrTopLeft, clrBottomRight);
    rcItem.DeflateRect(1,1);
    
    if (!IsWindowEnabled()) {
        pDC->Draw3dRect(rcItem, m_clr3DHilight, m_clr3DHilight);
    }
    else {
        pDC->Draw3dRect(rcItem, m_clr3DFace, m_clr3DFace);
    }

    // Cover up dark 3D shadow on drop arrow.
    rcItem.DeflateRect(1,1);
    rcItem.left = rcItem.right-m_cxHThumb;
    pDC->Draw3dRect(rcItem, m_clr3DFace, m_clr3DFace);
    
    // Cover up NORMAL 3D shadow on drop arrow.
    rcItem.DeflateRect(1,1);
    pDC->Draw3dRect(rcItem, m_clr3DFace, m_clr3DFace);
    
    if (IsWindowEnabled())
	{
		rcItem.InflateRect(0,1);
		switch (eState)
		{
		case NORMAL:
			pDC->Draw3dRect(rcItem, m_clr3DHilight, m_clr3DHilight);
			rcItem.left -= 1;
			pDC->Draw3dRect(rcItem, m_clr3DHilight, m_clr3DHilight);
			break;

		case RAISED:
			pDC->Draw3dRect(rcItem, m_clr3DHilight, m_clr3DShadow);
			break;

		case PRESSED:
			rcItem.OffsetRect(1,1);
			pDC->Draw3dRect(rcItem, m_clr3DShadow, m_clr3DHilight);
			break;
		}
    }
}

void CXTFlatComboBox::OnSetFocus()
{
    if (m_bFlatLook)
	{
        m_bHasFocus = TRUE;

		CClientDC dc(this);
        DrawCombo(&dc, RAISED, m_clr3DShadow, m_clr3DHilight);
    }
}

void CXTFlatComboBox::OnKillFocus() 
{
    if (m_bFlatLook)
	{
        m_bHasFocus = FALSE;

		CClientDC dc(this);
        DrawCombo(&dc, NORMAL, m_clr3DFace, m_clr3DFace);    
    }
}

BOOL CXTFlatComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if (m_bAutoComp)
	{
        // Make sure that the keystrokes continue to the edit control.
        if (pMsg->message == WM_KEYDOWN || pMsg->message==WM_KEYUP)
        {
			if (pMsg->message == WM_KEYDOWN)
			{
				if (pMsg->wParam == VK_DELETE ||
					pMsg->wParam == VK_BACK)
				{
					m_bDisableAC = TRUE;
				}
			}
			
			if (pMsg->message == WM_KEYUP)
			{
				if (pMsg->wParam == VK_DELETE ||
					pMsg->wParam == VK_BACK)
				{
					m_bDisableAC = FALSE;
				}
			}

            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);

            return TRUE;
		}
	}

    return CComboBox::PreTranslateMessage(pMsg);
}

void CXTFlatComboBox::OnEditUpdate() 
{
    // if we are not to auto update the text, get outta here
    if (m_bAutoComp)
	{
		if (m_bDisableAC)
		{
			Default();
		}
		else
		{
			// Get the text in the edit box
			CString strItemTyped;
			GetWindowText(strItemTyped);
			int nLength = strItemTyped.GetLength();

			if (nLength >= 1)
			{
				// Currently selected range
				DWORD dwCurSel = GetEditSel();
				WORD dStart = LOWORD(dwCurSel);
				WORD dEnd   = HIWORD(dwCurSel);
				
				// Search for, and select in, and string in the combo box that is prefixed
				// by the text in the edit box
				if (SelectString(-1, strItemTyped) == CB_ERR)
				{
					SetWindowText(strItemTyped);     // No text selected, so restore what was there before
					if (dwCurSel != CB_ERR) {
						SetEditSel(dStart, dEnd);   //restore cursor postion
					}
				}
				
				// Set the text selection as the additional text that we have added
				if (dEnd < nLength && dwCurSel != CB_ERR) {
					SetEditSel(dStart, dEnd);
				}
				else {
					SetEditSel(nLength, -1);
				}
			}
		}
	}
}

void CXTFlatComboBox::OnSysColorChange() 
{
	CComboBox::OnSysColorChange();
	
	// reset color member variables.
    m_clr3DHilight = ::GetSysColor(COLOR_3DHILIGHT);
    m_clr3DShadow  = ::GetSysColor(COLOR_3DSHADOW);
    m_clr3DFace    = ::GetSysColor(COLOR_3DFACE);
}
