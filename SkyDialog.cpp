/****************************************************************************************/
/*  SkyDialog.cpp                                                                       */
/*                                                                                      */
/*  Author:       Jim Mischel                                                           */
/*  Description:  Sky settings                                                          */
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
#include "GEditPro.h"
#include "SkyDialog.h"
#include "wadfile.h"
//#include "GEditProDoc.h"
#include "include/ram.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkyDialog dialog

CSkyDialog::CSkyDialog(CWnd *pParent /*=NULL*/)
	: CDialog(CSkyDialog::IDD, (CWnd*)pParent)
{
	//{{AFX_DATA_INIT(CSkyDialog)
	m_RotationAxis = -1;
	m_RotationSpeed = 0.0f;
	m_TextureScale = 1.0f;
	//}}AFX_DATA_INIT
	m_ptEditDoc = NULL;
//	m_pParentCtrl = pParent;

//	CDialog::Create (IDD, (CWnd*)pParent);
}

CSkyDialog::~CSkyDialog ()
{
}

void CSkyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkyDialog)
	DDX_Control(pDX, IDC_NULLCOMBO, m_NullCombo);
	DDX_Control(pDX, IDC_CLEAR_ALL_BTN, m_clearAllBtn);
	DDX_Control(pDX, IDC_ALL_SAMEBTN, m_allsameBtn);
	DDX_Control(pDX, IDC_CBALLSAME, m_SkyAlltheSameCombo);
	DDX_Control(pDX, IDC_EDITSPEED, m_EditSpeed);
	DDX_Control(pDX, IDC_SKYTOP, m_SkyTop);
	DDX_Control(pDX, IDC_SKYRIGHT, m_SkyRight);
	DDX_Control(pDX, IDC_SKYLEFT, m_SkyLeft);
	DDX_Control(pDX, IDC_SKYFRONT, m_SkyFront);
	DDX_Control(pDX, IDC_SKYBOTTOM, m_SkyBottom);
	DDX_Control(pDX, IDC_SKYBACK, m_SkyBack);
	DDX_Control(pDX, IDC_CBSKYTOP, m_SkyTopCombo);
	DDX_Control(pDX, IDC_CBSKYRIGHT, m_SkyRightCombo);
	DDX_Control(pDX, IDC_CBSKYLEFT, m_SkyLeftCombo);
	DDX_Control(pDX, IDC_CBSKYFRONT, m_SkyFrontCombo);
	DDX_Control(pDX, IDC_CBSKYBOTTOM, m_SkyBottomCombo);
	DDX_Control(pDX, IDC_CBSKYBACK, m_SkyBackCombo);
	DDX_Radio(pDX, IDC_AXISX, m_RotationAxis);
	DDX_Text(pDX, IDC_EDITSPEED, m_RotationSpeed);
	DDV_MinMaxFloat(pDX, m_RotationSpeed, 0.f, 3600.f);
	DDX_Text(pDX, IDC_EDITSCALE, m_TextureScale);
	DDV_MinMaxFloat(pDX, m_TextureScale, 0.f, 255.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSkyDialog, CDialog)
	//{{AFX_MSG_MAP(CSkyDialog)
	ON_BN_CLICKED(IDC_SKYBACK, OnSkyback)
	ON_BN_CLICKED(IDC_SKYBOTTOM, OnSkybottom)
	ON_BN_CLICKED(IDC_SKYFRONT, OnSkyfront)
	ON_BN_CLICKED(IDC_SKYLEFT, OnSkyleft)
	ON_BN_CLICKED(IDC_SKYRIGHT, OnSkyright)
	ON_BN_CLICKED(IDC_SKYTOP, OnSkytop)
	ON_CBN_SELCHANGE(IDC_CBSKYLEFT, OnSelchangeCbskyleft)
	ON_CBN_SELCHANGE(IDC_CBSKYRIGHT, OnSelchangeCbskyright)
	ON_CBN_SELCHANGE(IDC_CBSKYTOP, OnSelchangeCbskytop)
	ON_CBN_SELCHANGE(IDC_CBSKYBOTTOM, OnSelchangeCbskybottom)
	ON_CBN_SELCHANGE(IDC_CBSKYFRONT, OnSelchangeCbskyfront)
	ON_CBN_SELCHANGE(IDC_CBSKYBACK, OnSelchangeCbskyback)
	ON_EN_KILLFOCUS(IDC_EDITSPEED, OnKillfocusEditspeed)
	ON_BN_CLICKED(IDC_AXISX, OnAxisButton)
	ON_EN_KILLFOCUS(IDC_EDITSCALE, OnKillfocusEditscale)
	ON_CBN_SELCHANGE(IDC_CBALLSAME, OnSelchangeCBallsame)
	ON_BN_CLICKED(IDC_CLEAR_ALL_BTN, OnClearAllBtn)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_AXISY, OnAxisButton)
	ON_BN_CLICKED(IDC_AXISZ, OnAxisButton)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkyDialog message handlers

BOOL CSkyDialog::OnInitDialog() 
{
//	RECT rect;
//	RECT rect2;
	m_ptEditDoc = CGlobals::GetActiveDocument();

	CDialog::OnInitDialog();

/*	m_pParentCtrl->GetClientRect (&rect);
	m_pParentCtrl->GetItemRect( 0, &rect2 );
	rect.top = rect2.bottom + FTC_BORDER_SIZE_TOP;
	rect.left = rect.left + FTC_BORDER_SIZE_LEFT;
	rect.right = rect.right - FTC_BORDER_SIZE_RIGHT ;
	rect.bottom = rect.bottom - FTC_BORDER_SIZE_BOTTOM ;

	SetWindowPos( NULL, rect.left,
			rect.top, rect.right, rect.bottom, SWP_NOZORDER );
*/
	Update (m_ptEditDoc);	

	m_NullCombo.SetCurSel(-1);	//	a dummy combo
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	======================================================================================
void CSkyDialog::UpdateSkyFaceInfo (CButton &FaceButton, CComboBox &FaceCombo, int FaceIndex)
{
	if (m_hWnd)
	{
		CWadFile *pWad = NULL;

		FaceCombo.ResetContent ();
		pWad = m_ptEditDoc->m_pLevelMgr->GetWadFile (m_ptEditDoc->GetLevel());
		if (pWad)
		{
			for (int index = 0; index < pWad->mBitmapCount; ++index)
			{
				CString Name = pWad->mBitmaps[index].Name;
				FaceCombo.AddString (Name);
			}
			SetSkyFaceUI (FaceButton, FaceCombo, FaceIndex);
		}
	}
}

void CSkyDialog::Update (CGEditProDoc *pDoc)
{
	if (m_hWnd)
	{
		m_ptEditDoc = pDoc;

		geVec3d Axis;
		//	SkyFaceTexture *SkyInfo = 
		pDoc->m_pLevelMgr->GetSkyInfo (m_ptEditDoc->GetLevel(), &Axis, &m_RotationSpeed, &m_TextureScale);

		if (Axis.X != 0.0f) 
		{
			m_RotationAxis = 0;
			CButton *xaxis = (CButton*)GetDlgItem(IDC_AXISX);
			xaxis->SetCheck(1);

		}
		else if (Axis.Y != 0.0f) 
		{
			m_RotationAxis = 1;
			CButton *yaxis = (CButton*)GetDlgItem(IDC_AXISY);
			yaxis->SetCheck(1);
		}
		else
		{
			m_RotationAxis = 2;
			CButton *zaxis = (CButton*)GetDlgItem(IDC_AXISZ);
			zaxis->SetCheck(1);
		}

		UpdateSkyFaceInfo (m_SkyLeft, m_SkyLeftCombo, SkyFace_Left);
		UpdateSkyFaceInfo (m_SkyRight, m_SkyRightCombo, SkyFace_Right);
		UpdateSkyFaceInfo (m_SkyTop, m_SkyTopCombo, SkyFace_Top);
		UpdateSkyFaceInfo (m_SkyBottom, m_SkyBottomCombo, SkyFace_Bottom);
		UpdateSkyFaceInfo (m_SkyFront, m_SkyFrontCombo, SkyFace_Front);
		UpdateSkyFaceInfo (m_SkyBack, m_SkyBackCombo, SkyFace_Back);
		//	NOTE: G3DC added SkyFace_Null item to SkyFace enum type in Level.h
		UpdateSkyFaceInfo (m_allsameBtn, m_SkyAlltheSameCombo, SkyFace_Null);
		UpdateSkyFaceInfo (m_allsameBtn, m_NullCombo, SkyFace_Null);

		m_SkyAlltheSameCombo.EnableWindow(TRUE);

		if (m_SkyLeftCombo.GetCurSel() != -1)
		{
			m_SkyLeftCombo.EnableWindow();
			m_SkyLeft.SetCheck(1);
		}

		if (m_SkyRightCombo.GetCurSel() != -1)
		{
			m_SkyRightCombo.EnableWindow();
			m_SkyRight.SetCheck(1);
		}

		if (m_SkyTopCombo.GetCurSel() != -1)
		{
			m_SkyTopCombo.EnableWindow();
			m_SkyTop.SetCheck(1);
		}

		if (m_SkyBottomCombo.GetCurSel() != -1)
		{
			m_SkyBottomCombo.EnableWindow();
			m_SkyBottom.SetCheck(1);
		}

		if (m_SkyFrontCombo.GetCurSel() != -1)
		{
			m_SkyFrontCombo.EnableWindow();
			m_SkyFront.SetCheck(1);
		}

		if (m_SkyBackCombo.GetCurSel() != -1)
		{
			m_SkyBackCombo.EnableWindow();
			m_SkyBack.SetCheck(1);
		}
		//	Update the control windows based on these updated variables
		UpdateData(FALSE);	
	}
}



int CSkyDialog::FaceNameInCombo (char const *TextureName, CComboBox &FaceCombo)
{
	if (m_hWnd)
	{
		CString tempTextureName = TextureName;	//	post 0.57
		if (tempTextureName == _T(""))			//	post 0.57	for more reliable checking
		{
			return CB_ERR;	
		}
		return FaceCombo.FindStringExact (0, TextureName);
	}
}

void CSkyDialog::SetSkyFaceUI (CButton &FaceButton, CComboBox &FaceCombo, int FaceIndex)
{
	if (m_hWnd)
	{
		if (m_ptEditDoc != NULL)
		{
			int Index;
			SkyFaceTexture *SkyFaces = NULL;
			geVec3d Axis;
			geFloat Speed = 0.0f, Scale = 0.0f;

			SkyFaces = m_ptEditDoc->m_pLevelMgr->GetSkyInfo (m_ptEditDoc->GetLevel(), &Axis, &Speed, &Scale);
			if (SkyFaces)
			{
				if (SkyFaces->TextureName && SkyFaces->Apply)
				{
					// If the texture name is in the combo, then select that item.
					// Otherwise select -1
					Index = FaceNameInCombo (SkyFaces[FaceIndex].TextureName, FaceCombo);
					FaceCombo.SetCurSel (Index);
					FaceButton.SetCheck ((SkyFaces[FaceIndex].Apply) ? 1 : 0);
					FaceCombo.EnableWindow (SkyFaces[FaceIndex].Apply);
				}	//	if (SkyFaces->TextureName && SkyFaces->Apply)...
			}	//	if (SkyFaces...
		}
	}	//	if (m_hWnd)...
}

void CSkyDialog::UpdateSkyFaceUI (CButton &FaceButton, CComboBox &FaceCombo, int FaceIndex)
{
	if (m_hWnd)
	{
		BOOL Checked;
		Checked = (FaceButton.GetCheck() == 1);
		FaceCombo.EnableWindow (Checked);
		if (m_ptEditDoc != NULL)
		{
			SkyFaceTexture *SkyFaces = NULL;
			geVec3d Axis;
			geFloat Speed = 0.0f, Scale = 0.0f;

			SkyFaces = m_ptEditDoc->m_pLevelMgr->GetSkyInfo (m_ptEditDoc->GetLevel(), &Axis, &Speed, &Scale);
			if (SkyFaces)
			{
				SkyFaces[FaceIndex].Apply = Checked;
			}	//	if (SkyFaces...
		}
	}
}


void CSkyDialog::ResetSkySurface(CComboBox &FaceCombo, int FaceIndex)
{
	if (m_hWnd)
	{
	FaceCombo.SetCurSel(-1);
	UpdateFaceTextureName (m_NullCombo, FaceIndex, false);
	FaceCombo.SetCurSel(-1);
	m_SkyAlltheSameCombo.SetCurSel(-1);
	}
}


void CSkyDialog::OnSkyback() 
{
	if (m_hWnd)
	{
	if (m_SkyBack.GetCheck() == 0)
	{
		ResetSkySurface(m_SkyBackCombo, SkyFace_Back);
	}

	UpdateSkyFaceUI (m_SkyBack, m_SkyBackCombo, SkyFace_Back);
	}
}

void CSkyDialog::OnSkybottom() 
{
	if (m_hWnd)
	{
	if (m_SkyBottom.GetCheck() == 0)
	{
		ResetSkySurface(m_SkyBottomCombo, SkyFace_Bottom);
	}

	UpdateSkyFaceUI (m_SkyBottom, m_SkyBottomCombo, SkyFace_Bottom);
	}
}

void CSkyDialog::OnSkyfront() 
{
	if (m_hWnd)
	{
	if (m_SkyFront.GetCheck() == 0)
	{
		ResetSkySurface(m_SkyFrontCombo, SkyFace_Front);
	}

	UpdateSkyFaceUI (m_SkyFront, m_SkyFrontCombo, SkyFace_Front);
	}
}

void CSkyDialog::OnSkyleft() 
{
	if (m_hWnd)
	{
	if (m_SkyLeft.GetCheck() == 0)
	{
		ResetSkySurface(m_SkyLeftCombo, SkyFace_Left);
	}

	UpdateSkyFaceUI (m_SkyLeft, m_SkyLeftCombo, SkyFace_Left);
	}
}

void CSkyDialog::OnSkyright() 
{
	if (m_hWnd)
	{
	if (m_SkyRight.GetCheck() == 0)
	{
		ResetSkySurface(m_SkyRightCombo, SkyFace_Right);
	}

	UpdateSkyFaceUI (m_SkyRight, m_SkyRightCombo, SkyFace_Right);
	}
}

void CSkyDialog::OnSkytop() 
{
	if (m_hWnd)
	{
	if (m_SkyTop.GetCheck() == 0)
	{
		ResetSkySurface(m_SkyTopCombo, SkyFace_Top);
	}
	
	UpdateSkyFaceUI (m_SkyTop, m_SkyTopCombo, SkyFace_Top);
	}
}



void CSkyDialog::UpdateFaceTextureName (CComboBox &FaceCombo, int FaceIndex, bool bGotOne)
{
	if (m_hWnd)
	{
	int Index;

	assert (FaceIndex >= 0);
	assert (FaceIndex < 6);
	char *pFaceName = NULL;
	if (bGotOne == false)
	{
		SkyFaceTexture *SkyFaces;
		char *pFaceName;
		pFaceName = Util_Strdup ("");
		geVec3d Axis;
		geFloat Speed, Scale;
	
		SkyFaces = m_ptEditDoc->m_pLevelMgr->GetSkyInfo (m_ptEditDoc->GetLevel(), &Axis, &Speed, &Scale);

		SkyFaces[FaceIndex].TextureName = pFaceName;

		return;
	}

	Index = FaceCombo.GetCurSel ();
	if (Index != CB_ERR)
	{
		CString FaceString, AllSameString;

		FaceCombo.GetLBText (Index, FaceString);
		if (m_ptEditDoc != NULL)
		{
			SkyFaceTexture *SkyFaces;
			geVec3d Axis;
			geFloat Speed, Scale;
	
			SkyFaces = m_ptEditDoc->m_pLevelMgr->GetSkyInfo (m_ptEditDoc->GetLevel(), &Axis, &Speed, &Scale);
			pFaceName = SkyFaces[FaceIndex].TextureName;
			if (pFaceName != NULL)
			{
				geRam_Free (pFaceName);
				pFaceName = NULL;
			}

			pFaceName = Util_Strdup (FaceString);
			SkyFaces[FaceIndex].TextureName = pFaceName;

			CString testString = (CString)pFaceName;
			FaceCombo.GetLBText (Index, FaceString);
			if (testString != FaceString)
				AfxMessageBox("Unable to correctly process sky texture");

								//	test to see if this face is different from AllTheSame
			if (m_SkyAlltheSameCombo.GetCurSel() != -1)	//	is it not blank?
			{					//	get a string for the current selection
				m_SkyAlltheSameCombo.GetLBText(m_SkyAlltheSameCombo.GetCurSel(), AllSameString);
								//	if this string don't match with the AllTheSame?
				if (FaceString != AllSameString)
				{				//	blank the AllTheSame combo
					m_SkyAlltheSameCombo.SetCurSel(-1);
				}
			}
		}
	}
	}
	return;
}

	
void CSkyDialog::OnSelchangeCbskyleft() 
{
	UpdateFaceTextureName (m_SkyLeftCombo, SkyFace_Left, true);
}

void CSkyDialog::OnSelchangeCbskyright() 
{
	UpdateFaceTextureName (m_SkyRightCombo, SkyFace_Right, true);
}

void CSkyDialog::OnSelchangeCbskytop() 
{
	UpdateFaceTextureName (m_SkyTopCombo, SkyFace_Top, true);
}

void CSkyDialog::OnSelchangeCbskybottom() 
{
	UpdateFaceTextureName (m_SkyBottomCombo, SkyFace_Bottom, true);
}

void CSkyDialog::OnSelchangeCbskyfront() 
{
	UpdateFaceTextureName (m_SkyFrontCombo, SkyFace_Front, true);
}

void CSkyDialog::OnSelchangeCbskyback() 
{
	UpdateFaceTextureName (m_SkyBackCombo, SkyFace_Back, true);
}



void CSkyDialog::OnSelchangeCBallsame() 
{
	int		currSel = m_SkyAlltheSameCombo.GetCurSel();

	m_SkyLeftCombo.SetCurSel(currSel);
	m_SkyRightCombo.SetCurSel(currSel);
	m_SkyTopCombo.SetCurSel(currSel);	
	m_SkyBottomCombo.SetCurSel(currSel);
	m_SkyFrontCombo.SetCurSel(currSel);
	m_SkyBackCombo.SetCurSel(currSel);

	UpdateFaceTextureName (m_SkyLeftCombo, SkyFace_Left, true);
	UpdateFaceTextureName (m_SkyRightCombo, SkyFace_Right, true);
	UpdateFaceTextureName (m_SkyTopCombo, SkyFace_Top, true);
	UpdateFaceTextureName (m_SkyBottomCombo, SkyFace_Bottom, true);
	UpdateFaceTextureName (m_SkyFrontCombo, SkyFace_Front, true);
	UpdateFaceTextureName (m_SkyBackCombo, SkyFace_Back, true);
	
	m_SkyLeftCombo.EnableWindow();
	m_SkyRightCombo.EnableWindow();
	m_SkyTopCombo.EnableWindow();
	m_SkyBottomCombo.EnableWindow();
	m_SkyFrontCombo.EnableWindow();
	m_SkyBackCombo.EnableWindow();

	m_SkyLeft.SetCheck(1);
	m_SkyRight.SetCheck(1);
	m_SkyTop.SetCheck(1);
	m_SkyBottom.SetCheck(1);
	m_SkyFront.SetCheck(1);
	m_SkyBack.SetCheck(1);

	OnSkyleft();
	OnSkyright();
	OnSkytop();
	OnSkybottom();
	OnSkyfront();
	OnSkyback();

	UpdateData(TRUE);
	
}


void CSkyDialog::OnClearAllBtn() 
{
	m_SkyLeftCombo.SetCurSel(-1);
	m_SkyRightCombo.SetCurSel(-1);
	m_SkyTopCombo.SetCurSel(-1);
	m_SkyBottomCombo.SetCurSel(-1);
	m_SkyFrontCombo.SetCurSel(-1);
	m_SkyBackCombo.SetCurSel(-1);
	m_SkyAlltheSameCombo.SetCurSel(-1);

	UpdateFaceTextureName (m_SkyLeftCombo, SkyFace_Left, false);
	UpdateFaceTextureName (m_SkyRightCombo, SkyFace_Right, false);
	UpdateFaceTextureName (m_SkyTopCombo, SkyFace_Top, false);
	UpdateFaceTextureName (m_SkyBottomCombo, SkyFace_Bottom, false);
	UpdateFaceTextureName (m_SkyFrontCombo, SkyFace_Front, false);
	UpdateFaceTextureName (m_SkyBackCombo, SkyFace_Back, false);

	m_SkyLeftCombo.EnableWindow(FALSE);
	m_SkyRightCombo.EnableWindow(FALSE);
	m_SkyTopCombo.EnableWindow(FALSE);
	m_SkyBottomCombo.EnableWindow(FALSE);
	m_SkyFrontCombo.EnableWindow(FALSE);
	m_SkyBackCombo.EnableWindow(FALSE);

	m_SkyLeft.SetCheck(0);
	m_SkyRight.SetCheck(0);
	m_SkyTop.SetCheck(0);
	m_SkyBottom.SetCheck(0);
	m_SkyFront.SetCheck(0);
	m_SkyBack.SetCheck(0);

	OnSkyleft();
	OnSkyright();
	OnSkytop();
	OnSkybottom();
	OnSkyfront();
	OnSkyback();
	UpdateData(TRUE);
	
}




static const geVec3d AxisVectors[3] =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

void CSkyDialog::OnAxisButton() 
{
	UpdateData (TRUE);
	m_ptEditDoc->m_pLevelMgr->SetSkyRotationAxis (m_ptEditDoc->GetLevel(), &AxisVectors[m_RotationAxis]);
}

void CSkyDialog::OnKillfocusEditspeed() 
{
	UpdateData (TRUE);

	m_ptEditDoc->m_pLevelMgr->SetSkyRotationSpeed (m_ptEditDoc->GetLevel(), m_RotationSpeed);
}

void CSkyDialog::OnKillfocusEditscale() 
{
	UpdateData (TRUE);

	m_ptEditDoc->m_pLevelMgr->SetSkyTextureScale(m_ptEditDoc->GetLevel(), m_TextureScale);
}

void CSkyDialog::OnOK() 
{
	UpdateData(TRUE);

						//	post 0.5 release
						//	set command panel to the templates tab
	if (m_ptEditDoc->m_pMainFrame->m_CommandDialogBar)
	{
		CTabCtrl*	pCommandPanelTabCtrl = NULL;

		pCommandPanelTabCtrl = (CTabCtrl*)m_ptEditDoc->m_pMainFrame->m_CommandDialogBar.GetDlgItem(IDC_COMMAND_TAB);
		if (pCommandPanelTabCtrl)
		{
		pCommandPanelTabCtrl->SetCurSel((MAINFRM_COMMANDPANEL_TAB)MAINFRM_COMMANDPANEL_TEMPLATES);
		m_ptEditDoc->m_pMainFrame->SetCommandPanelTab(MAINFRM_COMMANDPANEL_TEMPLATES);
		}
	}

	this->DestroyWindow();	// post 0.5 release
//	CDialog::OnOK(); //for modal dialog
}

			//	post 0.5 release
void CSkyDialog::OnClose() 
{
	OnOK();	
	CDialog::OnClose();
}

			//	post 0.5 release
void CSkyDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this)
		SetFocus();
	
	CDialog::OnMouseMove(nFlags, point);
}
