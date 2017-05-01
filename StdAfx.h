/****************************************************************************************/
/*  StdAfx.h                                                                            */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax                                    */
/*  Description:  Genesis world editor header file                                      */
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
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#ifndef STDAFX_H
#define STDAFX_H

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

/*
  Disable warnings for Windows headers
	4201 - nonstandard extension used : nameless struct/union
	4214 - nonstandard extension used : bit field types other than int
	4115 - 'type' : named type definition in parentheses
	4514 - unreferenced inline/local function has been removed
*/
#pragma warning(disable : 4201 4214 4115 4514)

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#if _MFC_VER>0x0600 // only MFC70
#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxtempl.h>		// Support for templates.
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxole.h>         // MFC OLE classes


#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <MMSystem.h>
#include <objbase.h>
#include <strmif.h>
#include <control.h>
#include <evcode.h>
#include <uuids.h>
#if _MFC_VER>0x0600 // only MFC70
#include <afxdhtml.h>
#endif
#include <afx.h>

// re-enable warnings except 4514
#pragma warning(default : 4201 4214 4115)

#endif
