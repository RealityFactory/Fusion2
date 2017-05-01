#pragma once

#include "entity.h"
#include "brush.h"
#include "model.h"
#include "group.h"
#include "wadfile.h"
#include "compiler.h"
#include "EntView.h"
#include "BrushTemplate.h"
#include "EntityTable.h"

#include "tLevel.h"

// moved here from level.cpp for g3dc
#include "Parse3dt.h"
#include "EntTypeName.h"
#include <assert.h>
#include "include/ram.h"
#include "units.h"
#include "util.h"
#include "FilePath.h"
#define NUM_VIEWS (4)


#define LEVEL_VERSION_MAJOR	1
#define LEVEL_VERSION_MINOR 32			// Version 1.32 11/04/99 - Brian - Face Info save out Base Vec for Tex Lock


typedef geBoolean (*BrushList_CB)( Brush *pBrush, void * pVoid ) ;
typedef geBoolean (*Brush_FaceCallback)(Face *pFace, void *lParam);
typedef geBoolean (*Brush_CSGCallback)(const Brush *pBrush, void *lParam);
geBoolean FaceFixupCallback (Brush *pBrush, void *lParam);
//uint16 GetDibId (const CtLevel *pLevel, const char *Name);


// CtLevelMgr command target

class CtLevelMgr : public CObject
{
public:
	CtLevelMgr();
	virtual ~CtLevelMgr();

//void		AssignEntityName (CtLevel *pLevel, CEntity *pEnt);
//geBoolean	LoadEntities(CtLevel *pLevel, Parse3dt *Parser, int VersionMajor, int VersionMinor,
//							const char **Expected);
//geBoolean	SaveEntities (CEntityArray *Entities, FILE *f);
//void		UnloadEntityDefs (CtLevel *pLevel);
//uint16		GetDibIdFromWad (const CWadFile *WadFile, const char *Name);
//void		BrushListToTexels(CtLevel *pLevel);
//geBoolean	LoadSky(CtLevel *pLevel, Parse3dt *Parser, int VersionMajor, int VersionMinor, 
//						const char **Expected);
geBoolean LoadCompileInfo(CtLevel *pLevel, Parse3dt *Parser, int VersionMajor, 
						  int VersionMinor, const char **Expected);
//geBoolean LoadGridInfo(CtLevel *pLevel, Parse3dt *Parser, int VersionMajor, 
//					   int VersionMinor, const char **Expected);
geBoolean LoadOneView(ViewStateInfo *pViewInfo, const char *ViewName, Parse3dt *Parser, 
                      int VersionMajor, int VersionMinor, const char **Expected);
//geBoolean LoadViewInfo(CtLevel *pLevel, Parse3dt *Parser, int VersionMajor, 
//                       int VersionMinor, const char **Expected);
//geBoolean LoadBrushTemplates(CtLevel *pLevel, Parse3dt *Parser, int VersionMajor, 
//                             int VersionMinor, const char **Expected);
//geBoolean WriteSky(SkyFaceTexture const SkyFaces[], geVec3d const *Axis,
//                   const float Speed, const float Scale, FILE *ArFile);
//geBoolean WriteCompileInfo(const CompileParamsType *pParms, FILE *ArFile);
//geBoolean WriteGridInfo(const GridInfo *pGridInfo, FILE *ArFile);
geBoolean WriteOneView(const ViewStateInfo *pViewInfo, const char *ViewName, FILE *f);
//geBoolean WriteViewInfo(const ViewStateInfo pViewInfo[], FILE *ArFile);
//geBoolean WriteBrushTemplates(const CtLevel *pLevel, FILE *f);




CtLevel		*Create (const char *DefaultWad, const char *HeadersDir);

CtLevel		*CreateFromFile (const char *FileName, const char **ErrMsg, const char *DefaultHeadersDir);
void		Destroy(CtLevel **ppLevel);

geBoolean WriteToFile (CtLevel *pLevel, const char *Filename);


CEntityArray *GetEntities (CtLevel *pLevel);
BrushList *GetBrushes (CtLevel *pLevel);
SkyFaceTexture *GetSkyInfo (CtLevel *pLevel, geVec3d *Axis, geFloat *Speed, geFloat *Scale);

void SetSkyRotationAxis (CtLevel *pLevel, const geVec3d *Axis);
void SetSkyRotationSpeed (CtLevel *pLevel, const geFloat Speed);
void SetSkyTextureScale (CtLevel *pLevel, const geFloat Scale);

ModelInfo_Type *GetModelInfo (CtLevel *pLevel);
Model *GetModel (CtLevel *pLevel, int ModelId);
void AddModel (CtLevel *pLevel, Model *pModel);

GroupListType *GetGroups (CtLevel *pLevel);
Group *GetGroup (CtLevel *pLevel, int GroupId);
void AddGroup (CtLevel *pLevel, Group *pGroup);

const char *GetWadPath (const CtLevel *pLevel);
CString CloneWadPath(const CtLevel *pLevel);

SizeInfo *GetWadSizeInfos (CtLevel *pLevel);
CWadFile *GetWadFile (CtLevel *pLevel);

uint16 GetDibId (const CtLevel *pLevel, const char *Name);
WadFileEntry *GetWadBitmap (CtLevel *pLevel, const char *Name);

CompileParamsType *GetCompileParams (CtLevel *pLevel);

void SetGroupVisibility (CtLevel *pLevel, int Setting);
int GetGroupVisibility (const CtLevel *pLevel);
EntityViewList *GetEntityVisibilityInfo (CtLevel *pLevel);

void SetWadPath (CtLevel *pLevel, const char *NewWad);
geBoolean LoadWad (CtLevel *pLevel);
geBoolean LoadAdditionalWad (CtLevel *pLevel);
void UnloadWad (CtLevel *pLevel);

int AddEntity (CtLevel *pLevel, CEntity &Entity);
void AppendBrush (CtLevel *pLevel, Brush *pBrush);
void RemoveBrush (CtLevel *pLevel, Brush *pBrush);

int EnumEntities (CtLevel *pLevel, void *lParam, EntityList_CB Callback);
int EnumBrushes (CtLevel *pLevel, void *lParam, BrushList_CB Callback);
int EnumAllBrushes (CtLevel *pLevel, void *lParam, BrushList_CB Callback);
int EnumLeafBrushes (CtLevel *pLevel, void *lParam, BrushList_CB Callback);


//geBoolean FaceFixupCallback (Brush *pBrush, void *lParam);


void TranslateAll (CtLevel *pLevel, const geVec3d *VecXlate);
void CollapseGroups (CtLevel *pLevel, int StartingGroup);
void CollapseModels (CtLevel *pLevel, int StartingModel);

float GetGridSnapSize (const CtLevel *pLevel);
int GetRotationSnap (const CtLevel *pLevel);
int GetGridType (const CtLevel *pLevel);
geBoolean UseGrid (const CtLevel *pLevel);
GridInfo *GetGridInfo (CtLevel *pLevel);

geBoolean RebuildBspAlways (const CtLevel *pLevel);
void SetBspRebuild (CtLevel *pLevel, geBoolean RebuildFlag);

ViewStateInfo *GetViewStateInfo (CtLevel *pLevel, int iView);

BrushTemplate_Arch *GetArchTemplate (CtLevel *pLevel);
BrushTemplate_Box  *GetBoxTemplate (CtLevel *pLevel);
BrushTemplate_Cone *GetConeTemplate (CtLevel *pLevel);
BrushTemplate_Cylinder *GetCylinderTemplate (CtLevel *pLevel);
BrushTemplate_Spheroid *GetSpheroidTemplate (CtLevel *pLevel);
BrushTemplate_Staircase *GetStaircaseTemplate (CtLevel *pLevel);

geVec3d *GetTemplatePos (CtLevel *pLevel);

float GetDrawScale (const CtLevel *pLevel);
float GetLightmapScale (const CtLevel *pLevel);
void SetDrawScale (CtLevel *pLevel, float Scale);
void SetLightmapScale (CtLevel *pLevel, float Scale);

geBoolean LoadEntityDefs (CtLevel *pLevel, const char *HeadersDir);
const char *GetHeadersDirectory (const CtLevel *pLevel);
CString CloneHeadersDirectory(const CtLevel *pLevel);
const EntityTable *GetEntityDefs (const CtLevel *pLevel);







};


