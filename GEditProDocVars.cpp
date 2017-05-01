// GEditProDocVars.cpp : implementation file
//

#include "stdafx.h"
#include "GEditPro.h"
#include "GEditProDocVars.h"


// CGEditProDocVars

IMPLEMENT_SERIAL (CGEditProDocVars, CObject, 1)
CGEditProDocVars::CGEditProDocVars()
{
	m_pSelBrushes	=	NULL;
	m_pTempSelBrushes = NULL;
	m_pSelFaces	= NULL;
	m_pSelEntities = NULL;
	m_pTempSelEntities = NULL;
}

CGEditProDocVars::~CGEditProDocVars()
{

}





// CGEditProDocVars message handlers

void CGEditProDocVars::Serialize(CArchive &ar)
{
	return;
}
