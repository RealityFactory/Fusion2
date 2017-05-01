/****************************************************************************************/
/*  PreferencesDialog.cpp                                                               */
/*                                                                                      */
/*  Author:       Jim Mischel, Jeff Lomax                                               */
/*  Description:  Preferences for grid and other stuff                                  */
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
/*Genesis3D Version 1.1 released November 15, 1999                            */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/

#include "stdafx.h"
#include "Globals.h"
//#include "resource.h"
#include "PreferencesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog dialog


CPreferencesDialog::CPreferencesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesDialog::IDD, pParent)
	, m_iEditUndoMAX(0)
{
	//{{AFX_DATA_INIT(CPreferencesDialog)
		m_fEntityDrawScale = 1.0f;
		m_iEditEntityDrawScale = 10;
	//}}AFX_DATA_INIT
}


void CPreferencesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDialog)
	DDX_Control(pDX, IDC_SPIN_ENTITY_VISIBILITY, m_spinEntityVisibility);
	DDX_Control(pDX, IDC_PREFSSNAPGRID, m_SnapGrid);
	DDX_Control(pDX, IDC_PREFSGRID, m_Grid);
	DDX_Control(pDX, IDC_PREFSGRIDBG, m_GridBackground);
	DDX_Text(pDX, IDC_ENTITY_DRAW_SCALE, m_iEditEntityDrawScale);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_UNDOS, m_iEditUndoMAX);
	DDX_Control(pDX, IDC_SPIN_UNDOS, m_spinUndos);
}


BEGIN_MESSAGE_MAP(CPreferencesDialog, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDialog)
	ON_BN_CLICKED(IDC_PATHPREFS, OnPathprefs)
	ON_MESSAGE(CPN_SELCHANGE, OnChangeColor)		
	ON_EN_KILLFOCUS(IDC_ENTITY_DRAW_SCALE, OnKillfocusEntityDrawScale)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENTITY_VISIBILITY, OnDeltaposSpinEntityVisibility)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog message handlers

BOOL CPreferencesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

		//	post 0.57
	CGEditProDoc *pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		m_fEntityDrawScale = pDoc->GetEntityScaleInView();
		if ((m_fEntityDrawScale <0) || (m_fEntityDrawScale >20.0f))
		{
			m_fEntityDrawScale = 1.0f;
		}

		m_iEditEntityDrawScale = (int)(m_fEntityDrawScale*10);
	}
	
	m_iOldEntityDrawScale = m_iEditEntityDrawScale;
	m_spinEntityVisibility.SetRange(1,100);	// post 0.57

	m_spinUndos.SetRange(1,20);
	m_iEditUndoMAX = CGlobals::g_iUndoMAX;


	m_oldBackground = m_tempBackground = coBackground;	// posthoot
	m_oldGrid = m_tempGrid = coGrid;
	m_oldSnapGrid = m_tempSnapGrid = coSnapGrid;


	m_GridBackground.SetColor( coBackground ) ;
	m_Grid.SetColor( coGrid ) ;
	m_SnapGrid.SetColor( coSnapGrid ) ;

	m_GridBackground.TrackSelection = TRUE;
	m_Grid.TrackSelection = TRUE;
	m_SnapGrid.TrackSelection = TRUE;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



LONG CPreferencesDialog::OnChangeColor( UINT lParam, LONG wParam )	// new g3dc
{
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();
	CGEditProApp	*pApp = (CGEditProApp*)AfxGetApp();

						// get new color choices
	m_tempBackground = m_GridBackground.GetColor( ) ;
	m_tempSnapGrid = m_SnapGrid.GetColor( ) ;
	m_tempGrid = m_Grid.GetColor( ) ;

								// preview new color choices
	Prefs_SetBackgroundColor (pApp->pResolvedPrefs, m_tempBackground);
	Prefs_SetGridColor (pApp->pResolvedPrefs, m_tempGrid);
	Prefs_SetSnapGridColor (pApp->pResolvedPrefs, m_tempSnapGrid);	

	pDoc->SetModifiedFlag() ;
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
	return TRUE ;
	wParam;
	lParam;
}/* CGroupsDlg::OnChangeColor */



void CPreferencesDialog::OnOK() 
{
	UpdateData();				
	coBackground = m_tempBackground ;
	coSnapGrid = m_tempSnapGrid ;
	coGrid = m_tempGrid ;

	CGlobals::g_iUndoMAX = this->m_iEditUndoMAX;

	CDialog::OnOK();
}

void CPreferencesDialog::OnPathprefs() 
{
	// TODO: Add your control notification handler code here
	
}

void CPreferencesDialog::OnCancel() 
{
	CGEditProApp	*pApp = (CGEditProApp*)AfxGetApp();

							//	return colors to previous
	Prefs_SetBackgroundColor (pApp->pResolvedPrefs, m_oldBackground);
	Prefs_SetGridColor (pApp->pResolvedPrefs, m_oldGrid);
	Prefs_SetSnapGridColor (pApp->pResolvedPrefs, m_oldSnapGrid);	
		//	post 0.57
	m_iEditEntityDrawScale = m_iOldEntityDrawScale;
	UpdateData(FALSE);	// update the edit box with this value
	OnKillfocusEntityDrawScale(); 

	CDialog::OnCancel();
}



// post 0.57
void CPreferencesDialog::OnKillfocusEntityDrawScale() 
{
	float	lastValue = m_iEditEntityDrawScale;
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();
	UpdateData(TRUE);	// set the m_ScaleWorld variable to the value in the editbox
	
	if (pDoc)
	{
		if (m_iEditEntityDrawScale == NULL)
		{
			m_iEditEntityDrawScale = lastValue;
			UpdateData(FALSE);	//	set the edit box to this value
			return;
		}
		
		if ((m_iEditEntityDrawScale > 0) && (m_iEditEntityDrawScale < 101))
		{
			
			m_fEntityDrawScale = m_iEditEntityDrawScale;
			m_fEntityDrawScale*=(0.10f);
			
			pDoc->SetEntityScaleInView(m_fEntityDrawScale);
			pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
		}
		else
		{
			m_iEditEntityDrawScale = lastValue;
			UpdateData(FALSE);	//	set the edit box to this value
			AfxMessageBox("Value must be > 0 & < 100");
		}	
		
	}
}


//	post 0.57
void CPreferencesDialog::OnDeltaposSpinEntityVisibility(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	OnKillfocusEntityDrawScale();
	*pResult = 0;
}
