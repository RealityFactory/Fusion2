# Microsoft Developer Studio Project File - Name="GEditPro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GEditPro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GEditPro.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GEditPro.mak" CFG="GEditPro - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GEditPro - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GEditPro - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GEditPro - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I ".\ColorSelector" /I ".\DirectInput" /I ".\mp3Mgr" /I ".\PathSelector" /I ".\TypeParser" /I ".\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "STRICT" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x1009 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib dxguid.lib dinput.lib dinput8.lib strmiids.lib shlwapi.lib genesis.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"RFEditPro.exe"

!ELSEIF  "$(CFG)" == "GEditPro - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\ColorSelector" /I ".\DirectInput" /I ".\mp3Mgr" /I ".\PathSelector" /I ".\TypeParser" /I ".\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib dxguid.lib dinput.lib dinput8.lib strmiids.lib shlwapi.lib genesisd.lib /nologo /subsystem:windows /debug /machine:I386 /out:"RFEditPro_Dbg.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GEditPro - Win32 Release"
# Name "GEditPro - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ColorSelector"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ColorSelector\ColorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorSelector\ColorButton.h
# End Source File
# Begin Source File

SOURCE=.\ColorSelector\ColorPopup.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorSelector\ColorPopup.h
# End Source File
# End Group
# Begin Group "DirectInput"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DirectInput\DirectInputMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectInput\DirectInputMgr.h
# End Source File
# End Group
# Begin Group "PathSelector"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PathSelector\BmpData.h
# End Source File
# Begin Source File

SOURCE=.\PathSelector\listboxex.cpp
# End Source File
# Begin Source File

SOURCE=.\PathSelector\listboxex.h
# End Source File
# Begin Source File

SOURCE=.\PathSelector\SHBrowseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PathSelector\SHBrowseDlg.h
# End Source File
# End Group
# Begin Group "TypeParser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TypeParser\cparser.c
# End Source File
# Begin Source File

SOURCE=.\TypeParser\cparser.h
# End Source File
# Begin Source File

SOURCE=.\TypeParser\cscanner.c
# End Source File
# Begin Source File

SOURCE=.\TypeParser\cscanner.h
# End Source File
# Begin Source File

SOURCE=.\TypeParser\hash.c
# End Source File
# Begin Source File

SOURCE=.\TypeParser\hash.h
# End Source File
# Begin Source File

SOURCE=.\TypeParser\iden.c
# End Source File
# Begin Source File

SOURCE=.\TypeParser\iden.h
# End Source File
# Begin Source File

SOURCE=.\Parse3dt.c
# End Source File
# Begin Source File

SOURCE=.\Parse3dt.h
# End Source File
# Begin Source File

SOURCE=.\TypeParser\scanner.c
# End Source File
# Begin Source File

SOURCE=.\TypeParser\scanner.h
# End Source File
# Begin Source File

SOURCE=.\TypeParser\symtab.c
# End Source File
# Begin Source File

SOURCE=.\TypeParser\symtab.h
# End Source File
# Begin Source File

SOURCE=.\TypeParser\type.c
# End Source File
# Begin Source File

SOURCE=.\TypeParser\type.h
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\about.cpp
# End Source File
# Begin Source File

SOURCE=.\about.h
# End Source File
# Begin Source File

SOURCE=.\BrushAttributesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BrushAttributesDialog.h
# End Source File
# Begin Source File

SOURCE=.\CompileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CompileDialog.h
# End Source File
# Begin Source File

SOURCE=.\CreateArchDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateArchDialog.h
# End Source File
# Begin Source File

SOURCE=.\CreateBoxDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateBoxDialog.h
# End Source File
# Begin Source File

SOURCE=.\CreateConeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateConeDialog.h
# End Source File
# Begin Source File

SOURCE=.\CreateCylDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateCylDialog.h
# End Source File
# Begin Source File

SOURCE=.\CreateSpheroidDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateSpheroidDialog.h
# End Source File
# Begin Source File

SOURCE=.\CreateStaircaseDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateStaircaseDialog.h
# End Source File
# Begin Source File

SOURCE=.\DialogPrintView.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogPrintView.h
# End Source File
# Begin Source File

SOURCE=.\EntitiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EntitiesDialog.h
# End Source File
# Begin Source File

SOURCE=.\EntityVisDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EntityVisDlg.h
# End Source File
# Begin Source File

SOURCE=.\Export3dsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Export3dsDialog.h
# End Source File
# Begin Source File

SOURCE=.\FaceAttributesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FaceAttributesDialog.h
# End Source File
# Begin Source File

SOURCE=.\GridSizeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GridSizeDialog.h
# End Source File
# Begin Source File

SOURCE=.\GroupsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupsDlg.h
# End Source File
# Begin Source File

SOURCE=.\KeyEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModelsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelsDlg.h
# End Source File
# Begin Source File

SOURCE=.\PluginDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginDialog.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesDialog.h
# End Source File
# Begin Source File

SOURCE=.\QuickViewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickViewDlg.h
# End Source File
# Begin Source File

SOURCE=.\SkyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SkyDialog.h
# End Source File
# Begin Source File

SOURCE=.\TemplatesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplatesDlg.h
# End Source File
# Begin Source File

SOURCE=.\TextureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureDlg.h
# End Source File
# Begin Source File

SOURCE=.\TextureListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureListBox.h
# End Source File
# Begin Source File

SOURCE=.\TipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TipDlg.h
# End Source File
# Begin Source File

SOURCE=.\ViewMgrDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewMgrDlg.h
# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FlatSplitterWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FlatSplitterWnd.h
# End Source File
# Begin Source File

SOURCE=.\Memdc.cpp
# End Source File
# Begin Source File

SOURCE=.\Memdc.h
# End Source File
# Begin Source File

SOURCE=.\include\tObjectArray.h
# End Source File
# Begin Source File

SOURCE=.\include\tObjectList.h
# End Source File
# Begin Source File

SOURCE=.\flat_combo\XTFlatComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\flat_combo\XTFlatComboBox.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\GEditPro.cpp
# End Source File
# Begin Source File

SOURCE=.\GEditPro.h
# End Source File
# Begin Source File

SOURCE=.\GEditPro.rc
# End Source File
# Begin Source File

SOURCE=.\GEditProDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\GEditProDoc.h
# End Source File
# Begin Source File

SOURCE=.\GEditProView.cpp
# End Source File
# Begin Source File

SOURCE=.\GEditProView.h
# End Source File
# Begin Source File

SOURCE=.\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\LevelOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\LevelOptions.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Prefs.c
# End Source File
# Begin Source File

SOURCE=.\prefs.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WadFile.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\arch.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrow.cur
# End Source File
# Begin Source File

SOURCE=.\res\camera.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cameracur.cur
# End Source File
# Begin Source File

SOURCE=.\res\cone.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Cross_m.cur
# End Source File
# Begin Source File

SOURCE=.\res\cube.cur
# End Source File
# Begin Source File

SOURCE=.\res\cube_ico.ico
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00003.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\CursorPlus.CUR
# End Source File
# Begin Source File

SOURCE=.\res\cylinder.bmp
# End Source File
# Begin Source File

SOURCE=.\res\default.bmp
# End Source File
# Begin Source File

SOURCE=res\designer3D_splash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Dragcopy.cur
# End Source File
# Begin Source File

SOURCE=.\res\Dragmove.cur
# End Source File
# Begin Source File

SOURCE=.\res\eyedropper.cur
# End Source File
# Begin Source File

SOURCE=.\res\ffend.ico
# End Source File
# Begin Source File

SOURCE=.\res\ffframe.ico
# End Source File
# Begin Source File

SOURCE=.\res\g3dsplash.bmp
# End Source File
# Begin Source File

SOURCE=res\gedit_pro_splash.bmp
# End Source File
# Begin Source File

SOURCE=res\GEditPro.ico
# End Source File
# Begin Source File

SOURCE=res\GEditPro.manifest
# End Source File
# Begin Source File

SOURCE=res\GEditProDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\groupbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\handleco.bmp
# End Source File
# Begin Source File

SOURCE=.\res\handleedge.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hollowbo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hollowsp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\idb_.bmp
# End Source File
# Begin Source File

SOURCE=.\res\libobj.ico
# End Source File
# Begin Source File

SOURCE=.\res\light.bmp
# End Source File
# Begin Source File

SOURCE=.\res\light.ico
# End Source File
# Begin Source File

SOURCE=.\res\litebulb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mode.bmp
# End Source File
# Begin Source File

SOURCE=.\res\modelorg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Move4way.cur
# End Source File
# Begin Source File

SOURCE=.\res\playstart.ico
# End Source File
# Begin Source File

SOURCE=.\res\plugintext.bin
# End Source File
# Begin Source File

SOURCE=.\res\pointer.cur
# End Source File
# Begin Source File

SOURCE=.\res\protogame4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\quickbeep.wav
# End Source File
# Begin Source File

SOURCE=.\res\Ran_arrow.cur
# End Source File
# Begin Source File

SOURCE=.\res\Ran_no.ico
# End Source File
# Begin Source File

SOURCE=.\res\rf_edit_pro.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Rotate.cur
# End Source File
# Begin Source File

SOURCE=.\res\rotatebl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rotatebr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rotatetl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rotatetr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\RotationCenter.cur
# End Source File
# Begin Source File

SOURCE=.\res\rrstart.ico
# End Source File
# Begin Source File

SOURCE=.\res\rt_manif.bin
# End Source File
# Begin Source File

SOURCE=.\res\shearlr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ShearLR.cur
# End Source File
# Begin Source File

SOURCE=.\res\sheartb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ShearTB.cur
# End Source File
# Begin Source File

SOURCE=.\res\showvisi.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sky_icon.ico
# End Source File
# Begin Source File

SOURCE=.\res\skybitmap.bmp
# End Source File
# Begin Source File

SOURCE=.\res\solidbox.bmp
# End Source File
# Begin Source File

SOURCE=.\res\solidsph.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Speciale.cur
# End Source File
# Begin Source File

SOURCE=.\res\spotlight.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stairs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stop.ico
# End Source File
# Begin Source File

SOURCE=.\res\sunentity.bmp
# End Source File
# Begin Source File

SOURCE=.\res\svertex.bmp
# End Source File
# Begin Source File

SOURCE=..\..\tStudioWork\tBiteShield_icon_32.bmp
# End Source File
# Begin Source File

SOURCE=res\tDesigner3D.ico
# End Source File
# Begin Source File

SOURCE=.\res\tDesigner3D_splash.bmp
# End Source File
# Begin Source File

SOURCE=res\tDesigner3DDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\tdesigner_woosh.wav
# End Source File
# Begin Source File

SOURCE=.\res\tedit_splash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\template.bmp
# End Source File
# Begin Source File

SOURCE=.\res\text1.bin
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\vertex.bmp
# End Source File
# Begin Source File

SOURCE=.\res\view_tool.bmp
# End Source File
# Begin Source File

SOURCE=res\wave1.bin
# End Source File
# Begin Source File

SOURCE=.\res\Whoosh.wav
# End Source File
# End Group
# Begin Group "Variable Management"

# PROP Default_Filter ""
# Begin Source File

SOURCE=GEditProDocVars.cpp
# End Source File
# Begin Source File

SOURCE=GEditProDocVars.h
# End Source File
# Begin Source File

SOURCE=GEditProUpdateMgr.cpp
# End Source File
# Begin Source File

SOURCE=GEditProUpdateMgr.h
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActivationWatch.cpp
# End Source File
# Begin Source File

SOURCE=.\ActivationWatch.h
# End Source File
# Begin Source File

SOURCE=.\array.c
# End Source File
# Begin Source File

SOURCE=.\array.h
# End Source File
# Begin Source File

SOURCE=.\box3d.c
# End Source File
# Begin Source File

SOURCE=.\box3d.h
# End Source File
# Begin Source File

SOURCE=.\brush.cpp
# End Source File
# Begin Source File

SOURCE=.\brush.h
# End Source File
# Begin Source File

SOURCE=.\BrushTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\BrushTemplate.h
# End Source File
# Begin Source File

SOURCE=.\Compiler.cpp
# End Source File
# Begin Source File

SOURCE=.\Compiler.h
# End Source File
# Begin Source File

SOURCE=.\Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\Entity.h
# End Source File
# Begin Source File

SOURCE=.\EntityTable.cpp
# End Source File
# Begin Source File

SOURCE=.\EntityTable.h
# End Source File
# Begin Source File

SOURCE=.\EntTypeName.c
# End Source File
# Begin Source File

SOURCE=.\EntTypeName.h
# End Source File
# Begin Source File

SOURCE=.\EntView.cpp
# End Source File
# Begin Source File

SOURCE=.\EntView.h
# End Source File
# Begin Source File

SOURCE=.\face.cpp
# End Source File
# Begin Source File

SOURCE=.\face.h
# End Source File
# Begin Source File

SOURCE=.\FaceList.cpp
# End Source File
# Begin Source File

SOURCE=.\facelist.h
# End Source File
# Begin Source File

SOURCE=.\FilePath.c
# End Source File
# Begin Source File

SOURCE=.\FilePath.h
# End Source File
# Begin Source File

SOURCE=.\Gbsplib.h
# End Source File
# Begin Source File

SOURCE=.\group.cpp
# End Source File
# Begin Source File

SOURCE=.\group.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceCtrls.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceCtrls.h
# End Source File
# Begin Source File

SOURCE=level.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=level.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\List.c
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\model.cpp
# End Source File
# Begin Source File

SOURCE=.\model.h
# End Source File
# Begin Source File

SOURCE=.\Mydef.h
# End Source File
# Begin Source File

SOURCE=.\node.cpp
# End Source File
# Begin Source File

SOURCE=.\node.h
# End Source File
# Begin Source File

SOURCE=.\render.cpp
# End Source File
# Begin Source File

SOURCE=.\render.h
# End Source File
# Begin Source File

SOURCE=SelBrushList.cpp
# End Source File
# Begin Source File

SOURCE=.\SelBrushList.h
# End Source File
# Begin Source File

SOURCE=.\SelFaceList.c
# End Source File
# Begin Source File

SOURCE=.\SelFaceList.h
# End Source File
# Begin Source File

SOURCE=.\stack.c
# End Source File
# Begin Source File

SOURCE=.\stack.h
# End Source File
# Begin Source File

SOURCE=tLevel.cpp
# End Source File
# Begin Source File

SOURCE=tLevel.h
# End Source File
# Begin Source File

SOURCE=tLevelMgr.cpp
# End Source File
# Begin Source File

SOURCE=tLevelMgr.h
# End Source File
# Begin Source File

SOURCE=.\typeio.c
# End Source File
# Begin Source File

SOURCE=.\typeio.h
# End Source File
# Begin Source File

SOURCE=.\undostack.cpp
# End Source File
# Begin Source File

SOURCE=.\undostack.h
# End Source File
# Begin Source File

SOURCE=.\units.h
# End Source File
# Begin Source File

SOURCE=.\util.c
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\WadFile.h
# End Source File
# End Group
# End Target
# End Project
