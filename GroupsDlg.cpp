/****************************************************************************************/
/*  GroupsDlg.cpp																		*/
/*                                                                                      */
/*  Author:       Tom Morris															*/
/*  Description:  Group Control															*/
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
/*	Genesis3D Version 1.1 released November 15, 1999				                    */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved							*/
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov 2, 2002				*/
/****************************************************************************************/


#include "stdafx.h"

#include "GEditPro.h"
#include "GroupsDlg.h"
#include "Globals.h"
#include "keyeditdlg.h"
#include "assert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupsDlg dialog


CGroupsDlg::CGroupsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGroupsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGroupsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGroupsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupsDlg)
	DDX_Control(pDX, IDC_DESELECTBRUSHES, m_bDeselctBrushesBtn);
	DDX_Control(pDX, IDC_GROUPCOMBO, m_cGroupCombo);
	DDX_Control(pDX, IDC_SELECTBRUSHES, m_bSelectAllBrushesBtn);
	DDX_Control(pDX, IDC_REMOVEGROUP, m_bRemoveGroupBtn);
	DDX_Control(pDX, IDC_CREATENEWGROUP, m_bCreateNewGroupBtn);
	DDX_Control(pDX, IDC_REMOVEFROMCURRENT, m_bRemoveFromCurrentBtn);
	DDX_Control(pDX, IDC_ADDTOCURRENT, m_bAddtoCurrentBtn);
	DDX_Control(pDX, IDC_BRUSHLOCK, m_BrushLock);
	DDX_Control(pDX, IDC_VISIBLE, m_Visible);
	DDX_Control(pDX, IDC_BRUSHLIST, m_BrushList);
	DDX_Control(pDX, IDC_COLOURBUTTON, m_ColorButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupsDlg, CDialog)
	//{{AFX_MSG_MAP(CGroupsDlg)
	ON_BN_CLICKED(IDC_SELECTBRUSHES, OnSelectbrushes)
	ON_BN_CLICKED(IDC_DESELECTBRUSHES, OnDeselectbrushes)
	ON_BN_CLICKED(IDC_CREATENEWGROUP, OnCreateNewGroup)
	ON_BN_CLICKED(IDC_ADDTOCURRENT, OnAddToCurrent)
	ON_CBN_SELCHANGE(IDC_GROUPCOMBO, OnSelChangeGroupCombo)
	ON_BN_CLICKED(IDC_VISIBLE, OnVisible)
	ON_BN_CLICKED(IDC_REMOVEFROMCURRENT, OnRemovefromcurrent)
	ON_BN_CLICKED(IDC_BRUSHLOCK, OnBrushlock)
	ON_LBN_SELCHANGE(IDC_BRUSHLIST, OnSelchangeBrushlist)
	ON_BN_CLICKED(IDC_REMOVEGROUP, OnRemovegroup)
	ON_MESSAGE(CPN_SELCHANGE, OnChangeColor)		
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupsDlg message handlers

BOOL CGroupsDlg::OnInitDialog() 
{
	
	CGlobals::PositionDialogUnderTabs(this);

	m_ColorButton.TrackSelection = TRUE;

	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


bool	CGroupsDlg::SetupGroupDialog(CGEditProDoc *pDoc)
{
	BOOL	bEnableWindow ;
	
//	CButton * pBut = (CButton*)GetDlgItem( IDC_ADDTOCURRENT ) ;
	bEnableWindow = ( pDoc->m_iCurrentGroup == 0 ) ? FALSE : TRUE ;
//	pBut->EnableWindow( bEnableWindow ) ;
	m_bAddtoCurrentBtn.EnableWindow(bEnableWindow);

	if (pDoc != NULL)
	{
		GroupListType *Groups = pDoc->m_pLevelMgr->GetGroups (pDoc->GetLevel());
		if( Groups )
		{
			if( Group_GetCount( Groups ) )
			{					//	fill the combo on the group control bar
				pDoc->FillBrushGroupCombo( pDoc->m_pMainFrame->m_wndGroupBar.m_comboBox ) ;
								//	fill the combo on this dialog
				pDoc->FillBrushGroupCombo(m_cGroupCombo);
			}
		}
	}
	return true;
}



typedef struct
{
	CListBox	*BrushListbox;
	CGEditProDoc *pDoc;
	geBoolean	ItemsInLB;
} lbAddData;



static geBoolean ListboxAddBrushes (Brush *pBrush, void *lParam)
{
	lbAddData	*pData = NULL;
	char		szTempString[32];
	int			Index;

//	pData->pDoc = CGlobals::GetActiveDocument();

	pData = (lbAddData *)lParam;
	if (pData)
	{

	if (Brush_GetGroupId (pBrush) == pData->pDoc->m_iCurrentGroup)
	{
		sprintf( szTempString, "(B) %s", Brush_GetName( pBrush ) ) ;
		Index = pData->BrushListbox->AddString( szTempString ) ;
		pData->ItemsInLB = GE_TRUE;
		if( Index >= 0 ) /* LB_ERR is -1, LB_ERRSPACE  is -2 */
		{
			pData->BrushListbox->SetItemData( Index, (DWORD)pBrush ) ;
			if( pData->pDoc->BrushIsSelected( pBrush ) )
			{
				pData->BrushListbox->SetSel( Index, TRUE );
			}
		}
	}
	return GE_TRUE;
	}

	return GE_TRUE;
}



static geBoolean ListboxAddEntities (CEntity &Ent, void *lParam)
{
	lbAddData *pData = (lbAddData *)lParam;


	if (Ent.GetGroupId () == pData->pDoc->m_iCurrentGroup)
	{
		char	szTempString[32];
		int Index;

		sprintf( szTempString, "(E) %s", Ent.GetName() ) ;

		Index = pData->BrushListbox->AddString (szTempString);
		pData->ItemsInLB = GE_TRUE;

		if( Index >= 0 )	/* LB_ERR is -1, LB_ERRSPACE  is -2 */
		{
			pData->BrushListbox->SetItemData( Index, (DWORD)&Ent ) ;
			if( Ent.IsSelected() )
			{
				pData->BrushListbox->SetSel( Index, TRUE );
			}
		}
	}
	return GE_TRUE;
}

bool CGroupsDlg::UpdateGroups(CGEditProDoc* NewDoc)
{
	BOOL			bItemsInLB = TRUE ;
	GroupListType	*Groups;
	lbAddData		CallbackData;

	CGEditProDoc	*	pDoc	=NewDoc;
	Groups = pDoc->m_pLevelMgr->GetGroups (pDoc->GetLevel());



	if ((pDoc->m_iModeTool == ID_GENERALSELECT) && (pDoc->m_AdjustMode == ADJUST_MODE_FACE))
	{
		m_BrushList.EnableWindow(FALSE);
		return true;
	}
	else
	{
		m_BrushList.EnableWindow(TRUE);
	}

	if( Group_GetCount( Groups ) == 0 )	// Hack for while doc's are switching without built groups
		return false;

	BOOL	bEnableWindow ;
	
	bEnableWindow = ( pDoc->m_iCurrentGroup == 0 ) ? FALSE : TRUE ;
	m_bAddtoCurrentBtn.EnableWindow(bEnableWindow);


	m_Visible.EnableWindow(1);
	m_bSelectAllBrushesBtn.EnableWindow(TRUE);	// new g3dc
	m_bDeselctBrushesBtn.EnableWindow(TRUE);	//	new g3dc

	if(pDoc->m_iCurrentGroup)
	{
		m_bRemoveGroupBtn.EnableWindow(TRUE);	//	new g3dc
		m_ColorButton.EnableWindow(TRUE);
	}
	else
	{
		m_BrushLock.EnableWindow(FALSE);
		m_bRemoveGroupBtn.EnableWindow(FALSE);
	}

	// fill in name
//	char const *Name = Group_GetNameFromId (Groups, pDoc->m_iCurrentGroup);

	if(Group_IsVisible (Groups, pDoc->m_iCurrentGroup))
	{
		m_Visible.SetCheck (1);
	}
	else
	{
		m_Visible.SetCheck (0);
	}

	if(Group_IsLocked (Groups, pDoc->m_iCurrentGroup))
	{
		m_BrushLock.SetCheck (1);
	}
	else
	{
		m_BrushLock.SetCheck (0);
	}

				//	fill brush list box with names of current brushes and entities
	m_BrushList.ResetContent();

	CallbackData.BrushListbox = &m_BrushList;
	CallbackData.pDoc = pDoc;
	CallbackData.ItemsInLB = GE_FALSE;

	pDoc->m_pLevelMgr->EnumBrushes (pDoc->GetLevel(), &CallbackData, ::ListboxAddBrushes);
	pDoc->m_pLevelMgr->EnumEntities (pDoc->GetLevel(), &CallbackData, ::ListboxAddEntities);

				//	put the topmost selection at the top of the list box
	int	tempIndex;
	m_BrushList.GetSelItems(1, &tempIndex);
	if (tempIndex != -1)
	{
		m_BrushList.SetTopIndex(tempIndex);
	}
	
	LoadComboBox() ;

	if( pDoc->m_iCurrentGroup == 0 )
		bItemsInLB = FALSE ;	// Never let them remove from the default group

	m_bAddtoCurrentBtn.EnableWindow(TRUE);	// new g3dc
	m_bRemoveFromCurrentBtn.EnableWindow(bItemsInLB);	// new g3dc

	UpdateGroupSelection();

			//	Restore focus to active view
	CMDIChildWnd *pMDIChild	=(CMDIChildWnd *)pDoc->m_pMainFrame->MDIGetActive();
	if(pMDIChild)
	{
		CView	*cv	=(CView *)pMDIChild->GetActiveView();
		if( cv)
			cv->SetFocus();
	}




	return true;
}


// fill the combo box with the list of groups and ids
void CGroupsDlg::LoadComboBox(void)
{
//	CComboBox		*GCombo	=(CComboBox *)GetDlgItem(IDC_GROUPCOMBO);
	COLORREF		Color ;
	const GE_RGBA * pGeColor ;
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();

	GroupListType	*Groups = pDoc->m_pLevelMgr->GetGroups (pDoc->GetLevel());

	assert(m_cGroupCombo);
	assert(pDoc->m_pMainFrame->m_wndGroupBar.m_comboBox);
									//	this doesn't seem to be working
//	GroupList_FillCombobox (Groups, &m_cGroupCombo, pDoc->m_iCurrentGroup);
//	GroupList_FillCombobox(Groups, &pDoc->m_pMainFrame->m_wndGroupBar.m_comboBox, pDoc->m_iCurrentGroup);
	
	pDoc->FillBrushGroupCombo( pDoc->m_pMainFrame->m_wndGroupBar.m_comboBox ) ;
	pDoc->FillBrushGroupCombo(m_cGroupCombo);


	pGeColor = Group_GetColor( Groups, pDoc->m_iCurrentGroup ) ;
	Color = RGB( pGeColor->r, pGeColor->g, pGeColor->b ) ;
//		Color = RGB( 255, 255, 255 ) ;

	m_ColorButton.SetColor( Color ) ;

}/* CGroupsDlg::LoadComboBox */


void CGroupsDlg::OnOK() 
{
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();


	GroupListType *Groups = pDoc->m_pLevelMgr->GetGroups (pDoc->GetLevel());
	BrushList *BList = pDoc->m_pLevelMgr->GetBrushes (pDoc->GetLevel());
	CEntityArray *Entities = pDoc->m_pLevelMgr->GetEntities (pDoc->GetLevel());

	// Set visible and lock state on all group brushes/entities
	if (m_Visible.GetCheck ())
	{
		Group_Show (Groups, pDoc->m_iCurrentGroup, BList, Entities);
	}
	else
	{
		Group_Hide (Groups, pDoc->m_iCurrentGroup, BList, Entities);
	}

	if (m_BrushLock.GetCheck ())
	{
		Group_Lock (Groups, pDoc->m_iCurrentGroup, BList, Entities);
	}
	else
	{
		Group_Unlock (Groups, pDoc->m_iCurrentGroup, BList, Entities);
	}
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up

		pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);

	if (this->IsWindowVisible())	//	post 0.57
	{
						//	Restore focus to active view
		CMDIChildWnd *pMDIChild	=(CMDIChildWnd *)pDoc->m_pMainFrame->MDIGetActive();
		if(pMDIChild)
		{
			CView	*cv	=(CView *)pMDIChild->GetActiveView();
			if( cv)
				cv->SetFocus() ;
		}
	}


  //	CDialog::OnOK();
}


// this is the SelctAll button
void CGroupsDlg::OnSelectbrushes()
{
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();

		// Add all brushes/entities in the current group to the selected list
	pDoc->SelectGroupBrushes (TRUE, pDoc->m_iCurrentGroup);
	m_BrushList.SetSel( -1, TRUE );

	OnOK();

}

void CGroupsDlg::OnDeselectbrushes()
{

	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();

	// Remove all brushes/entities in the current group from the selected list
	pDoc->SelectGroupBrushes (FALSE, pDoc->m_iCurrentGroup);
	pDoc->ResetAllSelections();
	m_BrushList.SetSel( -1, FALSE );

				//	close out an existing Property Dialog boxes
	pDoc->m_pMainFrame->HideAllPropDialogsBut(this);	

	OnOK();
}


void CGroupsDlg::SelectNone()
{
	OnDeselectbrushes();
}


void CGroupsDlg::OnCancel() 
{
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();
	
	UpdateGroups(pDoc);
}

// create a new brush group and add all currently-selected
// brushes and entities to the new group.
void CGroupsDlg::OnCreateNewGroup(void) 
{

//AfxMessageBox("CreateNew Button is working");

	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();
	
	int	NewGroupId ;

	NewGroupId = pDoc->MakeNewBrushGroup( this ) ;	

	
	if( NewGroupId != 0 ) 
	{
		m_Visible.SetCheck( TRUE ) ;
		m_BrushLock.SetCheck( FALSE ) ;
		OnOK( );
//		UpdateTabDisplay( pDoc ) ;
//		pDoc->m_pMainFrame->UpdateActiveDoc() ;
		pDoc->m_pMainFrame->UpdateMainControls();
	}

}/* CGroupsDlg::OnCreateNewGroup */

void CGroupsDlg::OnAddToCurrent(void)
{
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();
	
	pDoc->AddSelToGroup() ;
	OnOK();
	UpdateGroups(pDoc);

}

void CGroupsDlg::OnSelChangeGroupCombo() 
{
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();
	
//	CComboBox		*GCombo	=(CComboBox *)GetDlgItem(IDC_GROUPCOMBO);

	assert(m_cGroupCombo);

	if (pDoc != NULL)
	{
		int Id = m_cGroupCombo.GetCurSel();

		if (Id != LB_ERR)
		{							//	deselect all from current goup
			OnDeselectbrushes();	//	otherwise, craziness on Prop Panel

			pDoc->m_iCurrentGroup = m_cGroupCombo.GetItemData(Id);
		}

		//	CHANGE!	03/24/97	John Moore
		//	Go ahead and reset the currentbrush
		pDoc->m_pCurBrush = pDoc->m_pBTemplate;
		//	End of CHANGE
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
		// Update the Toolbar		//	NOT HERE! G3DC
	//	pDoc->m_pMainFrame->LoadGroupComboBox() ;
	}	
			//	deslect all brushes, otherwise bursh/entity/face editing is chaotic
	
	UpdateGroups(pDoc);
							//	deselect all from goup we just switched to
	OnDeselectbrushes();	//	otherwise, craziness on Prop Panel



}/* CGroupsDlg::OnSelChangeGroupCombo */

void CGroupsDlg::OnVisible() 
{
	OnOK();
}

// removed selected brushes/entities from current group
void CGroupsDlg::OnRemovefromcurrent(void) 
{
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();

	pDoc->RemovesSelFromGroup() ;
	OnOK();
	UpdateGroups(pDoc);
}

void CGroupsDlg::OnBrushlock() 
{
	int c ;
	
	if( m_BrushLock.GetCheck () )
	{
		// If ANY items in the group are selected, select them all
		c = m_BrushList.GetSelCount() ;
		if( c > 0 )
		{
			OnSelectbrushes() ;
		}
		// Then disable the LB until the user unlocks the group
		m_BrushList.EnableWindow( FALSE ) ;
	}
	else
	{
		m_BrushList.EnableWindow( TRUE ) ;
	}
	
	OnOK();
}/* CGroupsDlg::OnBrushlock */




void CGroupsDlg::OnSelchangeBrushlist() 
{
	int			c ;
	int			i ;
	Brush	*	b;
	CEntity *	pEnt;
	char		szBuffer[64]; // Should have a limit for entity/brush name
	geBoolean	bChanged = FALSE ;	// You get selchanges on cursor movements that don't change selection...
	
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();
	if (pDoc == NULL)
		return;
/*	
	if (pDoc->m_iModeTool != ID_GENERALSELECT)
	{
		pDoc->DoGeneralSelect();
		int nState = pDoc->m_pMainFrame->m_wndBrushToolBar.GetToolBarCtrl().GetState(ID_GENERALSELECT);
		pDoc->m_pMainFrame->m_wndBrushToolBar.GetToolBarCtrl().SetState(ID_GENERALSELECT, nState | TBSTATE_CHECKED);
	}
		
*/		c = m_BrushList.GetCount() ;
		if( c > 0 )
		{
			for( i=0; i<c; i++ )
			{
				m_BrushList.GetText( i, szBuffer ) ;
				if( szBuffer[1] == 'B' )
				{
					b = (Brush *)m_BrushList.GetItemData( i );
					if( m_BrushList.GetSel( i ) )
					{	// User says brush should be selected
						if( pDoc->BrushIsSelected( b ) == GE_FALSE )
						{
							pDoc->DoBrushSelection( b, brushSelToggle ) ;
							bChanged = GE_TRUE ;
						}
					}
					else
					{	// User says don't select
						if( pDoc->BrushIsSelected( b ) == GE_TRUE )
						{
							pDoc->DoBrushSelection( b, brushSelToggle ) ;
							bChanged = GE_TRUE ;
						}
					}
				}// Item is Brush
				else if( szBuffer[1] == 'E' )
				{
					pEnt = (CEntity *)m_BrushList.GetItemData( i );	
					if( m_BrushList.GetSel( i ) )
					{
						if( pEnt->IsSelected() == GE_FALSE )
						{	// User says entity should be selected
							pDoc->DoEntitySelection( pEnt ) ;
							bChanged = GE_TRUE ;
						}
					}
					else
					{
						if( pEnt->IsSelected() == GE_TRUE )
						{	// User says entity should not be selected
							pDoc->DoEntitySelection( pEnt ) ;
							
							//	int CurrentSelection = m_BrushList.GetCurSel();
							//	pDoc->SetSelectedEntity(CurrentSelection);
							//	pDoc->UpdateSelected();
							bChanged = GE_TRUE ;
						}
					}
					
				}// Item is Entity	
			}// Loop thru Listbox
		}// There are items in the Listbox
		
		if( bChanged )
		{	pDoc->UpdateSelected();	//	calls UpdateMainControls()
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		// thes options are necessary in order for Face attributes to work right
// g3dc tuning		if (pDoc->m_AdjustMode == ADJUST_MODE_BRUSH)
//			pDoc->UpdateAllViews( UAV_ALLVIEWS | REBUILD_QUICK, NULL );
//		else
			pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
		}
}/* CGroupsDlg::OnSelchangeBrushlist */







void CGroupsDlg::UpdateGroupSelection( void ) 
{
#if 1
//	#pragma message ("Group selection stuff badly farkled")
#else
	int			c ;
	int			i ;
	Brush	*	b = NULL;
	CEntity *	pEnt = NULL;
	char		szBuffer[64]; // Should have a limit for entity/brush name
	BOOL		bSelect ;

	CGEditProDoc	*pDoc = NULL
	pDoc = CGlobals::GetActiveDocument();


	c = m_BrushList.GetCount() ;
	if( c > 0 )
	{
		for( i=0; i<c; i++ )
		{
			m_BrushList.GetText( i, szBuffer ) ;
			if( szBuffer[1] == 'B' )
			{
				b = (Brush *)m_BrushList.GetItemData( i );
				if (b)
				{
					if( Brush_GetGroupId(b) == pDoc->m_iCurrentGroup )
					{
						bSelect = pDoc->BrushIsSelected( b ) ;
						m_BrushList.SetSel( i, bSelect );
					}
				}
			}// Item is Brush
			else if( szBuffer[1] == 'E' )
			{
				pEnt = (CEntity *)m_BrushList.GetItemData( i );	
				if (pEnt)
				{
					if( pEnt->GetGroupId () == pDoc->m_iCurrentGroup )
					{
						bSelect = pEnt->IsSelected() ;
						m_BrushList.SetSel( i, bSelect );
					}
				}
			}// Item is Entity	
		}// Loop thru Listbox
	}// There are items in the Listbox
#endif
}/* CGroupsDlg::UpdateGroupSelection */


void CGroupsDlg::OnRemovegroup() 
{
	GroupIterator	gi;
	GroupListType *Groups = NULL;
	BrushList *BList = NULL;
	CEntityArray *Entities = NULL;
	CGEditProDoc	*pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{

	Groups = pDoc->m_pLevelMgr->GetGroups (pDoc->GetLevel());
	BList = pDoc->m_pLevelMgr->GetBrushes (pDoc->GetLevel());
	Entities = pDoc->m_pLevelMgr->GetEntities (pDoc->GetLevel());

	if (Groups && BList && Entities)
	{
		// Nuke the current group
		Group_RemoveFromList (Groups, pDoc->m_iCurrentGroup, BList, Entities);

		// Find another group for our current one
		pDoc->m_iCurrentGroup = Group_GetFirstId( Groups, &gi ) ;
		pDoc->SetModifiedFlag() ;

		m_BrushList.ResetContent() ;
		UpdateGroups(pDoc);
	}
	}
//	LoadComboBox( );

}/* CGroupsDlg::OnRemovegroup */


//LRESULT CGroupsDlg::OnChangeColor( WPARAM wParam, LPARAM lParam )
LONG CGroupsDlg::OnChangeColor( UINT lParam, LONG wParam )	// new g3dc
{
	CGEditProDoc	*pDoc = CGlobals::GetActiveDocument();
	
	COLORREF	color ;
	GE_RGBA		geColor ;
	GroupListType *Groups = pDoc->m_pLevelMgr->GetGroups (pDoc->GetLevel());

	color = m_ColorButton.GetColor() ;
	geColor.r = GetRValue( color ) ;
	geColor.g = GetGValue( color ) ;
	geColor.b = GetBValue( color ) ;
	Group_SetColor ( Groups, pDoc->m_iCurrentGroup, &geColor ) ;
	pDoc->SetModifiedFlag() ;
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
	return TRUE ;
	wParam;
	lParam;
}/* CGroupsDlg::OnChangeColor */




void CGroupsDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// otherwise color button disappears after app switching, or Minimize/Maximize
	m_ColorButton.RedrawWindow();	
	// Do not call CDialog::OnPaint() for painting messages
}
