#pragma once

class CGEditProDoc;

#include "GEditPro.h"
#include "GEditProDoc.h"
#include "GEditProDocVars.h"
#include "../include/tObjectList.h"

class CGEditProUpdateMgr
{
public:
	CGEditProUpdateMgr(void);
	virtual ~CGEditProUpdateMgr(void);

// Operations
public:

	CGEditProDocVars		DocVarsInitNew(CGEditProDocVars newDocVars);
	CGEditProDocVars&		DocVarsInitOpen(CGEditProDocVars *pDocVars);

	CGEditProDocVars&		DocVarsUpdate(CGEditProDocVars	DocVars);
	
	CGEditProDocVars		DocVarsUndo();
	CGEditProDocVars		DocVarsRedo();

	bool					UpdateLevelInfoInAll(CtLevel *pLevel);

	POSITION			m_posCurrentPosition;
// Implementation
public:
	bool UpdateUndoRedoAvailability();
// Members
public:

	CtObjectList<CGEditProDocVars, CGEditProDocVars&> 	m_lDocVarsList;

private:

#define 	MAX_DOC_VARS  10

	CGEditProDocVars		m_DocVars;

	CGEditProDocVars		m_CurrentDocVars;

};
