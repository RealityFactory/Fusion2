/****************************************************************************************/
/*  LevelOptions.h                                                                      */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax                                    */
/*  Description:  Genesis world editor header file                                      */
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
/*  Genesis3D Version 1.1 released November 15, 1999                                 */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/****************************************************************************************/
#if !defined(AFX_LEVELOPTIONS_H__BD140A61_2534_11D2_B69D_004005424FA9__INCLUDED_)
#define AFX_LEVELOPTIONS_H__BD140A61_2534_11D2_B69D_004005424FA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LevelOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLevelOptions dialog

class CLevelOptions : public CDialog
{
// Construction
public:
	CLevelOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLevelOptions)
	enum { IDD = IDD_LEVELOPTIONS };
	geFloat	m_DrawScale;
	geFloat	m_LightmapScale;
	CString	m_TextureLib;
	bool	m_TxlChanged;
	CString	m_HeadersDir;
	bool	m_HeadersChanged;
	CString	m_ActorsDir;
	bool	m_ActorsChanged;
	CString	m_PawnIni;
	bool	m_PawnIniChanged;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLevelOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLevelOptions)
	afx_msg void OnBrowsetxl();
	afx_msg void OnChangeEdittxl();
	afx_msg void OnChangeEditheadersdir();
	afx_msg void OnChangeEditactorsdir();
	afx_msg void OnBrowseini();
	afx_msg void OnChangeEditpawnini();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEVELOPTIONS_H__BD140A61_2534_11D2_B69D_004005424FA9__INCLUDED_)
