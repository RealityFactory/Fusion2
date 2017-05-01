/****************************************************************************************/
/*  TextureDlg.cpp																		*/
/*                                                                                      */
/*  Author:       Tom Morris															*/
/*  Description:  Texture selection														*/
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
/*	Genesis3D Version 1.1 released November 15, 1999				                    */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved							*/
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/


#include "stdafx.h"
#include "Globals.h"
#include "GEditPro.h"
#include "TextureDlg.h"
#include "WadFile.h"
#include "ChildFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define BM_WIDTH	256
#define BM_HEIGHT	256



/////////////////////////////////////////////////////////////////////////////
// CTextureDlg dialog

CTextureDlg::CTextureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextureDlg::IDD, pParent)
{

	m_currentTexture = "";

	//{{AFX_DATA_INIT(CTextureDlg)
	//}}AFX_DATA_INIT
}


void CTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextureDlg)
	DDX_Control(pDX, IDC_TXT_TEXTURENAME, m_textureNameText);
	DDX_Control(pDX, IDC_SIZE_TXT, m_sizeText);
//	DDX_Control(pDX, IDC_TEXTURE_BMP, m_textureBmp);
//	DDX_Control(pDX, IDC_TEXTURE_LIST, m_texture_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextureDlg, CDialog)
	//{{AFX_MSG_MAP(CTextureDlg)
	ON_BN_CLICKED(IDC_TEXTURE_BTN, OnTextureBtn)
	ON_LBN_DBLCLK(IDC_TEXTURE_LISTBOX, OnDblclkTextureListbox)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CTextureDlg message handlers

BOOL CTextureDlg::OnInitDialog() 
{
	CGlobals::PositionDialogUnderTabs(this);
//	MessageBox("TextureDlg Initializing");
	
	m_textureListBox.SubclassDlgItem(IDC_TEXTURE_LISTBOX, this);	

	CDialog::OnInitDialog();
	
	return TRUE;
}


/*
static void TextureFace(Face *pFace, int SelId, char const *Name)
{
	Face_SetTextureDibId (pFace, SelId);
	Face_SetTextureName (pFace, Name);
}

static void TextureBrushList(BrushList *pList, int SelId, char const *Name);

static void TextureBrush(Brush *pBrush, int SelId, char const *Name)
{
	int j;

	assert(pBrush);
	
	if(Brush_IsMulti(pBrush))
	{
		TextureBrushList((BrushList *)Brush_GetBrushList(pBrush), SelId, Name);
	}
	else
	{
		for (j = 0; j < Brush_GetNumFaces (pBrush); ++j)
		{
			Face *pFace;

			pFace = Brush_GetFace (pBrush, j);
			TextureFace (pFace, SelId, Name);
		}
	}
}




static void TextureBrushList(BrushList *pList, int SelId, char const *Name)
{
	Brush *b;
	BrushIterator bi;

	assert(pList);
	assert(Name);

#pragma message ("Change this and the function above to use BrushList_EnumAll")
	for(b=BrushList_GetFirst(pList, &bi);b;b=BrushList_GetNext(&bi))
	{
		TextureBrush(b, SelId, Name);
	}	
}

*/


void CTextureDlg::OnTextureBtn() 
{
	m_textureListBox.ApplyTexture();	//	defer to listbox class

	return;
}


//	post 0.5 release
//	greatly simplified to support shift-click texture selection in textured view
void CTextureDlg::SelectTexture(int SelNum)
{
	CGEditProDoc		*pDoc = CGlobals::GetActiveDocument();
				//	sets command panel tab to Textures, so we can confirm the texture we
				//	just selected by shift-clicking in the textured viewport
	if (pDoc->m_pMainFrame->m_CommandDialogBar)
	{
		CTabCtrl*	pCommandPanelTabCtrl = 
		(CTabCtrl*)m_pDoc->m_pMainFrame->m_CommandDialogBar.GetDlgItem(IDC_COMMAND_TAB);
		pCommandPanelTabCtrl->SetCurSel((MAINFRM_COMMANDPANEL_TAB)MAINFRM_COMMANDPANEL_TEXTURES);
		pDoc->m_pMainFrame->SetCommandPanelTab(MAINFRM_COMMANDPANEL_TEXTURES);
	}
	m_textureListBox.SetCurSel(SelNum);		//	set item to texture on shift-clicked brush
	m_textureListBox.SetTopIndex(SelNum);	//	bring selected item to top of list
	m_textureListBox.SetFocus();			//	set focus, so the texture gets highlighted
}



bool CTextureDlg::InitializeTextures(CGEditProDoc *pDoc)
{
	CWadFile		*pWad = NULL;
	CString			Name, FirstName;
	WadFileEntry	*BitmapPtr = NULL;
	
									//	check for a valid *txl file
	DWORD	fileAttribs = GetFileAttributes(pDoc->GetLevel()->m_pWadPath);
	if (fileAttribs == 0xFFFFFFFF)	//	 if no such path or file
	{
		CString noDirError = _T("This *.3dt file points to a *.txl file that cannot be found.\n");
		noDirError += _T("Please indicate the correct path location.");
		
		AfxMessageBox(noDirError);
		pDoc->OpenLevelOptionsDialog();

									//	ask again if they didn't spec one above
		fileAttribs = GetFileAttributes(pDoc->GetLevel()->m_pWadPath);
		if (fileAttribs == 0xFFFFFFFF)	//	 if no such path or file
		{
			CString wotsamattaUError = _T("This is your last warning!. GEditPro WILL CRASH unless you\n");
			wotsamattaUError += _T("indicate the correct path location for a valid *.txl file.");
			
			AfxMessageBox(wotsamattaUError);
			pDoc->OpenLevelOptionsDialog();
			//return true;
		}
	}
			//	reset to listbox to bring it inline with new *.txl
	m_textureListBox.ResetContent();

	pWad = pDoc->m_pLevelMgr->GetWadFile (pDoc->GetLevel());
	if (pWad)
	{
	Name = pWad->mBitmaps[0].Name;
	BitmapPtr = pDoc->GetDibBitmap( Name );
	if (BitmapPtr)
	{
	m_textureListBox.m_currentTextureName = BitmapPtr->Name;
	m_textureListBox.m_bitmapHeight = BitmapPtr->Height;
	m_textureListBox.m_bitmapWidth = BitmapPtr->Width;
	m_textureListBox.LoadList(pDoc);
	}
	}
	
	return true;
	
}

bool CTextureDlg::UpdateTextures( CGEditProDoc* pDoc )
{
	CString		Name, size;
	CString		FirstName;
	static		CString oldWadPath;
	int			height, width;



	//	if it's the same old doc and if the textures haven't changed
	//	AND if the current WadPath hasn't changed
	if ((pDoc) && 
		(m_TxlibChanged == false) &&
		(pDoc->GetLevel()->m_pWadPath == oldWadPath))
	{
		m_pDoc = pDoc;	// set the old doc to the new doc	
		m_currentTexture = m_textureListBox.m_currentTextureName;
		height = m_textureListBox.m_bitmapHeight;
		width = m_textureListBox.m_bitmapWidth;
		size.Format( "%d X %d", width, height );
		m_sizeText.SetWindowText(size);
		m_textureNameText.SetWindowText(m_currentTexture);

	}
	//	if the WadPath is different...
	else if ((pDoc) && pDoc->GetLevel()->m_pWadPath != oldWadPath)/*(m_pDoc != pDoc))*/ 
	{	
		m_pDoc = pDoc;	// set the old doc to the new doc
	
		oldWadPath = m_pDoc->GetLevel()->m_pWadPath;	//	update oldWadPath
		
		if (m_pDoc)
		{
			InitializeTextures(m_pDoc);	//	setup the texture list
			m_currentTexture = m_textureListBox.m_currentTextureName;
			height = m_textureListBox.m_bitmapHeight;
			width = m_textureListBox.m_bitmapWidth;
			size.Format( "%d X %d", width, height );
			m_sizeText.SetWindowText(size);
			m_textureNameText.SetWindowText(m_currentTexture);
		}
	}
	
	else
	{
		//	Documents all closed.  Update any values that need to be...
		m_pDoc = NULL;
	}
	
	m_TxlibChanged = false;
	
	
	/*
	//	Restore focus to active view
	CMDIChildWnd *pMDIChild	=(CMDIChildWnd *)pDoc->m_pMainFrame->MDIGetActive();
	if(pMDIChild)
	{
	CView	*cv	=(CView *)pMDIChild->GetActiveView();
	if( cv )
	cv->SetFocus() ;
	}
	
	*/

	return true;
}




const char *CTextureDlg::GetCurrentTexture()
//CString	*CTextureDlg::GetCurrentTexture()
{
	return	(LPCSTR)m_currentTexture;
}




void CTextureDlg::OnDblclkTextureListbox() 
{
	m_textureListBox.ApplyTexture();
}

			//	if the user hits the <Escape> key, ignore it.
void CTextureDlg::OnCancel()
{
	return;
}
			//	post 0.5 release
			//	if the user hits the <Enter> key, select and/or apply the current
			//	texture to the currently selected brush / face.
void CTextureDlg::OnOK()
{
	OnTextureBtn();
}


