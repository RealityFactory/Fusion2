/****************************************************************************************/
/*  about.cpp                                                                           */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird                                                */
/*  Description:  About box code                                                        */
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
 
#include "about.h"
#include "include/Ram.h"

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_about_text = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_EDIT1, m_about_text);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char * AboutText;

	AboutText = CGlobals::LoadText( IDR_TEXT1 );
	if( AboutText != NULL )
	{
		UpdateData( true );
		m_about_text = AboutText;
		geRam_Free( AboutText );
		//m_BuildInfoText = _T(BUILD_TYPE "   " BUILD_NUMBER);
		UpdateData( false );
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
