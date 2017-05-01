
/*
 * File:		FlatSplitterWnd.cpp
 *
 * Author:		Marc Richarme <devix@devix.cjb.net>
 *
 * Created:		28 Jan. 2001
 * Modified:	28 Jan. 2001
 *
 * Use as much as you want, wherever you want...
 * Claim you coded it, I don't mind.
 *
 */

#include "stdafx.h"
#include "FlatSplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlatSplitterWnd

CFlatSplitterWnd::CFlatSplitterWnd()
{
	m_cxSplitter = m_cySplitter = 3 + 1 + 1;
	m_cxBorderShare = m_cyBorderShare = 0;
	m_cxSplitterGap = m_cySplitterGap = 3 + 1 + 1;
	m_cxBorder = m_cyBorder = 1;
}

CFlatSplitterWnd::~CFlatSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(CFlatSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CFlatSplitterWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFlatSplitterWnd message handlers

void CFlatSplitterWnd::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	// Let CSplitterWnd handle everything but the border-drawing
	if((nType != splitBorder) || (pDC == NULL))
	{
		CSplitterWnd::OnDrawSplitter(pDC, nType, rectArg);
		return;
	}

	ASSERT_VALID(pDC);

	// Draw border
	pDC->Draw3dRect(rectArg, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
}

