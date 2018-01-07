/****************************************************************************************/
/*  CreateArchDialog.cpp                                                                */
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
/*  Genesis3D Version 1.1 released November 15, 1999                                    */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved                            */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002                          */
/****************************************************************************************/
#include "stdafx.h"
#include "Globals.h"
#include "CreateArchDialog.h"
#include "units.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCreateArchDialog::CCreateArchDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateArchDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateArchDialog)
	m_NumSlits	=3;				// must agree with defaults in BrushTemplate.c
	m_Thickness	=150;
	m_Width		=100;
	m_Radius	=200;
	m_WallSize	=16;
	m_Style		=1;
	m_EndAngle	=180.0f;
	m_StartAngle=0.0f;
	m_TCut = FALSE;
// changed QD 11/03
	m_Sides		=3;
	m_CW		=0;
	m_Shape		=0;
	m_Radius2	=64;
	m_Height	=0;
	m_Massive = FALSE;
	m_Steps = FALSE;
// end change
	//}}AFX_DATA_INIT

	m_minMaxErrorString.Format("Value must be between %d and %d",
		BRUSH_MIN, BRUSH_MAX);
	m_angleErrorString.Format("Value must be between %d and %d", 
		ARCH_ANGLE_MIN, ARCH_ANGLE_MAX);
	m_archThicknessErrorString.Format("Value must be between %d and %d", 
		ARCH_THICK_MIN, ARCH_THICK_MAX);
	m_wallThicknessErrorString.Format("Value must be between %d and %d", 
		WALL_THICK_MIN, WALL_THICK_MAX);

	pArch = NULL;
//	m_recentArch = NULL;
}


void CCreateArchDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateArchDialog)
	DDX_Control(pDX, IDC_ADD_ARCH_BTN, m_addArchBTN);
	DDX_Control(pDX, IDC_CUSTOM_ARCH_BTN, m_customizeArchBTN);
	DDX_Text(pDX, IDC_NUMSLITS, m_NumSlits);
	DDX_Text(pDX, IDC_THICKNESS, m_Thickness);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	DDX_Text(pDX, IDC_RADIUS, m_Radius);
	DDX_Text(pDX, IDC_WALLSIZE, m_WallSize);
	DDX_Radio(pDX, IDC_SOLID, m_Style);
	DDX_Text(pDX, IDC_ENDANGLE, m_EndAngle);
	DDX_Text(pDX, IDC_STARTANGLE, m_StartAngle);
	DDX_Check(pDX, IDC_TCUT, m_TCut);
// changed QD 11/03
	DDX_Text(pDX, IDC_SIDES, m_Sides);
	DDX_Radio(pDX, IDC_CCW, m_CW);
	DDX_Radio(pDX, IDC_RECTANGULAR, m_Shape);
	DDX_Text(pDX, IDC_RADIUS2, m_Radius2);
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDX_Check(pDX, IDC_MASSIVE, m_Massive);
	DDX_Check(pDX, IDC_STEPS, m_Steps);
// end change
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateArchDialog, CDialog)
	//{{AFX_MSG_MAP(CCreateArchDialog)
	ON_BN_CLICKED(ID_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_SOLID, OnSolid)
	ON_BN_CLICKED(IDC_HOLLOW, OnHollow)
	ON_BN_CLICKED(IDC_RING, OnRing)
	ON_BN_CLICKED(IDC_ADD_ARCH_BTN, OnAddArchBtn)
	ON_EN_KILLFOCUS(IDC_STARTANGLE, OnKillfocusStartangle)
	ON_EN_KILLFOCUS(IDC_ENDANGLE, OnKillfocusEndangle)
	ON_EN_KILLFOCUS(IDC_THICKNESS, OnKillfocusThickness)
	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_RADIUS, OnKillfocusRadius)
	ON_EN_KILLFOCUS(IDC_WALLSIZE, OnKillfocusWallsize)
	ON_EN_KILLFOCUS(IDC_NUMSLITS, OnKillfocusNumslits)
	ON_BN_CLICKED(IDC_CUSTOM_ARCH_BTN, OnCustomArchBtn)
// changed QD 11/03
	ON_EN_KILLFOCUS(IDC_SIDES, OnKillfocusSides)
	ON_EN_KILLFOCUS(IDC_RADIUS2, OnKillfocusRadius2)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)
	ON_BN_CLICKED(IDC_CCW, OnCCW)
	ON_BN_CLICKED(IDC_CW, OnCW)
	ON_BN_CLICKED(IDC_RECTANGULAR, OnRectangular)
	ON_BN_CLICKED(IDC_ROUND, OnRound)
	ON_BN_CLICKED(IDC_MASSIVE, OnMassive)
	ON_BN_CLICKED(IDC_STEPS, OnSteps)
// end change
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateArchDialog message handlers


void	CCreateArchDialog::ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Arch *pArchTemplate, CGEditProDoc *pDoc)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pArchTemplate = pArchTemplate;
	m_pDoc = pDoc;

	m_customTemplate = false;

			//	toggle switch so we render our brush and NOT the Entitiy template box	
	pDoc->m_bTempEnt = FALSE; 

	pDoc->m_pMainFrame->HideAllPropDialogsBut(this);

	this->GetDlgItem(IDC_STARTANGLE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_ENDANGLE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_THICKNESS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_WIDTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_RADIUS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_WALLSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_NUMSLITS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SOLID)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HOLLOW)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_RING)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
// changed QD 11/03
	this->GetDlgItem(IDC_HEIGHT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CCW)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_CW)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_RECTANGULAR)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_ROUND)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_RADIUS2)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SIDES)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_STEPS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_MASSIVE)->EnableWindow(FALSE);
// end change
	this->GetDlgItem(ID_DEFAULTS)->EnableWindow(FALSE);

	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	if (!strcmp(pDoc->m_pCurBrush->Name,"Arch"))
	{

		if (pDoc->m_pCurBrush)
		{
			pArch = m_pDoc->m_pCurBrush;	
		}
	}

		//	if the current template name is NOT Arch, we probly have
		//	come here from another template type.
	else
		if (strcmp(pDoc->m_currentTemplateName, "Arch"))
	{

		//	check to see if there is a previous cube brush from the last visit
		if (pDoc->m_pRecentArch != NULL)	//	post 0.55
		{			//	reset name 'cause sometimes it gets changed
			Brush_SetName((Brush*)pDoc->m_pRecentArch, "Arch");	
			pDoc->m_pCurBrush = (Brush*)pDoc->m_pRecentArch;
//			pDoc->m_pBTemplate = pDoc->m_pCurBrush;
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);
			Brush_SetName(pDoc->m_pBTemplate, "Arch");	// post 0.58
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			pDoc->UpdateAllViews (UAV_ALL3DVIEWS, NULL);
		}
			//	if no modified Arch brush from the previous visit
		if (!pDoc->m_pRecentArch)
		{
			pArch = BrushTemplate_CreateArch(m_pArchTemplate);
			
			if (pArch != NULL)
			{
				pDoc->CreateNewTemplateBrush ((Brush*)pArch);
				pDoc->m_pRecentArch = Brush_Clone(pArch);
				Brush_SetName(pDoc->m_pCurBrush, "Arch");	
				pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);
				Brush_SetName(pDoc->m_pBTemplate, "Arch");	
			}	
		}	//post 0.55
	}		

	//	set this now so we can come back later
	pDoc->m_currentTemplateName = "Arch";

	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);

	UpdateData(TRUE);
}


bool CCreateArchDialog::UpdateCreateArchDlg(CGEditProDoc *pDoc)
{
	m_pDoc = pDoc;
	//	post 0.55
		//	if the current template name IS Arch...
	if (!strcmp(pDoc->m_currentTemplateName, "Arch"))
	{
		if ((pDoc->m_pCurBrush != NULL) && !strcmp(Brush_GetName(pDoc->m_pCurBrush), "Arch")
										&& pDoc->m_pCurBrush != pDoc->m_pRecentArch)
		{
				// update the template brush as a clone of pDoc->m_pCurBrush
			pDoc->m_pRecentArch = Brush_Clone(pDoc->m_pCurBrush);
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);
			Brush_SetName(pDoc->m_pBTemplate, "Arch");	
		}
	}
	//	post 0.55	
	
	
	BrushTemplate_Arch *pArchTemplate = pDoc->m_pLevelMgr->GetArchTemplate (pDoc->GetLevel());
	m_pArchTemplate = pArchTemplate;

	if (!m_customTemplate)
	{
		this->GetDlgItem(IDC_STARTANGLE)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_ENDANGLE)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_THICKNESS)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_WIDTH)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RADIUS)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_WALLSIZE)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_NUMSLITS)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_SOLID)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_HOLLOW)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RING)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
// changed QD 11/03
		this->GetDlgItem(IDC_HEIGHT)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_CCW)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_CW)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RECTANGULAR)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_ROUND)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_RADIUS2)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_SIDES)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_STEPS)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_MASSIVE)->EnableWindow(FALSE);
// end change
		this->GetDlgItem(ID_DEFAULTS)->EnableWindow(FALSE);

		m_addArchBTN.EnableWindow(TRUE);
		m_customizeArchBTN.SetCheck(0);
	}
	else if (m_customTemplate)
	{
		this->GetDlgItem(IDC_STARTANGLE)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_ENDANGLE)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_THICKNESS)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_WIDTH)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_RADIUS)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_WALLSIZE)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_NUMSLITS)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_SOLID)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_HOLLOW)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_RING)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_TCUT)->EnableWindow(TRUE);
// changed QD 11/03
		this->GetDlgItem(IDC_HEIGHT)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_CCW)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_CW)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_RECTANGULAR)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_ROUND)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_RADIUS2)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_SIDES)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_STEPS)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_MASSIVE)->EnableWindow(TRUE);
// end change
		this->GetDlgItem(ID_DEFAULTS)->EnableWindow(TRUE);

		m_addArchBTN.EnableWindow(FALSE);
		m_customizeArchBTN.SetCheck(1);
	}


	m_NumSlits	= pArchTemplate->NumSlits;
	m_Thickness	= pArchTemplate->Thickness;
	m_Width		= pArchTemplate->Width;
	m_Radius	= pArchTemplate->Radius;
	m_WallSize	= pArchTemplate->WallSize;
	m_Style		= pArchTemplate->Style;
	m_EndAngle	= pArchTemplate->EndAngle;
	m_StartAngle = pArchTemplate->StartAngle;
	m_TCut		= pArchTemplate->TCut;
// changed QD 11/03
	m_Sides		= pArchTemplate->Sides;
	m_CW		= pArchTemplate->CW;
	m_Shape		= pArchTemplate->Shape;
	m_Radius2	= pArchTemplate->Radius2;
	m_Height	= pArchTemplate->Height;
	m_Massive	= pArchTemplate->Massive;
	m_Steps		= pArchTemplate->Steps;
// end change

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



void	CCreateArchDialog::dlgFieldsToTexels(void)
{
	m_Thickness	=Units_CentimetersToEngine(m_Thickness);
	m_Width		=Units_CentimetersToEngine(m_Width);
	m_Radius	=Units_CentimetersToEngine(m_Radius);
	m_WallSize	=Units_CentimetersToEngine(m_WallSize);
// changed QD 11/03
	m_Radius2	=Units_CentimetersToEngine(m_Radius2);
	m_Height	=Units_CentimetersToEngine(m_Height);
}

void	CCreateArchDialog::dlgFieldsToCentimeters(void)
{
	m_Thickness	=Units_FRound(Units_EngineToCentimeters(m_Thickness));
	m_Width		=Units_FRound(Units_EngineToCentimeters(m_Width));
	m_Radius	=Units_FRound(Units_EngineToCentimeters(m_Radius));
	m_WallSize	=Units_FRound(Units_EngineToCentimeters(m_WallSize));
// changed QD 11/03
	m_Radius2	=Units_FRound(Units_EngineToCentimeters(m_Radius2));
	m_Height	=Units_FRound(Units_EngineToCentimeters(m_Height));
}


void CCreateArchDialog::OnDefaults() 
{
	m_EndAngle = 180;
	m_NumSlits = 3;
	m_StartAngle = 0;
	m_Thickness = 150;
	m_Width = 100;
	m_Radius = 200;
	m_WallSize = 16;
	m_Style = 1;
// changed QD 11/03
	m_Sides = 3;
	m_CW = 0;
	m_Shape = 0;
	m_Radius2 = 64;
	m_Height = 0;
	m_Massive = FALSE;
	m_Steps = FALSE;
// end change
	UpdateData( FALSE );
	OnOK();
}

void CCreateArchDialog::OnSolid() 
{
	OnOK();
}

void CCreateArchDialog::OnHollow() 
{
	OnOK();
}

void CCreateArchDialog::OnRing() 
{
	OnOK();
}

// changed QD 11/03
void CCreateArchDialog::OnCCW()
{
	OnOK();
}

void CCreateArchDialog::OnCW()
{
	OnOK();
}

void CCreateArchDialog::OnRectangular()
{
	OnOK();
}

void CCreateArchDialog::OnRound()
{
	OnOK();
}

void CCreateArchDialog::OnMassive()
{
	OnOK();
}

void CCreateArchDialog::OnSteps()
{
	OnOK();
}
// end change

/* EOF: CreateArchDialog */

void CCreateArchDialog::OnOK() 
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
			pArch = m_pDoc->m_pCurBrush;
		}
	}	

	else if (m_customTemplate)	//	if the Custom Template button is pushed
	{

	m_pArchTemplate->NumSlits	= m_NumSlits;
	m_pArchTemplate->Thickness	= m_Thickness;
	m_pArchTemplate->Width		= m_Width;
	m_pArchTemplate->Radius		= m_Radius;
	m_pArchTemplate->WallSize	= m_WallSize;
	m_pArchTemplate->Style		= m_Style;
	m_pArchTemplate->EndAngle	= m_EndAngle;
	m_pArchTemplate->StartAngle	= m_StartAngle;
	m_pArchTemplate->TCut		= m_TCut;
// changed QD 11/03
	m_pArchTemplate->Sides		= m_Sides;
	m_pArchTemplate->CW			= m_CW;
	m_pArchTemplate->Shape		= m_Shape;
	m_pArchTemplate->Radius2	= m_Radius2;
	m_pArchTemplate->Height		= m_Height;
	m_pArchTemplate->Massive	= m_Massive;
	m_pArchTemplate->Steps		= m_Steps;
// end change

	pArch = BrushTemplate_CreateArch (m_pArchTemplate);

	if (pArch != NULL)
		{
			m_pDoc->CreateNewTemplateBrush ((Brush*)pArch);
			m_pDoc->m_pRecentArch = Brush_Clone(pArch); // post 0.55
			Brush_SetName(m_pDoc->m_pCurBrush, "Arch");	// post 0.55
			m_pDoc->m_pBTemplate = Brush_Clone(pArch);
			Brush_SetName(m_pDoc->m_pBTemplate, "Arch");	
		}	
	}
}


void CCreateArchDialog::OnAddArchBtn() 
{
	OnOK();	
	m_pDoc->AddBrushToWorld();	
	m_pDoc->m_pMainFrame->UpdateMainControls();		
}

void CCreateArchDialog::OnKillfocusStartangle() 
{
	float	lastValue = m_StartAngle;

// changed QD 11/03
//	if (GetDlgItemInt(IDC_STARTANGLE) == NULL)
//	{
//		this->SetDlgItemInt(IDC_STARTANGLE, lastValue);
//		return;
//	}

	UpdateData(TRUE);
	
	if ((m_StartAngle >= ARCH_ANGLE_MIN) && (m_StartAngle <= ARCH_ANGLE_MAX)) 
		OnOK();
	else
	{
		m_StartAngle = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_angleErrorString);
	}
}

void CCreateArchDialog::OnKillfocusEndangle() 
{
	float	lastValue = m_EndAngle;

// changed QD 11/03
//	if (GetDlgItemInt(IDC_ENDANGLE) == NULL)
//	{
//		this->SetDlgItemInt(IDC_ENDANGLE, lastValue);
//		return;
//	}

	UpdateData(TRUE);

	if ((m_EndAngle >= ARCH_ANGLE_MIN) && (m_EndAngle <= ARCH_ANGLE_MAX)) 
		OnOK();
	else
	{
		m_EndAngle = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_angleErrorString);
	}
}

void CCreateArchDialog::OnKillfocusThickness() 
{
	float	lastValue = m_Thickness;

	if (GetDlgItemInt(IDC_THICKNESS) == NULL)
	{
		this->SetDlgItemInt(IDC_THICKNESS, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Thickness >= ARCH_THICK_MIN) && (m_Thickness <= ARCH_THICK_MAX)) 
		OnOK();
	else
	{	
		m_Thickness = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_archThicknessErrorString);
	}
}

void CCreateArchDialog::OnKillfocusWidth() 
{
	float	lastValue = m_Width;

	if (GetDlgItemInt(IDC_WIDTH) == NULL)
	{
		this->SetDlgItemInt(IDC_WIDTH, lastValue);
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

void CCreateArchDialog::OnKillfocusRadius() 
{
	float	lastValue = m_Radius;

//changed QD 11/03
//	if (GetDlgItemInt(IDC_RADIUS) == NULL)
//	{
//		this->SetDlgItemInt(IDC_RADIUS, lastValue);
//		return;
//	}

	UpdateData(TRUE);
// changed QD 11/03
	if ((m_Radius >= 0) && (m_Radius <= BRUSH_MAX))
		OnOK();
	else
	{
		m_Radius = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}

}

void CCreateArchDialog::OnKillfocusWallsize() 
{
	float	lastValue = m_WallSize;

		if (GetDlgItemInt(IDC_WALLSIZE) == NULL)
	{
		this->SetDlgItemInt(IDC_WALLSIZE, lastValue);
		return;
	}

	UpdateData(TRUE);

	//	need to figure out the min/max dimensions possible for hollow wall size

	if ((m_WallSize >= WALL_THICK_MIN) && (m_WallSize <= WALL_THICK_MAX)) 
		OnOK();
	else
	{
		m_WallSize = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_wallThicknessErrorString);
	}	
}

void CCreateArchDialog::OnKillfocusNumslits() 
{
	int	lastValue = m_NumSlits;

		if (GetDlgItemInt(IDC_NUMSLITS) == NULL)
	{
		this->SetDlgItemInt(IDC_NUMSLITS, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_NumSlits >= 3) && (m_NumSlits <= 64)) 
		OnOK();
	else
	{
		m_NumSlits = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= 3 and <= 64");
	}
}

// changed QD 11/03
void CCreateArchDialog::OnKillfocusSides()
{
	int	lastValue = m_Sides;

	if (GetDlgItemInt(IDC_SIDES) == NULL)
	{
		this->SetDlgItemInt(IDC_SIDES, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Sides >= 3) && (m_Sides <= 64))
		OnOK();
	else
	{
		m_Sides = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= 3 and <= 64");
	}
}

void CCreateArchDialog::OnKillfocusRadius2()
{
	int	lastValue = m_Radius2;

	if (GetDlgItemInt(IDC_RADIUS2) == NULL)
	{
		this->SetDlgItemInt(IDC_RADIUS2, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Radius2 >= BRUSH_MIN) && (m_Radius2 <= BRUSH_MAX))
		OnOK();
	else
	{
		m_Radius2 = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= 1 and <= 4000");
	}
}

void CCreateArchDialog::OnKillfocusHeight()
{
	int	lastValue = m_Height;

	UpdateData(TRUE);

	if ((m_Height >= 0) && (m_Height <= BRUSH_MAX))
		OnOK();
	else
	{
		m_Height = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= 0 and <= 4000");
	}
}
// end change

void CCreateArchDialog::OnCustomArchBtn() 
{
	static bool toggle;
	toggle = !m_customTemplate;
	m_customTemplate = toggle;

	m_pDoc->m_pMainFrame->UpdateMainControls();

}


void CCreateArchDialog::OnCancel()
{
	return;
}

BOOL CCreateArchDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->GetDlgItem(IDC_CUSTOM_ARCH_BTN)->SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
