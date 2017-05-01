#ifndef __CCaptionView__
#define __CCaptionView__

#define DEFAULT_CAPTION_HEIGHT 15        // 15 arbitrary!

class CCaptionView : public CView
{
public:
    CCaptionView (int nHeight = DEFAULT_CAPTION_HEIGHT)
        : CView (),
				  m_nCaptionHeight (nHeight),
          m_bActive (FALSE)
			{
				ASSERT (m_nCaptionHeight >= 0);

				CaptionFont.CreateFont
				(
					8,						// logical height of font
					0,						// logical average character width
					0,						// angle of escapement
					0,						// base-line orientation angle
					FW_NORMAL,				// font weight
					0,						// italic attribute flag
					0,						// underline attribute flag
					0,						// strikeout attribute flag
					ANSI_CHARSET,			// character set identifier
					OUT_DEFAULT_PRECIS,		// output precision
					CLIP_DEFAULT_PRECIS,	// clipping precision
					DRAFT_QUALITY,			// output quality
					DEFAULT_PITCH,			// pitch and family
					"MS Sans Serif"			// pointer to typeface name string
				);
			}

public:
    int GetCaptionHeight () const
        { return m_nCaptionHeight; }
    void SetCaptionHeight (int nHeight)
        {
					ASSERT (m_nCaptionHeight >= 0);
					m_nCaptionHeight = nHeight;
					OnNcPaint();
        }
    void GetCaption (CString& sCaption)
        { sCaption = m_sCaption; }
    void SetCaption (LPCSTR szCaption)
        {
					m_sCaption = szCaption;
					OnNcPaint();
				}

protected:
    virtual void OnActivateView (BOOL bActivate,
        CView *pActivateView, CView *pDeactivateView);
    virtual void OnActivateFrame(UINT nState,
        CFrameWnd* pFrameWnd);
    virtual void DoDrawCaption (CDC *pDC, const RECT& rect);

    // Generated message map functions
    //{{AFX_MSG(CInfoView)
    afx_msg LRESULT OnNcCalcSize(WPARAM wParam,
        LPARAM lParam);
    afx_msg void OnNcPaint();
    afx_msg UINT OnNcHitTest (CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC (CCaptionView)

protected:
    int m_nCaptionHeight;
    CString m_sCaption;
    BOOL m_bActive;    // TRUE if active, FALSE otherwise

private:
		CFont CaptionFont;
};

#endif // __CCaptionView__
// End of File
