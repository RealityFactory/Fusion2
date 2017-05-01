/****************************************************************************************/
/*  CreateBoxDialog.cpp                                                                 */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, John Moore                                    */
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
/*Genesis3D Version 1.1 released November 15, 1999                            */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/
#include "stdafx.h"
#include "Globals.h"
#include "CreateBoxDialog.h"
#include "units.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




CCreateBoxDialog::CCreateBoxDialog(CWnd	*pParent /*=NULL*/ )
	: CDialog(CCreateBoxDialog::IDD, pParent)
{
	// We want to initialize our data to something
	// reasonable

	pCube = NULL;
	m_recentCube = NULL;


	//{{AFX_DATA_INIT(CCreateBoxDialog)
	m_YSize = 512.0f;		//	must match defaults in BrushTemplate.c
	m_Solid = 1;			//	0=solid  1=hollow
	m_XSizeBot = 512.0f;
	m_XSizeTop = 512.0f;
	m_ZSizeBot = 512.0f;
	m_ZSizeTop = 512.0f;
	m_TCut = FALSE;
	m_Thickness = 16.0f;
	m_TSheet = FALSE;
	//}}AFX_DATA_INIT
	m_minMaxErrorString.Format("Value must be between %d and %d",
		BRUSH_MIN, BRUSH_MAX);
	m_wallThicknessErrorString.Format("Value must be between %d and %d", 
		WALL_THICK_MIN, WALL_THICK_MAX);

	// we want to load up our bitmaps
	mHollowBitmap.LoadBitmap( IDB_HOLLOWBOX );
	mSolidBitmap.LoadBitmap( IDB_SOLIDBOX );

}

CCreateBoxDialog::~CCreateBoxDialog()
{
	// delete our stupid bitmaps
	mHollowBitmap.DeleteObject();
	mSolidBitmap.DeleteObject();
}


void CCreateBoxDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateBoxDialog)
	DDX_Control(pDX, ID_APPLY_BOX_BTN, m_applyBoxBTN);
	DDX_Control(pDX, IDC_CUSTOM_TEMPLATE_BTN, m_customTemplateBTN);
	DDX_Control(pDX, IDC_TCUT, m_CutBtn);
	DDX_Control(pDX, IDC_TSHEET, m_SheetBtn);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Text(pDX, IDC_YSIZE, m_YSize);
	DDX_Radio(pDX, IDC_SOLID, m_Solid);
	DDX_Text(pDX, IDC_XSIZEBOT, m_XSizeBot);
	DDX_Text(pDX, IDC_XSIZETOP, m_XSizeTop);
	DDX_Text(pDX, IDC_ZSIZEBOT, m_ZSizeBot);
	DDX_Text(pDX, IDC_ZSIZETOP, m_ZSizeTop);
	DDX_Check(pDX, IDC_TCUT, m_TCut);
	DDX_Text(pDX, IDC_THICKNESS, m_Thickness);
	DDX_Check(pDX, IDC_TSHEET, m_TSheet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateBoxDialog, CDialog)
	//{{AFX_MSG_MAP(CCreateBoxDialog)
	ON_BN_CLICKED(IDC_SOLID, OnSolid)
	ON_BN_CLICKED(IDC_HOLLOW, OnHollow)
	ON_BN_CLICKED(IDC_Defaults, OnDefaults)
	ON_BN_CLICKED(IDC_TCUT, OnTcut)
	ON_BN_CLICKED(IDC_TSHEET, OnTsheet)
	ON_EN_KILLFOCUS(IDC_XSIZETOP, OnKillfocusXsizetop)
	ON_EN_KILLFOCUS(IDC_ZSIZEBOT, OnKillfocusZsizebot)
	ON_EN_KILLFOCUS(IDC_YSIZE, OnKillfocusYsize)
	ON_EN_KILLFOCUS(IDC_THICKNESS, OnKillfocusThickness)
	ON_BN_CLICKED(ID_APPLY_BOX_BTN, OnApplyBoxBtn)
	ON_EN_KILLFOCUS(IDC_XSIZEBOT, OnKillfocusXsizebot)
	ON_EN_KILLFOCUS(IDC_ZSIZETOP, OnKillfocusZsizetop)
	ON_BN_CLICKED(IDC_CUSTOM_TEMPLATE_BTN, OnCustomTemplateBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateBoxDialog message handlers

// commented out -- obsolete part of gedit code
/*
int	CCreateBoxDialog::DoModal(geBoolean ConvertToMetric, BrushTemplate_Box *pBoxTemplate)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pBoxTemplate = pBoxTemplate;

	m_Solid		= pBoxTemplate->Solid;
	m_TCut		= pBoxTemplate->TCut;
	m_Thickness = pBoxTemplate->Thickness;
	m_XSizeTop	= pBoxTemplate->XSizeTop;
	m_XSizeBot	= pBoxTemplate->XSizeBot;
	m_YSize		= pBoxTemplate->YSize;
	m_ZSizeTop	= pBoxTemplate->ZSizeTop;
	m_ZSizeBot	= pBoxTemplate->ZSizeBot;

	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	return CDialog::DoModal();
}
*/

// new for g3dc
void CCreateBoxDialog::ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Box *pBoxTemplate, CGEditProDoc *pDoc)
{
	m_pDoc = pDoc;
	m_pBoxTemplate = pBoxTemplate;

	m_customTemplate = false;

			//	toggle switch so we render our brush and NOT the Entitiy template box	
	pDoc->m_bTempEnt = FALSE; 

	this->GetDlgItem(IDC_XSIZETOP)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_ZSIZETOP)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_XSIZEBOT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_ZSIZEBOT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_YSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_THICKNESS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SOLID)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HOLLOW)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TSHEET)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_Defaults)->EnableWindow(FALSE);


	m_ConvertToMetric	=ConvertToMetric;

	pDoc->m_pMainFrame->HideAllPropDialogsBut(this);


	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}


	if (!strcmp(pDoc->m_currentTemplateName, "Cube"))
	{
		if (pDoc->m_pCurBrush)
		{
			pCube = pDoc->m_pCurBrush;	
		}
	}

		//	if the current template name is NOT Cube, we probly have
		//	come here from another template type.
	else if (pDoc->m_currentTemplateName != "Cube")
	{
		//	check to see if there is a previous cube brush from the last visit
		if (pDoc->m_pRecentCube)	//	post 0.55
		{			//	reset name 'cause sometimes it gets changed
			Brush_SetName(pDoc->m_pRecentCube, "Cube");	
			pDoc->m_pCurBrush = (Brush*)pDoc->m_pRecentCube;
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);
			Brush_SetName(pDoc->m_pBTemplate, "Cube");	// post 0.58
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			pDoc->UpdateAllViews (UAV_ALL3DVIEWS, NULL);
		}
			//	if no modified Cube brush from the previous visit
		if (!pDoc->m_pRecentCube)
		{
			pCube = BrushTemplate_CreateBox(m_pBoxTemplate);
			
			if (pCube != NULL)
			{
				pDoc->CreateNewTemplateBrush ((Brush*)pCube);
				pDoc->m_pRecentCube = Brush_Clone(pCube);
				Brush_SetName(pDoc->m_pCurBrush, "Cube");
				pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);
				Brush_SetName(m_pDoc->m_pBTemplate, "Cube");	// post 0.58
			}	
		}	//post 0.55
	}		

	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);

	//	set this now so we can come back later
	pDoc->m_currentTemplateName = "Cube";

	UpdateData(TRUE);
}


bool CCreateBoxDialog::UpdateCreateBoxDlg(CGEditProDoc *pDoc)
{
	m_pDoc = pDoc;
	//	post 0.55
		//	if the current template name IS Cube...
	if (!strcmp(pDoc->m_currentTemplateName, "Cube"))
	{
		if ((pDoc->m_pCurBrush != NULL) && !strcmp(Brush_GetName(pDoc->m_pCurBrush), "Cube")
								&& pDoc->m_pCurBrush != pDoc->m_pRecentCube)
		{
				// update the template brush as a clone of pDoc->m_pCurBrush
			pDoc->m_pRecentCube = Brush_Clone(pDoc->m_pCurBrush);
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(m_pDoc->m_pBTemplate, "Cube");	// post 0.58
		}
	}
	//	post 0.55	
	
	
	BrushTemplate_Box *pBoxTemplate = pDoc->m_pLevelMgr->GetBoxTemplate (pDoc->GetLevel());
	m_pBoxTemplate = pBoxTemplate;

	if (!m_customTemplate)
	{
		this->GetDlgItem(IDC_XSIZETOP)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_ZSIZETOP)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_XSIZEBOT)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_ZSIZEBOT)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_YSIZE)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_THICKNESS)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_SOLID)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_HOLLOW)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_TSHEET)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_Defaults)->EnableWindow(FALSE);

		m_applyBoxBTN.EnableWindow(TRUE);
		m_customTemplateBTN.SetCheck(0);
	}

	else
	{
		this->GetDlgItem(IDC_XSIZETOP)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_ZSIZETOP)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_XSIZEBOT)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_ZSIZEBOT)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_YSIZE)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_THICKNESS)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_SOLID)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_HOLLOW)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_TCUT)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_TSHEET)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_Defaults)->EnableWindow(TRUE);

		m_applyBoxBTN.EnableWindow(FALSE);
		m_customTemplateBTN.SetCheck(1);

	}

	m_Solid		= pBoxTemplate->Solid;
	m_TCut		= pBoxTemplate->TCut;
	m_Thickness = pBoxTemplate->Thickness;
	m_XSizeTop	= pBoxTemplate->XSizeTop;
	m_XSizeBot	= pBoxTemplate->XSizeBot;
	m_YSize		= pBoxTemplate->YSize;
	m_ZSizeTop	= pBoxTemplate->ZSizeTop;
	m_ZSizeBot	= pBoxTemplate->ZSizeBot;

	UpdateData(TRUE);

	
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



void	CCreateBoxDialog::dlgFieldsToTexels(void)
{
	m_Thickness	=Units_CentimetersToEngine(m_Thickness);
	m_XSizeTop	=Units_CentimetersToEngine(m_XSizeTop);
	m_XSizeBot	=Units_CentimetersToEngine(m_XSizeBot);
	m_YSize		=Units_CentimetersToEngine(m_YSize);
	m_ZSizeTop	=Units_CentimetersToEngine(m_ZSizeTop);
	m_ZSizeBot	=Units_CentimetersToEngine(m_ZSizeBot);
}

void	CCreateBoxDialog::dlgFieldsToCentimeters(void)
{
	m_Thickness	=Units_FRound(Units_EngineToCentimeters(m_Thickness));
	m_XSizeTop	=Units_FRound(Units_EngineToCentimeters(m_XSizeTop));
	m_XSizeBot	=Units_FRound(Units_EngineToCentimeters(m_XSizeBot));
	m_YSize		=Units_FRound(Units_EngineToCentimeters(m_YSize));
	m_ZSizeTop	=Units_FRound(Units_EngineToCentimeters(m_ZSizeTop));
	m_ZSizeBot	=Units_FRound(Units_EngineToCentimeters(m_ZSizeBot));
}



void CCreateBoxDialog::OnSolid() 
{
	m_Picture.SetBitmap( mSolidBitmap );
	GetDlgItem (IDC_TSHEET)->EnableWindow (TRUE);
	OnOK();
}

void CCreateBoxDialog::OnHollow() 
{
	m_Picture.SetBitmap( mHollowBitmap );
	m_TSheet = FALSE;
	m_SheetBtn.EnableWindow (FALSE);
	m_SheetBtn.SetCheck (0);
	OnOK();
}

BOOL CCreateBoxDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

//	pCube = NULL;		//	post 0.55 commented out 

	if( m_Solid == 0 )
		m_Picture.SetBitmap( mSolidBitmap );
	else
		m_Picture.SetBitmap( mHollowBitmap );

	this->GetDlgItem(IDC_CUSTOM_TEMPLATE_BTN)->SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateBoxDialog::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	
	}
	else
	{	// loading code
	}
}

void CCreateBoxDialog::OnDefaults() 
{
	m_Picture.SetBitmap( mHollowBitmap );
	m_XSizeTop = 512.0;
	m_ZSizeTop = 512.0;
	m_XSizeBot = 512.0;
	m_ZSizeBot = 512.0;
	m_YSize = 512.0;
	m_Solid = 1;
	m_Thickness = 16.0;
	m_TCut = FALSE;
	m_TSheet = FALSE;
	UpdateData(FALSE);
	OnOK();
}

void CCreateBoxDialog::OnTcut() 
{
	if (m_CutBtn.GetCheck ())
	{
		m_SheetBtn.SetCheck (0);
			OnOK();	
	}
}

void CCreateBoxDialog::OnTsheet() 
{
	if (m_SheetBtn.GetCheck () == 0)
	{
		m_CutBtn.SetCheck (0);

		OnOK();	
	}
}

/* EOF: CreateBoxDialog.cpp */
void CCreateBoxDialog::OnOK() // old gedit
{
	
	UpdateData(TRUE);		//	old gedit
	if(m_ConvertToMetric)
	{
		dlgFieldsToTexels();
	}
	
	if (!m_customTemplate)	//	if the Custom Template button is NOT pushed
	{
		if (m_pDoc->m_pCurBrush)
		{
			pCube = m_pDoc->m_pCurBrush;
		}
	}	
	else if (m_customTemplate)	//	if the Custom Template button is pushed
	{
		
		m_pBoxTemplate->Solid		= m_Solid;
		m_pBoxTemplate->TCut		= m_TCut;
		m_pBoxTemplate->TSheet		= m_TSheet;
		m_pBoxTemplate->Thickness	= m_Thickness;
		m_pBoxTemplate->XSizeTop	= m_XSizeTop;
		m_pBoxTemplate->XSizeBot	= m_XSizeBot;
		m_pBoxTemplate->YSize		= m_YSize;
		m_pBoxTemplate->ZSizeTop	= m_ZSizeTop;
		m_pBoxTemplate->ZSizeBot	= m_ZSizeBot;
		
		pCube = ::BrushTemplate_CreateBox (m_pBoxTemplate);
		if (pCube != NULL)
			m_pDoc->CreateNewTemplateBrush ((Brush*)pCube);
			m_pDoc->m_pRecentCube = Brush_Clone(pCube);	//	post 0.55
			Brush_SetName(m_pDoc->m_pCurBrush, "Cube");	//	post 0.55
			m_pDoc->m_pBTemplate = Brush_Clone(m_pDoc->m_pCurBrush);
			Brush_SetName(m_pDoc->m_pBTemplate, "Cube");	// post 0.58
	}
}
	



void CCreateBoxDialog::OnKillfocusXsizetop() 
{
	float	lastValue = m_XSizeTop;

	if (GetDlgItemInt(IDC_XSIZETOP) == NULL)
	{
		this->SetDlgItemInt(IDC_XSIZETOP, lastValue);
		return;
	}

	UpdateData(TRUE);
	

	if ((m_XSizeTop >= BRUSH_MIN) && (m_XSizeTop <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_XSizeTop = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateBoxDialog::OnKillfocusXsizebot() 
{
	float	lastValue = m_XSizeBot;

		if (GetDlgItemInt(IDC_XSIZEBOT) == NULL)
	{
		this->SetDlgItemInt(IDC_XSIZEBOT, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_XSizeBot >= BRUSH_MIN) && (m_XSizeBot <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_XSizeBot = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}


void CCreateBoxDialog::OnKillfocusZsizetop() 
{
	float	lastValue = m_ZSizeTop;

		if (GetDlgItemInt(IDC_ZSIZETOP) == NULL)
	{
		this->SetDlgItemInt(IDC_ZSIZETOP, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_ZSizeTop >= BRUSH_MIN) && (m_ZSizeTop <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_ZSizeTop = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}


void CCreateBoxDialog::OnKillfocusZsizebot() 
{
	float	lastValue = m_ZSizeBot;

		if (GetDlgItemInt(IDC_ZSIZEBOT) == NULL)
	{
		this->SetDlgItemInt(IDC_ZSIZEBOT, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_ZSizeBot >= BRUSH_MIN) && (m_ZSizeBot <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_ZSizeBot = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateBoxDialog::OnKillfocusYsize() 
{
	float	lastValue = m_YSize;

		if (GetDlgItemInt(IDC_YSIZE) == NULL)
	{
		this->SetDlgItemInt(IDC_YSIZE, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_YSize >= BRUSH_MIN) && (m_YSize <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_YSize = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateBoxDialog::OnKillfocusThickness() 
{
	float	lastValue = m_Thickness;

		if (GetDlgItemInt(IDC_THICKNESS) == NULL)
	{
		this->SetDlgItemInt(IDC_THICKNESS, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Thickness >= WALL_THICK_MIN) && (m_Thickness <= WALL_THICK_MAX)) 
		OnOK();
	else
	{
		m_Thickness = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_wallThicknessErrorString);
	}
}

void CCreateBoxDialog::OnApplyBoxBtn() 
{
	OnOK();	
	m_pDoc->AddBrushToWorld();	
	m_pDoc->m_pMainFrame->UpdateMainControls();	
}



void CCreateBoxDialog::OnCustomTemplateBtn() 
{
	static bool toggle;
	toggle = !m_customTemplate;
	m_customTemplate = toggle;

	m_pDoc->m_pMainFrame->UpdateMainControls();
}


void CCreateBoxDialog::OnCancel()
{
	return;
}
