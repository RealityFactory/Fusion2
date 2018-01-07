// tObjectArray.h: interface for the CtObjectArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOBJECTARRAY_H__597D6560_D7AA_11D5_9556_BD7D240F9723__INCLUDED_)
#define AFX_TOBJECTARRAY_H__597D6560_D7AA_11D5_9556_BD7D240F9723__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


template <class TYPE, class ARG_TYPE>
class CtObjectArray  : public CArray<TYPE, ARG_TYPE>
{
public:
//	CtObjectArray();
//	virtual ~CtObjectArray();



	CtObjectArray(const CtObjectArray &temp) { (*this) = temp; }
	CtObjectArray<TYPE, ARG_TYPE>() {};
	CtObjectArray<TYPE, ARG_TYPE>& operator = (const CtObjectArray &temp);
	CtObjectArray<TYPE, ARG_TYPE>& operator == (const CtObjectArray &temp);

	BOOL operator != (const CtObjectArray &temp);

	CtObjectArray<TYPE, ARG_TYPE>& operator [] (int nIndex ) const;
	POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL);
	void Serialize(CArchive &ar);
	virtual ~CtObjectArray<TYPE, ARG_TYPE>() {};

private:
	BOOL	bNotEqual;

// Operations
public:



BOOL NotEqual()
{
	return bNotEqual;
}







TYPE	GetAt(int iIndex) const
{
	return CArray<TYPE, ARG_TYPE>::GetAt(iIndex);
}




void	SetAt(int iIndex, ARG_TYPE newElement)
{
	CArray<TYPE, ARG_TYPE>::SetAt(iIndex, newElement);
}



int Add(ARG_TYPE newElement)
{
	return CArray<TYPE, ARG_TYPE>::Add(newElement);
}



void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1)
{
	CArray<TYPE, ARG_TYPE>::InsertAt(nIndex, newElement, nCount);
}



void RemoveAt( int nIndex, int nCount = 1)
{
	CArray<TYPE, ARG_TYPE>::RemoveAt(nIndex, newElement, nCount);
}

};

#endif // !defined(AFX_TOBJECTARRAY_H__597D6560_D7AA_11D5_9556_BD7D240F9723__INCLUDED_)



template<class TYPE, class ARG_TYPE>
inline 	CtObjectArray<TYPE, ARG_TYPE>& CtObjectArray<TYPE, ARG_TYPE>::operator=(const CtObjectArray<TYPE, ARG_TYPE> &temp)
{
	ASSERT_VALID(this);
	if (this == &temp)
		return *this;

	RemoveAll();

	int iElements = temp.GetSize();

	for (int i = 0; i < iElements; i++)
	{
		TYPE	tempObject;

		tempObject = temp.GetAt(i);

		Add(tempObject);
	}
	return *this;
}


template<class TYPE, class ARG_TYPE>
inline   BOOL CtObjectArray<TYPE, ARG_TYPE>::operator!=(const CtObjectArray<TYPE, ARG_TYPE> &s) 	 //	assignment operator
{
	BOOL	bNotEqual;

	bNotEqual = FALSE;

	if (GetSize() != s.GetSize())
	{
		bNotEqual = TRUE;
		return bNotEqual;
	}

	for (int i = 0; i < GetSize();)
	{
		TYPE	tempObjectTHIS;
		TYPE	tempObjectS;
		tempObjectTHIS = GetAt(i);
		tempObjectS = s.GetAt(i);

		if (&tempObjectTHIS == &tempObjectS)
		{
			i++;
		}
		else
		{
			bNotEqual = TRUE;
			return bNotEqual;
		}
	}
	return bNotEqual;
}






template<class TYPE, class ARG_TYPE>
inline void CtObjectArray<TYPE, ARG_TYPE>::Serialize(CArchive &ar)
{
	ASSERT_VALID(this);

//	CObject::Serialize(ar);
	CArray<TYPE, ARG_TYPE>::Serialize(ar);

/*
	if (ar.IsStoring())
	{
		ar.WriteCount(m_nSize);
		for (int i = 0; i < m_nSize; i++)
		{
			ASSERT(AfxIsValidAddress(GetAt(i), sizeof(GetAt(i))));
//			pNode->data->Serialize(ar);
			SerializeElements<TYPE>(ar, m_pData, 1);
		}
	}
	else
	{
		DWORD nNewCount = ar.ReadCount();
		while (nNewCount--)
		{
			TYPE	newData;
//			newData.Serialize(ar);
			SerializeElements<TYPE>(ar, &newData, 1);
			Add(newData);
		}
	}
*/

}
