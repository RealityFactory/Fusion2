/****************************************************************************************/
/*  TextureDlg.h																		*/
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

#if !defined(AFX_TEXTUREDLG_H__F5338FA2_A111_11D4_9554_8AC95CEE0727__INCLUDED_)
#define AFX_TEXTUREDLG_H__F5338FA2_A111_11D4_9554_8AC95CEE0727__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextureDlg.h : header file
//

#include "brush.h"
#include "TextureListBox.h"
class CGEditProDoc;

/////////////////////////////////////////////////////////////////////////////
// CTextureDlg dialog

class CTextureDlg : public CDialog
{
// Construction
public:
	CTextureDlg(CWnd* pParent = NULL);   // standard constructor
	const char			*GetCurrentTexture();
//	CString				*GetCurrentTexture();
	CString				m_currentTexture;
//	CtEditTabControls	*m_pParentCtrl;
	bool m_TxlibChanged;
	void SelectTexture(int SelNum);
	bool UpdateTextures( CGEditProDoc* pDoc );
	bool InitializeTextures(CGEditProDoc *pDoc);

	CTextureListBox		m_textureListBox;

// Dialog Data
	//{{AFX_DATA(CTextureDlg)
	enum { IDD = IDD_TEXTURE_DLG };
	CStatic				m_textureNameText;
	CStatic				m_sizeText;
//	CStatic				m_textureBmp;
//	CListBox			m_texture_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureDlg)
	public:
//	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
	public:
		BOOL	SetInitialTexture(	CGEditProDoc *pDoc);
protected:
		CGEditProDoc* m_pDoc;


	// Generated message map functions
	//{{AFX_MSG(CTextureDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTextureBtn();
	afx_msg void OnDblclkTextureListbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//void UpdateBitmap();

//void ClearTextureImage(CDC* pDC);
//void UpdateSize();



//const char *GetCurrentTexture();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTUREDLG_H__F5338FA2_A111_11D4_9554_8AC95CEE0727__INCLUDED_)
