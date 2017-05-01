/****************************************************************************************/
/*  DirectInputMgr.cpp                                                                  */
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


#include "..\stdafx.h"
#include "..\Globals.h"
#include "DirectInputMgr.h"
#include <dinputd.h>



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////	TrilobiteShell

CDirectInputMgr::CDirectInputMgr()
{

}

CDirectInputMgr::~CDirectInputMgr()
{

}


#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION         0x0800
#endif
LPDIRECTINPUT8			pdi = NULL;
LPDIRECTINPUTDEVICE8	g_pdevKeyboard = NULL;	//	our keyboard device
LPDIRECTINPUTDEVICE8	g_pdevMouse = NULL;		//	our mouse device
DIDEVCAPS				g_diDevCaps;			//	our joystick's capabilities



int ACTION_FORWARD;
int ACTION_BACKWARD;
int ACTION_SLIDE_LEFT;
int ACTION_SLIDE_RIGHT;
int ACTION_MENU;




//	INITINPUT	================================================================
//	This function initializes DirectInput for the keyboard and all joysticks.
//	============================================================================

BOOL InitInput(HINSTANCE hInstance, HWND hWnd, int controller)
{
	HRESULT hr = NULL;

	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (VOID**)&pdi, NULL);
	if FAILED(hr) 
	{ 
		MessageBox(NULL,"Failed to DirectInputCreateEx","Error", MB_OK);
		return FALSE;
	}
	else if (!FAILED(hr))      
	{
		//	KEYBOARD ****************************************************************************

		hr = pdi->CreateDevice( GUID_SysKeyboard, &g_pdevKeyboard, NULL );
		if (hr != DI_OK) {return FALSE;} 
								// Set the data format using the predefined keyboard data 
								// format provided by the DirectInput object for keyboards. 
		hr = g_pdevKeyboard->SetDataFormat(&c_dfDIKeyboard); 
		if (hr != DI_OK) {return FALSE;} 
								// Set the cooperative level 
		hr = g_pdevKeyboard->SetCooperativeLevel(hWnd, 
			DISCL_FOREGROUND | DISCL_EXCLUSIVE); 
		if (hr != DI_OK) {return FALSE;} 
		Sleep(500);
		hr = g_pdevKeyboard->Acquire(); 
		if (hr != DI_OK)
		{
			if (hr == S_FALSE)
				AfxMessageBox(_T("Keyboard failed to acquire because already acquired"));
			if (hr == DIERR_INVALIDPARAM )
				AfxMessageBox(_T("Keyboard failed to acquire because DIERR_INVALIDPARAM"));
			if (hr == DIERR_NOTINITIALIZED )
				AfxMessageBox(_T("Keyboard failed to acquire because DIERR_NOTINITIALIZED"));
			if (hr == DIERR_OTHERAPPHASPRIO)
				AfxMessageBox(_T("Keyboard failed to acquire because DIERR_OTHERAPPHASPRIO"));
			return FALSE;
		} 
		//	END KEYBOARD INTIALIZATION ******************************************************** 
	
		
		//	MOUSE ///////////////////////////////////////////////////////////////////////////////

		hr = pdi->CreateDevice(GUID_SysMouse, &g_pdevMouse, NULL);
		if (FAILED(hr)) {return FALSE;}
		hr = g_pdevMouse->SetDataFormat(&c_dfDIMouse);
		if (FAILED(hr)) {return FALSE;}
		hr = g_pdevMouse->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		if (FAILED(hr)) {return FALSE;}
		hr = g_pdevMouse->Acquire(); 
		if FAILED(hr) {return FALSE;} 
		//	END MOUSE INTIALIZATION //////////////////////////////////////////////////////////// 
	

/*


	hr = DirectInputCreate( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         &pdi, NULL);
    if FAILED(hr) 
	{ 
		MessageBox(NULL,"Failed to DirectInputCreateEx","Error", MB_OK);
		return FALSE;
	}
	else if (!FAILED(hr))      
	{
		//	KEYBOARD ****************************************************************************

		  // Obtain an interface to the system keyboard device.
		hr = pdi->CreateDevice( GUID_SysKeyboard, &g_pdevKeyboard, NULL );
 		if FAILED(hr) {return FALSE;} 
		
		// Set the data format using the predefined keyboard data 
		// format provided by the DirectInput object for keyboards. 
		hr = g_pdevKeyboard->SetDataFormat(&c_dfDIKeyboard); 
		if FAILED(hr) {return FALSE;} 
								// Set the cooperative level 
		hr = g_pdevKeyboard->SetCooperativeLevel(hWnd, 
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
		if FAILED(hr) {return FALSE;} 
		hr = g_pdevKeyboard->Acquire(); 
		if FAILED(hr) {return FALSE;} 
		//	END KEYBOARD INTIALIZATION ******************************************************** 
		
			
		//	MOUSE ///////////////////////////////////////////////////////////////////////////////
		hr =pdi->CreateDevice( GUID_SysMouse, &g_pdevMouse, NULL );
		if (FAILED(hr)) {return FALSE;}
		hr = g_pdevMouse->SetDataFormat(&c_dfDIMouse);
		if (FAILED(hr)) {return FALSE;}
		hr = g_pdevMouse->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		if (FAILED(hr)) {return FALSE;}
		hr = g_pdevMouse->Acquire(); 
		if FAILED(hr) {return FALSE;} 
		//	END MOUSE INTIALIZATION //////////////////////////////////////////////////////////// 

	}
	*/
		}
    return TRUE;
}


//	UNLOADDIRECTINPUT	========================================================
//	This function cleans up all DirectInput objects.
//	============================================================================

void UnloadDirectInput(void)
{
	if (g_pdevKeyboard) 
	{
		g_pdevKeyboard->Unacquire();
		g_pdevKeyboard->Release();
		g_pdevKeyboard = NULL;
	}
	if (g_pdevMouse) 
	{
		g_pdevMouse->Unacquire();
		g_pdevMouse->Release();
		g_pdevMouse = NULL;
	}
}


BOOL ReacquireKeyboardInput(void) 
{ 
    HRESULT hRes; 

  if (g_pdevKeyboard) 
    { 
        // acquire the device 
        hRes = IDirectInputDevice_Acquire(g_pdevKeyboard); 
 
        if (SUCCEEDED(hRes)) 
        { 
            // acquisition successful 
            return TRUE; 
        } 
        else 
        { 
            // acquisition failed 
            return FALSE; 
        } 
    } 
    else 
    { 
        // we don't have a current device 
        return FALSE; 
    } 
}
  
  
BOOL ReacquireMouseInput(void) 
{ 
    HRESULT hRes;   
  
  if (g_pdevMouse) 
    { 
        // acquire the device 
        hRes = IDirectInputDevice_Acquire(g_pdevMouse); 
        
  
        if (SUCCEEDED(hRes)) 
        { 
            // acquisition successful 
            return TRUE; 
        } 
        else 
        { 
            // acquisition failed 
            return FALSE; 
        } 
    } 
    else 
    { 
        // we don't have a current device 
        return FALSE; 
    } 
}




///////////////////////////////////////////////////////////////////////////////////////
// This function gets keypresses as fast as possible.							- Orf
geBoolean IsKeyDown (int KeyCode)
{
		if (GetAsyncKeyState(KeyCode) & 0x8000)
			return GE_TRUE;

	return GE_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
// This function gets new keypresses.											- Orf
geBoolean NewKeyDown(int KeyCode)
{
	if (GetAsyncKeyState(KeyCode) & 1)
			return GE_TRUE;

	return GE_FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
//	SetKeyboardControls()
///////////////////////////////////////////////////////////////////////////////////////

void SetCommonKeyboardControls()
{
		ACTION_MENU = DIK_ESCAPE;
}


void SetKeyboardControls()
{
		ACTION_FORWARD = DIK_E;
		ACTION_BACKWARD = DIK_D;
		ACTION_SLIDE_LEFT = DIK_S;
		ACTION_SLIDE_RIGHT = DIK_F;
		return;
}

//////////////////////////////////////////////////////////////////////////////////////
//	GetHexFromVKCode
///////////////////////////////////////////////////////////////////////////////////////
long GetHexFromVKCode(char* vkcode)
{
	if (!stricmp(vkcode,"VK_LBUTTON")) return VK_LBUTTON;
	if (!stricmp(vkcode,"VK_RBUTTON")) return VK_RBUTTON;
	if (!stricmp(vkcode,"VK_CANCEL")) return VK_CANCEL;
	if (!stricmp(vkcode,"VK_MBUTTON")) return VK_MBUTTON;
	if (!stricmp(vkcode,"VK_BACK")) return VK_BACK;
	if (!stricmp(vkcode,"VK_TAB")) return VK_TAB;

	if (!stricmp(vkcode,"VK_CLEAR")) return VK_CLEAR;
	if (!stricmp(vkcode,"VK_RETURN")) return VK_RETURN;
	if (!stricmp(vkcode,"VK_SHIFT")) return VK_SHIFT;
	if (!stricmp(vkcode,"VK_CONTROL")) return VK_CONTROL;
	if (!stricmp(vkcode,"VK_MENU")) return VK_MENU;
	if (!stricmp(vkcode,"VK_PAUSE")) return VK_PAUSE;
	if (!stricmp(vkcode,"VK_ESCAPE")) return VK_ESCAPE;
	if (!stricmp(vkcode,"VK_SPACE")) return VK_SPACE;
	if (!stricmp(vkcode,"VK_PRIOR")) return VK_PRIOR;
	if (!stricmp(vkcode,"VK_NEXT")) return VK_NEXT;
	if (!stricmp(vkcode,"VK_END")) return VK_END;
	if (!stricmp(vkcode,"VK_HOME")) return VK_HOME;
	if (!stricmp(vkcode,"VK_LEFT")) return VK_LEFT;
	if (!stricmp(vkcode,"VK_UP")) return VK_UP;
	if (!stricmp(vkcode,"VK_RIGHT")) return VK_RIGHT;
	if (!stricmp(vkcode,"VK_DOWN")) return VK_DOWN;
	if (!stricmp(vkcode,"VK_SELECT")) return VK_SELECT;
	if (!stricmp(vkcode,"VK_PRINT")) return VK_PRINT;
	if (!stricmp(vkcode,"VK_EXECUTE")) return VK_EXECUTE;
	if (!stricmp(vkcode,"VK_SNAPSHOT")) return VK_SNAPSHOT;
	if (!stricmp(vkcode,"VK_INSERT")) return VK_INSERT;
	if (!stricmp(vkcode,"VK_DELETE")) return VK_DELETE;
	if (!stricmp(vkcode,"VK_HELP")) return VK_HELP;
	if (!stricmp(vkcode,"VK_0")) return 0x30;
	if (!stricmp(vkcode,"VK_1")) return 0x31;
	if (!stricmp(vkcode,"VK_2")) return 0x32;
	if (!stricmp(vkcode,"VK_3")) return 0x33;
	if (!stricmp(vkcode,"VK_4")) return 0x34;
	if (!stricmp(vkcode,"VK_5")) return 0x35;
	if (!stricmp(vkcode,"VK_6")) return 0x36;
	if (!stricmp(vkcode,"VK_7")) return 0x37;
	if (!stricmp(vkcode,"VK_8")) return 0x38;
	if (!stricmp(vkcode,"VK_9")) return 0x39;
	if (!stricmp(vkcode,"VK_A")) return 0x41;
	if (!stricmp(vkcode,"VK_B")) return 0x42;
	if (!stricmp(vkcode,"VK_C")) return 0x43;
	if (!stricmp(vkcode,"VK_D")) return 0x44;
	if (!stricmp(vkcode,"VK_E")) return 0x45;
	if (!stricmp(vkcode,"VK_F")) return 0x46;
	if (!stricmp(vkcode,"VK_G")) return 0x47;
	if (!stricmp(vkcode,"VK_H")) return 0x48;
	if (!stricmp(vkcode,"VK_I")) return 0x49;
	if (!stricmp(vkcode,"VK_J")) return 0x4A;
	if (!stricmp(vkcode,"VK_K")) return 0x4B;
	if (!stricmp(vkcode,"VK_L")) return 0x4C;
	if (!stricmp(vkcode,"VK_M")) return 0x4D;
	if (!stricmp(vkcode,"VK_N")) return 0x4E;
	if (!stricmp(vkcode,"VK_O")) return 0x4F;
	if (!stricmp(vkcode,"VK_P")) return 0x50;
	if (!stricmp(vkcode,"VK_Q")) return 0x51;
	if (!stricmp(vkcode,"VK_R")) return 0x52;
	if (!stricmp(vkcode,"VK_S")) return 0x53;
	if (!stricmp(vkcode,"VK_T")) return 0x54;
	if (!stricmp(vkcode,"VK_U")) return 0x55;
	if (!stricmp(vkcode,"VK_V")) return 0x56;
	if (!stricmp(vkcode,"VK_W")) return 0x57;
	if (!stricmp(vkcode,"VK_X")) return 0x58;
	if (!stricmp(vkcode,"VK_Y")) return 0x59;
	if (!stricmp(vkcode,"VK_Z")) return 0x5A;
	if (!stricmp(vkcode,"VK_LWIN")) return VK_LWIN;
	if (!stricmp(vkcode,"VK_RWIN")) return VK_RWIN;
	if (!stricmp(vkcode,"VK_APPS")) return VK_APPS;
	if (!stricmp(vkcode,"VK_NUMPAD0")) return VK_NUMPAD0;
	if (!stricmp(vkcode,"VK_NUMPAD1")) return VK_NUMPAD1;
	if (!stricmp(vkcode,"VK_NUMPAD2")) return VK_NUMPAD2;
	if (!stricmp(vkcode,"VK_NUMPAD3")) return VK_NUMPAD3;
	if (!stricmp(vkcode,"VK_NUMPAD4")) return VK_NUMPAD4;
	if (!stricmp(vkcode,"VK_NUMPAD5")) return VK_NUMPAD5;
	if (!stricmp(vkcode,"VK_NUMPAD6")) return VK_NUMPAD6;
	if (!stricmp(vkcode,"VK_NUMPAD7")) return VK_NUMPAD7;
	if (!stricmp(vkcode,"VK_NUMPAD8")) return VK_NUMPAD8;
	if (!stricmp(vkcode,"VK_NUMPAD9")) return VK_NUMPAD9;
	if (!stricmp(vkcode,"VK_MULTIPLY")) return VK_MULTIPLY;
	if (!stricmp(vkcode,"VK_ADD")) return VK_ADD;
	if (!stricmp(vkcode,"VK_SEPARATOR")) return VK_SEPARATOR;
	if (!stricmp(vkcode,"VK_SUBTRACT")) return VK_SUBTRACT;
	if (!stricmp(vkcode,"VK_DECIMAL")) return VK_DECIMAL;
	if (!stricmp(vkcode,"VK_DIVIDE")) return VK_DIVIDE;
	if (!stricmp(vkcode,"VK_F1")) return VK_F1;
	if (!stricmp(vkcode,"VK_F2")) return VK_F2;
	if (!stricmp(vkcode,"VK_F3")) return VK_F3;
	if (!stricmp(vkcode,"VK_F4")) return VK_F4;
	if (!stricmp(vkcode,"VK_F5")) return VK_F5;
	if (!stricmp(vkcode,"VK_F6")) return VK_F6;
	if (!stricmp(vkcode,"VK_F7")) return VK_F7;
	if (!stricmp(vkcode,"VK_F8")) return VK_F8;
	if (!stricmp(vkcode,"VK_F9")) return VK_F9;
	if (!stricmp(vkcode,"VK_F10")) return VK_F10;
	if (!stricmp(vkcode,"VK_F11")) return VK_F11;
	if (!stricmp(vkcode,"VK_F12")) return VK_F12;
	if (!stricmp(vkcode,"VK_F13")) return VK_F13;
	if (!stricmp(vkcode,"VK_F14")) return VK_F14;
	if (!stricmp(vkcode,"VK_F15")) return VK_F15;
	if (!stricmp(vkcode,"VK_F16")) return VK_F16;
	if (!stricmp(vkcode,"VK_F17")) return VK_F17;
	if (!stricmp(vkcode,"VK_F18")) return VK_F18;
	if (!stricmp(vkcode,"VK_F19")) return VK_F19;
	if (!stricmp(vkcode,"VK_F20")) return VK_F20;
	if (!stricmp(vkcode,"VK_F21")) return VK_F21;
	if (!stricmp(vkcode,"VK_F22")) return VK_F22;
	if (!stricmp(vkcode,"VK_F23")) return VK_F23;
	if (!stricmp(vkcode,"VK_F24")) return VK_F24;
	if (!stricmp(vkcode,"VK_NUMLOCK")) return VK_NUMLOCK;
	if (!stricmp(vkcode,"VK_SCROLL")) return VK_SCROLL;
	if (!stricmp(vkcode,"VK_LSHIFT")) return VK_LSHIFT;
	if (!stricmp(vkcode,"VK_RSHIFT")) return VK_RSHIFT;
	if (!stricmp(vkcode,"VK_LCONTROL")) return VK_LCONTROL;
	if (!stricmp(vkcode,"VK_RCONTROL")) return VK_RCONTROL;
	if (!stricmp(vkcode,"VK_LMENU")) return VK_LMENU;
	if (!stricmp(vkcode,"VK_RMENU")) return VK_RMENU;

	return	atol(vkcode);
}









