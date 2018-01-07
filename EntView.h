/****************************************************************************************/
/*  EntView.h                                                                           */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax                                    */
/*  Description:  Genesis world editor header file                                      */
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
/*  Genesis3D Version 1.1 released November 15, 1999                                 */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/
#ifndef ENTVIEW_H
#define ENTVIEW_H

#include "basetype.h"
#include "parse3dt.h"
#include "EntityTable.h"

#ifdef __cplusplus
	extern "C" {
#endif

struct EntityViewEntry
{
	char *pName;
	geBoolean IsVisible;
};

typedef struct tag_EntityViewList
{
	int nEntries;
	EntityViewEntry pEntries[1];
} EntityViewList;

EntityViewList *EntityViewList_Create
	(
	  const EntityTable *pEntityDefs
	);

void EntityViewList_Destroy
	(
	  EntityViewList **ppList
	);

EntityViewList *EntityViewList_Copy
	(
	  EntityViewList const *pSrc
	);

geBoolean EntityViewList_LoadSettings
	(
	  EntityViewList *pList,
	  Parse3dt *Parser,
	  int VersionMajor,
	  int VersionMinor,
	  const char **Expected
	);

geBoolean EntityViewList_WriteToFile
	(
	  EntityViewList *pList,
	  FILE *f
	);

#ifdef __cplusplus
	}
#endif

#endif
