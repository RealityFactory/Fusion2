/****************************************************************************************/
/*  EntitiesDialog.cpp                                                                  */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax                                    */
/*  Description:  Dialog code for templates                                             */
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
#include "stdafx.h"
#include "Globals.h"
#include "EntitiesDialog.h"
#include "KeyEditDlg.h"
#include "EntityTable.h"
#include <assert.h>
#include "ram.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntitiesDialog dialog

CEntitiesDialog::CEntitiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEntitiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEntitiesDialog)
	//}}AFX_DATA_INIT
	mCurrentEntity = 0;
	mCurrentKey = LB_ERR;
	m_pEntityArray = NULL;

	CDialog::Create(IDD,pParent);

	pDoc = CGlobals::GetActiveDocument();
}


void CEntitiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntitiesDialog)
	DDX_Control(pDX, IDC_ENTITIES_STATIC, m_entitiesStaticString);
	DDX_Control(pDX, IDC_PROPERTIESLIST, m_PropertiesList);
	DDX_Control(pDX, IDC_ENTITYCOMBO, m_EntityCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEntitiesDialog, CDialog)
	//{{AFX_MSG_MAP(CEntitiesDialog)
	ON_CBN_SELCHANGE(IDC_ENTITYCOMBO, OnSelchangeEntitylist)
	ON_LBN_DBLCLK(IDC_PROPERTIESLIST, OnDblclkPropertieslist)
	ON_WM_VKEYTOITEM()
	ON_WM_DRAWITEM()
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, NeedTextNotify )
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntitiesDialog message handlers



BOOL CEntitiesDialog::OnInitDialog() 
{
	pDoc = CGlobals::GetActiveDocument();


	CDialog::OnInitDialog();

	pDoc->m_pMainFrame->HideAllPropDialogsBut(this);


//	CEntityArray *Entities = Level_GetEntities (pDoc->GetLevel()); //	moved to UpdateEntities
//	EditEntity(*Entities, pDoc->m_iCurrentEntity, pDoc);	//	moved to UpdateEntities


	return true;

}


/*
UpdateMainControls --->(conditional)EditEntity ---> ShowDialog() ---> FillInDialog --->Reset combo box

UpdateMainControls ---> UpdateEntities ---> EditEntity--->ShowDialog ---> FillInDialog--->reset combo box
*/



bool CEntitiesDialog::UpdateEntities(CGEditProDoc *pDoc)
{
	if (pDoc != NULL)
	{
		CEntityArray *pTempEntities = NULL;
		CtLevel	*pTempLevel = NULL;
		CtLevelMgr *pTempLevelMgr = NULL;
		pTempLevel = pDoc->GetLevel();

		if (pTempLevel)
		{
			if (pDoc->m_pLevelMgr)
			{
				pTempEntities = pDoc->m_pLevelMgr->GetEntities(pTempLevel);	
				if (pTempEntities)
				{
					EditEntity(*pTempEntities, pDoc->m_iCurrentEntity, pDoc);
				}
			}
		}
		
		pDoc->UpdateEntityOrigins();
	
		if (((CDialogBar*)this->GetParent())->IsWindowVisible())	//	post 0.57
		{

				FillInDialog();
				//	Be very careful when speccing flags for UpdateAllViews()
				//	The wrong flags at the wrong time will totally screw things up
				pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
				//	pDoc->SetModifiedFlag();

			if (!((CDialogBar*)this->GetParent())->IsFloating())	//	post 0.57
			{
			//	Restore focus to active view ONLY when the Prop Panel is Docked
				CMDIChildWnd *pMDIChild	=(CMDIChildWnd *)pDoc->m_pMainFrame->MDIGetActive();
				if(pMDIChild)
				{
					CView	*cv	=(CView *)pMDIChild->GetActiveView();
					if( cv)
						cv->SetFocus() ;
				}
			}
		}
	}
	return true;
}


int CEntitiesDialog::EditEntity( CEntityArray& Entities, int CurrentEntity, CGEditProDoc* Doc)
{
	// If more than one entity selected, make sure they're all the same type.
	int nSel = 0;
	CString strEntityClassname;
	int i;

	pDoc = Doc;
	m_pEntityArray = &Entities;

	for (i = 0; i < m_pEntityArray->GetSize(); ++i)
	{
		CEntity *pEnt= NULL;

		pEnt = &Entities[i];
		if (pEnt)
		{
			if (pEnt->IsSelected ())
			{
				if (nSel == 0)	// if it's the first one selected
				{
					strEntityClassname = pEnt->GetClassname ();
					mCurrentEntity = i;
				}
				else	//	otherwise it's not the first selected entity
				{		//	compare this one with the first selected entity
						//	if selected entities are of different types
						//	TEST TO SEE IF SELECTED ENTITIES ARE OF DIFFERENT TYPES
					if (pEnt->GetClassname () != strEntityClassname)
					{						
									//	new g3dc
									//	show the dialog box, keeping in mind that
							ShowDialog(false);	//	multiple types of entities are selected	
	//						AfxMessageBox ("To edit multiple entities, they must all be of the same type.");
							
							return CurrentEntity; // we're outta here
					}
				}
				++nSel;
			}
		}
	}

	//	Now, we may have only one entity selected, OR we may have multiple
	//	entities selected. But since we already tested (above) to see if multiple
	//	entities are of different types, we are confident that multiple selected
	//	entities are of the same type.

									//	MultiEntityFlag gets set to TRUE if more than
									//	one entity OF THE SAME TYPE is selected.
	MultiEntityFlag = (nSel > 1);	// 
	if (MultiEntityFlag)	// if more than one entity is selected
	{						//	don't do nuthin'
	}
	else					//	only a single entity be selected
	{
		if (CurrentEntity != -1)	//	if it's not the NULL spot on the list
		{
			mCurrentEntity = CurrentEntity;
		}
		else
		{
			//	Let's set entity 0 as selected if it exists...
			mCurrentEntity = 0;
			if (m_pEntityArray->GetSize()  > 0)
			{
				pDoc->ResetAllSelectedEntities();
				pDoc->SelectEntity (&(*m_pEntityArray)[mCurrentEntity]);
				pDoc->m_iCurrentEntity = mCurrentEntity;
			}
		}
	}

	ShowDialog(true);

	m_iCurrentEntity = mCurrentEntity;
	return mCurrentEntity;
}





bool CEntitiesDialog::ShowDialog(bool bShow)
{
	if (this->GetParent()->IsWindowVisible()) // post 0.57 if Properties Panel Visible
	{
		SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
		this->ShowWindow(SW_SHOW);		
		
		if (bShow)	//	single entity or multiple entities of the same type are selected
		{
			//		if (pDoc->GetSelState() & ONEENTITY)	//	NEW G3DC
			//			MultiEntityFlag = 0;
			
			m_entitiesStaticString.SetWindowText("Ready...");
			
			// we have to go through and setup all of our stuff
			FillInDialog();
			
			m_PropertiesList.EnableWindow(TRUE);

			if (!((CDialogBar*)this->GetParent())->IsFloating()) //	post 0.57
			{
				m_PropertiesList.SetFocus(); // set focus ONLY when panel is NOT floating.
				//	While undocked, this will send a SetFocus message to the MainFrame
				//	which calls UpdateMainControls, which ultimately calls this function
				//	again, resulting in an endless loop.
			}
			
			if (MultiEntityFlag)
				m_EntityCombo.EnableWindow (FALSE);
			else
				m_EntityCombo.EnableWindow(TRUE);
			
			if (!EnableToolTips (TRUE))
			{
				return FALSE;
			}
			
		}
		
		else if (!bShow)	//	different entity types are selected
		{
			m_EntityCombo.EnableWindow (FALSE);
			//		MessageBeep(MB_ICONEXCLAMATION);
			m_entitiesStaticString.SetWindowText("Not Same Type...");
			m_PropertiesList.EnableWindow(FALSE);
		}
		return true;
	}
	return true;
}

int CEntitiesDialog::DoDialog( CEntityArray& Entities, CGEditProDoc* Doc)
{
	// the list of entities
	m_pEntityArray = &Entities;
	pDoc = Doc;
//	DoModal();

	return mCurrentEntity;
}


int CEntitiesDialog::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
	CPoint pt;
	UINT Index;
	BOOL bOutside;

	// Make point relative to properties list box
	pt = point;
	ClientToScreen (&pt);
	m_PropertiesList.ScreenToClient (&pt);

	// If the passed point is within the boundaries of the properties list box,
	// and is over one of the properties, instruct the caller to display
	// the tooltip obtained from EntityTable_GetToolTip.
	Index = m_PropertiesList.ItemFromPoint (pt, bOutside);
	if (bOutside)
	{
		return -1;
	}

	pTI->lpszText = LPSTR_TEXTCALLBACK;
	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)m_PropertiesList.m_hWnd;
	pTI->uFlags |= (TTF_IDISHWND | TTF_NOTBUTTON);
	// Ok, this is a seriously ugly hack.
	// But I can't seem to get it to work any other way.
	// I need to pass the item index to the NeedTextNotify callback, but there
	// is no field for it (apparently the above uId and uFlags must be set
	// exactly like they are, or things don't work).
	// Anyway, what I do here is set an object-global index and then retrieve it 
	// in the NeedTextNotify function.
	UglyGlobalItemId = Index;

	return 1;
}

#pragma warning (disable:4100)
BOOL CEntitiesDialog::NeedTextNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	const char *pDocField;
	CString EntityClassname;
	CString FieldName;
	int Index;
	CEntity *pEnt;

	// default return value is nothing...
	pTTT->szText[0] = '\0';

	// Fill the szText field with the documentation string for the current field.
	
	// Get current entity selection and retrieve its name
	Index = m_EntityCombo.GetCurSel ();
	if (Index == CB_ERR)
	{
		return FALSE;
	}
	Index = m_EntityCombo.GetItemData (Index);
	pEnt = &((*m_pEntityArray)[Index]);
	EntityClassname = pEnt->GetClassname ();

	m_PropertiesList.GetText (UglyGlobalItemId, FieldName);

	pDocField = EntityTable_GetEntityFieldDoc (pDoc->m_pLevelMgr->GetEntityDefs (pDoc->GetLevel()), EntityClassname, FieldName);
	if (pDocField != NULL)
	{
		strncpy (pTTT->szText, pDocField, sizeof (pTTT->szText));
		pTTT->szText[sizeof (pTTT->szText)-1] = '\0';
	}
	return FALSE;
}
#pragma warning (default:4100)

static void stripQuotes
	(
	  CString &str
	)
{
	char *temp = Util_Strdup (str);
	
	if (*temp == '"')
	{
	    strcpy (temp, temp+1);
	}
	if (temp[strlen (temp) - 1] == '"')
	{
		temp[strlen (temp) - 1] = '\0';
	}
	str = temp;

	geRam_Free (temp);
}

//============================================================
// This funtion will go through the list of key value
// pairs and add all of the entities properly into the
// correct arrays
//============================================================
typedef struct tagSortStruct
{
	CEntity *pEnt ;
	int		nItem ;
} SSortStruct, *PSortStruct;

static int compare( const void * arg1, const void * arg2 )
{
	PSortStruct pLeft ;
	PSortStruct pRight ;
	CString		Left ;
	CString		Right ;
	int32		LeftVal, LeftLen ;
	int32		RightVal, RightLen ;

	pLeft = (PSortStruct)arg1 ;
	pRight = (PSortStruct)arg2 ;

	Left = pLeft->pEnt->GetName ();
	if (Left == "")
	{
		Left = "Unnamed";
	}

	// strip quotes from the entity name
	stripQuotes ( Left );
	
	Right = pRight->pEnt->GetName ();
	if (Right == "")
	{
		Right = "Unnamed";
	}

	// strip quotes from the entity name
	stripQuotes ( Right );

	LeftLen = Left.GetLength() ;
	Util_GetEndStringValue( Left, &LeftVal, &LeftLen ) ;
	RightLen = Right.GetLength() ;
	Util_GetEndStringValue( Right, &RightVal, &RightLen ) ;

	if( Left.Left(LeftLen) == Right.Left(RightLen) )
	{
		return LeftVal - RightVal ;
	}

	return Left.Compare( Right ) ;
}/* ::compare */


void CEntitiesDialog::FillInDialog()
{
	int			CurrentEnt;
	int			NumberOfEnts = (*m_pEntityArray).GetSize();
//	int			RealIndex = 0;
	int			ArrayIndex = 0 ;
	geBoolean	bValidSelection = FALSE ;
	int			VisibleEnt = 0 ;
	PSortStruct	pSortArray ;
	int			Index ;
	CString EntityName;

	// clear our main stuff
	m_EntityCombo.ResetContent();

	// Get a qualified list count
	for( CurrentEnt = 0; CurrentEnt < NumberOfEnts; CurrentEnt++ ) 
	{
		CEntity *pEnt = NULL;

		// get the name
		pEnt = &((*m_pEntityArray)[CurrentEnt]);

		if( pEnt->IsVisible() )
		{
			VisibleEnt++ ;
		}
	}

	pSortArray = (PSortStruct)geRam_Allocate(sizeof(SSortStruct)*VisibleEnt);
	if( pSortArray == NULL )
		return ;	//We're called from OnInitDialog--the best err handling we can do is leave

	// Get a qualified list
	for( CurrentEnt = 0; CurrentEnt < NumberOfEnts; CurrentEnt++ ) 
	{
		CEntity *pEnt;

		// get the name
		pEnt = &((*m_pEntityArray)[CurrentEnt]);

		if( pEnt->IsVisible() )
		{
			pSortArray[ArrayIndex].pEnt = pEnt ;
			pSortArray[ArrayIndex].nItem = CurrentEnt ;
			ArrayIndex++ ;
		}
	}// Qualified List

	qsort( pSortArray, VisibleEnt, sizeof( SSortStruct ), compare ) ;

	for( CurrentEnt=0; CurrentEnt < VisibleEnt; CurrentEnt++ )
	{
		EntityName = pSortArray[CurrentEnt].pEnt->GetName() ;
		if (EntityName == "")
		{
			EntityName = "Unnamed";
		}
		// strip quotes from the entity name
		stripQuotes (EntityName);
		Index = m_EntityCombo.AddString( EntityName );
		m_EntityCombo.SetItemData (Index, pSortArray[CurrentEnt].nItem );
		if( pSortArray[CurrentEnt].nItem == mCurrentEntity )
		{
			m_EntityCombo.SetCurSel( Index );
			bValidSelection = TRUE ;
		}
	}// Fill the list

	// if we have no entities set to nothing.
	if( !NumberOfEnts )
	{
		mCurrentEntity = LB_ERR;
	}
	else if( bValidSelection == FALSE ) // None of the entities populated were selected
	{
		m_EntityCombo.SetCurSel( 0 );	// Select the first one
		mCurrentEntity = m_EntityCombo.GetItemData( m_EntityCombo.GetCurSel( ) ) ;
		if( mCurrentEntity != LB_ERR )
			pDoc->SetSelectedEntity( mCurrentEntity ) ;
	}
	geRam_Free ( pSortArray ) ;

	// fill in the key/value stuff for the current entity
	FillInKeyValuePairs (0);
}/* CEntitiesDialog::FillInDialog */

//================================================================================
// This function fills in the key value pairs for the current dialog.
//================================================================================
void CEntitiesDialog::FillInKeyValuePairs(int Selection)
{
	// now get the actual entity number from that
	int Entity ;

//ASSERT( m_EntityCombo.GetItemData( m_EntityCombo.GetCurSel( ) ) == (DWORD)Entity ) ;
	Entity = m_EntityCombo.GetItemData( m_EntityCombo.GetCurSel( ) ) ;	//	old gedit
//	Entity = pDoc->m_iCurrentEntity;	//	g3dc experiment
	// now go through that entity and add key/value pairs
	// to the dialog
	m_PropertiesList.ResetContent();

	// what do we have here?
	if( Entity == LB_ERR ) 
	{
		mCurrentKey = LB_ERR;
		return;
	}

	CEntity *Ent = &((*m_pEntityArray)[Entity]);
	EntityPropertiesList const *pProps;

	// Get sorted list of published key/value pairs
	CString EntityClassname = Ent->GetClassname ();
	pProps = EntityTable_GetEntityPropertiesFromName (pDoc->m_pLevelMgr->GetEntityDefs (pDoc->GetLevel()), EntityClassname, ET_PUBLISHED);
	if (pProps != NULL)
	{
		// Add key/value pairs to the listbox
		for (int iProp = 0; iProp < pProps->NumProps; iProp++)
		{
			EntityProperty const *p;

			p = &(pProps->Props[iProp]);
			if (p->KeyNum == -1)
			{
				// a key number of -1 indicates the end of valid keys...
				break;
			}

			// if this key doesn't exist for this entity, then add it
			// and its default value...
			CString TheVal;
			if (!Ent->GetKeyValue (p->pKey, TheVal))
			{
				Ent->SetKeyValue (p->pKey, p->pValue);
			}

			if (p->published)
			{
				// add it to the listbox
				m_PropertiesList.AddString (p->pKey);
			}
		}
		EntityTable_ReleaseEntityProperties (pProps);
	}
	else
	{
		/*
		  We know nothing about this entity's type.
		  We have the key/value pairs, but we don't know what's supposed
		  to be published.  So we'll display "unknown type".
		*/
		m_PropertiesList.AddString ("Unknown Entity Type");
		Selection = LB_ERR;
	}

	// set the current
	m_PropertiesList.SetCurSel (Selection);
}


void CEntitiesDialog::OnSelchangeEntitylist()
{
	// get the current selection from the entity list
	int CurrentSelection = m_EntityCombo.GetCurSel();

	// we have an error
	if( CurrentSelection == LB_ERR )
	{
		mCurrentEntity = LB_ERR;
	}
	else 
	{
		// now get the actual entity number from that
		mCurrentEntity = m_EntityCombo.GetItemData( CurrentSelection );
		//	Set this as the current entity...
		pDoc->SetSelectedEntity( mCurrentEntity ) ;
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );
	}

//	FillInKeyValuePairs (0);

}

void CEntitiesDialog::OnDblclkPropertieslist() 
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();

	// Double-click on the item...Edit it.
	mCurrentKey = m_PropertiesList.GetCurSel ();

	if (mCurrentKey != LB_ERR)
	{
		CEntity *Ent = &((*m_pEntityArray)[mCurrentEntity]);
		CString EntityTypeName = Ent->GetClassname ();
		CString KeyName;
		CString TheValue;

		m_PropertiesList.GetText (mCurrentKey, KeyName);
		Ent->GetKeyValue (KeyName, TheValue);

		
		TopType eType;
		
		if (EntityTable_GetEntityPropertyType (pDoc->m_pLevelMgr->GetEntityDefs (pDoc->GetLevel()), EntityTypeName, KeyName, &eType))
		{
			CDialog *pEditDialog = NULL;

			// Create the dialog that's appropriate for this type....
			switch (eType)
			{
				case T_INT :
					pEditDialog = new CIntKeyEditDlg (this, KeyName, &TheValue);
					break;
				case T_FLOAT :
					pEditDialog = new CFloatKeyEditDlg (this, KeyName, &TheValue);
					break;
				case T_COLOR :
					pEditDialog = new CColorKeyEditDlg (this, KeyName, &TheValue);
					break;
				case T_POINT :
					pEditDialog = new CPointKeyEditDlg (this, KeyName, &TheValue);
					break;
				case T_STRING :
					pEditDialog = new CKeyEditDlg (this, KeyName, &TheValue);
					break;
				case T_MODEL :
					pEditDialog = new CModelKeyEditDlg (this, pDoc->m_pLevelMgr->GetModelInfo (pDoc->GetLevel())->Models, KeyName, &TheValue);
					break;
				case T_STRUCT :
					pEditDialog = new CStructKeyEditDlg (this, *Ent, KeyName, m_pEntityArray, &TheValue, pDoc->m_pLevelMgr->GetEntityDefs (pDoc->GetLevel()));
					break;
				case T_BOOLEAN :
					pEditDialog = new CBoolKeyEditDlg (this, KeyName, &TheValue);
					break;
				case T_PTR :
				default :
					// bad or unknown type
					assert (0);
					break;
			}

			if (pEditDialog != NULL)
			{
				int ModalResult = pEditDialog->DoModal ();

				delete pEditDialog;

				if (ModalResult == IDOK)
				{
					if (MultiEntityFlag)
					{
						// multiple entities--change this property on all of them
						int i;

						for (i = 0; i < m_pEntityArray->GetSize (); ++i)
						{
							CEntity *pEnt;

							pEnt = &(*m_pEntityArray)[i];
							if (pEnt->IsSelected ())
							{
								pEnt->SetKeyValue (KeyName, TheValue);
							}
						}
					}
					else
					{
						// update
						Ent->SetKeyValue (KeyName, TheValue);
					}
					FillInKeyValuePairs(mCurrentKey);
				}
			}
		}
	}
}

int CEntitiesDialog::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex) 
{
	switch (nKey)
	{
		case VK_SPACE :
		case VK_RETURN :
			this->OnDblclkPropertieslist ();
			return -2;
		default :
			return CDialog::OnVKeyToItem(nKey, pListBox, nIndex);
	}
}

void CEntitiesDialog::OnOK (void)
{
	this->OnDblclkPropertieslist ();
}


void CEntitiesDialog::OnCancel()
{
	return;
}


static void FillTextBuffer
	(
	  HDC hdc,
	  char *Buffer,
	  char const *Text,
	  int MaxWidth
	)
{
	int len = strlen (Text);
	SIZE size;

	do
	{
		::GetTextExtentPoint32 (hdc, Text, len, &size);
		if (size.cy > MaxWidth)
		{
			--len;
		}
	} while ((size.cy > MaxWidth) && (len > 0));

	if (len > 0)
	{
		strncpy (Buffer, Text, len);
	}
	Buffer[len] = '\0';
}

void CEntitiesDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis) 
{
	if (nIDCtl != IDC_PROPERTIESLIST)
	{
		CDialog::OnDrawItem(nIDCtl, lpdis);
		return;
	}

	// don't worry about invalid items (yet)
	if (lpdis->itemID == -1)
	{
		// Don't call CDialog::OnDrawItem with an invalid item...
		// it will crash.
//		CDialog::OnDrawItem(nIDCtl, lpdis);
		return;
	}


	// we'll start out by just drawing the entire item every time
	char KeyText[255];
	char ValueText[255];
	int rectWidth = lpdis->rcItem.right - lpdis->rcItem.left;
	int halfWidth = rectWidth/2;
	int maxTextWidth = halfWidth - 2;  // 2 pixels on each side...
	int leftX = lpdis->rcItem.left + 2;
	int rightX = leftX + halfWidth + 2;
	
	RECT rectKey = lpdis->rcItem;
	rectKey.right = rightX - 2;

	RECT rectValue = lpdis->rcItem;
	rectValue.left = rightX - 2;

	// opaque inside of the box...
	++(rectKey.left);
	++(rectKey.top);
//	--(rectKey.right);
	--(rectKey.bottom);

//	++(rectValue.left);
	++(rectValue.top);
	--(rectValue.right);
	--(rectValue.bottom);


	CEntity *pEnt = &((*m_pEntityArray)[mCurrentEntity]);
	CString KeyName, Value;

	m_PropertiesList.GetText (lpdis->itemID, KeyName);
	pEnt->GetKeyValue (KeyName, Value);

	FillTextBuffer (lpdis->hDC, KeyText, KeyName, maxTextWidth);
	FillTextBuffer (lpdis->hDC, ValueText, Value, maxTextWidth);

	COLORREF OldBkColor;
	COLORREF OldTextColor;

	// draw the text
	if (lpdis->itemState & ODS_SELECTED)
	{
		// need to do inverted text...
		OldTextColor = ::SetTextColor (lpdis->hDC, GetSysColor (COLOR_HIGHLIGHTTEXT));
		OldBkColor = ::SetBkColor (lpdis->hDC, GetSysColor (COLOR_HIGHLIGHT));
	}
	else
	{
	    OldTextColor = ::SetTextColor (lpdis->hDC, GetSysColor (COLOR_WINDOWTEXT));
		OldBkColor = ::SetBkColor (lpdis->hDC, GetSysColor (COLOR_WINDOW));
	}

	int fuOptions = (ETO_CLIPPED | ETO_OPAQUE);

	::ExtTextOut (lpdis->hDC, leftX, lpdis->rcItem.top+1, fuOptions, &rectKey, KeyText, strlen (KeyText), NULL);
	::ExtTextOut (lpdis->hDC, rightX, lpdis->rcItem.top+1, fuOptions, &rectValue, ValueText, strlen(ValueText), NULL);

	::SetTextColor (lpdis->hDC, OldTextColor);
	::SetBkColor (lpdis->hDC, OldBkColor);

	// and the rectangles
	HBRUSH OldBrush = (HBRUSH)(::SelectObject (lpdis->hDC, GetStockObject (NULL_BRUSH)));
	::Rectangle (lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
	::Rectangle (lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.left+halfWidth, lpdis->rcItem.bottom);
	::SelectObject (lpdis->hDC, OldBrush);
}
/* EOF: EntitiesDialog.cpp */
