/****************************************************************************************/
/*  BrushAttributesDialog.cpp                                                           */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax, John Pollard                      */
/*  Description:  Dialog code for brush flags                                           */
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
/*  Copyright (C) 1996-1999 Eclipse Entertainment, L.L.C. All Rights Reserved           */
/*                                                                                      */
/****************************************************************************************/
#include "stdafx.h"
#include "BrushAttributesDialog.h"
#include "FUSIONDoc.h"
#include "EntityTable.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CString TEXT_NUM_BRUSHES = "Number of Brushes Selected: ";

//CBrushAttributesDialog::CBrushAttributesDialog(CFusionDoc *pDoc, CWnd* pParent)
CBrushAttributesDialog::CBrushAttributesDialog(CMainFrame* pMainFrame, CWnd* pParent)
	: CDialog(CBrushAttributesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBrushAttributesDialog)
	m_Name = _T("");
	m_Wavy = FALSE;
	m_Area = FALSE;
	m_BrushType = -1;
	m_Detail = FALSE;
	m_Flocking = FALSE;
	m_Sheet = FALSE;
	m_NumBrushes = TEXT_NUM_BRUSHES;
	//}}AFX_DATA_INIT

	m_pMainFrame = pMainFrame;
//	m_pDoc			=pDoc;

	CDialog::Create(IDD, pParent);

	SetDialogFields ();
	UpdateData (FALSE);
}

/*
void CBrushAttributesDialog::EnableTranslucency (void)
{
	BOOL Enabled;

	Enabled = m_Translucent;
}
*/

/*
void CBrushAttributesDialog::EnableHullsize (void)
{
	BOOL Enabled;

	Enabled = m_Hollow;
	m_LblHullsize.EnableWindow (Enabled);
	m_EditHullsize.EnableWindow (Enabled);
}
*/

void CBrushAttributesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrushAttributesDialog)
	DDX_Control(pDX, IDC_CONTENTSLIST, m_ContentsList);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 30);
	DDX_Check(pDX, IDC_BRUSHWAVY, m_Wavy);
	DDX_Check(pDX, IDC_BRUSHAREA, m_Area);
	DDX_Radio(pDX, IDC_BRUSHSOLID, m_BrushType);
	DDX_Check(pDX, IDC_BRUSHDETAIL, m_Detail);
	DDX_Check(pDX, IDC_BRUSHFLOCKING, m_Flocking);
	DDX_Check(pDX, IDC_BRUSHSHEET, m_Sheet);
	DDX_Text(pDX, IDC_LABEL_NUM_BRUSHES, m_NumBrushes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrushAttributesDialog, CDialog)
	//{{AFX_MSG_MAP(CBrushAttributesDialog)
	ON_BN_CLICKED(IDC_BRUSHSOLID, OnBrushsolid)
	ON_BN_CLICKED(IDC_BRUSHCLIP, OnBrushclip)
	ON_BN_CLICKED(IDC_BRUSHWINDOW, OnBrushwindow)
	ON_BN_CLICKED(IDC_BRUSHHINT, OnBrushhint)
	ON_BN_CLICKED(IDC_BRUSHSUBTRACT, OnBrushsubtract)
	ON_BN_CLICKED(IDC_BRUSHEMPTY, OnBrushempty)
	ON_BN_CLICKED(IDC_BRUSHFLOCKING, OnBrushflocking)
	ON_BN_CLICKED(IDC_BRUSHSHEET, OnBrushsheet)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BRUSHDETAIL, OnBrushdetail)
	ON_BN_CLICKED(IDC_BRUSHAREA, OnBrusharea)
	ON_BN_CLICKED(IDC_BRUSHWAVY, OnBrushwavy)
	ON_EN_KILLFOCUS(IDC_NAME, OnKillfocusName)
	ON_LBN_SELCHANGE(IDC_CONTENTSLIST, OnSelchangeContentslist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CBrushAttributesDialog::BrushFlagsToIndex( Brush *pBrush )
{
//	CFusionDoc *m_pDoc = m_pMainFrame->GetCurrentDoc();
//	if (m_pDoc)
	{
//	Brush *pBrush = SelBrushList_GetBrush (m_pDoc->pSelBrushes, 0);

	if( Brush_IsSolid (pBrush) )	return IDRB_SOLID ;
	if( Brush_IsClip (pBrush) )		return IDRB_CLIP ;
	if( Brush_IsWindow (pBrush) )	return IDRB_WINDOW ;
	if( Brush_IsHint (pBrush ) )	return IDRB_HINT ;
	if( Brush_IsSubtract (pBrush) )	return IDRB_SUBTRACT ;
	if( Brush_IsEmpty (pBrush) )	return IDRB_EMPTY ;
	}
	return IDRB_SOLID;	// nothing else, so force to solid...
}

void CBrushAttributesDialog::SetFlagsToBrushType( void )
{
	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
	pDoc->SetModifiedFlag();
	int i;
	int NumBrushes = SelBrushList_GetSize (pDoc->pSelBrushes);

	for (i = 0; i < NumBrushes; ++i)
	{
		Brush *pBrush = SelBrushList_GetBrush (pDoc->pSelBrushes, i);

		switch( m_BrushType )
		{
			case IDRB_SOLID	:		Brush_SetSolid (pBrush, GE_TRUE) ;	break ;
			case IDRB_CLIP :		Brush_SetClip (pBrush, GE_TRUE) ;		break ;
			case IDRB_WINDOW :		Brush_SetWindow (pBrush, GE_TRUE) ;	break ;
			case IDRB_HINT :		Brush_SetHint (pBrush, GE_TRUE) ;		break ;
			case IDRB_SUBTRACT :	Brush_SetSubtract (pBrush, GE_TRUE) ;	break ;
			case IDRB_EMPTY	:		Brush_SetEmpty (pBrush, GE_TRUE) ;	break ;
			default :
				assert( 0 ) ;
				break ;
		}
	}
	}
}

void CBrushAttributesDialog::BrushOptionsFromType( void ) 
{
//	SetFlagsToBrushType( ) ;	// Set Main Type flag
	
	switch( m_BrushType )
	{
		case IDRB_EMPTY :
			GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( TRUE ) ;
			GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( TRUE ) ;
			GetDlgItem( IDC_BRUSHAREA )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( TRUE );
			GetDlgItem( IDC_BRUSHFLOCKING )->EnableWindow( TRUE );
			m_Area = FALSE ;
			break ;

		case IDRB_SOLID :
			GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( TRUE ) ;
			GetDlgItem( IDC_BRUSHAREA )->EnableWindow( TRUE ) ;	
			GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( TRUE );
			GetDlgItem( IDC_BRUSHFLOCKING )->EnableWindow( TRUE );
//			m_Translucent = FALSE ;
			m_Wavy = FALSE ;
			break ;

		case IDRB_WINDOW :
			GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHAREA )->EnableWindow( FALSE ) ;	
			GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( FALSE );
			GetDlgItem( IDC_BRUSHFLOCKING )->EnableWindow( TRUE );
			m_Detail = TRUE ;
//			m_Translucent = TRUE ;
			m_Wavy = FALSE ;
			m_Area = FALSE ;
			break ;

		case IDRB_CLIP :
		case IDRB_HINT :
		case IDRB_SUBTRACT :
			GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHAREA )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( FALSE );
			GetDlgItem( IDC_BRUSHFLOCKING )->EnableWindow( TRUE );
			m_Detail = FALSE ;
			m_Wavy = FALSE ;
			m_Area = FALSE ;
//			m_Translucent = FALSE ;
			break ;
	}

/*
	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{

	int NumBrushes = SelBrushList_GetSize (pDoc->pSelBrushes);
	for (int i = 0; i < NumBrushes; ++i)
	{
		Brush *pBrush = SelBrushList_GetBrush (pDoc->pSelBrushes, i);

		switch( m_BrushType )
		{
			case IDRB_EMPTY :
				Brush_SetArea (pBrush, GE_FALSE);
				break ;

			case IDRB_SOLID :
//				Brush_SetTranslucent (pBrush, GE_FALSE);
				Brush_SetWavy (pBrush, GE_FALSE);
//				Brush_SetArea (pBrush, GE_FALSE);
				break ;

			case IDRB_WINDOW :
				Brush_SetWavy (pBrush, GE_FALSE);
				Brush_SetArea (pBrush, GE_FALSE);
//				Brush_SetTranslucent (pBrush, GE_TRUE);
				Brush_SetDetail (pBrush, GE_TRUE);
				Brush_SetSheet (pBrush, GE_FALSE);
				break ;

			case IDRB_CLIP :
			case IDRB_HINT :
			case IDRB_SUBTRACT :
//				Brush_SetTranslucent (pBrush, GE_FALSE);
				Brush_SetDetail (pBrush, GE_FALSE);
				Brush_SetWavy (pBrush, GE_FALSE);
				Brush_SetArea (pBrush, GE_FALSE);
				Brush_SetSheet (pBrush, GE_FALSE);
				break ;
		}
	}
	}
//	EnableTranslucency ();
//	EnableHullsize ();
*/
}


void CBrushAttributesDialog::AssignCurrentToValues()
{
	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
	pDoc->SetModifiedFlag();
	char const *pName;
	int NumBrushes = SelBrushList_GetSize (pDoc->pSelBrushes);
	int i;

	// update user flags
	unsigned long UserFlags = 0;
	for (i = 0; i < m_ContentsList.GetCount (); ++i)
	{
		if (m_ContentsList.GetCheck (i) == 1)
		{
			unsigned long Value;
			Value = m_ContentsList.GetItemData (i);
			UserFlags |= Value;
		}
	}

	for (i = 0; i < NumBrushes; ++i)
	{
		Brush *pBrush = SelBrushList_GetBrush (pDoc->pSelBrushes, i);

		pName = Brush_GetName (pBrush);	
		// Flags for the Types (RB's) are always up to date.
		if( strcmp( pName, m_Name ) )
		{
			Brush_SetName (pBrush, m_Name);
		}

		// Turn on any chosen flags
		Brush_SetDetail (pBrush, m_Detail);
		Brush_SetWavy (pBrush, m_Wavy);
		Brush_SetArea (pBrush, m_Area);
//		Brush_SetTranslucent (pBrush, m_Translucent);
		Brush_SetFlocking (pBrush, m_Flocking);
		Brush_SetSheet (pBrush, m_Sheet);

/*
		if (m_Hollow)
		{
			Brush_SetHullSize (pBrush, m_HullSize);
		}
*/
		Brush_SetUserFlags (pBrush, UserFlags);

		Brush_UpdateChildFaces(pBrush);
	}

	pDoc->SetModifiedFlag( ) ;
	}
}

void CBrushAttributesDialog::AssignCurrentToViews()
{
	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
	pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
	}
}

void CBrushAttributesDialog::SetDialogFields (void)
{
	int NumberOfBrushes;
	Brush *pBrush;
	
	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
		NumberOfBrushes = SelBrushList_GetSize (pDoc->pSelBrushes);
//		pBrush = SelBrushList_GetBrush (pDoc->pSelBrushes, 0);
		if (NumberOfBrushes)
			pBrush = SelBrushList_GetBrush (pDoc->pSelBrushes, (NumberOfBrushes-1));
		else
			pBrush = NULL;
	}
	else
	{
		NumberOfBrushes = 0;
		pBrush = NULL;
	}

	CString StringNumberOfBrushes;
	StringNumberOfBrushes.Format("%d",NumberOfBrushes);
	m_NumBrushes = TEXT_NUM_BRUSHES + StringNumberOfBrushes;

	if (pDoc && NumberOfBrushes && pBrush)
	{
		m_Wavy			=Brush_IsWavy (pBrush)			? TRUE : FALSE ;
		m_Area			=Brush_IsArea (pBrush)			? TRUE : FALSE ;	
//		m_Translucent	=Brush_IsTranslucent(pBrush)	? TRUE : FALSE ;
		m_Detail		=Brush_IsDetail (pBrush)		? TRUE : FALSE ;
	//	m_Hollow		=Brush_IsHollow (pBrush)		? TRUE : FALSE ;
		m_Flocking		=Brush_IsFlocking (pBrush)		? TRUE : FALSE ;
		m_Sheet			=Brush_IsSheet (pBrush)			? TRUE : FALSE ;

		m_Name		=Brush_GetName (pBrush);
	/*
		m_HullSize	=Brush_GetHullSize (pBrush);
		if (m_HullSize < 1.0f)
		{
			m_HullSize = 1.0f;
		}
	*/
		BrushColor	=Brush_GetColor (pBrush);
		m_BrushType = BrushFlagsToIndex( pBrush ) ;
		//SetFlagsToBrushType() ;

		BrushOptionsFromType () ;

		// set user contents flags...
		m_ContentsList.ResetContent ();
		unsigned long UserFlags = Brush_GetUserFlags (pBrush);
		ContentsTable *ContentsList = EntityTable_GetContentsList (Level_GetEntityDefs (pDoc->pLevel));

		for (int i = 0; i < ContentsList->nEntries; ++i)
		{
			int Index;
			ContentsTableEntry *pEntry;

			pEntry = &(ContentsList->Entries[i]);

			Index = m_ContentsList.AddString (pEntry->Name);
			if (Index != LB_ERR)
			{
				m_ContentsList.SetItemData (Index, pEntry->Value);
				m_ContentsList.SetCheck (Index, (UserFlags & pEntry->Value) ? 1 : 0);
			}
		}
		EntityTable_FreeContentsList (&ContentsList);

	//	EnableTranslucency ();
	//	EnableHullsize ();

		GetDlgItem( IDC_STATIC1 )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_STATIC2 )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_STATIC3 )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_STATIC4 )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_LABEL_NUM_BRUSHES )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_NAME )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_BRUSHSOLID )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_BRUSHEMPTY )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_BRUSHCLIP )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_BRUSHWINDOW )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_BRUSHHINT )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_BRUSHSUBTRACT )->EnableWindow( TRUE ) ;
		GetDlgItem( IDC_CONTENTSLIST )->EnableWindow( TRUE ) ;

	}
	else
	{
		m_Name		= "";

		m_BrushType = -1;

		m_Wavy			= FALSE ;
		m_Area			= FALSE ;	
//		m_Translucent	= FALSE ;
		m_Detail		= FALSE ;
	//	m_Hollow		= FALSE ;
		m_Flocking		= FALSE ;
		m_Sheet			= FALSE ;

		m_ContentsList.ResetContent();

		GetDlgItem( IDC_STATIC1 )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_STATIC2 )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_STATIC3 )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_STATIC4 )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_LABEL_NUM_BRUSHES )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_NAME )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHSOLID )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHEMPTY )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHCLIP )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHWINDOW )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHHINT )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHSUBTRACT )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHAREA )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( FALSE ) ;
		GetDlgItem( IDC_BRUSHFLOCKING )->EnableWindow( FALSE );
		GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( FALSE );
		GetDlgItem( IDC_CONTENTSLIST )->EnableWindow( FALSE ) ;
	}
}

BOOL CBrushAttributesDialog::OnInitDialog()
{
	if (CDialog::OnInitDialog ())
	{
		CenterWindow( AfxGetMainWnd() );
		SetDialogFields ();

		CEdit* NameEdit = (CEdit*)GetDlgItem(IDC_NAME);
		NameEdit->SetLimitText(255);
		
		return TRUE;
	}
	return FALSE;
}

void CBrushAttributesDialog::PostNcDestroy() 
{
//	if (pDoc != NULL)
	if (m_pMainFrame != NULL)
	{
//		pDoc->mpBrushAttributes=NULL;
		m_pMainFrame->mpBrushAttributes=NULL;
	}
	delete this;
}

//	Make it possible for the document to update the attributes
//	in the dialog when we switch focus to a new current brush...
void CBrushAttributesDialog::UpdateBrushFocus ()
{
	SetDialogFields ();
	UpdateData(FALSE);	// Update dialog from members
}

/*
void CBrushAttributesDialog::OnCancel()
{
	if (pDoc != NULL)
	{
		pDoc->mpBrushAttributes = NULL;
	}
	DestroyWindow();
}
*/

/*
void CBrushAttributesDialog::OnApply() 
{
	if (UpdateData(TRUE))
	{
		AssignCurrentToValues();
		AssignCurrentToViews();
		DestroyWindow();
	}
}
*/

void CBrushAttributesDialog::OnBrushsolid() 
{
	OnRadioButton(IDRB_SOLID) ;
}

void CBrushAttributesDialog::OnBrushclip() 
{
	OnRadioButton(IDRB_CLIP) ;
}

void CBrushAttributesDialog::OnBrushwindow() 
{
	OnRadioButton(IDRB_WINDOW) ;
}

void CBrushAttributesDialog::OnBrushhint() 
{
	OnRadioButton(IDRB_HINT) ;
}

void CBrushAttributesDialog::OnBrushsubtract() 
{
	OnRadioButton(IDRB_SUBTRACT) ;
}

void CBrushAttributesDialog::OnBrushempty() 
{
	OnRadioButton (IDRB_EMPTY);
}

void CBrushAttributesDialog::OnRadioButton( BrushTypeOption NewValue ) 
{
/*
	int OldBrushType ;

	OldBrushType = m_BrushType ;
	
	UpdateData(TRUE);
	if( OldBrushType == IDRB_WINDOW )
	{
		m_Detail = FALSE ;
	}
*/
	m_BrushType = NewValue;

	SetFlagsToBrushType();

	BrushOptionsFromType( ) ;
	UpdateData(FALSE);
}

/*
void CBrushAttributesDialog::OnBrushtranslucent() 
{
	m_Translucent = !m_Translucent;
	EnableTranslucency ();
}
*/

/*
void CBrushAttributesDialog::OnBrushhollow() 
{
	m_Hollow = !m_Hollow;
	EnableHullsize ();
}
*/

static geBoolean SetFlocking (Brush *pBrush, void *lParam)
{
	BOOL *pFlocking = (BOOL *)lParam;

	Brush_SetFlocking (pBrush, *pFlocking);
	return GE_TRUE;
}


void CBrushAttributesDialog::OnBrushflocking() 
{
	m_Flocking = !m_Flocking;	

	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
	pDoc->SetModifiedFlag();
	UpdateData (TRUE);
	SelBrushList_Enum (pDoc->pSelBrushes, ::SetFlocking, &m_Flocking);
	AssignCurrentToViews ();
	}
}

static geBoolean SetSheet (Brush *pBrush, void *lParam)
{
	BOOL *pSheet = (BOOL *)lParam;

	Brush_SetSheet (pBrush, *pSheet);
	return GE_TRUE;
}

void CBrushAttributesDialog::OnBrushsheet() 
{
	m_Sheet = !m_Sheet;	

	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
	pDoc->SetModifiedFlag();
	UpdateData (TRUE);
	SelBrushList_Enum (pDoc->pSelBrushes, ::SetSheet, &m_Sheet);
	AssignCurrentToViews ();
	}
}

void CBrushAttributesDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_pMainFrame->mpBrushAttributes = NULL;

	CDialog::OnClose();
}

static geBoolean SetDetail (Brush *pBrush, void *lParam)
{
	BOOL *pDetail = (BOOL *)lParam;

	Brush_SetDetail (pBrush, *pDetail);
	return GE_TRUE;
}

void CBrushAttributesDialog::OnBrushdetail() 
{
	m_Detail = !m_Detail;	

	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
	pDoc->SetModifiedFlag();
	UpdateData (TRUE);
	SelBrushList_Enum (pDoc->pSelBrushes, ::SetDetail, &m_Detail);
	AssignCurrentToViews ();
	}
}

static geBoolean SetArea (Brush *pBrush, void *lParam)
{
	BOOL *pArea = (BOOL *)lParam;

	Brush_SetArea (pBrush, *pArea);
	return GE_TRUE;
}

void CBrushAttributesDialog::OnBrusharea() 
{
	m_Area = !m_Area;	

	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
	pDoc->SetModifiedFlag();
	UpdateData (TRUE);
	SelBrushList_Enum (pDoc->pSelBrushes, ::SetArea, &m_Area);
	AssignCurrentToViews ();
	}
}

static geBoolean SetWavy (Brush *pBrush, void *lParam)
{
	BOOL *pWavy = (BOOL *)lParam;

	Brush_SetWavy (pBrush, *pWavy);
	return GE_TRUE;
}

void CBrushAttributesDialog::OnBrushwavy() 
{
	m_Wavy = !m_Wavy;	

	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
		pDoc->SetModifiedFlag();
		UpdateData (TRUE);
		SelBrushList_Enum (pDoc->pSelBrushes, ::SetWavy, &m_Wavy);
		AssignCurrentToViews ();
	}
}

static geBoolean SetName (Brush *pBrush, void *lParam)
{
	CString *pName = (CString *)lParam;

	Brush_SetName (pBrush, *pName);
	return GE_TRUE;
}

void CBrushAttributesDialog::OnKillfocusName() 
{
	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
		if (UpdateData (TRUE))
		{
			pDoc->SetModifiedFlag();
			SelBrushList_Enum (pDoc->pSelBrushes, ::SetName, &m_Name);
			AssignCurrentToViews ();
		}
	}
}

static geBoolean SetUserFlags (Brush *pBrush, void *lParam)
{
	unsigned long *pUserFlags = (unsigned long *)lParam;

	Brush_SetUserFlags (pBrush, *pUserFlags);
	return GE_TRUE;
}

void CBrushAttributesDialog::OnSelchangeContentslist() 
{
	CFusionDoc *pDoc = m_pMainFrame->GetCurrentDoc();
	if (pDoc)
	{
		unsigned long UserFlags = 0;
		for (int i = 0; i < m_ContentsList.GetCount (); ++i)
		{
			if (m_ContentsList.GetCheck (i) == 1)
			{
				unsigned long Value;
				Value = m_ContentsList.GetItemData (i);
				UserFlags |= Value;
			}
		}

		pDoc->SetModifiedFlag();
		UpdateData (TRUE);
		SelBrushList_Enum (pDoc->pSelBrushes, ::SetUserFlags, &UserFlags);
		AssignCurrentToViews ();
	}	
}
