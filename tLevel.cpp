// tLevel.cpp : implementation file
//

#include "stdafx.h"
#include "GEditPro.h"
#include "tLevel.h"


// CtLevel

CtLevel::CtLevel()
{
	m_pBrushes = NULL;
	m_pEntities = NULL;
    m_pWadPath = NULL;
	m_pHeadersDir = NULL;
	m_pEntTypeNames = NULL;
	m_pGroups = NULL;
	m_pWadSizeInfos = NULL;
	m_pWadFile = NULL;
	m_pEntityDefs = NULL;
	m_pEntityView = NULL;
// changed QD Actors
	m_pActorsDir = NULL;
	m_pPawnIniPath = NULL;
	m_ShowActors = GE_TRUE;
// end change

	m_pEntities = new CEntityArray;

	m_ViewInfo[0].CameraPos.X = 0.0f;
	m_ViewInfo[0].CameraPos.Y = 0.0f;
	m_ViewInfo[0].CameraPos.Z = 0.0f;
	m_ViewInfo[0].IsValid = 0;
	m_ViewInfo[0].PitchRollYaw.X = 0.0f;
	m_ViewInfo[0].PitchRollYaw.Y = 0.0f;
	m_ViewInfo[0].PitchRollYaw.Z = 0.0f;
	m_ViewInfo[0].ZoomFactor = 0.0f;

	m_ViewInfo[1].CameraPos.X = 0.0f;
	m_ViewInfo[1].CameraPos.Y = 0.0f;
	m_ViewInfo[1].CameraPos.Z = 0.0f;
	m_ViewInfo[1].IsValid = 0;
	m_ViewInfo[1].PitchRollYaw.X = 0.0f;
	m_ViewInfo[1].PitchRollYaw.Y = 0.0f;
	m_ViewInfo[1].PitchRollYaw.Z = 0.0f;
	m_ViewInfo[1].ZoomFactor = 0.0f;

	m_ViewInfo[2].CameraPos.X = 0.0f;
	m_ViewInfo[2].CameraPos.Y = 0.0f;
	m_ViewInfo[2].CameraPos.Z = 0.0f;
	m_ViewInfo[2].IsValid = 0;
	m_ViewInfo[2].PitchRollYaw.X = 0.0f;
	m_ViewInfo[2].PitchRollYaw.Y = 0.0f;
	m_ViewInfo[2].PitchRollYaw.Z = 0.0f;
	m_ViewInfo[2].ZoomFactor = 0.0f;

	m_ViewInfo[3].CameraPos.X = 0.0f;
	m_ViewInfo[3].CameraPos.Y = 0.0f;
	m_ViewInfo[3].CameraPos.Z = 0.0f;
	m_ViewInfo[3].IsValid = 0;
	m_ViewInfo[3].PitchRollYaw.X = 0.0f;
	m_ViewInfo[3].PitchRollYaw.Y = 0.0f;
	m_ViewInfo[3].PitchRollYaw.Z = 0.0f;
	m_ViewInfo[3].ZoomFactor = 0.0f;

	m_ModelInfo.CurrentModel = 0;
	m_ModelInfo.Models = NULL;

	m_DrawScale = 0.0f;
	m_LightmapScale = 0.0f;	// default lightmap scale
	m_SkyRotationSpeed = 0.0f;
	m_SkyTextureScale = 0.0f;
}

CtLevel::~CtLevel()
{
//	if (m_pEntities) {delete m_pEntities; m_pEntities = NULL;}
	/*
	if(	m_pBrushes)			{delete	m_pBrushes;			m_pBrushes = NULL;}
	if(	m_pEntities)		{delete	m_pEntities;		m_pEntities = NULL;}
	if(	m_pWadPath)			{delete	m_pWadPath;			m_pWadPath = NULL;}
	if(	m_pHeadersDir)		{delete	m_pHeadersDir;		m_pHeadersDir = NULL;}
	if(	m_pEntTypeNames)	{delete	m_pEntTypeNames;	m_pEntTypeNames = NULL;}
	if(	m_pGroups)			{delete	m_pGroups;			m_pGroups = NULL;}
	if(	m_pWadSizeInfos)	{delete	m_pWadSizeInfos;	m_pWadSizeInfos = NULL;}
	if(	m_pWadFile)			{delete	m_pWadFile;			m_pWadFile = NULL;}
	if(	m_pEntityDefs)		{delete	m_pEntityDefs;		m_pEntityDefs = NULL;}
	if(	m_pEntityView)		{delete	m_pEntityView;		m_pEntityView = NULL;}
*/
}


// CtLevel member functions
