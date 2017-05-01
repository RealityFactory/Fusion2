/****************************************************************************************/
/*  QuickViewDlg.cpp																*/
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

#include "stdafx.h"
#include "GEditPro.h"
#include "QuickViewDlg.h"
#include "DirectInput\DirectInputMgr.h"

extern	LPDIRECTINPUTDEVICE8 g_pdevKeyboard;
extern	LPDIRECTINPUTDEVICE8 g_pdevMouse;
extern	DIDEVCAPS            g_diDevCaps;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuickViewDlg dialog


CQuickViewDlg::CQuickViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuickViewDlg)
	m_cameraXPos = 0;
	m_cameraYPos = 0;
	m_cameraZPos = 0;
	m_DriverName = _T("");
	m_bDrawGripper = FALSE;
	Width = 640;
	Height = 480;
	m_collisionDetection = false;		//	no collision detection
	m_strPreviewLevel = _T("");
	//}}AFX_DATA_INIT

	Engine = NULL;
	DrvSys = NULL;
	Driver = NULL;
	Mode = NULL;
	SoundSys = NULL;
	Camera = NULL;
	World = NULL;
	m_previewWnd;
	m_softwareRadioBtn = NULL;
	m_direct3dRadioBtn = NULL;
	m_640RadioBtn = NULL;
	m_800RadioBtn = NULL;
	m_1024RadioBtn = NULL;
	m_collideRadioBtn = NULL;
	m_noCollideRadioBtn = NULL;

}


void CQuickViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickViewDlg)
	DDX_Control(pDX, IDC_ZERO_CAMERA_BTN, m_zeroCameraBtn);
	DDX_Control(pDX, IDC_CAMERA_SLIDERZ, m_cameraSliderZ);
	DDX_Control(pDX, IDC_CAMERA_SLIDERY, m_cameraSliderY);
	DDX_Control(pDX, IDC_CAMERA_SLIDERX, m_cameraSliderX);
	DDX_Control(pDX, IDC_SOFTWARE_TEXT, m_softwareText);
	DDX_Control(pDX, IDC_SHOW_PREVIEW_BTN, m_previewWithEngineBtn);
	DDX_Text(pDX, IDC_CAMERA_X, m_cameraXPos);
	DDX_Text(pDX, IDC_CAMERA_Y, m_cameraYPos);
	DDX_Text(pDX, IDC_CAMERA_Z, m_cameraZPos);
	DDX_Text(pDX, IDC_EDIT_PREVIEWLEVEL_PATH, m_strPreviewLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuickViewDlg, CDialog)
	//{{AFX_MSG_MAP(CQuickViewDlg)
	ON_BN_CLICKED(IDC_SHOW_PREVIEW_BTN, OnShowPreviewBtn)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PREVIEW_BROWSE_BTN, OnPreviewBrowseBtn)
	ON_BN_CLICKED(IDC_RADIO_640, OnRadio640)
	ON_BN_CLICKED(IDC_RADIO_1024, OnRadio1024)
	ON_BN_CLICKED(IDC_RADIO_800, OnRadio800)
	ON_BN_CLICKED(IDC_RADIO_COLLIDE, OnRadioCollide)
	ON_BN_CLICKED(IDC_RADIO_NOCOLLIDE, OnRadioNocollide)
	ON_BN_CLICKED(IDC_SOFTWARE_RADIO, OnSoftwareRadio)
	ON_BN_CLICKED(IDC_DIRECT3D_RADIO, OnDirect3dRadio)
	ON_EN_KILLFOCUS(IDC_CAMERA_X, OnKillfocusCameraX)
	ON_EN_KILLFOCUS(IDC_CAMERA_Y, OnKillfocusCameraY)
	ON_EN_KILLFOCUS(IDC_CAMERA_Z, OnKillfocusCameraZ)
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO() 
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ZERO_CAMERA_BTN, OnZeroCameraBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickViewDlg message handlers

BOOL CQuickViewDlg::OnInitDialog()		//	post 0.55	// this whole func
{
//	MSG					msg;
	CDialog::OnInitDialog();

	if (m_DriverName == "")
	{
		m_DriverName = "D3D";
	}
//	driverString = "(";
//	Width = -1;		//	for windowed mode
//	Height = -1;	//	for windowed mode	


	Engine = NULL;
	World = NULL;		//	post 0.55
	SoundSys = NULL;
	DrvSys = NULL;
	Camera = NULL;
	Driver = NULL;
	Mode = NULL;

	CameraExtBox.Min.X = -20.0f;
	CameraExtBox.Min.Y = -20.0f;
	CameraExtBox.Min.Z = -20.0f;
	CameraExtBox.Max.X = 20.0f;
	CameraExtBox.Max.Y = 20.0f;
	CameraExtBox.Max.Z = 20.0f;


	if (CGlobals::m_recentCompiledLevel != "")
	{
		m_strPreviewLevel = CGlobals::m_recentCompiledLevel;
		m_previewWithEngineBtn.EnableWindow(TRUE);
	}

	m_softwareRadioBtn = (CButton*)this->GetDlgItem(IDC_SOFTWARE_RADIO);
	m_direct3dRadioBtn = (CButton*)this->GetDlgItem(IDC_DIRECT3D_RADIO);

	m_640RadioBtn = (CButton*)this->GetDlgItem(IDC_RADIO_640);
	m_800RadioBtn = (CButton*)this->GetDlgItem(IDC_RADIO_800);
	m_1024RadioBtn = (CButton*)this->GetDlgItem(IDC_RADIO_1024);

	m_collideRadioBtn = (CButton*)this->GetDlgItem(IDC_RADIO_COLLIDE);
	m_noCollideRadioBtn = (CButton*)this->GetDlgItem(IDC_RADIO_NOCOLLIDE);


	if (m_DriverName == "" || m_DriverName == "D3D")
	{
		OnDirect3dRadio();
		m_direct3dRadioBtn->SetCheck(1);
	}
	
	if (m_DriverName == "Software")
	{
		OnSoftwareRadio();
		m_softwareRadioBtn->SetCheck(1);
		m_bDrawGripper = TRUE;
	}

	
	switch (Width)
	{
	case  640:
		m_640RadioBtn->SetCheck(1);
		break;
	case 800:
		m_800RadioBtn->SetCheck(1);
		break;
	case 1024:
		m_1024RadioBtn->SetCheck(1);
	}

	if (m_collisionDetection)
		m_collideRadioBtn->SetCheck(1);		
	if (!m_collisionDetection)
		m_noCollideRadioBtn->SetCheck(1);

	
	m_cameraSliderX.SetRange(-4000, 4000);
	m_cameraSliderY.SetRange(-4000, 4000);
	m_cameraSliderZ.SetRange(-4000, 4000);
	
	m_cameraSliderX.SetTicFreq(200);
	m_cameraSliderY.SetTicFreq(200);
	m_cameraSliderZ.SetTicFreq(200);

	m_cameraSliderX.SetPos(m_cameraXPos);
	m_cameraSliderY.SetPos(m_cameraYPos);
	m_cameraSliderZ.SetPos(m_cameraZPos);

//	if (m_strPreviewLevel != "")
//	{
//		m_previewLevelList.InsertItem(0, m_previewLevelPath);
//		m_previewWithEngineBtn.EnableWindow(TRUE);
//	}

	GetWindowRect(m_restoreRect);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}





void CQuickViewDlg::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	CRect previewRect;

	Width = previewRect.Width();
	Height = previewRect.Height();
	
	CDialog::CalcWindowRect(lpClientRect, nAdjustType);
}


				//	since this is a resizeable dialog box, we need to set
				//	its MINIMUM size
void CQuickViewDlg::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	CDialog::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x = m_restoreRect.right - m_restoreRect.left;
	lpMMI->ptMinTrackSize.y = m_restoreRect.bottom - m_restoreRect.top;

}


geBoolean CQuickViewDlg::InitEngine(HWND hWnd)
{
	SetCurrentDirectory(CGlobals::m_GlobalAppPath);

	if (Engine == NULL)
	{
		Engine = geEngine_Create(hWnd, "GEditPro QuckViewer", ".");
		if (!Engine)
		{
			AfxMessageBox( "Could not create engine object!");
			return GE_FALSE;
		}
	}

	if (!LoadDriver())
		return GE_FALSE;	//post 0.5

	//Create the camera object. 
	if (Camera == NULL)
	{
		Camera = geCamera_Create(2.0f, &Rect);
	}
	
	if (!Camera)
	{
		AfxMessageBox( "Could not create camera object!");
		return GE_FALSE;	//post 0.5
	}

	//Load the level
	char* levelpath = NULL;

	levelpath = m_strPreviewLevel.GetBuffer(m_strPreviewLevel.GetLength());
	
	if (levelpath)
	{
		if (!LoadLevel(levelpath))
		{
			return GE_FALSE;
		}
	}
	else
		return GE_FALSE;

	//Set up the XForm for the camera.  XForms determine our place in the world.
	//Clear the transform
	geXForm3d_SetIdentity(&XForm);

#pragma warning (disable:4244)
	XForm.Translation.X = m_cameraXPos;
	XForm.Translation.Y = m_cameraYPos;
	XForm.Translation.Z = m_cameraZPos;
#pragma warning (default:4244)
	//Setup the initial rotation
	geXForm3d_RotateX(&XForm, 0.0f);
	geXForm3d_RotateY(&XForm, 0.0f);
	geXForm3d_RotateZ(&XForm, 0.0f);

	//Set the XForm to the camera
	geCamera_SetWorldSpaceXForm(Camera, &XForm);

	return GE_TRUE;
}



///////////////////////////////////////////////////////////////////////////////
//	LoadDriver
///////////////////////////////////////////////////////////////////////////////
geBoolean	CQuickViewDlg::LoadDriver()
{
	CString sDriverFindError = _T("Could not find a valid driver!\n");
	sDriverFindError += _T("Make sure the following Genesis3D video drivers\n");
	sDriverFindError += _T("reside in the same directory as GEditPro.exe\n");
	sDriverFindError += _T("o  SoftDrv.dll and softdrv2.dll - for software rendering\n\n");
	sDriverFindError += _T("o  D3DDrv.dll - for Direct3D video cards\n");
	
	long	tempwidth;
	long	tempheight;
	const	char *drvname = NULL;	
	bool	weScrewedUp = false;
	
	if (DrvSys == NULL)
	{
		SetCurrentDirectory(CGlobals::m_GlobalAppPath);
		
		DrvSys = geEngine_GetDriverSystem(Engine);
		if (DrvSys == NULL)
		{
			AfxMessageBox( "Could not get the Genesis Driver System!");
			return GE_FALSE;
		}
	}
	
	if (Driver == NULL)	
	{
		Driver = geDriver_SystemGetNextDriver(DrvSys, NULL);
		
		if (Driver == NULL)
		{
			AfxMessageBox(sDriverFindError);
			weScrewedUp = true;
		}// if(Driver...
	}
	
	while (Driver != NULL)
	{
		geDriver_GetName(Driver, &drvname);
		if (strstr(drvname, m_DriverName) != NULL) break;
		
		Driver = geDriver_SystemGetNextDriver(DrvSys, Driver);
		
		if (!Driver && !strcmp(m_DriverName, "D3D"))
		{
			AfxMessageBox("Direct3D not available on this system. Please choose Software or Glide");
			weScrewedUp = true;
			//			return GE_FALSE;
		}
		
		if (!Driver)
		{
			AfxMessageBox("Cannot find the specified driver");
			weScrewedUp = true;		
			//			return GE_FALSE;
		}	
	}	//	while(Driver...
	
	
	if (weScrewedUp)
	{
		return GE_FALSE;
	}
#ifdef _DEBUG
	Height = -1;
	Width = -1;
#endif
	

	if (Mode == NULL)
	{	
		Mode = geDriver_GetNextMode(Driver, NULL);
		while (Mode != NULL)
		{
			geDriver_ModeGetWidthHeight(Mode, &tempwidth, &tempheight);
			if ((tempwidth == Width) && (tempheight == Height) ||	//	specced res
				(tempwidth == -1) && (tempheight == -1))			//	for software
				break;//	return TRUE;
			
			Mode = geDriver_GetNextMode(Driver, Mode);
		};	//	while (DriverMode...
		
	}
	geEngine_EnableFrameRateCounter(Engine, GE_FALSE);
	ShowCursor(FALSE);	
	
	if (!geEngine_SetDriverAndMode(Engine, Driver, Mode))		//	post 0.55
	{
		AfxMessageBox("Could not start the engine!\nDriver or Mode Problems");
		return GE_FALSE;
	}

	ShowWindow(SW_SHOWNORMAL);
	
	return GE_TRUE;
}




geBoolean CQuickViewDlg::LoadLevel(char *FileName)
{
	geVFile			*Level = NULL;

	Level = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, FileName, 
		NULL, GE_VFILE_OPEN_READONLY);
	
	if (!Level)
	{
		AfxMessageBox( "Could not load file!");
		return GE_FALSE;
	}

	World = geWorld_Create(Level);
	if (!World)
	{
		AfxMessageBox( "Could not create world!");
		return GE_FALSE;
	}

	if (!geEngine_AddWorld(Engine, World))
	{
		AfxMessageBox( "Could not add world to engine!");
		return GE_FALSE;
	}

	return GE_TRUE;
}

				//	post 0.55
void CQuickViewDlg::Shutdown()
{
	if (World != NULL)
	{
		if (!geEngine_RemoveWorld(Engine, World))
		{
			AfxMessageBox("Failed to Remove World");
		}
		geWorld_Free(World);
	}

	if (Camera != NULL)
		geCamera_Destroy(&Camera);

	if (Engine != NULL)
	{
		if (!geEngine_ShutdownDriver(Engine))
		{
			AfxMessageBox("Failed to Shutdown Driver");
		}

		if (!geEngine_UpdateWindow(Engine))
		{
			AfxMessageBox("Failed to Update Window following Diver Shutdown");
		}
		
		geEngine_Free(Engine);
	}
	
	World = NULL;
	Engine = NULL;
	SoundSys = NULL;
	DrvSys = NULL;
	Camera = NULL;
	Driver = NULL;
	Mode = NULL;

	UnloadDirectInput();
	ShowCursor(TRUE);

}


				//	post 0.55
void CQuickViewDlg::DisableDlgControls()
{
					//	if everything checks out, then disable all of the dialog
					//	components so we don't get that incessant dinging during
					//	our viewing session
	m_previewWithEngineBtn.EnableWindow(FALSE);

	m_softwareRadioBtn->EnableWindow(FALSE);
	m_direct3dRadioBtn->EnableWindow(FALSE);
	
	m_640RadioBtn->EnableWindow(FALSE);

	if (m_softwareRadioBtn->GetCheck() == 0)
	{
		m_800RadioBtn->EnableWindow(FALSE);
		m_1024RadioBtn->EnableWindow(FALSE);
	}

	m_collideRadioBtn->EnableWindow(FALSE);
	m_noCollideRadioBtn->EnableWindow(FALSE);

	m_zeroCameraBtn.EnableWindow(FALSE);

	GetDlgItem(IDC_CAMERA_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_CAMERA_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_CAMERA_Z)->EnableWindow(FALSE);

	GetDlgItem(IDC_CAMERA_SLIDERX)->EnableWindow(FALSE);
	GetDlgItem(IDC_CAMERA_SLIDERY)->EnableWindow(FALSE);
	GetDlgItem(IDC_CAMERA_SLIDERX)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_PREVIEWLEVEL_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_PREVIEW_BROWSE_BTN)->EnableWindow(FALSE);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
}

				//	post 0.55
void CQuickViewDlg::EnableDlgControls()
{
					//	enable all previously-disabled dialog box items
	m_previewWithEngineBtn.EnableWindow(TRUE);

	m_softwareRadioBtn->EnableWindow(TRUE);
	m_direct3dRadioBtn->EnableWindow(TRUE);

	m_640RadioBtn->EnableWindow(TRUE);
					//	if not software, then enable higher resoultions
	if (m_softwareRadioBtn->GetCheck() == 0)
	{
		m_800RadioBtn->EnableWindow(TRUE);
		m_1024RadioBtn->EnableWindow(TRUE);
	}

	m_collideRadioBtn->EnableWindow(TRUE);
	m_noCollideRadioBtn->EnableWindow(TRUE);

	m_zeroCameraBtn.EnableWindow(TRUE);

	GetDlgItem(IDC_CAMERA_X)->EnableWindow(TRUE);
	GetDlgItem(IDC_CAMERA_Y)->EnableWindow(TRUE);
	GetDlgItem(IDC_CAMERA_Z)->EnableWindow(TRUE);

	GetDlgItem(IDC_CAMERA_SLIDERX)->EnableWindow(TRUE);
	GetDlgItem(IDC_CAMERA_SLIDERY)->EnableWindow(TRUE);
	GetDlgItem(IDC_CAMERA_SLIDERX)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_PREVIEWLEVEL_PATH)->EnableWindow(TRUE);
	GetDlgItem(IDC_PREVIEW_BROWSE_BTN)->EnableWindow(TRUE);

	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

								//	post 0.55
void CQuickViewDlg::RestoreDialogBox()
{
	if (m_DriverName ==  _T("Glide"))
	{
		//	modified post 0.58
		SetWindowPos(&wndTopMost, m_restoreRect.left, m_restoreRect.top, 
		m_restoreRect.right-m_restoreRect.left, 
		m_restoreRect.bottom-m_restoreRect.top, SWP_SHOWWINDOW);
	}

	CenterWindow();
					//	once out of the loop, redraw the dialog box controls
	EnableDlgControls();	//	post 0.55
	RedrawWindow();	
}



void CQuickViewDlg::OnShowPreviewBtn() 
{
					//	get the current Rect of this dialog box
	GetClientRect(m_previewRect);

					//	if the Software driver is selected, then rendering will
					//	be in the window defined by this dialog box
	if (m_DriverName ==  _T("Software"))
	{
		Rect.Left = m_previewRect.left;
		Rect.Right = m_previewRect.right - 1;
		Rect.Top = m_previewRect.top;
		Rect.Bottom = m_previewRect.bottom - 1;
	}
	else			//	otherwise, it will be full screen
	{
		Rect.Left = 0;
		Rect.Right = Width - 1;
		Rect.Top = 0;
		Rect.Bottom = Height - 1;
	}
					//	initialize direct input and link it to this window
	if(!InitInput(AfxGetApp()->m_hInstance, this->m_hWnd, 0))	//	post 0.55
	{
		AfxMessageBox("Failed to initialize DirectInput");
		Shutdown();
		RestoreDialogBox();
		return;
	}
					// initialize thew Genesis3D engine and link it to this window
	if (!InitEngine(this->m_hWnd))		//	post 0.55
	{
		AfxMessageBox("Failed to initialize Engine");
		Shutdown();
		RestoreDialogBox();
		return;
	}
					//	establish how the keyboard keys will influence movement
	SetCommonKeyboardControls();
	SetKeyboardControls();

	DisableDlgControls();	//	post 0.55

	this->SetFocus();

					//	Enter the rendering loop
	m_iMasterRun = 1;
	OldTimeGetTime = (float)timeGetTime();
	MSG		msg;
	
	while (m_iMasterRun == 1)
	{
		m_cameraOldPos = XForm.Translation;		//	old position
		m_cameraNewPos = XForm.Translation;		//	new position	
		ControlCamera();						//	for controlling our camera's flight
		
		deltaTime = (float)(timeGetTime() - OldTimeGetTime)*0.001f;
		OldTimeGetTime = (float)timeGetTime();
			
		if (!geEngine_BeginFrame(Engine, Camera, GE_TRUE))
			m_iMasterRun = 0;
		
		if (!geEngine_RenderWorld(Engine, World, Camera, 0.0f))
			m_iMasterRun = 0;
		
		geEngine_Printf(Engine, Rect.Left + 10, Rect.Bottom - 20, "EDSF keys to move");
		geEngine_Printf(Engine, Rect.Right - 110, Rect.Bottom - 20, "<Esc> to exit");

		if (m_DriverName ==  _T("D3D"))
			geEngine_Printf(Engine, (Rect.Right/2) - 10, Rect.Bottom - 20, "Direct3D");
		if (m_DriverName ==  _T("Software"))
			geEngine_Printf(Engine, (Rect.Right/2) - 10, Rect.Bottom - 20, "Software");

		if (!geEngine_EndFrame(Engine))
			m_iMasterRun = 0;
		
					//	let window poll for messages
		while (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0 ))
			{
				m_iMasterRun = 0;
				break;
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
	}

					//	Shutdown the Genesis engine and clean up the memory	
	Shutdown();

	RestoreDialogBox();

}


//	MoveCamera	================================================================
//				Moves our camera forward or backward, and checks for collisions
//	============================================================================
void CQuickViewDlg::MoveCamera(float speed, geVec3d *Direction)
{
	float	movespeed;
	movespeed = speed * deltaTime; 
											//	Move forward or backward
	geVec3d_AddScaled (&m_cameraNewPos, Direction, movespeed, &m_cameraNewPos); 
	
	if (m_collisionDetection)
	{
		DetectCollisions();
	}
}




//	DetectCollisions	===============================================================
//						Checks for collisions w/ up to 3 intersecting planes
//	===========================================================================
void CQuickViewDlg::DetectCollisions()
{
	float	Slide;
	BOOL    Result;
	GE_Collision Collision;

	
	//	First, check to see if we are colliding with ONE surface 
	Result = geWorld_Collision(World, &CameraExtBox.Min, &CameraExtBox.Max, &m_cameraOldPos, &m_cameraNewPos, 
		GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_ALL, 0xffffffff, NULL, NULL, &Collision);
	
	if(Result == 1)					//Your new position collides with something
	{
		Slide = geVec3d_DotProduct (&m_cameraNewPos, &Collision.Plane.Normal) - Collision.Plane.Dist;
		m_cameraNewPos.X -= Collision.Plane.Normal.X * Slide;
		m_cameraNewPos.Y -= Collision.Plane.Normal.Y * Slide;
		m_cameraNewPos.Z -= Collision.Plane.Normal.Z * Slide;
	
		//	Now, test to see if you are also colliding with a SECOND surface
		Result = geWorld_Collision(World, &CameraExtBox.Min, &CameraExtBox.Max, &m_cameraOldPos, &m_cameraNewPos, 
			GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_ALL, 0xffffffff, NULL, NULL, &Collision);
		
		if(Result == 1)					//Your new position collides with something
		{
			Slide = geVec3d_DotProduct (&m_cameraNewPos, &Collision.Plane.Normal) - Collision.Plane.Dist;
			m_cameraNewPos.X -= Collision.Plane.Normal.X * Slide;
			m_cameraNewPos.Y -= Collision.Plane.Normal.Y * Slide;
			m_cameraNewPos.Z -= Collision.Plane.Normal.Z * Slide;
			
			//	Finally, test to see if you are also colliding with a THIRD surface			
			Result = geWorld_Collision(World, &CameraExtBox.Min, &CameraExtBox.Max, &m_cameraOldPos, &m_cameraNewPos, 
				GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_ALL, 0xffffffff, NULL, NULL, &Collision);
			
			if(Result == 1)					//Your new position collides with something
			{
				Slide = geVec3d_DotProduct (&m_cameraNewPos, &Collision.Plane.Normal) - Collision.Plane.Dist;
				m_cameraNewPos.X -= Collision.Plane.Normal.X * Slide;
				m_cameraNewPos.Y -= Collision.Plane.Normal.Y * Slide;
				m_cameraNewPos.Z -= Collision.Plane.Normal.Z * Slide;
			}
		}
	}
}



//	ControlCamera	==============================================================
//						Sets up rotation & movement based on mouse pos & key presses
//	==================================================================================
void CQuickViewDlg :: ControlCamera()		
{
	geXForm3d		TempXform;				//	for holding rotational info
	geQuaternion	Quat;					//	our quaternion variable
	geFloat			TURN_SPEED = 0.0f;		// speed camera will move if turning left/right
	geFloat			MOVE_SPEED = 0.0f;		//	speed craft will move forward or backward
	int				Movement = 0;
	char			buffer[256]; 
	HRESULT			hr; 
	DIMOUSESTATE	MouseState;
	float			Sensitivity = 0.005f;
	float			NormalSpeed = 60.0f;

	if (m_iMasterRun == 1)
	{
		//	DirectInput stuff
		hr = g_pdevKeyboard->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 
		if FAILED(hr) 
		{  if (hr == DIERR_INPUTLOST) ReacquireKeyboardInput();
		hr = g_pdevKeyboard->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 
		} 	

		hr = g_pdevMouse->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&MouseState); 
		if FAILED(hr) 
		{  if (hr == DIERR_INPUTLOST) ReacquireMouseInput();
		hr = g_pdevMouse->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&MouseState); 
		} 	

		if ((MouseState.lX) | (MouseState.lY) |
			(KEYDOWN(buffer, ACTION_FORWARD))  |
			(KEYDOWN(buffer, ACTION_BACKWARD)) |
			(KEYDOWN(buffer, ACTION_SLIDE_LEFT))  |
			(KEYDOWN(buffer, ACTION_SLIDE_RIGHT)))

		{
			if (MouseState.lX)		//	is mouse moving left or right?
			{
				TempXform = XForm;
				geXForm3d_SetZRotation(&TempXform, 0.0f);	//	Prevent actor roll
				geXForm3d_GetUp(&TempXform, &m_cameraUp);		//	Get Z axis

				TURN_SPEED   = MouseState.lX * Sensitivity *20;
				/*	We will create a quaternion based on our actor's
				object-space y axis. This is completely independent
				from the World's y axis. Then we will rotate this
				quaternion based on our mouse's position. */
				geQuaternion_SetFromAxisAngle(&Quat, &m_cameraUp, -RAD(TURN_SPEED));
				/*	Now, we convert our quaternion into a temporary
				transform. This is a preparatory step so we can
				give our PlayerXform the rotational properties of
				our quaternion. */
				geQuaternion_ToMatrix(&Quat, &TempXform);
				/*	Finally, we paste our TempXform's rotational
				properties into our PlayerXform. Our PlayerXform
				now is rotated in object-space. */
				geXForm3d_Multiply(&TempXform, &XForm, &XForm);
			}	//	if (MouseState.1X...

			if (MouseState.lY)			//	is mouse moving up or down?
			{
				TempXform = XForm;
				geXForm3d_SetZRotation(&TempXform, 0.0f);		//	Prevent actor roll
				geXForm3d_GetLeft(&XForm, &m_cameraLeft);		//	Get X axis

				TURN_SPEED   = MouseState.lY * Sensitivity *20;
				geQuaternion_SetFromAxisAngle(&Quat, &m_cameraLeft, RAD(TURN_SPEED));
				geQuaternion_ToMatrix(&Quat, &TempXform);
				geXForm3d_Multiply(&TempXform, &XForm, &XForm);
			}	//	if (MouseState.1Y...

			//	MOVEMENT CONTROL	=============================================================
			if (KEYDOWN(buffer, ACTION_FORWARD))					// Move Forward	
			{
				Movement += 1;
				MOVE_SPEED = Movement * Sensitivity * 2000;	//	determine the speed
				geXForm3d_GetIn(&XForm, &m_cameraIn);				//	Get the forward vector for the craft 
				MoveCamera ((float)NormalSpeed * MOVE_SPEED, &m_cameraIn);		//	move it forward
			}

			if (KEYDOWN(buffer, ACTION_BACKWARD))					// Move Backward
			{
				Movement -= 1;
				MOVE_SPEED = Movement * Sensitivity * 2000;	//	determine the speed
				geXForm3d_GetIn(&XForm, &m_cameraIn);				//	Get the forward vector for the craft 
				MoveCamera ((float)NormalSpeed * MOVE_SPEED, &m_cameraIn);		//	move it forward
			}

			if (KEYDOWN(buffer, ACTION_SLIDE_LEFT))					//	Slide left
			{
				Movement += 1;
				MOVE_SPEED = Movement * Sensitivity * 2000;	//	determine the speed
				geXForm3d_GetLeft(&XForm, &m_cameraLeft);			//	Get the left vector for the craft 
				MoveCamera ((float)NormalSpeed * MOVE_SPEED, &m_cameraLeft);	//	move it left
			}

			if (KEYDOWN(buffer, ACTION_SLIDE_RIGHT))				//	Slide right
			{
				Movement -= 1;
				MOVE_SPEED = Movement * Sensitivity * 2000;	//	determine the speed
				geXForm3d_GetLeft(&XForm, &m_cameraLeft);			//	Get the left vector for the craft 
				MoveCamera ((float)NormalSpeed * MOVE_SPEED, &m_cameraLeft);		//	move it right
			}

		}	//	if (MouseState.1X ||...


		//	Translate ONCE after rotation and movement
		XForm.Translation = m_cameraNewPos;	//	update our camera's position in the world
		geCamera_SetWorldSpaceXForm(Camera, &XForm);
	}	//	if (m_iMasterRun == 1)...

	if (KEYDOWN(buffer, ACTION_MENU))	//	check for <Esc>
	{m_iMasterRun = 0;}
}



void CQuickViewDlg::OnPreviewBrowseBtn() 
{

	int				itemtext = 0;
	int				nItem = 0;
	LPCTSTR			bsp = NULL;
	CFileDialog		m_levelFile
		(
		TRUE,
		bsp,
		CGlobals::m_GlobalAppPath,
		OFN_EXTENSIONDIFFERENT	|
		OFN_OVERWRITEPROMPT | 
		OFN_PATHMUSTEXIST |
		OFN_FILEMUSTEXIST,
		_T("bsp files(.bsp)|*.bsp|")
		);
	
	m_levelFile.m_ofn.lpstrTitle	="GEditPro QuickViewer Level Picker";

	if(IDOK !=m_levelFile.DoModal())
		return;
	
	POSITION pos = m_levelFile.GetStartPosition();
	while (pos)
	{
		m_strPreviewLevel = m_levelFile.GetNextPathName(pos);
	}
	if (m_strPreviewLevel != _T(""))
	{
		m_previewWithEngineBtn.EnableWindow(TRUE);
	}

	UpdateData(FALSE);
}


void CQuickViewDlg::OnRadio640() 
{
	Width = 640;
	Height = 480;
	
}


void CQuickViewDlg::OnRadio800() 
{
	Width = 800;
	Height = 600;
	
}


void CQuickViewDlg::OnRadio1024() 
{
	Width = 1024;
	Height = 768;
	
}


void CQuickViewDlg::OnSoftwareRadio() 
{
	m_DriverName = _T("Software");
	
	OnRadio640();

	m_640RadioBtn->SetCheck(1);
	m_800RadioBtn->SetCheck(0);
	m_1024RadioBtn->SetCheck(0);

	m_softwareText.ShowWindow(TRUE);

	m_800RadioBtn->EnableWindow(FALSE);
	m_1024RadioBtn->EnableWindow(FALSE);

	m_bDrawGripper = TRUE;
	RedrawWindow();

}


void CQuickViewDlg::OnDirect3dRadio() 
{
	m_DriverName = _T("D3D");	

	m_softwareText.ShowWindow(FALSE);

	m_800RadioBtn->EnableWindow(TRUE);
	m_1024RadioBtn->EnableWindow(TRUE);

	m_bDrawGripper = FALSE;
	RedrawWindow();

}


void CQuickViewDlg::OnRadioCollide() 
{
	m_collisionDetection = true;
	
}


void CQuickViewDlg::OnRadioNocollide() 
{
	m_collisionDetection = false;	
}



void CQuickViewDlg::OnClose() 
{
	m_iMasterRun = 0;
#ifdef _DEBUG
	
	Width = 640;
	Height = 480;
#endif

	CDialog::EndDialog(1);
}



void CQuickViewDlg::OnCancel()
{
	OnClose();
}



void CQuickViewDlg::OnOK()
{
	OnClose();
}


void CQuickViewDlg::OnKillfocusCameraX() 
{
	int	lastValue = m_cameraXPos;
	int currentValue = GetDlgItemInt(IDC_CAMERA_X);

	if (currentValue == 0)
	{
		this->SetDlgItemInt(IDC_CAMERA_X, 0);
		m_cameraSliderX.SetPos(0);
		UpdateData(TRUE);
		return;
	}

	if ((currentValue >= -4000) && (currentValue <= 4000)) 
	{
		m_cameraXPos = currentValue;
		m_cameraSliderX.SetPos(m_cameraXPos);
		UpdateData(TRUE);	
		return;
	}
	else
	{
		m_cameraXPos = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= -4000 and <= 4000");
	}
}

void CQuickViewDlg::OnKillfocusCameraY() 
{
	int	lastValue = m_cameraYPos;
	int currentValue = GetDlgItemInt(IDC_CAMERA_Y);

	if (currentValue == 0)
	{
		this->SetDlgItemInt(IDC_CAMERA_Y, 0);
		m_cameraSliderY.SetPos(0);
		UpdateData(TRUE);
		return;
	}

	if ((currentValue >= -4000) && (currentValue <= 4000)) 
	{
		m_cameraYPos = currentValue;
		m_cameraSliderY.SetPos(m_cameraYPos);
		UpdateData(TRUE);	
		return;
	}
	else
	{
		m_cameraYPos = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= -4000 and <= 4000");
	}
}

void CQuickViewDlg::OnKillfocusCameraZ() 
{
	int	lastValue = m_cameraZPos;
	int currentValue = GetDlgItemInt(IDC_CAMERA_Z);

	if (currentValue == 0)
	{
		this->SetDlgItemInt(IDC_CAMERA_Z, 0);
		m_cameraSliderZ.SetPos(0);
		UpdateData(TRUE);
		return;
	}

	if ((currentValue >= -4000) && (currentValue <= 4000)) 
	{
		m_cameraZPos = currentValue;
		m_cameraSliderZ.SetPos(m_cameraZPos);
		UpdateData(TRUE);	
		return;
	}
	else
	{
		m_cameraZPos = lastValue;
		UpdateData(FALSE);
		AfxMessageBox("Value must be >= -4000 and <= 4000");
	}
}


void CQuickViewDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl *pSlider = (CSliderCtrl*) pScrollBar;
	
	
	if (pSlider->m_hWnd == m_cameraSliderX.m_hWnd)
		m_cameraXPos = m_cameraSliderX.GetPos();
	if (pSlider->m_hWnd == m_cameraSliderY.m_hWnd)
		m_cameraYPos = m_cameraSliderY.GetPos();
	if (pSlider->m_hWnd == m_cameraSliderZ.m_hWnd)
		m_cameraZPos = m_cameraSliderZ.GetPos();
	
	UpdateData(FALSE);
}

void CQuickViewDlg::OnPaint() 
{
	CPaintDC dc(this); 

	if(m_bDrawGripper)
	{
		CRect rc;
		GetClientRect(rc);

		rc.left = rc.right - 20;
		rc.top = rc.bottom - 20;
		m_GripperRect = rc;
		dc.DrawFrameControl(rc,DFC_SCROLL,DFCS_SCROLLSIZEGRIP);
	}
}


UINT CQuickViewDlg::OnNcHitTest(CPoint point)
{
	UINT ht = CDialog::OnNcHitTest(point);

	if(ht==HTCLIENT && m_bDrawGripper)
	{
		CRect rc;
		GetWindowRect( rc );
		rc.left = rc.right - 20;
		rc.top = rc.bottom - 20;
		if(rc.PtInRect(point))
		{
			ht = HTBOTTOMRIGHT;
		}
	}
	return ht;
}

void CQuickViewDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	InvalidateRect(m_GripperRect);	

	RedrawWindow();
}


								//	post 0.55
								//	zero out camera coordinates, 
								//	set camera at level center
void CQuickViewDlg::OnZeroCameraBtn() 
{
	m_cameraSliderX.SetPos(0);
	m_cameraSliderY.SetPos(0);
	m_cameraSliderZ.SetPos(0);

	m_cameraXPos = m_cameraSliderX.GetPos();
	m_cameraYPos = m_cameraSliderY.GetPos();
	m_cameraZPos = m_cameraSliderZ.GetPos();

	UpdateData(FALSE);
	
}

