/****************************************************************************************/
/*  CreateStaircaseDialog.cpp                                                           */
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
#include "CreateStaircaseDialog.h"
#include "units.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCreateStaircaseDialog::CCreateStaircaseDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateStaircaseDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateStaircaseDialog)
	m_Height = 128.0;
	m_Length = 128.0;
	m_Width = 64.0;
	m_TCut = FALSE;
	m_NumberOfStairs = 8;
	m_MakeRamp = FALSE;
	m_customizeStairs = FALSE;
	//}}AFX_DATA_INIT

	m_minMaxErrorString.Format("Value must be between %d and %d",
		BRUSH_MIN, BRUSH_MAX);

	pStairs = NULL;
	m_recentStairs = NULL;
}


void CCreateStaircaseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateStaircaseDialog)
	DDX_Control(pDX, IDC_ADD_STAIRS, m_addStairs);
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_LENGTH, m_Length);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	DDX_Check(pDX, IDC_TCUT, m_TCut);
	DDX_Text(pDX, IDC_NUMBEROFSTAIRS, m_NumberOfStairs);
//	DDV_MinMaxInt(pDX, m_NumberOfStairs, 1, 64);
	DDX_Check(pDX, IDC_MAKE_RAMP, m_MakeRamp);
	DDX_Check(pDX, IDC_CUSTOMIZE_STAIRS, m_customizeStairs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateStaircaseDialog, CDialog)
	//{{AFX_MSG_MAP(CCreateStaircaseDialog)
	ON_BN_CLICKED(ID_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_ADD_STAIRS, OnAddStairs)
	ON_BN_CLICKED(IDC_CUSTOMIZE_STAIRS, OnCustomizeStairs)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)
	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_LENGTH, OnKillfocusLength)
	ON_EN_KILLFOCUS(IDC_NUMBEROFSTAIRS, OnKillfocusNumberofstairs)
	ON_BN_CLICKED(IDC_TCUT, OnTcut)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MAKE_RAMP, OnMakeRamp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* // old gedit
int	CCreateStaircaseDialog::DoModal(geBoolean ConvertToMetric, BrushTemplate_Staircase *pStairTemplate)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pStairTemplate = pStairTemplate;

	m_Height		= m_pStairTemplate->Height;
	m_Length		= m_pStairTemplate->Length;
	m_NumberOfStairs = m_pStairTemplate->NumberOfStairs;
	m_Width			= m_pStairTemplate->Width;
	m_MakeRamp		= m_pStairTemplate->MakeRamp;
	m_TCut			= m_pStairTemplate->TCut;

	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}

	return CDialog::DoModal();
}
*/

void CCreateStaircaseDialog::ShowDialog(geBoolean ConvertToMetric, BrushTemplate_Staircase *pStairTemplate, CGEditProDoc *pDoc)
{
	m_ConvertToMetric	=ConvertToMetric;
	m_pStairTemplate = pStairTemplate;
	m_pDoc = pDoc;

	m_customTemplate = false;

			//	toggle switch so we render our brush and NOT the Entitiy template box
	pDoc->m_bTempEnt = FALSE;

	this->GetDlgItem(IDC_HEIGHT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_WIDTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_LENGTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_NUMBEROFSTAIRS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_MAKE_RAMP)->EnableWindow(FALSE);
	this->GetDlgItem(ID_DEFAULTS)->EnableWindow(FALSE);

	pDoc->m_pMainFrame->HideAllPropDialogsBut(this);


	if(m_ConvertToMetric)
	{
		dlgFieldsToCentimeters();
	}


	if (!strcmp(pDoc->m_currentTemplateName, "Stairs"))
	{
		if (pDoc->m_pCurBrush)
		{
			pStairs = m_pDoc->m_pCurBrush;
		}
	}

		//	if the current template name is NOT Stairs, we probly have
		//	come here from another template type.
	else if (strcmp(pDoc->m_currentTemplateName, "Stairs"))
	{
		//	check to see if there is a previous stairs brush from the last visit
		if (pDoc->m_pRecentStairs)	//	post 0.55
		{			//	reset name 'cause sometimes it gets changed
			Brush_SetName((Brush*)pDoc->m_pRecentStairs, "Stairs");
			pDoc->m_pCurBrush = (Brush*)pDoc->m_pRecentStairs;
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(pDoc->m_pBTemplate, "Stairs");	// post 0.58
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			pDoc->UpdateAllViews (UAV_ALL3DVIEWS, NULL);
		}
			//	if no modified Stairs brush from the previous visit
		if (!pDoc->m_pRecentStairs)
		{
			pStairs = BrushTemplate_CreateStaircase(m_pStairTemplate);

			if (pStairs != NULL)
			{
				pDoc->CreateNewTemplateBrush ((Brush*)pStairs);
				pDoc->m_pRecentStairs = Brush_Clone(pStairs);
				Brush_SetName(pDoc->m_pCurBrush, "Stairs");
				pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
				Brush_SetName(pDoc->m_pBTemplate, "Stairs");	// post 0.58
			}
		}	//post 0.55
	}

	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);

	//	set this now so we can come back later
	pDoc->m_currentTemplateName = "Stairs";

	UpdateData(TRUE);

}



bool	CCreateStaircaseDialog::UpdateCreateStaircaseDlg(CGEditProDoc *pDoc)
{
	m_pDoc = pDoc;

	//	post 0.55
		//	if the current template name IS Stairs...
	if (!strcmp(pDoc->m_currentTemplateName, "Stairs"))
	{
		if ((pDoc->m_pCurBrush != NULL) && !strcmp(Brush_GetName(pDoc->m_pCurBrush), "Stairs")
									&& pDoc->m_pCurBrush != pDoc->m_pRecentStairs)
		{
				// update the template brush as a clone of pDoc->m_pCurBrush
			pDoc->m_pRecentStairs = Brush_Clone(pDoc->m_pCurBrush);
			pDoc->m_pBTemplate = Brush_Clone(pDoc->m_pCurBrush);	//	post 0.58
			Brush_SetName(m_pDoc->m_pBTemplate, "Stairs");	// post 0.58
		}
	}
	//	post 0.55

	BrushTemplate_Staircase *pStairTemplate = pDoc->m_pLevelMgr->GetStaircaseTemplate (pDoc->GetLevel());
	m_pStairTemplate = pStairTemplate;

	if (!m_customTemplate)
	{
	this->GetDlgItem(IDC_HEIGHT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_WIDTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_LENGTH)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_NUMBEROFSTAIRS)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_MAKE_RAMP)->EnableWindow(FALSE);
	this->GetDlgItem(ID_DEFAULTS)->EnableWindow(FALSE);


	m_customizeStairs = FALSE;
	m_addStairs.EnableWindow(TRUE);
	}

	else
	{
	this->GetDlgItem(IDC_HEIGHT)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_WIDTH)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_LENGTH)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_NUMBEROFSTAIRS)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_TCUT)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_MAKE_RAMP)->EnableWindow(TRUE);
	this->GetDlgItem(ID_DEFAULTS)->EnableWindow(TRUE);

	m_customizeStairs = TRUE;
	m_addStairs.EnableWindow(FALSE);
	}

	m_Height		= m_pStairTemplate->Height;
	m_Length		= m_pStairTemplate->Length;
	m_NumberOfStairs = m_pStairTemplate->NumberOfStairs;
	m_Width			= m_pStairTemplate->Width;
	m_MakeRamp		= m_pStairTemplate->MakeRamp;
	m_TCut			= m_pStairTemplate->TCut;

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


void	CCreateStaircaseDialog::dlgFieldsToTexels(void)
{
	m_Height	=Units_CentimetersToEngine(m_Height);
	m_Width		=Units_CentimetersToEngine(m_Width);
	m_Length	=Units_CentimetersToEngine(m_Length);
}

void	CCreateStaircaseDialog::dlgFieldsToCentimeters(void)
{
	m_Height	=Units_FRound(Units_EngineToCentimeters(m_Height));
	m_Width		=Units_FRound(Units_EngineToCentimeters(m_Width));
	m_Length	=Units_FRound(Units_EngineToCentimeters(m_Length));
}


void CCreateStaircaseDialog::OnDefaults()
{
	m_Height = 128.0;
	m_Length = 128.0;
	m_NumberOfStairs = 8;
	m_Width = 64.0;
	m_MakeRamp = FALSE;
	m_TCut = FALSE;
	UpdateData( FALSE );
	OnOK();
}/* CCreateStaircaseDialog::OnDefaults */

/* EOF: CreateStaircaseDialog */

void CCreateStaircaseDialog::OnOK()
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
			pStairs = m_pDoc->m_pCurBrush;
		}
	}

	else if (m_customTemplate)	//	if the Custom Template button is pushed
	{
	m_pStairTemplate->Height		= m_Height;
	m_pStairTemplate->Length		= m_Length;
	m_pStairTemplate->NumberOfStairs = m_NumberOfStairs;
	m_pStairTemplate->Width			= m_Width;
	m_pStairTemplate->MakeRamp		= m_MakeRamp;
	m_pStairTemplate->TCut			= m_TCut;

								// actually creates the brush geometry
		pStairs = BrushTemplate_CreateStaircase (m_pStairTemplate);

		if (pStairs != NULL)
		{						// adds the new brush to the main list
			m_pDoc->CreateNewTemplateBrush ((Brush*)pStairs);
			m_pDoc->m_pRecentStairs = Brush_Clone(pStairs);	//	post 0.55
			Brush_SetName(m_pDoc->m_pCurBrush, "Stairs");	//	post 0.55
			Brush_SetName(m_pDoc->m_pBTemplate, "Stairs");	// post 0.58
		}
	}
}

void CCreateStaircaseDialog::OnAddStairs()
{
	OnOK();
	m_pDoc->AddBrushToWorld();
	m_pDoc->m_pMainFrame->UpdateMainControls();
}

void CCreateStaircaseDialog::OnCustomizeStairs()
{
	static bool toggle;
	toggle = !m_customTemplate;
	m_customTemplate = toggle;

	m_pDoc->m_pMainFrame->UpdateMainControls();
}

void CCreateStaircaseDialog::OnKillfocusHeight()
{
	float	lastValue = m_Height;

	if (GetDlgItemInt(IDC_HEIGHT) == NULL)
	{
		this->SetDlgItemInt(IDC_HEIGHT, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Height >= BRUSH_MIN) && (m_Height <= BRUSH_MAX))
	{
		OnOK();
	}
	else
	{
		m_Height = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateStaircaseDialog::OnKillfocusWidth()
{
	float	lastValue = m_Width;

	if (GetDlgItemInt(IDC_WIDTH) == NULL)
	{
		this->SetDlgItemInt(IDC_WIDTH, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Width >= BRUSH_MIN) && (m_Width <= BRUSH_MAX))
	{
		OnOK();
	}
	else
	{
		m_Width = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateStaircaseDialog::OnKillfocusLength()
{
	float	lastValue = m_Length;

	if (GetDlgItemInt(IDC_LENGTH) == NULL)
	{
		this->SetDlgItemInt(IDC_LENGTH, lastValue);
		return;
	}

	UpdateData(TRUE);

	if ((m_Length >= BRUSH_MIN) && (m_Length <= BRUSH_MAX))
	{
		OnOK();
	}
	else
	{
		m_Length = lastValue;
		UpdateData(FALSE);
		AfxMessageBox(m_minMaxErrorString);
	}
}

void CCreateStaircaseDialog::OnKillfocusNumberofstairs()
{
	int	lastValue = m_NumberOfStairs;

	if (GetDlgItemInt(IDC_NUMBEROFSTAIRS) == NULL)
	{
		this->SetDlgItemInt(IDC_NUMBEROFSTAIRS, lastValue);
		return;
	}

	UpdateData(TRUE);

	if (m_NumberOfStairs >= 2 && m_NumberOfStairs <= 64)
	{
		OnOK();
	}
	else
	{
		m_NumberOfStairs = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be between 2 and 64");
	}
}

void CCreateStaircaseDialog::OnTcut()
{
	OnOK();
}

void CCreateStaircaseDialog::OnMakeRamp()
{
	OnOK();
}


void CCreateStaircaseDialog::OnDestroy()
{

	CDialog::OnDestroy();

}


void CCreateStaircaseDialog::OnCancel()
{
	return;
}

BOOL CCreateStaircaseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->GetDlgItem(IDC_CUSTOMIZE_STAIRS)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
