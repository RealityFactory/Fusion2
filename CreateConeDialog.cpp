/****************************************************************************************/
/*  CreateConeDialog.cpp                                                                */
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
#include "CreateConeDialog.h"
#include "units.h"
#include "facelist.h"
#include <math.h>
#include "include/ram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCreateConeDialog::CCreateConeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateConeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateConeDialog)
	m_Width = 200.0f;		//	must match defaults in BrushTemplate.c
	m_Height = 300.0f;
	m_VerticalStrips = 8;
	m_Style = 1;			//	0=solid  1=hollow
	m_TCut = FALSE;
	m_Thickness = 16.0f;
	m_customizeCone_Button = FALSE;
	//}}AFX_DATA_INIT

	m_minMaxErrorString.Format("Value must be between %d and %d",
		BRUSH_MIN, BRUSH_MAX);
	m_wallThicknessErrorString.Format("Value must be between %d and %d", 
		WALL_THICK_MIN, WALL_THICK_MAX);

	pCone = NULL;
	m_recentCone = NULL;
}

void CCreateConeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateConeDialog)
	DDX_Control(pDX, IDC_ADD_CONE_BTN, m_AddConeButton);
	DDX_Control(pDX, ID_DEFAULT_CONE, m_defaultCone);
	DDX_Text(pDX, IDC_CONE_WIDTH, m_Width);
	DDX_Text(pDX, IDC_CONE_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_CONE_STRIPS, m_VerticalStrips);
	DDX_Radio(pDX, IDC_SOLID_CONE, m_Style);
	DDX_Check(pDX, IDC_TCUT_CONE, m_TCut);
	DDX_Text(pDX, IDC_CONE_WALL, m_Thickness);
	DDX_Check(pDX, IDC_CUSTOMIZE_CONE_BTN, m_customizeCone_Button);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateConeDialog, CDialog)
	//{{AFX_MSG_MAP(CCreateConeDialog)
	ON_BN_CLICKED(IDC_FUNNEL, OnFunnel)
	ON_BN_CLICKED(IDC_ADD_CONE_BTN, OnAddConeBtn)
	ON_BN_CLICKED(IDC_CUSTOMIZE_CONE_BTN, OnCustomizeConeBtn)
	ON_EN_KILLFOCUS(IDC_CONE_WIDTH, OnKillfocusConeWidth)
	ON_EN_KILLFOCUS(IDC_CONE_HEIGHT, OnKillfocusConeHeight)
	ON_EN_KILLFOCUS(IDC_CONE_STRIPS, OnKillfocusConeStrips)
	ON_EN_KILLFOCUS(IDC_CONE_WALL, OnKillfocusConeWall)
	ON_BN_CLICKED(IDC_SOLID_CONE, OnSolidCone)
	ON_BN_CLICKED(IDC_HOLLOW_CONE, OnHollowCone)
	ON_BN_CLICKED(IDC_TCUT_CONE, OnTcutCone)
	ON_BN_CLICKED(ID_DEFAULT_CONE, OnDefaultCone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*
int	CCreateConeDialog::DoModal(geBoolean ConvertToMetric, BrushTemplate_Cone *pConeTemplate)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pConeTemplate = pConeTemplate;

	m_Style		= pConeTemplate->Style;
	m_Width		= pConeTemplate->Width;
	m_Height		= pConeTemplate->Height;
	m_VerticalStrips = pConeTemplate->VerticalStrips;
	m_Thickness	= pConeTemplate->Thickness;
	m_TCut		= pConeTemplate->TCut;


	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	return CDialog::DoModal();
}
*/

void CCreateConeDialog::ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Cone *pConeTemplate, CGEditProDoc *pDoc)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pConeTemplate = pConeTemplate;
	m_pDoc = pDoc;

	m_customTemplate = false;

			//	toggle switch so we render our brush and NOT the Entitiy template box	
	pDoc->m_bTempEnt = FALSE; 

	this->GetDlgItem(IDC_CONE_WIDTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CONE_HEIGHT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CONE_STRIPS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CONE_WALL)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SOLID_CONE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HOLLOW_CONE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_FUNNEL)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT_CONE)->EnableWindow(FALSE);
	this->GetDlgItem(ID_DEFAULT_CONE)->EnableWindow(FALSE);

	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	
	if (!strcmp(pDoc->m_currentTemplateName, "Cone"))
	{
		if (pDoc->m_pCurBrush)
		{
			pCone = pDoc->m_pCurBrush;	
		}
	}

		//	if the current template name is NOT Cone, we probly have
		//	come here from another template type.
	else if (strcmp(pDoc->m_currentTemplateName, "Cone"))
	{
		//	check to see if there is a previous cube brush from the last visit
		if (pDoc->m_pRecentCone)	//	post 0.55
		{			//	reset name 'cause sometimes it gets changed
			Brush_SetName((Brush*)pDoc->m_pRecentCone, "Cone");	
			pDoc->m_pCurBrush = (Brush*)pDoc->m_pRecentCone;
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(pDoc->m_pBTemplate, "Cone");	// post 0.58
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			pDoc->UpdateAllViews (UAV_ALL3DVIEWS, NULL);
		}
			//	if no modified Cone brush from the previous visit
		if (!pDoc->m_pRecentCone)
		{
			pCone = BrushTemplate_CreateCone(m_pConeTemplate);
			
			if (pCone != NULL)
			{
				pDoc->CreateNewTemplateBrush ((Brush*)pCone);
				pDoc->m_pRecentCone = Brush_Clone(pCone);
				Brush_SetName(pDoc->m_pCurBrush, "Cone");	
				pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
				Brush_SetName(m_pDoc->m_pBTemplate, "Cone");	// post 0.55
			}	
		}	//post 0.55
	}		

	pDoc->m_pMainFrame->HideAllPropDialogsBut(this);

	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);

	//	set this now so we can come back later
	pDoc->m_currentTemplateName = "Cone";


	UpdateData(TRUE);
}


bool CCreateConeDialog::UpdateCreateConeDlg(CGEditProDoc *pDoc)
{
	m_pDoc = pDoc;
	//	post 0.55
		//	if the current template name IS Cone...
	if (!strcmp(pDoc->m_currentTemplateName, "Cone"))
	{
		if ((pDoc->m_pCurBrush != NULL) && !strcmp(Brush_GetName(pDoc->m_pCurBrush), "Cone")
						&& pDoc->m_pCurBrush != pDoc->m_pRecentCone)
		{
				// update the template brush as a clone of pDoc->m_pCurBrush
			pDoc->m_pRecentCone = Brush_Clone(pDoc->m_pCurBrush);
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(m_pDoc->m_pBTemplate, "Cone");	// post 0.58
		}
	}
	//	post 0.55	
	
	BrushTemplate_Cone *pConeTemplate = pDoc->m_pLevelMgr->GetConeTemplate (pDoc->GetLevel());
	m_pConeTemplate = pConeTemplate;

	if (!m_customTemplate)
	{
	this->GetDlgItem(IDC_CONE_WIDTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CONE_HEIGHT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CONE_STRIPS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CONE_WALL)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SOLID_CONE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HOLLOW_CONE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_FUNNEL)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT_CONE)->EnableWindow(FALSE);
	this->GetDlgItem(ID_DEFAULT_CONE)->EnableWindow(FALSE);
	
	m_AddConeButton.EnableWindow(TRUE);
	m_customizeCone_Button = FALSE;
	}

	else
	{
		this->GetDlgItem(IDC_CONE_WIDTH)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_CONE_HEIGHT)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_CONE_STRIPS)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_CONE_WALL)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_SOLID_CONE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_HOLLOW_CONE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_FUNNEL)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_TCUT_CONE)->EnableWindow(TRUE);
	this->GetDlgItem(ID_DEFAULT_CONE)->EnableWindow(TRUE);
	
	m_AddConeButton.EnableWindow(FALSE);
	m_customizeCone_Button = TRUE;
	}

	m_Style		= pConeTemplate->Style;
	m_Width		= pConeTemplate->Width;
	m_Height		= pConeTemplate->Height;
	m_VerticalStrips = pConeTemplate->VerticalStrips;
	m_Thickness	= pConeTemplate->Thickness;
	m_TCut		= pConeTemplate->TCut;

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


void	CCreateConeDialog::dlgFieldsToTexels(void)
{
	m_Width		=Units_CentimetersToEngine(m_Width);
	m_Height	=Units_CentimetersToEngine(m_Height);
	m_Thickness	=Units_CentimetersToEngine(m_Thickness);
}

void	CCreateConeDialog::dlgFieldsToCentimeters(void)
{
	m_Width		=Units_FRound(Units_EngineToCentimeters(m_Width));
	m_Height	=Units_FRound(Units_EngineToCentimeters(m_Height));
	m_Thickness	=Units_FRound(Units_EngineToCentimeters(m_Thickness));
}

/*
void CCreateConeDialog::OnDefault()
{
	m_Style = 0;
	m_Width = 200;
	m_Height = 300;
	m_VerticalStrips = 4;
	m_Thickness = 16;
	m_TCut = FALSE;
	UpdateData( FALSE ) ;
}/* CCreateConeDialog::OnDefault */


/* EOF: CreateConeDialog.cpp */
void CCreateConeDialog::OnOK() 
{
	UpdateData(TRUE);
	if(m_ConvertToMetric)
	{
		dlgFieldsToTexels();
	}
	
	
	if (!m_customTemplate)	//	if the Custom Template button is NOT pushed
	{
		if (m_pDoc->m_pCurBrush)
		{
			pCone = m_pDoc->m_pCurBrush;
		}
	}	
	else if (m_customTemplate)	//	if the Custom Template button is pushed
	{
		
		m_pConeTemplate->Style		= m_Style;
		m_pConeTemplate->Width		= m_Width;
		m_pConeTemplate->Height		= m_Height;
		m_pConeTemplate->VerticalStrips = m_VerticalStrips;
		m_pConeTemplate->Thickness	= m_Thickness;
		m_pConeTemplate->TCut		= m_TCut;
		
		
		pCone = BrushTemplate_CreateCone (m_pConeTemplate);
		if (pCone != NULL)
		{
			m_pDoc->CreateNewTemplateBrush ((Brush*)pCone);
			m_pDoc->m_pRecentCone = Brush_Clone(pCone);	//	post 0.55
			Brush_SetName(m_pDoc->m_pCurBrush, "Cone");	// post 0.55
			Brush_SetName(m_pDoc->m_pBTemplate, "Cone");	// post 0.58
		}
	}
}

void CCreateConeDialog::OnAddConeBtn() 
{
	OnOK();	
	m_pDoc->AddBrushToWorld();	
	m_pDoc->m_pMainFrame->UpdateMainControls();	
	
}

void CCreateConeDialog::OnCustomizeConeBtn() 
{
	static bool toggle;
	toggle = !m_customTemplate;
	m_customTemplate = toggle;

	m_pDoc->m_pMainFrame->UpdateMainControls();

}


void CCreateConeDialog::OnKillfocusConeWidth() 
{
	float	lastValue = m_Width;

	if (GetDlgItemInt(IDC_CONE_WIDTH) == NULL)
	{
		this->SetDlgItemInt(IDC_CONE_WIDTH, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Width >= BRUSH_MIN) && (m_Width <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_Width = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}	
}

void CCreateConeDialog::OnKillfocusConeHeight() 
{
	float	lastValue = m_Height;

	if (GetDlgItemInt(IDC_CONE_HEIGHT) == NULL)
	{
		this->SetDlgItemInt(IDC_CONE_HEIGHT, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Height >= BRUSH_MIN) && (m_Height <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_Height = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateConeDialog::OnKillfocusConeStrips() 
{
	int	lastValue = m_VerticalStrips;

	if (GetDlgItemInt(IDC_CONE_STRIPS) == NULL)
	{
		this->SetDlgItemInt(IDC_CONE_STRIPS, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_VerticalStrips >= 3) && (m_VerticalStrips <= 64)) 
		OnOK();
	else
	{
		m_VerticalStrips = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= 3 and <= 64");
	}	
}

void CCreateConeDialog::OnKillfocusConeWall() 
{
	float	lastValue = m_Thickness;

	if (GetDlgItemInt(IDC_CONE_WALL) == NULL)
	{
		this->SetDlgItemInt(IDC_CONE_WALL, lastValue);
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

void CCreateConeDialog::OnSolidCone() 
{
	OnOK();		
}

void CCreateConeDialog::OnHollowCone() 
{
	OnOK();		
}

void CCreateConeDialog::OnTcutCone() 
{
	OnOK();		
}


void CCreateConeDialog::OnFunnel() 
{
		OnOK();	
}


void CCreateConeDialog::OnDefaultCone() 
{
	m_Style = 1;
	m_Width = 200;
	m_Height = 300;
	m_VerticalStrips = 8;
	m_Thickness = 16;
	m_TCut = FALSE;
	UpdateData( FALSE ) ;
	OnOK();	
}


void CCreateConeDialog::OnCancel()
{
	return;
}

BOOL CCreateConeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->GetDlgItem(IDC_CUSTOMIZE_CONE_BTN)->SetFocus();	

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
