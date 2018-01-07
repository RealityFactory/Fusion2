/****************************************************************************************/
/*  facelist.cpp                                                                        */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird                                                */
/*  Description:  Face list management, io, etc...                                      */
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
/*  Genesis3D Version 1.1 released November 15, 1999                                    */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved                            */
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002                          */
/****************************************************************************************/
#include "facelist.h"
#include <assert.h>
#include "typeio.h"
#include "ram.h"
#include <memory.h>
// changed QD 12/03
#include <string.h>
#include <malloc.h>
#include <stdafx.h>
#include "Globals.h"	//for conprintf
// end change

struct tag_FaceList
{
	int NumFaces;
	int Limit;
	Face **Faces;
	geBoolean Dirty;
	Box3d Bounds;
};

FaceList	*FaceList_Create(int NumFaces)
{
	FaceList *pList = NULL;

	assert (NumFaces > 0);

	// allocate the structure
	pList = (FaceList *)geRam_Allocate(sizeof (FaceList));
	if (pList != NULL)
	{
		pList->NumFaces = 0;
		pList->Limit = NumFaces;
		// allocate space for NumFaces pointers
		pList->Faces = (struct FaceTag **)geRam_Allocate(NumFaces * sizeof (Face *));
		if (pList->Faces != NULL)
		{
			int i;
			// set the pointers to NULL
			for (i = 0; i < pList->Limit; i++)
			{
				pList->Faces[i] = NULL;
			}
		}
		pList->Dirty = GE_TRUE;
	}

	return pList;
}

void	FaceList_Destroy(FaceList **ppList)
{
	int i;
	FaceList *pList;

	assert (ppList != NULL);
	assert (*ppList != NULL);

	pList = *ppList;
	for (i = 0; i < pList->NumFaces; i++)
	{
		Face_Destroy (&pList->Faces[i]);
	}
	geRam_Free (pList->Faces);
	geRam_Free (*ppList);
	*ppList = NULL;
}

// Grows the face list by doubling its size.
geBoolean FaceList_Grow (FaceList **ppList)
{
	FaceList *pList;
	FaceList *NewList;

	assert (ppList != NULL);
	assert (*ppList != NULL);

	pList = *ppList;
	// create the new list
	NewList = FaceList_Create (2 * pList->Limit);
	if (NewList == NULL)
	{
		return GE_FALSE;
	}

	// copy old list into new list
	NewList->NumFaces = pList->NumFaces;
	memcpy (NewList->Faces, pList->Faces, (pList->NumFaces * sizeof (Face *)));
	NewList->Dirty = GE_TRUE;

	// get rid of the old list, but not the pointers...
	geRam_Free ((*ppList)->Faces);
	geRam_Free (*ppList);

	// and set the new list pointer.
	*ppList= NewList;

	return GE_TRUE;
}

FaceList	*FaceList_Clone(const FaceList *pList)
{
	FaceList	*cpList;
	Face		*cpf;
	int			i;
	const Face	*f;

	assert (pList != NULL);
	assert (pList->NumFaces <= pList->Limit);

	// allocate the structure
	cpList	=FaceList_Create(pList->NumFaces);

	if(cpList)
	{
		for(i=0;i < pList->NumFaces;i++)
		{
			f	=FaceList_GetFace(pList, i);
			cpf	=Face_Clone(f);
			FaceList_AddFace(cpList, cpf);
		}
	}
	return	cpList;
}

void	FaceList_AddFace(FaceList *pList, Face *pFace)
{
	assert (pList != NULL);
	assert (pFace != NULL);
	assert (pList->NumFaces < pList->Limit);

	// here we could grow the list if we want...

	pList->Faces[pList->NumFaces] = pFace;
	++(pList->NumFaces);
	pList->Dirty = GE_TRUE;
}

void	FaceList_SetDirty(FaceList *pList)
{
	assert (pList != NULL);
	assert (pList->NumFaces <= pList->Limit);

	pList->Dirty = GE_TRUE;
}

//frees the affected face
void	FaceList_RemoveFace(FaceList *pList, int WhichFace)
{
	int	i;

	assert (pList != NULL);
	assert (pList->NumFaces <= pList->Limit);
	assert (pList->Faces[WhichFace] !=NULL);

	Face_Destroy(&pList->Faces[WhichFace]);

	for(i=WhichFace;i < pList->NumFaces-1;i++)
	{
		pList->Faces[i]	=pList->Faces[i+1];
	}
	pList->Faces[--pList->NumFaces]= NULL;

	pList->Dirty = GE_TRUE;
}

int	FaceList_GetNumFaces(const FaceList *pList)
{
	assert (pList != NULL);
	return pList->NumFaces;
}

int	FaceList_GetFaceLimit (const FaceList *pList)
{
	assert (pList != NULL);

	return pList->Limit;
}


Face	*FaceList_GetFace(const FaceList *pList, int WhichFace)
{
	assert (pList != NULL);
	assert (WhichFace < pList->NumFaces);

	return	pList->Faces[WhichFace];
}


void	FaceList_Rotate(FaceList *pList, const geXForm3d *pXfm, const geVec3d *pCenter)
{
	int i;

	assert (pList != NULL);
	assert (pXfm != NULL);
	assert (pCenter != NULL);

	for (i = 0; i < pList->NumFaces; i++)
	{
		Face_Rotate(pList->Faces[i], pXfm, pCenter);
	}
	pList->Dirty	=GE_TRUE;
}

void	FaceList_Move(FaceList *pList, const geVec3d *trans)
{
	int i;

	assert (pList != NULL);
	assert (trans != NULL);

	for (i = 0; i < pList->NumFaces; i++)
	{
		Face_Move(pList->Faces[i], trans);
	}
	pList->Dirty	=GE_TRUE;
}

void	FaceList_Transform(FaceList *pList, const geXForm3d *pXfm)
{
	int i;

	assert (pList != NULL);
	assert (pXfm != NULL);

	for (i = 0; i < pList->NumFaces; i++)
	{
		Face_Transform(pList->Faces[i], pXfm);
	}
	pList->Dirty	=GE_TRUE;
}

void	FaceList_Scale(FaceList *pList, const geVec3d *ScaleVec)
{
	int i;

	assert(pList != NULL);
	assert(ScaleVec != NULL);

	for(i=0;i < pList->NumFaces;i++)
	{
		Face_Scale(pList->Faces[i], ScaleVec);
	}
	pList->Dirty	=GE_TRUE;
}

void	FaceList_Shear(FaceList *pList, const geVec3d *ShearVec, const geVec3d *ShearAxis)
{
	int i;

	assert(pList != NULL);
	assert(ShearVec != NULL);
	assert(ShearAxis != NULL);

	for(i=0;i < pList->NumFaces;i++)
	{
		Face_Shear(pList->Faces[i], ShearVec, ShearAxis);
	}
	pList->Dirty	=GE_TRUE;
}

void	FaceList_GetCenter(const FaceList *pList, geVec3d *pCenter)
{
	Box3d	Bounds;

	assert (pList != NULL);
	assert (pCenter != NULL);

	FaceList_GetBounds(pList, &Bounds);
	Box3d_GetCenter(&Bounds, pCenter);
}

void	FaceList_GetBounds(const FaceList *pList, Box3d *pBounds)
{
	int i;

	assert (pList != NULL);
	assert (pBounds != NULL);
	assert (pList->NumFaces > 0);

	if(pList->Dirty)
	{
		Box3d	Bounds;

		Face_GetBounds(pList->Faces[0], &Bounds);
		for (i = 1; i < pList->NumFaces; i++)
		{
			Box3d FaceBounds;

			Face_GetBounds(pList->Faces[i], &FaceBounds);
			Box3d_Union(&Bounds, &FaceBounds, &Bounds);
		}
		// The casts are necessary because pList is const
		// The idea here is that Bounds and Dirty are implementation
		// details rather than interface data.  So it's OK to change
		// them here.  Like C++'s mutable keyword.
		((FaceList *)pList)->Bounds = Bounds;
		((FaceList *)pList)->Dirty = GE_FALSE;
	}
	*pBounds = pList->Bounds;
}

geBoolean	FaceList_Write(const FaceList *pList, FILE *f)
{
	int i;

	assert (pList != NULL);
	assert (f != NULL);

	if (fprintf(f, "\tBrushFaces %d\n",pList->NumFaces) < 0) return GE_FALSE;
	for(i=0;i < pList->NumFaces; i++)
	{
		if (!Face_Write(pList->Faces[i], f)) return GE_FALSE;
	}
	return GE_TRUE;
}

// changed QD 11/03
geBoolean FaceList_ExportTo3dtv1_32(const FaceList *pList, FILE *f)
{
	int i;

	assert (pList != NULL);
	assert (f != NULL);

	if (fprintf(f, "\tBrushFaces %d\n",pList->NumFaces) < 0) return GE_FALSE;
	for(i=0;i < pList->NumFaces; i++)
	{
		if (!Face_ExportTo3dtv1_32(pList->Faces[i], f)) return GE_FALSE;
	}
	return GE_TRUE;
}

//changed QD 12/03
geBoolean	FaceList_GetUsedTextures(const FaceList *pList, geBoolean *WrittenTex, CWadFile * WadFile)
{
	int i, index;
	for(i=0;i < pList->NumFaces; i++)
	{
		index = Face_GetTextureDibId(pList->Faces[i]);

		if(index<WadFile->mBitmapCount)
			WrittenTex[index]=GE_TRUE;
		else
		{
			CGlobals::GetActiveDocument()->m_pMainFrame->ConPrintf("Could not find texture '%s' in texture library.\n", Face_GetTextureName(pList->Faces[i]));
			CGlobals::GetActiveDocument()->m_pMainFrame->ConPrintf("Applying default texture '%s'\n", WadFile->mBitmaps[0].Name);
			WrittenTex[0]=GE_TRUE;
			Face_SetTextureDibId (pList->Faces[i], 0);
			Face_SetTextureName(pList->Faces[i], WadFile->mBitmaps[0].Name);
			Face_SetTextureSize (pList->Faces[i], WadFile->mBitmaps[0].Width, WadFile->mBitmaps[0].Height);
		}
	}

	return GE_TRUE;
}

// 3DS chunks
//#define CHUNK_MAIN3DS		0x4d4d
//#define CHUNK_EDIT3DS		0x3d3d
//#define CHUNK_COLORRGB    0x0011
//#define CHUNK_PERCENT		0x0030
//#define MASTER_SCALE		0x0100
// Object block
#define CHUNK_OBJBLOCK		0x4000
#define CHUNK_TRIMESH		0x4100
#define CHUNK_VERTLIST		0x4110
#define CHUNK_FACELIST		0x4120
#define CHUNK_MATLIST		0x4130
#define CHUNK_MAPLIST		0x4140 // uv coordinates
/* Material block
#define CHUNK_MATBLOCK		0xAFFF
#define CHUNK_MATNAME		0xA000
#define CHUNK_MATAMB		0xA010 // Ambient color
#define CHUNK_MATDIFF		0xA020 // Diffuse color
#define CHUNK_MATSPEC		0xA030 // Specular color
#define CHUNK_MATSHININESS	0xA040
#define CHUNK_MATSHIN2PCT	0xA041
#define CHUNK_MATTRANS		0xA050
#define CHUNK_MATXPFALL		0xA052
#define CHUNK_MATREFBLUR	0xA053
#define CHUNK_MATSHADING	0xA100
#define CHUNK_MATDECAL		0xA084
#define CHUNK_MATWIRESIZE	0xA087
#define CHUNK_MAP			0xA200
#define CHUNK_MAPNAME		0xA300 // name of bitmap
#define CHUNK_MAPTILING		0xA351
*/

#define SIZE_CHUNKID		sizeof(unsigned short)
#define SIZE_CHUNKLENGTH	sizeof(long)
#define SIZE_USHORT			sizeof(unsigned short)
#define SIZE_FLOAT			sizeof(float)

static void write_ushort(FILE *file,unsigned short i)
{
	fwrite(&i,1,sizeof(unsigned short),file);
}

static void write_int(FILE *file,int i)
{
	fwrite(&i,1,sizeof(int),file);
}

static void write_float(FILE *file,float i)
{
	fwrite(&i,1,sizeof(float),file);
}

static void write_char(FILE *file,char i)
{
	fwrite(&i,1,sizeof(char),file);
}

geBoolean	FaceList_ExportTo3ds(const FaceList *pList, FILE *f, int BrushCount, int SubBrushCount)
{
	int i, j, k, num_faces, num_verts, num_mats, num_chars, curnum_verts;
	int size_verts, size_faces, size_trimesh, size_objblock, size_name, size_mapuv, size_mats;
	char matname[9];

	char *matf=(char *)calloc(sizeof(char), pList->NumFaces);

	assert (pList != NULL);
	assert (f != NULL);

	num_faces = num_verts = num_mats = num_chars =0;
	// get the total number of verts, faces and materials of the object
	for(i=0;i < pList->NumFaces; i++)
	{
		curnum_verts=Face_GetNumPoints(pList->Faces[i]);
		num_faces+=(curnum_verts-2);
		num_verts+=curnum_verts;

		if(!matf[i])
		{
			matf[i]=1;
			num_mats++;

			for(j=i+1; j<pList->NumFaces; j++)
			{
				if(strcmp(Face_GetTextureName(pList->Faces[i]), Face_GetTextureName(pList->Faces[j]))==0)
					matf[j]=1;
			}

			strncpy (matname, Face_GetTextureName(pList->Faces[i]), 9);
			matname[8] = '\0';
			// get the number of characters for calculating size_mats
			for(j=0;matname[j]!='\0';j++,num_chars++);
		}
	}

	for(i=0;i<pList->NumFaces;i++)
		matf[i]=0;

	// calculate the size of the different chunks
	size_name		= 7; //xxx_xx\0
	size_verts		= SIZE_CHUNKID+SIZE_CHUNKLENGTH+SIZE_USHORT+3*SIZE_FLOAT*num_verts;
	size_mats		= (SIZE_CHUNKID+SIZE_CHUNKLENGTH+SIZE_USHORT)*num_mats+(num_chars+num_mats)+SIZE_USHORT*num_faces;
	size_faces		= SIZE_CHUNKID+SIZE_CHUNKLENGTH+SIZE_USHORT+4*SIZE_USHORT*num_faces+size_mats;
	size_mapuv		= SIZE_CHUNKID+SIZE_CHUNKLENGTH+SIZE_USHORT+2*SIZE_FLOAT*num_verts;
	size_trimesh	= SIZE_CHUNKID+SIZE_CHUNKLENGTH+size_verts+size_faces+size_mapuv;
	size_objblock	= SIZE_CHUNKID+SIZE_CHUNKLENGTH+size_name+size_trimesh;


	// write the objblock
	write_ushort(f,CHUNK_OBJBLOCK);
	write_int(f,size_objblock);
	// give each object a unique name xxx_xx\0
	write_char(f,(char)(48+(BrushCount-BrushCount%100)/100));
	write_char(f,(char)(48+((BrushCount-BrushCount%10)/10)%10));
	write_char(f,(char)(48+BrushCount%10));
	write_char(f,'_');
	write_char(f,(char)(48+(SubBrushCount-SubBrushCount%10)/10));
	write_char(f,(char)(48+SubBrushCount%10));
	write_char(f,'\0');
	// end name of this object

	// this object is a trimesh
	write_ushort(f,CHUNK_TRIMESH);
	write_int(f,size_trimesh);

	// write all vertices of each face of this object
	write_ushort(f,CHUNK_VERTLIST);
    write_int(f,size_verts);
	write_ushort(f, (unsigned short)num_verts);
	for(i=0;i<pList->NumFaces;i++)
	{
		const geVec3d	*verts;
		verts=Face_GetPoints(pList->Faces[i]);
		curnum_verts=Face_GetNumPoints(pList->Faces[i]);
		for(j=0;j<curnum_verts;j++)
		{
			write_float(f, verts[j].X);
			write_float(f, verts[j].Y);
			write_float(f, verts[j].Z);
		}
	}

	// write MAPPING COORDINATES
	/*
	Although from the chunk id you would suppose that FACELIST (0x4120) and
	CHUNK_MATLIST (0x4130) would be the next chunks, 3ds max does not recognize the
	mapping coordinates if they are not after the vertlist chunk!
	*/
    write_ushort(f,CHUNK_MAPLIST);
    write_int(f,size_mapuv);
	write_ushort(f, (unsigned short)num_verts);
	for(i=0;i<pList->NumFaces;i++)
	{
		const TexInfo_Vectors *TVecs = Face_GetTextureVecs (pList->Faces[i]);
		const geVec3d	*verts;
		geVec3d uVec, vVec;
		geFloat U, V;

		int txSize, tySize;

		Face_GetTextureSize(pList->Faces[i], &txSize, &tySize);

		// make sure that the texture size is set correctly (division!)
		if(txSize==0)
			txSize=32;
		if(tySize==0)
			tySize=32;


		geVec3d_Scale (&TVecs->uVec, 1.f/(geFloat)txSize, &uVec);
		geVec3d_Scale (&TVecs->vVec, -1.f/(geFloat)tySize, &vVec);

		verts=Face_GetPoints(pList->Faces[i]);
		curnum_verts=Face_GetNumPoints(pList->Faces[i]);

		for(j=0;j<curnum_verts;j++)
		{
			U = geVec3d_DotProduct(&(verts[j]), &uVec);
			V = geVec3d_DotProduct(&(verts[j]), &vVec);
			U+=(TVecs->uOffset/txSize);
			V-=(TVecs->vOffset/tySize);
			write_float(f, U);
			write_float(f, V);
		}
	}

	// write all faces of this object (all faces are split into triangles)
    write_ushort(f,CHUNK_FACELIST);
    write_int(f,size_faces);
	write_ushort(f, (unsigned short)num_faces);
	num_verts=0;
	for(i=0;i<pList->NumFaces;i++)
	{
		curnum_verts=Face_GetNumPoints(pList->Faces[i]);
		for(j=0;j<curnum_verts-2;j++)
		{
			write_ushort(f, (unsigned short)num_verts);
			write_ushort(f, (unsigned short)(num_verts+2+j));
			write_ushort(f, (unsigned short)(num_verts+1+j));
			write_ushort(f, 6);
		}
		num_verts+=curnum_verts;
	}

	// write MATERIALS
	for(i=0;i<pList->NumFaces;i++)
	{
		if(!matf[i])
		{
			matf[i]=1;

			int curnum_faces=(Face_GetNumPoints(pList->Faces[i])-2);

			for(j=i+1; j<pList->NumFaces; j++)
			{
				if(strcmp(Face_GetTextureName(pList->Faces[i]), Face_GetTextureName(pList->Faces[j]))==0)
				{
					curnum_faces+=(Face_GetNumPoints(pList->Faces[j])-2);
				}
			}

			strncpy (matname, Face_GetTextureName(pList->Faces[i]), 9);
			matname[8] = '\0';
			for(num_chars=0;matname[num_chars]!='\0';num_chars++);

			write_ushort(f,CHUNK_MATLIST);
			int size = SIZE_CHUNKID+SIZE_CHUNKLENGTH+(num_chars+1)+SIZE_USHORT+SIZE_USHORT*curnum_faces;
			write_int(f,size);

			// write matname
			for(j=0;j<=num_chars;j++)
				write_char(f, matname[j]);

			// write number of faces
			write_ushort(f, (unsigned short)curnum_faces);

			// write face numbers
			curnum_faces=0;
			for(j=0;j<i;j++)
				curnum_faces+=(Face_GetNumPoints(pList->Faces[j])-2);

			curnum_verts=Face_GetNumPoints(pList->Faces[i]);
			for(j=0; j<curnum_verts-2; j++)
				write_ushort(f, (unsigned short)(curnum_faces+j));


			curnum_faces+=(curnum_verts-2);

			for(j=i+1; j<pList->NumFaces; j++)
			{
				curnum_verts=Face_GetNumPoints(pList->Faces[j]);
				if(strcmp(Face_GetTextureName(pList->Faces[i]), Face_GetTextureName(pList->Faces[j]))==0)
				{
					matf[j]=1;
					for(k=0;k<curnum_verts-2;k++)
						write_ushort(f, (unsigned short)(curnum_faces+k));
				}
				curnum_faces+=(curnum_verts-2);
			}
		}
	}
	free(matf);

	return GE_TRUE;
}
// end change

FaceList	*FaceList_CreateFromFile
	(
	  Parse3dt *Parser,
	  int VersionMajor,
	  int VersionMinor,
	  const char **Expected
	)
{
	int NumFaces;
	FaceList *pList;

	assert (Parser != NULL);

	if (!Parse3dt_GetInt (Parser, (*Expected = "BrushFaces"), &NumFaces)) return NULL;

	pList = FaceList_Create (NumFaces);
	if (pList != NULL)
	{
		int i;
		for (i = 0; i < NumFaces; i++)
		{
			Face *pFace;

			pFace = Face_CreateFromFile (Parser, VersionMajor, VersionMinor, Expected);
			if (pFace != NULL)
			{
				FaceList_AddFace (pList, pFace);
			}
		}
		if(pList->NumFaces < 4)
		{
			FaceList_Destroy (&pList);
			return NULL;
		}
		pList->Dirty = GE_TRUE;
	}
	return pList;
}

geBoolean	FaceList_WriteToMap(const FaceList *pList, FILE *f)
{
	int	i;

	assert (pList != NULL);
	assert (f != NULL);

	TypeIO_WriteInt(f, pList->NumFaces);
	for (i = 0; i < pList->NumFaces; i++)
	{
		Face_WriteToMap(pList->Faces[i], f);
	}
	return	GE_TRUE;
}

geBoolean	FaceList_WriteToQuakeMap(const FaceList *pList, FILE *f)
{
	int	i;

	assert (pList != NULL);
	assert (f != NULL);

	fprintf(f, "{\n ");

	for (i = 0; i < pList->NumFaces; i++)
	{
		Face_WriteToQuakeMap(pList->Faces[i], f);
	}

	fprintf(f, "}\n");
	return	GE_TRUE;
}

static	geFloat		dists[256];
static	uint8		sides[256];

enum SideFlags
{
	SIDE_FRONT	=0,
	SIDE_BACK	=1,
	SIDE_ON		=2,
	SIDE_SPLIT	=3
};

//clips f behind every face in fl
void	FaceList_ClipFaceToList(const FaceList *fl, Face **f)
{
	int			i;
	const Plane	*p;
	uint8		cnt[3];

	assert(fl != NULL);
	assert(f != NULL);
	assert(*f != NULL);

	for(i=0;i < fl->NumFaces;i++)
	{
		p	=Face_GetPlane(fl->Faces[i]);

		Face_GetSplitInfo(*f, p, dists, sides, cnt);
		if(!cnt[SIDE_FRONT] && !cnt[SIDE_BACK])	//coplanar
		{
			Face_Destroy(f);
			return;
		}
		else if(!cnt[SIDE_FRONT])	//back
		{
			continue;
		}
		else if(!cnt[SIDE_BACK])	//front
		{
			Face_Destroy(f);
			return;
		}
		else	//split
		{
			Face_Clip(*f, p, dists, sides);
		}
	}
}

//if it's selected, chances are it will need modification
Face	*FaceList_GetSelectedFace(const FaceList *fl)
{
	int		i;

	assert(fl);

	for(i=0;i < fl->NumFaces;i++)
	{
		if(Face_IsSelected(fl->Faces[i]))
		{
			break;
		}
	}

	if(i < fl->NumFaces)
	{
		return	fl->Faces[i];
	}
	else
	{
		return	NULL;
	}
}

geBoolean	FaceList_SetNextSelectedFace(FaceList *fl)
{
	int		i;

	assert(fl);

	for(i=0;i < fl->NumFaces;i++)
	{
		if(Face_IsSelected(fl->Faces[i]))
		{
			break;
		}
	}

	if(i < fl->NumFaces-1)
	{
		Face_SetSelected(fl->Faces[i], GE_FALSE);
		i++;
		i	%=fl->NumFaces;
		Face_SetSelected(fl->Faces[i], GE_TRUE);
		return	GE_TRUE;
	}
	else if(i < fl->NumFaces)
	{
		Face_SetSelected(fl->Faces[i], GE_FALSE);
		return	GE_FALSE;	//skip to next brush or select first
	}
	else	//if it didn't overflow... there simply wasn't
	{		//anything selected, select the first face
		Face_SetSelected(fl->Faces[0], GE_TRUE);
		return	GE_TRUE;
	}
}

geBoolean	FaceList_SetPrevSelectedFace(FaceList *fl)
{
	int		i;

	assert(fl);

	for(i=0;i < fl->NumFaces;i++)
	{
		if(Face_IsSelected(fl->Faces[i]))
		{
			break;
		}
	}

	if(i >= fl->NumFaces)	//if it didn't underflow... there simply wasn't
	{						//anything selected, select the last face
		Face_SetSelected(fl->Faces[fl->NumFaces-1], GE_TRUE);
		return	GE_TRUE;
	}
	else if(i==0)
	{
		Face_SetSelected(fl->Faces[i], GE_FALSE);
		return	GE_FALSE;	//skip to prev brush or select last
	}
	else
	{
		Face_SetSelected(fl->Faces[i], GE_FALSE);
		i--;
		Face_SetSelected(fl->Faces[i], GE_TRUE);
		return	GE_TRUE;
	}
}

void	FaceList_CopyFaceInfo(const FaceList *src, FaceList *dst)
{
	int i;

	assert(src);
	assert(dst);

	if(src->NumFaces != dst->NumFaces)
	{
		return;
	}

	for(i=0;i < src->NumFaces;i++)
	{
		Face_CopyFaceInfo(src->Faces[i], dst->Faces[i]);
	}
	dst->Dirty	=GE_TRUE;
}

//Drops trans values into every face
void	FaceList_SetTranslucency(const FaceList *fl, geFloat trans)
{
	int			i;

	assert(fl != NULL);

	for(i=0;i < fl->NumFaces;i++)
	{
		Face_SetTranslucency(fl->Faces[i], trans);
	}
}

// sets every face to translucent
void		FaceList_SetTransparent (const FaceList *fl, geBoolean trans)
{
	int			i;

	assert(fl != NULL);

	for(i=0;i < fl->NumFaces;i++)
	{
		Face_SetTransparent (fl->Faces[i], trans);
	}
}

