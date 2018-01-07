/****************************************************************************************/
/*  tEditView.cpp                                                                      */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax, John Moore                        */
/*  Description:  MFC view stuff...  Alot of the editor UI functionality is here        */
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
/*Genesis3D Version 1.1 released November 15, 1999                            */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/
#include "stdafx.h"
#include "Globals.h"
#include "KeyEditDlg.h"
#include "GEditProView.h"
#include <assert.h>
#include "units.h"
#include "face.h"
#include "node.h"
#include "ram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
#define AXIS_X	0x1
#define AXIS_Y	0x2
#define AXIS_Z	0x4

#define MAX_PIXEL_DRAG_START_DIST (12.0f)

int CGEditProView::mCY_DRAG = 2 ;
int CGEditProView::mCX_DRAG = 2 ;

IMPLEMENT_DYNCREATE(CGEditProView, CView)

BEGIN_MESSAGE_MAP(CGEditProView, CView)
	ON_MESSAGE(WM_USER_COMPILE_MSG, OnCompileMessage)
	ON_MESSAGE(WM_USER_COMPILE_ERR, OnCompileError)
	ON_MESSAGE(WM_USER_COMPILE_DONE, OnCompileDone)
	//{{AFX_MSG_MAP(CGEditProView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_TOOLS_CAMERA, OnToolsCamera)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CAMERA, OnUpdateToolsCamera)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_TOOLS_BRUSH_MOVEROTATEBRUSH, OnToolsBrushMoverotatebrush)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_MOVEROTATEBRUSH, OnUpdateToolsBrushMoverotatebrush)
	ON_COMMAND(ID_TOOLS_BRUSH_SCALEBRUSH, OnToolsBrushScalebrush)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_SCALEBRUSH, OnUpdateToolsBrushScalebrush)
	ON_COMMAND(ID_TOOLS_BRUSH_SHOWBRUSH, OnToolsBrushShowbrush)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_SHOWBRUSH, OnUpdateToolsBrushShowbrush)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_TOOLS_BRUSH_SHEARBRUSH, OnToolsBrushShearbrush)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_SHEARBRUSH, OnUpdateToolsBrushShearbrush)
	ON_UPDATE_COMMAND_UI(ID_GROUPS_MAKENEWGROUP, OnUpdateBrushGroupsMakenewgroup)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_GROUPS_ADDTOGROUP, OnUpdateBrushGroupsAddtogroup)
	ON_COMMAND(ID_TOOLS_BRUSH_ROTATE90, OnToolsBrushRotate45)
	ON_COMMAND(ID_TOOLS_BRUSH_MOVESELECTEDBRUSHES, OnToolsBrushMoveselectedbrushes)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_MOVESELECTEDBRUSHES, OnUpdateToolsBrushMoveselectedbrushes)
	ON_COMMAND(ID_TOOLS_TEMPLATE, OnToolsTemplate)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TEMPLATE, OnUpdateToolsTemplate)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_ROTATE90, OnUpdateToolsBrushRotate45)
	ON_COMMAND(ID_BRUSH_REMOVESELECTEDFROMGROUP, OnBrushRemoveselectedfromgroup)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_REMOVESELECTEDFROMGROUP, OnUpdateBrushRemoveselectedfromgroup)
	ON_COMMAND(ID_BRUSH_GROUPS_ADDTOGROUP, OnBrushGroupsAddtogroup)
	ON_COMMAND(ID_DESELECTALL, OnDeselectall)
	ON_UPDATE_COMMAND_UI(ID_DESELECTALL, OnUpdateDeselectall)
	ON_COMMAND(ID_SELECTALL, OnSelectall)
	ON_UPDATE_COMMAND_UI(ID_SELECTALL, OnUpdateSelectall)
	ON_COMMAND(ID_TOOLS_SCALEWORLD, OnToolsScaleworld)
	ON_COMMAND(ID_TOOLS_BRUSH_MAKENEWEST, OnToolsBrushMakenewest)
	ON_COMMAND(ID_TOOLS_SETTEXTURESCALE, OnToolsSettexturescale)
	ON_COMMAND(ID_TOOLS_NEXTBRUSH, OnToolsNextbrush)
	ON_COMMAND(ID_TOOLS_PREVBRUSH, OnToolsPrevbrush)
	ON_COMMAND(ID_TOOLS_ADDTOLEVEL, OnToolsAddtolevel)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ADDTOLEVEL, OnUpdateToolsAddtolevel)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomout)
	ON_COMMAND(ID_CENTERTHING, OnCenterthing)
	ON_UPDATE_COMMAND_UI(ID_CENTERTHING, OnUpdateCenterthing)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TOOLS_BRUSH_ROTATEBRUSH, OnToolsBrushRotatebrush)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_ROTATEBRUSH, OnUpdateToolsBrushRotatebrush)
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()	
	ON_WM_MBUTTONDOWN()	
	ON_WM_MBUTTONUP()	
	ON_COMMAND(IDC_BTN_REBUILD_COMMAND, OnButtonRebuildShow)//	post 0.57
	ON_COMMAND(IDC_BTN_REBUILD_HIDE, OnButtonRebuildHide)//	post 0.57
	ON_BN_CLICKED(IDC_BTN_VIEW_REBUILD, OnButtonRebuild)//	post 0.57

	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE( ID_VIEW_3DWIREFRAME, ID_VIEW_TEXTUREVIEW, OnViewType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_3DWIREFRAME, ID_VIEW_TEXTUREVIEW, OnViewTypeCmdUi)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview/*CView::OnFilePrintPreview*/)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
END_MESSAGE_MAP()


#define SIDE_LEFT 1
#define SIDE_RIGHT 2
#define SIDE_TOP 4
#define SIDE_BOTTOM 8


////////////////////////////////////////////////////////////////////////////////
//	OnEditCopy()
//
////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnEditCopy()
{
	int				i;
	int				iNumSelBrushes;
	CEntity			tempEntity;
	CGEditProDoc	*pDoc = NULL;
	pDoc = GetDocument();

	if (pDoc)
	{
		pDoc->TempCopySelectedBrushes();
		CGEditProDocVars	tempCopyDocVars, *pTempCurrentDocVars = NULL;
		pTempCurrentDocVars = pDoc->DocVarsGetPointer();
		ModelList* pTempModelList = NULL;

		if (pTempCurrentDocVars)
		{
			tempCopyDocVars.m_pTempSelBrushes = SelBrushList_Clone(pTempCurrentDocVars->m_pTempSelBrushes);
			tempCopyDocVars.m_Level.m_pEntityDefs = EntityTable_Clone (pTempCurrentDocVars->m_Level.m_pEntityDefs);
			tempCopyDocVars.m_Level.m_pEntities = tempEntity.CloneEntityArray(pTempCurrentDocVars->m_Level.m_pEntities);

			pTempModelList = ModelList_Clone(pTempCurrentDocVars->m_Level.m_ModelInfo.Models);

			tempCopyDocVars.m_Level.m_ModelInfo.Models = ModelList_Create();
			if (tempCopyDocVars.m_Level.m_ModelInfo.Models && pTempModelList)
			{
				int iModelCount = ModelList_GetCount(pTempModelList);
				if (iModelCount >=1)
				{
					ModelIterator	mi = NULL;
					ListIterator li;

					Model* pOldModel = NULL;

					pOldModel = ModelList_GetFirst(pTempModelList, &mi);
					while (pOldModel)
					{
						if (Model_IsSelected(pOldModel, pTempCurrentDocVars->m_pSelBrushes, pTempCurrentDocVars->m_Level.m_pBrushes))
						{
							ModelList_AddModel(tempCopyDocVars.m_Level.m_ModelInfo.Models, pOldModel);		
						}
						if (mi)
							pOldModel = ModelList_GetNext(pTempModelList, &mi);
					}
				}
			}

			iNumSelBrushes = SelBrushList_GetSize(tempCopyDocVars.m_pTempSelBrushes);

			//	create a file type that we will be able to dump to memory
			CSharedFile	clipboardFile(GMEM_MOVEABLE|GMEM_ZEROINIT);

			//	now, create an OleDataSource that will take the memory bloc
			//	and place it onto the clipboard (later)
			COleDataSource*	pSource = NULL;
			pSource = new COleDataSource();

			clipboardFile.Write(&tempCopyDocVars, sizeof (tempCopyDocVars)); // You can write to the clipboard as you would to any CFile

			HGLOBAL hMem = NULL;
			hMem = clipboardFile.Detach();
			if (!hMem) return;
			((COleDataSource*)pSource)->CacheGlobalData(((CGEditProApp*)AfxGetApp())->GetClipboardFormat(), hMem);
			((COleDataSource*)pSource)->SetClipboard();

			pDoc->TempDeleteSelected(pTempCurrentDocVars);
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//	OnEditPaste
//
////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnEditPaste()
{
	int					iNumSelBrushes = 0;
	bool				bEntitySelected = false;
	geVec3d				vecOffset;
	CGEditProDocVars	tempPasteDocVars,	thisFunctionDocVars, *pTempCurrentDocVars = NULL;
	CGEditProDoc		*pDoc = NULL;
	pDoc = GetDocument();

	float fOffset = 40.0f;
	vecOffset.X =
		vecOffset.Y =
		vecOffset.Z = fOffset;

	//	create an OleDataObject for getting clipboard data	
	COleDataObject	obj;

	if (pDoc)
	{
		//	clear out "selected" states for all brushes and entities
		pDoc->ResetAllSelections();

		thisFunctionDocVars = pDoc->DocVarsCreateUniqueCopy(pDoc->DocVarsGetByValue());
		pTempCurrentDocVars = pDoc->DocVarsGetPointer();

		//	try to connect to the clipboard
		if (obj.AttachClipboard())
		{
			//	check to see if the clipboard's contents match this app's format
			if (obj.IsDataAvailable(((CGEditProApp*)AfxGetApp())->GetClipboardFormat()))
			{
				//	stream the clipboard data into a memory var
				HGLOBAL hmem = NULL;
				hmem = obj.GetGlobalData(((CGEditProApp*)AfxGetApp())->GetClipboardFormat());

				if (hmem)
				{
					//	setup a shared file for getting data from the memory var
					//	and put the data from the memory var into the shared file
					CMemFile clipboardFile((BYTE*)GlobalLock(hmem), GlobalSize(hmem));

					//	the trick here is to NOT OVER-read the clipboardFile.
					//	IF you do, you will get an access violation error upon completion
					//	of this OnEditPaste() function because of trying to read 
					//	empty and undefined memory.
					clipboardFile.Read((BYTE*)&tempPasteDocVars, sizeof (CGEditProDocVars));

					if (hmem)
					{
						if (!GlobalUnlock(hmem))
						{
							;;
#ifdef _DEBUG
							AfxMessageBox("Failed to GlobalUnlock(hmem)");
#endif
						}
						if (!GlobalFree(hmem))
						{
							;;
#ifdef _DEBUG
							AfxMessageBox("Failed to GlobalFree(hmem)");
#endif
						}
					}	//	if (hmem)...

					CString		strModelName;
					Model		*pOldModel = NULL;
					Model		*pNewModel = NULL;
					char const	*pcharName = NULL;
					int			iCopyNum;
					CString		strTag;

					//	FIRST, MODELS AND THEIR ASSOCIATED BRUSHES
					if (tempPasteDocVars.m_Level.m_ModelInfo.Models)
					{
						int iModelCount = ModelList_GetCount(tempPasteDocVars.m_Level.m_ModelInfo.Models);

						if (iModelCount >=1)
						{
							ModelIterator	mi;

							pOldModel = ModelList_GetFirst(tempPasteDocVars.m_Level.m_ModelInfo.Models, &mi);
							while (pOldModel)
							{
								// Create default unique name.
								pcharName = Model_GetName (pOldModel);
								if (pcharName == NULL)
								{
									pcharName = "Model";
								}

								iCopyNum = 0;

								CString strTemp = pcharName;

								strModelName = pcharName;
								while (ModelList_FindByName (pTempCurrentDocVars->m_Level.m_ModelInfo.Models, strModelName))
								{
									int iColonLoc222 = strModelName.ReverseFind(':');					
									if (iColonLoc222 >0)
									{
										char	charEndNumber[20] = "";
										CString strEndNumber = _T("");
										int		iEndNumber = 0;

										//	copy the number from the end of string
										strEndNumber = strModelName.Mid(iColonLoc222+1);
										//	trim the end of the string, just in case
										strEndNumber.TrimRight();
										//	delete the number and everything else from the end of the string
										strModelName.Delete(iColonLoc222+1, strModelName.GetLength() - (iColonLoc222+1));
										//	convert the characters representing the number int an integer
										iEndNumber = atoi(strEndNumber.GetBuffer(16));
										//	increment the integer
										iEndNumber++;
										//	now convert the integer back into characters
										_itoa(iEndNumber, charEndNumber, 10);
										//	now add the new number to the end of the string
										strModelName +=charEndNumber;
									}
									else	//	we need to add colons and the number 2. 
									{
										CString strColonsPlus2 = _T("::2");
										//	add a fresh pair of colons and the number 2
										strModelName +=strColonsPlus2;
									}
								}

								pcharName = strModelName.GetBuffer(strModelName.GetLength());
								Model_SetName(pOldModel, pcharName);
								pNewModel = Model_Clone(pOldModel);
								int iModelID = Model_GetId(pNewModel);
								int iNewID = iModelID;

								while (
									(ModelList_FindById (pTempCurrentDocVars->m_Level.m_ModelInfo.Models, iNewID))
									||	
									(ModelList_FindById (thisFunctionDocVars.m_Level.m_ModelInfo.Models, iNewID))
									)
								{
									iNewID++;
								}

								//	we are using the m_pTempSelBrushes list because the brushes
								//	in this list are clones of the selected brushes, not merely
								//	a list of pointers to the main brush list
								if (tempPasteDocVars.m_pTempSelBrushes)
								{
									iNumSelBrushes = SelBrushList_GetSize (tempPasteDocVars.m_pTempSelBrushes);

									if(iNumSelBrushes > 0)
									{
										// create copies / clones of the selected brushes
										for(int i=0;i < iNumSelBrushes;i++)
										{
											// clone the brush, add copy to level,
											// add copy to select list
											Brush *pBrush = NULL;
											Brush *pClone = NULL;

											pBrush = SelBrushList_GetBrush (tempPasteDocVars.m_pTempSelBrushes, i);

											if (pBrush)
											{
												if (Brush_GetModelId(pBrush) == iModelID)
												{
													Model_SetId(pNewModel, iNewID);
													Model_SetName(pNewModel, pcharName);

													//	increment the offset for the brush that's on
													//	the clipboard
													Brush_Move (pBrush, &vecOffset);

													pClone = Brush_Clone (pBrush);
													if (pClone)
													{
														Brush_SetModelId (pClone, iNewID);

														//	set to the current group in case the 
														//	pasted object's "ORIGINAL" group (from another doc)
														//	doesn't exist in THIS document
														pClone->GroupId = pDoc->m_iCurrentGroup;
														pDoc->m_pLevelMgr->AppendBrush (&thisFunctionDocVars.m_Level, pClone);
														SelBrushList_Add (thisFunctionDocVars.m_pSelBrushes, pClone);
													}
												}
											}
										}
									}
								}

								ModelList_AddModel(thisFunctionDocVars.m_Level.m_ModelInfo.Models, pNewModel);
								pOldModel = ModelList_GetNext(tempPasteDocVars.m_Level.m_ModelInfo.Models, &mi);
							}
						}
					}	//	if (tempPasteDocVars.m_Level.m_ModelInfo.Models)...


					//	NOW FOR BRUSHES NOT ASSOCIATED WITH MODELS...

					//	we are using the m_pTempSelBrushes list because the brushes
					//	in this list are clones of the selected brushes, not merely
					//	a list of pointers to the main brush list
					if (tempPasteDocVars.m_pTempSelBrushes)
					{
						iNumSelBrushes = SelBrushList_GetSize (tempPasteDocVars.m_pTempSelBrushes);

						if(iNumSelBrushes > 0)
						{
							// create copies / clones of the selected brushes
							for(int i=0;i < iNumSelBrushes;i++)
							{
								// clone the brush, add copy to level,
								// add copy to select list
								Brush *pBrush = NULL;
								Brush *pClone = NULL;

								pBrush = SelBrushList_GetBrush (tempPasteDocVars.m_pTempSelBrushes, i);

								if (pBrush)
								{
									if (Brush_GetModelId(pBrush) <= 0)
									{
										//	increment the offset for the brush that's on
										//	the clipboard
										Brush_Move (pBrush, &vecOffset);

										pClone = Brush_Clone (pBrush);
										if (pClone)
										{
											//	set to the current group in case the 
											//	pasted object's "ORIGINAL" group (from another doc)
											//	doesn't exist in THIS document
											pClone->GroupId = pDoc->m_iCurrentGroup;
											pDoc->m_pLevelMgr->AppendBrush (&thisFunctionDocVars.m_Level, pClone);
											SelBrushList_Add (thisFunctionDocVars.m_pSelBrushes, pClone);
										}
									}
								}	// if(iNumSelBrushes > 0)...
							}
						}
					}	//	if (tempPasteDocVars.m_pTempSelBrushes)...

					//	ENTITIES NOW...
					CEntity  TEnt;
					CEntityArray *Entities = NULL;
					int iCnt;

					//	now, deal with the entities
					Entities = tempPasteDocVars.m_Level.m_pEntities;
					if (Entities)
					{
						iCnt = Entities->GetSize() ;
						for( int i=0 ; i < iCnt; i++ )
						{
							CEntity *pEnt = NULL;
							pEnt = &(*Entities)[i];
							if (pEnt)
							{
								if( pEnt->IsCamera() == GE_FALSE  )	// Exclude Cameras
								{
									if (pEnt->IsSelected ())
									{
										CEntity WorkingEntity;
										int		Index ;

										//	increment the offset for the brush that's on
										//	the clipboard
										pEnt->SetOrigin(pEnt->mOrigin.X + fOffset, 
											pEnt->mOrigin.Y + fOffset,
											pEnt->mOrigin.Z + fOffset,
											thisFunctionDocVars.m_Level.m_pEntityDefs);

										//	set to the current group in case the 
										//	pasted object's "ORIGINAL" group (from another doc)
										//	doesn't exist in THIS document
										pEnt->SetGroupId(pDoc->m_iCurrentGroup);

										WorkingEntity = *pEnt;
										Index = pDoc->m_pLevelMgr->AddEntity (&thisFunctionDocVars.m_Level, WorkingEntity);
										pDoc->SelectEntity(&WorkingEntity);
										pDoc->m_iNumSelEntities++;
										pDoc->m_dwSelState &= ANYENTITY;
									}
								}
							}
						}
					}	//	if (Entities)...

					//	update the document
					pDoc->DocVarsUpdate(thisFunctionDocVars);
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					pDoc->UpdateAllViews( UAV_ALLVIEWS | REBUILD_QUICK, NULL );
					//pDoc->UpdateSelected();	//	calls updatemaincontrols

				}	//	if (hmem)...
			}
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//	OnUpdateEditCopy
//
////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	int					iNumSelBrushes;
	bool				bEntitySelected = false;
	CGEditProDocVars	*pTempCurrentDocVars = NULL;
	CGEditProDoc		*pDoc = NULL;
	pDoc = GetDocument();

	if (pDoc)
	{
		pTempCurrentDocVars = pDoc->DocVarsGetPointer();

		if (pTempCurrentDocVars)
		{
			iNumSelBrushes = SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes);

			//	if no brushes selected, check for selected entities
			if (iNumSelBrushes <= 0)	
			{
				CEntity  TEnt;
				CEntityArray *Entities = NULL;
				int iCnt;

				Entities = pDoc->m_pLevelMgr->GetEntities (&pTempCurrentDocVars->m_Level);
				if (Entities)
				{
					iCnt = Entities->GetSize() ;
					for(int i=0 ; i < iCnt; i++ )
					{
						CEntity *pEnt = &(*Entities)[i];
						if( pEnt->IsCamera() == GE_FALSE  )	// Exclude Cameras
						{
							if (pEnt->IsSelected ())
							{
								bEntitySelected = true;
								break;
							}
						}
					}
				}
			}

			if(iNumSelBrushes >0 || bEntitySelected)
			{
				pCmdUI->Enable();
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//	OnUpdateEditPaste
//
////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	//	create an OleDataObject for getting clipboard data	
		COleDataObject	obj;
	
		//	try to connect to the clipboard
		if (obj.AttachClipboard())
		{
			//	check to see if the clipboard's contents match this app's format
			if (obj.IsDataAvailable(((CGEditProApp*)AfxGetApp())->GetClipboardFormat()))
			{
				pCmdUI->Enable();
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
}




//	*************************************************************************
//	*************************************************************************
//	CGEditProView printing


//////////////////////////////////////////////////////////////////////////////////////
//	OnPrepareDC
//	
//////////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CString	thisFunctionName = "CGEditProView::OnPrepareDC";

	if (pInfo)
	{		
		CView::OnPrepareDC(pDC, pInfo);
		pDC->SetMapMode(MM_ISOTROPIC);
		pDC->SetWindowExt(1, 1);
		pDC->SetViewportExt(1, 1);
	}

}



//////////////////////////////////////////////////////////////////////////////////////
//	OnPrint
//	
//////////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CString	thisFunctionName = "CGEditProView::OnPrint";

		//	we are going to change the mapping mode to MM_ISOTROPIC so we can
		//	expand the size of the printed image

		//	get and store current settings
	int OldMapMode = pDC->GetMapMode();
	CSize oldViewportExt = pDC->GetViewportExt();
	CSize oldWindowExt = pDC->GetWindowExt();

		//	set mapping mode and extents to make the printed image bigger
	pDC->SetMapMode(MM_ISOTROPIC);
	pDC->SetWindowExt(1, 1);
	pDC->SetViewportExt(12,12);
	
		//	execute default printing with these settings
	CView::OnPrint(pDC, pInfo);

		//	restore mapping mode and extents 
	pDC->SetWindowExt(oldWindowExt);
	pDC->SetViewportExt(oldViewportExt);
	pDC->SetMapMode(OldMapMode);

}


//////////////////////////////////////////////////////////////////////////////////////
//	OnFilePrintPreview
//	
//////////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnFilePrintPreview() 
{
	CString	thisFunctionName = "CGEditProView::OnFilePrintPreview";

	
	CView::OnFilePrintPreview();
}



//////////////////////////////////////////////////////////////////////////////////////
//	OnPreparePrinting
//	
//////////////////////////////////////////////////////////////////////////////////////
BOOL CGEditProView::OnPreparePrinting(CPrintInfo* pInfo)
{
	CString	thisFunctionName = "CGEditProView::OnPreparePrinting";
	
	BOOL bResult;
	
	bResult = DoPreparePrinting(pInfo);
						// default preparation
	return bResult;
}

//////////////////////////////////////////////////////////////////////////////////////
//	OnBeginPrinting
//	
//////////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	CString	thisFunctionName = "Ct_EDIT_VIEW_ON_BEGIN_PRINTING";
	
	//	set the print / print preview flag to "true" in order to stop
	//	automatic view-switching during print ops
	CGEditProDoc *pDoc = NULL;
	pDoc = GetDocument();
	if (pDoc)
		pDoc->SetPrintingState(true);


// TODO: add extra initialization before printing



}

//////////////////////////////////////////////////////////////////////////////////////
//	OnEndPrinting
//	
//////////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	CString	thisFunctionName = "Ct_EDIT_VIEW_ON_END_PRINTING";

	// TODO: add cleanup after printing

	//	set the print / print preview flag to "false" in order to allow
	//	the resumption of automatic view-switching while print ops not happening	
	CGEditProDoc *pDoc = NULL;
	pDoc = GetDocument();
	if (pDoc)
		pDoc->SetPrintingState(false);
}








//	post 0.57	notifies user of need to rebuild
//	see pDoc::SelectRay for origin of message
void CGEditProView::OnButtonRebuildShow()
{
	if (mViewType == ID_VIEW_TEXTUREVIEW)
	{
		if (m_btnRebuild)
		{
			if (!m_btnRebuild.IsWindowVisible())
			{
				m_btnRebuild.ShowWindow(SW_SHOW);
				CGEditProDoc *pDoc = GetDocument();
				::PlaySound(MAKEINTRESOURCE(IDR_WAVE_QUICKBEEP), AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
			}
		}
	}
}

//	post 0.57	notifies user of need to rebuild
//	see pDoc::OngbspnoWater for origin of message
void CGEditProView::OnButtonRebuildHide()
{
	if (mViewType == ID_VIEW_TEXTUREVIEW)
	{
		if (m_btnRebuild)
		{
			if (m_btnRebuild.IsWindowVisible())
			{
				m_btnRebuild.ShowWindow(SW_HIDE);
			}
		}
	}
}


//	post 0.57
void CGEditProView::OnButtonRebuild()
{
	if (mViewType == ID_VIEW_TEXTUREVIEW)
	{
		CGEditProDoc *pDoc = GetDocument ();
		pDoc->UpdateAllViews( UAV_ALL3DVIEWS | REBUILD_QUICK, NULL, TRUE ); //post 0.57

		if (m_btnRebuild.IsWindowVisible())
		{
			m_btnRebuild.ShowWindow(SW_HIDE);
		}
		CGlobals::g_iRebuildClickCount = 0;
	}
}



void CGEditProView::OnUpdateCenterthing(CCmdUI* pCmdUI) 
{
	CGEditProDoc *pDoc = GetDocument ();

	if ((pDoc->m_iModeTool == ID_TOOLS_TEMPLATE) &&
	    ((mViewType == ID_VIEW_TOPVIEW) || (mViewType == ID_VIEW_SIDEVIEW) || (mViewType == ID_VIEW_FRONTVIEW)))
	{
		pCmdUI->Enable (TRUE);
	}
	else
	{
		pCmdUI->Enable (FALSE);
	}
}

// Center the template brush or entity in the selected view.
// Doesn't work for 3d views...
void CGEditProView::OnCenterthing()
{
	CGEditProDoc *pDoc = GetDocument ();
	// only works on templates
	if ((pDoc->m_iModeTool != ID_TOOLS_TEMPLATE) ||
	    ((mViewType != ID_VIEW_TOPVIEW) && (mViewType != ID_VIEW_SIDEVIEW) && (mViewType != ID_VIEW_FRONTVIEW)))
	{
		return;
	}

	geVec3d NewWorldPos = Render_GetViewCenter (VCam);

	// Get the current thing's position...
	geVec3d CurrentThingPos;

	if (pDoc->m_bTempEnt)
	{
		CurrentThingPos = pDoc->m_RegularEntity.mOrigin;
	}
	else
	{
		if (pDoc->m_pCurBrush == NULL)
		{
			return;
		}
		Brush_Center (pDoc->m_pCurBrush, &CurrentThingPos);
	}

	// Compute delta required to get thing to NewWorldPos.
	// One dimension won't be changed (i.e. in the top view, the Y won't be modified)
	geVec3d MoveDelta;

	geVec3d_Subtract (&NewWorldPos, &CurrentThingPos, &MoveDelta);

	switch (mViewType)
	{
		case ID_VIEW_TOPVIEW :
			MoveDelta.Y = 0.0f;
			break;

		case ID_VIEW_SIDEVIEW :
			MoveDelta.X = 0.0f;
			break;

		case ID_VIEW_FRONTVIEW :
			MoveDelta.Z = 0.0f;
			break;

		default :
			// don't do nothin!
			assert (0);
	}

	// We've computed the delta, so move the thing...
	pDoc->MoveTemplateBrush (&MoveDelta);

	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );
}

static geBoolean IsKeyDown (int Key)
{
	int KeyState;

	KeyState = ::GetAsyncKeyState (Key);
	return (KeyState & 0x8000);
}

// Prevent axis movement.  View space clipping.
void CGEditProView::LockAxisView (int *dx, int *dy)
{
	CGEditProDoc *pDoc = GetDocument ();
	int mLockAxis = pDoc->GetLockAxis ();

	switch (mViewType)
	{
		case ID_VIEW_TOPVIEW :
			if (mLockAxis & AXIS_X) *dx = 0;
			if (mLockAxis & AXIS_Z) *dy = 0;
			break;

		case ID_VIEW_SIDEVIEW :
			if (mLockAxis & AXIS_X) *dx = 0;
			if (mLockAxis & AXIS_Y) *dy = 0;
			break;
			
		case ID_VIEW_FRONTVIEW :
			if (mLockAxis & AXIS_Z) *dx = 0;
			if (mLockAxis & AXIS_Y) *dy = 0;
			break;
	}
}

// Prevent axis movement.  World space clipping.
void CGEditProView::LockAxis( geVec3d * pWP )
{
	int mLockAxis ;
	CGEditProDoc* pDoc = GetDocument();

	mLockAxis = pDoc->GetLockAxis() ;

	if( mLockAxis & AXIS_X )	pWP->X = 0.0f ;
	if( mLockAxis & AXIS_Y )	pWP->Y = 0.0f ;
	if( mLockAxis & AXIS_Z )	pWP->Z = 0.0f ;
}/* CGEditProView::LockAxis */

void CGEditProView::OnToolsBrushRotate45()
{
	geVec3d	rp;

	static int i_reminder;	//	so this messagebox only shows up once

	CGEditProDoc* pDoc = NULL;
	pDoc = GetDocument();
	if (pDoc)
	{
		pDoc->TempCopySelectedBrushes();

		//	post 0.57
	CString message = _T("If you choose this control while the Textured View is active,\n");
			message += _T("rotation will NOT occur.\n\n");
			message += _T("In order to get rotation in the view of your choice,\n");
			message += _T("make sure your view is active (highlighted).\n\n");
			message += _T("We recommend that you use the menu item for this control\n");
			message += _T("on the right-click context menu while in the view of your choice.");

	geVec3d_Clear (&rp);
	switch (mViewType)
	{
	case ID_VIEW_TEXTUREVIEW:	//	post 0.57
		if (i_reminder <1)		//	deliver the reminder once
		{
			AfxMessageBox(message);
			i_reminder++;
		}
		break;
		case ID_VIEW_TOPVIEW:	//	if textured view is highlighted, rotation will occur
			rp.Y = -(M_PI/4.0f);//	in the topo view
			break;
		case ID_VIEW_FRONTVIEW :
			rp.Z = -(M_PI/4.0f);
			break;
		case ID_VIEW_SIDEVIEW:
			rp.X = (M_PI/4.0f);
			break;
		default :
			return;
	}

	if(GetModeTool()!=ID_GENERALSELECT)
	{
		pDoc->RotateTemplateBrush(&rp);
		pDoc->UpdateSelected();	// calls UpdateMainControls()
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
	}
	else
	{
		pDoc->RotateSelectedBrushesDirect (&rp);
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		//	Make sure REBUILD_QUICK is absolutely necessary.
		pDoc->UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL);
	}

	//	post 0.57	send a message to display the Rebuild button on the textured view
	int ModeTool = GetModeTool ();
	int AdjustMode = GetAdjustMode ();
	if ((ModeTool == ID_GENERALSELECT) && (AdjustMode == ADJUST_MODE_BRUSH))
	{
		if (!CGlobals::g_bRebuildAlways)
		{
			pDoc->m_pMainFrame->SendMessageToDescendants(WM_COMMAND, IDC_BTN_REBUILD_COMMAND);
			CGlobals::g_iRebuildClickCount = 0;
		}
	}
	}
}


int CGEditProView::GetCursorBoxPos (const POINT *ptMousePos)
{
	CGEditProDoc *pDoc;
	const Box3d *pBrushBox;
	POINT ptMin, ptMax;
	int dx, dy;
	int x, y;
	int horiz, vert;
	int lookup[4] = {1, 2, 2, 3};

	/*
	  Split the box up into 3 sections vertically and 3 horizontally.
	  The center sections are 1/2 the width and height, and the end sections are
	  each 1/4 of width and height.

	  What we're creating is:

			0	   1       2        3	  4
				------------------------
				|     |          |     |
			5	|  6  |    7     |  8  |  9
				------------------------
				|     |          |     |
				|     |          |     | 
		   10	|  11 |   12     |  13 |  14
				|     |          |     |
				------------------------
				|     |          |     | 
		   15	|  16 |   17     |  18 |  19
				------------------------
		   20      21	  22	    23	  24
	  
	  Then we determine which of the sections the cursor is closest to,
	  and return that index.
	*/

	pDoc = GetDocument ();

	pBrushBox = Brush_GetBoundingBox (pDoc->m_pCurBrush);

	// obtain screen coordinates for bounding box min and max points
	ptMin = Render_OrthoWorldToView (VCam, &pBrushBox->Min);
	ptMax = Render_OrthoWorldToView (VCam, &pBrushBox->Max);

	// make sure the min and max points are correct...
	if (ptMin.x > ptMax.x)
	{
		int temp;

		temp = ptMin.x;
		ptMin.x = ptMax.x;
		ptMax.x = temp;
	}
	if (ptMin.y > ptMax.y)
	{
		int temp;

		temp = ptMin.y;
		ptMin.y = ptMax.y;
		ptMax.y = temp;
	}

	// compute horizontal first
	x = ptMousePos->x - ptMin.x;
	dx = (ptMax.x - ptMin.x);
	if (dx == 0) horiz = 0; else horiz = (4*x) / dx;
	if (horiz < 0) horiz = 0;
	else if (horiz > 3) horiz = 4;
	else horiz = lookup[horiz];

	// and vertical
	y = ptMousePos->y - ptMin.y;
	dy = (ptMax.y - ptMin.y);
	if (dy == 0) vert = 0; else vert = (4*y)/dy;
	if (vert < 0) vert = 0;
	else if (vert > 3) vert = 3;
	else vert = lookup[vert];

	// return index...
	return (vert * 5) + horiz;
}

void CGEditProView::SetEditCursor (int Tool, const POINT *pMousePos)
{
	//for sizing stuff
	static const char *SizeCursors[25]=
	{
		IDC_SIZENWSE,	IDC_SIZENWSE,	IDC_SIZENS,		IDC_SIZENESW,	IDC_SIZENESW,
		IDC_SIZENWSE,	IDC_SIZENWSE,	IDC_SIZENS,		IDC_SIZENESW,	IDC_SIZENESW,	
		IDC_SIZEWE,		IDC_SIZEWE,		IDC_NO,			IDC_SIZEWE,		IDC_SIZEWE,
		IDC_SIZENESW,	IDC_SIZENESW,	IDC_SIZENS,		IDC_SIZENWSE,	IDC_SIZENWSE,
		IDC_SIZENESW,	IDC_SIZENESW,	IDC_SIZENS,		IDC_SIZENWSE,	IDC_SIZENWSE
	};

	static const char *ShearCursors[25]=
	{
		IDC_NO,			IDC_SIZEWE,		IDC_SIZEWE,		IDC_SIZEWE,		IDC_NO,
		IDC_SIZENS,		IDC_NO,			IDC_SIZEWE,		IDC_NO,			IDC_SIZENS,
		IDC_SIZENS,		IDC_SIZENS,		IDC_NO,			IDC_SIZENS,		IDC_SIZENS,
		IDC_SIZENS,		IDC_NO,			IDC_SIZEWE,		IDC_NO,			IDC_SIZENS,
		IDC_NO,			IDC_SIZEWE,		IDC_SIZEWE,		IDC_SIZEWE,		IDC_NO
	};

	const char *WhichCursor = NULL;
	int CursorIndex;

	assert ((Tool == ID_TOOLS_BRUSH_SCALEBRUSH) || (Tool == ID_TOOLS_BRUSH_SHEARBRUSH));

	// Determine where the cursor is on the box surrounding the selected brush,
	// and set the appropriate cursor.
	if (pMousePos->x < 0 || pMousePos->y < 0)
	{
		return;
	}
	CursorIndex = GetCursorBoxPos (pMousePos);
	switch (Tool)
	{
		case ID_TOOLS_BRUSH_SCALEBRUSH :
			// Scaling it's just a simple lookup
			WhichCursor = SizeCursors [CursorIndex];
			break;

		case ID_TOOLS_BRUSH_SHEARBRUSH :
			WhichCursor = ShearCursors[CursorIndex];
			break;

		default :
			assert (0);
			break;
	}
	SetCursor (AfxGetApp()->LoadStandardCursor (WhichCursor));
}

void CGEditProView::Pan(
	  CGEditProDoc *pDoc,
	  int dx, int dy,
	  const geVec3d *dv,
	  BOOL LButtonIsDown, 
	  BOOL RButtonIsDown,
	  BOOL MButtonIsDown,
	  BOOL SpaceIsDown
	)
{
	if(mViewIs3d)
	{
		geVec3d MoveVec;

			//	if the left mouse button is down AND the spacebar is down,
			//	OR if the mousewheel button is down
		if((LButtonIsDown && SpaceIsDown) || (MButtonIsDown))		
		{
			geVec3d_Set (&MoveVec, (float)-dx, (float)-dy, 0.0f);
			Render_MoveCamPos( VCam, &MoveVec ) ;
			pDoc->UpdateCameraEntity( VCam ) ;
		}
		else if (LButtonIsDown && !SpaceIsDown)
		{
			geVec3d_Set (&MoveVec, 0.0f, 0.0f, (float)-dy);
			Render_MoveCamPos( VCam, &MoveVec ) ;
			if (Render_UpIsDown (VCam))
			{
				Render_IncrementYaw (VCam, (float)(-dx));
			}
			else
			{
				Render_IncrementYaw(VCam, (float)dx);
			}
			pDoc->UpdateCameraEntity( VCam ) ;
		}
/*
		else if (RButtonIsDown && SpaceIsDown)
		{





		}

*/		else if (RButtonIsDown)
		{
			if (Render_UpIsDown (VCam))
			{
				Render_IncrementYaw (VCam, (float)(-dx));
			}
			else
			{
				Render_IncrementYaw(VCam, (float)dx);
			}
			Render_IncrementPitch(VCam, (float)dy);
			pDoc->UpdateCameraEntity( VCam ) ;
		}
	}
	else	// we are in one of the ortho views
	{
		geVec3d dcamv;
		
		geVec3d_Scale (dv, -1.0f, &dcamv);
				
		if (
				(  
					(  (LButtonIsDown && SpaceIsDown) || MButtonIsDown) && pDoc->m_iModeTool != ID_TOOLS_CAMERA)
			
			||
				(  
					(  (LButtonIsDown || MButtonIsDown) && pDoc->m_iModeTool == ID_TOOLS_CAMERA)
				)
			)
		{
			Render_MoveCamPosOrtho(VCam, &dcamv);	//	pan the camera
		}
		else if (RButtonIsDown)
		{
			Render_ZoomChange(VCam, -(((float)dy) * 0.001f));	//	zoom
			pDoc->UpdateGridInformation ();
		}
	}
}

void CGEditProView::ScaleSelected (CGEditProDoc *pDoc, int dx, int dy)
{
	//smooth out the zoom scale curve with a scalar
	float	ZoomInv	=Render_GetZoom(VCam);

	ZoomInv	=(ZoomInv > .5)? 0.5f / ZoomInv : 1.0f;

	// negated here because Brush_Resize is still thinking weird
	pDoc->ResizeSelected (-(((float)dx)*ZoomInv), -(((float)dy)*ZoomInv), sides, Render_GetInidx(VCam));
}

void CGEditProView::ShearSelected (CGEditProDoc *pDoc, int dx, int dy)
{
	//smooth out the zoom scale curve with a scalar
	float	ZoomInv	=Render_GetZoom(VCam);

	ZoomInv	=(ZoomInv > .5)? 0.5f / ZoomInv : 1.0f;

	pDoc->ShearSelected(-(((float)dy)*ZoomInv), -(((float)dx)*ZoomInv), sides, Render_GetInidx(VCam));
}

#pragma warning (disable:4100)
void CGEditProView::OnMouseMove (UINT nFlags, CPoint point)
{
	int			dx, dy;
	geVec3d		sp, wp, dv;
	CGEditProDoc	*pDoc = NULL;
	geBoolean	ShiftHeld;
	geBoolean	ControlHeld;
	geBoolean	LButtonIsDown, RButtonIsDown, MButtonIsDown;	
	geBoolean	SpaceHeld;
	BOOL		ThisIsCaptured;
	int			ModeTool, Tool;
	fdocAdjustEnum AdjustMode;
	BOOL		DoRedraw = TRUE;
	POINT		RealCursorPosition;


	pDoc	=GetDocument();
	ThisIsCaptured = (this == GetCapture ());
	ModeTool = GetModeTool ();
	Tool		= GetTool ();
	AdjustMode  = GetAdjustMode ();

	/*
	You'll notice here that we don't use the nFlags parameter to get these
	settings.  Those flags tell what the state of things was when the
	MouseMove message was received, which could have been several seconds
	ago (due to delays caused by a mashed key, for example).  What we're
	really interested is the current state, so we can throw out old messages.
	*/
	ShiftHeld = IsKeyDown (VK_SHIFT);
	ControlHeld = IsKeyDown (VK_CONTROL);
	LButtonIsDown = IsKeyDown (VK_LBUTTON);
	RButtonIsDown = IsKeyDown (VK_RBUTTON);
	SpaceHeld	= IsKeyDown (VK_SPACE);
	MButtonIsDown = IsKeyDown (VK_MBUTTON);	


	//	automatically activate pane if mouse is over it
	switch( mViewType ) 
	{
	case ID_VIEW_3DWIREFRAME:
	case ID_VIEW_TEXTUREVIEW:
		{
			int row = TEXTURE_VIEW_ROW;
			int col = TEXTURE_VIEW_COL;
			CDC		*pDC = NULL;
			CWnd	*pWnd = NULL;
			CRect	rect;
			rect.SetRectEmpty();


			CSplitterWnd *pParent = NULL;
			pParent = STATIC_DOWNCAST(CSplitterWnd, GetParent());

			if (pParent)
			{
				VERIFY(pParent->IsChild(this));

				pWnd = pParent->GetPane(row,col);
				if (pWnd)
				{
					//	if the active pane is not the pane for this view
					if (pParent->GetActivePane(&row, &col) != pWnd)
					{
						//	make the pane for this view the active pane

						pDC = pWnd->GetDC();
						if (pDC)
						{
							pDC->GetClipBox(rect);
						}
						pParent->SetActivePane(row,col, this);
					}
					break;
				}
			}
		}
	case ID_VIEW_TOPVIEW:
		{
			int row = TOP_VIEW_ROW;
			int col = TOP_VIEW_COL;
			CDC		*pDC = NULL;
			CWnd	*pWnd = NULL;
			CRect	rect;
			rect.SetRectEmpty();

			CSplitterWnd *pParent = NULL;
			pParent = STATIC_DOWNCAST(CSplitterWnd, GetParent());

			if (pParent)
			{
				VERIFY(pParent->IsChild(this));

				pWnd = pParent->GetPane(row,col);
				if (pWnd)
				{
					//	if the active pane is not the pane for this view
					if (pParent->GetActivePane(&row, &col) != pWnd)
					{
						//	make the pane for this view the active pane

						pDC = pWnd->GetDC();
						if (pDC)
						{
							pDC->GetClipBox(rect);
						}
						pParent->SetActivePane(row,col, this);
					}
					break;
				}
			}
		}

	case ID_VIEW_FRONTVIEW:
		{
			int row = FRONT_VIEW_ROW;
			int col = FRONT_VIEW_COL;
			CDC		*pDC = NULL;
			CWnd	*pWnd = NULL;
			CRect	rect;
			rect.SetRectEmpty();

			CSplitterWnd *pParent = NULL;
			pParent = STATIC_DOWNCAST(CSplitterWnd, GetParent());

			if (pParent)
			{
				VERIFY(pParent->IsChild(this));

				pWnd = pParent->GetPane(row,col);
				if (pWnd)
				{
					//	if the active pane is not the pane for this view
					if (pParent->GetActivePane(&row, &col) != pWnd)
					{
						//	make the pane for this view the active pane

						pDC = pWnd->GetDC();
						if (pDC)
						{
							pDC->GetClipBox(rect);
						}

						pParent->SetActivePane(row,col, this);
					}
					break;
				}
			}
		}

	case ID_VIEW_SIDEVIEW:
		{
			int row = SIDE_VIEW_ROW;
			int col = SIDE_VIEW_COL;
			CDC		*pDC = NULL;
			CWnd	*pWnd = NULL;
			CRect	rect;
			rect.SetRectEmpty();

			CSplitterWnd *pParent = NULL;
			pParent = STATIC_DOWNCAST(CSplitterWnd, GetParent());

			if (pParent)
			{
				VERIFY(pParent->IsChild(this));

				pWnd = pParent->GetPane(row,col);
				if (pWnd)
				{
					//	if the active pane is not the pane for this view
					if (pParent->GetActivePane(&row, &col) != pWnd)
					{
						//	make the pane for this view the active pane

						pDC = pWnd->GetDC();
						if (pDC)
						{
							pDC->GetClipBox(rect);
						}

						pParent->SetActivePane(row,col, this);
					}
					break;
				}
			}
		}
	}
	//	IsPanning is true if any of the following conditions:
	//	1.	if either the spacebar is being held down or if we are
	//		just continuing on with a previously established state
	//		of IsPanning ---AND--- both left and righ mouse buttons
	//		are held down ---AND--- the mouse recently clicked in
	//		the view ---AND--- the view is the textured view.
	//	2.	The mousewheel button is down.Whenever the mousewheel
	//		button is down, we will be able to pan regardless of
	//		which view we are in.
	IsPanning =	  (
		((SpaceHeld || IsPanning) &&
		(LButtonIsDown | RButtonIsDown) && ThisIsCaptured /*&& mViewIs3d*/)

		|| (MButtonIsDown)		//	or if the mousewheel button is depressed

		);

	//	if neither left mouse button nor mousewheel button
	//	have been clicked while the cursor is in this view
	if (!ThisIsCaptured && !MButtonIsDown)	
	{
		// Mouse isn't captured.  So we're just moving the mouse around
		// in this view.  If we're not in camera mode and not panning,
		// then set the cursor accordingly and exit.
		if(!((ModeTool == ID_TOOLS_CAMERA) || IsPanning))
		{
			int Tool;

			Tool = GetTool ();
			if (mViewIs3d)
			{
				if (ShiftHeld)
				{
					SetCursor (AfxGetApp()->LoadCursor (IDC_EYEDROPPER));
				}
				else
				{
					SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW));
				}
			}

			if (!mViewIs3d)		
			{
				if ((Tool == ID_TOOLS_BRUSH_SCALEBRUSH) || (Tool == ID_TOOLS_BRUSH_SHEARBRUSH))
				{
					SetEditCursor (Tool, &point);
				}

				else if(Tool == ID_TOOLS_BRUSH_MOVEROTATEBRUSH) 
					SetCursor(AfxGetApp()->LoadCursor(IDC_MOVESELECT)); 
				else if(Tool == ID_TOOLS_BRUSH_ROTATEBRUSH) 
					SetCursor(AfxGetApp()->LoadCursor(IDC_ROTATE));
				else 
					SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW)); 
			}
		}
		return;
	}
	//	otherwise either the mouse left button or the mousewheel button
	//	were clicked while the cursor was in this view
	if(this==GetParentFrame()->GetActiveView())
	{
		pDoc->m_iActiveView	=mViewType;
	}

	/*
	The point parameter to this message gives us the mouse position when the
	message was received.  That could be old.  We really want the *current*
	position, so we get it here and convert it to client coordinates.  This
	prevents the panning runaway bug that plauged us for so long.
	*/
	::GetCursorPos (&RealCursorPosition);
	ScreenToClient (&RealCursorPosition);

	dx = (RealCursorPosition.x - mStartPoint.x);
	dy = (RealCursorPosition.y - mStartPoint.y);

	if ((dx == 0) && (dy == 0))	// don't do anything if no delta
	{
		return;
	}

	Render_ViewToWorld(VCam, mStartPoint.x, mStartPoint.y, &sp);
	Render_ViewToWorld(VCam, RealCursorPosition.x, RealCursorPosition.y, &wp);
	geVec3d_Subtract(&wp, &sp, &dv);	// delta in world space

	//	if in Camera mode or if in Panning mode
	//	then pan in in the active view (includes all 4 views)
	//	if ((ModeTool == ID_TOOLS_CAMERA)||IsPanning)
	//	the heck with it. We want the camera in all modes

	//	if we are in camera mode, then pan regardless of the active view
	//	if we are in either Brush or Templates mode, then Pan as long as
	//	we are in the textured view and not in the ortho views.
	//	Or if we passed the IsPanning() test above.
	if ((ModeTool == ID_TOOLS_CAMERA) ||
		(ID_TOOLS_TEMPLATE && mViewIs3d) ||
		(ID_GENERALSELECT && mViewIs3d) ||
		IsPanning)
	{
		Pan (pDoc, dx, dy, &dv, LButtonIsDown, RButtonIsDown, MButtonIsDown, SpaceHeld);	
	}
	// if it's one of the ortho views and not the textured view
	else if (!mViewIs3d) 
		// none of this stuff should be available in the 3d view.
	{
		switch (ModeTool)
		{
		case ID_GENERALSELECT :
			switch (Tool)
			{
			case CURTOOL_NONE :
				// no tool selected.  We're doing a drag select or clone operation
				if (AdjustMode == ADJUST_MODE_BRUSH)
				{
					// drag select or cloning
					if( IsDragging )
					{
						mDragCurrentPoint.x += (long)dx ;
						mDragCurrentPoint.y += (long)dy ;
					}
					else if( !IsCopying && !ShiftHeld && !RButtonIsDown )
					{
#pragma message ("Logic flawed here when space being held.  Don't know exactly what.")
						if((abs(dx) > mCX_DRAG) || (abs(dy) > mCY_DRAG))
						{
							mDragCurrentPoint = RealCursorPosition;
							IsDragging = TRUE ;
						}
					}// Drag Select
					else
					{	// Begin a copy operation			
						if((LButtonIsDown && ShiftHeld)&&(!IsCopying))
						{
							IsCopying	=TRUE;
							pDoc->CopySelectedBrushes();
							if (SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) > 0)
							{
								// make current brush point to first brush in list
								// so we can snap correctly.
								pDoc->m_pCurBrush = SelBrushList_GetBrush (pDoc->DocVarsGetPointer()->m_pSelBrushes, 0);
							}
						}
						if(IsCopying)
						{
							LockAxis( &dv ) ;
							if(LButtonIsDown)
							{
								pDoc->MoveSelectedClone(&dv);
							}
							SetTool(CURTOOL_NONE);
						}
					}// Not Drag Select
				}
				break;


				// REVISED FOR G3DC
			case ID_TOOLS_BRUSH_MOVEROTATEBRUSH :
				// moving/rotating brushes and entities
				SetTool(ID_TOOLS_BRUSH_MOVESELECTEDBRUSHES);

				if (LButtonIsDown)
				{
					LockAxis( &dv ) ;
					pDoc->MoveSelectedBrushes(&dv);
				}// LButtonDown

				SetTool(ID_TOOLS_BRUSH_MOVEROTATEBRUSH);

				break;

				// NEW FOR G3DC						
			case ID_TOOLS_BRUSH_ROTATEBRUSH:
				{
					if (LButtonIsDown)
					{
						if( pDoc->GetSelState() == ONEENTITYONLY )	// Angle,Arc,Radius control
						{
							if( !ShiftHeld && !ControlHeld )
							{
								pDoc->AdjustEntityAngle( VCam, (float)dx ) ;
							}
							else if( ShiftHeld && !ControlHeld )
							{
								pDoc->AdjustEntityArc( VCam, (float)dx ) ;
							}
							else if( !ShiftHeld && ControlHeld )
							{
								pDoc->AdjustEntityRadius( &dv ) ;
							}
						}
						else
						{			
							Render_ViewDeltaToRotation (VCam, (float)dx, &dv);
							pDoc->RotateSelectedBrushes(&dv);
						}
					}// LButtonDown
					SetTool(ID_TOOLS_BRUSH_ROTATEBRUSH);
				}
				break;


			case ID_TOOLS_BRUSH_SCALEBRUSH :
				//	Checking the button state is not always accurate since
				//	messaging delays in OnUpdate() commands may not be
				//	up-to-date.
				//	We need to directly confirm that multiple brushes are
				//	not selected. 
				if (SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) > 0)
				{
					if (LButtonIsDown)
					{
						LockAxisView (&dx, &dy);
						ScaleSelected (pDoc, dx, dy);
					}
					break;
				}
				break;

			case ID_TOOLS_BRUSH_SHEARBRUSH :
				//	Checking the button state is not always accurate since
				//	messaging delays in OnUpdate() commands may not be
				//	up-to-date.
				//	We need to directly confirm that multiple brushes are
				//	not selected. 
				if (SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) == 1)
				{
					if (LButtonIsDown)
					{
						LockAxisView (&dx, &dy);
						ShearSelected (pDoc, dx, dy);
					}
					break;
				}
				break;

			default :
				DoRedraw = FALSE;
				break;
			}
			break;

		case ID_TOOLS_TEMPLATE :
			switch (Tool)
			{
			case ID_TOOLS_BRUSH_MOVEROTATEBRUSH :
				if (LButtonIsDown)
				{
					LockAxis (&dv);
					pDoc->MoveTemplateBrush(&dv);
				}
				break; // changed for g3dc

			case ID_TOOLS_BRUSH_ROTATEBRUSH :
				if (LButtonIsDown)
				{
					Render_ViewDeltaToRotation (VCam, (float)dx, &dv);
					pDoc->RotateTemplateBrush(&dv);
				}
				break;

			case ID_TOOLS_BRUSH_SCALEBRUSH :
				if (LButtonIsDown)
				{
					LockAxisView (&dx, &dy);
					ScaleSelected (pDoc, dx, dy);
				}
				break;

			case ID_TOOLS_BRUSH_SHEARBRUSH :
				if (LButtonIsDown)
				{
					LockAxisView (&dx, &dy);
					ShearSelected (pDoc, dx, dy);
				}
				break;

			default :
				DoRedraw = FALSE;
				break;
			}
			break;

		default :
			DoRedraw = FALSE;
			break;
		} // Mode Tool
	}// Ortho Views

	if (DoRedraw)
	{
		RedrawWindow();
	}

	POINT pt = mStartPoint;	// The position works on the delta mStartPoint...
	ClientToScreen( &pt ) ;
	SetCursorPos( pt.x, pt.y );	
}
#pragma warning (default:4100)






BOOL CGEditProView::OnMouseWheel(UINT nFlags, short zDelta, CPoint ptMouse)
{
	CGEditProDoc		*pDoc = GetDocument();;
	geVec3d			sp, wp, dv;					//	vectors for our movement calcs
	int				ModeTool = GetModeTool();	//	our currently selected main tool

	if (!mViewIs3d)				//	if we are in one of the ortho views
	{
		if (zDelta < 0)			//	NOTE:	a forward roll-increment = zDelta = 120	
			OnViewZoomout();	//			a backward roll-increment = zDelta = -120
		if (zDelta > 0)
			OnViewZoomin();
	}
								//	restrict to Camera mode, otherwise too much confusion
								//	If the textured view is active, 
								//	AND we are in Camera mode
    if (/*(ModeTool == ID_TOOLS_CAMERA) &&*/ (mViewIs3d))
	{
							//	get a vector for the mouse cursor in the view
		Render_ViewToWorld(VCam, ptMouse.x, ptMouse.y, &sp); 
							//	calc a delta factor that would advance to a new position
		Render_ViewToWorld(VCam, ptMouse.x, (ptMouse.y + zDelta), &wp);
							//	calc the final vector location in world space
		geVec3d_Subtract(&wp, &sp, &dv);	
							//	send this info to Pan
									//left  right  middle  space
		Pan (pDoc, 0, (-zDelta), &dv, TRUE, FALSE, FALSE, FALSE);
							//	now redraw the textured view
		RedrawWindow();
	}
    return CView::OnMouseWheel(nFlags, zDelta,   ptMouse);
}




void CGEditProView::OnMButtonDown(UINT nFlags, CPoint point)
{

        SetCapture();			//	capture the current mouse position

		mStartPoint = point;	//	set the starting point to the current position
		HideTheCursor();		//	hide the cursor while we pan	
        
}

void CGEditProView::OnMButtonUp(UINT nFlags, CPoint point)
{
	        ReleaseCapture();	//	release control
     
			ShowTheCursor();	//	n'est pas?
}





void CGEditProView::DoZoom (float ZoomInc)
{
	if (!mViewIs3d)
	{
		CGEditProDoc *pDoc = GetDocument ();

		Render_ZoomChange( VCam, ZoomInc);
		pDoc->UpdateGridInformation ();
		RedrawWindow();
	}
}

void CGEditProView::OnViewZoomin() 
{
	DoZoom (0.1f);
}

void CGEditProView::OnUpdateViewZoomin(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CGEditProView::OnViewZoomout() 
{
	DoZoom (-0.1f);
}

void CGEditProView::OnUpdateViewZoomout(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}



void CGEditProView::HideTheCursor (void)
{
	while (ShowCursor (FALSE) >= 0)
	{
		;
	}
}

void CGEditProView::ShowTheCursor (void)
{
	while (ShowCursor (TRUE) < 0)
	{
		;
	}
}






// Snaps the closest edge to SnapSize.
static float ComputeSnap (float Cur, float Delta, float SnapSize)
{
	float Target;
	float SnapDelta;
	float Remainder;

	Target = Cur + Delta;
	Remainder = (float)fmod (Target, SnapSize);
	if (fabs (Remainder) < (SnapSize / 2.0f))
	{
		SnapDelta = -Remainder;
	}
	else
	{
		if (Target < 0.0f)
		{
			SnapDelta = -(SnapSize + Remainder);
		}
		else
		{
			SnapDelta = SnapSize - Remainder;
		}
	}
	return SnapDelta;
}

static float SnapSide (float CurMin, float CurMax, float Delta, float SnapSize)
{
	float MinDelta, MaxDelta;

	MinDelta = ComputeSnap (CurMin, Delta, SnapSize);
	MaxDelta = ComputeSnap (CurMax, Delta, SnapSize);

	return (fabs (MinDelta) < fabs (MaxDelta)) ? MinDelta : MaxDelta;
}

void CGEditProView::DoneMovingBrushes ()
{
	CGEditProDoc *pDoc = GetDocument ();
	int ModeTool = GetModeTool ();

	if (SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) > 0 || ModeTool == ID_TOOLS_TEMPLATE)
	{
		geFloat fSnapSize ;
		const geVec3d *vMin, *vMax;
		const Box3d *pBox;
		geVec3d SnapDelta;
		geBoolean SnapX, SnapY, SnapZ;

		fSnapSize = 1.0f;
		if (pDoc->m_pLevelMgr->UseGrid (pDoc->GetLevel()))
		{
			fSnapSize = pDoc->m_pLevelMgr->GetGridSnapSize (pDoc->GetLevel());
		}
		// do the snap thing...
		pBox = Brush_GetBoundingBox (pDoc->m_pCurBrush);
		vMin = Box3d_GetMin (pBox);
		vMax = Box3d_GetMax (pBox);
		geVec3d_Clear (&SnapDelta);
		/*
		  In template mode, the brush is moved directly, so we have to snap to
		  the current position, not current position plus delta.  Since we
		  clear the delta before computing the snap, we have to save these
		  flags.
		*/
		SnapX = (pDoc->m_vecFinalPos.X != 0.0f);
		SnapY = (pDoc->m_vecFinalPos.Y != 0.0f);
		SnapZ = (pDoc->m_vecFinalPos.Z != 0.0f);
		if ((ModeTool == ID_TOOLS_TEMPLATE) || IsCopying)
		{
			geVec3d_Clear (&pDoc->m_vecFinalPos);
		}
		if (SnapX)
		{
			SnapDelta.X = ::SnapSide (vMin->X, vMax->X, pDoc->m_vecFinalPos.X, fSnapSize);
		}
		if (SnapY)
		{
			SnapDelta.Y = ::SnapSide (vMin->Y, vMax->Y, pDoc->m_vecFinalPos.Y, fSnapSize);
		}
		if (SnapZ)
		{
			SnapDelta.Z = ::SnapSide (vMin->Z, vMax->Z, pDoc->m_vecFinalPos.Z, fSnapSize);
		}
		if (ModeTool == ID_TOOLS_TEMPLATE)
		{
			pDoc->m_vecFinalPos = SnapDelta;
		}
		else
		{
			geVec3d_Add (&pDoc->m_vecFinalPos, &SnapDelta, &pDoc->m_vecFinalPos);
		}
	}

	pDoc->DoneMove (); // already calls pDoc->UpdateSelected
}


#pragma warning (disable:4100)
#pragma warning (disable:4806)
void CGEditProView::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL		bWasCaptured = FALSE ;
	CGEditProDoc* pDoc = NULL;
	int ModeTool;
	int Tool;
	fdocAdjustEnum AdjustMode;

	ModeTool = GetModeTool ();
	Tool = GetTool ();
	pDoc = GetDocument ();
	AdjustMode = GetAdjustMode ();


	LMouseButtonDown = GE_FALSE;
	if (!RMouseButtonDown)
	{
		// right mouse button isn't down
		if(this == GetCapture ())
		{
			bWasCaptured = TRUE ;
			ReleaseCapture();
		}

		if (IsKeyDown (VK_SPACE) || IsPanning || ModeTool == ID_TOOLS_CAMERA)
		{
			/*
			Ok, here's the scoop.
			If we're in the middle of a move/rotate and the user mashes the space bar,
			we all of a sudden end up in panning mode with no context information to
			tell us that we were previously moving/rotating.  So we end up with the
			original brushes in the world, and the temporary brushes that are selected
			and being moved also in the world.  So here we do a TempDeleteSelected to
			remove those temporary brushes.  Otherwise they'd hang around forever.

			Ideally, the move/rotate would be separated from the panning so that we could
			move and pan at the same time.  Of course, I'd like to get rid of the whole
			temp selected thing, too, but that'll have to wait...
			*/
			pDoc->TempDeleteSelected(pDoc->DocVarsGetPointer());

			IsPanning = FALSE;
			ShowTheCursor ();

			RedrawWindow();
			return;
		}	//	IsKeyDown()

		if (mViewIs3d)	// we just clicked the textered view
		{
			if(ModeTool==ID_TOOLS_TEMPLATE && pDoc->m_bTempEnt)
			{
				pDoc->PlaceTemplateEntity3D(point, VCam);	
			}
			else if (IsKeyDown (VK_SHIFT))
			{
				pDoc->SelectTextureFromFace3D (point, VCam);
			}
			else
			{
				switch (AdjustMode)
				{
				case ADJUST_MODE_BRUSH :
				case ADJUST_MODE_FACE :
					pDoc->SelectRay (point, VCam);	//	calls UpdateSelected()
					pDoc->UpdateAllViews( UAV_ALLVIEWS, NULL );
					break;				
				default :
					break;
				}
			}
		}
		else	// we just clicked on one of the 3 ortho views
		{
			switch (Tool)
			{
			case CURTOOL_NONE :
				if (AdjustMode == ADJUST_MODE_BRUSH)
				{
					if( IsDragging )
					{							//	calls UpdateSelected()
						pDoc->SelectOrthoRect( mDragStartPoint, mDragCurrentPoint, VCam );
						//	Be very careful when speccing flags for UpdateAllViews()
						//	The wrong flags at the wrong time will totally screw things up
						pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
						IsDragging = FALSE ;
					}
					else if(IsCopying)
					{	//	calls DoneMove() which calls UpdateSelected()
						//	which calls UpdateMainControls()
						DoneMovingBrushes (); 
						IsCopying = FALSE;
					}
					else
					{
						pDoc->SelectOrtho(point, VCam);	//	calls UpdateSelected()
						//	Be very careful when speccing flags for UpdateAllViews()
						//	The wrong flags at the wrong time will totally screw things up
						pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );
					}
				}
				else
				{
					MessageBeep ((UINT)(-1));
				}

				break;

			case ID_TOOLS_BRUSH_MOVEROTATEBRUSH :
			case ID_TOOLS_BRUSH_MOVESELECTEDBRUSHES :

				DoneMovingBrushes ();		// calls pDoc->UpdateSelected
				if ((ModeTool == ID_TOOLS_TEMPLATE) ||
					((pDoc->GetSelState() & ANYENTITY) && (!(pDoc->GetSelState() & ANYBRUSH))) )
				{			
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );	// old gedit
				}
				else
				{
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					//	Make sure REBUILD_QUICK is absolutely necessary.
					pDoc->UpdateAllViews( UAV_ALL3DVIEWS | REBUILD_QUICK, NULL );
				}

				break; 

				// new g3dc	to accomodate the new Rotate tool
			case ID_TOOLS_BRUSH_ROTATEBRUSH:
				{
					pDoc->UpdateSelected();	//	calls UpdateMainControls()
					if (GetModeTool () == ID_GENERALSELECT)
					{
						pDoc->DoneRotate ();	//calls UpdateSelected
					}

					if (SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) != 0)
						//	Be very careful when speccing flags for UpdateAllViews()
						//	The wrong flags at the wrong time will totally screw things up
						//	Make sure REBUILD_QUICK is absolutely necessary.
						pDoc->UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL);	

					else
						pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );

					break;
				}

			case ID_TOOLS_BRUSH_SCALEBRUSH :
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				pDoc->SnapScaleNearest(sides, Render_GetInidx(VCam), VCam);
				if(pDoc->m_iLastOp == BRUSH_SCALE)
				{
					pDoc->DoneResize(sides, Render_GetInidx(VCam));	// calls UpdateSelected()
				}

				pDoc->UpdateSelected();	//	calls UpdateMainControls()

				if((ModeTool == ID_TOOLS_TEMPLATE) ||
					((pDoc->GetSelState() & ANYENTITY) && (!(pDoc->GetSelState() & ANYBRUSH))) )
				{			
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );
				}
				else	
				{
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					//	Make sure REBUILD_QUICK is absolutely necessary.
					pDoc->UpdateAllViews( UAV_ALL3DVIEWS | REBUILD_QUICK, NULL );
				}
				break;

			case ID_TOOLS_BRUSH_SHEARBRUSH :
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				if(pDoc->m_iLastOp==BRUSH_SHEAR)
					pDoc->DoneShear(sides, Render_GetInidx(VCam));

				pDoc->UpdateSelected(); 	//	calls UpdateMainControls()
				if((ModeTool == ID_TOOLS_TEMPLATE) ||
					((pDoc->GetSelState() & ANYENTITY) && (!(pDoc->GetSelState() & ANYBRUSH))) )
				{			
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );
				}
				else
				{
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					//	Make sure REBUILD_QUICK is absolutely necessary.
					pDoc->UpdateAllViews( UAV_ALL3DVIEWS | REBUILD_QUICK, NULL );
				}
				break;

			default :
				break;
			}

			//	new g3dc. pick the right UpdateAllViews flags so the Face Attributes
			//	controls work.
			switch (AdjustMode)
			{
			case ADJUST_MODE_BRUSH :
			case ADJUST_MODE_FACE :
				//	new g3dc
				//	critical for selecting brushes in textured view and
				//	and for getting the correct face information
				//	If a face is selected and a brush is not selected...
				if ((!pDoc->GetSelState() & ANYBRUSH) || (pDoc->GetSelState() & ANYFACE))
				{	//	if face is selected and brush is not
					//	absolutely necessary in order to conserve brush and 
					//	face dialog information
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL );
				}

				break;

			default :
				break;
			}
		}

		if( bWasCaptured )
		{
			ClientToScreen( &mStartPoint ) ;
			SetCursorPos( mStartPoint.x, mStartPoint.y ) ;
		}

		ShowTheCursor ();
	}

	assert( IsCopying == FALSE ) ;
}
#pragma warning (default:4100)
#pragma warning (default:4806)



#pragma warning (disable:4100)
void CGEditProView::OnLButtonDown(UINT nFlags, CPoint point)
{
	/*
	  These tables convert the index values returned by GetCursorBoxPos
	  into the bitmapped values expected by the brush routines (the "sides" values).
	  These sides values are encode the cursor's position in relation to the box
	  using this:

				   4
	           ----------
			   |		|
			1  |		|  2
			   |		|
			   |		|
	           ----------
				   8

	  So the cursor in the top-left corner of the box will be 5 (left+top = 1+4 = 5).
	*/
	static const int SideLookup[25] =
	{
		5,	5,	4,	6,	6,
		5,	5,	4,	6,	6,
		1,	1,	0,	2,	2,
		9,	9,	8,	10,	10,
		9,	9,	8,	10,	10
	};
	static const int SideLookupShear[25] =
	{
		0,	4,	4,	4,	0,
		1,	0,	4,	0,	2,
		1,	1,	0,	2,	2,
		1,	0,	8,	0,	2,
		0,	8,	8,	8,	0
	};
	int Tool = GetTool ();
	int ModeTool = GetModeTool ();
	int AdjustMode = GetAdjustMode ();

	CGEditProDoc* pDoc = GetDocument();

	LMouseButtonDown = GE_TRUE;

	geBoolean SpaceIsDown = IsKeyDown (VK_SPACE);

	assert( IsCopying == FALSE ) ;

	if (!RMouseButtonDown)
	{

/*		if ((mViewIs3d == FALSE) && (AdjustMode == ADJUST_MODE_FACE))
		{
			::MessageBeep( (uint32)-1 ) ;
			return ;
		}
*/

		SetCapture();
		HideTheCursor ();
		mStartPoint = point;

		if (mViewIs3d || IsPanning || SpaceIsDown)
			return ;

		if ((Tool == CURTOOL_NONE))// && (ModeTool == ID_GENERALSELECT) && (AdjustMode == ADJUST_MODE_BRUSH))
		{
			mDragStartPoint = point ;	// Drag mode is initiated in MouseMove
			mDragCurrentPoint = point ;
		}// Drag Select
		else if (mViewIs3d == FALSE)
		{
			int CursorSide;

			CursorSide = GetCursorBoxPos (&point);
			if (Tool == ID_TOOLS_BRUSH_SHEARBRUSH)
			{
				sides = SideLookupShear[CursorSide];
			}
			else
			{
				sides = SideLookup[CursorSide];
			}

			if(Tool == ID_TOOLS_BRUSH_SCALEBRUSH)
			{
				pDoc->m_iScaleNum	=0;
			
				geVec3d_Set (&pDoc->m_vecFinalScale, 1.0f, 1.0f, 1.0f);
				pDoc->TempCopySelectedBrushes();
			}
			else if(Tool == ID_TOOLS_BRUSH_SHEARBRUSH)
			{
				pDoc->m_iScaleNum	=0;
			
				geVec3d_Clear (&pDoc->m_vecFinalScale);
				if (ModeTool == ID_TOOLS_TEMPLATE)
				{
					pDoc->m_pTempShearTemplate	=Brush_Clone(pDoc->m_pCurBrush);
				}
				else
				{
					pDoc->TempCopySelectedBrushes();
				}
			}
			else if ((Tool == ID_TOOLS_BRUSH_MOVEROTATEBRUSH) || (Tool == ID_TOOLS_BRUSH_MOVESELECTEDBRUSHES))
			{
				geVec3d_Clear (&pDoc->m_vecFinalPos);
				pDoc->TempCopySelectedBrushes();
			}

// new g3dc		
			else if ((Tool == ID_TOOLS_BRUSH_ROTATEBRUSH) /*|| (Tool == ID_TOOLS_BRUSH_MOVESELECTEDBRUSHES)*/)
			{
				if (mViewIs3d || IsPanning || SpaceIsDown)
					return ;

				geVec3d_Set (&(pDoc->m_vecFinalRot), 0.0f, 0.0f, 0.0f);
				pDoc->TempCopySelectedBrushes();
			}
//	post	0.57

			if ((ModeTool == ID_GENERALSELECT) && (AdjustMode == ADJUST_MODE_BRUSH))
			{
				if (!CGlobals::g_bRebuildAlways)
				{
					pDoc->m_pMainFrame->SendMessageToDescendants(WM_COMMAND, IDC_BTN_REBUILD_COMMAND);
					CGlobals::g_iRebuildClickCount = 0;
				}
			}
	
		}// Not Drag-select 
	}// LButtonDown only
}
#pragma warning (default:4100)



#pragma warning (disable:4100)
void CGEditProView::OnRButtonDown(UINT nFlags, CPoint point) 
{
//	CGEditProDoc* pDoc = GetDocument();
	geBoolean SpaceIsDown = IsKeyDown (VK_SPACE);

	RMouseButtonDown = GE_TRUE;
	if (!LMouseButtonDown)
	{
		SetCapture();

		HideTheCursor ();

		mStartPoint = point;

		if (mViewIs3d || IsPanning || SpaceIsDown)
			return ;
	}

}
#pragma warning (default:4100)



#pragma warning (disable:4100)
void CGEditProView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);

	RMouseButtonDown = GE_FALSE;
	if (!LMouseButtonDown)
	{
		CGEditProDoc* pDoc = GetDocument();
		
		if(this==GetCapture())
		{
			ReleaseCapture();
		}

		if((IsKeyDown (VK_SPACE)) || IsPanning || GetModeTool()==ID_TOOLS_CAMERA)
		{
			pDoc->TempDeleteSelected(pDoc->DocVarsGetPointer());
			IsPanning	=FALSE;
			OnContextMenu(this, point);	//	new g3dc
			ShowTheCursor ();		
			RedrawWindow();
			return;
		}
	
		ShowTheCursor ();  	
		OnContextMenu(this, point);	//	new g3dc	

		//		AfxMessageBox("Finished processing context menu");
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);	// old gedit. DON'T REBUILD HERE
	}
	
	assert( IsCopying == FALSE ) ;
}
#pragma warning (disable:4100)





// new g3dc

void CGEditProView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//	setup test for Control key held down
	geBoolean	ControlHeld, SpaceHeld;
	ControlHeld = IsKeyDown (VK_CONTROL);
	SpaceHeld	= IsKeyDown (VK_SPACE);

	CGEditProDoc* pDoc = GetDocument();


	//	if the left mouse button is held down (in addition to the 
	//	rigiht mouse button), or if the active view is the textured
	//	view and the Control key is NOT being held down,
	//	or if the spacebar is being held down,
	//	or if we are in Camera Mode and NOT holding down the Ctrl key,
	//	then don't show the context menu
	if (LMouseButtonDown || (mViewIs3d && !ControlHeld) 
		|| 	SpaceHeld 
		||	(pDoc->m_iModeTool == ID_TOOLS_CAMERA && !ControlHeld))
		return;

	ShowTheCursor();		

	//	Adjust point if needed (for keyboard context menu)
	if( point.x == -1 && point.y == -1 )
	{
		CRect rect;
		GetClientRect( &rect );
		point = rect.TopLeft();
		point.Offset( 5, 5 );
		ClientToScreen( &point );
	}

	//  Load top-level menu from resource
	CMenu mnuTop;
	mnuTop.LoadMenu( IDR_POPUP_MENU );

	//  Get popup menu from first sub-menu
	m_pContextMenu = mnuTop.GetSubMenu( 0 );
	ASSERT_VALID( m_pContextMenu );

	//	Checked state for popup menu items is automatically
	//	managed by standard MFC UPDATE_COMMAND_UI mechanism!

	//  Display popup menu
	m_pContextMenu->TrackPopupMenu(	TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		point.x, point.y, AfxGetMainWnd(), NULL );

	//	Popup menu commands are automatically handled
	//	by standard MFC command-routing mechanism!
}



void CGEditProView::RedrawView(CDC *pDC)
{
	CGEditProDoc* pDoc = GetDocument();

	switch(mViewType)
	{
	case ID_VIEW_TEXTUREVIEW:
	case ID_VIEW_3DWIREFRAME:
		//don't draw before texinfos are valid
		if(Render_GetWadSizes(VCam))
		{
			pDoc->RenderWorld(VCam, pDC);
		}
		if(pDoc->IsLeakFileLoaded() && pDoc->bShowLeakFinder())
		{
			DrawLeakPoints3D(pDC->m_hDC, pDoc->GetLeakPoints(), pDoc->GetNumLeakPoints());
		}
		break;
	case ID_VIEW_TOPVIEW:
	case ID_VIEW_SIDEVIEW:
	case ID_VIEW_FRONTVIEW:
		pDoc->RenderOrthoView(VCam, pDC);
		if(pDoc->IsLeakFileLoaded() && pDoc->bShowLeakFinder())
		{
			DrawLeakPoints(pDC->m_hDC, pDoc->GetLeakPoints(), pDoc->GetNumLeakPoints());
		}
		break;
	}

	if( IsDragging && this==GetCapture() )
	{
		// DrawDragRect here just didn't show up against our grid...
		CBrush SelBrush ;
		SelBrush.CreateHatchBrush(HS_BDIAGONAL, CYAN);
		CRect NewRect(mDragStartPoint, mDragCurrentPoint );
		NewRect.NormalizeRect();
		pDC->FrameRect( &NewRect, &SelBrush ) ;
	}

	int m_nGridZoomDepth = 0;			//	for spacing grid dots

	int bkMode = pDC->GetBkMode();
	pDC->SetBkMode(TRANSPARENT);
	int		oldColor;

	//	if this view is the active view, then let's draw a pretty
	//	colored border around it right now.
	if(this==GetParentFrame()->GetActiveView())
	{
		RECT	rect ;
		GetClientRect(&rect) ;	
		CBrush	FrameBrush;
		//	using global color vars
		FrameBrush.CreateSolidBrush(CGlobals::g_cActiveViewFrameColor);
		CBrush *pFrameBrushOld = pDC->SelectObject(&FrameBrush);
		pDC->FrameRect(&rect, &FrameBrush);	
		oldColor = pDC->SetTextColor(CGlobals::g_cActiveViewTextColor);
		pDC->SelectObject(pFrameBrushOld);
		FrameBrush.DeleteObject();
	}
	//	otherwise, make the text in inactive views to be a different
	//	color from the text in the active view.
	else
	{	
		oldColor = pDC->SetTextColor(CGlobals::g_cInactiveViewTextColor); 	
	}

	//	Prepare to draw the axis indicator lines
	//	These have to be drawn last
	if (pDoc)
	{

		CRect	rect ;
		this->GetClientRect(&rect);
		rect.NormalizeRect();
		CPoint	textPoint, axisPoint, xArrowPoint, yArrowPoint;
		textPoint.x = rect.left + 90;
		textPoint.y = rect.bottom - 18;
		axisPoint.x = rect.left + 10;
		axisPoint.y = rect.bottom - 10;
		xArrowPoint.x = axisPoint.x + 40;
		xArrowPoint.y = axisPoint.y;
		yArrowPoint.x = axisPoint.x;
		yArrowPoint.y = axisPoint.y - 40;


		//	create the pen for drawing the Xaxis indicator lines
		CPen XaxisPen;
		XaxisPen.CreatePen(PS_SOLID, 0, RED);//	using global color vars

		//	create the pen for drawing the Yaxis indicator lines
		CPen YaxisPen;
		YaxisPen.CreatePen(PS_SOLID, 0, YELLOW);//	using global color vars

		//	create the pen for drawing the Zaxis indicator lines
		CPen ZaxisPen;
		ZaxisPen.CreatePen(PS_SOLID, 0, CYAN);//	using global color vars

		CPen *pPenOld = pDC->SelectObject(&XaxisPen);


		switch( mViewType ) 
		{
		case ID_VIEW_TOPVIEW:

			//	Here is the text to be displayed as viewport labels
			pDC->TextOut(textPoint.x, textPoint.y, "Top",3);

			//	draw the axis lines
			pDC->MoveTo(axisPoint);	
			pDC->LineTo(xArrowPoint);

			//	draw the letter X at the end of the X-axis line
			pDC->MoveTo(xArrowPoint.x + 12, xArrowPoint.y - 4);
			pDC->LineTo(xArrowPoint.x + 3, xArrowPoint.y + 5);
			pDC->MoveTo(xArrowPoint.x + 4, xArrowPoint.y - 4);
			pDC->LineTo(xArrowPoint.x + 13, xArrowPoint.y + 5);

			pDC->SelectObject(&ZaxisPen);

			//	draw the axis lines	
			pDC->MoveTo(axisPoint);	
			pDC->LineTo(yArrowPoint);

			//	draw the letter Z at the end of the Y-axis line
			pDC->MoveTo(yArrowPoint.x - 4, yArrowPoint.y - 12);
			pDC->LineTo(yArrowPoint.x + 4, yArrowPoint.y - 12);
			pDC->LineTo(yArrowPoint.x - 4, yArrowPoint.y - 4);
			pDC->LineTo(yArrowPoint.x + 5, yArrowPoint.y - 4);

			break;
		case ID_VIEW_SIDEVIEW:

			//	Here is the text to be displayed as viewport labels
			pDC->TextOut(textPoint.x, textPoint.y, "Side",4);

			pDC->SelectObject(&ZaxisPen);

			//	draw the axis lines
			pDC->MoveTo(axisPoint);	
			pDC->LineTo(xArrowPoint);

			//	draw the letter Z at the end of the X-axis line
			pDC->MoveTo(xArrowPoint.x + 4, xArrowPoint.y - 4);
			pDC->LineTo(xArrowPoint.x + 12, xArrowPoint.y - 4);
			pDC->LineTo(xArrowPoint.x + 4, xArrowPoint.y + 4);
			pDC->LineTo(xArrowPoint.x + 13, xArrowPoint.y + 4);

			pDC->SelectObject(&YaxisPen);

			//	draw the axis lines	
			pDC->MoveTo(axisPoint);	
			pDC->LineTo(yArrowPoint);

			//	draw the letter Y at the end of the Y-axis line
			pDC->MoveTo(yArrowPoint.x, yArrowPoint.y - 4);
			pDC->LineTo(yArrowPoint.x, yArrowPoint.y - 8);
			pDC->MoveTo(yArrowPoint.x - 4, yArrowPoint.y - 12);
			pDC->LineTo(yArrowPoint.x, yArrowPoint.y - 8);
			pDC->LineTo(yArrowPoint.x + 5, yArrowPoint.y - 13);

			break;
		case ID_VIEW_FRONTVIEW:

			//	Here is the text to be displayed as viewport labels
			pDC->TextOut(textPoint.x, textPoint.y, "Front",5);

			pDC->SelectObject(&XaxisPen);

			//	draw the axis lines
			pDC->MoveTo(axisPoint);	
			pDC->LineTo(xArrowPoint);

			//	draw the letter X at the end of the X-axis line
			pDC->MoveTo(xArrowPoint.x + 12, xArrowPoint.y - 4);
			pDC->LineTo(xArrowPoint.x + 3, xArrowPoint.y + 5);
			pDC->MoveTo(xArrowPoint.x + 4, xArrowPoint.y - 4);
			pDC->LineTo(xArrowPoint.x + 13, xArrowPoint.y + 5);

			pDC->SelectObject(&YaxisPen);

			//	draw the axis lines	
			pDC->MoveTo(axisPoint);	
			pDC->LineTo(yArrowPoint);

			//	draw the letter Y at the end of the Y-axis line
			pDC->MoveTo(yArrowPoint.x, yArrowPoint.y - 4);
			pDC->LineTo(yArrowPoint.x, yArrowPoint.y - 8);
			pDC->MoveTo(yArrowPoint.x - 4, yArrowPoint.y - 12);
			pDC->LineTo(yArrowPoint.x, yArrowPoint.y - 8);
			pDC->LineTo(yArrowPoint.x + 5, yArrowPoint.y - 13);

			break;
		}

		pDC->SelectObject(pPenOld);		//	cleanup
		XaxisPen.DeleteObject();		//	cleanup
		YaxisPen.DeleteObject();		//	cleanup
		ZaxisPen.DeleteObject();		//	cleanup

	}	//	if (pDoc)

	pDC->SetBkMode(bkMode);
	pDC->SetTextColor(oldColor);

}



//////////////////////////////////////////////////////////////////////////////////////
//	CalcSizeTotalAreaWnd
//	
//////////////////////////////////////////////////////////////////////////////////////
static void CalcSizeTotalAreaWnd( CDC* pDC, const CGEditProView* pView, 
								 CRect& totalAreaWnd )
{
	pView->GetClientRect( totalAreaWnd );
}




//////////////////////////////////////////////////////////////////////////////////////
//	OnEraseBkgnd
//	
//////////////////////////////////////////////////////////////////////////////////////
BOOL CGEditProView::OnEraseBkgnd(CDC* pDC) 
{
	CWnd::OnEraseBkgnd(pDC);
	return FALSE;
	//	return CView::OnEraseBkgnd(pDC);
}


//////////////////////////////////////////////////////////////////////////////////////
//	RedrawBackground
//	
//////////////////////////////////////////////////////////////////////////////////////
bool CGEditProView::RedrawBackground(CDC* pDC) 
{
	CRect rect ;

	if ((mViewType == ID_VIEW_TOPVIEW) || (mViewType == ID_VIEW_SIDEVIEW) || (mViewType == ID_VIEW_FRONTVIEW))
	{
		GetClientRect( &rect ) ;
		pDC->FillSolidRect( &rect, Prefs_GetBackgroundColor (((CGEditProApp *)AfxGetApp ())->GetPreferences ()));
	}
	return true;
}



//////////////////////////////////////////////////////////////////////////////////////
//	OnDraw
//	
//////////////////////////////////////////////////////////////////////////////////////
void CGEditProView::OnDraw(CDC* pDC)
{
	CString	thisFunctionName = "Ct_EDIT_VIEW_ONDRAW";

	CRect totalAreaWnd;				//	prepare for flicker-free view drawing
	CalcSizeTotalAreaWnd( pDC, this, totalAreaWnd );
									//	Create a memory bufffer DC and draw to THAT
	CMemDC memDC(pDC, totalAreaWnd, totalAreaWnd, &m_ViewBuffer );
									
	if (m_ViewBuffer.IsDirty())		//	If it's empty or uninitialized, let's draw to it
	{
		RedrawBackground(memDC.GetSafeCDC());	//	background first
		RedrawView( memDC.GetSafeCDC());		//	then foreground
												//	once all drawing is done,
		memDC.CopyToScreen(0);					//	flip the buffer to the screen
	}
}




void CGEditProView::OnInitialUpdate() 
{
	RECT r;
	CGEditProDoc*	pDoc = (CGEditProDoc*) GetDocument();
	SizeInfo *WadSizeInfos = pDoc->m_pLevelMgr->GetWadSizeInfos (pDoc->GetLevel());
	int iView;

	CView::OnInitialUpdate();
	GetClientRect(&r);

	if(WadSizeInfos)
	{
		Render_SetWadSizes(VCam, WadSizeInfos);
		Render_ResetSettings(VCam, r.right, r.bottom);
	}

	//	post 0.57	give user option to rebuild
	//	specs for Rebuild button on the view
	CRect btnRect(3,5,58,25);
	DWORD BUTTON_STYLE = WS_CHILD|WS_VISIBLE|BS_PUSHLIKE|BS_PUSHBUTTON|BS_TOP|BS_FLAT;

	switch(mViewType)
	{
	case ID_VIEW_TEXTUREVIEW:
		Render_SetViewType(VCam, VIEWTEXTURE);
		iView = 0;
		//	post 0.57	give user option to rebuild
		//	create  Rebuild button on the view
		m_btnRebuild.Create("Rebuild?",BUTTON_STYLE, btnRect, this, IDC_BTN_VIEW_REBUILD);
		m_btnRebuild.SetFont(&CGlobals::m_ControlBarFont);
		m_btnRebuild.ShowWindow(SW_HIDE);
		//	post 0.57

		break;
	case ID_VIEW_3DWIREFRAME:
		Render_SetViewType(VCam, VIEWWIRE);
		iView = 0;
		break;
	case ID_VIEW_TOPVIEW:
		Render_SetViewType(VCam, VIEWTOP);
		iView = 1;
		break;
	case ID_VIEW_SIDEVIEW:
		Render_SetViewType(VCam, VIEWSIDE);
		iView = 3;
		break;
	case ID_VIEW_FRONTVIEW:
		Render_SetViewType(VCam, VIEWFRONT);
		iView = 2;
		break;
	default :
		iView = -1;
		break;
	}

	GetParentFrame()->SetWindowText(pDoc->GetTitle());

	if (iView != -1)
	{
		// Update view state information that was saved in level
		ViewStateInfo *pViewStateInfo = NULL;

		CtLevel *pTempLevel = NULL;
		pTempLevel = pDoc->GetLevel();
		if (pTempLevel)
		{
			pViewStateInfo = pDoc->m_pLevelMgr->GetViewStateInfo (pTempLevel, iView);
			if (pViewStateInfo->IsValid)
			{
				Render_SetZoom (VCam, pViewStateInfo->ZoomFactor);
				Render_SetPitchRollYaw (VCam, &pViewStateInfo->PitchRollYaw);
				Render_SetCameraPos (VCam, &pViewStateInfo->CameraPos);
			}	
		}
	}

	pDoc->UpdateGridInformation ();
}

/////////////////////////////////////////////////////////////////////////////
// CGEditProView construction/destruction
CGEditProView::CGEditProView()
{
	VCam	=Render_AllocViewVars();
	Render_SetWadSizes(VCam, NULL);

	mStartPoint.x=mStartPoint.y=0;

	mViewType = ID_VIEW_NEW;

	IsDragging = IsCopying = IsPanning = FALSE;
	LMouseButtonDown = GE_FALSE;
	RMouseButtonDown = GE_FALSE;

	mCY_DRAG = ::GetSystemMetrics( SM_CYDRAG ) ;
	mCX_DRAG = ::GetSystemMetrics( SM_CXDRAG ) ;

	m_pContextMenu = NULL;
	m_bActive = FALSE;				//	view does not have focus 
}

CGEditProView::~CGEditProView()
{
	Render_FreeViewVars(VCam);
	if (VCam != NULL)
	{
		geRam_Free (VCam);
	}
}

BOOL CGEditProView::PreCreateWindow(CREATESTRUCT& cs)
{
	//get rid of default cursor
//	cs.lpszClass = AfxRegisterWndClass( CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
//		NULL, (HBRUSH)GetStockObject(GRAY_BRUSH));

	cs.lpszClass = AfxRegisterWndClass( CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
		NULL, (HBRUSH)GetStockObject(NULL_BRUSH));	//	post 0.58 support memDC
	

	return CView::PreCreateWindow(cs);
}

CGEditProDoc* CGEditProView::GetDocument()
{
	static CGEditProDoc	*pOldDoc;	//	post 0.57 checking for doc switching
	CGEditProDoc *pDoc;
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	//	post 0.57
	
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGEditProDoc)));
	pDoc = (CGEditProDoc *)m_pDocument;
	ASSERT_VALID (pDoc);
	
	
	//	post 0.57
	//	This little routine resets the correct template dialog box onto the Propeties 
	//	Panel after switching from one document to another.
	if (pDoc && pMainFrame && pOldDoc)
	{
		if (pDoc != pOldDoc)
		{
			if (pDoc == CGlobals::GetActiveDocument())
			{
				if (pDoc->m_iModeTool == ID_TOOLS_TEMPLATE)
				{
					if ((pDoc->m_currentTemplateName == "") ||  (pDoc->m_currentTemplateName == "Cube"))
						pDoc->CreateCube();
					if (pDoc->m_currentTemplateName == "Arch")
						pDoc->CreateArch();
					if (pDoc->m_currentTemplateName == "Cone")
						pDoc->CreateCone();
					if (pDoc->m_currentTemplateName == "Cylinder")
						pDoc->CreateCylinder();
					if (pDoc->m_currentTemplateName == "Sphere")
						pDoc->CreateSpheroid();
					if (pDoc->m_currentTemplateName == "Stairs")
						pDoc->CreateStaircase();
				}
			}
		}
	}
	
	pOldDoc = pDoc;	//	post 0.57	checking for doc-switching
	
	return pDoc;
}

void CGEditProView::OnSize(UINT nType, int cx, int cy) 
{
	CGEditProDoc* pDoc = GetDocument();
	SizeInfo *WadSizeInfos = pDoc->m_pLevelMgr->GetWadSizeInfos (pDoc->GetLevel());

	// call our oldself
	CView::OnSize(nType, cx, cy);
	
	// make sure that our camera knows our current size
	if(WadSizeInfos)
	{
		Render_SetWadSizes(VCam, WadSizeInfos);
		Render_ResizeView (VCam, cx, cy);
	}
}

void CGEditProView::OnToolsCamera() 
{
	CGEditProDoc* pDoc = GetDocument();

	SetModeTool(ID_TOOLS_CAMERA);

	pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	pDoc->m_pMainFrame->m_TemplatesTab.UpdateTemplates(pDoc);			//	new g3dc
}

void CGEditProView::OnUpdateToolsCamera(CCmdUI* pCmdUI) 
{
	if( GetModeTool() == ID_TOOLS_CAMERA )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);
}


void CGEditProView::OnAxisX() 
{
	CGEditProDoc* pDoc = GetDocument();

	pDoc->SetLockAxis( pDoc->GetLockAxis() ^ AXIS_X ) ;
}


void CGEditProView::OnUpdateAxisX(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();

	if( pDoc->GetLockAxis() & AXIS_X )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);
}


void CGEditProView::OnAxisY() 
{
	CGEditProDoc* pDoc = GetDocument();

	pDoc->SetLockAxis( pDoc->GetLockAxis() ^ AXIS_Y ) ;
}


void CGEditProView::OnUpdateAxisY(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();
	
	if( pDoc->GetLockAxis() & AXIS_Y )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);
}


void CGEditProView::OnAxisZ() 
{
	CGEditProDoc* pDoc = GetDocument();

	pDoc->SetLockAxis( pDoc->GetLockAxis() ^ AXIS_Z ) ;
}


void CGEditProView::OnUpdateAxisZ(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();
	
	if( pDoc->GetLockAxis() & AXIS_Z )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);
}


void CGEditProView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CGEditProDoc* pDoc = GetDocument();

	BOOL bStateChanged = (m_bActive != bActivate);
	m_bActive = bActivate;

	if (bStateChanged)
	{
		RedrawWindow();
	}
	// set our title
	GetParentFrame()->SetWindowText(pDoc->GetTitle());

	// make sure the bar is updated for our doc.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	//	If the application terminates after running a command line
	//	request, we don't want to update this combo box...
	if( pFrame->IsDestroyingApp )
		pFrame->UpdateMainControls();

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);


}


void CGEditProView::OnToolsBrushMoverotatebrush() 
{
	CGEditProDoc* pDoc = GetDocument();

	int mode=GetModeTool();

	if(mode==ID_TOOLS_TEMPLATE)
	{
		SetTool( ID_TOOLS_BRUSH_MOVEROTATEBRUSH );
		pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	} 
	else 
	{
		if(GetTool()==ID_TOOLS_BRUSH_MOVEROTATEBRUSH)
		{
			SetTool(CURTOOL_NONE);
			SetAdjustMode (ADJUST_MODE_BRUSH);
		} 
		else 
		{
			SetTool(ID_TOOLS_BRUSH_MOVEROTATEBRUSH);
		}
		pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	}
}

void CGEditProView::OnUpdateToolsBrushMoverotatebrush(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();

	//that's a pretty big if
	if(GetModeTool()==ID_TOOLS_TEMPLATE ||
		(GetModeTool()==ID_GENERALSELECT &&
		GetAdjustMode()==ADJUST_MODE_BRUSH &&
		pDoc->GetSelState()!=NOSELECTIONS)) 
	{
		pCmdUI->Enable();
		if(GetTool()==ID_TOOLS_BRUSH_MOVEROTATEBRUSH)
		{
			pCmdUI->SetCheck();
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	} 
	else 
	{
		pCmdUI->Enable(0);
		pCmdUI->SetCheck(0);
	}
}




void CGEditProView::OnToolsBrushRotatebrush() 
{
	CGEditProDoc* pDoc = GetDocument();

	int mode=GetModeTool();

	if(mode==ID_TOOLS_TEMPLATE)
	{
		SetTool( ID_TOOLS_BRUSH_ROTATEBRUSH );
		pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	} 
	else 
	{
		if(GetTool()==ID_TOOLS_BRUSH_ROTATEBRUSH)
		{
			SetTool(CURTOOL_NONE);
			SetAdjustMode (ADJUST_MODE_BRUSH);
		} 
		else 
		{
			SetTool(ID_TOOLS_BRUSH_ROTATEBRUSH);
		}
		pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	}
	
}




void CGEditProView::OnUpdateToolsBrushRotatebrush(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();

	//that's a pretty big if
	if(GetModeTool()==ID_TOOLS_TEMPLATE ||
		(GetModeTool()==ID_GENERALSELECT &&
		GetAdjustMode()==ADJUST_MODE_BRUSH &&
		pDoc->GetSelState()!=NOSELECTIONS)) 
	{
		pCmdUI->Enable();
		if(GetTool()==ID_TOOLS_BRUSH_ROTATEBRUSH)
		{
			pCmdUI->SetCheck();
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	} 
	else 
	{
		pCmdUI->Enable(0);
		pCmdUI->SetCheck(0);
	}
	
}




void CGEditProView::OnToolsBrushScalebrush() 
{
	CGEditProDoc* pDoc = GetDocument();

	int mode=GetModeTool();

	if(mode==ID_TOOLS_TEMPLATE)
	{
		SetTool(ID_TOOLS_BRUSH_SCALEBRUSH);
		pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	} 
	else 
	{
		if(GetTool()==ID_TOOLS_BRUSH_SCALEBRUSH)
		{
			SetTool(CURTOOL_NONE);
			SetAdjustMode (ADJUST_MODE_BRUSH);
		} 
		else 
		{
			SetTool(ID_TOOLS_BRUSH_SCALEBRUSH);
		}
		pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	}
}

void CGEditProView::OnUpdateToolsBrushScalebrush(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();

	//that's a very big if
	if((GetModeTool()==ID_TOOLS_TEMPLATE && !pDoc->m_bTempEnt) ||
		(GetModeTool()==ID_GENERALSELECT &&
		GetAdjustMode ()==ADJUST_MODE_BRUSH &&
		SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) > 0))
	{
		pCmdUI->Enable();
		if(GetTool()==ID_TOOLS_BRUSH_SCALEBRUSH)
		{
			pCmdUI->SetCheck();
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	} 
	else 
	{
		pCmdUI->Enable(0);
		pCmdUI->SetCheck(0);
	}
}

void CGEditProView::OnToolsBrushShowbrush() 
{

	CGEditProDoc* pDoc = GetDocument();

	// toggle brush
	pDoc->m_iShowBrush ^= 1;

	// redraw the screen
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}

void CGEditProView::OnUpdateToolsBrushShowbrush(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();

	pCmdUI->SetCheck (pDoc->m_iShowBrush);
}




// This is the range handler for the types of view that we have
// make sure when we add more view types that we update this.
void CGEditProView::OnViewType(UINT nID)
{
	CGEditProDoc	*pDoc	=GetDocument();
	mViewType	=nID;
	SizeInfo *WadSizeInfos = pDoc->m_pLevelMgr->GetWadSizeInfos (pDoc->GetLevel());

	if(WadSizeInfos)
	{
		geVec3d SaveCameraPos;
		geVec3d SaveOrientation;
		geFloat ZoomFactor;
		int Width, Height;

		ZoomFactor = Render_GetZoom (VCam);
		Render_GetCameraPos (VCam, &SaveCameraPos);
		Render_GetPitchRollYaw (VCam, &SaveOrientation);
		Width = Render_GetWidth (VCam);
		Height = Render_GetHeight (VCam);

		Render_SetWadSizes(VCam, WadSizeInfos);
		Render_ResetSettings(VCam, Render_GetWidth(VCam), Render_GetHeight(VCam));
		Render_ResizeView (VCam, Width, Height);
		Render_SetCameraPos (VCam, &SaveCameraPos);
		Render_SetPitchRollYaw (VCam, &SaveOrientation);
		if (ZoomFactor != 0.0f)
		{
			Render_SetZoom (VCam, ZoomFactor);
		}
	}
	switch(mViewType)
	{
	case ID_VIEW_TEXTUREVIEW:
		Render_SetViewType(VCam, VIEWTEXTURE);
		mViewIs3d = TRUE ;
		break;
	case ID_VIEW_3DWIREFRAME:
		Render_SetViewType(VCam, VIEWWIRE);
		mViewIs3d = TRUE ;
		break;
	case ID_VIEW_TOPVIEW:
		Render_SetViewType(VCam, VIEWTOP);
		mViewIs3d = FALSE ;
		break;
	case ID_VIEW_SIDEVIEW:
		Render_SetViewType(VCam, VIEWSIDE);
		mViewIs3d = FALSE ;
		break;
	case ID_VIEW_FRONTVIEW:
		Render_SetViewType(VCam, VIEWFRONT);
		mViewIs3d = FALSE ;
		break;
	}
	pDoc->UpdateGridInformation ();

	RedrawWindow();
	GetParentFrame()->SetWindowText(pDoc->GetTitle());
}

void CGEditProView::OnViewTypeCmdUi(CCmdUI* pCmdUI)
{
	BOOL	bEnable = TRUE ;

	if( !mViewIs3d )	// If this is an otho view, don't allow mutate to rendered
	{
		if( pCmdUI->m_nID == ID_VIEW_TEXTUREVIEW || 
			pCmdUI->m_nID == ID_VIEW_3DWIREFRAME )
			bEnable = FALSE ;
	}
	else				// If this is a rendered view, don't allow mutate to ortho
	{
		if( pCmdUI->m_nID == ID_VIEW_TOPVIEW || 
			pCmdUI->m_nID == ID_VIEW_SIDEVIEW || 
			pCmdUI->m_nID == ID_VIEW_FRONTVIEW )
			bEnable = FALSE ;
	}
	
	if( mViewType == pCmdUI->m_nID )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);

	pCmdUI->Enable( bEnable ) ;
}

void CGEditProView::SetTitle()
{
	switch(mViewType)
	{
	case ID_VIEW_NEW:
		GetParentFrame()->SetWindowText("-Creating New View-");
		break;
	case ID_VIEW_3DWIREFRAME:
		GetParentFrame()->SetWindowText("-3D Wireframe-");
		break;
	case ID_VIEW_TEXTUREVIEW:
		GetParentFrame()->SetWindowText("-Texture View-");
		break;
	case ID_VIEW_TOPVIEW:
		GetParentFrame()->SetWindowText("-Top View-");
		break;
	case ID_VIEW_FRONTVIEW:
		GetParentFrame()->SetWindowText("-Front View-");
		break;
	case ID_VIEW_SIDEVIEW:
		GetParentFrame()->SetWindowText("-Side View-");
		break;
	}
}

void CGEditProView::OnToolsBrushShearbrush() 
{
	CGEditProDoc* pDoc = GetDocument();

	int mode=GetModeTool();

	if(mode==ID_TOOLS_TEMPLATE)
	{
		SetTool( ID_TOOLS_BRUSH_SHEARBRUSH);
		pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	} 
	else 
	{
		if(GetTool()==ID_TOOLS_BRUSH_SHEARBRUSH)
		{
			SetTool(CURTOOL_NONE);
			SetAdjustMode (ADJUST_MODE_BRUSH);
		}
		else 
		{
			SetTool(ID_TOOLS_BRUSH_SHEARBRUSH);
		}
		pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
	}
}


void CGEditProView::OnUpdateToolsBrushShearbrush(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();

	//that's a very big if
	if((GetModeTool()==ID_TOOLS_TEMPLATE && !pDoc->m_bTempEnt) ||
		(GetModeTool()==ID_GENERALSELECT &&
		GetAdjustMode()==ADJUST_MODE_BRUSH &&
#pragma message ("Can't do multiple brush shear due to Brush_Shear implementation.")
		SelBrushList_GetSize (pDoc->DocVarsGetPointer()->m_pSelBrushes) == 1)) 
	{
		pCmdUI->Enable();
		if(GetTool()==ID_TOOLS_BRUSH_SHEARBRUSH)
		{
			pCmdUI->SetCheck();
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	} 
	else 
	{
		pCmdUI->Enable(0);
		pCmdUI->SetCheck(0);
	}
}

int CGEditProView::GetTool(void)
{
	CGEditProDoc* pDoc = GetDocument();

	return pDoc->m_iCurrentTool;
}

fdocAdjustEnum CGEditProView::GetAdjustMode(void)
{
	CGEditProDoc* pDoc = GetDocument();
	
	return pDoc->m_AdjustMode;
}

int CGEditProView::GetModeTool(void)
{
	CGEditProDoc* pDoc = GetDocument();
	
	return pDoc->m_iModeTool;
}

void CGEditProView::SetTool(int Tool)
{
	CGEditProDoc* pDoc = GetDocument();
	
	pDoc->m_iCurrentTool = Tool;
}

void CGEditProView::SetAdjustMode(fdocAdjustEnum Mode)
{
	CGEditProDoc* pDoc = GetDocument();
	
	pDoc->m_AdjustMode = Mode;
}

void CGEditProView::SetModeTool(int Tool)
{
	CGEditProDoc* pDoc = GetDocument();
	
	pDoc->m_iModeTool = Tool;
}


void CGEditProView::OnUpdateBrushGroupsMakenewgroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

void CGEditProView::OnBrushGroupsAddtogroup() 
{
	CGEditProDoc* pDoc = GetDocument();
	
	pDoc->AddSelToGroup() ;

	pDoc->m_pMainFrame->UpdateMainControls();		//	new g3dc
}

void CGEditProView::OnUpdateBrushGroupsAddtogroup(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();
	
	pCmdUI->Enable (!(pDoc->GetSelState() == NOSELECTIONS));
}

void CGEditProView::OnBrushRemoveselectedfromgroup() 
{
	CGEditProDoc* pDoc = GetDocument();
	
	pDoc->RemovesSelFromGroup() ;
	pDoc->m_pMainFrame->UpdateMainControls();		//	new g3dc

}/* CGEditProView::OnBrushRemoveselectedfromgroup */

void CGEditProView::OnUpdateBrushRemoveselectedfromgroup(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();

	if( (pDoc->m_iCurrentGroup == 0) || (pDoc->GetSelState() == NOSELECTIONS) )
		pCmdUI->Enable( FALSE ) ;
	else
		pCmdUI->Enable( TRUE ) ;
}

void CGEditProView::OnToolsBrushMoveselectedbrushes() 
{
	SetTool(ID_TOOLS_BRUSH_MOVESELECTEDBRUSHES);

	CGEditProDoc* pDoc = GetDocument();
	
	pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()
}

void CGEditProView::OnUpdateToolsBrushMoveselectedbrushes(CCmdUI* pCmdUI) 
{
	if(GetModeTool()!=ID_TOOLS_CAMERA) 
	{
		pCmdUI->Enable();
		pCmdUI->SetCheck (GetTool () == ID_TOOLS_BRUSH_MOVESELECTEDBRUSHES);
	} 
	else 
	{
		pCmdUI->Enable(0);
		pCmdUI->SetCheck(0);
	}
}

void CGEditProView::OnToolsTemplate() 
{
	CGEditProDoc* pDoc = GetDocument();

	pDoc->ResetAllSelectedEntities();	//	calls DoGeneralSelect which calls
										//	ConfigureCurrentTool(), which calls
										//	UpdateAllViews()
	pDoc->ResetAllSelectedFaces();
	pDoc->ResetAllSelectedBrushes();

	SetModeTool(ID_TOOLS_TEMPLATE);
	if(pDoc->m_bTempEnt) 
	{
		SetTool( ID_TOOLS_BRUSH_MOVEROTATEBRUSH );
	}
	else 
	{
		SetTool( ID_TOOLS_BRUSH_MOVEROTATEBRUSH );
	}
	pDoc->SetAdjustmentMode( ADJUST_MODE_BRUSH ) ;
	pDoc->ConfigureCurrentTool();	//	calls UpdateAllViews()

				//	new g3dc
				//	sets command panel tab to Templates
	if (pDoc->m_pMainFrame->m_CommandDialogBar)
	{
		CTabCtrl*	pCommandPanelTabCtrl = 
		(CTabCtrl*)pDoc->m_pMainFrame->m_CommandDialogBar.GetDlgItem(IDC_COMMAND_TAB);
		pCommandPanelTabCtrl->SetCurSel((MAINFRM_COMMANDPANEL_TAB)MAINFRM_COMMANDPANEL_TEMPLATES);
		pDoc->m_pMainFrame->SetCommandPanelTab(MAINFRM_COMMANDPANEL_TEMPLATES);
	}

	pDoc->m_pMainFrame->UpdateMainControls();	//	new g3dc
}

void CGEditProView::OnUpdateToolsTemplate(CCmdUI* pCmdUI) 
{
	if( GetModeTool() == ID_TOOLS_TEMPLATE ) pCmdUI->SetCheck();
	else pCmdUI->SetCheck(0);
}

void CGEditProView::OnUpdateToolsBrushRotate45(CCmdUI* pCmdUI) 
{
	CGEditProDoc* pDoc = GetDocument();

	//that's a pretty big if
	if((GetModeTool()==ID_TOOLS_TEMPLATE && !pDoc->m_bTempEnt) || 
	   (GetModeTool()==ID_GENERALSELECT &&
		GetAdjustMode()==ADJUST_MODE_BRUSH &&
		pDoc->GetSelState()!=NOSELECTIONS))
	{
		pCmdUI->Enable();
	}
	else 
	{
		pCmdUI->Enable(0);
	}
}

void CGEditProView::DrawLeakPoints(HDC ViewDC, geVec3d *LeakPoints, int NumLeakPoints)
{
	POINT	pnt = {0,0};
	POINT	nullpnt;
	int		i;
	CPen	PenRed(PS_SOLID, 1, RGB(255,0,0));
	HPEN	oldpen;

	assert(LeakPoints != NULL);
	assert(NumLeakPoints > 0);

	oldpen	=(HPEN)SelectObject(ViewDC, (HPEN)PenRed);

	for(i=0;i < NumLeakPoints-1;i++)
	{
		pnt	=Render_OrthoWorldToView(VCam, &LeakPoints[i]);
		MoveToEx(ViewDC, pnt.x, pnt.y, &nullpnt);

		pnt	=Render_OrthoWorldToView(VCam, &LeakPoints[i+1]);
		LineTo(ViewDC, pnt.x, pnt.y);
	}
	LineTo(ViewDC, pnt.x, pnt.y);

	SelectObject(ViewDC, oldpen);
}

void CGEditProView::DrawLeakPoints3D(HDC ViewDC, geVec3d *LeakPoints, int NumLeakPoints)
{
	POINT	nullpnt;
	int		i;
	CPen	PenRed(PS_SOLID, 1, RGB(255,0,0));
	HPEN	oldpen;
	geVec3d	pnt3;

	assert(LeakPoints != NULL);
	assert(NumLeakPoints > 0);

	oldpen	=(HPEN)SelectObject(ViewDC, (HPEN)PenRed);
	geVec3d_Clear (&pnt3);

	for(i=0;i < NumLeakPoints-1;i++)
	{
		pnt3	=Render_XFormVert(VCam, &LeakPoints[i]);

		if(pnt3.Z < 0.0f)
			continue;

		MoveToEx(ViewDC, Units_Round(pnt3.X), Units_Round(pnt3.Y), &nullpnt);

		pnt3	=Render_XFormVert(VCam, &LeakPoints[i+1]);
		LineTo(ViewDC, Units_Round(pnt3.X), Units_Round(pnt3.Y));
	}
	if(pnt3.Z > 1.0f)
		LineTo(ViewDC, Units_Round(pnt3.X), Units_Round(pnt3.Y));

	SelectObject(ViewDC, oldpen);
}


void CGEditProView::OnDeselectall() 
{
	CGEditProDoc* pDoc = GetDocument();

	pDoc->ResetAllSelections() ;
	pDoc->UpdateSelected();	//	calls UpdateMainControls()
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL ) ;
}

void CGEditProView::OnUpdateDeselectall(CCmdUI* pCmdUI) 
{
	BOOL		bEnable ;
	CGEditProDoc* pDoc = GetDocument();
	
	bEnable = ( pDoc->GetSelState() == NOSELECTIONS ) ? FALSE : TRUE ;
	pCmdUI->Enable( bEnable ) ;
}

void CGEditProView::OnSelectall() 
{
	CGEditProDoc* pDoc = GetDocument();
	
	pDoc->SelectAll () ;
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	pDoc->UpdateAllViews( UAV_ALL3DVIEWS, NULL ) ;
}

void CGEditProView::OnUpdateSelectall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE ) ;
}


LRESULT CGEditProView::OnCompileMessage (WPARAM wParam, LPARAM lParam)
{
	if (wParam == COMPILER_PROCESSID)
	{
		char *msg;

		msg = (char *)lParam;
//		ConPrintf ("%s", msg);		//	oldgedit
		CGlobals::GetActiveDocument()->m_pMainFrame->ConPrintf("%s",msg);		//	new g3dc
		geRam_Free (msg);
	}
	return 0;
}

LRESULT CGEditProView::OnCompileError (WPARAM wParam, LPARAM lParam)
{
	if (wParam == COMPILER_PROCESSID)
	{
		char *msg;

		msg = (char *)lParam;
		CGlobals::GetActiveDocument()->m_pMainFrame->ConError ("%s", msg);
		geRam_Free (msg);
	}
	return 0;
}

LRESULT CGEditProView::OnCompileDone (WPARAM wParam, LPARAM lParam)
{
	if (wParam == COMPILER_PROCESSID)
	{
		CGEditProDoc *pDoc;

		pDoc = GetDocument ();

		if (pDoc != NULL)
		{
			pDoc->CompileDone ((CompilerErrorEnum)lParam);
		}
	}
	return 0;
}

void CGEditProView::OnToolsScaleworld() 
{
	CGEditProDoc* pDoc = GetDocument();
	if (pDoc)		//	post 0.57 redirect to level options dialog box
		SendMessage(WM_COMMAND,IDM_LEVELOPTIONS);	//	post 0.57
}

void CGEditProView::OnToolsBrushMakenewest() 
{
	CGEditProDoc *pDoc = GetDocument ();

	pDoc->MakeSelectedBrushNewest();
}

void CGEditProView::OnToolsSettexturescale() 
{
	CGEditProDoc* pDoc = GetDocument();
	float	scf;
	CString	szKey, szVal;
	int		ModalResult;
	CDialog	*pEditDialog;

	szKey = "Enter texture scale for selected brushes";
	szVal = "1.0";
	pEditDialog = new CFloatKeyEditDlg(this, szKey, &szVal);
	if (pEditDialog != NULL)
	{
		ModalResult = pEditDialog->DoModal();
		delete pEditDialog;
		if(ModalResult == IDOK)
		{
			sscanf((LPCSTR)szVal, "%f", &scf);
			pDoc->SetAllFacesTextureScale(scf);
		}
	}
}

void CGEditProView::OnToolsNextbrush() 
{
	CGEditProDoc *pDoc = GetDocument ();
	BrushList *BList = pDoc->m_pLevelMgr->GetBrushes (pDoc->GetLevel());

	if(GetModeTool()==ID_GENERALSELECT && !pDoc->IsSelectionLocked())
	{
		switch (pDoc->m_AdjustMode)
		{
			case ADJUST_MODE_FACE :
			{
				int nSelectedFaces = SelFaceList_GetSize (pDoc->DocVarsGetPointer()->m_pSelFaces);
				Face *pFace;

				if (nSelectedFaces == 0)
				{
					BrushIterator bi;

					pDoc->m_pCurBrush = BrushList_GetFirst (BList, &bi);
					pFace = Brush_SelectFirstFace (pDoc->m_pCurBrush);
					SelBrushList_Add (pDoc->DocVarsGetPointer()->m_pSelBrushes, pDoc->m_pCurBrush);
				}
				else
				{
					Brush *pBrush;

					// get first selected face
					pFace = SelFaceList_GetFace (pDoc->DocVarsGetPointer()->m_pSelFaces, nSelectedFaces-1);
					// Remove all face selections
					pDoc->ResetAllSelectedFaces ();

					Face_SetSelected (pFace, GE_TRUE);
					pBrush = BrushList_FindTopLevelFaceParent (pDoc->m_pLevelMgr->GetBrushes (pDoc->GetLevel()), pFace);

					// select next face
					if(!Brush_SetNextSelectedFace(pBrush))
					{
						pFace = Brush_SelectFirstFace(pBrush);
					}
					else
					{
						pFace = Brush_GetSelectedFace (pBrush);
					}
				}
				SelFaceList_Add (pDoc->DocVarsGetPointer()->m_pSelFaces, pFace);
				pDoc->UpdateSelected ();	// calls UpdateMainControls()
				
				//	Be very careful when speccing flags for UpdateAllViews()
				//	The wrong flags at the wrong time will totally screw things up
				pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
				break;
			}

			case ADJUST_MODE_BRUSH :
				if(pDoc->GetSelState()&ONEBRUSH)
				{
					SelBrushList_RemoveAll (pDoc->DocVarsGetPointer()->m_pSelBrushes);
					SelBrushList_Add (pDoc->DocVarsGetPointer()->m_pSelBrushes, Brush_GetNextBrush(pDoc->m_pCurBrush, BList));
					pDoc->UpdateSelected();
				
					//update the brush attributes dialog...
					pDoc->UpdateBrushAttributesDlg ();
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
				}
				else if(!(pDoc->GetSelState() & ANYBRUSH))
				{
					Brush *pBrush;
					BrushIterator bi;

					pBrush = BrushList_GetFirst (BList, &bi);
					if(pBrush != NULL)
					{
						SelBrushList_Add (pDoc->DocVarsGetPointer()->m_pSelBrushes, pBrush);
						pDoc->UpdateSelected();
						//	Be very careful when speccing flags for UpdateAllViews()
						//	The wrong flags at the wrong time will totally screw things up
						pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
					}
				}
				break;

			default :
				assert (0);  // bad adjust mode
				break;
		}
	}
}

void CGEditProView::OnToolsPrevbrush() 
{
	CGEditProDoc *pDoc = GetDocument ();
	BrushList *BList = pDoc->m_pLevelMgr->GetBrushes (pDoc->GetLevel());

	if(GetModeTool()==ID_GENERALSELECT && !pDoc->IsSelectionLocked())
	{
		switch (pDoc->m_AdjustMode)
		{
			case ADJUST_MODE_FACE :
			{
				int nSelectedFaces = SelFaceList_GetSize (pDoc->DocVarsGetPointer()->m_pSelFaces);
				Face *pFace;

				if (nSelectedFaces == 0)
				{
					BrushIterator bi;

					pDoc->m_pCurBrush = BrushList_GetFirst (BList, &bi);
					pFace = Brush_SelectFirstFace (pDoc->m_pCurBrush);
					SelBrushList_Add (pDoc->DocVarsGetPointer()->m_pSelBrushes, pDoc->m_pCurBrush);
				}
				else
				{
					Brush *pBrush;

					// get the last selected face
					pFace = SelFaceList_GetFace (pDoc->DocVarsGetPointer()->m_pSelFaces, 0);

					// Remove all face selections
					pDoc->ResetAllSelectedFaces ();

					// Select the next face in order, using selected brush list...
					pBrush = BrushList_FindTopLevelFaceParent (pDoc->m_pLevelMgr->GetBrushes (pDoc->GetLevel()), pFace);
					Face_SetSelected (pFace, GE_TRUE);

					// select next face
					if(!Brush_SetPrevSelectedFace(pBrush))
					{
						pFace = Brush_SelectLastFace(pBrush);
					}
					else
					{
						pFace = Brush_GetSelectedFace (pBrush);
					}
				}
				SelFaceList_Add (pDoc->DocVarsGetPointer()->m_pSelFaces, pFace);
				pDoc->UpdateSelected ();	// calls UpdateMainControls()
				
				//	Be very careful when speccing flags for UpdateAllViews()
				//	The wrong flags at the wrong time will totally screw things up
				pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
				break;
			}

			case ADJUST_MODE_BRUSH :
				if(pDoc->GetSelState()&ONEBRUSH)
				{
					SelBrushList_RemoveAll (pDoc->DocVarsGetPointer()->m_pSelBrushes);
					SelBrushList_Add (pDoc->DocVarsGetPointer()->m_pSelBrushes, Brush_GetPrevBrush(pDoc->m_pCurBrush, BList));
					pDoc->UpdateSelected();
				
					//update the brush attributes dialog...
					pDoc->UpdateBrushAttributesDlg ();
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
				}
				else if(!(pDoc->GetSelState() & ANYBRUSH))
				{
					Brush *pBrush;
					BrushIterator bi;

					pBrush = BrushList_GetLast(BList, &bi);
					if (pBrush != NULL)
					{
						SelBrushList_Add (pDoc->DocVarsGetPointer()->m_pSelBrushes, pBrush);
						pDoc->UpdateSelected();
						//	Be very careful when speccing flags for UpdateAllViews()
						//	The wrong flags at the wrong time will totally screw things up
						pDoc->UpdateAllViews(UAV_ALL3DVIEWS, NULL);
					}
				}
				break;

			default :
				assert (0);		// bad adjust mode
				break;
		}
	}
}



//	this is called when the user hits the <ENTER> key
void CGEditProView::OnToolsAddtolevel() 
{
	CGEditProDoc *pDoc = GetDocument ();

	if(GetModeTool()==ID_TOOLS_TEMPLATE)
		pDoc->AddBrushToWorld();
}

void CGEditProView::OnUpdateToolsAddtolevel(CCmdUI* pCmdUI) 
{
									//	new for g3dc	
	if(GetModeTool()==ID_TOOLS_TEMPLATE)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}
