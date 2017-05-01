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
#include "tObjectArray.h"

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





struct SkyFaceTexture
{
	char *TextureName;
	geBoolean Apply;
};

enum
{
	SkyFace_Left	= 0,
	SkyFace_Right	= 1,
	SkyFace_Top		= 2,
	SkyFace_Bottom	= 3,
	SkyFace_Front	= 4,
	SkyFace_Back	= 5,
	SkyFace_Null	= -1		//	added for g3dc
};

enum
{
	Group_ShowAll,
	Group_ShowVisible,
	Group_ShowCurrent
};

enum {GridMetric = 0, GridTexel = 1};

enum
{
	GridSize_Centimeter	=1,
	GridSize_Decimeter	=10,
	GridSize_Meter		=100
};

typedef struct
{
	geBoolean UseGrid;
	int GridType;

	int SnapType;
	int MetricSnapSize;
	int TexelSnapSize;
	int RotationSnap;
} GridInfo;

typedef struct
{
	geBoolean IsValid;
	float ZoomFactor;
	geVec3d PitchRollYaw;
	geVec3d CameraPos;
} ViewStateInfo;

//typedef struct tag_Level Level;







// CtLevel command target

class CtLevel : public CObject
{
public:
	CtLevel();
	virtual ~CtLevel();

    BrushList *m_pBrushes;
	CEntityArray *m_pEntities;
    char *m_pWadPath;
	char *m_pHeadersDir;
	EntTypeNameList	*m_pEntTypeNames;
	GroupListType *m_pGroups;
	SizeInfo	*m_pWadSizeInfos;
	CWadFile	*m_pWadFile;
	EntityTable	*m_pEntityDefs;

	ModelInfo_Type	m_ModelInfo;

	SkyFaceTexture m_SkyFaces[6];
	geVec3d m_SkyRotationAxis;
	geFloat m_SkyRotationSpeed;
	geFloat	m_SkyTextureScale;
	
	// level edit settings
	CompileParamsType m_CompileParams;
	int m_GroupVisSetting;
	EntityViewList *m_pEntityView;

	GridInfo m_GridSettings;
	geBoolean m_BspRebuildFlag;
	ViewStateInfo m_ViewInfo[NUM_VIEWS];

	BrushTemplate_Arch m_ArchTemplate;
	BrushTemplate_Box	m_BoxTemplate;
	BrushTemplate_Cone	m_ConeTemplate;
	BrushTemplate_Cylinder m_CylinderTemplate;
	BrushTemplate_Spheroid	m_SpheroidTemplate;
	BrushTemplate_Staircase m_StaircaseTemplate;

	geVec3d m_TemplatePos;

	float m_DrawScale;		// default draw scale
	float m_LightmapScale;	// default lightmap scale

	CtLevel(const CtLevel &s)	//	copy constructor
	{
		m_ArchTemplate = s.m_ArchTemplate;
		m_BoxTemplate = s.m_BoxTemplate;

/*		m_BoxTemplate.Solid = s.m_BoxTemplate.Solid;
		m_BoxTemplate.TCut	= s.m_BoxTemplate.TCut;
		m_BoxTemplate.Thickness	= s.m_BoxTemplate.Thickness;
		m_BoxTemplate.TSheet	= s.m_BoxTemplate.TSheet;
		m_BoxTemplate.XSizeBot	= s.m_BoxTemplate.XSizeBot;
		m_BoxTemplate.XSizeTop	= s.m_BoxTemplate.XSizeTop;
		m_BoxTemplate.YSize	= s.m_BoxTemplate.YSize;
		m_BoxTemplate.ZSizeBot	= s.m_BoxTemplate.ZSizeBot;
		m_BoxTemplate.ZSizeTop	= s.m_BoxTemplate.ZSizeTop;
*/

		m_BspRebuildFlag = s.m_BspRebuildFlag;
		m_CompileParams = s.m_CompileParams;
		m_ConeTemplate = s.m_ConeTemplate;
		m_CylinderTemplate = s.m_CylinderTemplate;
		m_DrawScale = s.m_DrawScale;
		m_GridSettings = s.m_GridSettings;
		m_GroupVisSetting = s.m_GroupVisSetting;
		m_LightmapScale = s.m_LightmapScale;
		m_ModelInfo = s.m_ModelInfo;
	
		m_pBrushes = s.m_pBrushes;
		m_pEntities = s.m_pEntities;
//		m_Entities = s.m_Entities;
		m_pEntityDefs = s.m_pEntityDefs;
		m_pEntityView = s.m_pEntityView;

		m_pEntTypeNames = s.m_pEntTypeNames;
		m_pGroups = s.m_pGroups;
		m_pHeadersDir = s.m_pHeadersDir;
		m_pWadFile = s.m_pWadFile;
		m_pWadPath = s.m_pWadPath;
		m_pWadSizeInfos = s.m_pWadSizeInfos;
		
		for (int i = 0; i < 6; i++)
		{
			m_SkyFaces[i].Apply = s.m_SkyFaces[i].Apply;
			m_SkyFaces[i].TextureName = s.m_SkyFaces[i].TextureName;
		}
	
		m_SkyRotationAxis = s.m_SkyRotationAxis;
		m_SkyRotationSpeed = s.m_SkyRotationSpeed;
		m_SkyTextureScale = s.m_SkyTextureScale;

		m_SpheroidTemplate = s.m_SpheroidTemplate;
		m_StaircaseTemplate = s.m_StaircaseTemplate;
		m_TemplatePos = s.m_TemplatePos;
		m_ViewInfo->CameraPos = s.m_ViewInfo->CameraPos;
		m_ViewInfo->IsValid = s.m_ViewInfo->IsValid;
		m_ViewInfo->PitchRollYaw = s.m_ViewInfo->PitchRollYaw;
		m_ViewInfo->ZoomFactor = s.m_ViewInfo->ZoomFactor;
	}

	CtLevel& operator=(const CtLevel &s)	//	assignment operator
	{
		m_ArchTemplate = s.m_ArchTemplate;
		m_BoxTemplate = s.m_BoxTemplate;

/*		m_BoxTemplate.Solid = s.m_BoxTemplate.Solid;
		m_BoxTemplate.TCut	= s.m_BoxTemplate.TCut;
		m_BoxTemplate.Thickness	= s.m_BoxTemplate.Thickness;
		m_BoxTemplate.TSheet	= s.m_BoxTemplate.TSheet;
		m_BoxTemplate.XSizeBot	= s.m_BoxTemplate.XSizeBot;
		m_BoxTemplate.XSizeTop	= s.m_BoxTemplate.XSizeTop;
		m_BoxTemplate.YSize	= s.m_BoxTemplate.YSize;
		m_BoxTemplate.ZSizeBot	= s.m_BoxTemplate.ZSizeBot;
		m_BoxTemplate.ZSizeTop	= s.m_BoxTemplate.ZSizeTop;
*/

		m_BspRebuildFlag = s.m_BspRebuildFlag;
		m_CompileParams = s.m_CompileParams;
		m_ConeTemplate = s.m_ConeTemplate;
		m_CylinderTemplate = s.m_CylinderTemplate;
		m_DrawScale = s.m_DrawScale;
		m_GridSettings = s.m_GridSettings;
		m_GroupVisSetting = s.m_GroupVisSetting;
		m_LightmapScale = s.m_LightmapScale;
		m_ModelInfo = s.m_ModelInfo;
	
		m_pBrushes = s.m_pBrushes;
		m_pEntities = s.m_pEntities;
//		m_Entities = s.m_Entities;
		m_pEntityDefs = s.m_pEntityDefs;
		m_pEntityView = s.m_pEntityView;

		m_pEntTypeNames = s.m_pEntTypeNames;
		m_pGroups = s.m_pGroups;
		m_pHeadersDir = s.m_pHeadersDir;
		m_pWadFile = s.m_pWadFile;
		m_pWadPath = s.m_pWadPath;
		m_pWadSizeInfos = s.m_pWadSizeInfos;
	
		for (int i = 0; i < 6; i++)
		{
			m_SkyFaces[i].Apply = s.m_SkyFaces[i].Apply;
			m_SkyFaces[i].TextureName = s.m_SkyFaces[i].TextureName;
		}
		m_SkyRotationAxis = s.m_SkyRotationAxis;
		m_SkyRotationSpeed = s.m_SkyRotationSpeed;
		m_SkyTextureScale = s.m_SkyTextureScale;

		m_SpheroidTemplate = s.m_SpheroidTemplate;
		m_StaircaseTemplate = s.m_StaircaseTemplate;
		m_TemplatePos = s.m_TemplatePos;
		m_ViewInfo->CameraPos = s.m_ViewInfo->CameraPos;
		m_ViewInfo->IsValid = s.m_ViewInfo->IsValid;
		m_ViewInfo->PitchRollYaw = s.m_ViewInfo->PitchRollYaw;
		m_ViewInfo->ZoomFactor = s.m_ViewInfo->ZoomFactor;
		return *this;
	}
};


