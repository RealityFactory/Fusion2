/****************************************************************************************/
/*  CreateSpheroidDialog.cpp                                                            */
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
#include "CreateSpheroidDialog.h"
#include "units.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCreateSpheroidDialog::CCreateSpheroidDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateSpheroidDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateSpheroidDialog)
	m_HorizontalBands = 4;
	m_VerticalBands = 8;
	m_YSize = 256.0;
	m_Solid = 1;			//	0=solid  1=hollow
	m_Thickness = 16;
	m_TCut = FALSE;
	m_customizeSphere = FALSE;
	//}}AFX_DATA_INIT

	m_minMaxErrorString.Format("Value must be between %d and %d",
		BRUSH_MIN, BRUSH_MAX);
	m_wallThicknessErrorString.Format("Value must be between %d and %d", 
		WALL_THICK_MIN, WALL_THICK_MAX);

	mSolidSphere.LoadBitmap(IDB_SOLIDSPHERE);
	mHollowSphere.LoadBitmap(IDB_HOLLOWSPHERE);

	pSphere = NULL;
	m_recentSphere = NULL;
}

CCreateSpheroidDialog::~CCreateSpheroidDialog()
{
	mSolidSphere.DeleteObject();
	mHollowSphere.DeleteObject();
}

void CCreateSpheroidDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateSpheroidDialog)
	DDX_Control(pDX, IDC_ADD_SPHERE_BTN, m_addSphere);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Text(pDX, IDC_HORSTRIPES, m_HorizontalBands);
	DDX_Text(pDX, IDC_VERTSTRIPES, m_VerticalBands);
	DDX_Text(pDX, IDC_YSIZE, m_YSize);
	DDX_Radio(pDX, IDC_SOLID, m_Solid);
	DDX_Text(pDX, IDC_THICKNESS, m_Thickness);
	DDX_Check(pDX, IDC_TCUT, m_TCut);
	DDX_Check(pDX, IDC_CUSTOMIZE_SPHERE_BTN, m_customizeSphere);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateSpheroidDialog, CDialog)
	//{{AFX_MSG_MAP(CCreateSpheroidDialog)
	ON_BN_CLICKED(IDC_HOLLOW, OnHollow)
	ON_BN_CLICKED(IDC_SOLID, OnSolid)
	ON_BN_CLICKED(IDC_Defaults, OnDefaults)
	ON_BN_CLICKED(IDC_ADD_SPHERE_BTN, OnAddSphereBtn)
	ON_BN_CLICKED(IDC_CUSTOMIZE_SPHERE_BTN, OnCustomizeSphereBtn)
	ON_EN_KILLFOCUS(IDC_YSIZE, OnKillfocusYsize)
	ON_EN_KILLFOCUS(IDC_VERTSTRIPES, OnKillfocusVertstripes)
	ON_EN_KILLFOCUS(IDC_HORSTRIPES, OnKillfocusHorstripes)
	ON_EN_KILLFOCUS(IDC_THICKNESS, OnKillfocusThickness)
	ON_BN_CLICKED(IDC_TCUT, OnTcut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*
int	CCreateSpheroidDialog::DoModal(geBoolean ConvertToMetric, BrushTemplate_Spheroid *pTemplate)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pSphereTemplate = pTemplate;

	m_HorizontalBands	= m_pSphereTemplate->HorizontalBands;
	m_VerticalBands		= m_pSphereTemplate->VerticalBands;
	m_YSize				= m_pSphereTemplate->YSize;
	m_Solid				= m_pSphereTemplate->Solid;
	m_Thickness			= m_pSphereTemplate->Thickness;
	m_TCut				= m_pSphereTemplate->TCut;

	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	return CDialog::DoModal();
}

*/

// new g3dc
void	CCreateSpheroidDialog::ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Spheroid *pSphereTemplate, CGEditProDoc *pDoc)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pSphereTemplate = pSphereTemplate;
	m_pDoc	= pDoc;

	m_customTemplate = false;

			//	toggle switch so we render our brush and NOT the Entitiy template box	
	pDoc->m_bTempEnt = FALSE; 

	this->GetDlgItem(IDC_YSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_VERTSTRIPES)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HORSTRIPES)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_THICKNESS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SOLID)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HOLLOW)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_Defaults)->EnableWindow(FALSE);

					//	hide the entities and brush attributes dialogs
	pDoc->m_pMainFrame->HideAllPropDialogsBut(this);	

	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	if (!strcmp(pDoc->m_currentTemplateName, "Sphere"))
	{
		if (pDoc->m_pCurBrush)
		{
			pSphere = m_pDoc->m_pCurBrush;	
		}
	}

		//	if the current template name is NOT Sphere, we probly have
		//	come here from another template type.
	else if (strcmp(pDoc->m_currentTemplateName, "Sphere"))
	{
		//	check to see if there is a previous cube brush from the last visit
		if (pDoc->m_pRecentSphere)	//	post 0.55
		{			//	reset name 'cause sometimes it gets changed
			Brush_SetName((Brush*)pDoc->m_pRecentSphere, "Sphere");				
			pDoc->m_pCurBrush = (Brush*)pDoc->m_pRecentSphere;
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(pDoc->m_pBTemplate, "Sphere");	// post 0.58
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			pDoc->UpdateAllViews (UAV_ALL3DVIEWS, NULL);
		}
			//	if no modified Sphere brush from the previous visit
		if (!pDoc->m_pRecentSphere)
		{
			pSphere = BrushTemplate_CreateSpheroid(m_pSphereTemplate);
			
			if (pSphere != NULL)
			{
				pDoc->CreateNewTemplateBrush ((Brush*)pSphere);
				pDoc->m_pRecentSphere = Brush_Clone(pSphere);
				Brush_SetName(pDoc->m_pCurBrush, "Sphere");				
				pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
				Brush_SetName(pDoc->m_pBTemplate, "Sphere");	// post 0.58			
			}	
		}	//post 0.55
	}		

	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);

	//	set this now so we can come back later
	pDoc->m_currentTemplateName = "Sphere";


	UpdateData(TRUE);
}


bool CCreateSpheroidDialog::UpdateCreateSpheroidDlg(CGEditProDoc *pDoc)
{
	m_pDoc = pDoc;

	//	post 0.55
		//	if the current template name IS Sphere...
	if (!strcmp(pDoc->m_currentTemplateName, "Sphere"))
	{
		if ((pDoc->m_pCurBrush != NULL) && !strcmp(Brush_GetName(pDoc->m_pCurBrush), "Sphere")
									&& pDoc->m_pCurBrush != pDoc->m_pRecentSphere)
		{
				// update the template brush as a clone of pDoc->m_pCurBrush
			pDoc->m_pRecentSphere = Brush_Clone(pDoc->m_pCurBrush);
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(m_pDoc->m_pBTemplate, "Sphere");	// post 0.58
		}
	}
	//	post 0.55	

	BrushTemplate_Spheroid *pSphereTemplate = pDoc->m_pLevelMgr->GetSpheroidTemplate (pDoc->GetLevel());
	m_pSphereTemplate = pSphereTemplate;

	if (!m_customTemplate)
	{
	this->GetDlgItem(IDC_YSIZE)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_VERTSTRIPES)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HORSTRIPES)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_THICKNESS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_SOLID)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_HOLLOW)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_Defaults)->EnableWindow(FALSE);

	m_customizeSphere = FALSE;
	m_addSphere.EnableWindow(TRUE);
	}
	else
	{
	this->GetDlgItem(IDC_YSIZE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_VERTSTRIPES)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_HORSTRIPES)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_THICKNESS)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_SOLID)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_HOLLOW)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_Defaults)->EnableWindow(TRUE);

	m_customizeSphere = TRUE;
	m_addSphere.EnableWindow(FALSE);
	}
		
	m_HorizontalBands	= m_pSphereTemplate->HorizontalBands;
	m_VerticalBands		= m_pSphereTemplate->VerticalBands;
	m_YSize				= m_pSphereTemplate->YSize;
	m_Solid				= m_pSphereTemplate->Solid;
	m_Thickness			= m_pSphereTemplate->Thickness;
	m_TCut				= m_pSphereTemplate->TCut;

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


BOOL CCreateSpheroidDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_Solid==0)
		m_Picture.SetBitmap(mSolidSphere);
	else
		m_Picture.SetBitmap(mHollowSphere);

	this->GetDlgItem(IDC_CUSTOMIZE_SPHERE_BTN)->SetFocus();
	
	return	FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//#define PI2 (2.0 * M_PI)

void	CCreateSpheroidDialog::dlgFieldsToTexels(void)
{
	m_YSize			=Units_CentimetersToEngine(m_YSize);
}

void	CCreateSpheroidDialog::dlgFieldsToCentimeters(void)
{
	m_YSize			=Units_FRound(Units_EngineToCentimeters(m_YSize));
}

void CCreateSpheroidDialog::OnDefaults() 
{
	m_HorizontalBands	=4;
	m_VerticalBands		=8;
	m_YSize				=256.0;
	m_Solid				=1;
	m_Thickness			=16;
	UpdateData(FALSE);
	m_Picture.SetBitmap(mHollowSphere);
	OnOK();
}

void CCreateSpheroidDialog::OnOK() 
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
			pSphere = m_pDoc->m_pCurBrush;
		}
	}

	else if (m_customTemplate)	//	if the Custom Template button is pushed
	{
	m_pSphereTemplate->HorizontalBands	= m_HorizontalBands;
	m_pSphereTemplate->VerticalBands		= m_VerticalBands;
	m_pSphereTemplate->YSize				= m_YSize;
	m_pSphereTemplate->Solid				= m_Solid;
	m_pSphereTemplate->Thickness			= m_Thickness;
	m_pSphereTemplate->TCut				= m_TCut;
	

		pSphere = BrushTemplate_CreateSpheroid (m_pSphereTemplate);
		if (pSphere != NULL)
		{
			m_pDoc->CreateNewTemplateBrush ((Brush*)pSphere);
			m_pDoc->m_pRecentSphere = Brush_Clone(pSphere);	//	post 0.55
			Brush_SetName(m_pDoc->m_pCurBrush, "Sphere");		//	post 0.55
			Brush_SetName(m_pDoc->m_pBTemplate, "Sphere");	// post 0.58
		}
	}

}

void CCreateSpheroidDialog::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_HorizontalBands;
		ar << m_Solid;
		ar << m_Thickness;
		ar << m_VerticalBands;
		ar << m_YSize;
	}
	else
	{	// loading code
		ar >> m_HorizontalBands;
		ar >> m_Solid;
		ar >> m_Thickness;
		ar >> m_VerticalBands;
		ar >> m_YSize;
	}
}

void CCreateSpheroidDialog::OnAddSphereBtn() 
{
	OnOK();	
	m_pDoc->AddBrushToWorld();	
	m_pDoc->m_pMainFrame->UpdateMainControls();	
		
}

void CCreateSpheroidDialog::OnCustomizeSphereBtn() 
{
	static bool toggle;
	toggle = !m_customTemplate;
	m_customTemplate = toggle;

	m_pDoc->m_pMainFrame->UpdateMainControls();
}

void CCreateSpheroidDialog::OnKillfocusYsize() 
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

void CCreateSpheroidDialog::OnKillfocusVertstripes() 
{
	int	lastValue = m_VerticalBands;

	if (GetDlgItemInt(IDC_VERTSTRIPES) == NULL)
	{
		this->SetDlgItemInt(IDC_VERTSTRIPES, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_VerticalBands >= 3) && (m_VerticalBands <= 100)) 
		OnOK();
	else
	{
		m_VerticalBands = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= 3 and <= 100");
	}	
}

void CCreateSpheroidDialog::OnKillfocusHorstripes() 
{
	int	lastValue = m_HorizontalBands;

	if (GetDlgItemInt(IDC_HORSTRIPES) == NULL)
	{
		this->SetDlgItemInt(IDC_HORSTRIPES, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_HorizontalBands >= 3) && (m_HorizontalBands <= 100)) 
		OnOK();
	else
	{
		m_HorizontalBands = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= 3 and <= 100");
	}
}

void CCreateSpheroidDialog::OnKillfocusThickness() 
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

void CCreateSpheroidDialog::OnTcut() 
{
	OnOK();		
}

void CCreateSpheroidDialog::OnHollow() 
{
	m_Picture.SetBitmap(mHollowSphere);
	OnOK();	
}

void CCreateSpheroidDialog::OnSolid() 
{
	m_Picture.SetBitmap(mSolidSphere);
	OnOK();	
}


void CCreateSpheroidDialog::OnCancel()
{
	return;
}