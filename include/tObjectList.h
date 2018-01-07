#if !defined(AFX_TOBJECTLIST_H__22EDCA62_2B68_11D5_9555_D26355CF6020__INCLUDED_)
#define AFX_TOBJECTLIST_H__22EDCA62_2B68_11D5_9555_D26355CF6020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// tObjectList.h : header file
// For tLife ver. 0.3 and tStudio
// Created on: 05/10/2002
// Author:  Tom Morris
// Company: TrilobiteWorks
// http://planetarybiology.com/trilobiteworks
//
// This file is (c) 2002 TrilobiteWorks. All rights reserved.
//////////////////////////////////////////////////////////////////////////////

//	***	tPLUGIN SUPPORT	***
//	***	THIS FILE IS NOT MEANT TO BE CUSTOMIZED	***
//	*** DO NOT MODIFY UNLESS ABSOLUTELY NECESSARY	***

template <class TYPE, class ARG_TYPE>
class CtObjectList : public CList<TYPE, ARG_TYPE>
{

//	DECLARE_SERIAL (CtObjectList)
// Construction
public:

	CtObjectList(const CtObjectList &temp) { (*this) = temp; }
	CtObjectList<TYPE, ARG_TYPE>() {};
	CtObjectList<TYPE, ARG_TYPE>& operator = (const CtObjectList &temp);
	CtObjectList<TYPE, ARG_TYPE>& operator == (const CtObjectList &temp);
 	POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL);
	void Serialize(CArchive &ar);
	virtual ~CtObjectList<TYPE, ARG_TYPE>() {};

// Attributes
public:

private:
	BOOL	bNotEqual;

// Operations
public:



BOOL NotEqual()
{
	return bNotEqual;
}



BOOL   operator!=(const CtObjectList<TYPE, ARG_TYPE> &s)	 //	assignment operator
{

	CtObjectList<TYPE, ARG_TYPE>::bNotEqual = FALSE;

	if (GetCount() != s.GetCount())
	{
		bNotEqual = TRUE;
		return CtObjectList<TYPE, ARG_TYPE>::bNotEqual;
	}

	POSITION pos = s.GetHeadPosition();
	while (pos != NULL)
	{
		TYPE	tempObjectTHIS;
		TYPE	tempObjectS;
		tempObjectTHIS = GetAt(pos);
		tempObjectS = s.GetAt(pos);

		if (&tempObjectTHIS == &tempObjectS)
		{
			((  CtObjectList  )s).GetNext(pos);
		}
		else
		{
			bNotEqual = TRUE;
			return CtObjectList<TYPE, ARG_TYPE>::bNotEqual;
		}
	}
	return CtObjectList<TYPE, ARG_TYPE>::bNotEqual;
}


POSITION	GetHeadPosition() const
{
	return CList<TYPE, ARG_TYPE>::GetHeadPosition();
}


TYPE		GetHead()
{
	return CList<TYPE, ARG_TYPE>::GetHead();
}


POSITION	AddHead(ARG_TYPE newElement)
{
	return CList<TYPE, ARG_TYPE>::AddHead(newElement);
}


POSITION	AddTail(ARG_TYPE newElement)
{
	return CList<TYPE, ARG_TYPE>::AddTail(newElement);
}


POSITION	InsertAfter(POSITION position, ARG_TYPE newElement)
{
	return CList<TYPE, ARG_TYPE>::InsertAfter(position, newElement);
}

POSITION	InsertBefore(POSITION position, ARG_TYPE newElement)
{
	return CList<TYPE, ARG_TYPE>::InsertBefore(position, newElement);
}


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CtObjectList)
	//}}AFX_VIRTUAL

// Implementation
public:
//	virtual ~CtObjectList();


	// Generated message map functions
protected:
	//{{AFX_MSG(CtObjectList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	//	DECLARE_MESSAGE_MAP()

};


template<class TYPE, class ARG_TYPE>
inline 	CtObjectList<TYPE, ARG_TYPE>& CtObjectList<TYPE, ARG_TYPE>::operator=(const CtObjectList<TYPE, ARG_TYPE> &temp)
{
	ASSERT_VALID(this);
	if (this == &temp)
		return *this;

	RemoveAll();

	POSITION pos = temp.GetHeadPosition();

	while (pos != NULL)
	{
		TYPE	tempObject;

		tempObject = temp.GetNext(pos);

		AddTail(tempObject);
	}
	return *this;
}


template<class TYPE, class ARG_TYPE>
inline 	POSITION CtObjectList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, POSITION startAfter)
{
	ASSERT_VALID(this);
	POSITION pos = GetHeadPosition();

	pos	= Find(searchValue);
	return pos;
}

template<class TYPE, class ARG_TYPE>
inline void CtObjectList<TYPE, ARG_TYPE>::Serialize(CArchive &ar)
{
	ASSERT_VALID(this);

	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.WriteCount(m_nCount);
		for (CNode* pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		{
			ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));
			pNode->data.Serialize(ar);
//			SerializeElements<TYPE>(ar, &pNode->data, 1);
		}
	}
	else
	{
		DWORD nNewCount = ar.ReadCount();
		while (nNewCount--)
		{
			TYPE	newData;
			newData.Serialize(ar);
//			SerializeElements<TYPE>(ar, &newData, 1);
			AddTail(newData);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOBJECTLIST_H__22EDCA62_2B68_11D5_9555_D26355CF6020__INCLUDED_)
