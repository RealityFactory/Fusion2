#include "stdafx.h"
#include "Globals.h"
#include "GEditProDoc.h"

CGEditProUpdateMgr::CGEditProUpdateMgr(void)
{
	m_CurrentDocVars = m_DocVars;
	m_posCurrentPosition = NULL;
}

CGEditProUpdateMgr::~CGEditProUpdateMgr(void)
{
}



/////////////////////////////////////////////////////////////////////////////
//	DocVarsInitNew
//	
/////////////////////////////////////////////////////////////////////////////
CGEditProDocVars		CGEditProUpdateMgr::DocVarsInitNew(CGEditProDocVars newDocVars)
{
	m_CurrentDocVars = newDocVars;
	m_lDocVarsList.AddTail(m_CurrentDocVars);

	m_posCurrentPosition = m_lDocVarsList.GetTailPosition();
	return m_CurrentDocVars;
}


/////////////////////////////////////////////////////////////////////////////
//	DocVarsInitOpen
//	
/////////////////////////////////////////////////////////////////////////////
CGEditProDocVars&		CGEditProUpdateMgr::DocVarsInitOpen(CGEditProDocVars *pDocVars)
{
	m_posCurrentPosition = m_lDocVarsList.GetTailPosition();
	m_CurrentDocVars = m_lDocVarsList.GetAt(m_posCurrentPosition);

	return m_CurrentDocVars;
}




/////////////////////////////////////////////////////////////////////////////
//	DocVarsUpdate
//	
/////////////////////////////////////////////////////////////////////////////
CGEditProDocVars&	CGEditProUpdateMgr::DocVarsUpdate(CGEditProDocVars	DocVars)
{
	if (m_posCurrentPosition)
	{
		m_posCurrentPosition = m_lDocVarsList.InsertAfter(m_posCurrentPosition, m_CurrentDocVars);
		m_lDocVarsList.GetAt(m_posCurrentPosition) = DocVars;
		m_CurrentDocVars = m_lDocVarsList.GetAt(m_posCurrentPosition);

		//	if we are updating while in the middle of the list, we have to 
		//	remove all trailing items in the list. Otherwise a Redo request
		//	will "advance" us to a previous state... CONFUSING!	
		//	Therefore, all edits and updates from mid-list will force
		//	a tail-trimming and loss of access to trimmed list items
		POSITION tempPos = m_lDocVarsList.GetTailPosition();
		while (tempPos != m_posCurrentPosition)
		{
			m_lDocVarsList.RemoveTail();
			tempPos = m_lDocVarsList.GetTailPosition();
		}

		//	trim the list if we have exceeded the Max undo count
		int iCount = m_lDocVarsList.GetCount();
		if (iCount > CGlobals::g_iUndoMAX)
		{
			m_lDocVarsList.RemoveHead();
		}

		UpdateUndoRedoAvailability();
		return m_CurrentDocVars;
	}
	else if (!m_posCurrentPosition)
	{
		m_posCurrentPosition = m_lDocVarsList.AddTail(m_CurrentDocVars);
		m_lDocVarsList.GetTail() = DocVars;
		m_CurrentDocVars = m_lDocVarsList.GetTail();
		UpdateUndoRedoAvailability();
		return m_CurrentDocVars;
	}

	UpdateUndoRedoAvailability();
	return  m_CurrentDocVars;
}





/////////////////////////////////////////////////////////////////////////////
//	DocVarsUndo
//	
/////////////////////////////////////////////////////////////////////////////
CGEditProDocVars		CGEditProUpdateMgr::DocVarsUndo()
{
	if (m_posCurrentPosition != m_lDocVarsList.GetHeadPosition())
	{
		m_lDocVarsList.GetPrev(m_posCurrentPosition);
		m_CurrentDocVars = m_lDocVarsList.GetAt(m_posCurrentPosition);
		UpdateUndoRedoAvailability();
		return m_CurrentDocVars;
	}
	else if (m_posCurrentPosition == m_lDocVarsList.GetHeadPosition())
	{
		m_CurrentDocVars = m_lDocVarsList.GetHead();
	}
	UpdateUndoRedoAvailability();
	return m_CurrentDocVars;
}



/////////////////////////////////////////////////////////////////////////////
//	DocVarsRedo
//	
/////////////////////////////////////////////////////////////////////////////
CGEditProDocVars		CGEditProUpdateMgr::DocVarsRedo()
{
	if (m_posCurrentPosition != m_lDocVarsList.GetTailPosition())
	{
		m_lDocVarsList.GetNext(m_posCurrentPosition);
		m_CurrentDocVars = m_lDocVarsList.GetAt(m_posCurrentPosition);
		UpdateUndoRedoAvailability();
		return m_CurrentDocVars;
	}

	else if (m_posCurrentPosition == m_lDocVarsList.GetTailPosition())
	{
		m_CurrentDocVars = m_lDocVarsList.GetTail();
	}
	UpdateUndoRedoAvailability();
	return m_CurrentDocVars;
}




bool CGEditProUpdateMgr::UpdateUndoRedoAvailability()
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	if (pDoc)
	{
		POSITION	tempHeadPOS = m_lDocVarsList.GetHeadPosition();
		//	this code seeks out the first LEGITIMATE DocVars position
		POSITION	tempHeadNextPOS = m_lDocVarsList.GetHeadPosition();
		m_lDocVarsList.GetNext(tempHeadNextPOS);

		if ((m_lDocVarsList.GetCount() > 1) && (m_posCurrentPosition != m_lDocVarsList.GetTailPosition()))
			pDoc->SetRedoAvailability(true);
		else
			pDoc->SetRedoAvailability(false);

		if ((m_lDocVarsList.GetCount() > 1) && (m_posCurrentPosition != tempHeadPOS
												&& m_posCurrentPosition != tempHeadNextPOS))
			pDoc->SetUndoAvailability(true);
		else
			pDoc->SetUndoAvailability(false);

		return true;
	}
	return false;
}


bool	CGEditProUpdateMgr::UpdateLevelInfoInAll(CtLevel *pLevel)
{
	POSITION	POS = NULL;
	POS = m_lDocVarsList.GetHeadPosition();
	while (POS)
	{
		CGEditProDocVars	tempDocVars;
		tempDocVars = m_lDocVarsList.GetAt(POS);
		tempDocVars.m_Level.m_pWadFile = pLevel->m_pWadFile;
		tempDocVars.m_Level.m_pWadPath = pLevel->m_pWadPath;
		tempDocVars.m_Level.m_pWadSizeInfos = pLevel->m_pWadSizeInfos;
		tempDocVars.m_Level.m_pEntityDefs = pLevel->m_pEntityDefs;
		tempDocVars.m_Level.m_pHeadersDir = pLevel->m_pHeadersDir;
// changed QD
		tempDocVars.m_Level.m_pActorsDir = pLevel->m_pActorsDir;
		tempDocVars.m_Level.m_pPawnIniPath = pLevel->m_pPawnIniPath;
// end change
		m_lDocVarsList.SetAt(POS, tempDocVars);
		m_lDocVarsList.GetNext(POS);
	}

	return true;
}







