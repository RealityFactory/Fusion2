/****************************************************************************************/
/*  LevelOptions.cpp                                                                    */
/*                                                                                      */
/*  Author:       Jim Mischel                                                           */
/*  Description:  Saves settings for levels                                             */
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
#include "GEditPro.h"
#include "LevelOptions.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLevelOptions dialog


CLevelOptions::CLevelOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CLevelOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLevelOptions)
	m_DrawScale = 0.0f;
	m_LightmapScale = 0.0f;
	m_TextureLib = _T("");
	m_TxlChanged = false;
	m_HeadersDir = _T("");
	m_HeadersDir.GetBufferSetLength(MAX_PATH*2);	//	post 0.58
	m_tempHeadersDir.GetBufferSetLength(MAX_PATH*2);	//	post 0.58
	m_ScaleWorld = 1.0f;
	m_CurrentScale = 0.0f;
	m_SnapDegrees = 0.0;
	m_UseSnap = FALSE;
	MetricOrTexelSnap = -1;
	MetricOrTexelGrid = -1;
	m_GridUnits = -1;
	//}}AFX_DATA_INIT
}


void CLevelOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLevelOptions)
	DDX_Text(pDX, IDC_DRAWSCALE, m_DrawScale);
	DDX_Text(pDX, IDC_LIGHTMAPSCALE, m_LightmapScale);
	DDX_Text(pDX, IDC_EDITTXL, m_TextureLib);
	DDX_Control(pDX, IDC_PATH_LIST, m_ListBoxEx);
	DDX_Text(pDX, IDC_SCALE_WORLD, m_ScaleWorld);
	DDX_Text(pDX, IDC_CURRENT_SCALE, m_CurrentScale);
			DDX_Text(pDX, IDC_ROTSNAPDEGREES, m_SnapDegrees);
	DDV_MinMaxDouble(pDX, m_SnapDegrees, 0., 90.);
	DDX_Check(pDX, IDC_USEROTSNAP, m_UseSnap);
	DDX_Radio(pDX, IDC_TEXEL1_RADIO, m_GridUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLevelOptions, CDialog)
	//{{AFX_MSG_MAP(CLevelOptions)
	ON_BN_CLICKED(IDC_BROWSETXL, OnBrowsetxl)
	ON_EN_CHANGE(IDC_EDITTXL, OnChangeEdittxl)
	ON_EN_CHANGE(IDC_EDITHEADERSDIR, OnChangeEditheadersdir)
	ON_EN_KILLFOCUS(IDC_DRAWSCALE, OnKillfocusDrawscale)
	ON_EN_KILLFOCUS(IDC_LIGHTMAPSCALE, OnKillfocusLightmapscale)
	ON_EN_KILLFOCUS(IDC_SCALE_WORLD, OnKillfocusScaleWorld)
			ON_BN_CLICKED(IDC_SNAP15, OnSnap15)
	ON_BN_CLICKED(IDC_SNAP30, OnSnap30)
	ON_BN_CLICKED(IDC_SNAP45, OnSnap45)
	ON_BN_CLICKED(IDC_SNAP60, OnSnap60)
	ON_BN_CLICKED(IDC_USEROTSNAP, OnUsertosnap)
	ON_BN_CLICKED(IDC_SNAP90, OnSnap90)
	ON_BN_CLICKED(IDC_TEXEL1_RADIO, OnTexel1Radio)
	ON_BN_CLICKED(IDC_TEXEL2_RADIO, OnTexel2Radio)
	ON_BN_CLICKED(IDC_TEXEL4_RADIO, OnTexel4Radio)
	ON_BN_CLICKED(IDC_TEXEL8_RADIO, OnTexel8Radio)
	ON_BN_CLICKED(IDC_TEXEL16_RADIO, OnTexel16Radio)
	ON_BN_CLICKED(IDC_TEXEL32_RADIO, OnTexel32Radio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLevelOptions message handlers


static UINT snapdisable[] =
{
	IDC_SNAP15,         
	IDC_SNAP30,
	IDC_SNAP45,
	IDC_SNAP60,
	IDC_SNAP90,
	IDC_ROTSNAPDEGREES,
	IDC_SPINROTSNAPDEGREES
};


BOOL CLevelOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_pDoc = CGlobals::GetActiveDocument();
	
						//	get stored level options
	m_DrawScale = m_pDoc->m_pLevelMgr->GetDrawScale (m_pDoc->GetLevel());
	m_LightmapScale = m_pDoc->m_pLevelMgr->GetLightmapScale (m_pDoc->GetLevel());
	m_TextureLib = m_pDoc->m_pLevelMgr->GetWadPath (m_pDoc->GetLevel());
						//	get the headers dir info
	m_HeadersDir = m_pDoc->m_pLevelMgr->GetHeadersDirectory (m_pDoc->GetLevel());
	m_CurrentScale	= m_pDoc->GetWorldScaleFactor();

	int nfirst = 0;		//	set the start for string copying
	CString copyString;	//	for lifting out individual paths
	CString delimitString = ";";	//	our delimiter
	
						//	start searching headersDir string
						//	for our delimitString
	for (int i = 0; i < m_HeadersDir.GetLength(); i++)
	{
						//	get each char in the string, one-at-a-time
		TCHAR oneChar = m_HeadersDir.GetAt(i);	

						//	convert the char into a string so we can use
						//	it for doing comparisons later
		CString oneCharString = oneChar;

						//	see if this char is the one we are looking for
						//	If it IS, lift out the path and insert it
						//	on a new line in the list box
		if (oneCharString == delimitString)
		{
						//	extract a section of the string
			copyString = m_HeadersDir.Mid(nfirst, (i-nfirst));

						//	insert this string into the list box
			m_ListBoxEx.InsertString( -1, copyString);

						//	set nfirst to i
			nfirst = i + 1;
		}
	}

						//	if no delimitString(;) was found, but our
						//	headers dir is not zero, it's probably a valid
						//	path but without the ending delimitString.
						//	So, put it in there.
	if ((copyString.GetLength() == 0) &&  (m_HeadersDir.GetLength() !=0))
		m_ListBoxEx.InsertString(-1, m_HeadersDir);


						//	Add the listbox buddy. This is our button bar
						//	at the top of the headers Dir listbox.
         m_ListBoxExBuddy.SubclassDlgItem( IDC_LISTBUDDY, this );
         m_ListBoxExBuddy.SetListbox( &m_ListBoxEx );	


//	for texel and grid initialization
	CWnd *	pWnd = NULL;
	CSpinButtonCtrl	* pSpin  = NULL;
	CDialog::OnInitDialog();
	

	for( i=0; i< sizeof( snapdisable ) / sizeof( UINT); i++ )
	{
		pWnd = GetDlgItem( snapdisable[i] ) ;
		pWnd->EnableWindow( m_UseSnap ) ;
	}
	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPINROTSNAPDEGREES ) ;
	pSpin->SetRange( 0, 90 ) ;			 

	 UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CLevelOptions::OnBrowsetxl() 
{
	CFileDialog FileDlg (TRUE,
						"txl",
						m_TextureLib,
						OFN_FILEMUSTEXIST,
						"Texture Library Files (*.txl)|*.txl||");

	FileDlg.m_ofn.lpstrTitle	="GEditPro Texture Finder";

	if (FileDlg.DoModal () == IDOK)
	{
		m_TxlChanged = true;
		m_TextureLib = FileDlg.GetPathName ();
		UpdateData (FALSE);
	}
}


void CLevelOptions::OnChangeEdittxl() 
{
	UpdateData (TRUE);	
	m_TxlChanged = true;
}


//	obsolete gedit function
void CLevelOptions::OnChangeEditheadersdir() 
{
	m_HeadersChanged = true;
}

void CLevelOptions::OnOK() 
{
	CString		retreivalString = "";
	m_tempHeadersDir = "";
	TCHAR		lastChar;		//	the last char of each line of the headers listbox
	CString		lastCharString;	//	needed so we can do CString comparisons

						//	get the number of items in the headers listbox
	int count = m_ListBoxEx.GetCount();

						//	start processing each line
	for (int i = 0; i < count; i++)
	{
						//	get the whole string for this line
		m_ListBoxEx.GetText(i, retreivalString);

						//	get the last character of this string
		lastChar = retreivalString.GetAt(retreivalString.GetLength() - 1);

						//	convert this char to a CString so we can compare later
		lastCharString = lastChar;

						//	now we compare
						//	if the last char is not a ";", then we need to add one
		if (lastCharString != ";")
			retreivalString += _T(";");

						//	now add this string to our accumulation string
		m_tempHeadersDir += retreivalString;
	}

						//	compare our accumulated string to the original
						//	m_HeadersDir string. If they are identical,
						//	then let's forget it...
	if (m_HeadersDir == m_tempHeadersDir)
	{	
		UpdateData(FALSE);
		CDialog::OnOK();
		return;	//	post 058
	}
						//	Otherwise, set the m_HeadersDir string to our new string
						//	and let the document know that it's new and different
	else
	{
		if (m_tempHeadersDir.GetLength() >=230)	//	post 058
		{
			CString message = _T("The current headers string exceeds the maximum string size.\n\n");
			message += _T("Consolidate your headers into fewer directories, or arrange their\n");
			message += _T("paths on your system so their path names require fewer characters.");

			AfxMessageBox(message, MB_ICONEXCLAMATION|MB_OK|MB_DEFBUTTON1);
			UpdateData(FALSE);
			return;
		}

		m_HeadersDir = m_tempHeadersDir;
		m_HeadersChanged = true;
	}

	UpdateData(FALSE);
	CDialog::OnOK();
}

// post 0.57 - added handling of level-wide draw scale specs
void CLevelOptions::OnKillfocusDrawscale() 
{
	float	lastValue = m_DrawScale;

	UpdateData(TRUE);	// set the m_DrawScale variable to the value in the editbox

	if (m_DrawScale == NULL)
	{
		m_DrawScale = lastValue;
		UpdateData(FALSE);	//	set the edit box to this value
		return;
	}

	if ((m_DrawScale >= 0) && (m_DrawScale <= 10000)) 
	{
		m_pDoc->m_pLevelMgr->SetDrawScale (m_pDoc->GetLevel(), m_DrawScale);
	}
	else
	{
		m_DrawScale = lastValue;
		UpdateData(FALSE);	//	set the edit box to this value
		AfxMessageBox("Value must be >0 and <= 10000");
	}	
}


// post 0.57 - added handling of level-wide lightmap scale specs
void CLevelOptions::OnKillfocusLightmapscale() 
{
	float	lastValue = m_LightmapScale;

	UpdateData(TRUE);	// set the m_DrawScale variable to the value in the editbox

	if (m_LightmapScale == NULL)
	{
		m_LightmapScale = lastValue;
		UpdateData(FALSE);	//	set the edit box to this value
		return;
	}

	if ((m_LightmapScale >= 0) && (m_DrawScale <= 10000)) 
	{
		m_pDoc->m_pLevelMgr->SetLightmapScale(m_pDoc->GetLevel(), m_LightmapScale);
	}
	else
	{
		m_LightmapScale = lastValue;
		UpdateData(FALSE);	//	set the edit box to this value
		AfxMessageBox("Value must be >0 and <= 10000");
	}	
	
}

void CLevelOptions::OnKillfocusScaleWorld() //	post 0.57
{
	float	lastValue = m_ScaleWorld;

	UpdateData(TRUE);	// set the m_ScaleWorld variable to the value in the editbox

	if (m_ScaleWorld == NULL)
	{
		m_ScaleWorld = lastValue;
		UpdateData(FALSE);	//	set the edit box to this value
		return;
	}

	if (m_ScaleWorld > 0.0f)
	{
		if (m_ScaleWorld * m_CurrentScale >10)
		{
			CString	message = _T("CAUTION!\n\n");
			message += _T("Worlds of large scales can cause the editor and\n");
			message += _T("Genesis3D to become unstable, and could cause\n");
			message += _T("either or both to crash.\n\n");
			message += _T("We recommend that you SAVE YOUR WORK\n");
			message += _T("before experimenting with this feature.\n\n");
			message += _T("o Choose CANCEL to cancel this scaling operation.\n");
			message += _T("o Choose OK to proceed with the new scaling factor.");

			int RESULT;
			RESULT = AfxMessageBox(message, MB_OKCANCEL | MB_ICONEXCLAMATION);
			if (RESULT == IDCANCEL)
			{
				m_ScaleWorld = 1.0f;
				UpdateData(FALSE);	//	set the edit box to this value
				return;
			}
		}
		m_pDoc->ScaleWorld(m_ScaleWorld);	
		m_pDoc->UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL);
		m_CurrentScale	= m_pDoc->GetWorldScaleFactor();
		m_ScaleWorld = 1.0f;
		UpdateData(FALSE);	//	set the edit box to this value
	}
	else
	{
		m_ScaleWorld = lastValue;
		UpdateData(FALSE);	//	set the edit box to this value
		AfxMessageBox("Value must be > 0.0");
	}	
}



void CLevelOptions::OnSnap15() 
{
	UpdateData();
	m_SnapDegrees = 15.0;
	UpdateData(FALSE);
}

void CLevelOptions::OnSnap30() 
{
	UpdateData();
	m_SnapDegrees = 30.0;
	UpdateData(FALSE);
}

void CLevelOptions::OnSnap45() 
{
	UpdateData();
	m_SnapDegrees = 45.0;
	UpdateData(FALSE);
}

void CLevelOptions::OnSnap60() 
{
	UpdateData();
	m_SnapDegrees = 60.0;
	UpdateData(FALSE);
}

void CLevelOptions::OnSnap90() 
{
	UpdateData();
	m_SnapDegrees = 90.0;
	UpdateData(FALSE);
}

void CLevelOptions::OnUsertosnap() 
{
	int i ;

	m_UseSnap = !m_UseSnap ;
	for( i=0; i< sizeof( snapdisable ) / sizeof( UINT); i++ )
	{
		CWnd *	pWnd = NULL;
		pWnd = GetDlgItem( snapdisable[i] );
		if (pWnd)
			pWnd->EnableWindow( m_UseSnap ) ;
	}
}


void CLevelOptions::OnTexel1Radio() 
{
	m_GridUnits = 0;
}

void CLevelOptions::OnTexel2Radio() 
{
	m_GridUnits = 1;
}

void CLevelOptions::OnTexel4Radio() 
{
	m_GridUnits = 2;
	
}

void CLevelOptions::OnTexel8Radio() 
{
	m_GridUnits = 3;
}

void CLevelOptions::OnTexel16Radio() 
{
	m_GridUnits = 4;
}

void CLevelOptions::OnTexel32Radio() 
{
	m_GridUnits = 5;
}
