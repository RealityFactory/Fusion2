#include "tLevel.h"
#include "SelFaceList.h"

#pragma once

// CGEditProDocVars

class CGEditProDocVars : public CObject
{
	DECLARE_SERIAL (CGEditProDocVars)

public:
	CGEditProDocVars();

	CtLevel	m_Level;
	SelBrushList *m_pSelBrushes;
	SelBrushList *m_pTempSelBrushes;
	SelFaceList *m_pSelFaces;
	CEntityArray *m_pSelEntities;
	CEntityArray *m_pTempSelEntities;


	
	CGEditProDocVars(const CGEditProDocVars &s)	//	copy constructor
	{
		m_Level = s.m_Level;
		m_pSelBrushes	=	s.m_pSelBrushes;
		m_pTempSelBrushes = s.m_pTempSelBrushes;
		m_pSelEntities = s.m_pSelEntities;
		m_pTempSelEntities = s.m_pTempSelEntities;
	}
	CGEditProDocVars& operator=(const CGEditProDocVars &s)	//	assignment operator
	{
		m_Level = s.m_Level;
		m_pSelBrushes	=	s.m_pSelBrushes;
		m_pTempSelBrushes = s.m_pTempSelBrushes;
		m_pSelEntities = s.m_pSelEntities;
		m_pTempSelEntities = s.m_pTempSelEntities;

		return *this;
	}


	bool operator!=(const CGEditProDocVars &s)	//	assignment operator
	{
//		if (m_Level.m_ArchTemplate != s.m_Level.m_ArchTemplate) return false;
//		if (m_restorepath != s.m_restorepath) return false;
//		if (m_oldpath != s.m_oldpath) return false;
//		if (m_strWorldPath != s.m_strWorldPath) return false;
//		if (m_mapPluginsMap != s.m_mapPluginsMap) return false;
//		if (m_arrayPluginClassNames != s.m_arrayPluginClassNames) return false;

		return true;
	}

	bool operator ==(const CGEditProDocVars &s)const	//	assignment operator
	{
//		if (m_Level.m_ArchTemplate != s.m_Level.m_ArchTemplate) return false;
//		if (m_restorepath != s.m_restorepath) return false;
//		if (m_oldpath != s.m_oldpath) return false;
//		if (m_strWorldPath != s.m_strWorldPath) return false;
//		if (m_mapPluginsMap != s.m_mapPluginsMap) return false;
//		if (m_arrayPluginClassNames != s.m_arrayPluginClassNames) return false;

		return true;
	}

	void	Serialize(CArchive &ar);


	virtual ~CGEditProDocVars();

protected:
	
};


