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
/*  Genesis3D Version 1.1 released November 15, 1999                                    */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved                            */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002                          */
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
#include "GEditProDoc.h"

#include "PathSelector\ListBoxEx.h"
#include "PathSelector\SHBrowseDlg.h"

class CPathListBoxEx: public CListBoxEx
{
public:

   CPathListBoxEx()
   {
      SetEditStyle( LBEX_EDITBUTTON );
   };

   virtual void OnBrowseButton( int iItem )
   {
      iItem;
      CSHBrowseDlg dlgBrowse;
      if ( dlgBrowse.DoModal() ) SetEditText( dlgBrowse.GetFullPath() );
   };
};




class CLevelOptions : public CDialog
{
// Construction
public:
	CLevelOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLevelOptions)
	enum { IDD = IDD_LEVELOPTIONS };
	float	m_DrawScale;
	float	m_LightmapScale;
	CString	m_TextureLib;
	bool	m_TxlChanged;
	CString	m_HeadersDir;
	bool	m_HeadersChanged;
// changed QD Actors
	CString	m_PawnIni;
	bool	m_PawnIniChanged;
	CString	m_ActorsDir;
	bool	m_ActorsChanged;
// end change
	CPathListBoxEx	m_ListBoxEx;
	float	m_ScaleWorld;	//	post 0.57
	float	m_CurrentScale;	//	post 0.57
	double	m_SnapDegrees;
	BOOL	m_UseSnap;
	int		MetricOrTexelSnap;
	int		MetricOrTexelGrid;
	int		TexelGridUnits;
	int		m_GridUnits;
	//}}AFX_DATA
      CListBoxExBuddy m_ListBoxExBuddy;
	  CString m_tempHeadersDir;

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLevelOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CGEditProDoc	*m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CLevelOptions)
	afx_msg void OnBrowsetxl();
	afx_msg void OnChangeEdittxl();
	afx_msg void OnChangeEditheadersdir();
// changed QD Actors
	afx_msg void OnChangeEditpawnini();
	afx_msg void OnChangeEditactorsdir();
	afx_msg void OnBrowsepawnini();
// end change
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusDrawscale();
	afx_msg void OnKillfocusLightmapscale();
	afx_msg void OnKillfocusScaleWorld();
			afx_msg void OnSnap15();
	afx_msg void OnSnap30();
	afx_msg void OnSnap45();
	afx_msg void OnSnap60();
	afx_msg void OnUsertosnap();
	afx_msg void OnSnap90();
	afx_msg void OnTexel1Radio();
	afx_msg void OnTexel2Radio();
	afx_msg void OnTexel4Radio();
	afx_msg void OnTexel8Radio();
	afx_msg void OnTexel16Radio();
	afx_msg void OnTexel32Radio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEVELOPTIONS_H__BD140A61_2534_11D2_B69D_004005424FA9__INCLUDED_)
