/****************************************************************************************/
/*  CreateCylDialog.cpp                                                                 */
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
#include "CreateCylDialog.h"
#include <math.h>
#include "units.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCreateCylDialog::CCreateCylDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateCylDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateCylDialog)
	m_BotXOffset = 0.0;			//	must match defaults in BrushTemplate.c
	m_BotXSize = 128.0;
	m_BotZOffset = 0.0;
	m_BotZSize = 128.0;
	m_Solid = 1;				//	0=solid  1=hollow
	m_TopXOffset = 0.0;
	m_TopXSize = 128.0;
	m_TopZOffset = 0.0;
	m_TopZSize = 128.0;
	m_YSize = 512.0;
	m_RingLength = 3.0;
	m_TCut = FALSE;
	m_VerticalStripes = 8;
	m_Thickness = 16.0f;
	m_customizeCylBtn = FALSE;
	//}}AFX_DATA_INIT
	m_minMaxErrorString.Format("Value must be between %d and %d",
		BRUSH_MIN, BRUSH_MAX);
	m_wallThicknessErrorString.Format("Value must be between %d and %d", 
		WALL_THICK_MIN, WALL_THICK_MAX);

	pCyl = NULL;
	m_recentCyl = NULL;

}


void CCreateCylDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateCylDialog)
	DDX_Control(pDX, IDC_APPLY_CYL_BTN, m_applyCylBTN);
	DDX_Text(pDX, IDC_BOTXOFF, m_BotXOffset);
	DDX_Text(pDX, IDC_BOTXSIZE, m_BotXSize);
	DDX_Text(pDX, IDC_BOTZOFF, m_BotZOffset);
	DDX_Text(pDX, IDC_BOTZSIZE, m_BotZSize);
	DDX_Radio(pDX, IDC_SOLID, m_Solid);
	DDX_Text(pDX, IDC_TOPXOFF, m_TopXOffset);
	DDX_Text(pDX, IDC_TOPXSIZE, m_TopXSize);
	DDX_Text(pDX, IDC_TOPZOFF, m_TopZOffset);
	DDX_Text(pDX, IDC_TOPZSIZE, m_TopZSize);
	DDX_Text(pDX, IDC_YSIZE, m_YSize);
	DDX_Text(pDX, IDC_RINGLENGTH, m_RingLength);
	DDX_Check(pDX, IDC_TCUT, m_TCut);
	DDX_Text(pDX, IDC_VERTSTRIPES, m_VerticalStripes);
	DDX_Text(pDX, IDC_THICKNESS, m_Thickness);
	DDX_Check(pDX, IDC_CUSTOMIZE_CYL_BTN, m_customizeCylBtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateCylDialog, CDialog)
	//{{AFX_MSG_MAP(CCreateCylDialog)
	ON_BN_CLICKED(IDC_Defaults, OnDefaults)
	ON_BN_CLICKED(IDC_CUSTOMIZE_CYL_BTN, OnCustomizeCylBtn)
	ON_BN_CLICKED(IDC_APPLY_CYL_BTN, OnApplyCylBtn)
	ON_EN_KILLFOCUS(IDC_TOPXSIZE, OnKillfocusTopxsize)
	ON_EN_KILLFOCUS(IDC_TOPXOFF, OnKillfocusTopxoff)
	ON_EN_KILLFOCUS(IDC_TOPZSIZE, OnKillfocusTopzsize)
	ON_EN_KILLFOCUS(IDC_TOPZOFF, OnKillfocusTopzoff)
	ON_EN_KILLFOCUS(IDC_BOTXSIZE, OnKillfocusBotxsize)
	ON_EN_KILLFOCUS(IDC_BOTXOFF, OnKillfocusBotxoff)
	ON_EN_KILLFOCUS(IDC_BOTZSIZE, OnKillfocusBotzsize)
	ON_EN_KILLFOCUS(IDC_BOTZOFF, OnKillfocusBotzoff)
	ON_EN_KILLFOCUS(IDC_YSIZE, OnKillfocusYsize)
	ON_EN_KILLFOCUS(IDC_VERTSTRIPES, OnKillfocusVertstripes)
	ON_EN_KILLFOCUS(IDC_THICKNESS, OnKillfocusThickness)
	ON_EN_KILLFOCUS(IDC_RINGLENGTH, OnKillfocusRinglength)
	ON_BN_CLICKED(IDC_SOLID, OnSolid)
	ON_BN_CLICKED(IDC_HOLLOW, OnHollow)
	ON_BN_CLICKED(IDC_RING, OnRing)
	ON_BN_CLICKED(IDC_TCUT, OnTcut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*	// old gedit
int	CCreateCylDialog::DoModal(geBoolean ConvertToMetric, BrushTemplate_Cylinder *pCylTemplate)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pCylTemplate = pCylTemplate;

	m_BotXOffset	= m_pCylTemplate->BotXOffset;
	m_BotXSize		= m_pCylTemplate->BotXSize;
	m_BotZOffset	= m_pCylTemplate->BotZOffset;
	m_BotZSize		= m_pCylTemplate->BotZSize;
	m_Solid			= m_pCylTemplate->Solid;
	m_Thickness		= m_pCylTemplate->Thickness;
	m_TopXOffset	= m_pCylTemplate->TopXOffset;
	m_TopXSize		= m_pCylTemplate->TopXSize;
	m_TopZOffset	= m_pCylTemplate->TopZOffset;
	m_TopZSize		= m_pCylTemplate->TopZSize;
	m_VerticalStripes	= m_pCylTemplate->VerticalStripes;
	m_YSize			= m_pCylTemplate->YSize;
	m_RingLength	= m_pCylTemplate->RingLength;
	m_TCut			= m_pCylTemplate->TCut;

	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	return CDialog::DoModal();
}

*/

// new g3dc
void CCreateCylDialog::ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Cylinder *pCylTemplate, CGEditProDoc *pDoc)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pCylTemplate = pCylTemplate;
	m_pDoc = pDoc;

	m_customTemplate = false;

			//	toggle switch so we render our brush and NOT the Entitiy template box	
	pDoc->m_bTempEnt = FALSE; 

	this->GetDlgItem(IDC_TOPXSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TOPXOFF)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TOPZSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TOPZOFF)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_BOTXSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_BOTXOFF)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_BOTZSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_BOTZOFF)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_YSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_VERTSTRIPES)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_THICKNESS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_RINGLENGTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SOLID)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HOLLOW)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_RING)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_Defaults)->EnableWindow(FALSE);

	
	pDoc->m_pMainFrame->HideAllPropDialogsBut(this);

	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	if (!strcmp(pDoc->m_currentTemplateName, "Cylinder"))
	{
		if (pDoc->m_pCurBrush)
		{
			pCyl = m_pDoc->m_pCurBrush;	
		}
	}

		//	if the current template name is NOT Cylinder, we probly have
		//	come here from another template type.
	else if (strcmp(pDoc->m_currentTemplateName, "Cylinder"))
	{
		//	check to see if there is a previous cylinder brush from the last visit
		if (pDoc->m_pRecentCyl)	//	post 0.55
		{			//	reset name 'cause sometimes it gets changed
			Brush_SetName((Brush*)pDoc->m_pRecentCyl, "Cylinder");				
			pDoc->m_pCurBrush = (Brush*)pDoc->m_pRecentCyl;
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(pDoc->m_pBTemplate, "Cylinder");	// post 0.58
			//	Be very careful when speccing flags for UpdateAllViews()
				//	The wrong flags at the wrong time will totally screw things up
			pDoc->UpdateAllViews (UAV_ALL3DVIEWS, NULL);
		
		}
			//	if no modified Cyl brush from the previous visit
		if (!pDoc->m_pRecentCyl)
		{
			pCyl = BrushTemplate_CreateCylinder(m_pCylTemplate);
			
			if (pCyl != NULL)
			{
				pDoc->CreateNewTemplateBrush ((Brush*)pCyl);
				pDoc->m_pRecentCyl = Brush_Clone(pCyl);
				Brush_SetName(pDoc->m_pCurBrush, "Cylinder");	
				pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
				Brush_SetName(m_pDoc->m_pBTemplate, "Cylinder");	// post 0.58
				
			}	
		}	//post 0.55
	}		

	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);

	//	set this now so we can come back later
	pDoc->m_currentTemplateName = "Cylinder";


	UpdateData(TRUE);
}

bool	CCreateCylDialog::UpdateCreateCylDlg(CGEditProDoc *pDoc)
{
	m_pDoc = pDoc;
		//	post 0.55
		//	if the current template name IS Cylinder...
	if (!strcmp(pDoc->m_currentTemplateName, "Cylinder"))
	{
		if ((pDoc->m_pCurBrush != NULL) && !strcmp(Brush_GetName(pDoc->m_pCurBrush), "Cylinder")
									&& pDoc->m_pCurBrush != pDoc->m_pRecentCyl)
		{
				// update the template brush as a clone of pDoc->m_pCurBrush
			pDoc->m_pRecentCyl = Brush_Clone(pDoc->m_pCurBrush);
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(m_pDoc->m_pBTemplate, "Cylinder");	// post 0.58
		}
	}
	//	post 0.55	
		
	BrushTemplate_Cylinder *pCylTemplate = pDoc->m_pLevelMgr->GetCylinderTemplate (pDoc->GetLevel());
	m_pCylTemplate = pCylTemplate;

	if (!m_customTemplate)
	{
	this->GetDlgItem(IDC_TOPXSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TOPXOFF)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TOPZSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TOPZOFF)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_BOTXSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_BOTXOFF)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_BOTZSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_BOTZOFF)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_YSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_VERTSTRIPES)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_THICKNESS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_RINGLENGTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SOLID)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HOLLOW)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_RING)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_Defaults)->EnableWindow(FALSE);

	m_applyCylBTN.EnableWindow(TRUE);
	m_customizeCylBtn = FALSE;

	}

	else
	{
	this->GetDlgItem(IDC_TOPXSIZE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_TOPXOFF)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_TOPZSIZE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_TOPZOFF)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_BOTXSIZE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_BOTXOFF)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_BOTZSIZE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_BOTZOFF)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_YSIZE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_VERTSTRIPES)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_THICKNESS)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_RINGLENGTH)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_SOLID)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_HOLLOW)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_RING)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_Defaults)->EnableWindow(TRUE);

	m_applyCylBTN.EnableWindow(FALSE);
	m_customizeCylBtn = TRUE;

	}

	m_BotXOffset	= m_pCylTemplate->BotXOffset;
	m_BotXSize		= m_pCylTemplate->BotXSize;
	m_BotZOffset	= m_pCylTemplate->BotZOffset;
	m_BotZSize		= m_pCylTemplate->BotZSize;
	m_Solid			= m_pCylTemplate->Solid;
	m_Thickness		= m_pCylTemplate->Thickness;
	m_TopXOffset	= m_pCylTemplate->TopXOffset;
	m_TopXSize		= m_pCylTemplate->TopXSize;
	m_TopZOffset	= m_pCylTemplate->TopZOffset;
	m_TopZSize		= m_pCylTemplate->TopZSize;
	m_VerticalStripes	= m_pCylTemplate->VerticalStripes;
	m_YSize			= m_pCylTemplate->YSize;
	m_RingLength	= m_pCylTemplate->RingLength;
	m_TCut			= m_pCylTemplate->TCut;

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

void	CCreateCylDialog::dlgFieldsToTexels(void)
{
	m_BotXOffset	=Units_CentimetersToEngine(m_BotXOffset);
	m_BotXSize		=Units_CentimetersToEngine(m_BotXSize);
	m_BotZOffset	=Units_CentimetersToEngine(m_BotZOffset);
	m_BotZSize		=Units_CentimetersToEngine(m_BotZSize);
	m_Thickness		=Units_CentimetersToEngine(m_Thickness);
	m_TopXOffset	=Units_CentimetersToEngine(m_TopXOffset);
	m_TopXSize		=Units_CentimetersToEngine(m_TopXSize);
	m_TopZOffset	=Units_CentimetersToEngine(m_TopZOffset);
	m_TopZSize		=Units_CentimetersToEngine(m_TopZSize);
	m_YSize			=Units_CentimetersToEngine(m_YSize);
	m_RingLength	=Units_CentimetersToEngine(m_RingLength);
}

void	CCreateCylDialog::dlgFieldsToCentimeters(void)
{
	m_BotXOffset	=Units_FRound(Units_EngineToCentimeters(m_BotXOffset));
	m_BotXSize		=Units_FRound(Units_EngineToCentimeters(m_BotXSize));
	m_BotZOffset	=Units_FRound(Units_EngineToCentimeters(m_BotZOffset));
	m_BotZSize		=Units_FRound(Units_EngineToCentimeters(m_BotZSize));
	m_Thickness		=Units_FRound(Units_EngineToCentimeters(m_Thickness));
	m_TopXOffset	=Units_FRound(Units_EngineToCentimeters(m_TopXOffset));
	m_TopXSize		=Units_FRound(Units_EngineToCentimeters(m_TopXSize));
	m_TopZOffset	=Units_FRound(Units_EngineToCentimeters(m_TopZOffset));
	m_TopZSize		=Units_FRound(Units_EngineToCentimeters(m_TopZSize));
	m_YSize			=Units_FRound(Units_EngineToCentimeters(m_YSize));
	m_RingLength	=Units_FRound(Units_EngineToCentimeters(m_RingLength));
}


void CCreateCylDialog::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	
		ar << m_BotXOffset;
		ar << m_BotXSize;
		ar << m_BotZOffset;
		ar << m_BotZSize;
		ar << m_Solid;
		ar << m_Thickness;
		ar << m_TopXOffset;
		ar << m_TopXSize;
		ar << m_TopZOffset;
		ar << m_TopZSize;
		ar << m_VerticalStripes;
		ar << m_YSize;
	}
	else
	{	// loading code
		ar >> m_BotXOffset;
		ar >> m_BotXSize;
		ar >> m_BotZOffset;
		ar >> m_BotZSize;
		ar >> m_Solid;
		ar >> m_Thickness;
		ar >> m_TopXOffset;
		ar >> m_TopXSize;
		ar >> m_TopZOffset;
		ar >> m_TopZSize;
		ar >> m_VerticalStripes;
		ar >> m_YSize;
	}
}

void CCreateCylDialog::OnDefaults() 
{
	m_BotXOffset = 0.0;
	m_BotXSize = 128.0;
	m_BotZOffset = 0.0;
	m_BotZSize = 128.0;
	m_Solid = 1;
	m_Thickness = 16.0;
	m_TopXOffset = 0.0;
	m_TopXSize = 128.0;
	m_TopZOffset = 0.0;
	m_TopZSize = 128.0;
	m_VerticalStripes = 8;
	m_YSize = 512.0;
	m_RingLength = 3.0;
	m_TCut = FALSE;
	UpdateData(FALSE);
	OnOK();
}
/* EOF: CreateCylDialog.cpp */
void CCreateCylDialog::OnOK() 
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
			pCyl = m_pDoc->m_pCurBrush;
		}
	}

	else if (m_customTemplate)	//	if the Custom Template button is pushed
	{
	m_pCylTemplate->BotXOffset	= m_BotXOffset;
	m_pCylTemplate->BotXSize	= m_BotXSize;
	m_pCylTemplate->BotZOffset	= m_BotZOffset;
	m_pCylTemplate->BotZSize	= m_BotZSize;
	m_pCylTemplate->Solid		= m_Solid;
	m_pCylTemplate->Thickness	= m_Thickness;
	m_pCylTemplate->TopXOffset	= m_TopXOffset;
	m_pCylTemplate->TopXSize	= m_TopXSize;
	m_pCylTemplate->TopZOffset	= m_TopZOffset;
	m_pCylTemplate->TopZSize	= m_TopZSize;
	m_pCylTemplate->VerticalStripes	= m_VerticalStripes;
	m_pCylTemplate->YSize		= m_YSize;
	m_pCylTemplate->RingLength	= m_RingLength;
	m_pCylTemplate->TCut		= m_TCut;

		pCyl = BrushTemplate_CreateCylinder (m_pCylTemplate);
		if (pCyl != NULL)
		{
			m_pDoc->CreateNewTemplateBrush ((Brush*)pCyl);
			m_pDoc->m_pRecentCyl = Brush_Clone(pCyl);	//	post 0.55
			Brush_SetName(m_pDoc->m_pCurBrush, "Cylinder");	// post 0.55
			Brush_SetName(m_pDoc->m_pBTemplate, "Cylinder");	// post 0.58
		}
	}

}

void CCreateCylDialog::OnCustomizeCylBtn() 
{
	static bool toggle;
	toggle = !m_customTemplate;
	m_customTemplate = toggle;

	m_pDoc->m_pMainFrame->UpdateMainControls();
}

void CCreateCylDialog::OnApplyCylBtn() 
{
	OnOK();	
	m_pDoc->AddBrushToWorld();	
	m_pDoc->m_pMainFrame->UpdateMainControls();	
	
	
}

void CCreateCylDialog::OnKillfocusTopxsize() 
{
	float	lastValue = m_TopXSize;

	if (GetDlgItemInt(IDC_TOPXSIZE) == NULL)
	{
		this->SetDlgItemInt(IDC_TOPXSIZE, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_TopXSize >= BRUSH_MIN) && (m_TopXSize <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_TopXSize = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}	
}

void CCreateCylDialog::OnKillfocusTopxoff() 
{
	float	lastValue = m_TopXOffset;

	if (GetDlgItemInt(IDC_TOPXOFF) == NULL)
	{
		this->SetDlgItemInt(IDC_TOPXOFF, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_TopXOffset >= 0) && (m_TopXOffset <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_TopXOffset = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}	
}

void CCreateCylDialog::OnKillfocusTopzsize() 
{
	float	lastValue = m_TopZSize;

	if (GetDlgItemInt(IDC_TOPZSIZE) == NULL)
	{
		this->SetDlgItemInt(IDC_TOPZSIZE, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_TopZSize >= BRUSH_MIN) && (m_TopZSize <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_TopZSize = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateCylDialog::OnKillfocusTopzoff() 
{
	float	lastValue = m_TopZOffset;

	if (GetDlgItemInt(IDC_TOPZOFF) == NULL)
	{
		this->SetDlgItemInt(IDC_TOPZOFF, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_TopZOffset >= 0) && (m_TopZOffset <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_TopZOffset = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateCylDialog::OnKillfocusBotxsize() 
{
	float	lastValue = m_BotXSize;

	if (GetDlgItemInt(IDC_BOTXSIZE) == NULL)
	{
		this->SetDlgItemInt(IDC_BOTXSIZE, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_BotXSize >= BRUSH_MIN) && (m_BotXSize <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_BotXSize = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}	
}

void CCreateCylDialog::OnKillfocusBotxoff() 
{
	float	lastValue = m_BotXOffset;

	if (GetDlgItemInt(IDC_BOTXOFF) == NULL)
	{
		this->SetDlgItemInt(IDC_BOTXOFF, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_BotXOffset >= 0) && (m_BotXOffset <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_BotXOffset = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}	
}

void CCreateCylDialog::OnKillfocusBotzsize() 
{
	float	lastValue = m_BotZSize;

	if (GetDlgItemInt(IDC_BOTZSIZE) == NULL)
	{
		this->SetDlgItemInt(IDC_BOTZSIZE, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_BotZSize >= BRUSH_MIN) && (m_BotZSize <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_BotZSize = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateCylDialog::OnKillfocusBotzoff() 
{
	float	lastValue = m_BotZOffset;

	if (GetDlgItemInt(IDC_BOTZOFF) == NULL)
	{
		this->SetDlgItemInt(IDC_BOTZOFF, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_BotZOffset >= 0) && (m_BotZOffset <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_BotZOffset = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}		
}

void CCreateCylDialog::OnKillfocusYsize() 
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

void CCreateCylDialog::OnKillfocusVertstripes() 
{
	int	lastValue = m_VerticalStripes;

	if (GetDlgItemInt(IDC_VERTSTRIPES) == NULL)
	{
		this->SetDlgItemInt(IDC_VERTSTRIPES, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_VerticalStripes >= 5) && (m_VerticalStripes <= 64)) 
		OnOK();
	else
	{
		m_VerticalStripes = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= 5 and <= 64");
	}		
}

void CCreateCylDialog::OnKillfocusThickness() 
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

void CCreateCylDialog::OnKillfocusRinglength() 
{
	float	lastValue = m_RingLength;

	if (GetDlgItemInt(IDC_RINGLENGTH) == NULL)
	{
		this->SetDlgItemInt(IDC_RINGLENGTH, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_RingLength >= BRUSH_MIN) && (m_RingLength <= BRUSH_MAX)) 
		OnOK();
	else
	{
		m_RingLength = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}	
}

void CCreateCylDialog::OnSolid() 
{
	OnOK();		
}

void CCreateCylDialog::OnHollow() 
{
	OnOK();		
}

void CCreateCylDialog::OnRing() 
{
	OnOK();		
}

void CCreateCylDialog::OnTcut() 
{
	OnOK();		
}


void CCreateCylDialog::OnCancel()
{
	return;
}

BOOL CCreateCylDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->GetDlgItem(IDC_CUSTOMIZE_CYL_BTN)->SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
