
/*
 * File:		FlatSplitterWnd.h
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

#if !defined(AFX_FLATSPLITTERWND_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_)
#define AFX_FLATSPLITTERWND_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CFlatSplitterWnd

class CFlatSplitterWnd : public CSplitterWnd
{
public:
	CFlatSplitterWnd();
	virtual ~CFlatSplitterWnd();
	void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatSplitterWnd)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CFlatSplitterWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLATSPLITTERWND_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_)
