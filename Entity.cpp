/****************************************************************************************/
/*  Entity.cpp                                                                          */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax, Bruce Cooner                      */
/*  Description:  Entity code                                                           */
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
/****************************************************************************************/
#include "stdafx.h"
#include "Entity.h"
#include <stdio.h>
#include "typeio.h"
#include <assert.h>
#include "units.h"
#include "util.h"
// changed QD Actors
#include "actor.h"
#include "body.h"
#include "body._h"
#include "vfile.h"
#include "facelist.h"
#include "xform3d.h"
// end change

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define CLASSNAME "classname"


void CEntity::Move(geVec3d const *v)
{
	assert (v != NULL);

	if(EntityStyle!=ENTITY_S_ORIGIN)
		return;

	geVec3d_Add (&mOrigin, v, &mOrigin);
// changed QD Actors
// update actor geometry position
	if(mActorBrush!=NULL)
		Brush_Move(mActorBrush, v);
// end change
}

void CEntity::Rotate
	(
	  geXForm3d const *pXfmRotate, 
	  geVec3d const *pCenter,
	  const EntityTable *pEntityDefs
	)
// Rotate the entity about the given point
{
	geVec3d NewPos;

	assert (pXfmRotate != NULL);
	assert (pCenter != NULL);

	geVec3d_Subtract (&mOrigin, pCenter, &NewPos);
	geXForm3d_Rotate (pXfmRotate, &NewPos, &NewPos);
	geVec3d_Add (&NewPos, pCenter, &NewPos);

	SetOrigin (NewPos.X, NewPos.Y, NewPos.Z, pEntityDefs);
}

void CEntity::Scale(geFloat ScaleFactor, const EntityTable *pEntityDefs)
{
	char	temp[255];
	geVec3d NewPos;

	assert(ScaleFactor > 0.0f);

	geVec3d_Scale(&mOrigin, ScaleFactor, &NewPos);

	SetOrigin(NewPos.X, NewPos.Y, NewPos.Z, pEntityDefs);

	if(GetKeyValue("light", temp))
	{
		int	lval	=atoi(temp);
		itoa((int)(((geFloat)lval)*ScaleFactor), temp, 10);
		SetKeyValue("light", temp);
	}
}


CEntity::CEntity()
{
	// start of with new pairs but room for 10.
	mKeyArray.SetSize(0, 10);
	mValueArray.SetSize(0, 10);

	// default is an origin entity
	EntityStyle = ENTITY_S_ORIGIN;

	// start off active
	mFlags = 0;

	// start off with no group
	mGroup = 0;

// changed QD Actors
	mActorBrush = NULL;
// end change

// changed QD Actors 01/04
	geVec3d_Clear(&mOrigin);
// end change
}

CEntity::~CEntity ()
{
	mKeyArray.RemoveAll ();
	mValueArray.RemoveAll ();
// changed QD Actors
	if(mActorBrush!=NULL)
		Brush_Destroy(&mActorBrush);
// end change
}

geBoolean CEntity::IsCamera( void ) const
{
	return GetClassname().Compare( "Camera" ) == 0 ;
}

// changed QD Actors
// get the file name of the actor, if entity has an actor
BOOL CEntity::HasActor(char *ActorFile, char *PawnIni)
{
	int i, NumKeys;

	NumKeys = this->mKeyArray.GetSize();

	if(!strcmp(GetClassname(),"Pawn"))
	{
		CString Type;
		if(!GetKeyValue("PawnType",Type))
			return FALSE;

		geVFile *IniFile = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, PawnIni, NULL, GE_VFILE_OPEN_READONLY);

		if(IniFile)
		{
			CString readinfo;
			CString keyname, valuename, value;

			char szInputLine[132];

			while(geVFile_GetS(IniFile, szInputLine, 132)==GE_TRUE)
			{
				if(strlen(szInputLine) <= 1)
					readinfo = "";
				else
					readinfo = szInputLine;

				readinfo.TrimRight();

				if (readinfo != "")
				{
					if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']') //if a section heading
					{
						keyname = readinfo;
						keyname.TrimLeft('[');
						keyname.TrimRight(']');

						if(!strcmp(keyname,Type))
						{
							while(geVFile_GetS(IniFile, szInputLine, 132)==GE_TRUE)
							{
								if(strlen(szInputLine) <= 1)
									readinfo = "";
								else
									readinfo = szInputLine;

								readinfo.TrimRight();

								if (readinfo != "")
								{
									if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']')
									{
										geVFile_Close(IniFile);
										return FALSE;
									}

									if(readinfo[0] != ';')
									{
										if(readinfo.Find("=")!=-1)
										{
											valuename = readinfo.Left(readinfo.Find("="));
											value = readinfo.Right(readinfo.GetLength()-valuename.GetLength()-1);
										}
										else
											continue;

										valuename.TrimLeft();
										valuename.TrimRight();
										value.TrimLeft();
										value.TrimRight();
										if(value=="")
											continue;

										if(!strcmp(valuename, "actorname"))
										{
											strcpy(ActorFile, value);
											geVFile_Close(IniFile);
											return TRUE;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
//			AfxMessageBox("Can't open pawn.ini", MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
		}
		geVFile_Close(IniFile);
	}
	else
	{
		for(i=0;i<NumKeys;i++)
		{
			CString Value;

			Value = mValueArray[i];

			if(strstr(Value,".act")!=NULL)
			{
				strcpy(ActorFile, Value);
				return TRUE;
			}
		}
	}
	return FALSE;
}

Brush *CEntity::CreateActorBrush(char *ActorFile, char *ActorDir, char *PawnIni)
{
	char filename[255];
	geVFile *ActFile;
	geActor_Def *mActorDef;
	geActor *Actor;
	CString ActorRotStr;
	geVec3d ActorRotation;
	geXForm3d thePosition;
	geFloat Scale;
	CString ActorScale;

	// if there's an old brush, destroy it
	if(mActorBrush!=NULL)
		Brush_Destroy(&mActorBrush);

	// build filename
	strcpy(filename, ActorDir);
	strcat(filename, "\\");
	strcat(filename, ActorFile);

	ActFile = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, filename, NULL, GE_VFILE_OPEN_READONLY);
	if(ActFile)
	{
		mActorDef = geActor_DefCreateFromFile(ActFile);
		if(!mActorDef)
		{
			geVFile_Close(ActFile);
			AfxMessageBox("Failed to create ActorDef", MB_ICONEXCLAMATION | MB_OK);
			return NULL;
		}
	}
	else
	{
//		AfxMessageBox("Can't open actor file", MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}
	geVFile_Close(ActFile);
	Actor = geActor_Create(mActorDef);

	geVec3d_Clear(&ActorRotation);	// Initialize
	Scale=1.0f;
	geXForm3d_SetIdentity(&thePosition);

// reading pawn.ini (actorrotation, actorscale)
	if(!strcmp(GetClassname(),"Pawn"))
	{
		CString Type;
		GetKeyValue("PawnType",Type);

		geVFile *IniFile = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, PawnIni, NULL, GE_VFILE_OPEN_READONLY);

		if(IniFile)
		{
			CString readinfo;
			CString keyname, valuename, value;

			char szInputLine[132];

			while(geVFile_GetS(IniFile, szInputLine, 132)==GE_TRUE)
			{
				if(strlen(szInputLine) <= 1)
					readinfo = "";
				else
					readinfo = szInputLine;

				readinfo.TrimRight();

				if (readinfo != "")
				{
					if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']') //if a section heading
					{
						keyname = readinfo;
						keyname.TrimLeft('[');
						keyname.TrimRight(']');

						if(!strcmp(keyname,Type))
						{
							while(geVFile_GetS(IniFile, szInputLine, 132)==GE_TRUE)
							{
								if(strlen(szInputLine) <= 1)
									readinfo = "";
								else
									readinfo = szInputLine;

								readinfo.TrimRight();

								if (readinfo != "")
								{
									if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']')
										goto DoneRead;

									if(readinfo[0] != ';')
									{
										if(readinfo.Find("=")!=-1)
										{
											valuename = readinfo.Left(readinfo.Find("="));
											value = readinfo.Right(readinfo.GetLength()-valuename.GetLength()-1);
										}
										else
											continue;

										valuename.TrimLeft();
										valuename.TrimRight();
										value.TrimLeft();
										value.TrimRight();
										if(value=="")
											continue;

										if(!strcmp(valuename, "actorrotation"))
											sscanf(value, "%f %f %f", &ActorRotation.X, &ActorRotation.Y, &ActorRotation.Z);
										else if(!strcmp(valuename, "actorscale"))
											sscanf(value, "%f", &Scale);
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			AfxMessageBox("Can't open pawn.ini", MB_ICONEXCLAMATION | MB_OK);
		}


DoneRead:
		geVFile_Close(IniFile);
		if(GetKeyValue ("Angle", ActorRotStr))
		{
			geVec3d Angle;
			sscanf(ActorRotStr, "%f %f %f", &Angle.X, &Angle.Y, &Angle.Z);

			geVec3d_Scale(&Angle,0.0174532925199433f,&Angle);

			geXForm3d_RotateZ(&thePosition, Angle.Z);
			geXForm3d_RotateX(&thePosition, Angle.X);
			geXForm3d_RotateY(&thePosition, Angle.Y);
		}

	}
//****************************************************************************
	else
	{

// get rotation of the actor
		if(GetKeyValue ("ActorRotation", ActorRotStr))
			sscanf(ActorRotStr, "%f %f %f", &ActorRotation.X, &ActorRotation.Y, &ActorRotation.Z);
// get scaling of the actor
		if(GetKeyValue("Scale", ActorScale))
			sscanf(ActorScale, "%f", &Scale);
		else if(GetKeyValue("ScaleFactor", ActorScale))
			sscanf(ActorScale, "%f", &Scale);
		else if(GetKeyValue("ActorScaleFactor", ActorScale))
		{
			geFloat PScale;
			sscanf(ActorScale, "%f", &Scale);
			GetKeyValue("PlayerScaleFactor", ActorScale);
			sscanf(ActorScale, "%f", &PScale);
			Scale*=PScale;
		}
	}
// rotate the Actor
	geVec3d_Scale(&ActorRotation,0.0174532925199433f,&ActorRotation);

	geXForm3d_RotateZ(&thePosition, ActorRotation.Z);
	geXForm3d_RotateX(&thePosition, ActorRotation.X);
	geXForm3d_RotateY(&thePosition, ActorRotation.Y);

	geActor_ClearPose(Actor, &thePosition);

// get the body data
	geBody *B = geActor_GetBody(mActorDef);

	if(!B)
	{
		if(mActorDef!=NULL)
			geActor_DefDestroy(&mActorDef);

		if(Actor!=NULL)
			geActor_Destroy(&Actor);

		return NULL;
	}

	FaceList *fl;
	Face	*f;
	{
		int i,NumFaces=0;
		geBody_Index Index;
		geBody_Index BoneIndex;
		char BoneName[255];
		geXForm3d Transform;
		const char *BName;


		NumFaces = B->SkinFaces[0].FaceCount;

// create the facelist for the brush
		fl=FaceList_Create(NumFaces);
		if(!fl)
		{
			char szError[256];
			sprintf(szError,"Error creating FaceList: '%s'", ActorFile);
			AfxMessageBox(szError, MB_ICONEXCLAMATION | MB_OK);

			if(mActorDef!=NULL)
				geActor_DefDestroy(&mActorDef);
			if(Actor!=NULL)
				geActor_Destroy(&Actor);

			return NULL;
		}

		for(i=0;i<NumFaces;i++)
		{
			geVec3d FaceVerts[3];

			for(int j=0;j<3;j++)
			{
				// have to reverse vertex order for some reason
				Index = B->SkinFaces[0].FaceArray[i].VtxIndex[j];
				geVec3d_Copy(&(B->XSkinVertexArray[Index].XPoint),&(FaceVerts[2-j]));

				// transform vertex point by bone transformation
				BoneIndex = B->XSkinVertexArray[Index].BoneIndex;
				BName = geStrBlock_GetString(B->BoneNames, BoneIndex);
				strcpy(BoneName,BName);
				geActor_GetBoneTransform(Actor, BoneName, &Transform);
				geXForm3d_Transform(&Transform,&(FaceVerts[2-j]),&(FaceVerts[2-j]));

				geVec3d_Scale(&FaceVerts[2-j], Scale, &FaceVerts[2-j]);
			}

// create faces  and add them to the facelist
			f=Face_Create(3,FaceVerts,0);

			if(f)
				FaceList_AddFace(fl,f);
			else
			{
				// hack; if original face has no normal create a valid face instead
				geVec3d_Set(&(FaceVerts[0]), 10000.0f, 10000.0f, 10000.0f);
				geVec3d_Set(&(FaceVerts[1]), 10001.0f, 10000.0f, 10000.0f);
				geVec3d_Set(&(FaceVerts[2]), 10000.0f, 10001.0f, 10000.0f);

				f=Face_Create(3,FaceVerts,0);

				if(f)
					FaceList_AddFace(fl,f);
			}
		}
	}

	mActorBrush = Brush_Create(BRUSH_LEAF, fl, NULL);//int Type, const FaceList *fl, const BrushList *BList);
	if(!mActorBrush)
	{
		char szError[256];
		sprintf(szError,"Error creating brush: '%s'", ActorFile);
		AfxMessageBox(szError, MB_ICONEXCLAMATION | MB_OK);

		FaceList_Destroy(&fl);

		if(mActorDef!=NULL)
			geActor_DefDestroy(&mActorDef);

		if(Actor!=NULL)
			geActor_Destroy(&Actor);

		return NULL;
	}

	Brush_SetName(mActorBrush, ActorFile);

	if(mActorDef!=NULL)
		geActor_DefDestroy(&mActorDef);

	if(Actor!=NULL)
		geActor_Destroy(&Actor);

// move the brush to the right position
	Brush_Move(mActorBrush,&mOrigin);

	return mActorBrush;
}

void CEntity::DeleteActorBrush()
{
	if(mActorBrush!=NULL)
	{
		Brush_Destroy(&mActorBrush);
		mActorBrush=NULL;
	}
}
// end change

// sets selected state of entity
void CEntity::Select()
{
	mFlags |= ENTITY_SELECTED;
}

// clears selected flag of entity
void CEntity::DeSelect()
{ 
	mFlags &= ~ENTITY_SELECTED;
}

// returns selected state of entity
int CEntity::IsSelected() const
{
	return mFlags & ENTITY_SELECTED;
}

CString CEntity::GetClassname
	(
	  void
	) const
{
	CString MyClassname = "";

	this->GetKeyValue (CLASSNAME, MyClassname);
	return MyClassname;
}

CString CEntity::GetName 
	(
	  void
	) const
{
	CString MyName = "";

	this->GetKeyValue ("%name%", MyName);
	return MyName;
}

CEntity& CEntity::operator=( CEntity& Entity )
{
	if(&Entity == this)
		return *this;
	EntityStyle = Entity.EntityStyle;
	mFlags = Entity.mFlags;
	mGroup = Entity.mGroup;
	mOrigin = Entity.mOrigin;
// changed QD Actors
	if(mActorBrush!=NULL)
		Brush_Destroy(&mActorBrush);
	mActorBrush=NULL;
	if(Entity.GetActorBrush()!=NULL)
		mActorBrush =Brush_Clone(Entity.GetActorBrush());
// end change
	mKeyArray.RemoveAll();
	mValueArray.RemoveAll();

	// copy the key/value entries
	CString Key, Value;
	for( int Current = 0; Current < Entity.GetNumKeyValuePairs (); Current++ )
	{
		if (Entity.GetKeyValuePair (Current, Key, Value))
		{
			this->SetKeyValue (Key, Value);
		}
	}

	return *this;
}

// Key/value string manipulation

int CEntity::GetKeyIndex (const char *Key) const
{
	int CurrentString;
	int NumberOfStrings = mKeyArray.GetSize();

	// go through the array of entities
	for( CurrentString = 0; CurrentString < NumberOfStrings; CurrentString++ ) {
		if( !mKeyArray[CurrentString].CompareNoCase(Key) )
			return CurrentString;
	}
    return -1;
}

void CEntity::SetKeyValue (const char *Key, const char *Value)
{
	int KeyNo;

	ASSERT (Key != NULL);
	ASSERT (*Key != '\0');  // no empty key strings
	ASSERT (Value != NULL);

	KeyNo = this->GetKeyIndex (Key);
	if (KeyNo == -1)
	{
		// doesn't exist, add it...
		// the arrays must be the same size...
		ASSERT (this->mKeyArray.GetSize () == this->mValueArray.GetSize ());
		this->mKeyArray.Add (Key);
		this->mValueArray.Add (Value);
	}
	else
	{
		this->mValueArray[KeyNo] = Value;
	}
}

BOOL CEntity::GetKeyValue (const char *Key, char *Value) const
{
	CString ValueString;

	if (this->GetKeyValue (Key, ValueString))
	{
		strcpy (Value, ValueString);
		return TRUE;
	}
	return FALSE;
}

BOOL CEntity::GetKeyValue (const char *Key, CString &Value) const
{
	int KeyNo;

	KeyNo = this->GetKeyIndex (Key);
	if (KeyNo == -1)
	{
		return FALSE;
	}
	Value = this->mValueArray[KeyNo];
	return TRUE;
}

int CEntity::GetNumKeyValuePairs 
	(
	  void
	) const
{
	return this->mKeyArray.GetSize ();
}

BOOL CEntity::GetKeyValuePair 
	(
	  int Index,
	  CString &Key, 
	  CString &Value
	) const
{
	if (Index < this->GetNumKeyValuePairs ())
	{
		Key = mKeyArray[Index];
		Value = mValueArray[Index];
		return TRUE;
	}
	return FALSE;
}

// update our origin
void CEntity::UpdateOrigin (const EntityTable *pEntityDefs)
{
	// find our origin
	CString OriginStr;
	
	// if we empty leave
	if (!GetOriginString (OriginStr, pEntityDefs))
	{
		EntityStyle = ENTITY_S_BRUSH;
		return;
	}
	else {
		EntityStyle = ENTITY_S_ORIGIN;
	}

	// get our x y z
	int x, y, z;
	sscanf(OriginStr, "%d %d %d", &x, &y, &z);
// changed QD Actors
	geVec3d BrushPos;
	geVec3d_Copy(&mOrigin, &BrushPos);
// end change

	// assign them 
	geVec3d_Set (&mOrigin, (geFloat)x, (geFloat)y, (geFloat)z);
// changed QD Actors
// update actor geometry position
	if(mActorBrush!=NULL)
	{
		geVec3d MoveVec;
		geVec3d_Subtract(&mOrigin, &BrushPos, &MoveVec);
		Brush_Move(mActorBrush,&MoveVec);
	}
// end change
}

BOOL CEntity::GetOriginFieldName 
	(
	  CString &FieldName,
	  const EntityTable *pEntityDefs
	) const
{
	CString Classname;

	if (!this->GetKeyValue (CLASSNAME, Classname))
	{
		return FALSE;
	}

	return EntityTable_GetEntityOriginFieldName (pEntityDefs, Classname, FieldName);
}

BOOL CEntity::SetOrigin 
	(
	  geFloat x, 
	  geFloat y, 
	  geFloat z,
	  const EntityTable *pEntityDefs
	)
{
	CString OriginFieldName;
// changed QD Actors 12/03
	geVec3d BrushPos;
	geVec3d_Copy(&mOrigin, &BrushPos);
// end change

	geVec3d_Set (&mOrigin, x, y, z);

	if (GetOriginFieldName (OriginFieldName, pEntityDefs))
	{
		CString NewOriginString;

		NewOriginString.Format ("%d %d %d", (int)x, (int)y, (int)z);

		// update the origin string
		SetKeyValue (OriginFieldName, NewOriginString);
	}

// changed QD Actors 12/03
// update actor geometry position
	if(mActorBrush!=NULL)
	{
		geVec3d MoveVec;
		geVec3d_Subtract(&mOrigin, &BrushPos, &MoveVec);
		Brush_Move(mActorBrush,&MoveVec);
	}
// end change

	return TRUE;
}

BOOL CEntity::GetOriginString
	(
	  CString &OriginStr, 
	  const EntityTable *pEntityDefs
	) const
{
	CString OriginFieldName; 
	
	if (!GetOriginFieldName (OriginFieldName, pEntityDefs) || OriginFieldName.IsEmpty ())
	{
		return FALSE;
	}

	return this->GetKeyValue (OriginFieldName, OriginStr);
}

BOOL CEntity::GetSpecialField
	(
	  EntityTable_GetFieldFunc Callback,
	  CString &FieldValue,
	  const EntityTable *pEntityDefs
	) const
{
	CString Classname;

	if (GetKeyValue (CLASSNAME, Classname))
	{
		CString FieldName;

		if (Callback (pEntityDefs, Classname, FieldName))
		{
			return GetKeyValue (FieldName, FieldValue);
		}
	}
	return FALSE;
}

BOOL CEntity::SetSpecialField
	(
	  EntityTable_GetFieldFunc Callback,
	  CString const FieldValue,
	  const EntityTable *pEntityDefs
	)
{
	CString Classname;

	if (GetKeyValue (CLASSNAME, Classname))
	{
		CString FieldName;

		if (Callback (pEntityDefs, Classname, FieldName))
		{
			SetKeyValue (FieldName, FieldValue);
			return TRUE;
		}
	}
	return FALSE;
}

// Angles are stored here in degrees, but reported in radians
BOOL CEntity::GetAngles (geVec3d *pDir, const EntityTable *pEntityDefs) const
{
	CString FieldValue;

	if (GetSpecialField (EntityTable_GetEntityAnglesFieldName, FieldValue, pEntityDefs))
	{
		geVec3d Degrees;

		sscanf (FieldValue, "%f %f %f", &Degrees.X, &Degrees.Y, &Degrees.Z);
		geVec3d_Set (pDir, Units_DegreesToRadians (Degrees.X),Units_DegreesToRadians (Degrees.Y),Units_DegreesToRadians (Degrees.Z));
		return TRUE;
	}

	return FALSE;
}

BOOL CEntity::SetAngles (const geVec3d *pDir, const EntityTable *pEntityDefs)
{
	CString FieldValue;
	geVec3d Degrees;

	geVec3d_Set (&Degrees, Units_RadiansToDegrees (pDir->X), Units_RadiansToDegrees (pDir->Y), Units_RadiansToDegrees (pDir->Z)) ;

	FieldValue.Format ("%f %f %f", Degrees.X, Degrees.Y, Degrees.Z);
	return SetSpecialField (EntityTable_GetEntityAnglesFieldName, FieldValue, pEntityDefs);
}

/*
  The arc value is stored as an integer degrees, and returned as a geFloat radians.
*/
BOOL CEntity::GetArc (geFloat *pArc, const EntityTable *pEntityDefs) const
{
	CString FieldValue;

	if (GetSpecialField (EntityTable_GetEntityArcFieldName, FieldValue, pEntityDefs))
	{
		int ArcDegrees;

		sscanf (FieldValue, "%d", &ArcDegrees);
		*pArc = Units_DegreesToRadians (ArcDegrees);
		return TRUE;
	}
	return FALSE;
}

// SetArc takes a floating point radians value but stores the arc as an integer
// number of degrees.
BOOL CEntity::SetArc (geFloat Arc, const EntityTable *pEntityDefs)
{
	CString FieldValue;
	int ArcDegrees;

	ArcDegrees = Units_Round (Units_RadiansToDegrees (Arc));
	if( ArcDegrees > 359 )
		ArcDegrees = 359 ;
	if( ArcDegrees < 0 )
		ArcDegrees = 0 ;
	FieldValue.Format ("%d", ArcDegrees);
	return SetSpecialField (EntityTable_GetEntityArcFieldName, FieldValue, pEntityDefs);
}

BOOL CEntity::GetRadius (geFloat *pRadius, const EntityTable *pEntityDefs) const
{
	CString FieldValue;

	if (GetSpecialField (EntityTable_GetEntityRadiusFieldName, FieldValue, pEntityDefs))
	{
		sscanf (FieldValue, "%f", pRadius);
		return TRUE;
	}
	return FALSE;
}

BOOL CEntity::SetRadius (geFloat Radius, const EntityTable *pEntityDefs)
{
	CString FieldValue;

	FieldValue.Format ("%f", Radius);
	return SetSpecialField (EntityTable_GetEntityRadiusFieldName, FieldValue, pEntityDefs);
}

void CEntity::Export(FILE *OutFile)
{
	int		i;

	fprintf( OutFile, "{\n");

	for(i = 0; i < this->GetNumKeyValuePairs (); i++)
	{
		CString KeyS, ValueS;

		if (this->GetKeyValuePair (i, KeyS, ValueS))
		{
			fprintf (OutFile, "\"%s\" \"%s\"\n", (LPCSTR)KeyS, (LPCSTR)ValueS);
		}
	}

}

geFloat CEntity::DistanceFrom (geVec3d const *pPoint)
{
	return geVec3d_DistanceBetween (pPoint, &mOrigin);
}

geFloat CEntity::RayDistance(CPoint point, ViewVars *v)
{
	if(EntityStyle!=ENTITY_S_ORIGIN)
		return 0;

	//first check the entity bounds to see if the click is inside
	geVec3d box[8], wPoint;
	CPoint min, max, pnt;
	min.x=min.y=9999;
	max.x=max.y=-9999;

	geFloat xPlus8  = (geFloat)(mOrigin.X + 8.0);
	geFloat xMinus8 = (geFloat)(mOrigin.X - 8.0);
	geFloat yPlus8  = (geFloat)(mOrigin.Y + 8.0);
	geFloat yMinus8 = (geFloat)(mOrigin.Y - 8.0);
	geFloat zPlus8  = (geFloat)(mOrigin.Z + 8.0);
	geFloat zMinus8 = (geFloat)(mOrigin.Z - 8.0);

	geVec3d_Set (&box[0], xPlus8,  yPlus8,  zPlus8);
	geVec3d_Set (&box[1], xMinus8, yPlus8,  zPlus8);
	geVec3d_Set (&box[2], xMinus8, yMinus8, zPlus8);
	geVec3d_Set (&box[3], xPlus8,  yMinus8, zPlus8);
	geVec3d_Set (&box[4], xPlus8,  yPlus8,  zMinus8);
	geVec3d_Set (&box[5], xMinus8, yPlus8,  zMinus8);
	geVec3d_Set (&box[6], xMinus8, yMinus8, zMinus8);
	geVec3d_Set (&box[7], xPlus8,  yMinus8, zMinus8);

//	BOOL anyIn=false;

	for(int i=0;i<8;i++)
	{
		wPoint	=Render_XFormVert(v, &box[i]);
		pnt.x	= (long)(wPoint.X);
		pnt.y	= (long)(wPoint.Y);
		//anyIn=(anyIn || PtInRect(Camera.mClip, pnt));
		if(pnt.x < min.x) min.x=pnt.x;
		if(pnt.x > max.x) max.x=pnt.x;
		if(pnt.y < min.y) min.y=pnt.y;
		if(pnt.y > max.y) max.y=pnt.y;
	}

	if((point.x > min.x && point.x < max.x)
		&&(point.y > min.y && point.y < max.y))
	{
		Render_GetCameraPos(v, &wPoint);
		geVec3d_Subtract(&mOrigin, &wPoint, &wPoint);

		return	geVec3d_Length(&wPoint);
	}
	else
	{
		return	0;
	}
}

// check whether or not this entity is within this
// bounding box
//==================================================================
// When we are done moving the entity we want to snap it to the
// grid etc.
//==================================================================
#define RoundDouble(x)  (floor( (x) + 0.5 ))

static geFloat SnapToGrid
	(
	  geFloat x,
	  double GridSize
	)
{
	return (geFloat)(RoundDouble ((x/GridSize) ) * GridSize) ;
}

void CEntity::DoneMove(double GridSize, const EntityTable *pEntityDefs)
{
	geFloat x, y, z;
// changed QD Actors
	geVec3d BrushPos;
	geVec3d_Copy(&mOrigin, &BrushPos);
// end change
	// Snap to the grid
	x = SnapToGrid (mOrigin.X, GridSize);
	y = SnapToGrid (mOrigin.Y, GridSize);
	z = SnapToGrid (mOrigin.Z, GridSize);


	SetOrigin (x, y, z, pEntityDefs);
// changed QD Actors
// update actor geometry position
	if(mActorBrush!=NULL)
	{
		geVec3d MoveVec;
		geVec3d_Subtract(&mOrigin, &BrushPos, &MoveVec);
		Brush_Move(mActorBrush,&MoveVec);
	}
// end change
}

// update our origin
void CEntity::UpdateOriginFirst(const EntityTable *pEntityDefs)
{
	// find our origin
	CString OriginStr;

	// if we empty leave
	if (!GetOriginString (OriginStr, pEntityDefs)) 
	{
		EntityStyle = ENTITY_S_BRUSH;
		return;
	}
	else 
	{
		EntityStyle = ENTITY_S_ORIGIN;
	}

	// get our x y z
	int x, y, z;
	sscanf(OriginStr, "%d %d %d", &x, &y, &z);

	SetOrigin ((geFloat)x, (geFloat)y, (geFloat)z, pEntityDefs);
}


#define CENTITYTYPE "CEntity"
#define CENTSTYLE "eStyle"
#define CENTORIGIN "eOrigin"
#define CENTRENDERORIGIN "eRenderOrigin"
#define CENTPAIRCOUNT "ePairCount"
#define CENTFLAGS "eFlags"
#define CENTGROUP "eGroup"
#define CENTKEY "K"
#define CENTVALUE "V"
#define CENDENTTYPE "End CEntity"

// -------------------------------------------------------------------------------
// saves the entity out to a specified file stream
geBoolean CEntity::SaveToFile( FILE *file )
{
	ASSERT( file );

	int	Count;	// number of keys

	if (fprintf(file, "CEntity\neStyle %d\n", EntityStyle ) < 0) return GE_FALSE;
	if (fprintf(file, "eOrigin %d %d %d 2\n",
		Units_Round( mOrigin.X ), Units_Round( mOrigin.Y ), Units_Round( mOrigin.Z ) ) < 0) return GE_FALSE;

	if (fprintf(file, "eFlags %d\n",  mFlags) < 0) return GE_FALSE;
	if (fprintf(file, "eGroup %d\n",  mGroup) < 0) return GE_FALSE;

	Count	= GetNumKeyValuePairs ();
	if (fprintf(file, "ePairCount %d\n", Count) < 0) return GE_FALSE;

	for (int j = 0; j < Count; j++)
	{
		CString Key, Value;

		if (GetKeyValuePair (j, Key, Value))
		{
			char QuotedValue[SCANNER_MAXDATA];

			Util_QuoteString (Value, QuotedValue);
			if (fprintf (file, "Key %s Value %s\n", (LPCSTR)Key, QuotedValue) < 0) return GE_FALSE;
		}
	}
	return GE_TRUE;
}

// changed QD 12/03
#define CHUNK_COLORRGB				0x0011

#define CHUNK_OBJBLOCK				0x4000
#define CHUNK_LIGHT					0x4600	// Light
#define CHUNK_SPOTLIGHT				0x4610	// Spotlight
#define CHUNK_SPOT_RAYTRACE			0x4627	// Spot raytrace
#define CHUNK_LIGHTSHADOWED			0x4630	// Light shadowed
#define CHUNK_SPOT_SHADOWMAP		0x4641	// Spot shadow map //float bias float filter ushort size
//#define CHUNK_SPOT_SHOWCONE		0x4650	// Spot show cone
//#define CHUNK_SPOT_ISRECTANGULAR	0x4651	// Spot is rectangular
//#define CHUNK_SPOT_OVERSHOOT		0x4652	// Spot overshoot
//#define CHUNK_SPOT_MAP			0x4653	// Spot map (projector)
#define CHUNK_SPOT_ROLL				0x4656	// Spot roll
//#define CHUNK_SPOT_ASPECT			0x4657
#define CHUNK_SPOT_RAYTRACEBIAS		0x4658	// Spot ray trace bias

//#define CHUNK_LIGHTOFF			0x4620	// Light off
#define CHUNK_ATTENUATIONON			0x4625	// Attenuation on
#define CHUNK_RANGESTART			0x4659	// Range start
#define CHUNK_RANGEEND				0x465A	// Range end
#define CHUNK_MULTIPLIER			0x465B	// Multiplier
//#define CHUNK_EXCLUDE				0x4654

// Keyframe chunks
#define CHUNK_KF_OMNILIGHT		0xB005
#define CHUNK_KF_LIGHTTARGET	0xB006
#define CHUNK_KF_SPOTLIGHT		0xB007
#define CHUNK_KF_NODE_ID		0xB030
#define CHUNK_KF_NODE_HDR		0xB010
#define CHUNK_KF_POSTRACK		0xB020
#define CHUNK_KF_COLORTRACK		0xB025
#define CHUNK_KF_HOTSPOTTRACK	0xB027
#define CHUNK_KF_FALLOFFTRACK	0xB028
#define CHUNK_KF_ROLLTRACK		0xB024

#define SIZE_CHUNKID		sizeof(unsigned short)
#define SIZE_CHUNKLENGTH	sizeof(long)
#define SIZE_USHORT			sizeof(unsigned short)
#define SIZE_FLOAT			sizeof(float)

// saves a light entity out to a 3ds
geBoolean CEntity::ExportTo3ds(FILE *file, int EntityCount)
{
	int size_name, size_color, size_spotlight, size_light, size_objblock;
	if(!file) return GE_FALSE;

	if(strcmp(GetClassname(),"light")&&strcmp(GetClassname(),"spotlight"))
		return GE_TRUE;

	// calculate the size of the different chunks
	size_name		= 8; //AAAAxxx\0
	size_color		= 9;
	size_spotlight	= SIZE_CHUNKID+SIZE_CHUNKLENGTH+5*SIZE_FLOAT+10+16+10+6;
	size_light		= SIZE_CHUNKID+SIZE_CHUNKLENGTH+3*SIZE_FLOAT+size_color+30+6;
	if(!strcmp(GetClassname(),"spotlight"))
		size_light+=size_spotlight;

	size_objblock	= SIZE_CHUNKID+SIZE_CHUNKLENGTH+size_name+size_light;

	// write the objblock
	TypeIO_WriteUshort(file,CHUNK_OBJBLOCK);
	TypeIO_WriteInt(file,size_objblock);
	// give each object a unique name AAAAxxx\0
	if(!strcmp(GetClassname(),"light"))
	{
		TypeIO_WriteUChar(file,'O');
		TypeIO_WriteUChar(file,'m');
		TypeIO_WriteUChar(file,'n');
		TypeIO_WriteUChar(file,'i');
	}
	else if(!strcmp(GetClassname(),"spotlight"))
	{
		TypeIO_WriteUChar(file,'S');
		TypeIO_WriteUChar(file,'p');
		TypeIO_WriteUChar(file,'o');
		TypeIO_WriteUChar(file,'t');
	}

	TypeIO_WriteUChar(file,(unsigned char)(48+(EntityCount-EntityCount%100)/100));
	TypeIO_WriteUChar(file,(unsigned char)(48+((EntityCount-EntityCount%10)/10)%10));
	TypeIO_WriteUChar(file,(unsigned char)(48+EntityCount%10));
	TypeIO_WriteUChar(file,'\0');
	// end name of this object

	// this object is a light
	TypeIO_WriteUshort(file,CHUNK_LIGHT);
	TypeIO_WriteInt(file,size_light);
	TypeIO_WriteFloat(file, mOrigin.X);
	TypeIO_WriteFloat(file, mOrigin.Y);
	TypeIO_WriteFloat(file, mOrigin.Z);

	{
		CString ColorStr;
		int r, g, b;
		r=g=b=128;
		if(GetKeyValue ("color", ColorStr))
			sscanf(ColorStr, "%d %d %d", &r, &g, &b);

		TypeIO_WriteUshort(file, CHUNK_COLORRGB);
		TypeIO_WriteInt(file,6+3);
		TypeIO_WriteUChar(file, (unsigned char)r);
		TypeIO_WriteUChar(file, (unsigned char)g);
		TypeIO_WriteUChar(file, (unsigned char)b);
	}

	{
		CString RangeStr;
		int range;
		range=200;
		if(GetKeyValue ("light", RangeStr))
			sscanf(RangeStr, "%d", &range);

		TypeIO_WriteUshort(file, CHUNK_RANGEEND);
		TypeIO_WriteInt(file,10);
		TypeIO_WriteFloat(file, (geFloat)range);
	}

	TypeIO_WriteUshort(file, CHUNK_RANGESTART);
	TypeIO_WriteInt(file,10);
	TypeIO_WriteFloat(file, 100.0f);

	TypeIO_WriteUshort(file, CHUNK_MULTIPLIER);
	TypeIO_WriteInt(file,10);
	TypeIO_WriteFloat(file, 1.0f);

	if(!strcmp(GetClassname(),"spotlight"))
	{
		CString DirStr, AngleStr;
		geVec3d Direction, Angles, target;
		geXForm3d Rotation;
		int Hotspot;

		TypeIO_WriteUshort(file, CHUNK_SPOTLIGHT);
		TypeIO_WriteInt(file, size_spotlight);

		Direction.X=1.0f;
		Direction.Y=0.0f;
		Direction.Z=0.0f;

		if(GetKeyValue ("angles", DirStr))
			sscanf(DirStr, "%f %f %f", &Angles.X, &Angles.Y, &Angles.Z);

		geVec3d_Scale(&Angles, GE_PI/180.0f, &Angles);

		geXForm3d_SetIdentity(&Rotation);
		geXForm3d_SetEulerAngles(&Rotation, &Angles);
		geXForm3d_Transform(&Rotation, &Direction, &Direction);
		geVec3d_Normalize(&Direction);
		geVec3d_Copy(&mOrigin, &target);
		geVec3d_AddScaled(&target, &Direction, 200.0f, &target);

		TypeIO_WriteFloat(file, target.X);
		TypeIO_WriteFloat(file, target.Y);
		TypeIO_WriteFloat(file, target.Z);

		Hotspot =60;
		if(GetKeyValue ("arc", AngleStr))
			sscanf(AngleStr, "%i", &Hotspot);

		TypeIO_WriteFloat(file, (geFloat)Hotspot);
		TypeIO_WriteFloat(file, (geFloat)(Hotspot+5.0f));

		TypeIO_WriteUshort(file, CHUNK_SPOT_ROLL);
		TypeIO_WriteInt(file, 10);
		TypeIO_WriteFloat(file, 0.0f);

		TypeIO_WriteUshort(file, CHUNK_LIGHTSHADOWED);
		TypeIO_WriteInt(file,6);

		TypeIO_WriteUshort(file, CHUNK_SPOT_SHADOWMAP);
		TypeIO_WriteInt(file, 16);
		TypeIO_WriteFloat(file, 1.0f);
		TypeIO_WriteFloat(file, 5.0f);
		TypeIO_WriteUshort(file, 512);

		TypeIO_WriteUshort(file, CHUNK_SPOT_RAYTRACEBIAS);
		TypeIO_WriteInt(file, 10);
		TypeIO_WriteFloat(file, 0.0f);
	}

	TypeIO_WriteUshort(file, CHUNK_ATTENUATIONON);
	TypeIO_WriteInt(file,6);

	return GE_TRUE;
}

// lights need keyframes or they will be placed at the origin by default
geBoolean CEntity::ExportKFTo3ds(FILE *file, int LCount, int SLCount)
{
	int i, EntityCount;
	if(!file) return GE_FALSE;

	if(strcmp(GetClassname(),"light")&&strcmp(GetClassname(),"spotlight"))
		return GE_TRUE;

	EntityCount = LCount+2*SLCount;

	if(!strcmp(GetClassname(),"light"))
	{
		TypeIO_WriteUshort(file,CHUNK_KF_OMNILIGHT);
		TypeIO_WriteInt(file, 110);
	}
	else
	{
		TypeIO_WriteUshort(file,CHUNK_KF_SPOTLIGHT);
		TypeIO_WriteInt(file, 200);
	}

	TypeIO_WriteUshort(file,CHUNK_KF_NODE_ID);
	TypeIO_WriteInt(file, 8);
	TypeIO_WriteUshort(file, (unsigned short)EntityCount);

	TypeIO_WriteUshort(file,CHUNK_KF_NODE_HDR);
	TypeIO_WriteInt(file, 20);
	// unique name AAAAxxx\0
	if(!strcmp(GetClassname(),"light"))
	{
		TypeIO_WriteUChar(file,'O');
		TypeIO_WriteUChar(file,'m');
		TypeIO_WriteUChar(file,'n');
		TypeIO_WriteUChar(file,'i');
		TypeIO_WriteUChar(file,(unsigned char)(48+(LCount-LCount%100)/100));
		TypeIO_WriteUChar(file,(unsigned char)(48+((LCount-LCount%10)/10)%10));
		TypeIO_WriteUChar(file,(unsigned char)(48+LCount%10));
		TypeIO_WriteUChar(file,'\0');
	}
	else if(!strcmp(GetClassname(),"spotlight"))
	{
		TypeIO_WriteUChar(file,'S');
		TypeIO_WriteUChar(file,'p');
		TypeIO_WriteUChar(file,'o');
		TypeIO_WriteUChar(file,'t');
		TypeIO_WriteUChar(file,(unsigned char)(48+((SLCount)-(SLCount)%100)/100));
		TypeIO_WriteUChar(file,(unsigned char)(48+(((SLCount)-(SLCount)%10)/10)%10));
		TypeIO_WriteUChar(file,(unsigned char)(48+(SLCount)%10));
		TypeIO_WriteUChar(file,'\0');
	}
	// end name of this object
	TypeIO_WriteUshort(file, 16384);
	TypeIO_WriteUshort(file, 0);
	TypeIO_WriteUshort(file, 65535);

	// position
	{
		TypeIO_WriteUshort(file,CHUNK_KF_POSTRACK);
		TypeIO_WriteInt(file,38);
		for(i=0;i<5;i++)
			TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,1);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteInt(file, 0);
		TypeIO_WriteFloat(file, mOrigin.X);
		TypeIO_WriteFloat(file, mOrigin.Y);
		TypeIO_WriteFloat(file, mOrigin.Z);
	}
	// color
	{
		CString ColorStr;
		int r, g, b;
		r=g=b=128;
		if(GetKeyValue ("color", ColorStr))
			sscanf(ColorStr, "%d %d %d", &r, &g, &b);

		TypeIO_WriteUshort(file,CHUNK_KF_COLORTRACK);
		TypeIO_WriteInt(file,38);
		for(i=0;i<5;i++)
			TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,1);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteInt(file, 0);
		TypeIO_WriteFloat(file, (geFloat)r/255.0f);
		TypeIO_WriteFloat(file, (geFloat)g/255.0f);
		TypeIO_WriteFloat(file, (geFloat)b/255.0f);
	}

	if(!strcmp(GetClassname(),"spotlight"))
	{
		CString DirStr, AngleStr;
		geVec3d Direction, Angles, target;
		geXForm3d Rotation;
		int Hotspot;

		Hotspot =60;
		if(GetKeyValue ("arc", AngleStr))
			sscanf(AngleStr, "%i", &Hotspot);

		TypeIO_WriteUshort(file,CHUNK_KF_HOTSPOTTRACK);
		TypeIO_WriteInt(file,30);
		for(i=0;i<5;i++)
			TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,1);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteInt(file, 0);
		TypeIO_WriteFloat(file, (geFloat)Hotspot);

		TypeIO_WriteUshort(file,CHUNK_KF_FALLOFFTRACK);
		TypeIO_WriteInt(file,30);
		for(i=0;i<5;i++)
			TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,1);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteInt(file, 0);
		TypeIO_WriteFloat(file, (geFloat)Hotspot+5.0f);

		TypeIO_WriteUshort(file,CHUNK_KF_ROLLTRACK);
		TypeIO_WriteInt(file,30);
		for(i=0;i<5;i++)
			TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,1);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteInt(file, 0);
		TypeIO_WriteFloat(file, 0.0f);

		Direction.X=1.0f;
		Direction.Y=0.0f;
		Direction.Z=0.0f;

		if(GetKeyValue ("angles", DirStr))
			sscanf(DirStr, "%f %f %f", &Angles.X, &Angles.Y, &Angles.Z);

		geVec3d_Scale(&Angles, GE_PI/180.0f, &Angles);

		geXForm3d_SetIdentity(&Rotation);
		geXForm3d_SetEulerAngles(&Rotation, &Angles);
		geXForm3d_Transform(&Rotation, &Direction, &Direction);
		geVec3d_Normalize(&Direction);
		geVec3d_Copy(&mOrigin, &target);
		geVec3d_AddScaled(&target, &Direction, 200.0f, &target);

		TypeIO_WriteUshort(file,CHUNK_KF_LIGHTTARGET);
		TypeIO_WriteInt(file, 72);

		TypeIO_WriteUshort(file,CHUNK_KF_NODE_ID);
		TypeIO_WriteInt(file, 8);
		TypeIO_WriteUshort(file, (unsigned short)(EntityCount+1));

		TypeIO_WriteUshort(file,CHUNK_KF_NODE_HDR);
		TypeIO_WriteInt(file, 20);

		// unique name AAAAxxx\0
		TypeIO_WriteUChar(file,'S');
		TypeIO_WriteUChar(file,'p');
		TypeIO_WriteUChar(file,'o');
		TypeIO_WriteUChar(file,'t');
		TypeIO_WriteUChar(file,(unsigned char)(48+(SLCount-SLCount%100)/100));
		TypeIO_WriteUChar(file,(unsigned char)(48+((SLCount-SLCount%10)/10)%10));
		TypeIO_WriteUChar(file,(unsigned char)(48+SLCount%10));
		TypeIO_WriteUChar(file,'\0');
		TypeIO_WriteUshort(file, 16384);
		TypeIO_WriteUshort(file, 0);
		TypeIO_WriteUshort(file, 65535);

		TypeIO_WriteUshort(file,CHUNK_KF_POSTRACK);
		TypeIO_WriteInt(file,38);
		for(i=0;i<5;i++)
			TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,1);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteUshort(file,0);
		TypeIO_WriteInt(file, 0);
		TypeIO_WriteFloat(file, target.X);
		TypeIO_WriteFloat(file, target.Y);
		TypeIO_WriteFloat(file, target.Z);
	}

	return GE_TRUE;
}
// end change

static char *StripNewline
	(
	  char *s
	)
// terminates the string at the first cr or lf character
{
	char *c;

	ASSERT (s != NULL);

	c = s;
	while (*c != '\0')
	{
		switch (*c)
		{
			case '\r' :
			case '\n' :
				*c = '\0';
				break;
			default :
				++c;
				break;
		}
	}
	return s;
}

// -----------------------------------------------------------------------------------
// reads the entity in from a specified file stream
geBoolean CEntity::ReadFromFile
	(
	  Parse3dt *Parser,
	  int VersionMajor,
	  int VersionMinor,
	  const char **Expected,
	  const EntityTable *pEntityDefs
	)
{
	int Trash;
	int Count;

	assert (Parser != NULL);

	if (!Parse3dt_ScanExpectingText (Parser, (*Expected = "CEntity"))) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "eStyle"), &Trash)) return GE_FALSE;
	EntityStyle = (enum EntityStyles)Trash;

	if (!Parse3dt_GetVec3d (Parser, (*Expected = "eOrigin"), &mOrigin)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, NULL, &Trash)) return GE_FALSE;

	if ((VersionMajor == 1) && (VersionMinor < 21))
	{
		geVec3d TrashVec;
		if (!Parse3dt_GetVec3d (Parser, (*Expected = "eRenderOrigin"), &TrashVec)) return GE_FALSE;
		if (!Parse3dt_GetInt (Parser, NULL, &Trash)) return GE_FALSE;
	}

	// need to flip Z on file versions prior to 1.4
	if ((VersionMajor==1) && (VersionMinor < 4))
	{
		mOrigin.Z		=-mOrigin.Z;
	}

	if (!Parse3dt_GetInt (Parser, (*Expected = "eFlags"), &mFlags)) return GE_FALSE;
	if (!Parse3dt_GetInt (Parser, (*Expected = "eGroup"), &mGroup)) return GE_FALSE;

	if( (VersionMajor==1) && (VersionMinor <= 6 ) )
	{
		if( mGroup == -1 )	// "No group" was -1 and now is 0
			mGroup = 0 ;
	}

	if (!Parse3dt_GetInt (Parser, (*Expected = "ePairCount"), &Count)) return GE_FALSE;

	if ((VersionMajor == 1) && (VersionMinor < 14))
	{
		// old key/value stuff
		FILE *file;

		file = Scanner_GetFile (Parser->Scanner);

		for( int j = 0; j < Count; j++)
		{
			char Key[255];
			char Value[255];

			fscanf( file, "K " );
			fgets( Key, 255, file );
			StripNewline (Key);

			fscanf( file, "V " );
			fgets( Value, 255, file );
			StripNewline (Value);

			this->SetKeyValue (Key, Value);
		}
	}
	else
	{
		int j;
		char Key[SCANNER_MAXDATA];
		char Value[SCANNER_MAXDATA];

		for (j = 0; j < Count; ++j)
		{
			if (!Parse3dt_GetIdentifier (Parser, (*Expected = "Key"), Key)) return GE_FALSE;
			if (!Parse3dt_GetLiteral (Parser, (*Expected = "Value"), Value)) return GE_FALSE;
			SetKeyValue (Key, Value);
		}
	}
	/*
	  If the Z was flipped above (i.e. this is a version 1.3 file or earlier,
	  then the Z in the origin key string (if it exists) will not agree
	  with mOrigin.Z.
	  So here we make sure they agree.  This has the effect of making the
	  origin key string superfluous on save/restore, but wtf.
	*/

	//versions < 1.10 need to convert to texels from centimeters
	if ((VersionMajor==1) && (VersionMinor < 10))
	{
		geVec3d_Scale (&mOrigin, Units_CentimetersToEngine (1.0f), &mOrigin);
	}
	SetOrigin (mOrigin.X, mOrigin.Y, mOrigin.Z, pEntityDefs);

	return GE_TRUE;
}

void CEntity::SetGroupId 
	(
	  int id
	)
{
	this->mGroup = id;
}

int CEntity::GetGroupId 
	(
	  void
	) const 
{
	return this->mGroup;
}

void CEntity::SetVisible 
	(
	  BOOL flag
	)
{
	if (flag)
	{
		this->mFlags &= ~ENTITY_HIDDEN;
	}
	else
	{
		this->mFlags |= ENTITY_HIDDEN;
	}	
}

BOOL CEntity::IsVisible 
	(
	  void
	) const
{
	return !(this->mFlags & ENTITY_HIDDEN);
}

void CEntity::SetLock
	(
	  BOOL flag
	)
{
	if (flag)
	{
		this->mFlags |= ENTITY_LOCKED;
	}
	else
	{
		this->mFlags &= ~ENTITY_LOCKED;
	}
}

BOOL CEntity::IsLocked 
	(
	  void
	) const
{
	return (this->mFlags & ENTITY_LOCKED);
}

static CEntity const *EntityList_FindByName
	(
	  CEntityArray  *pEnts,
	  char const *pName
	)
{
	for (int i = 0; i < pEnts->GetSize (); ++i)
	{
		CEntity const *pEnt;
		CString Name;

//		pEnt = &((*mEntityArray)[CurrentEnt]);
		pEnt = &((*pEnts)[i]);
		pEnt->GetKeyValue ("%name%", Name);
		if (stricmp (pName, Name) == 0)
		{
			return pEnt;
		}
	}
	return NULL;
}


CEntity * EntityList_FindByClassName
	(
	  CEntityArray  *	pEnts,
	  const char	*	pName
	)
{
	CEntity * pEnt;

	for (int i = 0; i < pEnts->GetSize (); ++i)
	{
		pEnt = &((*pEnts)[i]);
		if( pEnt->GetClassname().Compare( pName ) == 0 )
		{
			return pEnt;
		}
	}
	return NULL;
}/* EntityList_FindByClassName */


BOOL CEntity::IsValidKey
	(
	  CString const &Key,
	  ModelList const *pModels, 
	  CEntityArray const *pEnts,
	  const EntityTable *pEntityDefs
	) const
// returns TRUE if the key is one of the specials, or if it has type information
{
	BOOL Success;
	TopType EntityType;
	CString EntityClassname = GetClassname ();

	if ((Key == CLASSNAME) || (Key == "%name%"))
	{
		Success = TRUE;
	}
	else
	{
		Success = EntityTable_GetEntityPropertyType (pEntityDefs, EntityClassname, Key, &EntityType);
		if (Success)
		{
			CString Value;

			GetKeyValue (Key, Value);
			switch (EntityType)
			{
				case T_STRUCT :
				{
					// look for an entity with this name...
					CEntity const *Target;

					// yeah, I know the cast is ugly.
					// FindByName didn't like taking the address of something in a
					// const array.  I don't know what the deal is...
					Target = EntityList_FindByName (const_cast<CEntityArray *>(pEnts), Value);
					Success = (Target != NULL);
					break;
				}

				case T_MODEL :
				{
					// look for model of this name in models list
					Model *pModel;

					pModel = ModelList_FindByName (pModels, Value);
					Success = (pModel != NULL);
					break;
				}

				default :
					break;
			}
		}
	}
	return Success;
}

int CEntity::GetNumValidKeyValuePairs
	(
	  ModelList const *pModels, 
	  CEntityArray const *pEnts,
	  const EntityTable *pEntityDefs
	) const
{
	int j;
	int NumKeys;
	int NumValidKeys;

	NumValidKeys = 0;
	NumKeys = GetNumKeyValuePairs ();
	for (j = 0; j < NumKeys; j++)
	{
		CString Key, Value;

		if (GetKeyValuePair (j, Key, Value))
		{
			BOOL Success;

			// only count those that have type information entries
			Success = IsValidKey (Key, pModels, pEnts, pEntityDefs);

			if (Success)
			{
				++NumValidKeys;
			}
		}
	}
	return NumValidKeys;
}

// writes the entity out to the map file
void CEntity::WriteToMap 
	(
	  FILE *exfile, 
	  ModelList const *pModels, 
	  CEntityArray const *pEnts,
	  const EntityTable *pEntityDefs
	) const
{
	int NumValidKeys;
	int NumKeys;
#ifdef _DEBUG
	int NumKeysWritten;
#endif
	CString EntityClassname = GetClassname ();

	TypeIO_WriteInt (exfile, 0);	// numbrushes
	TypeIO_WriteInt (exfile, 0);	// no motion data today

	// output entity key/value pairs
	// We output only those keys that have type information entries.
	NumValidKeys = GetNumValidKeyValuePairs (pModels, pEnts, pEntityDefs);
	TypeIO_WriteInt (exfile, NumValidKeys);	//number of keys
	
	NumKeys = GetNumKeyValuePairs ();

#ifdef _DEBUG
	NumKeysWritten = 0;
#endif

	for(int j = 0; j < NumKeys; j++)
	{
		CString Key, Value;

		if (GetKeyValuePair (j, Key, Value))
		{
			// only output those that have type information entries
			BOOL Success;

			Success = IsValidKey (Key, pModels, pEnts, pEntityDefs);
		
			if (Success)
			{
				char ValueString [100];  // string to store value
				TopType KeyType;

				// get the type.
				// Since classname and %name% don't have type info,
				// we need to check success flag ...
				Success = EntityTable_GetEntityPropertyType (pEntityDefs, GetClassname(), Key, &KeyType);

				// Boolean keys are converted to integer
				if (Success && (KeyType == T_BOOLEAN))
				{
					if (Value == "True")
					{
						strcpy (ValueString, "1");
					}
					else
					{
						strcpy (ValueString, "0");
					}
				}
				else
				{
					strcpy (ValueString, Value);
				}

				// output name and value
				TypeIO_WriteString (exfile, Key);
				TypeIO_WriteString (exfile, ValueString);
#ifdef _DEBUG
				++NumKeysWritten;
#endif
			}
		}
	}
#ifdef _DEBUG
	assert (NumKeysWritten == NumValidKeys);
#endif
}


int EntityList_Enum
	(
		CEntityArray&		EntityArray,
		void *				lParam,
		EntityList_CB		CallBack
	)
{
	geBoolean bResult = GE_TRUE ;	// TRUE means entire list was processed
	int i ;

	for( i=0; i< EntityArray.GetSize(); i++ )
	{
		if( (bResult = CallBack( EntityArray[i], lParam )) == GE_FALSE )
			break ;
	}

	return bResult ;
}

const geBitmap *CEntity::GetBitmapPtr (const EntityTable *pEntityDefs) const
{
	return EntityTable_GetBitmapPtr (pEntityDefs, GetClassname ());
}
