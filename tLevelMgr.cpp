// tLevelMgr.cpp : implementation file
//

#include "stdafx.h"
#include "GEditPro.h"
#include "tLevelMgr.h"

#include "shlwapi.h"

// CtLevelMgr

CtLevelMgr::CtLevelMgr()
{

}

CtLevelMgr::~CtLevelMgr()
{

}


// CtLevelMgr member functions


EntityViewList *CtLevelMgr::GetEntityVisibilityInfo (CtLevel *pLevel)
{
	return pLevel->m_pEntityView;
}

void CtLevelMgr::SetGroupVisibility (CtLevel *pLevel, int Setting)
{
	assert ((Setting == Group_ShowAll) ||
			(Setting == Group_ShowVisible) ||
			(Setting == Group_ShowCurrent));
	assert (pLevel != NULL);

	pLevel->m_GroupVisSetting = Setting;
}

int CtLevelMgr::GetGroupVisibility (const CtLevel *pLevel)
{
	assert (pLevel != NULL);

//	return pLevel->m_GroupVisSetting;
	return CGlobals::g_iGroupVisibility;
}

//void CtLevelMgr::AssignEntityName (CtLevel *pLevel, CEntity *pEnt)
static void Level_AssignEntityName (CtLevel *pLevel, CEntity *pEnt)
{
	CString EntityClassname;
	CString NewName;
	int Num;

	EntityClassname = pEnt->GetClassname ();
	Num = EntTypeNameList_UpdateCount (pLevel->m_pEntTypeNames, EntityClassname);
	NewName.Format ("%s%d", EntityClassname, Num);
	pEnt->SetKeyValue ("%name%", NewName);
}

// changed QD
geBoolean CtLevelMgr::LoadEntities
//static geBoolean Level_LoadEntities
(
	  CtLevel *pLevel,
	  Parse3dt *Parser,
	  int VersionMajor,
	  int VersionMinor,
	  const char **Expected
	)
{
	int EntityCount, i;
	
	if (!Parse3dt_ScanExpectingText (Parser, (*Expected = "Class"))) return GE_FALSE;
	if (!Parse3dt_ScanExpectingText (Parser, (*Expected = "CEntList"))) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "EntCount"), &EntityCount)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "CurEnt"), &i)) return GE_FALSE;	

	for (i=0; i < EntityCount; i++)
	{
		CEntity ent;
		CString Value;

		if (!ent.ReadFromFile (Parser, VersionMajor, VersionMinor, Expected, pLevel->m_pEntityDefs)) return GE_FALSE;
		ent.DeSelect ();  // don't want it selected on load...
		if (!Parse3dt_ScanExpectingText (Parser, (*Expected = "End"))) return GE_FALSE;
		if (!Parse3dt_ScanExpectingText (Parser, (*Expected = "CEntity"))) return GE_FALSE;

		if( ent.IsCamera() == GE_FALSE )	// Exclude cameras
		{
			if (ent.GetKeyValue ("%name%", Value))
			{
				EntTypeNameList_UpdateCount (pLevel->m_pEntTypeNames, Value);
			}
			else
			{
				Level_AssignEntityName (pLevel, &ent);
			}
// changed QD Actors
// create actordefs when loading a level
			int j =	pLevel->m_pEntities->Add (ent);
			char ActorFile[256], ActorDir[256], PawnIni[256];
			strcpy(PawnIni, GetPawnIniPath(pLevel));
			if((*(pLevel->m_pEntities))[j].HasActor(ActorFile, PawnIni))
			{
				Brush *pBrush;
				strcpy(ActorDir, GetActorsDirectory(pLevel));
				pBrush=(*(pLevel->m_pEntities))[j].CreateActorBrush(ActorFile, ActorDir, PawnIni);
				if(pBrush)
				{
					AppendBrush(pLevel,pBrush);
					if(!pLevel->m_ShowActors)
						Brush_SetVisible(pBrush, GE_FALSE);
				}
			}
// end change
		}
	}
	return GE_TRUE;
}

//geBoolean CtLevelMgr::SaveEntities (CEntityArray *Entities, FILE *f)
static geBoolean Level_SaveEntities (CEntityArray *Entities, FILE *f)
{
	int i;
	int NumEntities;

	assert (Entities != NULL);
	assert (f != NULL);

	NumEntities = Entities->GetSize ();
	if (fprintf(f, "Class CEntList\nEntCount %d\n", NumEntities) < 0) return GE_FALSE;
	if (fprintf(f, "CurEnt 0\n") < 0) return GE_FALSE;

	for(i=0;i < NumEntities;i++)
	{
		if (!(*Entities)[i].SaveToFile (f)) return GE_FALSE;
		if (fprintf(f, "End CEntity\n") < 0) return GE_FALSE;
	}
	return GE_TRUE;
}

// changed QD 12/03
static geBoolean Level_ExportLightsTo3ds(CEntityArray *Entities, FILE *f, int ExpSelected,
										 int GroupID, int *LCount, int *SLCount)
{
	int i, LightCount, SLightCount;
	int NumEntities;

	if(!Entities) return GE_FALSE;
	if(!f) return GE_FALSE;

	NumEntities = Entities->GetSize ();
	//if (fprintf(f, "Class CEntList\nEntCount %d\n", NumEntities) < 0) return GE_FALSE;
	//if (fprintf(f, "CurEnt 0\n") < 0) return GE_FALSE;

	LightCount = SLightCount = 0;
	for(i=0;i < NumEntities;i++)
	{
		if ((*Entities)[i].IsSelected()||!ExpSelected)
		{
			if(GroupID==-1||GroupID==(*Entities)[i].GetGroupId())
			{
				if(!strcmp((*Entities)[i].GetClassname(),"light"))
				{
					if (!(*Entities)[i].ExportTo3ds (f, LightCount)) return GE_FALSE;
					LightCount++;
				}
				else if(!strcmp((*Entities)[i].GetClassname(),"spotlight"))
				{
					if (!(*Entities)[i].ExportTo3ds (f, SLightCount)) return GE_FALSE;
					SLightCount++;
				}
			}
		}
		//if (fprintf(f, "End CEntity\n") < 0) return GE_FALSE;
	}
	*LCount = LightCount;
	*SLCount = SLightCount;
	return GE_TRUE;
}

static geBoolean Level_ExportLightsKFTo3ds(CEntityArray *Entities, FILE *f, int ExpSelected, int GroupID)
{
	int i, LightCount, SLightCount;
	int NumEntities;

	if(!Entities) return GE_FALSE;
	if(!f) return GE_FALSE;

	NumEntities = Entities->GetSize ();

	LightCount = SLightCount = 0;
	for(i=0;i < NumEntities;i++)
	{
		if ((*Entities)[i].IsSelected()||!ExpSelected)
		{
			if(GroupID==-1||GroupID==(*Entities)[i].GetGroupId())
			{
				if(!strcmp((*Entities)[i].GetClassname(),"light"))
				{
					if (!(*Entities)[i].ExportKFTo3ds (f, LightCount, SLightCount)) return GE_FALSE;
					LightCount++;
				}
				else if(!strcmp((*Entities)[i].GetClassname(),"spotlight"))
				{
					if (!(*Entities)[i].ExportKFTo3ds (f, LightCount, SLightCount)) return GE_FALSE;
					SLightCount++;
				}
			}
		}
	}
	return GE_TRUE;
}
// end change

//void CtLevelMgr::UnloadEntityDefs (CtLevel *pLevel)
static void Level_UnloadEntityDefs (CtLevel *pLevel)
{
	if (pLevel->m_pEntityDefs != NULL)
	{
		EntityTable_Destroy (&pLevel->m_pEntityDefs);
	}
	if (pLevel->m_pHeadersDir != NULL)
	{
		geRam_Free (pLevel->m_pHeadersDir);
	}
}

geBoolean CtLevelMgr::LoadEntityDefs (CtLevel *pLevel, const char *HeadersDir)
{
	Level_UnloadEntityDefs (pLevel);
	pLevel->m_pHeadersDir = Util_Strdup (HeadersDir);
	pLevel->m_pEntityDefs = EntityTable_Create (pLevel->m_pHeadersDir);
	if (pLevel->m_pEntityDefs == NULL)
	{
		return GE_FALSE;
	}
	return GE_TRUE;
}



const EntityTable *CtLevelMgr::GetEntityDefs (const CtLevel *pLevel)
{
	return pLevel->m_pEntityDefs;
}

/*
CtLevel *CtLevelMgr::Create (const char *pWadName, const char *HeadersDir)
{
	CtLevel *pLevel = NULL;
	pLevel = new CtLevel;

//	pLevel = GE_RAM_ALLOCATE_STRUCT (CtLevel);
	if (pLevel != NULL)
	{
		m_pLevel->m_pBrushes = BrushList_Create ();
		if (m_pLevel->m_pBrushes == NULL) goto CreateError;

		m_pLevel->m_pEntities = new (CEntityArray);
		if (m_pLevel->m_pEntities == NULL) goto CreateError;

		m_pLevel->m_pEntities->SetSize (0, 20);

		m_pLevel->m_pEntTypeNames = EntTypeNameList_Create ();
		if (m_pLevel->m_pEntTypeNames == NULL) goto CreateError;

		m_pLevel->m_pGroups = Group_CreateList ();
		if (m_pLevel->m_pGroups == NULL) goto CreateError;

		{
			// add the default group
			Group *pGroup = Group_Create (0, "Default");
			if (pGroup != NULL)
			{
				GroupList_Add (m_pLevel->m_pGroups, pGroup);
			}
		}


		m_pLevel->m_ModelInfo.CurrentModel = 0;
		m_pLevel->m_ModelInfo.Models = ModelList_Create ();
		if (m_pLevel->m_ModelInfo.Models == NULL) goto CreateError;

		m_pLevel->m_pHeadersDir = NULL;
		m_pLevel->m_pEntityDefs = NULL;
		if (CtLevelMgr::LoadEntityDefs (pLevel, HeadersDir) == GE_FALSE)
		{
			goto CreateError;
		}

		m_pLevel->m_pWadPath = Util_Strdup (pWadName);
		m_pLevel->m_pWadFile = NULL;
		m_pLevel->m_pWadSizeInfos = NULL;

		// initialize sky
		geVec3d_Set (&m_pLevel->m_SkyRotationAxis, 1.0f, 0.0f, 0.0f);
		m_pLevel->m_SkyRotationSpeed = 10.0f;
		m_pLevel->m_SkyTextureScale = 1.0f;
		for (int i = 0; i < 6; ++i)
		{
			m_pLevel->m_SkyFaces[i].TextureName = NULL;
			m_pLevel->m_SkyFaces[i].Apply = GE_FALSE;
		}

		// Set default compile dialog params
		m_pLevel->m_CompileParams.EntitiesOnly = GE_FALSE;
		m_pLevel->m_CompileParams.VisDetailBrushes = GE_FALSE;
		m_pLevel->m_CompileParams.DoVis = GE_TRUE;
		m_pLevel->m_CompileParams.DoLight = GE_TRUE;
		m_pLevel->m_CompileParams.RunBsp = GE_TRUE;
		m_pLevel->m_CompileParams.RunPreview = GE_FALSE;
		m_pLevel->m_CompileParams.UseMinLight = GE_TRUE;
		m_pLevel->m_CompileParams.SuppressHidden = GE_FALSE;
		m_pLevel->m_CompileParams.Filename[0] = '\0';

		m_pLevel->m_CompileParams.Light.Verbose = GE_FALSE;
		m_pLevel->m_CompileParams.Light.ExtraSamples = GE_FALSE;
		m_pLevel->m_CompileParams.Light.LightScale = 1.0f;
		m_pLevel->m_CompileParams.Light.Radiosity = GE_FALSE;
		m_pLevel->m_CompileParams.Light.NumBounce = 10;
		m_pLevel->m_CompileParams.Light.PatchSize = 128.0f;
		m_pLevel->m_CompileParams.Light.FastPatch = GE_FALSE;
		m_pLevel->m_CompileParams.Light.ReflectiveScale = 1.0f;
		geVec3d_Set (&m_pLevel->m_CompileParams.Light.MinLight, 128.0f, 128.0f, 128.0f);

		m_pLevel->m_CompileParams.Bsp.Verbose = GE_FALSE;
		m_pLevel->m_CompileParams.Bsp.EntityVerbose = GE_FALSE;

		m_pLevel->m_CompileParams.Vis.Verbose = GE_FALSE;
		m_pLevel->m_CompileParams.Vis.FullVis = GE_FALSE;
		m_pLevel->m_CompileParams.Vis.SortPortals = GE_TRUE;

		m_pLevel->m_GroupVisSetting = Group_ShowVisible;

		m_pLevel->m_pEntityView = EntityViewList_Create (m_pLevel->m_pEntityDefs);

		// grid settings
		//default to texel grid and snap
		//with rotational snap of 15
		{
			GridInfo *pGridInfo;

			pGridInfo = &m_pLevel->m_GridSettings;
			pGridInfo->UseGrid = GE_TRUE;
			pGridInfo->GridType = GridTexel;
			pGridInfo->SnapType = GridTexel;
			pGridInfo->MetricSnapSize = GridSize_Decimeter;
			pGridInfo->TexelSnapSize = 8;
			pGridInfo->RotationSnap = 15;
		}

		m_pLevel->m_BspRebuildFlag = GE_TRUE;
		for (int iView = 0; iView < NUM_VIEWS; ++iView)
		{
			ViewStateInfo *pInfo;

			pInfo = &m_pLevel->m_ViewInfo[iView];
			pInfo->IsValid = GE_FALSE;
			pInfo->ZoomFactor = 1.0f;
			geVec3d_Clear (&pInfo->PitchRollYaw);
			geVec3d_Set (&pInfo->CameraPos, 0.0f, 0.0f, 0.0f);
		}

		BrushTemplate_ArchDefaults (&m_pLevel->m_ArchTemplate);
		BrushTemplate_BoxDefaults (&m_pLevel->m_BoxTemplate);
		BrushTemplate_ConeDefaults (&m_pLevel->m_ConeTemplate);
		BrushTemplate_CylinderDefaults (&m_pLevel->m_CylinderTemplate);
		BrushTemplate_SpheroidDefaults (&m_pLevel->m_SpheroidTemplate);
		BrushTemplate_StaircaseDefaults (&m_pLevel->m_StaircaseTemplate);

		geVec3d_Clear (&m_pLevel->m_TemplatePos);

		m_pLevel->m_DrawScale = 1.0f;
		m_pLevel->m_LightmapScale = 2.0f;
	}

	return pLevel;

CreateError :
	CtLevelMgr::Destroy (&pLevel);
	return pLevel;
}

*/


// changed QD
CtLevel *CtLevelMgr::Create (const char *pWadName, const char *HeadersDir, const char *ActorsDir, const char *PawnIni)
// end change
{
	CtLevel *pLevel = NULL;
	pLevel = new CtLevel;

	if (pLevel)
	{
		pLevel->m_pBrushes = BrushList_Create ();
		if (pLevel->m_pBrushes == NULL) goto CreateError;

//		pLevel->m_pEntities = new (CEntityArray);
//		is done in CtLevel constructor
		if (pLevel->m_pEntities == NULL) {AfxMessageBox("Entities array pointer is NULL");goto CreateError;}

		pLevel->m_pEntities->SetSize(0,20);
	
		pLevel->m_pEntTypeNames = EntTypeNameList_Create ();
		if (pLevel->m_pEntTypeNames == NULL) {AfxMessageBox("m_pEntTypeNames pointer is NULL");goto CreateError;}


		pLevel->m_pGroups = Group_CreateList ();
		if (pLevel->m_pGroups == NULL) goto CreateError;

		{
			// add the default group
			Group *pGroup = NULL;
			pGroup = Group_Create (0, "Default");
			if (pGroup != NULL)
			{
				GroupList_Add (pLevel->m_pGroups, pGroup);
			}
		}


		pLevel->m_ModelInfo.CurrentModel = 0;
		pLevel->m_ModelInfo.Models = ModelList_Create ();
		if (pLevel->m_ModelInfo.Models == NULL) goto CreateError;

		pLevel->m_pHeadersDir = NULL;
		pLevel->m_pEntityDefs = NULL;
		if (LoadEntityDefs (pLevel, HeadersDir) == GE_FALSE)
		{
			goto CreateError;
		}

// changed QD Actors
		pLevel->m_pActorsDir = Util_Strdup (ActorsDir);
		pLevel->m_pPawnIniPath = Util_Strdup (PawnIni);
// end change

		pLevel->m_pWadPath = Util_Strdup (pWadName);
		pLevel->m_pWadFile = NULL;
		pLevel->m_pWadSizeInfos = NULL;

		// initialize sky
		geVec3d_Set (&pLevel->m_SkyRotationAxis, 1.0f, 0.0f, 0.0f);
		pLevel->m_SkyRotationSpeed = 10.0f;
		pLevel->m_SkyTextureScale = 1.0f;
		for (int i = 0; i < 6; ++i)
		{
			pLevel->m_SkyFaces[i].TextureName = NULL;
			pLevel->m_SkyFaces[i].Apply = GE_FALSE;
		}

		// Set default compile dialog params
		pLevel->m_CompileParams.EntitiesOnly = GE_FALSE;
		pLevel->m_CompileParams.VisDetailBrushes = GE_FALSE;
		pLevel->m_CompileParams.DoVis = GE_TRUE;
		pLevel->m_CompileParams.DoLight = GE_TRUE;
		pLevel->m_CompileParams.RunBsp = GE_TRUE;
		pLevel->m_CompileParams.RunPreview = GE_FALSE;
		pLevel->m_CompileParams.UseMinLight = GE_TRUE;
		pLevel->m_CompileParams.SuppressHidden = GE_FALSE;
		pLevel->m_CompileParams.Filename[0] = '\0';

		pLevel->m_CompileParams.Light.Verbose = GE_FALSE;
		pLevel->m_CompileParams.Light.ExtraSamples = GE_FALSE;
		pLevel->m_CompileParams.Light.LightScale = 1.0f;
		pLevel->m_CompileParams.Light.Radiosity = GE_FALSE;
		pLevel->m_CompileParams.Light.NumBounce = 10;
		pLevel->m_CompileParams.Light.PatchSize = 128.0f;
		pLevel->m_CompileParams.Light.FastPatch = GE_FALSE;
		pLevel->m_CompileParams.Light.ReflectiveScale = 1.0f;
		geVec3d_Set (&pLevel->m_CompileParams.Light.MinLight, 128.0f, 128.0f, 128.0f);

		pLevel->m_CompileParams.Bsp.Verbose = GE_FALSE;
		pLevel->m_CompileParams.Bsp.EntityVerbose = GE_FALSE;

		pLevel->m_CompileParams.Vis.Verbose = GE_FALSE;
		pLevel->m_CompileParams.Vis.FullVis = GE_FALSE;
		pLevel->m_CompileParams.Vis.SortPortals = GE_TRUE;

		pLevel->m_GroupVisSetting = Group_ShowVisible;

		pLevel->m_pEntityView = EntityViewList_Create (pLevel->m_pEntityDefs);

		// grid settings
		//default to texel grid and snap
		//with rotational snap of 15
		{
			GridInfo *pGridInfo;

			pGridInfo = &pLevel->m_GridSettings;
			pGridInfo->UseGrid = GE_TRUE;
			pGridInfo->GridType = GridTexel;
			pGridInfo->SnapType = GridTexel;
			pGridInfo->MetricSnapSize = GridSize_Decimeter;
			pGridInfo->TexelSnapSize = 8;
			pGridInfo->RotationSnap = 15;
		}

		pLevel->m_BspRebuildFlag = GE_TRUE;
		for (int iView = 0; iView < NUM_VIEWS; ++iView)
		{
			ViewStateInfo *pInfo;

			pInfo = &pLevel->m_ViewInfo[iView];
			pInfo->IsValid = GE_FALSE;
			pInfo->ZoomFactor = 1.0f;
			geVec3d_Clear (&pInfo->PitchRollYaw);
			geVec3d_Set (&pInfo->CameraPos, 0.0f, 0.0f, 0.0f);
		}

		BrushTemplate_ArchDefaults (&pLevel->m_ArchTemplate);
		BrushTemplate_BoxDefaults (&pLevel->m_BoxTemplate);
		BrushTemplate_ConeDefaults (&pLevel->m_ConeTemplate);
		BrushTemplate_CylinderDefaults (&pLevel->m_CylinderTemplate);
		BrushTemplate_SpheroidDefaults (&pLevel->m_SpheroidTemplate);
		BrushTemplate_StaircaseDefaults (&pLevel->m_StaircaseTemplate);

		geVec3d_Clear (&pLevel->m_TemplatePos);

		pLevel->m_DrawScale = 1.0f;
		pLevel->m_LightmapScale = 2.0f;
	}

	return pLevel;

CreateError :
	CtLevelMgr::Destroy (&pLevel);
	return pLevel;
}


geBoolean CtLevelMgr::LoadAdditionalWad (CtLevel *pLevel)
{
	pLevel->m_pWadFile = new CWadFile;
	if (pLevel->m_pWadFile == NULL)
	{
		return GE_FALSE;
	}

	if (pLevel->m_pWadFile->Setup (pLevel->m_pWadPath))
	{
		pLevel->m_pWadSizeInfos = (SizeInfo *)geRam_Allocate(sizeof(SizeInfo)*pLevel->m_pWadFile->mBitmapCount);

		if (pLevel->m_pWadSizeInfos != NULL)
		{
			int i;

			for (i = 0; i < pLevel->m_pWadFile->mBitmapCount;i++)
			{
				SizeInfo *pInfo;
				WadFileEntry *Entry;

				pInfo = &(pLevel->m_pWadSizeInfos[i]);
				Entry = &(pLevel->m_pWadFile->mBitmaps[i]);

				pInfo->TexWidth		=Entry->Width;
				pInfo->TexHeight	=Entry->Height;
				pInfo->TexData		=(uint8 *)Entry->BitsPtr;
			}
		}
	}

	return (pLevel->m_pWadSizeInfos != NULL);
}


geBoolean CtLevelMgr::LoadWad (CtLevel *pLevel)
{
	// get rid of the old wad...
	CtLevelMgr::UnloadWad (pLevel);

	pLevel->m_pWadFile = NULL;
	pLevel->m_pWadFile = new CWadFile;
	if (pLevel->m_pWadFile)
	{
		if (pLevel->m_pWadFile->Setup (pLevel->m_pWadPath))
		{
			pLevel->m_pWadSizeInfos = NULL;
			pLevel->m_pWadSizeInfos = (SizeInfo *)geRam_Allocate(sizeof(SizeInfo)*pLevel->m_pWadFile->mBitmapCount);

			if (pLevel->m_pWadSizeInfos)
			{
				int i;

				for (i = 0; i < pLevel->m_pWadFile->mBitmapCount;i++)
				{
					SizeInfo *pInfo = NULL;
					WadFileEntry *pEntry = NULL;

					pInfo = &(pLevel->m_pWadSizeInfos[i]);
					pEntry = &(pLevel->m_pWadFile->mBitmaps[i]);

					if (pInfo && pEntry)
					{
						pInfo->TexWidth	= pEntry->Width;
						pInfo->TexHeight = pEntry->Height;
						pInfo->TexData = (uint8 *)pEntry->BitsPtr;
					}
				}
			}
		}
		return (pLevel->m_pWadSizeInfos != NULL);
	}
	else
		return FALSE;
}

void CtLevelMgr::UnloadWad (CtLevel *pLevel)
{
	if (pLevel->m_pWadSizeInfos != NULL)
	{
		geRam_Free (pLevel->m_pWadSizeInfos);
		pLevel->m_pWadSizeInfos = NULL;
	}
	if (pLevel->m_pWadFile != NULL)
	{
		delete pLevel->m_pWadFile;
		pLevel->m_pWadFile = NULL;
	}
}

void CtLevelMgr::Destroy (CtLevel **ppLevel)
{
	CtLevel *pLevel = NULL;

	assert (ppLevel != NULL);
	assert (*ppLevel != NULL);

	if (*ppLevel)
	{

		pLevel = *ppLevel;

// changed QD Actors
// remove ActorBrushes from BrushList
// these brushes will be deleted  by delete pLevel->Entities
		int i;
		for (i = 0; i < pLevel->m_pEntities->GetSize (); ++i)
		{
			Brush *b =(*(pLevel->m_pEntities))[i].GetActorBrush();
			if(b!=NULL)
				RemoveBrush(pLevel, b);
		}
// end change
		if (pLevel)
		{

			if (pLevel->m_pBrushes != NULL)
			{
				BrushList_Destroy (&pLevel->m_pBrushes);
			}
			if (pLevel->m_pEntities != NULL)
			{
				delete pLevel->m_pEntities;
			}
			if (pLevel->m_pWadPath != NULL)
			{
				geRam_Free (pLevel->m_pWadPath);
			}
// changed QD Actors
			if (pLevel->m_pActorsDir!= NULL)
			{
				geRam_Free (pLevel->m_pActorsDir);
			}
			if (pLevel->m_pPawnIniPath != NULL)
			{
				geRam_Free (pLevel->m_pPawnIniPath);
			}
// end change
			if (pLevel->m_pEntTypeNames != NULL)
			{
				EntTypeNameList_Destroy (&pLevel->m_pEntTypeNames);
			}
			if (pLevel->m_pGroups != NULL)
			{
				Group_DestroyList (&pLevel->m_pGroups);
			}
			Level_UnloadEntityDefs (pLevel);
			UnloadWad (pLevel);
			if (pLevel->m_ModelInfo.Models != NULL)
			{
				ModelList_Destroy (&pLevel->m_ModelInfo.Models);
			}
			// destroy sky...
			{
				int i;

				for (i = 0; i < 6; ++i)
				{
					SkyFaceTexture *sft;

					sft = &(pLevel->m_SkyFaces[i]);
					if (sft->TextureName != NULL)
					{
						geRam_Free (sft->TextureName);
					}
				}
			}

			if (pLevel->m_pEntityView != NULL)
			{
				EntityViewList_Destroy (&pLevel->m_pEntityView);
			}

			delete pLevel;
			pLevel = NULL;
			//	geRam_Free (pLevel);
			*ppLevel = NULL;
		}
	}
}

//uint16 CtLevelMgr::GetDibIdFromWad (const CWadFile *WadFile, const char *Name)
static uint16 Level_GetDibIdFromWad (const CWadFile *WadFile, const char *Name)
{
	uint16	i;
	
	if (WadFile && Name)
	{
	for(i=0;i < WadFile->mBitmapCount;i++)
	{
		if(strcmpi(WadFile->mBitmaps[i].Name, Name) == 0)
		{
			return i;
		}
	}
	}

	return 0xffff;
}

uint16 CtLevelMgr::GetDibId (const CtLevel *pLevel, const char *Name)
//uint16 GetDibId (const CtLevel *pLevel, const char *Name)
{
	return Level_GetDibIdFromWad (pLevel->m_pWadFile, Name);
}


WadFileEntry *CtLevelMgr::GetWadBitmap (CtLevel *pLevel, const char *Name)
{
	uint16 i;

	i = Level_GetDibIdFromWad (pLevel->m_pWadFile, Name);
	if (i != 0xffff)
	{
		return &(pLevel->m_pWadFile->mBitmaps[i]);
	}
	else
	{
		return NULL;
	}
}

CompileParamsType *CtLevelMgr::GetCompileParams (CtLevel *pLevel)
{
	return &(pLevel->m_CompileParams);
}

//geBoolean CtLevelMgr::FaceFixupCallback (Brush *pBrush, void *lParam)
geBoolean FaceFixupCallback (Brush *pBrush, void *lParam)
{
	int i;
	CtLevel *pLevel = NULL;
	CtLevelMgr	levelMgr;

	pLevel = (CtLevel *)lParam;
	// set up the dib ids in the brush faces...
	for (i = 0;i < Brush_GetNumFaces(pBrush); i++)
	{
		Face	*f;

		f	=Brush_GetFace(pBrush, i);
		Face_SetTextureDibId(f, levelMgr.GetDibId(pLevel, Face_GetTextureName(f)));
// changed QD 12/03
		const WadFileEntry *pbmp = levelMgr.GetWadBitmap(pLevel, Face_GetTextureName(f));
		if(pbmp)
			Face_SetTextureSize (f, pbmp->Width, pbmp->Height);
// end change
	}
	return GE_TRUE;
}

//void CtLevelMgr::BrushListToTexels(CtLevel *pLevel)
static void Level_BrushListToTexels(CtLevel *pLevel)
{
	int				i;
	Brush			*cb = NULL;
	BrushIterator	bi;
	Face			*f = NULL;
	geFloat			uscale, vscale;

	cb = BrushList_GetFirst (pLevel->m_pBrushes, &bi);
	while (cb != NULL)
	{
		for(i=0;i < Brush_GetNumFaces(cb);i++)
		{
			f	=Brush_GetFace(cb, i);
			Face_GetTextureScale(f, &uscale, &vscale);
			Face_SetTextureScale(f, CENTIMETERS_TO_ENGINE(uscale), CENTIMETERS_TO_ENGINE(vscale));
		}
		cb = BrushList_GetNext (&bi);
	}
}

#pragma warning (disable:4100)
//geBoolean CtLevelMgr::LoadSky 
static geBoolean Level_LoadSky(CtLevel *pLevel, Parse3dt *Parser, int VersionMajor,
                               int VersionMinor, const char **Expected)
{
	int i;
	char BigString[SCANNER_MAXDATA];
	int Apply;
		
	if (!Parse3dt_ScanExpectingText (Parser, (*Expected = "Sky"))) return GE_FALSE;

	for (i = 0; i < 6; ++i)
	{
		*Expected = "Integer";
		if (!Parse3dt_GetInt (Parser, NULL, &Apply)) return GE_FALSE;
		pLevel->m_SkyFaces[i].Apply = (Apply) ? GE_TRUE : GE_FALSE;

		*Expected = "String";
		if ((VersionMajor == 1) && (VersionMinor < 14))
		{
			// first character of the string is x
			if (!Parse3dt_GetIdentifier (Parser, NULL, BigString)) return GE_FALSE;
			pLevel->m_SkyFaces[i].TextureName = Util_Strdup (&BigString[1]);
		}
		else
		{
			if (!Parse3dt_GetLiteral (Parser, NULL, BigString)) return GE_FALSE;
			pLevel->m_SkyFaces[i].TextureName = Util_Strdup (BigString);
		}
	}
	if ((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor >= 26)))
	{
		if (!Parse3dt_GetVec3d (Parser, "Axis", &pLevel->m_SkyRotationAxis)) return GE_FALSE;
		if (!Parse3dt_GetFloat (Parser, "Speed", &pLevel->m_SkyRotationSpeed)) return GE_FALSE;
	}
	if ((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor >= 28)))
	{
		if (!Parse3dt_GetFloat (Parser, "Scale", &pLevel->m_SkyTextureScale)) return GE_FALSE;
	}
	return GE_TRUE;
}
#pragma warning (default:4100)

#pragma warning (disable:4100)
//geBoolean CtLevelMgr::LoadCompileInfo
static geBoolean Level_LoadCompileInfo(CtLevel *pLevel, Parse3dt *Parser, int VersionMajor,
                                       int VersionMinor, const char **Expected)
{
	CompileParamsType *pParms = NULL;
	LightParms *pLight = NULL;
	VisParms *pVis = NULL;
	BspParms *pBsp = NULL;

	pParms = &(pLevel->m_CompileParams);
	if (!Parse3dt_ScanExpectingText (Parser, (*Expected = "CompileInfo"))) return GE_FALSE;
	if (!Parse3dt_GetLiteral (Parser, (*Expected = "Filename"), pParms->Filename)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Vis"), &pParms->DoVis)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Light"), &pParms->DoLight)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Bsp"), &pParms->RunBsp)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Preview"), &pParms->RunPreview)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "MinLight"), &pParms->UseMinLight)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "SuppressHidden"), &pParms->SuppressHidden)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "EntitiesOnly"), &pParms->EntitiesOnly)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "VisDetail"), &pParms->VisDetailBrushes)) return GE_FALSE;

	// light params
	pLight = &(pParms->Light);
	if (!Parse3dt_GetInt (Parser, (*Expected = "Verbose"), &pLight->Verbose)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "ExtraSamples"), &pLight->ExtraSamples)) return GE_FALSE;
	if (!Parse3dt_GetFloat (Parser, (*Expected = "LightScale"), &pLight->LightScale)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Radiosity"), &pLight->Radiosity)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "NumBounce"), &pLight->NumBounce)) return GE_FALSE;
	if (!Parse3dt_GetFloat (Parser, (*Expected = "PatchSize"), &pLight->PatchSize)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "FastPatch"), &pLight->FastPatch)) return GE_FALSE;
	if (!Parse3dt_GetFloat (Parser, (*Expected = "ReflectScale"), &pLight->ReflectiveScale)) return GE_FALSE;
	if (!Parse3dt_GetVec3d (Parser, (*Expected = "MinLight"), &pLight->MinLight)) return GE_FALSE;

	// vis params
	pVis = &(pParms->Vis);
	if (!Parse3dt_GetInt (Parser, (*Expected = "VisVerbose"), &pVis->Verbose)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "FullVis"), &pVis->FullVis)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "SortPortals"), &pVis->SortPortals)) return GE_FALSE;

	// Bsp Params
	pBsp = &(pParms->Bsp);
	if (!Parse3dt_GetInt (Parser, (*Expected = "BspVerbose"), &pBsp->Verbose)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "EntityVerbose"), &pBsp->EntityVerbose)) return GE_FALSE;

	return GE_TRUE;
}
#pragma warning (default:4100)

#pragma warning (disable:4100)
//geBoolean CtLevelMgr::LoadGridInfo
static geBoolean Level_LoadGridInfo
	(
	  CtLevel *pLevel,
	  Parse3dt *Parser, 
	  int VersionMajor, 
	  int VersionMinor, 
	  const char **Expected
	)
{
	GridInfo *pGridInfo = &pLevel->m_GridSettings;

	if (!Parse3dt_GetInt (Parser, (*Expected = "Grid"), &pGridInfo->UseGrid)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Type"), &pGridInfo->GridType)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Snap"), &pGridInfo->SnapType)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Metric"), &pGridInfo->MetricSnapSize)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Texel"), &pGridInfo->TexelSnapSize)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "Rotation"), &pGridInfo->RotationSnap)) return GE_FALSE;

	// force texel grid and snap
	pGridInfo->GridType = GridTexel;
	pGridInfo->SnapType = GridTexel;

    return GE_TRUE;
}
#pragma warning (default:4100)

static char *ViewNames[NUM_VIEWS] = {"TexturedView", "TopView", "FrontView", "SideView"};

#pragma warning (disable:4100)
//geBoolean CtLevelMgr::LoadOneView
static geBoolean Level_LoadOneView
	(
	  ViewStateInfo *pViewInfo,
	  const char *ViewName,
	  Parse3dt *Parser, 
	  int VersionMajor, 
	  int VersionMinor, 
	  const char **Expected
	)
{
	if (!Parse3dt_GetInt (Parser, (*Expected = ViewName), &pViewInfo->IsValid)) return GE_FALSE;
	if (!Parse3dt_GetFloat (Parser, (*Expected = "Zoom"), &pViewInfo->ZoomFactor)) return GE_FALSE;
	if (!Parse3dt_GetVec3d (Parser, (*Expected = "PitchRollYaw"), &pViewInfo->PitchRollYaw)) return GE_FALSE;
	if (!Parse3dt_GetVec3d (Parser, (*Expected = "CamPos"), &pViewInfo->CameraPos)) return GE_FALSE;
	return GE_TRUE;
}
#pragma warning (default:4100)

//geBoolean CtLevelMgr::LoadViewInfo
static geBoolean Level_LoadViewInfo
	(
	  CtLevel *pLevel,
	  Parse3dt *Parser, 
	  int VersionMajor, 
	  int VersionMinor, 
	  const char **Expected
	)
{
	int iView;

	for (iView = 0; iView < NUM_VIEWS; ++iView)
	{
		if (!Level_LoadOneView (&pLevel->m_ViewInfo[iView], ViewNames[iView], Parser, VersionMajor, VersionMinor, Expected)) return GE_FALSE;
	}
	return GE_TRUE;
}

//geBoolean CtLevelMgr::LoadBrushTemplates
static geBoolean Level_LoadBrushTemplates
	(
	  CtLevel *pLevel,
	  Parse3dt *Parser, 
	  int VersionMajor, 
	  int VersionMinor, 
	  const char **Expected
	)
{
	if (!BrushTemplate_LoadArch (&pLevel->m_ArchTemplate, Parser, VersionMajor, VersionMinor, Expected)) return GE_FALSE;
	if (!BrushTemplate_LoadBox (&pLevel->m_BoxTemplate, Parser, VersionMajor, VersionMinor, Expected)) return GE_FALSE;
	if (!BrushTemplate_LoadCone (&pLevel->m_ConeTemplate, Parser, VersionMajor, VersionMinor, Expected)) return GE_FALSE;
	if (!BrushTemplate_LoadCylinder (&pLevel->m_CylinderTemplate, Parser, VersionMajor, VersionMinor, Expected)) return GE_FALSE;
	if (!BrushTemplate_LoadSpheroid (&pLevel->m_SpheroidTemplate, Parser, VersionMajor, VersionMinor, Expected)) return GE_FALSE;
	if (!BrushTemplate_LoadStaircase (&pLevel->m_StaircaseTemplate, Parser, VersionMajor, VersionMinor, Expected)) return GE_FALSE;

	return GE_TRUE;
}

// changed QD Actors
CtLevel *CtLevelMgr::CreateFromFile (const char *FileName, const char **ErrMsg, const char *DefaultHeadersDir,
									 const char *DefaultActorsDir, const char *DefaultPawnIni)
{
	int NumModels;
	int VersionMajor, VersionMinor;
	int NumGroups = 0;
	int NumBrushes = 0;
	int NumEntities;
	Parse3dt *Parser;
	const char *Expected = "!*ERROR*!";
	int k;
	CtLevel *pLevel = NULL;
	char WadPath[MAX_PATH];
	char HeadersDir[MAX_PATH];
// changed QD Actors
	char ActorsDir[MAX_PATH];
	char PawnIniPath[MAX_PATH];
// end change

	CGEditProDoc *pDoc = CGlobals::GetActiveDocument();

	assert (FileName != NULL);

	Parser = Parse3dt_Create (FileName);
	if (Parser == NULL)
	{
		*ErrMsg = "Can't open file";
		return NULL;
	}

	Expected = "3dtVersion";
	if (!Parse3dt_GetVersion (Parser, &VersionMajor, &VersionMinor)) goto DoneLoad;

	if(VersionMajor > LEVEL_VERSION_MAJOR)
	{
		*ErrMsg = "Version mismatch.";
		return NULL ;
	}

	if	(VersionMajor <= 1 && VersionMinor < 16)
	{
		char	PalPath[_MAX_PATH];

		if (!Parse3dt_GetPath (Parser, (Expected = "PalLoc"), PalPath)) goto DoneLoad;
	}

	// texture library path
	if ((VersionMajor <= 1) && (VersionMinor < 18))
	{
		if (!Parse3dt_GetPath (Parser, (Expected = "WadLoc"), WadPath)) goto DoneLoad;
	}
	else
	{
		if (!Parse3dt_GetLiteral (Parser, (Expected = "TextureLib"), WadPath)) goto DoneLoad;
	}

	// headers directory
	if ((VersionMajor <= 1) && (VersionMinor < 31))
	{
		strcpy (HeadersDir, DefaultHeadersDir);		//	post 0.55
	}
	else
	{
		if (!Parse3dt_GetLiteral (Parser, (Expected = "HeadersDir"), HeadersDir)) goto DoneLoad;	//	post 0.55
	}

	//	post 0.55	//	check to see if HeadersDir and WadPath actually exist on this system
	if (pDoc->m_currentTemplateName != "PreFab")	// ignore if we are adding prefab
	{
		if (!PathFileExists(WadPath))
		{
			CString	wadpath = _T("");
			
			CString	txlPathError = _T("GEditPro checks for valid *.txl files when opening new worlds.\n\n");
			txlPathError += _T("This *.3dt file's specified *.txl file... ");
			txlPathError += _T(WadPath);
			txlPathError += _T("\ndoes not exist at the specified path.\n\n");
			txlPathError += _T("To avoid this message in the future, set the *.txl path for this *.3dt file\n");
			txlPathError += _T("in the Project, Level Options dialog box, then save the file.\n\n");
			txlPathError += _T("For now, please select a valid *.txl file or the application will exit.");
	
			AfxMessageBox(txlPathError);
	
				CFileDialog FileDlg (TRUE,
					"txl",
					WadPath,
					OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR,
					"Texture Library Files (*.txl)|*.txl||");
				
				FileDlg.m_ofn.lpstrTitle	="GEditPro Texture Finder";
				
				if (FileDlg.DoModal () == IDOK)
				{
					wadpath = FileDlg.GetPathName ();
					strcpy(WadPath, wadpath.GetBuffer(260));
				}
		}
		
		//	post 0.55	//	check to see if headers directory exists
		{
			CString tempHeaderDir = HeadersDir;
			// report path minus semicolon
			if (tempHeaderDir.Find(";") > 0) // post 0.57
			{
				tempHeaderDir = tempHeaderDir.Left(tempHeaderDir.Find(";"));
			}
			//	if path doesn't exist	
			if (!PathIsDirectory(tempHeaderDir))
			{
				CString	headersPathError = _T("GEditPro checks for valid header directories when opening new worlds.\n\n");
				headersPathError += _T("The path to this *.3dt file's specified Headers directory does not exist on this system.\n\n");
				headersPathError += _T("Choose [OK] if you want GEditPro to set the headers path to:\n\n");
				headersPathError += _T(CGlobals::m_GlobalAppPath);
				headersPathError += _T("\n\nChoose [Cancel] if you want the headers path to stay as:\n\n");
				headersPathError += _T(HeadersDir);
				headersPathError += _T("\n\n");
				headersPathError += _T("After your new level file has opened, please adjust the true headers path(s) using the\n");
				headersPathError += _T("Project, CtLevel Options dialog box.");
				
				if (AfxMessageBox(headersPathError, MB_OKCANCEL) == IDOK)
				{									//	set headers path to this app's path
					strcpy(HeadersDir, CGlobals::m_GlobalAppPath);
				}
			}
		}
	}		//	end post 0.55

// changed QD Actors
	// actors directory
	if ((VersionMajor <= 1) && (VersionMinor < 33))
	{
		strcpy (ActorsDir, DefaultActorsDir);
	}
	else
	{
		if (!Parse3dt_GetLiteral (Parser, (Expected = "ActorsDir"), ActorsDir)) goto DoneLoad;
	}
	// PawnIni
	if ((VersionMajor <= 1) && (VersionMinor < 33))
	{
		strcpy (PawnIniPath, DefaultPawnIni);
	}
	else
	{
		if (!Parse3dt_GetLiteral (Parser, (Expected = "PawnIni"), PawnIniPath)) goto DoneLoad;
	}

	pLevel = Create (WadPath, HeadersDir, ActorsDir, PawnIniPath);
// end change
	if (pLevel == NULL)
	{
		*ErrMsg = "Error creating level.";
		return NULL;
	}


	if ((VersionMajor == 1) && (VersionMinor < 15))
	{
		if (!Parse3dt_GetInt  (Parser, (Expected = "NumBrushes"), &NumBrushes)) goto DoneLoad;
	}
	if (!Parse3dt_GetInt  (Parser, (Expected = "NumEntities"), &NumEntities)) goto DoneLoad;
	if (!Parse3dt_GetInt  (Parser, (Expected = "NumModels"), &NumModels)) goto DoneLoad;

	if ((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor >= 3)))
	{
		if (!Parse3dt_GetInt (Parser, (Expected = "NumGroups"), &NumGroups)) goto DoneLoad;
	}


	if ((VersionMajor == 1) && (VersionMinor < 15))
	{
		for (k = 0; k < NumBrushes; k++)
		{
			Brush *pBrush;

			pBrush	=Brush_CreateFromFile(Parser, VersionMajor, VersionMinor, &Expected);
			if (pBrush == NULL) goto DoneLoad;
			BrushList_Append (pLevel->m_pBrushes, pBrush);
		}
	}
	else
	{
		if (pLevel->m_pBrushes != NULL)
		{
			BrushList_Destroy (&pLevel->m_pBrushes);
		}
		pLevel->m_pBrushes = BrushList_CreateFromFile (Parser, VersionMajor, VersionMinor, &Expected);
		if (pLevel->m_pBrushes == NULL)
			goto DoneLoad;
	}

	if((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor < 6)))
	{
		Level_BrushListToTexels (pLevel);
	}
// changed QD
	if (!LoadEntities (pLevel, Parser, VersionMajor, VersionMinor, &Expected)) goto DoneLoad;
// end change
	if ((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor >= 2)))
	{
		if (!ModelList_Read (pLevel->m_ModelInfo.Models, NumModels, Parser, VersionMajor, VersionMinor, &Expected))
		{
			goto DoneLoad;
		}
	}

	// collapse model list so numbers are consecutive
	CollapseModels (pLevel, 1);

	if ((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor >= 3)))
	{
		if (!Group_ReadList (pLevel->m_pGroups, NumGroups, Parser, VersionMajor, VersionMinor, &Expected ))
		{
			goto DoneLoad;
		}
	}
	// collapse the group list so numbers are consecutive
	CollapseGroups (pLevel, 1);

	// load sky information...
	if ((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor >= 12)))
	{
		if (!Level_LoadSky (pLevel, Parser, VersionMajor, VersionMinor, &Expected)) goto DoneLoad;
	}

	// load compile information and other editing settings
	if ((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor >= 20)))
	{
		if (!Level_LoadCompileInfo (pLevel, Parser, VersionMajor, VersionMinor, &Expected)) goto DoneLoad;
		if (!Parse3dt_GetInt (Parser, (Expected = "ShowGroups"), &pLevel->m_GroupVisSetting)) goto DoneLoad;
		if (!EntityViewList_LoadSettings (pLevel->m_pEntityView, Parser, VersionMajor, VersionMinor, &Expected)) goto DoneLoad;
		if (!Level_LoadGridInfo (pLevel, Parser, VersionMajor, VersionMinor, &Expected)) goto DoneLoad;
		if (!Parse3dt_GetInt (Parser, (Expected = "BspRebuild"), &pLevel->m_BspRebuildFlag)) goto DoneLoad;
		if (!Level_LoadViewInfo (pLevel, Parser, VersionMajor, VersionMinor, &Expected)) goto DoneLoad;
		if (!Level_LoadBrushTemplates (pLevel, Parser, VersionMajor, VersionMinor, &Expected)) goto DoneLoad;
		if (!Parse3dt_GetVec3d (Parser, (Expected = "TemplatePos"), &pLevel->m_TemplatePos)) goto DoneLoad;
	}

	// load level settings
	if ((VersionMajor > 1) || ((VersionMajor == 1) && (VersionMinor >= 22)))
	{
		if (!Parse3dt_GetFloat (Parser, (Expected = "DrawScale"), &pLevel->m_DrawScale)) goto DoneLoad;
		if (!Parse3dt_GetFloat (Parser, (Expected = "LightmapScale"), &pLevel->m_LightmapScale)) goto DoneLoad;
	}

	goto AllDone;

DoneLoad:
	*ErrMsg = Parse3dt_Error (Parser, "Expected %s", Expected);

//DoneLoad1:
	if (pLevel != NULL)
	{
		Destroy (&pLevel);
	}

AllDone:
	if (Parser != NULL)
	{
		Parse3dt_Destroy (&Parser);
	}

	//fixup hollows
	if(pLevel != NULL)
	{
		BrushList_MakeHollowsMulti(pLevel->m_pBrushes);
	}

	return pLevel;
}



//geBoolean CtLevelMgr::WriteSky
static geBoolean Level_WriteSky
	(
	  SkyFaceTexture const SkyFaces[],
	  geVec3d const *Axis,
	  const float Speed,
	  const float Scale,
	  FILE *ArFile
	)
{
	int i;

	if (fprintf (ArFile, "%s\n", "Sky") < 0) return GE_FALSE;
	for (i = 0; i < 6; ++i)
	{
		char QuotedValue[SCANNER_MAXDATA];
		char const *StringToQuote;

		StringToQuote = "";
		if (SkyFaces[i].TextureName != NULL)
		{
			StringToQuote = SkyFaces[i].TextureName;
		}
		Util_QuoteString (StringToQuote, QuotedValue);
		if (fprintf (ArFile, "%d %s\n", SkyFaces[i].Apply, QuotedValue) < 0) return GE_FALSE;
	}

	// Write rotation axis and speed
	if (fprintf (ArFile, "Axis %f %f %f\n", Axis->X, Axis->Y, Axis->Z) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Speed %f\n", Speed) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Scale %f\n", Scale) < 0) return GE_FALSE;

	return GE_TRUE;
}

//geBoolean CtLevelMgr::WriteCompileInfo 
static geBoolean Level_WriteCompileInfo 
	(
	  const CompileParamsType *pParms,
	  FILE *ArFile
	)
{
	char QuotedString[MAX_PATH];
	const LightParms *pLight;
	const VisParms *pVis;
	const BspParms *pBsp;

	if (fprintf (ArFile, "%s\n", "CompileInfo") < 0) return GE_FALSE;
	Util_QuoteString (pParms->Filename, QuotedString);
	if (fprintf (ArFile, "Filename %s\n", QuotedString) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Vis %d\n", pParms->DoVis) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Light %d\n", pParms->DoLight) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Bsp %d\n", pParms->RunBsp) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Preview %d\n", pParms->RunPreview) < 0) return GE_FALSE;
	if (fprintf (ArFile, "MinLight %d\n", pParms->UseMinLight) < 0) return GE_FALSE;
	if (fprintf (ArFile, "SuppressHidden %d\n", pParms->SuppressHidden) < 0) return GE_FALSE;
	if (fprintf (ArFile, "EntitiesOnly %d\n", pParms->EntitiesOnly) < 0) return GE_FALSE;
	if (fprintf (ArFile, "VisDetail %d\n", pParms->VisDetailBrushes) < 0) return GE_FALSE;

	// light params
	pLight = &(pParms->Light);
	if (fprintf (ArFile, "Verbose %d\n", pLight->Verbose) < 0) return GE_FALSE;
	if (fprintf (ArFile, "ExtraSamples %d\n", pLight->ExtraSamples) < 0) return GE_FALSE;
	if (fprintf (ArFile, "LightScale %f\n", pLight->LightScale) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Radiosity %d\n", pLight->Radiosity) < 0) return GE_FALSE;
	if (fprintf (ArFile, "NumBounce %d\n", pLight->NumBounce) < 0) return GE_FALSE;
	if (fprintf (ArFile, "PatchSize %f\n", pLight->PatchSize) < 0) return GE_FALSE;
	if (fprintf (ArFile, "FastPatch %d\n", pLight->FastPatch) < 0) return GE_FALSE;
	if (fprintf (ArFile, "ReflectScale %f\n", pLight->ReflectiveScale) < 0) return GE_FALSE;
	if (fprintf (ArFile, "MinLight %f %f %f\n", pLight->MinLight.X, pLight->MinLight.Y, pLight->MinLight.Z) < 0) return GE_FALSE;

	// vis params
	pVis = &(pParms->Vis);
	if (fprintf (ArFile, "VisVerbose %d\n", pVis->Verbose) < 0) return GE_FALSE;
	if (fprintf (ArFile, "FullVis %d\n", pVis->FullVis) < 0) return GE_FALSE;
	if (fprintf (ArFile, "SortPortals %d\n", pVis->SortPortals) < 0) return GE_FALSE;

	// Bsp Params
	pBsp = &(pParms->Bsp);
	if (fprintf (ArFile, "BspVerbose %d\n", pBsp->Verbose) < 0) return GE_FALSE;
	if (fprintf (ArFile, "EntityVerbose %d\n", pBsp->EntityVerbose) < 0) return GE_FALSE;

	return GE_TRUE;
}

//geBoolean CtLevelMgr::WriteGridInfo 
static geBoolean Level_WriteGridInfo 
	(
	  const GridInfo *pGridInfo,
	  FILE *ArFile
	)
{
	if (fprintf (ArFile, "Grid %d\n", pGridInfo->UseGrid) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Type %d\n", pGridInfo->GridType) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Snap %d\n", pGridInfo->SnapType) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Metric %d\n", pGridInfo->MetricSnapSize) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Texel %d\n", pGridInfo->TexelSnapSize) < 0) return GE_FALSE;
	if (fprintf (ArFile, "Rotation %d\n", pGridInfo->RotationSnap) < 0) return GE_FALSE;

	return GE_TRUE;
}

//geBoolean CtLevelMgr::WriteOneView
static geBoolean Level_WriteOneView
	(
	  const ViewStateInfo *pViewInfo,
	  const char *ViewName,
	  FILE *f
	)
{
	if (fprintf (f, "%s %d\n", ViewName, pViewInfo->IsValid) < 0) return GE_FALSE;
	if (fprintf (f, "Zoom %f\n", pViewInfo->ZoomFactor) < 0) return GE_FALSE;
	if (fprintf (f, "PitchRollYaw %f %f %f\n", pViewInfo->PitchRollYaw.X, pViewInfo->PitchRollYaw.Y, pViewInfo->PitchRollYaw.Z) < 0) return GE_FALSE;
	if (fprintf (f, "CamPos %f %f %f\n", pViewInfo->CameraPos.X, pViewInfo->CameraPos.Y, pViewInfo->CameraPos.Z) < 0) return GE_FALSE;
	return GE_TRUE;
}

//geBoolean CtLevelMgr::WriteViewInfo
static geBoolean Level_WriteViewInfo
	(
	  const ViewStateInfo pViewInfo[],
	  FILE *ArFile
	)
{
	int iView;

	for (iView = 0; iView < NUM_VIEWS; iView++)
	{
		if (!Level_WriteOneView (&pViewInfo[iView], ViewNames[iView], ArFile)) return GE_FALSE;
	}
	return GE_TRUE;
}

//geBoolean CtLevelMgr::WriteBrushTemplates
static geBoolean Level_WriteBrushTemplates
	(
	  const CtLevel *pLevel,
	  FILE *f
	)
{
	if (BrushTemplate_WriteArch (&pLevel->m_ArchTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteBox (&pLevel->m_BoxTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteCone (&pLevel->m_ConeTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteCylinder (&pLevel->m_CylinderTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteSpheroid (&pLevel->m_SpheroidTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteStaircase (&pLevel->m_StaircaseTemplate, f) == GE_FALSE) return GE_FALSE;

	return GE_TRUE;
}

// changed QD 11/03
static geBoolean Level_ExportBrushTemplatesTo3dtv1_32
	(
	  const CtLevel *pLevel,
	  FILE *f
	)
{
	if (BrushTemplate_WriteArchTo3dtv1_32 (&pLevel->m_ArchTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteBoxTo3dtv1_32 (&pLevel->m_BoxTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteConeTo3dtv1_32 (&pLevel->m_ConeTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteCylinderTo3dtv1_32 (&pLevel->m_CylinderTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteSpheroidTo3dtv1_32 (&pLevel->m_SpheroidTemplate, f) == GE_FALSE) return GE_FALSE;
	if (BrushTemplate_WriteStaircaseTo3dtv1_32 (&pLevel->m_StaircaseTemplate, f) == GE_FALSE) return GE_FALSE;

	return GE_TRUE;
}
// end change

geBoolean CtLevelMgr::WriteToFile (CtLevel *pLevel, const char *Filename)
{
	FILE	*ArFile = NULL;
	char QuotedString[MAX_PATH];
	geBoolean WriteRslt;

	assert (pLevel != NULL);
	assert (Filename != NULL);

	// error checking required!
	ArFile = fopen(Filename, "wt");

	if (ArFile == NULL)
	{
		return GE_FALSE;
	}

	WriteRslt = GE_FALSE;
	if (fprintf(ArFile, "3dtVersion %d.%d\n", LEVEL_VERSION_MAJOR, LEVEL_VERSION_MINOR) < 0) goto WriteDone;

	Util_QuoteString (pLevel->m_pWadPath, QuotedString);
	if (fprintf(ArFile, "TextureLib %s\n", QuotedString) < 0) goto WriteDone;

	Util_QuoteString (pLevel->m_pHeadersDir, QuotedString);
	if (fprintf (ArFile, "HeadersDir %s\n", QuotedString) < 0) goto WriteDone;
// changed QD Actors
	Util_QuoteString (pLevel->m_pActorsDir, QuotedString);
	if (fprintf (ArFile, "ActorsDir %s\n", QuotedString) < 0) goto WriteDone;

	Util_QuoteString (pLevel->m_pPawnIniPath, QuotedString);
	if (fprintf (ArFile, "PawnIni %s\n", QuotedString) < 0) goto WriteDone;
// remove ActorBrushes from List, so they don't get written to the file
	int i;
	for (i = 0; i < pLevel->m_pEntities->GetSize (); ++i)
	{
		Brush *b =(*(pLevel->m_pEntities))[i].GetActorBrush();
		if(b!=NULL)
			RemoveBrush(pLevel, b);
	}
// end change

	if (fprintf(ArFile, "NumEntities %d\n", pLevel->m_pEntities->GetSize ()) < 0) goto WriteDone;
	if (fprintf(ArFile, "NumModels %d\n", ModelList_GetCount (pLevel->m_ModelInfo.Models)) < 0) goto WriteDone;
	if (fprintf(ArFile, "NumGroups %d\n", Group_GetCount (pLevel->m_pGroups)) < 0) goto WriteDone;
	if (BrushList_Write (pLevel->m_pBrushes, ArFile) == GE_FALSE) goto WriteDone;
	if (Level_SaveEntities (pLevel->m_pEntities, ArFile) == GE_FALSE) goto WriteDone;
	if (ModelList_Write (pLevel->m_ModelInfo.Models, ArFile) == GE_FALSE) goto WriteDone;
	if (Group_WriteList (pLevel->m_pGroups, ArFile) == GE_FALSE) goto WriteDone;
	if (Level_WriteSky (pLevel->m_SkyFaces, &pLevel->m_SkyRotationAxis, pLevel->m_SkyRotationSpeed, pLevel->m_SkyTextureScale, ArFile) == GE_FALSE) goto WriteDone;
	if (Level_WriteCompileInfo (&pLevel->m_CompileParams, ArFile) == GE_FALSE) goto WriteDone;
	if (fprintf (ArFile, "ShowGroups %d\n", pLevel->m_GroupVisSetting) < 0) goto WriteDone;
	if (EntityViewList_WriteToFile (pLevel->m_pEntityView, ArFile) == GE_FALSE) goto WriteDone;
	if (Level_WriteGridInfo (&pLevel->m_GridSettings, ArFile) == GE_FALSE) goto WriteDone;
	if (fprintf (ArFile, "BspRebuild %d\n", pLevel->m_BspRebuildFlag) < 0) goto WriteDone;
	if (Level_WriteViewInfo (pLevel->m_ViewInfo, ArFile) == GE_FALSE) goto WriteDone;
	if (Level_WriteBrushTemplates (pLevel, ArFile) == GE_FALSE) goto WriteDone;
	if (fprintf (ArFile, "TemplatePos %f %f %f\n", pLevel->m_TemplatePos.X, pLevel->m_TemplatePos.Y, pLevel->m_TemplatePos.Z) < 0) goto WriteDone;

	// level options
	if (fprintf (ArFile, "DrawScale %f\n", pLevel->m_DrawScale) < 0) goto WriteDone;
	if (fprintf (ArFile, "LightmapScale %f\n", pLevel->m_LightmapScale) < 0) goto WriteDone;

// changed QD Actors
// add ActorBrushes to the List again
	for (i = 0; i < pLevel->m_pEntities->GetSize (); ++i)
	{
		Brush *b =(*(pLevel->m_pEntities))[i].GetActorBrush();
		if(b!=NULL)
			AppendBrush(pLevel, b);
	}
// end change
	WriteRslt = GE_TRUE;

WriteDone:
	if (fclose(ArFile) != 0) return GE_FALSE;

	return GE_TRUE ;
}

// changed QD 11/03
geBoolean CtLevelMgr::ExportTo3dtv1_32(CtLevel *pLevel, const char *Filename)
{
	FILE	*ArFile = NULL;
	char QuotedString[MAX_PATH];
	geBoolean WriteRslt;

	assert (pLevel != NULL);
	assert (Filename != NULL);

	// error checking required!
	ArFile = fopen(Filename, "wt");

	if (ArFile == NULL)
	{
		return GE_FALSE;
	}

	WriteRslt = GE_FALSE;
	if (fprintf(ArFile, "3dtVersion %d.%d\n", LEVEL_VERSION_MAJOR, LEVEL_VERSION_MINOR) < 0) goto WriteDone;

	Util_QuoteString (pLevel->m_pWadPath, QuotedString);
	if (fprintf(ArFile, "TextureLib %s\n", QuotedString) < 0) goto WriteDone;

	Util_QuoteString (pLevel->m_pHeadersDir, QuotedString);
	if (fprintf (ArFile, "HeadersDir %s\n", QuotedString) < 0) goto WriteDone;

// remove ActorBrushes from List, so they don't get written to the file
	int i;
	for (i = 0; i < pLevel->m_pEntities->GetSize (); ++i)
	{
		Brush *b =(*(pLevel->m_pEntities))[i].GetActorBrush();
		if(b!=NULL)
			RemoveBrush(pLevel, b);
	}

	if (fprintf(ArFile, "NumEntities %d\n", pLevel->m_pEntities->GetSize ()) < 0) goto WriteDone;
	if (fprintf(ArFile, "NumModels %d\n", ModelList_GetCount (pLevel->m_ModelInfo.Models)) < 0) goto WriteDone;
	if (fprintf(ArFile, "NumGroups %d\n", Group_GetCount (pLevel->m_pGroups)) < 0) goto WriteDone;
	// differs from Level_WriteToFile
	if (BrushList_ExportTo3dtv1_32 (pLevel->m_pBrushes, ArFile) == GE_FALSE) goto WriteDone;
	//
	if (Level_SaveEntities (pLevel->m_pEntities, ArFile) == GE_FALSE) goto WriteDone;
	if (ModelList_Write (pLevel->m_ModelInfo.Models, ArFile) == GE_FALSE) goto WriteDone;
	if (Group_WriteList (pLevel->m_pGroups, ArFile) == GE_FALSE) goto WriteDone;
	if (Level_WriteSky (pLevel->m_SkyFaces, &pLevel->m_SkyRotationAxis, pLevel->m_SkyRotationSpeed, pLevel->m_SkyTextureScale, ArFile) == GE_FALSE) goto WriteDone;
	if (Level_WriteCompileInfo (&pLevel->m_CompileParams, ArFile) == GE_FALSE) goto WriteDone;
	if (fprintf (ArFile, "ShowGroups %d\n", pLevel->m_GroupVisSetting) < 0) goto WriteDone;
	if (EntityViewList_WriteToFile (pLevel->m_pEntityView, ArFile) == GE_FALSE) goto WriteDone;
	if (Level_WriteGridInfo (&pLevel->m_GridSettings, ArFile) == GE_FALSE) goto WriteDone;
	if (fprintf (ArFile, "BspRebuild %d\n", pLevel->m_BspRebuildFlag) < 0) goto WriteDone;
	if (Level_WriteViewInfo (pLevel->m_ViewInfo, ArFile) == GE_FALSE) goto WriteDone;
	// differs from Level_WriteToFile
	if (Level_ExportBrushTemplatesTo3dtv1_32 (pLevel, ArFile) == GE_FALSE) goto WriteDone;
	//
	if (fprintf (ArFile, "TemplatePos %f %f %f\n", pLevel->m_TemplatePos.X, pLevel->m_TemplatePos.Y, pLevel->m_TemplatePos.Z) < 0) goto WriteDone;

	// level options
	if (fprintf (ArFile, "DrawScale %f\n", pLevel->m_DrawScale) < 0) goto WriteDone;
	if (fprintf (ArFile, "LightmapScale %f\n", pLevel->m_LightmapScale) < 0) goto WriteDone;

// add ActorBrushes to the List again
	for (i = 0; i < pLevel->m_pEntities->GetSize (); ++i)
	{
		Brush *b =(*(pLevel->m_pEntities))[i].GetActorBrush();
		if(b!=NULL)
			AppendBrush(pLevel, b);
	}
// end change

	WriteRslt = GE_TRUE;

WriteDone:
	if (fclose(ArFile) != 0) return GE_FALSE;

	return GE_TRUE ;
}


// changed QD 12/03
#define CHUNK_MAIN3DS		0x4d4d
#define CHUNK_VERSION		0x0002
#define CHUNK_EDIT3DS		0x3d3d
#define CHUNK_MESH_VERSION	0x3d3e
// global chunks
#define CHUNK_COLORRGB		0x0011
#define CHUNK_PERCENT		0x0030
#define MASTER_SCALE		0x0100
/*
#define CHUNK_OBJBLOCK		0x4000
#define CHUNK_TRIMESH		0x4100
#define CHUNK_VERTLIST		0x4110
#define CHUNK_FACELIST		0x4120
#define CHUNK_MAPLIST		0x4140
*/
// Material block
#define CHUNK_MATBLOCK		0xAFFF
#define CHUNK_MATNAME		0xA000
#define CHUNK_MATAMB		0xA010 // Ambient color
#define CHUNK_MATDIFF		0xA020 // Diffuse color
#define CHUNK_MATSPEC		0xA030 // Specular color
#define CHUNK_MATSHININESS	0xA040
#define CHUNK_MATSHIN2PCT	0xA041
#define CHUNK_MATTRANS		0xA050
#define CHUNK_MATXPFALL		0xA052
#define CHUNK_MATREFBLUR	0xA053
#define CHUNK_MATSHADING	0xA100
#define CHUNK_MATDECAL		0xA084
#define CHUNK_MATWIRESIZE	0xA087
#define CHUNK_MAP			0xA200
#define CHUNK_MAPNAME		0xA300 // name of bitmap
#define CHUNK_MAPTILING		0xA351

//#define CHUNK_MATAMB		0xA010 // Ambient color
// Keyframe chunks
#define CHUNK_KEYFRAME		0xB000
#define CHUNK_KF_HDR		0xB00a
#define CHUNK_KF_SEG		0xB008 // start, end
#define CHUNK_KF_CURTIME	0xB009

geBoolean CtLevelMgr::ExportTo3ds(CtLevel *pLevel, const char *Filename, BrushList *BList,
							int ExpSelected, geBoolean ExpLights, int GroupID)
{
	FILE	*f;
	geBoolean WriteRslt;
	int size, i;
	geBoolean *WrittenTex;

	if(!pLevel) return GE_FALSE;
	if(!Filename) return GE_FALSE;

	// error checking required!
	f = fopen(Filename, "wb");

	if (!f)	return GE_FALSE;

	WriteRslt = GE_FALSE;

	// get the number of textures
	WrittenTex=(geBoolean *)calloc(sizeof(geBoolean), pLevel->m_pWadFile->mBitmapCount);
	// which textures are used?
	BrushList_GetUsedTextures(BList, WrittenTex, pLevel->m_pWadFile);

	/*
	We will squeeze in CHUNK_MAIN3DS, CHUNK_VERSION, CHUNK_EDIT3DS, CHUNK_MESH_VERSION
	when we know the size of the file, so move the filepointer forward now
	*/

	fseek(f, 32L, SEEK_SET);

	// write all used materials to the file
	for(i=0;i<pLevel->m_pWadFile->mBitmapCount;i++)
	{
		if(WrittenTex[i])
		{
			// 3ds only allows DOS 8.3 file names, so cut the name if necessary
			char matname[9];
			int j,k;
			strncpy (matname, pLevel->m_pWadFile->mBitmaps[i].Name, 9);
			matname[8] = '\0';
			for(j=0;matname[j]!='\0';j++);


			TypeIO_WriteUshort(f, CHUNK_MATBLOCK);
			TypeIO_WriteInt(f, 6+6+j+1 +(3*15) + 102 +6 +8 +6+j+5 +8);

			// write material name
			TypeIO_WriteUshort(f, CHUNK_MATNAME);
			TypeIO_WriteInt(f,(6+j+1));
			for(k=0;k<=j;k++)
				TypeIO_WriteUChar(f, matname[k]);

			// ambient color
			TypeIO_WriteUshort(f, CHUNK_MATAMB);		//0xA010  Ambient color
			TypeIO_WriteInt(f,6+6+3);

			TypeIO_WriteUshort(f, CHUNK_COLORRGB);
			TypeIO_WriteInt(f,6+3);
			TypeIO_WriteUChar(f, (char)120);
			TypeIO_WriteUChar(f, (char)120);
			TypeIO_WriteUChar(f, (char)120);

			// diffuse color
			TypeIO_WriteUshort(f, CHUNK_MATDIFF);		//0xA020  Diffuse color
			TypeIO_WriteInt(f,6+6+3);

			TypeIO_WriteUshort(f, CHUNK_COLORRGB);
			TypeIO_WriteInt(f,6+3);
			TypeIO_WriteUChar(f, (char)120);
			TypeIO_WriteUChar(f, (char)120);
			TypeIO_WriteUChar(f, (char)120);

			// specular color
			TypeIO_WriteUshort(f, CHUNK_MATSPEC);		//0xA030  Specular color
			TypeIO_WriteInt(f,6+6+3);

			TypeIO_WriteUshort(f, CHUNK_COLORRGB);
			TypeIO_WriteInt(f,6+3);
			TypeIO_WriteUChar(f, (char)120);
			TypeIO_WriteUChar(f, (char)120);
			TypeIO_WriteUChar(f, (char)120);

			TypeIO_WriteUshort(f, CHUNK_MATSHININESS);
			TypeIO_WriteInt(f,14);
			TypeIO_WriteUshort(f, CHUNK_PERCENT);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 0);

			TypeIO_WriteUshort(f, CHUNK_MATSHIN2PCT);
			TypeIO_WriteInt(f,14);
			TypeIO_WriteUshort(f, CHUNK_PERCENT);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 0);

			TypeIO_WriteUshort(f, CHUNK_MATTRANS);
			TypeIO_WriteInt(f,14);
			TypeIO_WriteUshort(f, CHUNK_PERCENT);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 0);

			TypeIO_WriteUshort(f, CHUNK_MATXPFALL);
			TypeIO_WriteInt(f,14);
			TypeIO_WriteUshort(f, CHUNK_PERCENT);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 0);

			TypeIO_WriteUshort(f, CHUNK_MATREFBLUR);
			TypeIO_WriteInt(f,14);
			TypeIO_WriteUshort(f, CHUNK_PERCENT);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 0);

			TypeIO_WriteUshort(f, CHUNK_MATSHADING);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 3);

			TypeIO_WriteUshort(f, CHUNK_MATDECAL);
			TypeIO_WriteInt(f,14);
			TypeIO_WriteUshort(f, CHUNK_PERCENT);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 0);

			TypeIO_WriteUshort(f, CHUNK_MATWIRESIZE);
			TypeIO_WriteInt(f,10);
			TypeIO_WriteFloat(f, 1.0f);

			// texture map
			TypeIO_WriteUshort(f, CHUNK_MAP);
			TypeIO_WriteInt(f,6+8+6+j+5+8);

			TypeIO_WriteUshort(f, CHUNK_PERCENT);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 100);

			// write map name
			TypeIO_WriteUshort(f, CHUNK_MAPNAME);
			TypeIO_WriteInt(f, 6+j+5);
			for(k=0;k<j;k++)
				TypeIO_WriteUChar(f, matname[k]);
			TypeIO_WriteUChar(f, '.');
			if(geBitmap_HasAlpha(pLevel->m_pWadFile->mBitmaps[i].bmp))
			{
				TypeIO_WriteUChar(f, 't');
				TypeIO_WriteUChar(f, 'g');
				TypeIO_WriteUChar(f, 'a');
			}
			else
			{
				TypeIO_WriteUChar(f, 'b');
				TypeIO_WriteUChar(f, 'm');
				TypeIO_WriteUChar(f, 'p');
			}

			TypeIO_WriteUChar(f, '\0');

			TypeIO_WriteUshort(f, CHUNK_MAPTILING);
			TypeIO_WriteInt(f,8);
			TypeIO_WriteUshort(f, 0);
		}
	}

    // write out the master scale
    TypeIO_WriteUshort(f, MASTER_SCALE);
	TypeIO_WriteInt(f, 10);
	TypeIO_WriteFloat(f, 1.0f);

	// export the brushes
	if (BrushList_ExportTo3ds (BList, f, GE_FALSE) == GE_FALSE) goto WriteDone;

// changed QD 12/03
	int size_kf;
	size_kf=0;

	if(ExpLights)
	{
		int LCount, SLCount;
		LCount = SLCount = 0;
		// export the light entities
		if (Level_ExportLightsTo3ds(pLevel->m_pEntities, f, ExpSelected, GroupID, &LCount, &SLCount) == GE_FALSE) goto WriteDone;
		// lights need keyframes or they will be placed at the origin by default
		// baaah!!!

		if(LCount>0||SLCount>0)
		{
			size_kf=6+21+14+10+LCount*110+SLCount*272;
			TypeIO_WriteUshort(f, CHUNK_KEYFRAME);
			TypeIO_WriteInt(f, size_kf);

			TypeIO_WriteUshort(f, CHUNK_KF_HDR);
			TypeIO_WriteInt(f, 21);
			TypeIO_WriteUshort(f, 5);
			TypeIO_WriteUChar(f, 'M');
			TypeIO_WriteUChar(f, 'A');
			TypeIO_WriteUChar(f, 'X');
			TypeIO_WriteUChar(f, 'S');
			TypeIO_WriteUChar(f, 'C');
			TypeIO_WriteUChar(f, 'E');
			TypeIO_WriteUChar(f, 'N');
			TypeIO_WriteUChar(f, 'E');
			TypeIO_WriteUChar(f, '\0');
			TypeIO_WriteUChar(f, 'd');
			TypeIO_WriteUChar(f, '\0');
			TypeIO_WriteUshort(f, 0);

			TypeIO_WriteUshort(f, CHUNK_KF_SEG);
			TypeIO_WriteInt(f, 14);
			TypeIO_WriteInt(f, 0);
			TypeIO_WriteInt(f, 100);

			TypeIO_WriteUshort(f, CHUNK_KF_CURTIME);
			TypeIO_WriteInt(f, 10);
			TypeIO_WriteInt(f, 0);

			if (Level_ExportLightsKFTo3ds(pLevel->m_pEntities, f, ExpSelected, GroupID) == GE_FALSE) goto WriteDone;
		}
	}
// end change

	size = ftell(f);
	fseek(f, 0L, SEEK_SET );

	TypeIO_WriteUshort(f,CHUNK_MAIN3DS);
	TypeIO_WriteInt(f, size);

	TypeIO_WriteUshort(f, CHUNK_VERSION);
	TypeIO_WriteInt(f, 10);
	TypeIO_WriteInt(f, 3);

	TypeIO_WriteUshort(f,CHUNK_EDIT3DS);
// changed QD 12/03
	TypeIO_WriteInt(f, size-16-size_kf);
// end change
	TypeIO_WriteUshort(f,CHUNK_MESH_VERSION);
	TypeIO_WriteInt(f, 10);
	TypeIO_WriteInt(f, 3);

	WriteRslt = GE_TRUE;

WriteDone:

	free(WrittenTex);

	if (fclose(f) != 0) return GE_FALSE;

// changed QD 12/03
	if((size-size_kf)<=42)
		_unlink(Filename);
// end change

	return WriteRslt;
}
// end change

CEntityArray *CtLevelMgr::GetEntities (CtLevel *pLevel)
{
	return pLevel->m_pEntities;
}

BrushList *CtLevelMgr::GetBrushes (CtLevel *pLevel)
{
	return pLevel->m_pBrushes;
}

SkyFaceTexture *CtLevelMgr::GetSkyInfo (CtLevel *pLevel, geVec3d *Axis, geFloat *Speed, geFloat *Scale)
{
	*Axis = pLevel->m_SkyRotationAxis;
	*Speed = pLevel->m_SkyRotationSpeed;
	*Scale = pLevel->m_SkyTextureScale;
	return pLevel->m_SkyFaces;
}


void CtLevelMgr::SetSkyRotationAxis (CtLevel *pLevel, const geVec3d *Axis)
{
	pLevel->m_SkyRotationAxis = *Axis;
}

void CtLevelMgr::SetSkyRotationSpeed (CtLevel *pLevel, const geFloat Speed)
{
	pLevel->m_SkyRotationSpeed = Speed;
}

void CtLevelMgr::SetSkyTextureScale (CtLevel *pLevel, const geFloat Scale)
{
	pLevel->m_SkyTextureScale = Scale;
}

ModelInfo_Type *CtLevelMgr::GetModelInfo (CtLevel *pLevel)
{
	assert (pLevel != NULL);

	return &(pLevel->m_ModelInfo);
}

Model *CtLevelMgr::GetModel (CtLevel *pLevel, int ModelId)
{
	return ModelList_FindById (pLevel->m_ModelInfo.Models, ModelId);
}

void CtLevelMgr::AddModel (CtLevel *pLevel, Model *pModel)
{
	ModelList_AddModel (pLevel->m_ModelInfo.Models, pModel);
}


GroupListType *CtLevelMgr::GetGroups (CtLevel *pLevel)
{
	assert (pLevel != NULL);

	return pLevel->m_pGroups;
}

Group *CtLevelMgr::GetGroup (CtLevel *pLevel, int GroupId)
{
	assert (pLevel != NULL);
	
	return GroupList_GetFromId (pLevel->m_pGroups, GroupId);
}

void CtLevelMgr::AddGroup (CtLevel *pLevel, Group *pGroup)
{
	GroupList_Add (pLevel->m_pGroups, pGroup);
}


const char *CtLevelMgr::GetWadPath (const CtLevel *pLevel)
{
	assert (pLevel != NULL);

	return (pLevel->m_pWadPath);
}

CString CtLevelMgr::CloneWadPath(const CtLevel *pLevel)
{
	CString strClone = (CString)pLevel->m_pWadPath;
	//char* charTemp = strClone.GetBuffer(strClone.GetLength());
	return strClone;
}




void CtLevelMgr::SetWadPath (CtLevel *pLevel, const char *NewWad)
{
	if (pLevel->m_pWadPath != NULL)
	{
		geRam_Free (pLevel->m_pWadPath);
	}
	pLevel->m_pWadPath = Util_Strdup (NewWad);
}


SizeInfo *CtLevelMgr::GetWadSizeInfos (CtLevel *pLevel)
{
	assert (pLevel != NULL);

	return pLevel->m_pWadSizeInfos;
}

CWadFile *CtLevelMgr::GetWadFile (CtLevel *pLevel)
{
	return pLevel->m_pWadFile;
}



int CtLevelMgr::AddEntity (CtLevel *pLevel, CEntity &Entity)
{
	assert (pLevel != NULL);

	Level_AssignEntityName (pLevel, &Entity);
// changed QD Actors
	int index = pLevel->m_pEntities->Add (Entity);
	char ActorFile[256], ActorDir[256], PawnIni[256];
	strcpy(PawnIni, GetPawnIniPath(pLevel));

	if((*(pLevel->m_pEntities))[index].HasActor(ActorFile, PawnIni))
	{
		Brush *pBrush;
		strcpy(ActorDir, GetActorsDirectory(pLevel));
		pBrush=(*(pLevel->m_pEntities))[index].CreateActorBrush(ActorFile, ActorDir, PawnIni);
		if(pBrush)
		{
			AppendBrush(pLevel,pBrush);
			if(!pLevel->m_ShowActors)
				Brush_SetVisible(pBrush, GE_FALSE);
		}
	}
	return index;// pLevel->m_pEntities->Add (Entity);
// end change
}

void CtLevelMgr::AppendBrush (CtLevel *pLevel, Brush *pBrush)
{
	assert (pLevel != NULL);
	assert (pBrush != NULL);

	BrushList_Append (pLevel->m_pBrushes, pBrush);
}

void CtLevelMgr::RemoveBrush (CtLevel *pLevel, Brush *pBrush)
{
	assert (pLevel != NULL);
	assert (pBrush != NULL);

	BrushList_Remove (pLevel->m_pBrushes, pBrush);
}


int CtLevelMgr::EnumEntities (CtLevel *pLevel, void *lParam, EntityList_CB Callback)
{
	assert (pLevel != NULL);

	return EntityList_Enum (*(pLevel->m_pEntities), lParam, Callback);
}

int CtLevelMgr::EnumBrushes (CtLevel *pLevel, void *lParam, BrushList_CB Callback)
{
	return BrushList_Enum (pLevel->m_pBrushes, lParam, Callback);
}

int CtLevelMgr::EnumAllBrushes (CtLevel *pLevel, void *lParam, BrushList_CB Callback)
{
	return BrushList_EnumAll (pLevel->m_pBrushes, lParam, Callback);
}


int CtLevelMgr::EnumLeafBrushes (CtLevel *pLevel, void *lParam, BrushList_CB Callback)
{
	return BrushList_EnumLeafBrushes (pLevel->m_pBrushes, lParam, Callback);
}



void CtLevelMgr::TranslateAll (CtLevel *pLevel, const geVec3d *VecXlate)
{
	int i;
	CEntityArray *Entities;

	assert (pLevel != NULL);

	// move all brushes
	BrushList_Move (pLevel->m_pBrushes, VecXlate);
	// and entities
	Entities = pLevel->m_pEntities;
	for (i = 0; i < Entities->GetSize (); ++i)
	{
		(*Entities)[i].Move (VecXlate);
	}

	// update the models' origins
	{
		Model *pModel;
		ModelIterator mi;

		pModel = ModelList_GetFirst (pLevel->m_ModelInfo.Models, &mi);
		while (pModel != NULL)
		{
			Model_UpdateOrigin (pModel, BRUSH_MOVE, VecXlate);
			pModel = ModelList_GetNext (pLevel->m_ModelInfo.Models, &mi);
		}
	}
}

void CtLevelMgr::CollapseGroups (CtLevel *pLevel, int StartingGroup)
{
	assert (pLevel != NULL);

	GroupList_Collapse (pLevel->m_pGroups, StartingGroup, pLevel->m_pBrushes, pLevel->m_pEntities);
}

void CtLevelMgr::CollapseModels (CtLevel *pLevel, int StartingModel)
{
	assert (pLevel != NULL);

	ModelList_Collapse (pLevel->m_ModelInfo.Models, StartingModel, pLevel->m_pBrushes);
}

float CtLevelMgr::GetGridSnapSize (const CtLevel *pLevel)
{
	const GridInfo *pGridInfo = &pLevel->m_GridSettings;

	switch (pGridInfo->SnapType)
	{
		case GridMetric :
			return CENTIMETERS_TO_ENGINE (pGridInfo->MetricSnapSize);
			break;
		default :
			assert (0);
		case GridTexel :
			return (float)pGridInfo->TexelSnapSize;
			break;
	}
}

int CtLevelMgr::GetRotationSnap (const CtLevel *pLevel)
{
	return pLevel->m_GridSettings.RotationSnap;
}

int CtLevelMgr::GetGridType (const CtLevel *pLevel)
{
	return pLevel->m_GridSettings.GridType;
}

geBoolean CtLevelMgr::UseGrid (const CtLevel *pLevel)
{
	return pLevel->m_GridSettings.UseGrid;
}

GridInfo *CtLevelMgr::GetGridInfo (CtLevel *pLevel)
{
	return &pLevel->m_GridSettings;
}

geBoolean CtLevelMgr::RebuildBspAlways (const CtLevel *pLevel)
{
	return pLevel->m_BspRebuildFlag;
}

void CtLevelMgr::SetBspRebuild (CtLevel *pLevel, geBoolean RebuildFlag)
{
	pLevel->m_BspRebuildFlag = RebuildFlag;
}

ViewStateInfo *CtLevelMgr::GetViewStateInfo (CtLevel *pLevel, int iView)
{
	assert (iView >= 0);
	assert (iView < 4);

	return &pLevel->m_ViewInfo[iView];
}

BrushTemplate_Arch *CtLevelMgr::GetArchTemplate (CtLevel *pLevel)
{
	return &pLevel->m_ArchTemplate;
}

BrushTemplate_Box  *CtLevelMgr::GetBoxTemplate (CtLevel *pLevel)
{
	return &pLevel->m_BoxTemplate;
}

BrushTemplate_Cone *CtLevelMgr::GetConeTemplate (CtLevel *pLevel)
{
	return &pLevel->m_ConeTemplate;
}

BrushTemplate_Cylinder *CtLevelMgr::GetCylinderTemplate (CtLevel *pLevel)
{
	return &pLevel->m_CylinderTemplate;
}

BrushTemplate_Spheroid *CtLevelMgr::GetSpheroidTemplate (CtLevel *pLevel)
{
	return &pLevel->m_SpheroidTemplate;
}

BrushTemplate_Staircase *CtLevelMgr::GetStaircaseTemplate (CtLevel *pLevel)
{
	return &pLevel->m_StaircaseTemplate;
}

geVec3d *CtLevelMgr::GetTemplatePos (CtLevel *pLevel)
{
	return &pLevel->m_TemplatePos;
}

float CtLevelMgr::GetDrawScale (const CtLevel *pLevel)
{
	return pLevel->m_DrawScale;
}

float CtLevelMgr::GetLightmapScale (const CtLevel *pLevel)
{
	return pLevel->m_LightmapScale;
}

void CtLevelMgr::SetDrawScale (CtLevel *pLevel, float Scale)
{
	pLevel->m_DrawScale = Scale;
}

void CtLevelMgr::SetLightmapScale (CtLevel *pLevel, float Scale)
{
	pLevel->m_LightmapScale = Scale;
}

const char *CtLevelMgr::GetHeadersDirectory (const CtLevel *pLevel)
{
	return pLevel->m_pHeadersDir;
}

CString CtLevelMgr::CloneHeadersDirectory(const CtLevel *pLevel)
{
	CString strClone = (CString)pLevel->m_pHeadersDir;
	//char* charTemp = strClone.GetBuffer(strClone.GetLength());
	return strClone;
}

// changed QD Actors
const char *CtLevelMgr::GetActorsDirectory (const CtLevel *pLevel)
{
	return pLevel->m_pActorsDir;
}

void CtLevelMgr::SetActorsDir(CtLevel *pLevel, const char *NewActorsDir)
{
	if (pLevel->m_pActorsDir != NULL)
	{
		geRam_Free (pLevel->m_pActorsDir);
	}
	pLevel->m_pActorsDir = Util_Strdup (NewActorsDir);
}

CString CtLevelMgr::CloneActorsDir(const CtLevel *pLevel)
{
	CString strClone = (CString)pLevel->m_pActorsDir;
	//char* charTemp = strClone.GetBuffer(strClone.GetLength());
	return strClone;
}

const char *CtLevelMgr::GetPawnIniPath (const CtLevel *pLevel)
{
	assert (pLevel != NULL);

	return (pLevel->m_pPawnIniPath);
}

void CtLevelMgr::SetPawnIniPath (CtLevel *pLevel, const char *PawnIni)
{
	if (pLevel->m_pPawnIniPath != NULL)
	{
		geRam_Free (pLevel->m_pPawnIniPath);
	}
	pLevel->m_pPawnIniPath = Util_Strdup (PawnIni);
}

CString CtLevelMgr::ClonePawnIniPath(const CtLevel *pLevel)
{
	CString strClone = (CString)pLevel->m_pPawnIniPath;
	//char* charTemp = strClone.GetBuffer(strClone.GetLength());
	return strClone;
}

void CtLevelMgr::SetShowActors(CtLevel *pLevel, geBoolean Show)
{
	pLevel->m_ShowActors=Show;
}

geBoolean CtLevelMgr::GetShowActors(const CtLevel *pLevel)
{
	return pLevel->m_ShowActors;
}
// end change
