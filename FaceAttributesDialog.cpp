/****************************************************************************************/
/*  FaceAttributesDialog.cpp                                                            */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax, John Moore                        */
/*  Description:  Face flag UI                                                          */
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
/*  Modified by Tom Morris for tDesigner3d ver. 0.7, Nov. 2, 2002                       */
/****************************************************************************************/
#include "stdafx.h"
#include "Globals.h"
#include "FaceAttributesDialog.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* 10/31/2002 Wendell Buckner
    Raise texture limits to 16384 x 16384.
#define MIN_TEX_OFF	-256
#define MAX_TEX_OFF	256 */
#define MIN_TEX_OFF	-16384
#define MAX_TEX_OFF	 16384
#define MIN_TEX_SCL	-20
#define MAX_TEX_SCL	20
#define MIN_TEX_ANG	-359
#define MAX_TEX_ANG	359

#define UNIT_OFFSET_INCR	1
#define UNIT_SCALE_INCR	0.05f
#define UNIT_ANGLE_INCR	1


// This table keeps track of the latest selection used in the combo box
// so that it can be set correctly when the dialog is closed and re-opened.
static int ComboBoxSelections[][2] =
	{
		{IDC_CBXOFFSET, 2},
		{IDC_CBYOFFSET, 2},
		{IDC_CBXSCALE, 2},
		{IDC_CBYSCALE, 2},
		{IDC_CBXLIGHTSCALE, 2},
		{IDC_CBYLIGHTSCALE, 2},
		{IDC_CBANGLE, 2}
	};
static int nComboSelections = sizeof (ComboBoxSelections) / (2*sizeof (int));

static int GetComboSelection (const CComboBox &Combo)
{
	int CtrlId = Combo.GetDlgCtrlID ();

	for (int i = 0; i < nComboSelections; ++i)
	{
		if (ComboBoxSelections[i][0] == CtrlId)
		{
			return ComboBoxSelections[i][1];
		}
	}
	return 0;
}
/*
static void SetComboSelection (const CComboBox &Combo)
{
	int CtrlId = Combo.GetDlgCtrlID ();

	for (int i = 0; i < nComboSelections; ++i)
	{
		if (ComboBoxSelections[i][0] == CtrlId)
		{
			ComboBoxSelections[i][1] = Combo.GetCurSel ();
			return;
		}
	}
}
*/


typedef struct
{
	char *Text;
	float val;
} ComboBoxItem;



static void FillComboBox (CComboBox &Combo, const ComboBoxItem Items[], int nItems)
{
	int i;

	Combo.ResetContent ();
	for (i = 0; i < nItems; ++i)
	{
		int Index;

		Index = Combo.AddString (Items[i].Text);
		if ((Index != CB_ERR) && (Index != CB_ERRSPACE))
		{
			// this big ugly cast lets us store a float in a DWORD.
			Combo.SetItemData (Index, *((DWORD *)&(Items[i].val)));
		}
	}

	Combo.SetCurSel (::GetComboSelection (Combo));
}



//CFaceAttributesDialog::CFaceAttributesDialog(CtDesignerDoc* ptEditDoc, CWnd* pParent) // old gedit
CFaceAttributesDialog::CFaceAttributesDialog(CWnd* pParent  /*=NULL*/)	// new g3dc
	: CDialog(CFaceAttributesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFaceAttributesDialog)
	m_TextureYScale = 1.0;
	m_TextureXScale = 1.0;
	m_TextureYOffset = 0;
	m_TextureXOffset = 0;
	m_TextureAngle = 0.0;
	m_FullBright = FALSE;
	m_Light = FALSE;
	m_Mirror = FALSE;
	m_Sky = FALSE;
	m_LightXScale = 0.0f;
	m_LightYScale = 0.0f;
	m_LightIntensity = 0;
	m_Flat = FALSE;
	m_Gouraud = FALSE;
	m_MipMapBias = 1.0f;
	m_TextureLock = FALSE;
	m_Reflectivity = 1.0f;
	m_Translucency = 255.0f;
	m_Transparent = FALSE;
	//}}AFX_DATA_INIT

	m_iOldTexYOffset = 0;
	m_iOldTexXOffset = 0;

	CDialog::Create(IDD,pParent);

//	m_pDoc = CGlobals::GetActiveDocument();

	SetupDialog();
}

void CFaceAttributesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFaceAttributesDialog)
	DDX_Control(pDX, IDC_FACETRANSLUCENCY, m_EditTranslucency);
	DDX_Control(pDX, IDC_REFLECTIVITY, m_EditReflectivity);
	DDX_Control(pDX, IDC_MIPMAPBIAS, m_EditMipMapBias);
	DDX_Control(pDX, IDC_FACELIGHTINTENSITY, m_EditLightIntensity);
	DDX_Control(pDX, IDC_CBYLIGHTSCALE, m_ComboYLightScale);
	DDX_Control(pDX, IDC_CBXLIGHTSCALE, m_ComboXLightScale);
	DDX_Control(pDX, IDC_EDITYLIGHTSCALE, m_EditLightYScale);
	DDX_Control(pDX, IDC_EDITXLIGHTSCALE, m_EditLightXScale);
	DDX_Control(pDX, IDC_EDITANGLE, m_EditAngle);
	DDX_Control(pDX, IDC_EDITYSCALE, m_EditYScale);
	DDX_Control(pDX, IDC_EDITXSCALE, m_EditXScale);
	DDX_Control(pDX, IDC_EDITYOFFSET, m_EditYOffset);
	DDX_Control(pDX, IDC_EDITXOFFSET, m_EditXOffset);
	DDX_Control(pDX, IDC_CBXOFFSET, m_ComboXOffset);
	DDX_Control(pDX, IDC_CBYOFFSET, m_ComboYOffset);
	DDX_Control(pDX, IDC_CBXSCALE, m_ComboXScale);
	DDX_Control(pDX, IDC_CBYSCALE, m_ComboYScale);
	DDX_Control(pDX, IDC_CBANGLE, m_ComboAngle);
	DDX_Text(pDX, IDC_EDITYSCALE, m_TextureYScale);
	DDX_Text(pDX, IDC_EDITXSCALE, m_TextureXScale);
	DDV_MinMaxFloat(pDX, m_TextureXScale, -20.0f, 20.0f);
	DDV_MinMaxFloat(pDX, m_TextureYScale, -20.0f, 20.0f);
	DDX_Text(pDX, IDC_EDITYOFFSET, m_TextureYOffset);
	DDX_Text(pDX, IDC_EDITXOFFSET, m_TextureXOffset);
// changed QD
	DDV_MinMaxInt(pDX, m_TextureXOffset, -16384.0f, 16384.0f);
	DDV_MinMaxInt(pDX, m_TextureYOffset, -16384.0f, 16384.0f);
// end change
	DDX_Text(pDX, IDC_EDITANGLE, m_TextureAngle);
	DDV_MinMaxFloat(pDX, m_TextureAngle, -359.0, 359.0);
	DDX_Check(pDX, IDC_FACEFULLBRIGHT, m_FullBright);
	DDX_Check(pDX, IDC_FACELIGHT, m_Light);
	DDX_Check(pDX, IDC_FACEMIRROR, m_Mirror);
	DDX_Check(pDX, IDC_FACESKY, m_Sky);
	DDX_Text(pDX, IDC_EDITXLIGHTSCALE, m_LightXScale);
	DDX_Text(pDX, IDC_EDITYLIGHTSCALE, m_LightYScale);
	DDV_MinMaxFloat(pDX, m_LightXScale, -20.0f, 20.0f);
	DDV_MinMaxFloat(pDX, m_LightYScale, -20.0f, 20.0f);

	DDX_Text(pDX, IDC_FACELIGHTINTENSITY, m_LightIntensity);
	DDV_MinMaxInt(pDX, m_LightIntensity, 0, 999999);
	DDX_Check(pDX, IDC_FACEFLAT, m_Flat);
	DDX_Check(pDX, IDC_FACEGOURAUD, m_Gouraud);
	DDX_Text(pDX, IDC_MIPMAPBIAS, m_MipMapBias);
	DDX_Check(pDX, IDC_TEXTURELOCK, m_TextureLock);
	DDX_Text(pDX, IDC_REFLECTIVITY, m_Reflectivity);
	DDX_Text(pDX, IDC_FACETRANSLUCENCY, m_Translucency);
	DDX_Check(pDX, IDC_TRANSPARENT, m_Transparent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFaceAttributesDialog, CDialog)
	//{{AFX_MSG_MAP(CFaceAttributesDialog)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDITXOFFSET, OnChangeXOffset)
	ON_EN_KILLFOCUS(IDC_EDITXOFFSET, OnKillfocusXOffset)
	ON_EN_CHANGE(IDC_EDITYOFFSET, OnChangeYOffset)
	ON_EN_KILLFOCUS(IDC_EDITYOFFSET, OnKillfocusYOffset)
	ON_EN_CHANGE(IDC_EDITXSCALE, OnChangeXScale)
	ON_EN_KILLFOCUS(IDC_EDITXSCALE, OnKillfocusXScale)
	ON_EN_CHANGE(IDC_EDITYSCALE, OnChangeYScale)
	ON_EN_KILLFOCUS(IDC_EDITYSCALE, OnKillfocusYScale)
	ON_EN_CHANGE(IDC_EDITANGLE, OnChangeAngle)
	ON_EN_KILLFOCUS(IDC_EDITANGLE, OnKillfocusAngle)
	ON_BN_CLICKED(IDC_FLIPVERTICAL, OnFlipvertical)
	ON_BN_CLICKED(IDC_FLIPHORIZONTAL, OnFliphorizontal)
	ON_WM_ACTIVATE()
	ON_EN_CHANGE(IDC_EDITXLIGHTSCALE, OnChangeEditxlightscale)
	ON_EN_KILLFOCUS(IDC_EDITXLIGHTSCALE, OnKillfocusEditxlightscale)
	ON_EN_CHANGE(IDC_EDITYLIGHTSCALE, OnChangeEditylightscale)
	ON_EN_KILLFOCUS(IDC_EDITYLIGHTSCALE, OnKillfocusEditylightscale)
	ON_BN_CLICKED(IDC_FACEMIRROR, OnFacemirror)
	ON_BN_CLICKED(IDC_FACESKY, OnFacesky)
	ON_BN_CLICKED(IDC_FACEFULLBRIGHT, OnFacefullbright)
	ON_EN_KILLFOCUS(IDC_FACELIGHTINTENSITY, OnKillfocusFacelightintensity)
	ON_BN_CLICKED(IDC_FACEGOURAUD, OnFacegouraud)
	ON_BN_CLICKED(IDC_FACEFLAT, OnFaceflat)
	ON_EN_KILLFOCUS(IDC_MIPMAPBIAS, OnKillfocusMipmapbias)
	ON_BN_CLICKED(IDC_TEXTURELOCK, OnTexturelock)
	ON_EN_KILLFOCUS(IDC_REFLECTIVITY, OnKillfocusReflectivity)
	ON_EN_KILLFOCUS(IDC_FACETRANSLUCENCY, OnKillfocusFacetranslucency)
	ON_BN_CLICKED(IDC_TRANSPARENT, OnTransparent)
	ON_BN_CLICKED(IDC_FACE_APPLYBTN, OnFaceApplybtn)
	ON_BN_CLICKED(ID_DEFAULTS, OnResetAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaceAttributesDialog message handlers


BOOL CFaceAttributesDialog::OnInitDialog()
{
	m_pDoc = CGlobals::GetActiveDocument();

	m_pDoc->m_pMainFrame->HideAllPropDialogsBut(this);

	pFace = NULL;

//	UpdatePolygonFocus (pFace);			// Update dialog members from face

	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);


	GetDlgItem( IDC_FACELIGHTINTENSITY )->EnableWindow( m_Light ) ;

	UpdateData (FALSE);
	CDialog::OnInitDialog();



	return TRUE;
}


bool CFaceAttributesDialog::UpdateFaceAttributes(CGEditProDoc *pDoc, Face *pFace)
{
	if (pDoc != m_pDoc)
		AfxMessageBox("pDoc != m_pDoc in UpdateFaceAttributes");

	if (m_pDoc != NULL)
	{
		UpdatePolygonFocus (pFace);			// Update dialog members from face
//		SetupDialog ();		//	no no no don't put that thang here.



//		GetDlgItem( IDC_FACELIGHTINTENSITY )->EnableWindow( m_Light ) ;

				//	Restore focus to active view so arrow keys work
				//	for cycling through individual faces

				//	Restore focus to active view
	CMDIChildWnd *pMDIChild	=(CMDIChildWnd *)pDoc->m_pMainFrame->MDIGetActive();
	if(pMDIChild)
	{
		CView	*cv	=(CView *)pMDIChild->GetActiveView();
		if( cv)
			cv->SetFocus() ;
	}

	UpdateData (FALSE);

	}

	return true;
}


void CFaceAttributesDialog::ShowDialog()
{
	SetWindowPos(NULL,5,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
	this->ShowWindow(SW_SHOW);
}




void CFaceAttributesDialog::SetupDialog()
{

	// Fill combo boxes with proper data and set accordingly
	static const ComboBoxItem OffsetValues[] =
	{
		{"4", 4.0f},
		{"8", 8.0f},
		{"16", 16.0f},
		{"32", 32.0f},
		{"64", 64.0f},
		{"128", 128.0f}
	};
	static const int nOffsetItems = sizeof (OffsetValues) / sizeof (ComboBoxItem);

	static const ComboBoxItem ScaleValues[] =
	{
		"0.1", 0.1f,
		"0.2", 0.2f,
		"0.5", 0.5f,
		"0.75", 0.75f,
		"1.0", 1.0f,
		"2.0", 2.0f
	};
	static const int nScaleItems = sizeof (ScaleValues) / sizeof (ComboBoxItem);

	static const ComboBoxItem AngleValues[] =
	{
		"5", 5.0f,
		"10", 10.0f,
		"15", 15.0f,
		"30", 30.0f,
		"45", 45.0f,
		"90", 90.0f
	};
	static const int nAngleItems = sizeof (AngleValues) / sizeof (ComboBoxItem);

	FillComboBox (m_ComboXOffset, OffsetValues, nOffsetItems);
	FillComboBox (m_ComboYOffset, OffsetValues, nOffsetItems);
	FillComboBox (m_ComboXScale, ScaleValues, nScaleItems);
	FillComboBox (m_ComboYScale, ScaleValues, nScaleItems);
	FillComboBox (m_ComboXLightScale, ScaleValues, nScaleItems);
	FillComboBox (m_ComboYLightScale, ScaleValues, nScaleItems);
	FillComboBox (m_ComboAngle, AngleValues, nAngleItems);
}





void CFaceAttributesDialog::AssignFaceValues (Face *pFace)
{
	Face_SetLight (pFace, m_Light);
	Face_SetFullBright (pFace, m_FullBright);
	Face_SetGouraud (pFace, m_Gouraud);
	Face_SetFlat (pFace, m_Flat);
	Face_SetTextureLock (pFace, m_TextureLock);
	Face_SetTransparent (pFace, m_Transparent);
	Face_SetSky (pFace, m_Sky);
	Face_SetMirror (pFace, m_Mirror);
	Face_SetTextureRotate (pFace, (geFloat)m_TextureAngle);
	Face_SetTextureShift (pFace, m_TextureXOffset, m_TextureYOffset);
	Face_SetTextureScale (pFace, m_TextureXScale, m_TextureYScale);
	Face_SetLightScale (pFace, m_LightXScale, m_LightYScale);
	Face_SetLightIntensity (pFace, m_LightIntensity) ;
	Face_SetMipMapBias (pFace, m_MipMapBias);
	Face_SetTranslucency (pFace, m_Translucency);
	Face_SetReflectivity (pFace, m_Reflectivity);
}

static geBoolean AssignFaceValues (Face *pFace, void *lParam)
{
	CFaceAttributesDialog *pDlg = (CFaceAttributesDialog *)lParam;

	pDlg->AssignFaceValues (pFace);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnResetAll()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData(TRUE);

	m_TextureXOffset = 0;
	m_TextureYOffset = 0;
	m_TextureAngle = 0.0f;

	m_TextureXScale = 1.0f;
	m_TextureYScale = 1.0f;
	m_LightXScale = 2.0f;
	m_LightYScale = 2.0f;

	m_Mirror = FALSE;
	m_FullBright = FALSE;
	m_Gouraud = FALSE;
	m_Flat = FALSE;
	m_TextureLock = FALSE;
	m_Transparent = FALSE;
	m_Sky = FALSE;
	m_Light = FALSE;
	m_LightIntensity = 0;
	m_MipMapBias = 1.0f;
	m_Reflectivity = 1.0f;
	m_Translucency = 255.0f;

	UpdateData(FALSE);

	m_iOldTexYOffset = 0;
	m_iOldTexXOffset = 0;

	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::AssignFaceValues, this);

	AssignCurrentToViews();
}

float CFaceAttributesDialog::DoIncrement (float v, float incr, float vMin, float vMax, int nSBCode)
{
	if (nSBCode == SB_LINEUP)
	{
		v += incr;
	}
	else
	{
		v -= incr;
	}

	if (v < vMin)
	{
		v = vMin;
	}
	if (v > vMax)
	{
		v = vMax;
	}
	return v;
}


float CFaceAttributesDialog::GetIncrement (CComboBox &combo)
{
	int CurSel;
	DWORD d;

	CurSel = combo.GetCurSel ();
	if (CurSel == CB_ERR)
	{
		return 1.0f;
	}
	d = combo.GetItemData (CurSel);
	return *((float *)&d);
}


#pragma warning (disable:4100)
void CFaceAttributesDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ((nSBCode != SB_LINEUP) && (nSBCode != SB_LINEDOWN))
	{
		return;
	}

	UpdateData(TRUE);
	switch (pScrollBar->GetDlgCtrlID ())
	{
			//	First we deal with all of the Unit scrolling bars...
			//	X Offset...
		case IDC_SBXOFFSET_UNIT:
			m_TextureXOffset = (int)DoIncrement ((float)m_TextureXOffset, UNIT_OFFSET_INCR, MIN_TEX_OFF, MAX_TEX_OFF, nSBCode);
			UpdateData (FALSE);
			OnKillfocusXOffset ();
			break;
			//	Y Offset...
		case IDC_SBYOFFSET_UNIT:
			m_TextureYOffset = (int)DoIncrement ((float)m_TextureYOffset, UNIT_OFFSET_INCR, MIN_TEX_OFF, MAX_TEX_OFF, nSBCode);
			UpdateData (FALSE);
			OnKillfocusYOffset ();
			break;
			//	X Scale...
		case IDC_SBXSCALE_UNIT:
			m_TextureXScale = DoIncrement (m_TextureXScale, UNIT_SCALE_INCR, MIN_TEX_SCL, MAX_TEX_SCL, nSBCode);
			UpdateData (FALSE);
			OnKillfocusXScale ();
			break;
			//	Y Scale...
		case IDC_SBYSCALE_UNIT:
			m_TextureYScale = DoIncrement (m_TextureYScale, UNIT_SCALE_INCR, MIN_TEX_SCL, MAX_TEX_SCL, nSBCode);
			UpdateData (FALSE);
			OnKillfocusYScale ();
			break;
			//	X Scale...
		case IDC_SBXLIGHTSCALE_UNIT:
			m_LightXScale = DoIncrement (m_LightXScale, UNIT_SCALE_INCR, MIN_TEX_SCL, MAX_TEX_SCL, nSBCode);
			UpdateData (FALSE);
			OnKillfocusEditxlightscale ();
			break;
			//	Y Scale...
		case IDC_SBYLIGHTSCALE_UNIT:
			m_LightYScale = DoIncrement (m_LightYScale, UNIT_SCALE_INCR, MIN_TEX_SCL, MAX_TEX_SCL, nSBCode);
			UpdateData (FALSE);
			OnKillfocusEditylightscale ();
			break;
			//	Angle...
		case IDC_SBANGLE_UNIT:
			m_TextureAngle = DoIncrement (m_TextureAngle, UNIT_ANGLE_INCR, MIN_TEX_ANG, MAX_TEX_ANG, nSBCode);
			UpdateData (FALSE);
			OnKillfocusAngle ();
			break;

			//	Now deal with all of the increment scrollers...
			//	X Offset...
		case IDC_SBXOFFSET_INCR:
			m_TextureXOffset = (int)DoIncrement ((float)m_TextureXOffset, GetIncrement (m_ComboXOffset), MIN_TEX_OFF, MAX_TEX_OFF, nSBCode);
			UpdateData (FALSE);
			OnKillfocusXOffset ();
			break;
			//	Y Offset...
		case IDC_SBYOFFSET_INCR:
			m_TextureYOffset = (int)DoIncrement ((float)m_TextureYOffset, GetIncrement (m_ComboYOffset), MIN_TEX_OFF, MAX_TEX_OFF, nSBCode);
			UpdateData (FALSE);
			OnKillfocusYOffset ();
			break;
			//	X Scale...
		case IDC_SBXSCALE_INCR:
			m_TextureXScale = DoIncrement (m_TextureXScale, GetIncrement (m_ComboXScale), MIN_TEX_SCL, MAX_TEX_SCL, nSBCode);
			UpdateData (FALSE);
			OnKillfocusXScale ();
			break;
			//	Y Scale...
		case IDC_SBYSCALE_INCR:
			m_TextureYScale = DoIncrement (m_TextureYScale, GetIncrement (m_ComboYScale), MIN_TEX_SCL, MAX_TEX_SCL, nSBCode);
			UpdateData (FALSE);
			OnKillfocusYScale ();
			break;
		case IDC_SBXLIGHTSCALE_INCR:
			m_LightXScale = DoIncrement (m_LightXScale, GetIncrement (m_ComboXLightScale), MIN_TEX_SCL, MAX_TEX_SCL, nSBCode);
			UpdateData (FALSE);
			OnKillfocusEditxlightscale ();
			break;
			//	Y Scale...
		case IDC_SBYLIGHTSCALE_INCR:
			m_LightYScale = DoIncrement (m_LightYScale, GetIncrement (m_ComboYLightScale), MIN_TEX_SCL, MAX_TEX_SCL, nSBCode);
			UpdateData (FALSE);
			OnKillfocusEditylightscale ();
			break;
			//	Angle...
		case IDC_SBANGLE_INCR:
			m_TextureAngle = DoIncrement (m_TextureAngle, GetIncrement (m_ComboAngle), MIN_TEX_ANG, MAX_TEX_ANG, nSBCode);
			UpdateData (FALSE);
			OnKillfocusAngle ();
			break;

		default :
			assert (0);
			return;

	}
	UpdateData(FALSE);
}

#pragma warning (default:4100)

//	And this is here to update the views as changes are made.
void CFaceAttributesDialog::AssignCurrentToViews()
{
	if (!m_pDoc)
		m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		// update child faces on all selected brushes
		int NumSelBrushes = 0;

		NumSelBrushes = SelBrushList_GetSize (m_pDoc->DocVarsGetPointer()->m_pSelBrushes);
		for (int i = 0; i < NumSelBrushes; ++i)
		{
			Brush *pBrush = NULL;
			pBrush = SelBrushList_GetBrush (m_pDoc->DocVarsGetPointer()->m_pSelBrushes, i);
			if (pBrush)
				Brush_UpdateChildFaces (pBrush);
		}

		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		m_pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);	//	best current one
	}
}

/*
Problem updating textured view
See the following functions
CtDesignerDoc::UpdateFaceAttributes, RebuildTrees, UpdateAllViews, OnGbspnowater()



*/



/*
void ::OnCancel()
{
	if (m_pDoc == NULL)
	m_pDoc = CGlobals::GetActiveDocument();

	m_pDoc->NullFaceAttributes ();
	DestroyWindow();
}
*/

void CFaceAttributesDialog::OnCancel()
{
	return;
}

void CFaceAttributesDialog::PostNcDestroy()
{
//	delete this;
//	DestroyWindow();
}

// The document calls this function whenever a face is selected or deselected.
void CFaceAttributesDialog::UpdatePolygonFocus (Face *pFace)
{
	if (!m_pDoc)
	{
		AfxMessageBox("No Document for UpdatePolygonFocus in FaceAttribs");
		m_pDoc = CGlobals::GetActiveDocument();
	}


//	Face *pFace = NULL;
	if (pFace == NULL)
		pFace = SelFaceList_GetFace (m_pDoc->DocVarsGetPointer()->m_pSelFaces, 0);

	if (!pFace)
		AfxMessageBox("No pFace in FaceAttribs");

	else if (pFace)
	{
		m_TextureAngle	= Face_GetTextureRotate (pFace);
		Face_GetTextureShift (pFace, &m_TextureXOffset, &m_TextureYOffset);
		Face_GetTextureScale (pFace, &m_TextureXScale, &m_TextureYScale);
		Face_GetLightScale (pFace, &m_LightXScale, &m_LightYScale);
		m_LightIntensity = Face_GetLightIntensity (pFace);
		m_MipMapBias = Face_GetMipMapBias (pFace);
		m_Translucency = Face_GetTranslucency (pFace);
		m_Reflectivity = Face_GetReflectivity (pFace);

		m_Light			=Face_IsLight (pFace);
		m_FullBright	=Face_IsFullBright (pFace);
		m_Gouraud		=Face_IsGouraud (pFace);
		m_Flat			=Face_IsFlat (pFace);
		m_TextureLock	=Face_IsTextureLocked (pFace);
		m_Transparent	=Face_IsTransparent (pFace);
		m_Sky			=Face_IsSky (pFace);
		m_Mirror		=Face_IsMirror (pFace);
		GetDlgItem( IDC_FACELIGHTINTENSITY )->EnableWindow( m_Light ) ;
	}

	UpdateData(FALSE);
}
/*
static BOOL CheckIntValue (CEdit &Edit)
{
	CString Text;

	Edit.GetWindowText (Text);
	return (Text != "");
}
*/

/*
static BOOL CheckFloatValue (CEdit &Edit)
{
	CString Text;

	Edit.GetWindowText (Text);
	if (Text != "")
	{
		int pos, len;
		len = Text.GetLength ();
		pos = Text.Find ('.') + 1;

		if (pos != len)
		{
			return TRUE;
		}
	}
	return FALSE;
}
*/

/*
#pragma warning (disable:4100)
void CFaceAttributesDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint CursorPos;

	//	CHANGE!	04/16/97	John Moore
	//	Is the mouse cursor within this dialog?  If so, activate it...
	CRect WindowRect;

	GetCursorPos( &CursorPos );
	GetWindowRect( &WindowRect );
	//ClientToScreen( &WindowRect );

	if( WindowRect.PtInRect( CursorPos ) )
	{
		if( GetActiveWindow() != (CWnd*) this )
		{
			BringWindowToTop();
			SetFocus();
		}
	}
	//	End of CHANGE

	//CDialog::OnMouseMove(nFlags, point);
}
#pragma warning (default:4100)
*/


static BOOL OnIntEditChange (CEdit &Edit, int *val, int DefaultVal)
{
	CString Text;

	Edit.GetWindowText (Text);
	if (Text == "")
	{
		*val = DefaultVal;
		return FALSE;
	}
	return TRUE;
}

static void OnIntKillFocus (CEdit &Edit, int *val, int DefaultVal, const char *Defstr)
{
	if (!OnIntEditChange (Edit, val, DefaultVal))
	{
		Edit.SetWindowText (Defstr);
	}
}

static BOOL OnFloatEditChange(CEdit &Edit, float *val, float DefaultVal)
{
	CString Text;

	Edit.GetWindowText(Text);
	if (Text == "")
	{
		*val = DefaultVal;
		return FALSE;
	}
	return TRUE;
}

static void OnFloatKillFocus (CEdit &Edit, float *val, float DefaultVal, const char *Defstr)
{
	if (!OnFloatEditChange (Edit, val, DefaultVal))
	{
		Edit.SetWindowText (Defstr);
	}
}

void CFaceAttributesDialog::OnChangeXOffset()
{
	OnIntEditChange (m_EditXOffset, &m_TextureXOffset, 0);
}

void CFaceAttributesDialog::OnChangeYOffset()
{
	OnIntEditChange (m_EditYOffset, &m_TextureYOffset, 0);
}

void CFaceAttributesDialog::OnChangeXScale()
{
	OnFloatEditChange (m_EditXScale, &m_TextureXScale, 1.0f);
}

void CFaceAttributesDialog::OnChangeYScale()
{
	OnFloatEditChange (m_EditYScale, &m_TextureYScale, 1.0f);
}

void CFaceAttributesDialog::OnChangeEditxlightscale()
{
	OnFloatEditChange (m_EditLightXScale, &m_LightXScale, 1.0f);
}

void CFaceAttributesDialog::OnChangeEditylightscale()
{
	OnFloatEditChange (m_EditLightYScale, &m_LightYScale, 1.0f);
}

void CFaceAttributesDialog::OnChangeAngle()
{
	OnFloatEditChange (m_EditAngle, &m_TextureAngle, 0.0f);
}

static geBoolean ChangeXOffset (Face *pFace, void *lParam)
{
	int *pXOffset = (int *)(lParam);
	int xOff, yOff;

	Face_GetTextureShift (pFace, &xOff, &yOff);
	Face_SetTextureShift (pFace, *pXOffset, yOff);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusXOffset()
{
	if (!m_pDoc)
		m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		UpdateData (TRUE);
// changed QD
		if (m_TextureXOffset > 16384.0f)
		{
			m_TextureXOffset = 16384.0f;
			UpdateData(FALSE);
		}
		if (m_TextureXOffset < -16384.0f)
		{
			m_TextureXOffset = -16384.0f;
			UpdateData(FALSE);
		}
// end change
		SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ChangeXOffset, &m_TextureXOffset);

		AssignCurrentToViews ();
	}
}


static geBoolean ChangeYOffset (Face *pFace, void *lParam)
{
	int *pYOffset = (int *)(lParam);
	int xOff, yOff;

	Face_GetTextureShift (pFace, &xOff, &yOff);
	Face_SetTextureShift (pFace, xOff, *pYOffset);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusYOffset()
{
	if (!m_pDoc)
		m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		UpdateData (TRUE);
// changed QD
		if (m_TextureYOffset > 16384.0f)
		{
			m_TextureYOffset = 16384.0f;
			UpdateData(FALSE);
		}
		if (m_TextureYOffset < -16384.0f)
		{
			m_TextureYOffset = -16384.0f;
			UpdateData(FALSE);
		}
// end change
		SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ChangeYOffset, &m_TextureYOffset);

		AssignCurrentToViews ();
	}
}

static geBoolean ChangeTextureXScale (Face *pFace, void *lParam)
{
	float *pXScale = (float *)lParam;
	float xScale, yScale;

	Face_GetTextureScale (pFace, &xScale, &yScale);
	Face_SetTextureScale (pFace, *pXScale, yScale);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusXScale()
{
	CString testString;							//	post 0.5 release
	if (!m_pDoc)
		m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		UpdateData (TRUE);

		if (m_TextureXScale > 20.0f)
		{
			m_TextureXScale = 20.0f;
			UpdateData(FALSE);
		}
		if (m_TextureXScale < -20.0f)
		{
			m_TextureXScale = -20.0f;
			UpdateData(FALSE);
		}

		SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ChangeTextureXScale, &m_TextureXScale);

		AssignCurrentToViews ();
	}
}

static geBoolean ChangeTextureYScale (Face *pFace, void *lParam)
{
	float *pYScale = (float *)lParam;
	float xScale, yScale;

	Face_GetTextureScale (pFace, &xScale, &yScale);
	Face_SetTextureScale (pFace, xScale, *pYScale);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusYScale()
{
	CString testString;							//	post 0.5 release
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		UpdateData (TRUE);

		if (m_TextureYScale > 20.0f)
		{
			m_TextureYScale = 20.0f;
			UpdateData(FALSE);
		}
		if (m_TextureYScale < -20.0f)
		{
			m_TextureYScale = -20.0f;
			UpdateData(FALSE);
		}

		SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ChangeTextureYScale, &m_TextureYScale);
		AssignCurrentToViews ();
	}
}

static geBoolean ChangeLightXScale (Face *pFace, void *lParam)
{
	float *pXScale = (float *)lParam;
	float xScale, yScale;

	Face_GetLightScale (pFace, &xScale, &yScale);
	Face_SetLightScale (pFace, *pXScale, yScale);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusEditxlightscale()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		UpdateData (TRUE);

		if (m_LightXScale > 20.0f)
		{
			m_LightXScale = 20.0f;
			UpdateData(FALSE);
		}
		if (m_LightXScale < -20.0f)
		{
			m_LightXScale = -20.0f;
			UpdateData(FALSE);
		}

		SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ChangeLightXScale, &m_LightXScale);
		AssignCurrentToViews ();
	}
}


static geBoolean ChangeLightYScale (Face *pFace, void *lParam)
{
	float *pYScale = (float *)lParam;
	float xScale, yScale;

	Face_GetLightScale (pFace, &xScale, &yScale);
	Face_SetLightScale (pFace, xScale, *pYScale);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusEditylightscale()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		UpdateData (TRUE);

		if (m_LightYScale > 20.0f)
		{
			m_LightYScale = 20.0f;
			UpdateData(FALSE);
		}
		if (m_LightYScale < -20.0f)
		{
			m_LightYScale = -20.0f;
			UpdateData(FALSE);
		}
		SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ChangeLightYScale, &m_LightYScale);
		AssignCurrentToViews ();
	}
}

static geBoolean ChangeTextureAngle (Face *pFace, void *lParam)
{
	float *pAngle = (float *)lParam;

	Face_SetTextureRotate (pFace, *pAngle);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusAngle()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		UpdateData (TRUE);

		if (m_TextureAngle > 359.0f)
		{
			m_TextureAngle = 359.0f;
			UpdateData(FALSE);
		}
		if (m_TextureAngle < -359.0f)
		{
			m_TextureAngle = -359.0f;
			UpdateData(FALSE);
		}
		SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::ChangeTextureAngle, &m_TextureAngle);
		AssignCurrentToViews ();
	}
}

static geBoolean ChangeLightIntensity (Face *pFace, void *lParam)
{
	int *pIntensity = (int *)lParam;

	Face_SetLightIntensity (pFace, *pIntensity);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusFacelightintensity()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	int	lastValue = m_LightIntensity;
	if (GetDlgItemInt(IDC_FACELIGHTINTENSITY) == NULL)
	{
		this->SetDlgItemInt(IDC_FACELIGHTINTENSITY, lastValue);
		return;
	}

	UpdateData (TRUE);
	OnIntKillFocus (m_EditLightIntensity, &m_LightIntensity, 300, "300");
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::ChangeLightIntensity, &m_LightIntensity);
	AssignCurrentToViews ();
}

static geBoolean ChangeMipMapBias (Face *pFace, void *lParam)
{
	float *pMipMapBias = (float *)lParam;

	Face_SetMipMapBias (pFace, *pMipMapBias);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusMipmapbias()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	int	lastValue = m_MipMapBias;
	if (GetDlgItemInt(IDC_MIPMAPBIAS) == NULL)
	{
		this->SetDlgItemInt(IDC_MIPMAPBIAS, lastValue);
		return;
	}


	UpdateData (TRUE);
	OnFloatKillFocus (m_EditMipMapBias, &m_MipMapBias, 0, "1.0");
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::ChangeMipMapBias, &m_MipMapBias);
	AssignCurrentToViews ();
}

static geBoolean ChangeReflectivity (Face *pFace, void *lParam)
{
	float *pReflectivity = (float *)lParam;

	Face_SetReflectivity (pFace, *pReflectivity);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusReflectivity()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	int	lastValue = m_Reflectivity;
	if (GetDlgItemInt(IDC_REFLECTIVITY) == NULL)
	{
		this->SetDlgItemInt(IDC_REFLECTIVITY, lastValue);
		return;
	}

	UpdateData (TRUE);
	OnFloatKillFocus (m_EditReflectivity, &m_Reflectivity, 0, "1.0");

	if(m_Reflectivity < 0.0f)
	{
		m_Reflectivity	=0.0f;
	}
	else if(m_Reflectivity > 255.0f)
	{
		m_Reflectivity	=10.0f;
	}

	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::ChangeReflectivity, &m_Reflectivity);
	AssignCurrentToViews ();
}

static geBoolean ChangeTranslucency (Face *pFace, void *lParam)
{
	float *pTranslucency = (float *)lParam;

	Face_SetTranslucency (pFace, *pTranslucency);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnKillfocusFacetranslucency()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	int	lastValue = m_Translucency;
	if (GetDlgItemInt(IDC_FACETRANSLUCENCY) == NULL)
	{
		this->SetDlgItemInt(IDC_FACETRANSLUCENCY, lastValue);
		return;
	}


	UpdateData (TRUE);
	OnFloatKillFocus (m_EditTranslucency, &m_Translucency, 0, "255.0");

	if(m_Translucency < 0.0f)
	{
		m_Translucency	=0.0f;
	}
	else if(m_Translucency > 255.0f)
	{
		m_Translucency	=255.0f;
	}
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::ChangeTranslucency, &m_Translucency);
	AssignCurrentToViews ();
}

static geBoolean FlipVertical (Face *pFace, void *)
{
	float xScale, yScale;

	Face_GetTextureScale (pFace, &xScale, &yScale);
	Face_SetTextureScale (pFace, xScale, -yScale);
	return GE_TRUE;
}

//	Flip it vertical by inverting the Y scale sign...
void CFaceAttributesDialog::OnFlipvertical()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData(TRUE);

	m_TextureYScale = -m_TextureYScale;

	UpdateData(FALSE);

	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, FlipVertical, NULL);

	AssignCurrentToViews();
}

static geBoolean FlipHorizontal (Face *pFace, void *)
{
	float xScale, yScale;

	Face_GetTextureScale (pFace, &xScale, &yScale);
	Face_SetTextureScale (pFace, -xScale, yScale);
	return GE_TRUE;
}

//	Same here except flip it horizontal...
void CFaceAttributesDialog::OnFliphorizontal()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData(TRUE);

	m_TextureXScale = -m_TextureXScale;

	UpdateData(FALSE);

	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, FlipHorizontal, NULL);

	AssignCurrentToViews();
}

static geBoolean SetLight (Face *pFace, void *lParam)
{
	BOOL *pLight = (BOOL *)lParam;

	Face_SetLight (pFace, *pLight);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnFacelight( void )
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData (TRUE);
	GetDlgItem( IDC_FACELIGHTINTENSITY )->EnableWindow( m_Light ) ;
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, SetLight, &m_Light);
	AssignCurrentToViews ();
}


static geBoolean SetMirror (Face *pFace, void *lParam)
{
	BOOL *pMirror = (BOOL *)lParam;

	Face_SetMirror (pFace, *pMirror);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnFacemirror()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData (TRUE);
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, SetMirror, &m_Mirror);
	AssignCurrentToViews ();
}

static geBoolean SetSky (Face *pFace, void *lParam)
{
	BOOL *pSky = (BOOL *)lParam;

	Face_SetSky (pFace, *pSky);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnFacesky()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData (TRUE);
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, SetSky, &m_Sky);
	AssignCurrentToViews ();
}

static geBoolean SetFullBright (Face *pFace, void *lParam)
{
	BOOL *pFullBright = (BOOL *)lParam;

	Face_SetFullBright (pFace, *pFullBright);
	return GE_TRUE;
}

void CFaceAttributesDialog::SetShadingChecks ()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	Face *pFace = SelFaceList_GetFace (m_pDoc->DocVarsGetPointer()->m_pSelFaces, 0);

	m_FullBright	=Face_IsFullBright (pFace);
	m_Gouraud		=Face_IsGouraud (pFace);
	m_Flat			=Face_IsFlat (pFace);
	UpdateData (FALSE);
}

void CFaceAttributesDialog::OnFacefullbright()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData (TRUE);
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::SetFullBright, &m_FullBright);
	SetShadingChecks ();
	AssignCurrentToViews ();
}

static geBoolean SetGouraud (Face *pFace, void *lParam)
{
	BOOL *pGouraud = (BOOL *)lParam;

	Face_SetGouraud (pFace, *pGouraud);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnFacegouraud()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData (TRUE);
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::SetGouraud, &m_Gouraud);
	SetShadingChecks ();
	AssignCurrentToViews ();
}

static geBoolean SetFlat (Face *pFace, void *lParam)
{
	BOOL *pFlat = (BOOL *)lParam;

	Face_SetFlat (pFace, *pFlat);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnFaceflat()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData (TRUE);
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::SetFlat, &m_Flat);
	SetShadingChecks ();
	AssignCurrentToViews ();
}

static geBoolean SetTextureLock (Face *pFace, void *lParam)
{
	BOOL *pTextureLock = (BOOL *)lParam;

	Face_SetTextureLock (pFace, *pTextureLock);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnTexturelock()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData (TRUE);
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::SetTextureLock, &m_TextureLock);
	AssignCurrentToViews ();
}

static geBoolean SetTransparent (Face *pFace, void *lParam)
{
	 BOOL *pTransparent = (BOOL *)lParam;

	Face_SetTransparent (pFace, *pTransparent);
	return GE_TRUE;
}

void CFaceAttributesDialog::OnTransparent()
{
	if (m_pDoc == NULL)
		m_pDoc = CGlobals::GetActiveDocument();

	UpdateData (TRUE);
	SelFaceList_Enum (m_pDoc->DocVarsGetPointer()->m_pSelFaces, ::SetTransparent, &m_Transparent);
	AssignCurrentToViews ();
}

void CFaceAttributesDialog::OnOK ()
{
	// do nothing...don't want to close the thing, ya know...
}

void CFaceAttributesDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized  )
{
	CDialog::OnActivate( nState, pWndOther, bMinimized ) ;
}

BOOL CFaceAttributesDialog::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	switch( nID )
	{
		case IDC_FACELIGHT:
			OnFacelight() ;
			break;
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}




void CFaceAttributesDialog::OnFaceApplybtn()
{
//	m_pDoc->m_pMainFrame->UpdateMainControls();

}


