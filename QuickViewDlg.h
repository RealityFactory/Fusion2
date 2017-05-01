/****************************************************************************************/
/*  QuickViewDlg.h																	*/
/*                                                                                      */
/*  Author:       Tom Morris															*/
/*  Description:  QuickViewer for viewing *.bsp files with the engine					*/
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


#if !defined(AFX_QuickViewDlg_H__EE526980_C9BC_11D4_9554_B04EBC493420__INCLUDED_)
#define AFX_QuickViewDlg_H__EE526980_C9BC_11D4_9554_B04EBC493420__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuickViewDlg.h : header file
//
#include <genesis.h>
/////////////////////////////////////////////////////////////////////////////
// CQuickViewDlg dialog

class CQuickViewDlg : public CDialog
{
// Construction
public:
	CQuickViewDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuickViewDlg)
	enum { IDD = IDD_GENEDIT_PREVIEW };
	CButton	m_zeroCameraBtn;
	CSliderCtrl	m_cameraSliderZ;
	CSliderCtrl	m_cameraSliderY;
	CSliderCtrl	m_cameraSliderX;
	CStatic	m_softwareText;
	CButton	m_previewWithEngineBtn;
	int		m_cameraXPos;
	int		m_cameraYPos;
	int		m_cameraZPos;
	CString	m_strPreviewLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuickViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowPreviewBtn();
	afx_msg void OnClose();
	afx_msg void OnPreviewBrowseBtn();
	afx_msg void OnRadio640();
	afx_msg void OnRadio1024();
	afx_msg void OnRadio800();
	afx_msg void OnRadioCollide();
	afx_msg void OnRadioNocollide();
	afx_msg void OnSoftwareRadio();
	afx_msg void OnDirect3dRadio();
	afx_msg void OnKillfocusCameraX();
	afx_msg void OnKillfocusCameraY();
	afx_msg void OnKillfocusCameraZ();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI );
	afx_msg void OnPaint();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnZeroCameraBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//Ok here we go...First thing we need to do is declare some varibles.  These variables reflect the different
	//objects in Genesis.
//	HWND						hWnd;			//The window
	geEngine					*Engine;		//The engine object
	geDriver_System				*DrvSys;		//Genesis Driver System
	geDriver					*Driver;		//Your driver (DirectX, Glide, or Software)
	geDriver_Mode				*Mode;			//The mode you selected

	geSound_System				*SoundSys;		//The Genesis Sound System

	geCamera					*Camera;		//The camera object
	GE_Rect						Rect;			//For the camera object's rectangular view
	geXForm3d					XForm;			//Camera's XForm
	geVec3d						m_cameraIn, m_cameraLeft, m_cameraUp;
	geVec3d						m_cameraOldPos, m_cameraNewPos;
	geExtBox					CameraExtBox;
	geWorld						*World;			//World Object

	char						ourdriver;		//Driver we want to use
	CString						m_DriverName, driverString;
	int							Width;			//Width of the mode
	int							Height;			//Height of the mode
	int							m_iMasterRun;
	float						OldTimeGetTime;
	float						deltaTime;
	CRect						m_previewRect;
	CWnd						*m_previewWnd;
//	CString						m_previewLevelPath;
	bool						m_collisionDetection;
	CButton						*m_softwareRadioBtn, *m_direct3dRadioBtn,
								*m_640RadioBtn, *m_800RadioBtn, *m_1024RadioBtn,
								*m_collideRadioBtn, *m_noCollideRadioBtn;

	CRect						m_restoreRect;
	CRect						m_GripperRect;
	BOOL						m_bDrawGripper;

	//Here are the prototypes of some functions we are going to create
//	void		SetDriverChoice();
	void		DisableDlgControls();
	void		EnableDlgControls();
	void		RestoreDialogBox();
	geBoolean	InitEngine(HWND hWnd);
	geBoolean	LoadDriver();		//	post 0.5
	geBoolean	LoadLevel(char *FileName);
	void		Shutdown();
	void		ControlCamera();
	void		MoveCamera(float speed, geVec3d *Direction);
	void		DetectCollisions();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QuickViewDlg_H__EE526980_C9BC_11D4_9554_B04EBC493420__INCLUDED_)
