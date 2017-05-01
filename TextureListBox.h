/****************************************************************************************/
/*  TextureListBox.h                                                                           */
/*                                                                                      */
/*  Author:       Tom Morris						                                    */
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
/*  Genesis3D Version 1.1 released November 15, 1999                                 */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/

#ifndef __TEXTURELISTBOX_H__
#define __TEXTURELISTBOX_H__

#include "WadFile.h"
class tEditDoc;
//#include "GEditProDoc.h"

// This class handles an owner-draw list box.  The list box in question
// handles color bars instead of text.  It shows all of the required
// features for an owner-draw list box. See the on-line help for CListBox 
// for other ways you can manipulate the contents of an owner-draw list
// box.
//
// You can actually handle just about anything you want in an owner-draw 
// list box as long as you override functions CompareItem, DrawItem, and 
// MeasureItem.  Strings are easiest and don't require all the overrides.  
// Other examples: bitmaps, text in varying fonts, text in a non-standard 
// font, etc.
//
// TODO:
//	1.	Create an owner-draw list box in your resources to use with
//		this class.  You will need to have the following features set
//		on the Styles page for the list box:
//		- Owner-Draw: Variable
//		- Has Strings: Unchecked
//		- Sort: Checked (to show off owner-draw sorting)
//
//	2.	Subclass your owner-draw list box to associate it with this
//		class.  The following statement placed in OnInitDialog() for 
//		the dialog you used this list box in should work fine:
//			m_myDraw.SubclassDlgItem(IDC_MYDRAW, this);
//		where:
//			m_myDraw is an instance of the owner-draw list box class
//			IDC_MYDRAW is the ID associated with your list box resource
//
//	3.	Call m_myDraw.LoadList() to initialize the contents of the owner-draw
//		list box.  Do this immediately after you subclass the list box
//		control.
//
//	4.	The list box implemented here is single selection for simplicity.  
//		If you want	something different (like multiple or extended selection),
//		you will need to do the following:
//		- change the Selections type on the Styles page for the list box 
//			appropriately
//		- use the multiple-selection operations functions provided for 
//			CListBox to manipulate the selections (see the on-line help 
//			for CListBox for more details on these functions)

/////////////////////////////////////////////////////////////////////////////
// CTextureListBox window

class CTextureListBox : public CListBox
{
// Construction
public:
	CTextureListBox();

// Attributes
public:

// Operations
public:


//	void	AddListItem(COLORREF color);
	void	AddListItem(WadFileEntry	*BitmapPtr);
	void	LoadList(CGEditProDoc *pDoc);
	CString	m_currentTextureName;
	int		m_bitmapHeight, m_bitmapWidth;
	void	ApplyTexture();

	WadFileEntry	*BitmapPtr;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
//	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTextureListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTextureListBox)

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif __TEXTURELISTBOX_H__	// end define of CTextureListBox
