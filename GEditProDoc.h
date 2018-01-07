/****************************************************************************************/
/*  GEditProDoc.h                                                                       */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax                                    */
/*  Description:  Genesis world editor header file                                      */
/*                                                                                      */
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
#ifndef GEDITPRODOC_H
#define GEDITPRODOC_H

#pragma once
#include <assert.h>
#include "GEditProDocVars.h"

#include "mainfrm.h"
#include "tLevelMgr.h"
#include "tLevel.h"

#include "FaceAttributesDialog.h"
#include "BrushAttributesDialog.h"
#include "SelFaceList.h"
#include "SelBrushList.h"
#include "EntityTable.h"
#include "TextureDlg.h"
#include "Prefs.h"

#include "undostack.h"

#include <afxpriv.h>	// for WM_SETMESSAGESTRING message
#include "GEditProUpdateMgr.h"

#define WINDOW_TOP_VIEW 0
#define WINDOW_FRONT_VIEW 1
#define WINDOW_SIDE_VIEW 2
#define MAX_SEL	1000
#define CURTOOL_NONE	(-1)

enum UpdateFlags
{
	REBUILD_QUICK				=1,
	UAV_RENDER_ONLY				=2,
	UAV_ACTIVE3DVIEW_ONLY		=4,
	UAV_NONACTIVE3DVIEWS_ONLY	=8,
	UAV_TEXTUREVIEW_ONLY		=16,
	UAV_GRID_ONLY				=32,
	UAV_TEXTUREBROWSER_ONLY		=64,
	UAV_ALLVIEWS				=128,
	UAV_ALL3DVIEWS				=256,
	UAV_THIS_GRID_ONLY			=512
};

enum
{
	fctNOTHING,
	fctENTITY,
	fctBRUSH
};

enum BrushSel
{
	brushSelToggle,
	brushSelAlways,
	brushDeselAlways
} ;

enum GridFlags
{
	USE_GRID			=1,
	CONSTRAIN_HOLLOWS	=2
};

enum SelectionState
{
	NOBRUSHES=0, ONEBRUSH=1, MULTIBRUSH=2, RESERVEDBRUSH=4,
	NOFACES=8, ONEFACE=16, MULTIFACE=32, RESERVEDFACE=64,
	NOENTITIES=128, ONEENTITY=256, MULTIENTITY=512, RESERVEDENTITY=1024,
	ONEBRUSHONLY=137, ONEBRUSH_ONEFACE=145, ONEENTITYONLY=264,
	ONEBRUSH_ONEENTITY=265, NOSELECTIONS=136,
	BRUSHCLEAR=7, FACECLEAR=120, ENTITYCLEAR=1920,
	ANYENTITY=1792, ANYBRUSH=7, ANYFACE=112
};

//bool callback for flag checking
typedef geBoolean (*BrushFlagTest)(const Brush *pBrush);


enum fdocAdjustEnum
{
	ADJUST_MODE_TOGGLE,
	ADJUST_MODE_BRUSH,
	ADJUST_MODE_FACE
};

class CMainFrame;

class CGEditProDoc : public CDocument
{
protected: // create from serialization only
	CGEditProDoc();
	DECLARE_SERIAL(CGEditProDoc);

public:
	CtLevelMgr	*m_pLevelMgr;

	bool		DocVarsUndo();
	bool		DocVarsRedo();
	bool GetUndoAvailability();
	void SaveBrushUndo();
	bool GetRedoAvailability();
	bool SetRedoAvailability(bool bState);
	bool SetUndoAvailability(bool bState);
	CGEditProUpdateMgr	*GetUpdateMgr();

	CGEditProDocVars		DocVarsGetByValue();
	CGEditProDocVars		*DocVarsGetPointer();
	CtLevel*				GetLevel();


private:
	CGEditProUpdateMgr		*m_pUpdateMgr;	
	CGEditProDocVars		m_CurrentDocVars;	//	Main holder of current DocVars values

public:
	CGEditProDocVars	DocVarsCreateUniqueCopy(CGEditProDocVars theseDocVars);
	bool		DocVarsUpdate(CGEditProDocVars DocVars);



	//states and stats
	int	m_iNumSelEntities;

	int m_iShowSelectedFaces;
	int m_iCurrentTool, m_iCurrentBitmap, m_iShowBrush, m_iModeTool;
	int m_iShowEntities, m_iCurrentGroup, m_iCurTextureSelection;
	int m_iIsNewDocument, m_iActiveView, m_iLastOp;
	fdocAdjustEnum m_AdjustMode;
	int m_iShowSelectedBrushes;
	int m_iLockAxis;
	geBoolean	m_bConstrainHollows ;
	BOOL m_bSelectLock, m_bTempEnt;
    BOOL m_bPlaceObjectFlag;	// this flag signifies to place an object rather than putting
							// down an entity when enter is pressed
	DWORD m_dwSelState;

	//list related
	Brush			*m_pBTemplate, *m_pCurBrush, *m_pTempShearTemplate;
	Node			*m_pWorldBsp;
	CEntity			m_RegularEntity;
	int				m_iCurrentEntity, m_iScaleNum;
	geVec3d			m_vecSelectedGeoCenter, m_vecFinalRot, m_vecFinalScale, m_vecFinalPos;

	//dialogs / controls
	CFrameWnd *m_pActiveViewFrame;
	CBrushAttributesDialog *m_pBrushAttributes;
	CMainFrame *m_pMainFrame;
	CFaceAttributesDialog *m_pFaceAttributes;

	geBoolean BrushIsVisible( const Brush * pBrush ) const ;
	geBoolean EntityIsVisible( const CEntity *pEntity ) const ;
	void TempCopySelectedBrushes();
	void DoneRotate(void);
	void DoneResize(int sides, int inidx);
	void DoneShear(int sides, int inidx);
	void DoneMove(void);
	void SnapScaleNearest(int sides, int inidx, ViewVars *v);
	DWORD GetSelState(void){  return m_dwSelState;  }
	int	 GetLockAxis( void ) { return m_iLockAxis ; } ;
	void SetLockAxis( int Lock ) { m_iLockAxis = Lock ; } ;
	void SetCurrentBrushTexture();
	BOOL IsEntitySelected();
	void SelectAll (void);
	void DeleteSelectedEntities();
	void UpdateSelected();
	void ResetAllSelectedEntities();
	void CopySelectedBrushes();
	void SetSelectedEntity( int ID );
	void AdjustEntityAngle( const ViewVars * v, const geFloat dx ) ;
	void AdjustEntityArc( const ViewVars * v, const geFloat dx ) ;
	void AdjustEntityRadius( const geVec3d *pVec ) ;

	void CenterThingInActiveView( Brush* pBrush, CEntity* pEntity );
	void SelectAllBrushes();
	void ResetAllSelections();
	void SelectGroupBrushes (BOOL Select, int WhichGroup);
	void SelectModelBrushes (BOOL Select, int ModelId);

	void OnBrushAddtoworld();
	void DoGeneralSelect (void);
	void CreateEntity(char const *pEntityName);
	geBoolean CreateEntityFromName (char const *pEntityType, CEntity &NewEnt);

	// creates a template entity with which the user specifies a location for any objects
	// they place
	void CreateObjectTemplate();

	void CreateStaircase();
	void CreateCylinder();
	void CreateSpheroid();
	void CreateCube();
	void CreateCone();
	void CreateArch();

	void SetMode(int mMode);			//	new g3dc
	void CreateNewTemplateBrush (Brush *pBrush);		// brought into public scope by g3dc
	CString		m_currentTemplateName;
	Brush		*m_pRecentStairs;
	Brush		*m_pRecentCyl;
	Brush		*m_pRecentSphere;
	Brush		*m_pRecentCube;
	Brush		*m_pRecentCone;
	Brush		*m_pRecentArch;

	void SetSelectedBrush();
	void SetSelectedFace(int PolyNum);
	void ResetSelectedBrush();
	void ResetSelectedFace(int PolyNum = 0);
	BOOL OneBrushSelectedOnly(void);
	void ToggleSelectionLock(void){ m_bSelectLock=!(m_bSelectLock); }
	BOOL IsSelectionLocked(void){ return m_bSelectLock; }
	void RotateSelectedBrushes(geVec3d const *v);
	void RotateSelectedBrushesDirect(geVec3d const *v);

	void SelectTextureFromFace3D(CPoint point, ViewVars *v);
	void PlaceTemplateEntity3D(CPoint point, ViewVars *v);
	void MoveToNearest(void);
	void ResetAllSelectedFaces();
	void ResetAllSelectedBrushes();
	void AddBrushToWorld();
	void NullBrushAttributes();
	void ConfigureCurrentTool();
	void UpdateCameraEntity( const ViewVars *v ) ;
	void SetRenderedViewCamera( const geVec3d * pVec, const geVec3d * pPRY ) ;
	void GetCursorInfo(char *info, int MaxSize);
	void NullFaceAttributes();
	void FaceAttributesDialog ();
	void UpdateAllViews(int Mode, CView* pSender, BOOL Override = FALSE );
	int SubtractBrushFromList(Brush& Brush);
	void ResetSelectedFaceAttributes ();
	void ResizeSelected(float dx, float dy, int sides, int inidx);
	void ShearSelected(float dx, float dy, int sides, int inidx);
	void MoveSelectedBrushes (geVec3d const *v);
	void MoveSelectedClone (geVec3d const *v);
	void MoveTemplateBrush (geVec3d *);
	void UpdateGridInformation();
	void DeleteCurrentThing();
	void MoveEntity(geVec3d *);
	void DoneMoveEntity(CGEditProDocVars	*pDocVars);
	void DeleteEntity(int EntityIndex);
	void DoneRotateBrush();
	void OnSelectedTypeCmdUI(CCmdUI* pCmdUI);
	void RenderOrthoView(ViewVars *, CDC *);
	int AreBrushesSelected();
	void UpdateEntityOrigins();
// changed QD Actors
	void UpdateEntityActors();
// end change
	void RenderWorld(ViewVars *v, CDC* pDC);
	void RotateTemplateBrush(geVec3d *);
	void ShearBrush(geVec3d *);
	void ResetSelectedBrushAttributes();
	void BrushListToMeters(void);
	void BrushListToTexels(void);
	void MakeSelectedBrushNewest(void);
	void ScaleWorld(geFloat ScaleFactor);
	void SetAllFacesTextureScale(geFloat ScaleVal);

	void		SetDefaultBrushTexInfo(Brush *);
	void		SelectOrtho(CPoint point, ViewVars *v);
	void		SelectOrthoRect(CPoint ptStart, CPoint ptEnd, ViewVars *v) ;
	void		SelectRay(CPoint point, ViewVars *v);
	WadFileEntry* GetDibBitmap(const char *Name);
	BOOL		DeleteSelectedBrushes();
	BOOL		TempDeleteSelected(CGEditProDocVars *pDocVars);

	int			MakeNewBrushGroup ( CWnd * pParent );
	int			FillBrushGroupCombo (CComboBox &cb);
	int			DoBrushGroupEditor (CComboBox &cb);
	void		AddSelToGroup( void ) ;
	void		RemovesSelFromGroup( void ) ;
	void		SetAdjustmentMode ( fdocAdjustEnum nCmdIDMode ) ;

	// places the specified object down into the level at the specified location
	geBoolean	PlaceObject( const char *Objectname, const geVec3d *location, CGEditProDocVars *pDocVars);
	geBoolean	ImportFile (const char *PathName, const geVec3d *location, CGEditProDocVars *pDocVars);

	void		SelectEntity (CEntity *pEnt);
	void		DeselectEntity (CEntity *pEnt);

	void		ShowEntitiesEditor();	//	new g3dc
	void		ShowBrushAttributesDialog(); // new g3dc

	geBoolean	ValidateEntities( CGEditProDocVars *pDocVars ) ;
	geBoolean	ValidateBrushes( void ) ;
	void		SelectTab( int nTabIndex ) ;

	void		ShowConsole();
	void		OpenLevelOptionsDialog();

	void		BrushSelect (Brush *pBrush);
	geBoolean	BrushIsSelected (Brush const *pBrush);

	void		DoBrushSelection (Brush *pBrush, BrushSel nSelType ) ;
	void		DoEntitySelection (CEntity *pEntity);
	int			FindClosestThing (POINT const *ptFrom, ViewVars *v, Brush **ppMinBrush, CEntity **ppMinEntity, geFloat *pDist);
	geBoolean	fdocShowBrush( Brush const *b, Box3d const *ViewBox ) ;

	geBoolean	bShowLeakFinder(void) { return m_bShowLeak; }
	void		SetShowLeakFinder(geBoolean bShow) { m_bShowLeak=bShow; }
	geBoolean	IsLeakFileLoaded(void) { return m_bLeakLoaded; }
	geVec3d		*GetLeakPoints(void) { assert(m_pVecLeakPoints); return m_pVecLeakPoints; }
	int			GetNumLeakPoints(void) { return m_iNumLeakPoints; }
	void		CompileDone (CompilerErrorEnum CompileRslt);
	void		UpdateFaceAttributesDlg (void);
	void		UpdateBrushAttributesDlg (void);
	const		Prefs *GetPrefs (void);
	void		RebuildTrees(void);
	void		InvalidateDrawTreeOriginalFaces(void);

	geFloat	GetWorldScaleFactor();		//	post 0.57
	bool SetEntityScaleInView(geFloat scale);	//	post 0.57
	geFloat GetEntityScaleInView();		//	post 0.57	

	geBoolean PlayResource (char const * pName);

	bool	SetPrintingState(bool bState);
	bool	GetPrintingState();

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGEditProDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGEditProDoc();

private:
	bool	m_bPrintingView;	// flag to help prevent view-switching during print ops

// Generated message map functions
protected:
	//{{AFX_MSG(CGEditProDoc)
	afx_msg void OnToolsUsegrid();
	afx_msg void OnUpdateToolsUsegrid(CCmdUI* pCmdUI);
	afx_msg void OnEntitiesShow();
	afx_msg void OnUpdateEntitiesShow(CCmdUI* pCmdUI);
	afx_msg void OnViewShowAllGroups();
	afx_msg void OnViewShowCurrentGroup();
	afx_msg void OnViewShowVisibleGroups();
	afx_msg void OnUpdateViewShowVisibleGroups(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewShowAllGroups(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewShowCurrentGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBrushAdjustmentmode(CCmdUI* pCmdUI);
	afx_msg void OnBrushSelectedCopytocurrent();
	afx_msg void OnBrushSelectedDelete();
	afx_msg void OnRebuildBSPTree();
	afx_msg void OnUpdateFaceAdjustmentmode(CCmdUI* pCmdUI);
	afx_msg void OnConstrainhollows();
	afx_msg void OnUpdateConstrainhollows(CCmdUI* pCmdUI);
	afx_msg void OnGeneralselect();
	afx_msg void OnUpdateGeneralselect(CCmdUI* pCmdUI);
	afx_msg void OnThingAttributes();
	afx_msg void OnBrushSubtractfromworld();
	afx_msg void OnCompile();
	afx_msg void OnUpdateBrushSubtractfromworld(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThingAttributes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsBrushShowassociatedentity(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEntitiesEditor(CCmdUI* pCmdUI);
	afx_msg void OnNewLibObject();
	afx_msg void OnFileOpen();
	afx_msg void OnToolsBrushAdjustmentmode();
	afx_msg void OnToolsFaceAdjustmentmode();
	afx_msg void OnUpdateBrushPrimitives(CCmdUI* pCmdUI);
	afx_msg void OnBrushPrimitivesCube();
	afx_msg void OnBrushPrimitivesSpheroid();
	afx_msg void OnBrushPrimitivesCylinder();
	afx_msg void OnBrushPrimitivesStaircase();
	afx_msg void OnViewShowClip();
	afx_msg void OnUpdateViewShowClip(CCmdUI* pCmdUI);
	afx_msg void OnViewShowDetail();
	afx_msg void OnUpdateViewShowDetail(CCmdUI* pCmdUI);
	afx_msg void OnViewShowHint();
	afx_msg void OnUpdateViewShowHint(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnBrushPrimitivesArch();
	afx_msg void OnBrushPrimitivesCone();
	afx_msg void OnFileImport();
	afx_msg void OnToolsBrushAttributes();
	afx_msg void OnUpdateToolsBrushAttributes(CCmdUI* pCmdUI);
	afx_msg void OnToolsFaceAttributes();
	afx_msg void OnUpdateToolsFaceAttributes(CCmdUI* pCmdUI);
	afx_msg void OnEntityVisibility();
	afx_msg void OnToggleRebuildBspMode();
	afx_msg void OnUpdateToggleRebuildBspMode(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnToolsToggleadjustmode();
	afx_msg void OnUpdateToolsToggleadjustmode(CCmdUI* pCmdUI);
	afx_msg void OnLeveloptions();
// changed QD Actors
	afx_msg void OnViewShowActors();
	afx_msg void OnUpdateViewShowActors(CCmdUI* pCmdUI);
// changed QD 11/03
	afx_msg void OnFileExport3dtv1_32();
	afx_msg void OnFileExport3ds();
// end change
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



private:
	geVec3d		*m_pVecLeakPoints, m_vecCameraOrigin, m_vecCameraAngles;
	int			m_iNumLeakPoints;
	geBoolean	m_bLeakLoaded, m_bShowLeak;
	geBoolean	LoadLeakFile(const char *Filename);
	geBoolean	LoadMapFile(const char *Filename);
	UndoStack	*m_pUndoStack;
	char		LastPath[MAX_PATH];  //no cstrings for dialog action gedit
	geFloat		m_fWorldScaleFactor;	//	post 0.57
	geFloat		m_fEntityScaleInViewFactor;	//	post 0.57
// changed QD Actors
	geBoolean	m_bShowClipBrushes, m_bShowDetailBrushes, m_bShowHintBrushes, m_bShowActors;
// end change
	Brush*		m_pTempBrush;	//	post 058

	bool		m_bRedoAvailability, m_bUndoAvailability;

	int DoCompileDialog (void);
	geBoolean WriteLevelToMap (const char *Filename);

	geBoolean Load( const char *FileName );  // Loads from filename
	geBoolean Save( const char *FileName );  // saves filename
// changed QD 11/03
	geBoolean ExportTo3dtv1_32(const char *FileName);
// changed QD 12/03
	geBoolean ExportTo3ds(const char *FileName, int ExpSelected, geBoolean ExpLights, geBoolean ExpFiles);
// end change

	void UpdateSelectedModel (int MoveRotate, geVec3d const *pVecDelta);

	// insures all of the Dib ID's in the brush are correct
	// returns GE_TRUE if all textures fixed up.
	// returns GE_FALSE if any texture was not found
	geBoolean FixUpBrushDibIDs( Brush *brush );

	// our current setting for our grid movement
	int m_iCurrentBrushId;
	void	SetCameraDocumentCoords(geVec3d *pVecOrigin, geVec3d *pVecAngles);
	void	GetCameraDocumentCoords(geVec3d *pVecOrigin, geVec3d *pVecAngles);

	void RotateSelectedBrushList (SelBrushList *pList, geVec3d const *v);
	void MoveSelectedBrushList (SelBrushList *pList, geVec3d const *v);
	void GetRotationPoint (geVec3d *pVec);
	void AddCameraEntityToLevel (void);
	CEntity	*FindCameraEntity(CGEditProDocVars *pDocVars);
	geBoolean FindClosestEntity (POINT const *ptFrom, ViewVars *v, CEntity **ppMinEntity, geFloat *pMinEntityDist);
	geBoolean FindClosestBrush (POINT const *ptFrom, ViewVars *v, Brush **ppFoundBrush, geFloat *pMinEdgeDist);

	void SetupDefaultFilename (void);
	const char *FindTextureLibrary (char const *WadName);
	Face *FindSelectedFace (void);
	void DeleteBrushAttributes (void);
	void DeleteFaceAttributes (void);
};


geBoolean BrushSelect( Brush *pBrush, uint32 Action, uint32 Data, void * pVoid ) ;
geBoolean BrushDraw( Brush *pBrush, uint32 Action, uint32 Data, void * pVoid ) ;
geBoolean EntitySelect( CEntity& Entity, uint32 Action, uint32 Data, void * pVoid ) ;
geBoolean EntityDraw( CEntity& Entity, uint32 Action, uint32 Data, void * pVoid ) ;

#endif

