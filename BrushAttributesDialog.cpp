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
/*Genesis3D Version 1.1 released November 15, 1999                            */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Prepared for GEditPro ver. 0.7, Nov. 2, 2002											*/
/****************************************************************************************/
#include "stdafx.h"
#include "Globals.h"
#include "BrushAttributesDialog.h"
#include "EntityTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	IDRB_SOLID,
	IDRB_CLIP,
	IDRB_EMPTY,
	IDRB_WINDOW,
	IDRB_HINT,
	IDRB_SUBTRACT
} ;

CBrushAttributesDialog::CBrushAttributesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushAttributesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBrushAttributesDialog)
	m_Name = _T("");
	m_HullSize = 1.0f;
	m_Wavy = FALSE;
	m_Area = FALSE;
	m_Translucent = FALSE;
	m_BrushType = -1;
	m_Detail = FALSE;
	m_Hollow = FALSE;
	m_Flocking = FALSE;
	m_Sheet = FALSE;
	//}}AFX_DATA_INIT

	CDialog::Create(IDD, pParent);

//	SetDialogFields ();
	UpdateData (FALSE);
}



void CBrushAttributesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrushAttributesDialog)
	DDX_Control(pDX, IDC_BRUSH_ALL_SAME, m_btnMakeAllSame);
	DDX_Control(pDX, IDC_CONTENTSLIST, m_ContentsList);
	DDX_Control(pDX, IDC_HULLSIZE, m_EditHullsize);
	DDX_Control(pDX, IDC_LBLHULLSIZE, m_LblHullsize);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_HULLSIZE, m_HullSize);
	DDV_MinMaxFloat(pDX, m_HullSize, 1.0f, 30000.f);
	DDX_Check(pDX, IDC_BRUSHWAVY, m_Wavy);
	DDX_Check(pDX, IDC_BRUSHAREA, m_Area);
	DDX_Check(pDX, IDC_BRUSHTRANSLUCENT, m_Translucent);
	DDX_Radio(pDX, IDC_BRUSHSOLID, m_BrushType);
	DDX_Check(pDX, IDC_BRUSHDETAIL, m_Detail);
	DDX_Check(pDX, IDC_BRUSHHOLLOW, m_Hollow);
	DDX_Check(pDX, IDC_BRUSHFLOCKING, m_Flocking);
	DDX_Check(pDX, IDC_BRUSHSHEET, m_Sheet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrushAttributesDialog, CDialog)
	//{{AFX_MSG_MAP(CBrushAttributesDialog)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_BRUSHSOLID, OnBrushsolid)
	ON_BN_CLICKED(IDC_BRUSHCLIP, OnBrushclip)
	ON_BN_CLICKED(IDC_BRUSHWINDOW, OnBrushwindow)
	ON_BN_CLICKED(IDC_BRUSHHINT, OnBrushhint)
	ON_BN_CLICKED(IDC_BRUSHSUBTRACT, OnBrushsubtract)
	ON_BN_CLICKED(IDC_BRUSHHOLLOW, OnBrushhollow)
	ON_BN_CLICKED(IDC_BRUSHEMPTY, OnBrushempty)
	ON_BN_CLICKED(IDC_BRUSHTRANSLUCENT, OnBrushtranslucent)
	ON_BN_CLICKED(IDC_BRUSHFLOCKING, OnBrushflocking)
	ON_BN_CLICKED(IDC_BRUSHSHEET, OnBrushsheet)
	ON_EN_KILLFOCUS(IDC_HULLSIZE, OnKillfocusHullsize)
	ON_EN_KILLFOCUS(IDC_NAME, OnKillfocusName)
	ON_BN_CLICKED(IDC_BRUSH_ALL_SAME, OnBrushAllSame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



						//	post 0.57
bool CBrushAttributesDialog::ThisDocIsValid(CGEditProDoc *pDoc)
{
	if (!pDoc) 
	{
		#ifdef _DEBUG			//	post 0.57
			AfxMessageBox("BrushAttributes -- No m_pDoc");
		#endif	
		return false;
	}
	if (pDoc->DocVarsGetPointer()->m_pSelBrushes == NULL) return false;

	if (GetParent() == NULL) return false;


	return true;
}





void CBrushAttributesDialog::EnableTranslucency (void)
{
	BOOL Enabled;

	Enabled = m_Translucent;
}

void CBrushAttributesDialog::EnableHullsize (void)
{
	BOOL Enabled;

	Enabled = m_Hollow;
	if (m_LblHullsize)	//	post 0.57
		m_LblHullsize.EnableWindow (Enabled);
	
	if (m_EditHullsize)	//	post 0.57
		m_EditHullsize.EnableWindow (Enabled);
}


BOOL CBrushAttributesDialog::OnInitDialog()	//	modified post 0.57
{
	m_pDoc = CGlobals::GetActiveDocument();


	if (ThisDocIsValid(m_pDoc))	//	post 0.57
	{
		if (m_pDoc->m_pMainFrame)	//	post 0.57
			m_pDoc->m_pMainFrame->HideAllPropDialogsBut(this);

		SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
		this->ShowWindow(SW_SHOW);
	}

	CDialog::OnInitDialog ();


	return TRUE;
}

	//	modified post 0.57
void CBrushAttributesDialog::OnBrushAllSame() 
{

	GetDlgItem(IDC_NAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_BRUSHSOLID)->EnableWindow(TRUE);
	GetDlgItem(IDC_BRUSHCLIP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BRUSHEMPTY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BRUSHWINDOW)->EnableWindow(TRUE);
	GetDlgItem(IDC_BRUSHHINT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BRUSHSUBTRACT)->EnableWindow(TRUE);
	GetDlgItem(IDC_HULLSIZE)->EnableWindow(TRUE);
	GetDlgItem( IDC_BRUSHTRANSLUCENT )->EnableWindow( TRUE ) ;
	GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( TRUE ) ;
	GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( TRUE ) ;
	GetDlgItem( IDC_BRUSHAREA )->EnableWindow( TRUE ) ;
	GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( TRUE );
	GetDlgItem(IDC_BRUSHFLOCKING)->EnableWindow(TRUE);
	GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONTENTSLIST)->EnableWindow(TRUE);	
	
	SetDialogFields ();
	UpdateData (FALSE);
	
}

	//	modified post 0.57
void CBrushAttributesDialog::PrepAllTheSame()
{

	GetDlgItem(IDC_BRUSH_ALL_SAME)->EnableWindow(TRUE);

	GetDlgItem(IDC_NAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BRUSHSOLID)->EnableWindow(FALSE);
	GetDlgItem(IDC_BRUSHCLIP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BRUSHEMPTY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BRUSHWINDOW)->EnableWindow(FALSE);
	GetDlgItem(IDC_BRUSHHINT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BRUSHSUBTRACT)->EnableWindow(FALSE);
	GetDlgItem(IDC_HULLSIZE)->EnableWindow(FALSE);
	GetDlgItem( IDC_BRUSHTRANSLUCENT )->EnableWindow( FALSE ) ;
	GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( FALSE ) ;
	GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( FALSE ) ;
	GetDlgItem( IDC_BRUSHAREA )->EnableWindow( FALSE ) ;
	GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( FALSE );
	GetDlgItem(IDC_BRUSHFLOCKING)->EnableWindow(FALSE);
	GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONTENTSLIST)->EnableWindow(FALSE);

}

	//	modified post 0.57
bool CBrushAttributesDialog::UpdateBrushAttributes(CGEditProDoc *pDoc)
{
	m_pDoc = pDoc;

	if (ThisDocIsValid(m_pDoc))	//	post 0.57
	{
		int selectedBrushes = SelBrushList_GetSize (m_pDoc->DocVarsGetPointer()->m_pSelBrushes);
		
		if (selectedBrushes > 1)
			PrepAllTheSame();

		if (selectedBrushes == 1)
		{
			SetDialogFields ();
			UpdateData (FALSE);
			GetDlgItem(IDC_BRUSH_ALL_SAME)->EnableWindow(FALSE);
		}
	}

					//	Restore focus to active view
	CMDIChildWnd *pMDIChild	=(CMDIChildWnd *)pDoc->m_pMainFrame->MDIGetActive();
	if(pMDIChild)
	{
		CView	*cv	=(CView *)pMDIChild->GetActiveView();
		if( cv)
			cv->SetFocus() ;
	}

	return true;
}


void CBrushAttributesDialog::ShowDialog()
{
	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);
	
//	SetDialogFields ();
//	UpdateData (FALSE);

}



int CBrushAttributesDialog::BrushFlagsToIndex( void )
{
	m_pDoc = CGlobals::GetActiveDocument();	// post 0.57

	if (ThisDocIsValid(m_pDoc))	//	post 0.57
	{	
		Brush *pBrush = NULL;
		pBrush = SelBrushList_GetBrush (m_pDoc->DocVarsGetPointer()->m_pSelBrushes, 0);
		if (pBrush)
		{
			if( Brush_IsSolid (pBrush) )	return IDRB_SOLID ;
			if( Brush_IsClip (pBrush) )		return IDRB_CLIP ;
			if( Brush_IsWindow (pBrush) )	return IDRB_WINDOW ;
			if( Brush_IsHint (pBrush ) )	return IDRB_HINT ;
			if( Brush_IsSubtract (pBrush) )	return IDRB_SUBTRACT ;
			if( Brush_IsEmpty (pBrush) )	return IDRB_EMPTY ;
		}
	}

	return IDRB_SOLID;	// nothing else, so force to solid...
}

void CBrushAttributesDialog::SetFlagsToBrushType( void )
{
	m_pDoc = CGlobals::GetActiveDocument();

	if (ThisDocIsValid(m_pDoc))	//	post 0.57
	{
		int i;
		int NumBrushes = SelBrushList_GetSize (m_pDoc->DocVarsGetPointer()->m_pSelBrushes);
		
		for (i = 0; i < NumBrushes; ++i)
		{
			Brush *pBrush = NULL;
			pBrush = SelBrushList_GetBrush (m_pDoc->DocVarsGetPointer()->m_pSelBrushes, i);
			if (pBrush)
			{
				switch( m_BrushType )
				{
					case IDRB_SOLID	:		Brush_SetSolid (pBrush, GE_TRUE) ;		break ;
					case IDRB_CLIP :		Brush_SetClip (pBrush, GE_TRUE) ;		break ;
					case IDRB_WINDOW :		Brush_SetWindow (pBrush, GE_TRUE) ;		break ;
					case IDRB_HINT :		Brush_SetHint (pBrush, GE_TRUE) ;		break ;
					case IDRB_SUBTRACT :	Brush_SetSubtract (pBrush, GE_TRUE) ;	break ;
					case IDRB_EMPTY	:		Brush_SetEmpty (pBrush, GE_TRUE) ;		break ;
					default :
						assert( 0 ) ;
						break ;
				}
			}
		}
	}
}

void CBrushAttributesDialog::BrushOptionsFromType( void ) 
{
	m_pDoc = CGlobals::GetActiveDocument();

	if (ThisDocIsValid(m_pDoc))	//	post 0.57
	{
		SetFlagsToBrushType( ) ;	// Set Main Type flag

		switch( m_BrushType )
		{
		case IDRB_EMPTY :
			GetDlgItem( IDC_BRUSHTRANSLUCENT )->EnableWindow( TRUE ) ;
			GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( TRUE ) ;
			GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( TRUE ) ;
			GetDlgItem( IDC_BRUSHAREA )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( TRUE );
			m_Area = FALSE ;
			break ;

		case IDRB_SOLID :
			GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( TRUE ) ;
			GetDlgItem( IDC_BRUSHAREA )->EnableWindow( TRUE ) ;	
			GetDlgItem( IDC_BRUSHTRANSLUCENT )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( TRUE );

			m_Translucent = FALSE ;
			m_Wavy = FALSE ;
			break ;

		case IDRB_WINDOW :
			GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHTRANSLUCENT )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHAREA )->EnableWindow( FALSE ) ;	
			GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( FALSE );
			m_Detail = TRUE ;
			m_Translucent = TRUE ;
			m_Wavy = FALSE ;
			m_Area = FALSE ;
			break ;

		case IDRB_CLIP :
		case IDRB_HINT :
		case IDRB_SUBTRACT :
			GetDlgItem( IDC_BRUSHTRANSLUCENT )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHDETAIL )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHWAVY )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHAREA )->EnableWindow( FALSE ) ;
			GetDlgItem( IDC_BRUSHSHEET )->EnableWindow( FALSE );
			m_Detail = FALSE ;
			m_Wavy = FALSE ;
			m_Area = FALSE ;
			m_Translucent = FALSE ;
			break ;
		}

		int NumBrushes = SelBrushList_GetSize (m_pDoc->DocVarsGetPointer()->m_pSelBrushes);
		for (int i = 0; i < NumBrushes; ++i)
		{
			Brush *pBrush = NULL;
			pBrush = SelBrushList_GetBrush (m_pDoc->DocVarsGetPointer()->m_pSelBrushes, i);
			if (pBrush)
			{
				switch( m_BrushType )
				{
				case IDRB_EMPTY :
					Brush_SetArea (pBrush, GE_FALSE);
					break ;

				case IDRB_SOLID :
					Brush_SetTranslucent (pBrush, GE_FALSE);
					Brush_SetWavy (pBrush, GE_FALSE);
					//				Brush_SetArea (pBrush, GE_FALSE);
					break ;

				case IDRB_WINDOW :
					Brush_SetWavy (pBrush, GE_FALSE);
					Brush_SetArea (pBrush, GE_FALSE);
					Brush_SetTranslucent (pBrush, GE_TRUE);
					Brush_SetDetail (pBrush, GE_TRUE);
					Brush_SetSheet (pBrush, GE_FALSE);
					break ;

				case IDRB_CLIP :
				case IDRB_HINT :
				case IDRB_SUBTRACT :
					Brush_SetTranslucent (pBrush, GE_FALSE);
					Brush_SetDetail (pBrush, GE_FALSE);
					Brush_SetWavy (pBrush, GE_FALSE);
					Brush_SetArea (pBrush, GE_FALSE);
					Brush_SetSheet (pBrush, GE_FALSE);
					break ;
				}
			}
		}
	}

	EnableTranslucency ();
	EnableHullsize ();
}


void CBrushAttributesDialog::AssignCurrentToValues()
{
	m_pDoc = CGlobals::GetActiveDocument();

	if (ThisDocIsValid(m_pDoc))	//	post 0.57
	{
		char const *pName = NULL;
		int NumBrushes = SelBrushList_GetSize (m_pDoc->DocVarsGetPointer()->m_pSelBrushes);
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
			Brush *pBrush = NULL;
			pBrush = SelBrushList_GetBrush (m_pDoc->DocVarsGetPointer()->m_pSelBrushes, i);
			if (pBrush)
			{
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
				Brush_SetTranslucent (pBrush, m_Translucent);
				Brush_SetFlocking (pBrush, m_Flocking);
				Brush_SetSheet (pBrush, m_Sheet);

				if (m_Hollow)		
				{
					Brush_SetHullSize (pBrush, m_HullSize);
				}

				Brush_SetUserFlags (pBrush, UserFlags);

				Brush_UpdateChildFaces(pBrush);
			}
		}
		m_pDoc->SetModifiedFlag( ) ;
	}
}

void CBrushAttributesDialog::AssignCurrentToViews()
{
	m_pDoc = CGlobals::GetActiveDocument();

	if (ThisDocIsValid(m_pDoc))	//	post 0.57
	{
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		m_pDoc->UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL); // does update ortho views
		//	m_pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);	//	won't update orthoviews
	}
}


void CBrushAttributesDialog::SetDialogFields (void)
{
	m_pDoc = CGlobals::GetActiveDocument();

	if (ThisDocIsValid(m_pDoc))	//	post 0.57
	{
		Brush *pBrush = NULL;
		pBrush = SelBrushList_GetBrush (m_pDoc->DocVarsGetPointer()->m_pSelBrushes, 0);
	
		if (pBrush)
		{
			m_Wavy			=Brush_IsWavy (pBrush)			? TRUE : FALSE ;
			m_Area			=Brush_IsArea (pBrush)			? TRUE : FALSE ;	
			m_Translucent	=Brush_IsTranslucent(pBrush)	? TRUE : FALSE ;
			m_Detail		=Brush_IsDetail (pBrush)		? TRUE : FALSE ;
			m_Hollow		=Brush_IsHollow (pBrush)		? TRUE : FALSE ;
			m_Flocking		=Brush_IsFlocking (pBrush)		? TRUE : FALSE ;
			m_Sheet			=Brush_IsSheet (pBrush)			? TRUE : FALSE ;
			
			m_Name		=(CString)Brush_GetName (pBrush);
			m_HullSize	=Brush_GetHullSize (pBrush);
			
			if (m_HullSize < 1.0f)
			{
				m_HullSize = 1.0f;
			}
			BrushColor	=Brush_GetColor (pBrush);
			m_BrushType = BrushFlagsToIndex( ) ;
			SetFlagsToBrushType() ;
			BrushOptionsFromType () ;
			
			// set user contents flags...
			if(m_ContentsList)			//	post 0.57
				m_ContentsList.ResetContent ();

			EnableTranslucency ();
			EnableHullsize ();
		}
	}
}

void CBrushAttributesDialog::PostNcDestroy() 
{
	m_pDoc = CGlobals::GetActiveDocument();
	
	
	if (m_pDoc)	//	Don't use ThisDocIsValid() here
	{
		;;
	}
//	delete this;
}

//	Make it possible for the document to update the attributes
//	in the dialog when we switch focus to a new current brush...
void CBrushAttributesDialog::UpdateBrushFocus ()
{
//	SetDialogFields ();
	UpdateData(FALSE);	// Update dialog from members
}



void CBrushAttributesDialog::OnKillfocusName() 
{
	CString	lastValue = m_Name;
	CString currentValue;

	GetDlgItemText(IDC_NAME, currentValue);
	if (currentValue == "")
	{
		this->SetDlgItemText(IDC_NAME, lastValue);
		return;
	}
}


		//	this just prevents an endless loop requesting data if
		//	the user blanks this edit box 
void CBrushAttributesDialog::OnKillfocusHullsize() 
{
	int	lastValue = m_HullSize;
	if (GetDlgItemInt(IDC_HULLSIZE) == NULL)
	{
		this->SetDlgItemInt(IDC_HULLSIZE, lastValue);
		return;
	}

}




/*
void CBrushAttributesDialog::OnCancel()
{
	m_pDoc = CGlobals::GetActiveDocument();

	
	if (m_pDoc != NULL)
	{
		m_pDoc->mpBrushAttributes = NULL;
	}
//	DestroyWindow();
}
*/

void CBrushAttributesDialog::OnCancel()
{
	return;
}

void CBrushAttributesDialog::OnApply() 
{
	m_pDoc = CGlobals::GetActiveDocument();
	
	if (m_pDoc)		//	post 0.57
	{
		if (UpdateData(TRUE))
		{
			AssignCurrentToValues();
			AssignCurrentToViews();
		
			//	post 0.57	for putting 'Rebuild' button on Texture view
//			if(!m_pDoc->m_pLevelMgr->RebuildBspAlways(m_pDoc->GetLevel()))	
			if (!CGlobals::g_bRebuildAlways)
			{
				m_pDoc->m_pMainFrame->SendMessageToDescendants(WM_COMMAND, IDC_BTN_REBUILD_COMMAND);
				CGlobals::g_iRebuildClickCount = 0;
			}
			
			if (m_pDoc->m_pMainFrame)		//	post 0.57
				m_pDoc->m_pMainFrame->UpdateMainControls();			
			//		DestroyWindow();
		}
	}
}

void CBrushAttributesDialog::OnBrushsolid() 
{
	OnRadioButton() ;
}

void CBrushAttributesDialog::OnBrushclip() 
{
	OnRadioButton() ;
}

void CBrushAttributesDialog::OnBrushwindow() 
{
	OnRadioButton() ;
}

void CBrushAttributesDialog::OnBrushhint() 
{
	OnRadioButton() ;
}

void CBrushAttributesDialog::OnBrushsubtract() 
{
	OnRadioButton() ;
	
}

void CBrushAttributesDialog::OnBrushempty() 
{
	OnRadioButton ();
}

void CBrushAttributesDialog::OnRadioButton( void ) 
{
	int OldBrushType ;

	OldBrushType = m_BrushType ;
	
	UpdateData(TRUE);
	if( OldBrushType == IDRB_WINDOW )
	{
		m_Detail = FALSE ;
	}

	BrushOptionsFromType( ) ;
	UpdateData(FALSE);
}

void CBrushAttributesDialog::OnBrushtranslucent() 
{
	m_Translucent = !m_Translucent;
	EnableTranslucency ();
}

void CBrushAttributesDialog::OnBrushhollow() 
{
	m_Hollow = !m_Hollow;
	EnableHullsize ();
}

void CBrushAttributesDialog::OnBrushflocking() 
{
	m_Flocking = !m_Flocking;	
}

void CBrushAttributesDialog::OnBrushsheet() 
{
	m_Sheet = !m_Sheet;	
}

