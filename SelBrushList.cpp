/****************************************************************************************/
/*  SelBrushList.c                                                                      */
/*                                                                                      */
/*  Author:       Jim Mischel                                                           */
/*  Description:  Selected brush list stuff                                             */
/*                                                                                      */
/*  The contents of this file are subject to the Genesis3D Public License               */
/*  Version 1.01 (the "License"); you may not use this file except in                   */
/*  compliance with the License. You may obtain a copy of the License at                */
/*  http://www.genesis3d.com                                                            */
/*                                                                                      */
/*  Software distributed under the License is distributed on an "AS IS"                 */
/*  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See                */
/*  the License for the specific language governing rights and limitations              */
/*  under the License.                                                                  */
/*                                                                                      */
/*  The Original Code is Genesis3D, released March 25, 1999.                            */
/*Genesis3D Version 1.1 released November 15, 1999                            */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/

#include "stdafx.h"
#include "SelBrushList.h"

#include "array.h"


#include "ram.h"
#include <assert.h>
#include "Globals.h"


struct tag_SelBrushList
{
	Array *pItems;
	int FirstFree;
};


SelBrushList *SelBrushList_Create (void)
{
	SelBrushList *pList = NULL;
	
	pList = GE_RAM_ALLOCATE_STRUCT (SelBrushList);
	if (pList != NULL)
	{
		pList->pItems = Array_Create (10, sizeof (Brush *));
		if (pList->pItems != NULL)
		{
			pList->FirstFree = NULL;
		}
		else
		{
			SelBrushList_Destroy (&pList);
		}
	}
	return pList;
}

void SelBrushList_Destroy (SelBrushList **ppList)
{
	SelBrushList *pList;

	assert (ppList != NULL);
	assert (*ppList != NULL);
	pList = *ppList;

	if (pList->pItems != NULL)
	{
		Array_Destroy (&pList->pItems);
	}
	geRam_Free (*ppList);
}




SelBrushList	*SelBrushList_Clone(SelBrushList *inList)
{
	SelBrushList	*pOutList = NULL;
	Brush		*b = NULL, *b2 = NULL;
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	//assert(inList != NULL);
	if (inList)
	{
		pOutList = SelBrushList_Create();

		if (pOutList)
		{
			int i;
			int iInListSize = SelBrushList_GetSize(inList);
			for (i = 0; i < iInListSize; i++ )
			{
				Brush* pTempInBrush = NULL;
				pTempInBrush = SelBrushList_GetBrush(inList, i);
				if (pTempInBrush)
				{
					b2 = NULL;
					b2	= Brush_Clone(pTempInBrush);
					if (b2)
						SelBrushList_Add(pOutList, b2);

					if (pDoc)
					{
						pDoc->m_pLevelMgr->RemoveBrush (&pDoc->DocVarsGetPointer()->m_Level, pTempInBrush);
					}
				}
			}
		}
	}

	return	pOutList;
}






geBoolean SelBrushList_Find (SelBrushList *pList, const Brush *pBrush)
{
	int i;

	// go through list to see if this Brush is already in the list
	for (i = 0; i < pList->FirstFree; ++i)
	{
		Brush *pRet;

		pRet= SelBrushList_GetBrush (pList, i);
		if (pRet == pBrush)
		{
			// Brush already in list
			return GE_TRUE;
		}
	}
	return GE_FALSE;
}

geBoolean SelBrushList_Add (SelBrushList *pList, Brush *pBrush)
{
	int Size;

	if (SelBrushList_Find (pList, pBrush))
	{
		return GE_FALSE;
	}

	Size = Array_GetSize (pList->pItems);
	assert (pList->FirstFree <= Size);

	// Brush isn't already in list.  Put it at the end...
	if (pList->FirstFree == Size)
	{
		int NewSize;
		// Need to allocate more space
		NewSize = Array_Resize (pList->pItems, 2*Size);
		if (NewSize == Size)
		{
			// couldn't resize.  Guess I can't add the Brush
			return GE_FALSE;
		}
	}
	Array_PutAt (pList->pItems, pList->FirstFree, &pBrush, sizeof (pBrush));
	++(pList->FirstFree);

	return GE_TRUE;
}

geBoolean SelBrushList_Remove (SelBrushList *pList, Brush *pBrush)
{
	int i;

	// find the item in the list
	for (i = 0; i < pList->FirstFree; ++i)
	{
		Brush *pRet;

		pRet = SelBrushList_GetBrush (pList, i);
		if (pRet == pBrush)
		{
			Array_DeleteAt (pList->pItems, i);
			--(pList->FirstFree);
			return GE_TRUE;
		}
	}
	return GE_FALSE;	// not found
}

void SelBrushList_RemoveAll (SelBrushList *pList)
{
	pList->FirstFree = 0;
}

int SelBrushList_GetSize (SelBrushList *pList)
{
	if (pList)
	{
		return pList->FirstFree;
	}
	else
		return 0;
}

Brush *SelBrushList_GetBrush (SelBrushList *pList, int BrushIndex)
{
	Brush **ppBrush = NULL;

	if (BrushIndex < pList->FirstFree)
	{
		ppBrush = (Brush **)Array_ItemPtr (pList->pItems, BrushIndex);

		return *ppBrush;
	}
	else
		return NULL;
}

void SelBrushList_Enum (SelBrushList *pList, SelBrushList_Callback Callback, void *lParam)
{
	int i;

	for (i = 0; i < pList->FirstFree; ++i)
	{
		Brush *pBrush;

		pBrush = SelBrushList_GetBrush (pList, i);
		Callback (pBrush, lParam);
	}
}
