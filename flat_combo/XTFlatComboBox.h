// XTFlatComboBox.h interface for the CXTFlatComboBox class.
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

#if !defined(__XTFLATCOMBOBOX_H__)
#define __XTFLATCOMBOBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// The CXTFlatComboBox class can easily give your application the Flat Look to 
// your standard CComboBox control, that as seen in many of the Microsoft® Office 
// products. To use the control, use Class Wizard to add the CComboBox member 
// variable ( Class Wizard -> Member Variables Tab -> Add Variable ) and rename 
// CComboBox to CXTFlatComboBox.

class CXTFlatComboBox : public CComboBox
{
    DECLARE_DYNAMIC(CXTFlatComboBox)

public:
    
    // -> Remarks:  Constructs a CXTFlatComboBox object.
    CXTFlatComboBox();

    // -> Remarks:  Destroys a CXTFlatComboBox object, handles cleanup and de-allocation.
    virtual ~CXTFlatComboBox();

protected:

    // enum used to determine the state the combo box should be
    enum STATE { NORMAL = 1, RAISED = 2, PRESSED = 3 };

    int      m_cxHThumb;     // Size of the drop arrow for the combo box.
    BOOL     m_bLBtnDown;    // TRUE if left mouse button is pressed
    BOOL     m_bPainted;     // used during paint operations
    BOOL     m_bHasFocus;    // TRUE if control has focus
    BOOL     m_bAutoComp;    // Used by Autocompleting.
    BOOL     m_bFlatLook;    // TRUE if the control is flat.
	BOOL     m_bDisableAC;   // TRUE when auto complete is disabled internally.
    COLORREF m_clr3DHilight; // System color for ::GetSysColor(COLOR_3DHILIGHT).
	COLORREF m_clr3DShadow;  // System color for ::GetSysColor(COLOR_3DSHADOW).
	COLORREF m_clr3DFace;    // System color for ::GetSysColor(COLOR_3DFACE).

public:

    // -> Input:   bDisable - TRUE to disable flat appearance.
    // -> Remarks: This member function will disable the flat look for the combo box.
    void DisableFlatLook(BOOL bDisable);

    // -> Input:   bEnable - TRUE to enable auto completion; otherwise FALSE.
    // -> Remarks: This member function enables or disables auto completion.
    virtual void EnableAutoCompletion(BOOL bEnable=TRUE);

    // -> Returns: TRUE if the mouse is over the combo box; otherwise FALSE.
    // -> Remarks: This member function determines if the mouse is within the controls rect.
    virtual BOOL PointInRect();

protected:

    // -> Input:   pDC            - Points to the current device context.
	//             eState         - Current state of the combo box, either NORMAL, RAISED, or PRESSED.
    //             clrTopLeft     - RGB color value for top left 3D borders.
    //             clrBottomRight - RGB color value for bottom right 3D borders.
    //             
    // -> Remarks: This member function is called by the combo box whenever a paint 
    //             operation should occur.
    virtual void DrawCombo(CDC* pDC, STATE eState, COLORREF clrTopLeft, COLORREF clrBottomRight);

	// -> Input:   pDC - Points to the current device context.
	// -> Remarks: This member function is called by the control to for flicker
	//             free drawing.
	void OnNoFlickerPaint(CDC* pDC);

    //{{AFX_VIRTUAL(CXTFlatComboBox)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CXTFlatComboBox)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnPaint();
    afx_msg void OnSetFocus();
    afx_msg void OnKillFocus();
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTFlatComboBox::DisableFlatLook(BOOL bDisable) {
    m_bFlatLook = !bDisable;
}
AFX_INLINE void CXTFlatComboBox::EnableAutoCompletion(BOOL bEnable/*=TRUE*/) {
    ASSERT(::IsWindow(m_hWnd)); m_bAutoComp = bEnable;
}
AFX_INLINE BOOL CXTFlatComboBox::PointInRect() {
    ASSERT(::IsWindow(m_hWnd)); CPoint pt; GetCursorPos(&pt); CRect rcItem; GetWindowRect(&rcItem); return rcItem.PtInRect(pt);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTFLATCOMBOBOX_H__)
