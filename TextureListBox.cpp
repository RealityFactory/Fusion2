/****************************************************************************************/
/*  TextureListBox.h                                                                    */
/*                                                                                      */
/*  Author:       Tom Morris                                                            */
/*  Description:  Helper class that controls the textures list box                      */
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

// See TextureListBox.h for details on how to use this class
// This class is used by the texture listbox in CTextureDlg.cpp

#include "stdafx.h"
#include "Globals.h"
#include "TextureListBox.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextureListBox

CTextureListBox::CTextureListBox()
{
	// You can't actually add anything to the list box here since the
	// associated CWnd hasn't been created yet.  Save any initialization
	// you need to do for after the CWnd has been constructed.  The TODO:
	// comments in TextureListBox.h explain how to do this for
	// CTextureListBox as implemented here.

}

CTextureListBox::~CTextureListBox()
{

}


BEGIN_MESSAGE_MAP(CTextureListBox, CListBox)
	//{{AFX_MSG_MAP(CTextureListBox)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextureListBox message handlers


void CTextureListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CWadFile		*pWad = NULL;
	CGEditProDoc *pDoc = CGlobals::GetActiveDocument();
	CString			Name = _T("");


	if (!pDoc)
	{
		return;
	}

	if (pDoc)
	{

		if (pDoc->GetLevel() == NULL)
			return;

		pWad = pDoc->m_pLevelMgr->GetWadFile (pDoc->GetLevel());

		if (pWad == NULL)
		{
			return;
		}

		 Name = pWad->mBitmaps[lpDrawItemStruct->itemID].Name;

		if (Name == _T(""))
		{
			return;
		}


		int	bmHeight, bmWidth;

		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

//		BitmapPtr = (WadFileEntry*)lpDrawItemStruct->itemData;
		BitmapPtr = pDoc->GetDibBitmap( Name );
		if (BitmapPtr->Width <= 0 || BitmapPtr->Height <= 0 ||	BitmapPtr->BitsPtr <= 0)
		{
				pWad = pDoc->m_pLevelMgr->GetWadFile (pDoc->GetLevel());

				Name = pWad->mBitmaps[lpDrawItemStruct->itemID].Name;

				BitmapPtr = pDoc->GetDibBitmap( Name );

				if (BitmapPtr->Width <= 0 || BitmapPtr->Height <= 0 ||	BitmapPtr->BitsPtr <= 0)
				{
					return;
				}
		}

		// White space
		CRect rcItem(lpDrawItemStruct->rcItem);
		//	rcItem.InflateRect(-1, -1);

		// new g3dc. display all texture bitmaps at 128 x 128 resoulution
		if (BitmapPtr->Height > 128 || BitmapPtr->Height < 128)
			bmHeight = 128;
		else
			bmHeight = BitmapPtr->Height;

		if (BitmapPtr->Width >128 || BitmapPtr->Height < 128)
			bmWidth = 128;
		else
			bmWidth = BitmapPtr->Width;

		// Create a BITMAPINFOHEADER structure for blitting...
		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof (BITMAPINFOHEADER);
		bmih.biWidth = BitmapPtr->Width;
		bmih.biHeight = -BitmapPtr->Height;
		bmih.biPlanes = 1;
		bmih.biBitCount = 16;
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		if (StretchDIBits(
			(HDC)*pDC,
			rcItem.left,					//	x var for upper left corner of dest
			rcItem.top,					//	y var for uper left corner of dest
			bmWidth,			//	new for g3dc
			bmHeight,			//	new for g3dc
			0,
			0,
			BitmapPtr->Width,
			BitmapPtr->Height,
			BitmapPtr->BitsPtr,
			(BITMAPINFO *) &bmih,
			DIB_RGB_COLORS,
			SRCCOPY ) == GDI_ERROR )
		{
			TRACE("Could not blit to screen!");
			_ASSERT(0);
		}

		// Focus rect
		if (lpDrawItemStruct->itemAction & ODA_FOCUS)
		{
			pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);

			BitmapPtr = pDoc->GetDibBitmap( Name );

			m_currentTextureName = BitmapPtr->Name;
			m_bitmapHeight = BitmapPtr->Height;
			m_bitmapWidth = BitmapPtr->Width;
												//	UpdateTextures
			pDoc->m_pMainFrame->m_TexturesTab.UpdateTextures(pDoc);
		}
	}
}

// This function compares two colors to each other. If you're doing
// something different, like comparing two bitmaps to each other, use
// a different algorithm.  If you are working with strings, the default
// implementation provided by MFC will probably do the job just fine.
// (Example of a string where you need to do your own comparisons: if
// your sorting scheme is different than a standard collating sequence,
// such as one where the comparisons need to be case-insensitive.)
/*
int CTextureListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	// return -1 = item 1 sorts before item 2
	// return 0 = item 1 and item 2 sort the same
	// return 1 = item 1 sorts after item 2

	WadFileEntry	*BitmapPtr1 = (WadFileEntry*)lpCompareItemStruct->itemData1;
	WadFileEntry	*BitmapPtr2 = (WadFileEntry*)lpCompareItemStruct->itemData2;

	if (BitmapPtr1->Name == BitmapPtr2->Name)
		return 0;
	if (BitmapPtr1->Name[0] >  BitmapPtr2->Name[0])
		return 1;
	else
		return -1;

/*
	COLORREF color1 = (COLORREF)lpCompareItemStruct->itemData1;
	COLORREF color2 = (COLORREF)lpCompareItemStruct->itemData2;
	if (color1 == color2)
		return 0;       // exact match

	// first do an intensity sort, lower intensities go first
	int intensity1 = GetRValue(color1) + GetGValue(color1) + GetBValue(color1);
	int intensity2 = GetRValue(color2) + GetGValue(color2) + GetBValue(color2);
	if (intensity1 < intensity2)
		return -1;      // lower intensity goes first
	else if (intensity1 > intensity2)
		return 1;       // higher intensity goes second

	// if same intensity, sort by color (blues first, reds last)
	if (GetBValue(color1) > GetBValue(color2))
		return -1;
	else if (GetGValue(color1) > GetGValue(color2))
		return -1;
	else if (GetRValue(color1) > GetRValue(color2))
		return -1;
	else
		return 1;

}
*/

#define COLOR_ITEM_HEIGHT   128

// We made our color bars all be the same height for simplicity.  You
// can actually specify variable heights as long as you set the
// CBS_TextureVARIABLE style.
void CTextureListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// all items are of fixed size
	// must use LBS_TextureVARIABLE for this to work
	lpMeasureItemStruct->itemHeight = COLOR_ITEM_HEIGHT;
}



void CTextureListBox::AddListItem(WadFileEntry	*BitmapPtr)
{
	// add a listbox item
//	this->AddString((LPCTSTR) BitmapPtr);		//	commented out post 0.5 release
	this->AddString((LPCTSTR) BitmapPtr->Name);	//	post 0.5 release
}



void CTextureListBox::LoadList(CGEditProDoc *pDoc)
{
	CWadFile		*pWad = NULL;
	CString			Name;

	pDoc->m_pLevelMgr->GetWadPath (pDoc->GetLevel());
					//	get the wad file that contains the texture bitmaps

	pWad = pDoc->m_pLevelMgr->GetWadFile (pDoc->GetLevel());

						// fill list box with texture names...
	for (int index = 0; index < pWad->mBitmapCount; index++)
	{
		Name = pWad->mBitmaps[index].Name;

		BitmapPtr = pDoc->GetDibBitmap( Name );

		if (BitmapPtr->Width == 0 || BitmapPtr->Height == 0 ||	BitmapPtr->BitsPtr == 0)
			return;

		AddListItem(BitmapPtr);

			// Listbox is sorted, so we need to put the wad file index into the item data.
		/*		int lbIndex = m_texture_list.AddString( (LPCTSTR) Name );
		if (lbIndex != LB_ERR)
		{
		m_texture_list.SetItemData (lbIndex, index);
		}
		*/
	}
}

// changed QD 12/03
static void TextureFace(Face *pFace, int SelId, char const *Name, WadFileEntry* pbmp)
{
	Face_SetTextureDibId (pFace, SelId);
	Face_SetTextureName (pFace, Name);
// changed QD 12/03
	Face_SetTextureSize (pFace, pbmp->Width, pbmp->Height);
// end change
}

// changed QD 12/03
static void TextureBrushList(BrushList *pList, int SelId, char const *Name, WadFileEntry* pbmp);

// changed QD 12/03
static void TextureBrush(Brush *pBrush, int SelId, char const *Name, WadFileEntry* pbmp)
{
	int j;

	assert(pBrush);

	if(Brush_IsMulti(pBrush))
	{
		// changed QD 12/03
		TextureBrushList((BrushList *)Brush_GetBrushList(pBrush), SelId, Name, pbmp);
	}
	else
	{
		for (j = 0; j < Brush_GetNumFaces (pBrush); ++j)
		{
			Face *pFace;

			pFace = Brush_GetFace (pBrush, j);
			TextureFace (pFace, SelId, Name, pbmp); // changed QD 12/03
		}
	}
}



// changed QD 12/03
static void TextureBrushList(BrushList *pList, int SelId, char const *Name, WadFileEntry* pbmp)
{
	Brush *b;
	BrushIterator bi;

	assert(pList);
	assert(Name);

#pragma message ("Change this and the function above to use BrushList_EnumAll")
	for(b=BrushList_GetFirst(pList, &bi);b;b=BrushList_GetNext(&bi))
	{
		TextureBrush(b, SelId, Name, pbmp);// changed QD 12/03
	}
}



void CTextureListBox::ApplyTexture()
{
	int i;
	int SelectedItem	= GetCurSel();
	int  SelId = SelectedItem;
	CGEditProDoc		*m_pDoc = NULL;
	m_pDoc = CGlobals::GetActiveDocument();

	if (m_pDoc)
	{
		if(m_pDoc->m_iModeTool==ID_TOOLS_TEMPLATE)
			return;

		switch (m_pDoc->m_AdjustMode)
		{
		case ADJUST_MODE_FACE :
			{
				int Size;

				Size = SelFaceList_GetSize (m_pDoc->DocVarsGetPointer()->m_pSelFaces);
				for (i = 0; i < Size; ++i)
				{
					Face *pFace = NULL;
					pFace = SelFaceList_GetFace (m_pDoc->DocVarsGetPointer()->m_pSelFaces, i);

					if (pFace)
					{
						// changed QD 12/03
						WadFileEntry* BitmapPtr = m_pDoc->GetDibBitmap( m_currentTextureName );
						::TextureFace (pFace, SelId, (LPCSTR)m_currentTextureName, BitmapPtr);
					}
				}
				// have to go through the selected brushes and update their child faces
				int NumSelBrushes = SelBrushList_GetSize (m_pDoc->DocVarsGetPointer()->m_pSelBrushes);
				for (i = 0; i < NumSelBrushes; ++i)
				{
					Brush *pBrush = NULL;

					pBrush = SelBrushList_GetBrush (m_pDoc->DocVarsGetPointer()->m_pSelBrushes, i);
					if (pBrush)
					{
						Brush_UpdateChildFaces (pBrush);
					}
				}
				break;
			}

		case ADJUST_MODE_BRUSH :
			{
				if(m_pDoc->GetSelState() & MULTIBRUSH)
				{
					int NumSelBrushes = SelBrushList_GetSize (m_pDoc->DocVarsGetPointer()->m_pSelBrushes);
					for (i = 0; i < NumSelBrushes; ++i)
					{
						Brush *pBrush = NULL;
						pBrush = SelBrushList_GetBrush (m_pDoc->DocVarsGetPointer()->m_pSelBrushes, i);
						if (pBrush)
						{
							// changed QD 12/03
							WadFileEntry* BitmapPtr = m_pDoc->GetDibBitmap( m_currentTextureName);
							::TextureBrush (pBrush, SelId, (LPCSTR)m_currentTextureName, BitmapPtr);
							// end change
							Brush_UpdateChildFaces (pBrush);
						}
					}
				}
				else
				{
					// changed QD 12/03
					WadFileEntry* BitmapPtr = m_pDoc->GetDibBitmap( m_currentTextureName);
					::TextureBrush (m_pDoc->m_pCurBrush, SelId, (LPCSTR)m_currentTextureName, BitmapPtr);
					// end change
					Brush_UpdateChildFaces (m_pDoc->m_pCurBrush);
				}
				break;
			}

		default :
			return;
		}

		if ((!m_pDoc->GetSelState() & ANYBRUSH) || (m_pDoc->GetSelState() & ANYFACE))
		{	//	if face is selected and brush is not
			//	absolutely necessary in order to conserve brush and
			//	face dialog information
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			m_pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );
		}
		else
		{	//	if brush is selected but face is not
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			//	Make sure REBUILD_QUICK is absolurelty necessary.
			m_pDoc->UpdateAllViews( UAV_ALL3DVIEWS | REBUILD_QUICK, NULL );
		}


		if (!CGlobals::g_bRebuildAlways)
		{
			m_pDoc->m_pMainFrame->SendMessageToDescendants(WM_COMMAND, IDC_BTN_REBUILD_COMMAND);
			CGlobals::g_iRebuildClickCount = 0;
		}
	}
}

