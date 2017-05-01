/****************************************************************************************/
/*  DirectInputMgr.h                                                                    */
/*                                                                                      */
/*  Author:       Tom Morris						                                    */
/*  Description:  Helper class that receives interprets mouse movement for QuickViewer  */
/*					Used by CQuickViewDlg											*/
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


#if !defined(AFX_DIRECTINPUTMGR_H__BF1E4741_1DAC_11D4_9553_E7D967777940__INCLUDED_)
#define AFX_DIRECTINPUTMGR_H__BF1E4741_1DAC_11D4_9553_E7D967777940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <direct.h>
#include <dinput.h>
#include <dinputd.h>

#define KEYDOWN(name,key) (name[key] & 0x80) 
#define MOUSE_LEFT_BUTTON	0
#define MOUSE_RIGHT_BUTTON	1


BOOL CALLBACK InitJoystickInput(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef);
BOOL	ReacquireJoystickInput(void);
BOOL	ReacquireKeyboardInput(void);
BOOL	ReacquireMouseInput(void);
BOOL	InitInput(HINSTANCE hInst, HWND hWnd, int controller );
void	UnloadDirectInput(void);


BOOL	IsKeyDown (int KeyCode);
BOOL	NewKeyDown(int KeyCode);
void	SetCommonKeyboardControls();
void	SetKeyboardControls();
long	GetHexFromVKCode(char* vkcode);



#define DINPUT_BUFFERSIZE  16



extern int ACTION_FORWARD;
extern int ACTION_BACKWARD;
extern int ACTION_SLIDE_LEFT;
extern int ACTION_SLIDE_RIGHT;
extern int ACTION_MENU;

class CDirectInputMgr  
{
public:
	CDirectInputMgr();
	virtual ~CDirectInputMgr();

};

#endif // !defined(AFX_DIRECTINPUTMGR_H__BF1E4741_1DAC_11D4_9553_E7D967777940__INCLUDED_)
