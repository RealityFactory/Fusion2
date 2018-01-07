/****************************************************************************************/
/*  GEditProDoc.cpp                                                                     */
/*                                                                                      */
/*  Author:       Jim Mischel, Ken Baird, Jeff Lomax, John Moore, Bruce Cooner          */
/*  Description:  A very large file that does too much to describe                      */
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
#include "stdafx.h"
#include "Globals.h"
#include "GEditProDoc.h"
#include <float.h>

#include "CreateArchDialog.h"
#include "CreateBoxDialog.h"
#include "CreateConeDialog.h"
#include "CreateCylDialog.h"
#include "CreateSpheroidDialog.h"
#include "CreateStaircaseDialog.h"
#include "CompileDialog.h"
#include "gridsizedialog.h"
#include "Entity.h"
#include "entitiesdialog.h"
#include "FaceAttributesDialog.h"
#include "BrushAttributesDialog.h"
#include "KeyEditDlg.h"
#include "EntityVisDlg.h"
#include "LevelOptions.h"

#include "GEditProView.h"
#include "wadfile.h"
#include "XForm3d.h"
#include "gbsplib.h"
#include "render.h"
#include "EntityTable.h"
#include "brush.h"
#include "node.h"
#include "facelist.h"
#include "ModelsDlg.h"

#include "ChildFrm.h"

#include "tLevelMgr.h"

// changed QD 12/03
#include "Export3dsDialog.h"
// end change

#include <afxole.h>
#pragma warning(disable : 4201 4214 4115)
#include <mmsystem.h>
#pragma warning(default : 4201 4214 4115)
#include <errno.h>
#include <direct.h>
#include <assert.h>
#include "basetype.h"
#include "Vec3d.h"
#include "brush.h"
#include "typeio.h"
#include "units.h"
#include "FilePath.h"
#include <io.h>		// for _access
#include "ram.h"
#include "util.h"
#include "BrushTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ENUMERATION FUNCTIONS (File Scope)
static void fdocDrawEntity
	(
	  CEntity const *	pEnt,
	  ViewVars  const *	v,
	  CDC			*	pDC,
	  EntityTable const *pEntityDefs,
	  BOOL				bShowUI
	)
{
#define ENTITY_SIZE (8.0f)  // 16" across	//	old gedit
//#define ENTITY_SIZE (16.0f)  // 32" across	//	new g3dc


	geVec3d VecOrigin;
	geVec3d EntSizeWorld;	// entity size in world space

	POINT EntPosView;
	POINT EntSizeView;
	POINT EntWidthHeight ;
	POINT OriginView;

	POINT TopLeft, BottomRight;
	POINT TopRight, BottomLeft;

	static const float COS45	= (float)cos (M_PI/4.0f);
	static const float SIN45	= (float)sin (M_PI/4.0f);
	static const float MCOS45	= (float)cos (-(M_PI/4.0f));
	static const float MSIN45	= (float)sin (-(M_PI/4.0f));

	// compute entity size in view coordinates
	geVec3d_Set (&EntSizeWorld, ENTITY_SIZE, ENTITY_SIZE, ENTITY_SIZE);
	EntSizeView = Render_OrthoWorldToView ( v, &EntSizeWorld);
	geVec3d_Clear (&VecOrigin);
	OriginView = Render_OrthoWorldToView ( v, &VecOrigin);
	// This one is the width and height of the Entity
	EntWidthHeight.x = max( OriginView.x, EntSizeView.x ) - min( OriginView.x, EntSizeView.x ) ;
	EntWidthHeight.y = max( OriginView.y, EntSizeView.y ) - min( OriginView.y, EntSizeView.y ) ;

	// This can have negative numbers
	EntSizeView.x -= OriginView.x;
	EntSizeView.y -= OriginView.y;

	// entity's position in the view
	EntPosView = Render_OrthoWorldToView ( v, &(pEnt->mOrigin));

	{
			//	post 0.57	for changing the scale of entities
			//	as they are drawn in the ortho views
		static float entityScaleFactor;
		CGEditProDoc *pDoc = CGlobals::GetActiveDocument();
		if (pDoc)
			entityScaleFactor = pDoc->GetEntityScaleInView();

		// Draw an X at the entity's position...
		TopLeft.x = EntPosView.x - EntSizeView.x*entityScaleFactor;	//	post 0.57
		TopLeft.y = EntPosView.y - EntSizeView.y*entityScaleFactor;	//	post 0.57
		BottomRight.x = EntPosView.x + EntSizeView.x*entityScaleFactor;	//	post 0.57
		BottomRight.y = EntPosView.y + EntSizeView.y*entityScaleFactor;	//	post 0.57
		TopRight.x = BottomRight.x;
		TopRight.y = TopLeft.y;
		BottomLeft.x = TopLeft.x;
		BottomLeft.y = BottomRight.y;

		pDC->MoveTo (TopLeft);
		pDC->LineTo (BottomRight);
		pDC->MoveTo (TopRight);
		pDC->LineTo (BottomLeft);

		// g3dc experiment draws a square around entity.
		// do not change pen characteristics in this function
		// it is called by all template, brush and entity types
		pDC->LineTo(TopLeft);
		pDC->LineTo(TopRight);
		pDC->LineTo(BottomRight);
		pDC->LineTo(BottomLeft);
		// end g3dc experiment
	}

	// and then show the aiming arrow and arc stuff...
	if( bShowUI )
	{
		POINT		ArcTopLeft, ArcBottomRight;
		POINT		ptDirSlope ;		// Slope of the "Direction" line
		POINT		ptRotationPoint ;	// Point near end of "Direction" line we rotate to get arrowhead points
		POINT		ptRelRotatePoint ;	// Rotation points about zero
		POINT		ptPlus45 ;			// Final Arrowhead point
		POINT		ptMinus45 ;			// Final Arrowhead point
		POINT		ptStart ;			// Start point for Arc
		POINT		ptEnd ;				// End point of Arc
		float		fPercentIntoLine ;	// Distance into Direction line for rotation point
		float		fDirLength ;		// Direction line length
		float		fEntityLength ;		// Entity length
		float		fAngleToTarget ;	// Radians of arc midpoint
		geFloat		fRadius ;
		geVec3d		Angles ;
		geXForm3d	Xfm ;
		geVec3d		VecTarg ;
		float		fArc ;
		POINT		LineEndView;
		geBoolean	bUIAvailable ;

		// Get the Radius and the Angle  ONE of these must be present to show UI
		bUIAvailable = GE_FALSE ;
		if( pEnt->GetRadius( &fRadius, pEntityDefs ) == GE_FALSE )
			fRadius = 100.0f ;
		else
			bUIAvailable = GE_TRUE ;

		if( pEnt->GetAngles( &Angles, pEntityDefs ) == GE_FALSE )
			geVec3d_Clear( &Angles ) ;
		else
			bUIAvailable = GE_TRUE ;

		if( bUIAvailable == GE_FALSE )
			return ;

		// The camera angles are given in camera coordinates rather than
		// world coordinates (don't ask).
		// So we convert them here.
		if (pEnt->IsCamera ())
		{
			geVec3d_Set(&Angles, Angles.Z, (-Angles.Y-M_PI/2.0f), Angles.X);
		}

		geXForm3d_SetEulerAngles( &Xfm, &Angles ) ;
		geVec3d_Set( &VecTarg, fRadius, 0.0f, 0.0f ) ;
		geXForm3d_Transform( &Xfm, &VecTarg, &VecTarg ) ;
		geVec3d_Add( &(pEnt->mOrigin), &VecTarg, &VecTarg ) ;

		LineEndView = Render_OrthoWorldToView ( v, &VecTarg );

		// Draw to the end point
		pDC->MoveTo( EntPosView ) ;
		pDC->LineTo( LineEndView ) ;

		ptDirSlope.x = LineEndView.x - EntPosView.x ;	// Slope of Direction line
		ptDirSlope.y = LineEndView.y - EntPosView.y ;

		fDirLength = (float)sqrt( (ptDirSlope.x*ptDirSlope.x) + (ptDirSlope.y*ptDirSlope.y)) ;	// Length of Direction line
		fEntityLength = (float)sqrt( (EntSizeView.x*EntSizeView.x)+(EntSizeView.y*EntSizeView.y)) ;
		fEntityLength *= 2 ;	// Arrow 2x entity size
		fPercentIntoLine = 1.0f - (fEntityLength / fDirLength ) ;
		ptRotationPoint.x = (long)(ptDirSlope.x * fPercentIntoLine) ;
		ptRotationPoint.y = (long)(ptDirSlope.y * fPercentIntoLine) ;
		ptRotationPoint.x += EntPosView.x ;
		ptRotationPoint.y += EntPosView.y ;

		ptRelRotatePoint.x = ptRotationPoint.x - LineEndView.x ;
		ptRelRotatePoint.y = ptRotationPoint.y - LineEndView.y ;

		ptPlus45.x = (long)(ptRelRotatePoint.x * COS45 - ptRelRotatePoint.y * SIN45 ) ;
		ptPlus45.y = (long)(ptRelRotatePoint.y * COS45 + ptRelRotatePoint.x * SIN45 ) ;
		ptMinus45.x = (long)(ptRelRotatePoint.x * MCOS45 - ptRelRotatePoint.y * MSIN45 ) ;
		ptMinus45.y = (long)(ptRelRotatePoint.y * MCOS45 + ptRelRotatePoint.x * MSIN45 ) ;

		ptPlus45.x += LineEndView.x ;
		ptPlus45.y += LineEndView.y ;
		ptMinus45.x += LineEndView.x ;
		ptMinus45.y += LineEndView.y ;

		pDC->LineTo( ptPlus45 ) ;
		pDC->LineTo( ptMinus45 ) ;
		pDC->LineTo( LineEndView ) ;

		if( pEnt->GetArc( &fArc, pEntityDefs ) == GE_FALSE )
		{
			fArc = 0.0f ;	// All Directions
		}
		if( fArc != 0.0f )			// Draw the arc
		{
			fArc = 2*M_PI - fArc;
			fArc /= 2.0f ;	// We need half the angle
			EntSizeView.x *= 3;
			EntSizeView.y *= 3;
			EntWidthHeight.x *= 3 ;
			EntWidthHeight.y *= 3 ;
			// Arc BB is an enlarged Entity BB
			ArcTopLeft.x		= EntPosView.x - EntSizeView.x;
			ArcTopLeft.y		= EntPosView.y - EntSizeView.y;
			ArcBottomRight.x	= EntPosView.x + EntSizeView.x;
			ArcBottomRight.y	= EntPosView.y + EntSizeView.y;

			fAngleToTarget = (float)atan2( ptDirSlope.y, ptDirSlope.x ) ;	// Angle line leaves
			fAngleToTarget += M_PI ;	// The other side is where the angle starts

			ptStart.x = (long)((EntWidthHeight.x) * cos( fAngleToTarget + fArc )) ;
			ptStart.y = (long)((EntWidthHeight.y) * sin( fAngleToTarget + fArc )) ;
			ptEnd.x = (long)((EntWidthHeight.x) * cos( fAngleToTarget - fArc )) ;
			ptEnd.y = (long)((EntWidthHeight.y) * sin( fAngleToTarget - fArc )) ;
			ptStart.x += EntPosView.x ;
			ptStart.y += EntPosView.y ;
			ptEnd.x += EntPosView.x ;
			ptEnd.y += EntPosView.y ;

			// If Start and end point are different
			if( !(ptStart.x == ptEnd.x && ptStart.y == ptEnd.y) )
			{
				pDC->Arc
				(
					ArcTopLeft.x, ArcTopLeft.y, ArcBottomRight.x, ArcBottomRight.y,
					ptStart.x, ptStart.y,
					ptEnd.x, ptEnd.y
				);
			}

			// Draw the two rays out the same distance as the Direction
			ptStart.x = (long)((fDirLength) * cos( fAngleToTarget + fArc )) ;
			ptStart.y = (long)((fDirLength) * sin( fAngleToTarget + fArc )) ;
			ptStart.x += EntPosView.x ;
			ptStart.y += EntPosView.y ;
			pDC->MoveTo( EntPosView ) ;
			pDC->LineTo( ptStart ) ;

			ptEnd.x = (long)((fDirLength) * cos( fAngleToTarget - fArc )) ;
			ptEnd.y = (long)((fDirLength) * sin( fAngleToTarget - fArc )) ;
			ptEnd.x += EntPosView.x ;
			ptEnd.y += EntPosView.y ;
			pDC->MoveTo( EntPosView ) ;
			pDC->LineTo( ptEnd ) ;
		}// Arc for this entity exists
	}
}/* fdocDrawEntity */




static geBoolean fdocBrushCSGCallback (const Brush *pBrush, void *lParam)
{
	CGEditProDoc *pDoc = (CGEditProDoc *)lParam;

	return (pDoc->BrushIsVisible (pBrush) && (!Brush_IsHint(pBrush)) && (!Brush_IsClip(pBrush)));
}


typedef struct
{
	BOOL Select;
	int WhichGroup;
	CGEditProDoc *pDoc;
} BrushSelectCallbackData;

// ::BrushList_CB Enumeration function to select/deselect brushes
static geBoolean BrushSelect( Brush *pBrush, void *lParam)
{
	BrushSelectCallbackData *pData = NULL;

	pData = (BrushSelectCallbackData *)lParam;

	if (pData)
	{
	// select/deselect all group brushes
	if( Brush_GetGroupId( pBrush ) == pData->WhichGroup )
	{
		if( pData->Select )
		{	// add this brush to the selected list
			SelBrushList_Add (pData->pDoc->DocVarsGetByValue().m_pSelBrushes, pBrush);
		}
		else
		{
			// remove this brush from the selection list
			SelBrushList_Remove (pData->pDoc->DocVarsGetByValue().m_pSelBrushes, pBrush);
		}
	}
	return GE_TRUE ;	// Continue enumeration
	}
	return GE_TRUE;
}/* ::BrushSelect */

// ::EntityList_CB Enumeration function to select/deselect brushes
static geBoolean EntitySelect( CEntity& Entity, void * lParam )
{
	BrushSelectCallbackData *pData = NULL;
	CGEditProDoc *pDoc = NULL;

	pData = (BrushSelectCallbackData *)lParam;
	pDoc = pData->pDoc;

	if( Entity.GetGroupId () == pData->WhichGroup )
	{
		if( pData->Select )
		{
			pDoc->SelectEntity( &Entity );
		}
		else
		{
			pDoc->DeselectEntity( &Entity );
		}
	}
	return GE_TRUE ;	// Continue enumeration
}/* ::EntitySelect */

#define fdoc_SHOW_ALL_GROUPS -1

typedef struct tagBrushDrawData
{
	const Box3d	*	pViewBox ;
	CDC *		pDC ;
	ViewVars *	v ;
	int GroupId;
	CGEditProDoc *pDoc;
	BrushFlagTest FlagTest;
	uint32		Color;
} BrushDrawData ;

static geBoolean BrushDraw( Brush *pBrush, void *lParam)
{
	BrushDrawData * pData = NULL;
	pData =  (BrushDrawData*)lParam ;
	CGEditProDoc * pDoc = pData->pDoc;

	if (pData)
	{

	if ((pData->GroupId == fdoc_SHOW_ALL_GROUPS) || (Brush_GetGroupId (pBrush) == pData->GroupId))
	{
		if ((pData->FlagTest == NULL) || pData->FlagTest (pBrush))
		{
			if (pDoc->fdocShowBrush (pBrush, pData->pViewBox))
			{
				Render_RenderBrushFacesOrtho(pData->v, pBrush, pData->pDC->m_hDC);
			}
		}
	}
	return GE_TRUE ;
	}
	return GE_TRUE;
}/* ::BrushDraw */


static geBoolean BrushDrawSelFacesOrtho(Brush *pBrush, void *lParam)
{
	BrushDrawData	*pData;

	pData	=(BrushDrawData *)lParam;

	Render_RenderBrushSelFacesOrtho(pData->v, pBrush, pData->pDC->m_hDC);

	return	GE_TRUE;
}

static geBoolean BrushDrawSheetFacesOrtho(Brush *pBrush, void *lParam)
{
	BrushDrawData	*pData;

	if(Brush_IsSheet(pBrush))
	{
		pData	=(BrushDrawData *)lParam;

		Render_RenderBrushSheetFacesOrtho(pData->v, pBrush, pData->pDC->m_hDC);
	}
	return	GE_TRUE;
}

static geBoolean EntityDraw( CEntity& Entity, void * lParam )
{
	BrushDrawData *pData = NULL;

	pData = (BrushDrawData *)lParam;

	if( Entity.GetGroupId () != pData->GroupId )
		return GE_TRUE ;

	if ( (Entity.IsSelected() == GE_FALSE ) && pData->pDoc->EntityIsVisible( &Entity ) )
	{
		fdocDrawEntity (&Entity, pData->v, pData->pDC, pData->pDoc->m_pLevelMgr->GetEntityDefs (&pData->pDoc->DocVarsGetByValue().m_Level), GE_FALSE );
	}
	return GE_TRUE ;
}/* ::EntityDraw */

static geBoolean	BrushDrawWire3dCB(Brush *pBrush, void *lParam)
{
	BrushDrawData *pData = NULL;
	pData = (BrushDrawData *)lParam;
	if (pData)
	{
	CGEditProDoc *pDoc = NULL;
	pDoc = pData->pDoc;

	if (pDoc)
	{
	if ((pData->GroupId == fdoc_SHOW_ALL_GROUPS) || (Brush_GetGroupId (pBrush) == pData->GroupId))
	{
		if ((pData->FlagTest == NULL) || pData->FlagTest (pBrush))
		{
			if (pDoc->BrushIsVisible (pBrush))
			{
				Render_RenderBrushFaces (pData->v, pBrush, pData->Color);
			}
		}
	}
	return	GE_TRUE ;
	}
	return GE_TRUE;
	}
	return GE_TRUE;
}

static geBoolean	BrushDrawWire3dZBufferCB(Brush *pBrush, void *lParam)
{
	BrushDrawData *pData = NULL;
	pData = (BrushDrawData *)lParam;
	if (pData)
	{
		CGEditProDoc *pDoc = pData->pDoc;
		if (pDoc)
		{
			if ((pData->GroupId == fdoc_SHOW_ALL_GROUPS) || (Brush_GetGroupId (pBrush) == pData->GroupId))
			{
				if ((pData->FlagTest == NULL) || pData->FlagTest (pBrush))
				{
					if (pDoc->BrushIsVisible (pBrush))
					{
						Render_RenderBrushFacesZBuffer(pData->v, pBrush, pData->Color);
					}
				}
			}
			return	GE_TRUE;
		}
		return	GE_TRUE;
	}
	return GE_TRUE;
}

static geBoolean	BrushDrawWireSel3dCB(Brush *b, void *lParam)
{
	BrushDrawData *pData;

	pData = (BrushDrawData *)lParam;
	Render_RenderBrushSelFaces(pData->v, b, pData->Color);
	return	GE_TRUE ;
}

// END ENUMERATION FUNCTIONS



static geBoolean fdocSetEntityVisibility (CEntity &Ent, void *lParam)
{
	EntityViewEntry *pEntry = (EntityViewEntry *)lParam;

	if (Ent.GetClassname () == pEntry->pName)
	{
		Ent.SetVisible (pEntry->IsVisible);
	}
	return GE_TRUE;
}



struct fdocFaceScales
{
	float DrawScale;
	float LightmapScale;
};

static geBoolean fdocSetFaceScales (Face *pFace, void *lParam)
{
	fdocFaceScales *pScales = (fdocFaceScales *)lParam;

	Face_SetTextureScale (pFace, pScales->DrawScale, pScales->DrawScale);
	Face_SetLightScale (pFace, pScales->LightmapScale, pScales->LightmapScale);
	return GE_TRUE;
}

typedef struct
{
	CGEditProDoc *pDoc;
	const char *TexName;
} BrushTexSetData;

static geBoolean	BrushTexSetCB (Brush *b, void *lParam)
{
	int			i;
	BrushTexSetData *pData;

	pData = (BrushTexSetData *)lParam;

	Brush_SetName(b, pData->TexName);
	char const * const BrushName = Brush_GetName (b);
	const int NumFaces = Brush_GetNumFaces (b);

	//copy face TexInfos
	for(i=0;i < NumFaces;i++)
	{
		Face	*f	=Brush_GetFace(b, i);
		WadFileEntry *pbmp = NULL;
		CGEditProDocVars	thisFunctionDocVars = 	pData->pDoc->DocVarsGetByValue();

		Face_SetTextureName(f, BrushName);
//		Face_SetTextureDibId(f, pData->pDoc->m_pLevelMgr->GetDibId (&pData->pDoc->DocVarsGetByValue().m_Level, BrushName));
//		pbmp = pData->pDoc->m_pLevelMgr->GetWadBitmap (&pData->pDoc->DocVarsGetByValue().m_Level, BrushName);
		Face_SetTextureDibId(f, pData->pDoc->m_pLevelMgr->GetDibId (&thisFunctionDocVars.m_Level, BrushName));
		pbmp = pData->pDoc->m_pLevelMgr->GetWadBitmap (&thisFunctionDocVars.m_Level, BrushName);
		if (pbmp != NULL)
		{
			Face_SetTextureSize (f, pbmp->Width, pbmp->Height);
		}
	}
	Brush_SetFaceListDirty(b);
	return GE_TRUE ;
}



static geBoolean fdocBrushNotDetail (const Brush *b)
{
	return !Brush_IsDetail (b);
}

static geBoolean fdocBrushIsSubtract (const Brush *b)
{
	return (Brush_IsSubtract (b) && !Brush_IsHollowCut (b));
}


static geFloat PointToLineDist
	(
	  POINT const *ptFrom,
	  POINT const *ptLine1,
	  POINT const *ptLine2
	)
{
	geFloat xkj, ykj;
	geFloat xlk, ylk;
	geFloat denom;
	geFloat dist;

	xkj = (geFloat)(ptLine1->x - ptFrom->x);
	ykj = (geFloat)(ptLine1->y - ptFrom->y);
	xlk = (geFloat)(ptLine2->x - ptLine1->x);
	ylk = (geFloat)(ptLine2->y - ptLine1->y);
	denom = (xlk*xlk) + (ylk*ylk);
	if (denom < .0005f)
	{
		// segment ends coincide
		dist = xkj*xkj + ykj*ykj;
	}
	else
	{
		geFloat t;
		geFloat xfac, yfac;

		t = -(xkj*xlk + ykj*ylk)/denom;
		t = max (t, 0.0f);
		t = min (t, 1.0f);
		xfac = xkj + t*xlk;
		yfac = ykj + t*ylk;
		dist = xfac*xfac + yfac*yfac;
	}
	return (geFloat)sqrt (dist);
}


typedef struct FindClosestInfoTag
{
	CGEditProDoc	*pDoc;
	ViewVars	*v;
	Brush		**ppFoundBrush;
	geFloat		*pMinEdgeDist;
	const POINT	*ptFrom;
} FindClosestInfo;

static geBoolean	FindClosestBrushCB(Brush *pBrush, void *pVoid)
{
	FindClosestInfo	*fci	=(FindClosestInfo *)pVoid;

	if (pBrush == NULL)	//	post 0.55
	{
		MessageBox(NULL, "Brush == NULL", "GEditProDoc FindClosestBrushCB", MB_OK);
		return GE_FALSE;
	}

	if(fci->pDoc->BrushIsVisible(pBrush))
	{
		// for each face...
		for (int iFace = 0; iFace < Brush_GetNumFaces(pBrush); ++iFace)
		{
			POINT			pt1, pt2;
			Face			*pFace		=Brush_GetFace(pBrush, iFace);
			const geVec3d	*FacePoints	=Face_GetPoints(pFace);
			int				NumPoints	=Face_GetNumPoints(pFace);

			// Starting with the edge formed by the last point and the first point,
			// determine distance from mouse cursor pos to the edge.
			pt1 = Render_OrthoWorldToView(fci->v, &FacePoints[NumPoints-1]);
			for(int iPoint = 0; iPoint < NumPoints; ++iPoint)
			{
				geFloat Dist;

				pt2 = Render_OrthoWorldToView (fci->v, &FacePoints[iPoint]);
				Dist = PointToLineDist (fci->ptFrom, &pt1, &pt2);
				if (Dist < *fci->pMinEdgeDist)
				{
					*fci->pMinEdgeDist = Dist;
					*fci->ppFoundBrush = pBrush;
				}
				pt1 = pt2;	// next edge...
			}
		}
	}
	return GE_TRUE ;
}


static geBoolean fdocDeselectEntity (CEntity &Ent, void *lParam)
{
	CGEditProDoc *pDoc = (CGEditProDoc *)lParam;

	pDoc->DeselectEntity (&Ent);
	return GE_TRUE;
}



static geBoolean fdocSelectEntity (CEntity &Ent, void *lParam)
{
	CGEditProDoc *pDoc = (CGEditProDoc *)lParam;

	Ent.DeSelect ();
	pDoc->SelectEntity (&Ent);
	return GE_TRUE;
}

static geBoolean fdocSelectBrush (Brush *pBrush, void *lParam)
{
	CGEditProDoc *pDoc = (CGEditProDoc *)lParam;

	SelBrushList_Add (pDoc->DocVarsGetByValue().m_pSelBrushes, pBrush);
	return GE_TRUE;
}



typedef struct
{
	geVec3d	vp, wp;
	geFloat	MinBDist, CurDist;
	Face	*HitFace, *CurFace;
	Brush	*m_pCurBrush;
	CGEditProDoc *pDoc;
} SelectBrush3DCBData;

static geBoolean SelectBrush3DCB(Brush *b, void * lParam)
{
	SelectBrush3DCBData	*pData	=(SelectBrush3DCBData *)lParam;

	if (pData->pDoc->BrushIsVisible (b))
	{
		if(!(Brush_IsSubtract(b)))
		{
			Face *HitFace;
			float CurDist;

			HitFace	=Brush_RayCast(b, &pData->vp, &pData->wp, &CurDist);
			if (HitFace != NULL)
			{
				pData->HitFace = HitFace;
				if(CurDist < pData->MinBDist)
				{
					pData->CurDist = CurDist;
					pData->MinBDist	=pData->CurDist;
					pData->m_pCurBrush	=b;
					pData->CurFace	=pData->HitFace;
				}
			}
		}
	}
	return	GE_TRUE;
}


struct FaceSearchCallbackData
{
	const Plane *pFacePlane;
	geBoolean Found;
	geVec3d ImpactPoint;
	Face *pFoundFace;
	Brush *pFoundBrush;
};
static geBoolean fdocPointOnFace
	(
	  const Face *pFace,
	  const geVec3d *pPoint
	)
{
	int pt1, pt2;
	int iVert, NumVerts;
	geVec3d const *Verts;

	NumVerts = Face_GetNumPoints (pFace);
	Verts = Face_GetPoints (pFace);
	pt1 = NumVerts-1;
	pt2 = 0;
	for (iVert = 0; iVert < NumVerts; ++iVert)
	{
		geFloat Dot;
		geVec3d VertToPoint, VertToVert;

		geVec3d_Subtract (&Verts[pt2], &Verts[pt1], &VertToVert);
		geVec3d_Subtract (pPoint, &Verts[pt1], &VertToPoint);

		Dot = geVec3d_DotProduct (&VertToVert, &VertToPoint);
		if (Dot < 0)
		{
			return GE_FALSE;
		}
		pt1 = pt2;
		++pt2;
	}
	return GE_TRUE;
}

static geBoolean fdocPointInBrushFace
	(
	  Brush *pBrush,
	  const Plane *pPlane,
	  const geVec3d *pPoint,
	  Face **ppFoundFace
	)
{
	int NumFaces;
	int i;

	NumFaces = Brush_GetNumFaces (pBrush);
	for (i = 0; i < NumFaces; ++i)
	{
		Face *pFace;
		const Plane *pFacePlane;

		pFace = Brush_GetFace (pBrush, i);
		pFacePlane = Face_GetPlane (pFace);

//	******************	post 0.57 for debugging purposes
		geBoolean comparison = geVec3d_Compare(&pPlane->Normal, &pFacePlane->Normal, 0.01f);
		geFloat fComparison = fabs (pPlane->Dist - pFacePlane->Dist);
		comparison; fComparison;
		//	the problem is that the pPlane->Dist is not reported accurately
		//	when INSIDE the brush we are trying to select
//	******************
		if(geVec3d_Compare(&pPlane->Normal, &pFacePlane->Normal, 0.01f) &&
		   (fabs (pPlane->Dist - pFacePlane->Dist) < 0.01f))
		{
			// if the face plane matches the passed plane
			// then see if the ImpactPoint is within the bounds of the face
			if (::fdocPointOnFace (pFace, pPoint))
			{
				*ppFoundFace = pFace;
				return GE_TRUE;
			}
		}
	}
	return GE_FALSE;
}

static geBoolean fdocFindLeafFace (Brush *pBrush, void *lParam)
{
	if (!Brush_IsSubtract (pBrush))		// don't want cut brushes
	{
		FaceSearchCallbackData *pData;

		pData = (FaceSearchCallbackData *)lParam;
		if (::fdocPointInBrushFace (pBrush, pData->pFacePlane, &(pData->ImpactPoint), &(pData->pFoundFace)))
		{
			pData->pFoundBrush = pBrush;
			pData->Found = GE_TRUE;
			return GE_FALSE;	// found it, so quit...
		}
	}
	return GE_TRUE;
}

static geBoolean fdocFindCutFace (Brush *pBrush, void *lParam)
{
	if (Brush_IsSubtract (pBrush))		// want only cut brushes
	{
		FaceSearchCallbackData *pData;
		Plane PlaneInv;

		pData = (FaceSearchCallbackData *)lParam;
#pragma message ("Need to reverse plane?")
		PlaneInv = *pData->pFacePlane;
		geVec3d_Inverse (&PlaneInv.Normal);
		PlaneInv.Dist = -PlaneInv.Dist;

		pData = (FaceSearchCallbackData *)lParam;
		if (::fdocPointInBrushFace (pBrush, &PlaneInv, &(pData->ImpactPoint), &(pData->pFoundFace)))
		{
			pData->pFoundBrush = pBrush;
			pData->Found = GE_TRUE;
			return GE_FALSE;	// found it, so quit...
		}
	}
	return GE_TRUE;
}

typedef struct
{
	CGEditProDoc	*pDoc;
	int			CurId;
	Node		**pTree;
	geBoolean	bAddFlocking;	//set for adding flocking brushes
} AddBrushCBData;

static geBoolean AddBrushToBspCB(Brush *pBrush, void * lParam)
{
	AddBrushCBData *pData;
	Node **tree;
	CGEditProDoc *pDoc;

	pData = (AddBrushCBData *)lParam;
	tree = pData->pTree;
	pDoc = pData->pDoc;

	assert(tree);

	if(pData->bAddFlocking ^ Brush_IsFlocking(pBrush))
	{
		if((Brush_GetModelId(pBrush)==pData->CurId) && !Brush_IsSubtract(pBrush))
		{
			if(pDoc)
			{
				if(pDoc->BrushIsVisible(pBrush)
					&& (!Brush_IsClip(pBrush)) && (!Brush_IsHint(pBrush)))
				{
					*tree	=Node_AddBrushToTree(*tree, pBrush);
				}
			}
			else
			{
				*tree	=Node_AddBrushToTree(*tree, pBrush);
			}
		}
	}
	return	GE_TRUE;
}



static geBoolean	IsBrushNotClipOrHint(const Brush *b)
{
	assert(b != NULL);

	return	(Brush_IsHint(b) || Brush_IsClip(b))? GE_FALSE : GE_TRUE;
}


static void DrawEntity (CEntity *pEnt, ViewVars *v, const EntityTable *pEntityDefs)
{
	const geBitmap *pBitmap;

	pBitmap = pEnt->GetBitmapPtr (pEntityDefs);

	Render_3DTextureZBuffer(v, &pEnt->mOrigin, pBitmap);
}



static geBoolean fdocBrushTextureScaleCallback (Brush *pBrush, void *lParam)
{
	const geFloat *pScaleVal = (geFloat *)lParam;

	Brush_SetTextureScale (pBrush, *pScaleVal);
	return GE_TRUE;
}


#pragma warning (disable:4100)
static geBoolean	ResetSelectedFacesCB(Brush *b, void *pVoid)
{
	int	i;

	for(i=0;i < Brush_GetNumFaces(b);i++)
	{
		Face	*pFace;

		pFace	=Brush_GetFace(b, i);
		Face_SetSelected(pFace, GE_FALSE);
	}
	return GE_TRUE ;
}
#pragma warning (default:4100)



static geBoolean FindSelectedFaceCB (Brush *b, void *lParam)
{
	Face **ppSelectedFace = (Face **)lParam;
	int i;

	for (i = 0; i < Brush_GetNumFaces (b); ++i)
	{
		Face *pFace;

		pFace = Brush_GetFace (b, i);
		if (Face_IsSelected (pFace))
		{
			*ppSelectedFace = pFace;
			return GE_FALSE;
		}
	}
	return GE_TRUE;
}


static geBoolean SelAllBrushFaces (Brush *pBrush, void *lParam)
{
	int iFace, nFaces;
	CGEditProDoc *pDoc = (CGEditProDoc *)lParam;

	if (pDoc)
	{

	nFaces = Brush_GetNumFaces (pBrush);
	for (iFace = 0; iFace < nFaces; ++iFace)
	{
		Face *pFace = NULL;

		pFace = Brush_GetFace (pBrush, iFace);
		if (pFace)
		{
		Face_SetSelected (pFace, GE_TRUE);

		SelFaceList *pList = NULL;
		pList = pDoc->DocVarsGetPointer()->m_pSelFaces;
		if (pList)
		{
			SelFaceList_Add (pList, pFace);
		}
		}
	}
	return GE_TRUE;
	}
	return GE_TRUE;
}

static geBoolean fdocSelectBrushesFromFaces (Brush *pBrush, void *lParam)
{
	CGEditProDoc *pDoc = (CGEditProDoc *)lParam;
	int iFace, nFaces;

	// if any of the brush's faces is selected, then select the brush.
	nFaces = Brush_GetNumFaces (pBrush);
	for (iFace = 0; iFace < nFaces; ++iFace)
	{
		Face *pFace;

		pFace = Brush_GetFace (pBrush, iFace);
		if (Face_IsSelected (pFace))
		{
			pDoc->DoBrushSelection (pBrush, brushSelAlways);
			break;
		}
	}
	return GE_TRUE;
}



static char *SkyFaceNames[6] =
	{"SkyLeft", "SkyRight", "SkyTop", "SkyBottom", "SkyFront", "SkyBack"};

typedef struct
{
	geBoolean SuppressHidden;
	geBoolean VisDetail;
	CGEditProDoc *pDoc;
	int BrushCount;
	FILE *f;
} fdocBrushEnumData;


typedef struct
{
	GroupListType *Groups;
	geBoolean AllGood;
} fdocBrushValidateData;


static geBoolean fdocBrushCountCallback(Brush *b, void * pVoid)
{
	fdocBrushEnumData *pData;

	pData = (fdocBrushEnumData *)pVoid;

	if ((Brush_GetModelId(b) == 0) && (!Brush_IsSubtract(b)) &&
	    ((pData->SuppressHidden == GE_FALSE) || (pData->pDoc->BrushIsVisible (b))))
	{
		++(pData->BrushCount);
	}
	return	GE_TRUE;
}


static geBoolean fdocBrushWriteCallback(Brush *b, void * pVoid)
{
	fdocBrushEnumData *pData;

	pData = (fdocBrushEnumData *)pVoid;

	if ((Brush_GetModelId(b) == 0) && (!Brush_IsSubtract(b)) &&
	    ((pData->SuppressHidden == GE_FALSE) || (pData->pDoc->BrushIsVisible (b))))
	{
		Brush_WriteToMap (b, pData->f, pData->VisDetail);
	}
	return	GE_TRUE;
}

typedef struct
{
	FILE *f;
	ModelList *Models;
	CEntityArray *Entities;
	CGEditProDoc	*pDoc;
} fdocEntityToMapData;

static geBoolean fdocEntityToMap (CEntity &Ent, void *lParam)
{
	fdocEntityToMapData *pData;

	pData = (fdocEntityToMapData *)lParam;
	if (!Ent.IsCamera ())
	{
		CompileParamsType *CompileParams = pData->pDoc->m_pLevelMgr->GetCompileParams (&pData->pDoc->DocVarsGetByValue().m_Level);
		if(CompileParams->SuppressHidden)
		{
			if(pData->pDoc->EntityIsVisible(&Ent))
			{
				Ent.WriteToMap (pData->f, pData->Models, pData->Entities, pData->pDoc->m_pLevelMgr->GetEntityDefs (&pData->pDoc->DocVarsGetByValue().m_Level));
			}
		}
		else
		{
			Ent.WriteToMap (pData->f, pData->Models, pData->Entities, pData->pDoc->m_pLevelMgr->GetEntityDefs (&pData->pDoc->DocVarsGetByValue().m_Level));
		}
	}
	return GE_TRUE;
}

typedef geBoolean (*EntityCount_CB)(const class CEntity *);
typedef struct fdocEntityCountDataTag
{
	int				Count;
	CGEditProDoc		*pDoc;
} fdocEntityCountData;

static geBoolean fdocCountNonCameraEntities (CEntity &Ent, void *lParam)
{
	fdocEntityCountData	*ecnt	=(fdocEntityCountData *)lParam;
	if(!Ent.IsCamera())
	{
		CompileParamsType *CompileParams = ecnt->pDoc->m_pLevelMgr->GetCompileParams (&ecnt->pDoc->DocVarsGetByValue().m_Level);
		if(CompileParams->SuppressHidden)
		{
			if(ecnt->pDoc->EntityIsVisible(&Ent))
			{
				ecnt->Count++;
			}
		}
		else
		{
			ecnt->Count++;
		}
	}
	return GE_TRUE;
}



struct ScaleEntityInfo
{
	const EntityTable *pEntityDefs;
	float ScaleValue;
};

static geBoolean fdocScaleEntityCallback (CEntity &Ent, void *lParam)
{
	ScaleEntityInfo *pInfo = (ScaleEntityInfo *)lParam;

	Ent.Scale (pInfo->ScaleValue, pInfo->pEntityDefs);
	return GE_TRUE;
}


typedef struct
{
	int ModelId;
	BOOL Select;
	CGEditProDoc *pDoc;
} fdocBrushSelectData;

static geBoolean fdocSelectBrushCallback (Brush *pBrush, void *lParam)
{
	fdocBrushSelectData *pData;

	pData = (fdocBrushSelectData *)lParam;
	if (Brush_GetModelId (pBrush) == pData->ModelId)
	{
		if (pData->Select)
		{
			SelBrushList_Add (pData->pDoc->DocVarsGetByValue().m_pSelBrushes, pBrush);
		}
		else
		{
			SelBrushList_Remove (pData->pDoc->DocVarsGetByValue().m_pSelBrushes, pBrush);
		}
	}
	return GE_TRUE;
}



typedef struct
{
	GroupListType *Groups;
	int CurrentGroup;
} fdocAddEntityData;






static geBoolean fdocRemoveEntityFromGroupCallback (CEntity &Ent, void *lParam)
{
	fdocAddEntityData *pData;

	pData = (fdocAddEntityData *)lParam;
	if (Ent.IsSelected () && (Ent.GetGroupId () == pData->CurrentGroup))
	{
		Group_RemoveEntity (pData->Groups, pData->CurrentGroup, &Ent);
	}
	return GE_TRUE;
}




static geBoolean GetObjectName (char *Name, char *Path, CGEditProDoc *pDoc)
{
	CKeyEditDlg *pEditDlg;
	CString ObjName;
	char ObjectsDir[MAX_PATH];
	char WorkPath[MAX_PATH];
	int rslt;

	strcpy (ObjectsDir, Prefs_GetObjectsDir (pDoc->GetPrefs ()));

	do
	{
		rslt = IDYES;
		pEditDlg = new CKeyEditDlg (AfxGetMainWnd (), "Enter object name", &ObjName);
		if (pEditDlg->DoModal () != IDOK)
		{
			return GE_FALSE;
		}
		// see if an object of this name already exists
		::FilePath_AppendName (ObjectsDir, ObjName, WorkPath);
		::FilePath_SetExt (WorkPath, ".3dt", WorkPath);
		if (_access (WorkPath, 0) == 0)
		{
			static const char Prompt[] =
				"An object of that name already exists.\r"
				"Do you want to replace it?";

			rslt = AfxMessageBox (Prompt, MB_YESNOCANCEL);
		}
	} while (rslt == IDNO);

	if (rslt == IDYES)
	{
		strcpy (Path, WorkPath);
		::FilePath_GetName (Path, Name);
		return GE_TRUE;
	}
	else
	{
		return GE_FALSE;
	}
}

typedef struct
{
	CtLevel *NewLevel;
	CtLevel *OldLevel;
} AddPremadeEnumData;

static void fdocAddReferencedGroup (AddPremadeEnumData *pData, int GroupId)
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();

	if (pDoc)
	{

		if (pDoc->m_pLevelMgr->GetGroup (pData->NewLevel, GroupId) == NULL)
		{
			// group doesn't exist in the new level, so add it
			Group *OldGroup, *NewGroup;

			OldGroup = pDoc->m_pLevelMgr->GetGroup (pData->OldLevel, GroupId);
			NewGroup = Group_Clone (OldGroup);
			pDoc->m_pLevelMgr->AddGroup (pData->NewLevel, NewGroup);
		}
	}
}

static void fdocAddReferencedModel (AddPremadeEnumData *pData, int ModelId)
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();

	if (pDoc)
	{
		if ((ModelId != 0) && (pDoc->m_pLevelMgr->GetModel (pData->NewLevel, ModelId) == NULL))
		{
			Model *OldModel, *NewModel;

			OldModel = pDoc->m_pLevelMgr->GetModel (pData->OldLevel, ModelId);
			NewModel = Model_Clone (OldModel);
			pDoc->m_pLevelMgr->AddModel (pData->NewLevel, NewModel);
		}
	}
}

static geBoolean fdocAddSelectedEntities (CEntity &Ent, void *lParam)
{
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();

	if (pDoc)
	{
		AddPremadeEnumData *pData;

		pData = (AddPremadeEnumData *)lParam;

		if (Ent.IsSelected ())
		{
			pDoc->m_pLevelMgr->AddEntity (pData->NewLevel, Ent);
			::fdocAddReferencedGroup (pData, Ent.GetGroupId ());
		}
	}
	return GE_TRUE;
}


/*
typedef struct
{
	CGEditProDoc *pDoc;
	CtLevel *NewLevel;
} fdocAddPremadeData;
*/
typedef struct
{
	CGEditProDocVars *pDocVars;
	CGEditProDoc *pDoc;
	CtLevel *NewLevel;
} fdocAddPremadeData;



static geBoolean fdocAddPremadeEntity (CEntity &Ent, void *lParam)
{
	fdocAddPremadeData *pData = NULL;
	CEntityArray *Entities = NULL;
	CEntity *NewEnt = NULL;
	int Index;

	pData = (fdocAddPremadeData *)lParam;

	if (pData)
	{
		Index = pData->pDoc->m_pLevelMgr->AddEntity (&pData->pDocVars->m_Level, Ent);
//		Entities = pData->pDoc->m_pLevelMgr->GetEntities (&pData->pDocVars->m_Level);
//		if (Entities)
//		{
//			NewEnt = &((*Entities)[Index]);
//
//			if (NewEnt)
//			{
//				pData->pDoc->SelectEntity (NewEnt);
//			}
//		}
		return GE_TRUE;
	}
	return GE_FALSE;
}


typedef struct
{
	GroupListType *Groups;
	geBoolean AllGood;
	const EntityTable *pEntityTable;
} fdocEntityValidateData;

static geBoolean fdocValidateEntity (CEntity &Ent, void *lParam)
{
	fdocEntityValidateData *pData;
	int			nGroupId ;
	CString		cstr ;

	pData = (fdocEntityValidateData *)lParam;

	// Validate the entity type (Class)
	cstr = Ent.GetClassname() ;
	if (!EntityTable_IsValidEntityType (pData->pEntityTable, cstr))
	{
		cstr = Ent.GetName() ;
//		ConPrintf ("Entity %s: type not found.\n", cstr );	//	old gedit
		CGlobals::GetActiveDocument()->m_pMainFrame->ConPrintf("Entity %s: type not found.\n", cstr );	//	new g3dc
		pData->AllGood = GE_FALSE ;
	}

	nGroupId = Ent.GetGroupId() ;
	if (nGroupId != 0)
	{
		if (GroupList_IsValidId (pData->Groups, nGroupId) == FALSE)
		{
			cstr = Ent.GetName() ;
//			ConPrintf("Entity %s: Group %d missing--set to no group\n", cstr, nGroupId );	//	old gedit
			CGlobals::GetActiveDocument()->m_pMainFrame->ConPrintf("Entity %s: Group %d missing--set to no group\n", cstr, nGroupId );	// new g3dc
			Ent.SetGroupId( 0 ) ;
			pData->AllGood = GE_FALSE;
		}
	}

	return GE_TRUE;
}


// Quake 2 flag sets for importing
enum	Q2ContentsFlags
{
	CONTENTS_SOLID			=1,
	CONTENTS_WINDOW			=2,
	CONTENTS_AUX			=4,
	CONTENTS_LAVA			=8,
	CONTENTS_SLIME			=16,
	CONTENTS_WATER			=32,
	CONTENTS_MIST			=64,
	LAST_VISIBLE_CONTENTS	=64,
	CONTENTS_PLAYERCLIP		=0x10000,
	CONTENTS_MONSTERCLIP	=0x20000,
	CONTENTS_CURRENT_0		=0x40000,
	CONTENTS_CURRENT_90		=0x80000,
	CONTENTS_CURRENT_180	=0x100000,
	CONTENTS_CURRENT_270	=0x200000,
	CONTENTS_CURRENT_UP		=0x400000,
	CONTENTS_CURRENT_DOWN	=0x800000,
	CONTENTS_ORIGIN			=0x1000000,
	CONTENTS_MONSTER		=0x2000000,
	CONTENTS_DEADMONSTER	=0x4000000,
	CONTENTS_DETAIL			=0x8000000,
	CONTENTS_TRANSLUCENT	=0x10000000
};

enum	Q2SurfaceFlags
{
	SURF_LIGHT		=0x1,		// value will hold the light strength
	SURF_SLICK		=0x2,		// effects game physics
	SURF_SKY		=0x4,		// don't draw, but add to skybox
	SURF_WARP		=0x8,		// turbulent water warp
	SURF_TRANS33	=0x10,
	SURF_TRANS66	=0x20,
	SURF_FLOWING	=0x40,	// scroll towards angle
	SURF_NODRAW		=0x80	// don't bother referencing the texture
};

static geBoolean fdocCheckAddFace (FaceList **ppList, Face *f)
{
	if (FaceList_GetNumFaces (*ppList) >= FaceList_GetFaceLimit (*ppList))
	{
		if (!FaceList_Grow (ppList))
		{
			return GE_FALSE;
		}
	}
	FaceList_AddFace (*ppList, f);
	return GE_TRUE;
}


typedef geBoolean (*Brush_FaceCallback)(Face *pFace, void *lParam);

static geBoolean fdocUpdateFaceTextures (Face *pFace, void *lParam)
{
	CGEditProDoc *pDoc = (CGEditProDoc *)lParam;


	Face_SetTextureDibId (pFace, pDoc->m_pLevelMgr->GetDibId (&pDoc->DocVarsGetByValue().m_Level, Face_GetTextureName (pFace)));
// changed QD 12/03
	const WadFileEntry * const pbmp = pDoc->m_pLevelMgr->GetWadBitmap(&pDoc->DocVarsGetByValue().m_Level, Face_GetTextureName (pFace));
	if(pbmp)
		Face_SetTextureSize (pFace, pbmp->Width, pbmp->Height);
// end change
	return GE_TRUE;
}

static geBoolean fdocUpdateBrushFaceTextures (Brush *pBrush, void *pVoid)
{
	Brush_EnumFaces (pBrush, pVoid, ::fdocUpdateFaceTextures);
	return GE_TRUE;
}


static geBoolean fdocFindAnyFace (Brush *pBrush, void *lParam)
{
	FaceSearchCallbackData *pData = NULL;
	Plane PlaneInv;

	pData = (FaceSearchCallbackData *)lParam;

	if (pData)
	{
		PlaneInv = *pData->pFacePlane;
		geVec3d_Inverse (&PlaneInv.Normal);
		PlaneInv.Dist = -PlaneInv.Dist;

		pData = (FaceSearchCallbackData *)lParam;
		if (::fdocPointInBrushFace (pBrush, &PlaneInv, &(pData->ImpactPoint), &(pData->pFoundFace)))
		{
			pData->pFoundBrush = pBrush;
			pData->Found = GE_TRUE;
			return GE_FALSE;	// found it, so quit...
		}
	}
	return GE_TRUE;
}




















// Must be within this distance (in pixels) to select anything.
// (Yes, 10,000 is a big number.  Right now we want to select the closest thing,
// no matter how far away it is...)
#define MAX_PIXEL_SELECT_DIST (10000)
#define MIN_ENTITY_SELECT_DIST (8.0f)

// Maximum distance from entity in order for it to be selected.
// This is in world space coordinates and is used in rendered view only.
#define MAX_ENTITY_SELECT_DIST (16.0f)

IMPLEMENT_SERIAL(CGEditProDoc, CDocument, 0);
BEGIN_MESSAGE_MAP(CGEditProDoc, CDocument)
	//{{AFX_MSG_MAP(CGEditProDoc)
	ON_COMMAND(ID_TOOLS_USEGRID, OnToolsUsegrid)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_USEGRID, OnUpdateToolsUsegrid)
//	ON_COMMAND(ID_TOOLS_GRIDSETTINGS, OnToolsGridsettings)
//	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
//	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
//	ON_COMMAND(ID_ENTITIES_EDITOR, OnEntitiesEditor)
	ON_COMMAND(ID_ENTITIES_SHOW, OnEntitiesShow)
	ON_UPDATE_COMMAND_UI(ID_ENTITIES_SHOW, OnUpdateEntitiesShow)
	ON_COMMAND(ID_VIEW_SHOW_ALLGROUPS, OnViewShowAllGroups)
	ON_COMMAND(ID_VIEW_SHOW_CURRENTGROUP, OnViewShowCurrentGroup)
	ON_COMMAND(ID_VIEW_SHOW_VISIBLEGROUPS, OnViewShowVisibleGroups)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_VISIBLEGROUPS, OnUpdateViewShowVisibleGroups)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_ALLGROUPS, OnUpdateViewShowAllGroups)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_CURRENTGROUP, OnUpdateViewShowCurrentGroup)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_ADJUSTMENTMODE, OnUpdateBrushAdjustmentmode)
	ON_COMMAND(ID_BRUSH_SELECTED_COPYTOCURRENT, OnBrushSelectedCopytocurrent)
	ON_COMMAND(ID_BRUSH_SELECTED_DELETE, OnBrushSelectedDelete)
	ON_COMMAND(ID_GBSPNOWATER, OnRebuildBSPTree)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FACE_ADJUSTMENTMODE, OnUpdateFaceAdjustmentmode)
	ON_COMMAND(ID_CONSTRAINHOLLOWS, OnConstrainhollows)
	ON_UPDATE_COMMAND_UI(ID_CONSTRAINHOLLOWS, OnUpdateConstrainhollows)
	ON_COMMAND(ID_GENERALSELECT, OnGeneralselect)
	ON_UPDATE_COMMAND_UI(ID_GENERALSELECT, OnUpdateGeneralselect)
	ON_COMMAND(ID_THING_ATTRIBUTES, OnThingAttributes)
	ON_COMMAND(ID_BRUSH_SUBTRACTFROMWORLD, OnBrushSubtractfromworld)
	ON_COMMAND(ID_COMPILE, OnCompile)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_SUBTRACTFROMWORLD, OnUpdateBrushSubtractfromworld)
	ON_UPDATE_COMMAND_UI(ID_THING_ATTRIBUTES, OnUpdateThingAttributes)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_SHOWASSOCIATEDENTITY, OnUpdateToolsBrushShowassociatedentity)
	ON_UPDATE_COMMAND_UI(ID_ENTITIES_EDITOR, OnUpdateEntitiesEditor)
	ON_COMMAND(ID_NEW_LIB_OBJECT, OnNewLibObject)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_TOOLS_BRUSH_ADJUSTMENTMODE, OnToolsBrushAdjustmentmode)
	ON_COMMAND(ID_TOOLS_FACE_ADJUSTMENTMODE, OnToolsFaceAdjustmentmode)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_PRIMITIVES_CUBE, OnUpdateBrushPrimitives)
	ON_COMMAND(ID_BRUSH_PRIMITIVES_CUBE, OnBrushPrimitivesCube)
	ON_COMMAND(ID_BRUSH_PRIMITIVES_SPHEROID, OnBrushPrimitivesSpheroid)
	ON_COMMAND(ID_BRUSH_PRIMITIVES_CYLINDER, OnBrushPrimitivesCylinder)
	ON_COMMAND(ID_BRUSH_PRIMITIVES_STAIRCASE, OnBrushPrimitivesStaircase)
	ON_COMMAND(ID_VIEW_SHOW_CLIP, OnViewShowClip)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_CLIP, OnUpdateViewShowClip)
	ON_COMMAND(ID_VIEW_SHOW_DETAIL, OnViewShowDetail)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_DETAIL, OnUpdateViewShowDetail)
	ON_COMMAND(ID_VIEW_SHOW_HINT, OnViewShowHint)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_HINT, OnUpdateViewShowHint)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_BRUSH_PRIMITIVES_ARCH, OnBrushPrimitivesArch)
	ON_COMMAND(ID_BRUSH_PRIMITIVES_CONE, OnBrushPrimitivesCone)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_TOOLS_BRUSH_ATTRIBUTES, OnToolsBrushAttributes)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BRUSH_ATTRIBUTES, OnUpdateToolsBrushAttributes)
	ON_COMMAND(ID_TOOLS_FACE_ATTRIBUTES, OnToolsFaceAttributes)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FACE_ATTRIBUTES, OnUpdateToolsFaceAttributes)
	ON_COMMAND(ID_ENTITYVISIBILITY, OnEntityVisibility)
	ON_COMMAND(IDM_REBUILD_BSP, OnToggleRebuildBspMode)
	ON_UPDATE_COMMAND_UI(IDM_REBUILD_BSP, OnUpdateToggleRebuildBspMode)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_TOOLS_TOGGLEADJUSTMODE, OnToolsToggleadjustmode)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TOGGLEADJUSTMODE, OnUpdateToolsToggleadjustmode)
	ON_COMMAND(IDM_LEVELOPTIONS, OnLeveloptions)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_PRIMITIVES_SPHEROID, OnUpdateBrushPrimitives)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_PRIMITIVES_CYLINDER, OnUpdateBrushPrimitives)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_PRIMITIVES_STAIRCASE, OnUpdateBrushPrimitives)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_PRIMITIVES_ARCH, OnUpdateBrushPrimitives)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_PRIMITIVES_CONE, OnUpdateBrushPrimitives)
// changed QD Actors
	ON_COMMAND(ID_VIEW_SHOW_ACTORS, OnViewShowActors)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_ACTORS, OnUpdateViewShowActors)
// changed QD 11/03
	ON_COMMAND(ID_FILE_EXPORT3DT, OnFileExport3dtv1_32)
	ON_COMMAND(ID_FILE_EXPORT3DS, OnFileExport3ds)
// end change
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(  ID_BRUSH_SELECTED_DELETE, ID_BRUSH_SELECTED_COPYTOCURRENT, OnSelectedTypeCmdUI)
END_MESSAGE_MAP()


CGEditProDoc::CGEditProDoc() : CDocument (),
	m_bSelectLock (FALSE),
	m_bTempEnt (FALSE),
	m_dwSelState (NOSELECTIONS),
	m_iShowSelectedFaces (FALSE),
	m_iShowSelectedBrushes (FALSE),
	m_pVecLeakPoints (NULL),
	m_iNumLeakPoints (0),
	m_bLeakLoaded (FALSE),
	m_bShowLeak (TRUE),
	m_iIsNewDocument (1),
	m_iShowEntities (GE_TRUE),
	m_iCurTextureSelection (1),
	m_bShowClipBrushes (GE_TRUE),
	m_bShowDetailBrushes (GE_TRUE),
	m_bShowHintBrushes (GE_TRUE),
	m_bShowActors (GE_TRUE), // changed QD Actors
	m_pActiveViewFrame (NULL),
	m_pBrushAttributes (NULL),
	m_pFaceAttributes (NULL),
	m_pWorldBsp (NULL),
	m_iActiveView (-1),
	m_iCurrentEntity (-1),
	m_iModeTool (ID_TOOLS_TEMPLATE),
	m_AdjustMode (ADJUST_MODE_BRUSH),
	m_iCurrentTool (ID_TOOLS_BRUSH_SCALEBRUSH),
	m_iShowBrush (TRUE),
	m_bConstrainHollows (GE_TRUE),
	m_iCurrentBitmap (0),
	m_iNumSelEntities (0),
	m_iCurrentGroup (0),
	m_pTempShearTemplate (NULL),
	m_bPlaceObjectFlag (FALSE),
	m_fWorldScaleFactor(1.0f),
	m_fEntityScaleInViewFactor(1.0f),
	m_pTempBrush(NULL),
	m_bPrintingView(FALSE),
	m_bRedoAvailability(false),
	m_bUndoAvailability(false),
	m_pLevelMgr(NULL),
	m_pUpdateMgr(NULL)

{
	const char *DefaultWadName = "";
	const Prefs  *pPrefs = GetPrefs ();
	m_pLevelMgr = new CtLevelMgr;
	m_pUpdateMgr = new CGEditProUpdateMgr;

	DefaultWadName = Prefs_GetTxlName (pPrefs);

	strcpy (LastPath, Prefs_GetProjectDir (pPrefs));

	const char *WadPath = FindTextureLibrary (DefaultWadName);

	//	Creates:
	//	1. new CtLevel
	//	2. new Brush List
	//	3. new Entity array
	//	4. new Group List
// changed QD
	m_CurrentDocVars.m_Level = *m_pLevelMgr->Create (WadPath, Prefs_GetHeadersList (pPrefs),
									Prefs_GetActorsList (pPrefs), Prefs_GetPawnIni (pPrefs));
// end change
	if (!m_pLevelMgr->LoadWad (&m_CurrentDocVars.m_Level))
	{
		CString Msg;

		AfxFormatString1 (Msg, IDS_CANTLOADTXL, WadPath);
		AfxMessageBox (Msg);
	}

	m_pMainFrame=(CMainFrame*)AfxGetMainWnd();

	m_CurrentDocVars.m_pSelBrushes = SelBrushList_Create ();
	m_CurrentDocVars.m_pTempSelBrushes = SelBrushList_Create ();
	m_CurrentDocVars.m_pSelFaces = SelFaceList_Create ();
	m_CurrentDocVars.m_pSelEntities = new CEntityArray;
	m_CurrentDocVars.m_pTempSelEntities = new CEntityArray;

	SetLockAxis( 0 ) ;	// Start with no axis locked

	{
		// create our default box
		BrushTemplate_Box *pBoxTemplate = NULL;
		pBoxTemplate = m_pLevelMgr->GetBoxTemplate (&m_CurrentDocVars.m_Level);
		m_pBTemplate = BrushTemplate_CreateBox (pBoxTemplate);
	}

	Brush_Bound(m_pBTemplate);
	m_pCurBrush	=m_pBTemplate;

	m_pRecentStairs = NULL;
	m_pRecentCyl = NULL;
	m_pRecentSphere = NULL;
	m_pRecentCube = NULL;
	m_pRecentCone = NULL;
	m_pRecentArch = NULL;

	geVec3d_Clear(&m_vecSelectedGeoCenter);
	geVec3d_Clear(&m_vecCameraOrigin);
	geVec3d_Clear(&m_vecCameraAngles);
	m_vecCameraAngles.X = RAD(180.0f);

	AddCameraEntityToLevel ();
						//	new g3dc
	CreateCube();		//	added in order to display cube template
						//	dialog on Perties Panel at startup

	m_CurrentDocVars = m_pUpdateMgr->DocVarsUpdate(m_CurrentDocVars);


}/* CGEditProDoc::CGEditProDoc */


CGEditProDoc::~CGEditProDoc()
{
	if (m_pWorldBsp != NULL)		{Node_ClearBsp(m_pWorldBsp); m_pWorldBsp = NULL;}
	if (m_pBTemplate != NULL)		{Brush_Destroy (&m_pBTemplate); m_pBTemplate = NULL;}
	if (m_CurrentDocVars.m_pSelBrushes != NULL)		{SelBrushList_Destroy (&m_CurrentDocVars.m_pSelBrushes); m_CurrentDocVars.m_pSelBrushes = NULL;}
	if (m_CurrentDocVars.m_pTempSelBrushes != NULL)	{SelBrushList_Destroy (&m_CurrentDocVars.m_pTempSelBrushes); m_CurrentDocVars.m_pTempSelBrushes = NULL;}
	if (m_pVecLeakPoints != NULL)	{geRam_Free(m_pVecLeakPoints); m_pVecLeakPoints = NULL;}
	if (m_CurrentDocVars.m_pSelFaces != NULL)		{SelFaceList_Destroy (&m_CurrentDocVars.m_pSelFaces); m_CurrentDocVars.m_pSelFaces = NULL;}

	if (m_CurrentDocVars.m_pSelEntities) { delete m_CurrentDocVars.m_pSelEntities; m_CurrentDocVars.m_pSelEntities = NULL;}
	if (m_CurrentDocVars.m_pTempSelEntities) { delete m_CurrentDocVars.m_pTempSelEntities; m_CurrentDocVars.m_pTempSelEntities = NULL;}

	OpenClipboard(m_pMainFrame->GetSafeHwnd());
	EmptyClipboard();
	CloseClipboard();

	if (m_pLevelMgr) {delete m_pLevelMgr; m_pLevelMgr = NULL;}
	if (m_pUpdateMgr) {delete m_pUpdateMgr; m_pUpdateMgr = NULL;}

}


#pragma warning (disable:4100)
void CGEditProDoc::Serialize(CArchive& ar)
{
	_ASSERTE(0);
}
#pragma warning (default:4100)


WadFileEntry* CGEditProDoc::GetDibBitmap(const char *Name)
{

	return m_pLevelMgr->GetWadBitmap (&m_CurrentDocVars.m_Level, Name);
}

const char *CGEditProDoc::FindTextureLibrary (char const *WadName)
{
	static char WorkPath[MAX_PATH];
	const Prefs  *pPrefs;

	::FilePath_AppendName (LastPath, WadName, WorkPath);
	if (_access (WorkPath, 0) == 0)
	{
		return WorkPath;
	}

	pPrefs = GetPrefs ();
	if (FilePath_SearchForFile (WadName, Prefs_GetTxlSearchPath (pPrefs), WorkPath))
	{
		return WorkPath;
	}

//	post 0.55	//	prompts user to select a valid *.txl file should editor
				//	fail to locate the default *.txl
	else
	{
		CString	txlPathError = _T("GEditPro checks for the default *.txl file when opening new worlds.\n\n");
		txlPathError += _T("GEditPro now is looking for...\n\n");
		txlPathError += _T(WorkPath);
		txlPathError += _T("\n\nbut cannot find it.\n\n");
		txlPathError += _T("To avoid this message in the future, place or rename a *.txl file as...");
		txlPathError += _T(WadName);
		txlPathError += _T("\ninto the above path. Or edit the RFEditPro.ini file to reflect\n");
		txlPathError += _T("the name of your default *.txl file.\n\n");
		txlPathError += _T("For now, please select a valid *.txl file or the application will exit.");

		MessageBox(NULL, txlPathError, "GEditPro cannot find default *.txl", MB_OK);


		CFileDialog FileDlg (TRUE,
			"txl",
			WorkPath,
			OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR,
			"Texture Library Files (*.txl)|*.txl||");

		FileDlg.m_ofn.lpstrTitle	="GEditPro Texture Finder";

		if (FileDlg.DoModal () == IDOK)
		{
			CString	wadpath = FileDlg.GetPathName();
			CString wadFileName = FileDlg.GetFileName();
			strcpy(WorkPath, wadpath.GetBuffer(260));
					//	set the Prefs wadfile to THIS one -- this session only
			Prefs_SetTxlName((Prefs*)pPrefs, wadFileName.GetBuffer(64));
		}

		return WorkPath;
	}

	return NULL;
}



void CGEditProDoc::AddCameraEntityToLevel (void)
{
	// Make default camera entity
	CEntity CameraEntity ;
	CString cstr;

	CreateEntityFromName( "Camera", CameraEntity ) ;
	cstr.LoadString( IDS_CAMERAENTITYNAME ) ;
	CameraEntity.SetKeyValue ("%name%", cstr );
	CameraEntity.SetOrigin ( 0.0f, 0.0f, 0.0f, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) );
	m_pLevelMgr->AddEntity (&m_CurrentDocVars.m_Level, CameraEntity);
}


//		obsolete old gedit
void CGEditProDoc::DeleteFaceAttributes (void)
{
	if (m_pFaceAttributes != NULL)
	{
		delete m_pFaceAttributes;
		m_pFaceAttributes = NULL;
	}
}

//		obsolete old gedit
void CGEditProDoc::DeleteBrushAttributes (void)
{
	if (m_pBrushAttributes != NULL)
	{
		delete m_pBrushAttributes;
		m_pBrushAttributes = NULL;
	}
}


bool CGEditProDoc::SetEntityScaleInView(geFloat scale)	//	post 0.57
{
	m_fEntityScaleInViewFactor = scale;
	return true;
}


geFloat CGEditProDoc::GetEntityScaleInView()		//	post 0.57
{
	return m_fEntityScaleInViewFactor;
}



geBoolean CGEditProDoc::fdocShowBrush
	(
	  Brush const *b,
	  Box3d const *ViewBox
	)
{
	return (BrushIsVisible (b) && Brush_TestBoundsIntersect(b, ViewBox));
}/* CGEditProDoc::fdocShowBrush */


BOOL CGEditProDoc::OnNewDocument()
{
	if(!CDocument::OnNewDocument())
	{
		return FALSE;
	}

	SetupDefaultFilename ();

	UpdateGridInformation();

	//	reset the ViewMgr Dialog
	CMainFrame	*pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (pMainFrame)
	{
		pMainFrame->ResetViewMgrDialog();
	}

	DocVarsUpdate(m_CurrentDocVars);
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews( UAV_ALL3DVIEWS, NULL );
	return TRUE;
}



geBoolean CGEditProDoc::Save(const char *FileName)
{
	{
		// update view information in level
		ViewStateInfo *pViewStateInfo = NULL;
		POSITION		pos = NULL;
		CGEditProView	*	pView = NULL;
		int iView;

		pos = GetFirstViewPosition();
		while( pos != NULL )
		{
			pView = (CGEditProView*)GetNextView(pos) ;
			if (pView)
			{
				switch (Render_GetViewType (pView->VCam))
				{
				case VIEWSOLID :
				case VIEWTEXTURE :
				case VIEWWIRE :
					iView = 0;
					break;
				case VIEWTOP :
					iView = 1;
					break;
				case VIEWFRONT :
					iView = 2;
					break;
				case VIEWSIDE :
					iView = 3;
					break;
				default :
					iView = -1;
				}
				if (iView != -1)
				{
					pViewStateInfo = m_pLevelMgr->GetViewStateInfo (&m_CurrentDocVars.m_Level, iView);
					pViewStateInfo->IsValid = GE_TRUE;
					pViewStateInfo->ZoomFactor = Render_GetZoom (pView->VCam);
					Render_GetPitchRollYaw (pView->VCam, &pViewStateInfo->PitchRollYaw);
					Render_GetCameraPos (pView->VCam, &pViewStateInfo->CameraPos);
				}
			}
		}
	}

	// and then write the level info to the file
	return m_pLevelMgr->WriteToFile (&m_CurrentDocVars.m_Level, FileName);
}

// changed QD 11/03
geBoolean CGEditProDoc::ExportTo3dtv1_32(const char *FileName)
{
	{
		// update view information in level
		ViewStateInfo *pViewStateInfo;
		POSITION		pos;
		CGEditProView	*	pView;
		int iView;

		pos = GetFirstViewPosition();
		while( pos != NULL )
		{
			pView = (CGEditProView*)GetNextView(pos) ;
			switch (Render_GetViewType (pView->VCam))
			{
				case VIEWSOLID :
				case VIEWTEXTURE :
				case VIEWWIRE :
					iView = 0;
					break;
				case VIEWTOP :
					iView = 1;
					break;
				case VIEWFRONT :
					iView = 2;
					break;
				case VIEWSIDE :
					iView = 3;
					break;
				default :
					iView = -1;
			}
			if (iView != -1)
			{
				pViewStateInfo = m_pLevelMgr->GetViewStateInfo (&m_CurrentDocVars.m_Level, iView);
				pViewStateInfo->IsValid = GE_TRUE;
				pViewStateInfo->ZoomFactor = Render_GetZoom (pView->VCam);
				Render_GetPitchRollYaw (pView->VCam, &pViewStateInfo->PitchRollYaw);
				Render_GetCameraPos (pView->VCam, &pViewStateInfo->CameraPos);
			}
		}
	}

	// and then write the level info to the file
	geBoolean fResult = m_pLevelMgr->ExportTo3dtv1_32(&m_CurrentDocVars.m_Level, FileName);

/*	if(fResult == GE_TRUE)
	{
		// Ok, the save was successful.  Gun any ".old" files we
		// ..have laying around for this file.
		char szTemp[512];
		sprintf(szTemp,"%s.old", FileName);
		_unlink(szTemp);
		ConPrintf("Removed file %s\n", szTemp);
	}
*/

	return fResult;
}

geBoolean CGEditProDoc::ExportTo3ds(const char *FileName, int ExpSelected, geBoolean ExpLights, geBoolean ExpFiles)
{
	{
		// update view information in level
		ViewStateInfo *pViewStateInfo;
		POSITION		pos;
		CGEditProView	*	pView;
		int iView;

		pos = GetFirstViewPosition();
		while( pos != NULL )
		{
			pView = (CGEditProView*)GetNextView(pos) ;
			switch (Render_GetViewType (pView->VCam))
			{
				case VIEWSOLID :
				case VIEWTEXTURE :
				case VIEWWIRE :
					iView = 0;
					break;
				case VIEWTOP :
					iView = 1;
					break;
				case VIEWFRONT :
					iView = 2;
					break;
				case VIEWSIDE :
					iView = 3;
					break;
				default :
					iView = -1;
			}
			if (iView != -1)
			{
				pViewStateInfo = m_pLevelMgr->GetViewStateInfo (&m_CurrentDocVars.m_Level, iView);
				pViewStateInfo->IsValid = GE_TRUE;
				pViewStateInfo->ZoomFactor = Render_GetZoom (pView->VCam);
				Render_GetPitchRollYaw (pView->VCam, &pViewStateInfo->PitchRollYaw);
				Render_GetCameraPos (pView->VCam, &pViewStateInfo->CameraPos);
			}
		}
	}

// changed QD 12/03
	BrushList *BList;
	geBoolean fResult;

	BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);
	if(!ExpSelected&&!ExpFiles)
		fResult = m_pLevelMgr->ExportTo3ds(&m_CurrentDocVars.m_Level, FileName, BList, ExpSelected, ExpLights, -1);
	else
	{
		int i, GroupID, GroupCount;
		char NewFileName[MAX_PATH];
		strcpy(NewFileName, FileName);
		GroupID=-1;
		GroupCount=1;

		if(ExpFiles)
		{
			GroupListType *GroupList;

			GroupList=m_pLevelMgr->GetGroups(&m_CurrentDocVars.m_Level);
			GroupCount=Group_GetCount(GroupList);
		}

		for(i=0;i<GroupCount;i++)
		{
			BrushList *SBList;
			Brush *pBrush;
			BrushIterator bi;

			SBList=BrushList_Create();

			pBrush = BrushList_GetFirst (BList, &bi);
			while (pBrush != NULL)
			{
				if(!strstr(Brush_GetName(pBrush),".act"))
				{
					if(!ExpSelected || SelBrushList_Find(m_CurrentDocVars.m_pSelBrushes, pBrush))
					{
						if(!ExpFiles || Brush_GetGroupId(pBrush)==i)
						{
							Brush *pClone =	Brush_Clone(pBrush);
							BrushList_Append(SBList, pClone);
						}
					}
				}

				pBrush = BrushList_GetNext(&bi);
			}
			// do CSG
			{
				ModelIterator	mi;
				int				j, CurId = 0;
				ModelInfo_Type	*ModelInfo;
				Model			*pMod;

				BrushList_ClearAllCSG (SBList);

				BrushList_DoCSG(SBList, CurId, ::fdocBrushCSGCallback, this);

				//build individual model mini trees
				ModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);
				pMod = ModelList_GetFirst (ModelInfo->Models, &mi);

				for(j=0;j < ModelList_GetCount(ModelInfo->Models);j++)//, n=NULL)
				{
					CurId = Model_GetId (pMod);

					BrushList_DoCSG(SBList, CurId, ::fdocBrushCSGCallback, this);
				}
			}

			if(ExpFiles)
			{
				GroupID=i;

				//build individual filenames
				char Name[MAX_PATH];
				char c[2];
				c[1]='\0';
				::FilePath_GetName (FileName, Name);
				c[0] = (char)(48+(i-i%100)/100);
				strcat(Name, c);
				c[0] = (char)(48+((i-i%10)/10)%10);
				strcat(Name, c);
				c[0] = (char)(48+i%10);
				strcat(Name, c);

				::FilePath_ChangeName(FileName, Name, NewFileName);
			}

			fResult = m_pLevelMgr->ExportTo3ds(&m_CurrentDocVars.m_Level, NewFileName, SBList, ExpSelected, ExpLights, GroupID);
			if(!fResult)
				CGlobals::GetActiveDocument()->m_pMainFrame->ConPrintf("Error exporting group %i\n", i);
			BrushList_Destroy(&SBList);
		}

	}
// end change


/*
	if(fResult == GE_TRUE)
	{
		// Ok, the save was successful.  Gun any ".old" files we
		// ..have laying around for this file.
		char szTemp[512];
		sprintf(szTemp,"%s.old", FileName);
		_unlink(szTemp);
		ConPrintf("Removed file %s\n", szTemp);
	}
*/
	return fResult;
}

// end change

/*
	Load file versions later than 1.0
*/
geBoolean CGEditProDoc::Load(const char *FileName)
{
	const char		*Errmsg = NULL, *WadPath = NULL;
	int				i;
	EntityViewList	*pEntityView = NULL;
	const Prefs		*pPrefs = NULL;
	GroupIterator gi;
	GroupListType *Groups = NULL;
	char WorkingDir[MAX_PATH];
	Brush *pBox = NULL;

	pPrefs = GetPrefs();

	FilePath_GetDriveAndDir (FileName, WorkingDir);
	::SetCurrentDirectory (WorkingDir);

// changed QD
	if (pPrefs)
		m_CurrentDocVars.m_Level = *m_pLevelMgr->CreateFromFile (FileName, &Errmsg, Prefs_GetHeadersList (pPrefs),
															Prefs_GetActorsList(pPrefs), Prefs_GetPawnIni(pPrefs));
// end change
	WadPath = m_pLevelMgr->GetWadPath (&m_CurrentDocVars.m_Level);

	if (WadPath)
	{
		if (!m_pLevelMgr->LoadWad(&m_CurrentDocVars.m_Level))
		{
			CString Msg;

			AfxFormatString1 (Msg, IDS_CANTLOADTXL, WadPath);
			AfxMessageBox (Msg);
		}
	}

	m_pLevelMgr->EnumLeafBrushes (&m_CurrentDocVars.m_Level, &m_CurrentDocVars.m_Level, FaceFixupCallback);

	if( ValidateEntities( &m_CurrentDocVars) == FALSE || ValidateBrushes( ) == FALSE )
	{
		ShowConsole();
		AfxMessageBox( IDS_LOAD_WARNING ) ;

		if (ValidateEntities( &m_CurrentDocVars) == FALSE)	//	post 0.55
			OnLeveloptions();	// open up level options dialog right away
	}

	Groups = m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level);
	if (Groups)
	{
		m_iCurrentGroup = Group_GetFirstId (Groups, &gi);
	}

	AddCameraEntityToLevel ();

	pBox = BrushTemplate_CreateBox (m_pLevelMgr->GetBoxTemplate (&m_CurrentDocVars.m_Level));
	if (pBox)
	{
		CreateNewTemplateBrush (pBox);
	}

	// update entity visibility info
	pEntityView	=m_pLevelMgr->GetEntityVisibilityInfo (&m_CurrentDocVars.m_Level);
	if (pEntityView)
	{
		for (i = 0; i < pEntityView->nEntries; ++i)
		{
			m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, &pEntityView->pEntries[i], ::fdocSetEntityVisibility);
		}
	}

	DocVarsUpdate(m_CurrentDocVars);
	return GE_TRUE;
LoadError:
	AfxMessageBox (Errmsg);
	return GE_FALSE;
}

//	obsolete -- not used
// play a sound from a resource
geBoolean CGEditProDoc::PlayResource (char const * pName)
{
	BOOL bRtn;
	char * lpRes = NULL;
	HGLOBAL hRes;
	HRSRC hResInfo;
	HINSTANCE hInst = AfxGetInstanceHandle ();

	// Find the WAVE resource.
    hResInfo = ::FindResource (hInst, pName, "WAVE");
	if (hResInfo == NULL)
	{
        return FALSE;
	}

	// Load the WAVE resource.
    hRes = ::LoadResource(hInst, hResInfo);
	if (hRes == NULL)
	{
        return FALSE;
	}

	// Lock the WAVE resource and play it.
    lpRes = (char *)::LockResource(hRes);
	if (lpRes != NULL)
	{
        bRtn = (::sndPlaySound(lpRes, SND_MEMORY | SND_SYNC | SND_NODEFAULT) != 0);
		::UnlockResource(hRes);
	}
	else
	{
        bRtn = GE_FALSE;
	}

	// Free the WAVE resource and return success or failure.
    FreeResource (hRes);
	return bRtn;
}



//	called by AddBrushToWorld
void CGEditProDoc::OnBrushAddtoworld()
{
	geBoolean Placed;
	CGEditProDocVars thisFunctionDocVars;
	thisFunctionDocVars = DocVarsCreateUniqueCopy(m_CurrentDocVars);

	if(m_iModeTool!=ID_TOOLS_TEMPLATE)
		return;

	Placed = GE_FALSE;
	if(m_bTempEnt)
	{
		// here we check to see if the user is placing prefab templates down
		if ( m_bPlaceObjectFlag )
		{
			geBoolean GotName;
			char ObjectName[MAX_PATH];

			// get the current object from the library and place it down
			GotName = m_pMainFrame->m_TemplatesTab.GetCurrentTemplateName(ObjectName);
			if (GotName)
			{
				Placed = PlaceObject (ObjectName, &m_RegularEntity.mOrigin, &thisFunctionDocVars);

				::PlaySound(MAKEINTRESOURCE(IDR_WAVE_WOOSH), AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);

	//			PlayResource ("SND_WHOOSH");
				//	Be very careful when speccing flags for UpdateAllViews()
				//	The wrong flags at the wrong time will totally screw things up
				//	g3dc tuning comment
				//	SetMode() has its own UpdateMainControls()
				//	and ConfigureCurrentTool() calls
				//UpdateAllViews(UAV_ALL3DVIEWS, NULL);

				//	need to SetMode because a DoGeneralSelect() was performed in the
				//	chain above. DoGeneralSelect() sets the mode to ID_GENERALSELECT,
				//	and sets the edit tool to NULL. But we want to continue to
				//	place prefabs. So we call SetMode to bring us back into
				//	ID_TOOLS_TEMPLATE mode and set the move/rotate button. :-)
				//	SetMode() has its own UpdateMainControls()
				//	and ConfigureCurrentTool() calls
				SetMode(MAINFRM_COMMANDPANEL_TEMPLATES);	//	new g3dc
			}
		}

		else	//	we are placing an entity
		{
			// now create a new entity of the currently-selected type
			//			GotName = m_pMainFrame->m_wndTabControls->m_pBrushEntityDialog->GetCurrentEntityName (EntityName); // old gedit
			geBoolean	GotName;
			CEntity		NewEnt;
			char		EntityName[MAX_PATH];

			m_RegularEntity.DoneMove (1, m_pLevelMgr->GetEntityDefs (&thisFunctionDocVars.m_Level));

			// now create a new entity of the currently-selected type
			GotName = m_pMainFrame->m_TemplatesTab.GetCurrentEntityName(EntityName);	// new g3dc
			if (GotName)
			{
				if (CreateEntityFromName (EntityName, NewEnt))
				{
					geVec3d org;
					org = m_RegularEntity.mOrigin;
					// set the new entity's origin
					NewEnt.SetOrigin (org.X, org.Y, org.Z, m_pLevelMgr->GetEntityDefs (&thisFunctionDocVars.m_Level));
					// add to current group
					NewEnt.SetGroupId (m_iCurrentGroup);
					m_pLevelMgr->AddEntity (&thisFunctionDocVars.m_Level, NewEnt);
					Placed = GE_TRUE;

					::PlaySound(MAKEINTRESOURCE(IDR_WAVE_WOOSH), AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);

					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					UpdateAllViews(UAV_ALL3DVIEWS, NULL);

					m_pMainFrame->UpdateMainControls();	// new g3dc

				}
			}
		}
	}
	else	// if it's a brush and not an entity or prefab
	{
		Brush	*nb = NULL;
		geVec3d *pTemplatePos = NULL;

		nb	= Brush_Clone(m_pCurBrush);
		if (nb)
		{
			SetDefaultBrushTexInfo(nb);
			Brush_Bound (nb);
			pTemplatePos = m_pLevelMgr->GetTemplatePos (&thisFunctionDocVars.m_Level);
			Brush_Center (nb, pTemplatePos);
			// add to current group
			Brush_SetGroupId (nb, m_iCurrentGroup);

			{
				// set draw scale and lightmap scale defaults for all faces
				fdocFaceScales Scales;

				Scales.DrawScale = m_pLevelMgr->GetDrawScale (&thisFunctionDocVars.m_Level);
				Scales.LightmapScale = m_pLevelMgr->GetLightmapScale (&thisFunctionDocVars.m_Level);
				Brush_EnumFaces (nb, &Scales, ::fdocSetFaceScales);

			}

			m_pLevelMgr->AppendBrush (&thisFunctionDocVars.m_Level, nb);

			::PlaySound(MAKEINTRESOURCE(IDR_WAVE_WOOSH), AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);

			if(!Brush_IsHollow(nb) && !Brush_IsMulti(nb))
			{
				m_pWorldBsp	=Node_AddBrushToTree(m_pWorldBsp, nb);
				//	Be very careful when speccing flags for UpdateAllViews()
				//	The wrong flags at the wrong time will totally screw things up
				UpdateAllViews(UAV_ALL3DVIEWS, NULL);
			}
			else
			{
				//	Be very careful when speccing flags for UpdateAllViews()
				//	The wrong flags at the wrong time will totally screw things up
				UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL, TRUE);
			}

			m_pMainFrame->UpdateMainControls();	// new g3dc

			Placed = GE_TRUE;
		}
	}


	//	this cluster of code fixes a problem that occurs when placing prefabs that contain
	//	entities. The problem is that the origin values for the included entities are not
	//	set, and when the user later selects the prefab or just the entity, the entitiy
	//	moves to 0,0,0 -- level center.
	//	This code surveys the level for all entities --including those that have accompanied
	//	recently-added prefabs. It gets each entity's origin values (from the entities
	//	themselves,then sets their origins based on their internal origin values.
	geVec3d			org;
	CEntityArray	*Entities = NULL;
	Entities = m_pLevelMgr->GetEntities (&thisFunctionDocVars.m_Level);
	if (Entities)
	{
		for( int Ent = 0; Ent < Entities->GetSize(); Ent++ )
		{
			org = (*Entities)[ Ent ].mOrigin;
			((*Entities)[ Ent ].SetOrigin(org.X, org.Y, org.Z, m_pLevelMgr->GetEntityDefs (&thisFunctionDocVars.m_Level)));
		}
	}


	if (Placed)
	{
		SetModifiedFlag();
	}

	DocVarsUpdate(thisFunctionDocVars);
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALLVIEWS /*| REBUILD_QUICK*/, NULL, TRUE);
	m_pMainFrame->UpdateMainControls();	// new g3dc
}

// This code is STILL called from the toolbar button, but the "correct"
// interface is now on the Brush Attributes Dialog
void CGEditProDoc::OnBrushSubtractfromworld()
{
	CGEditProDocVars thisFunctionDocVars;
	thisFunctionDocVars = DocVarsCreateUniqueCopy(m_CurrentDocVars);


	Brush	*nb = NULL;
	BrushList *BList = NULL;
	BList = m_pLevelMgr->GetBrushes (&thisFunctionDocVars.m_Level);

	if (BList)
	{
	if ((m_iModeTool==ID_GENERALSELECT) && (BrushList_Count (BList, BRUSH_COUNT_MULTI | BRUSH_COUNT_LEAF) < 2))
	{
		// cuts shouldn't start the list
		return;
	}

	SetModifiedFlag();

	if(m_iModeTool==ID_GENERALSELECT)
	{
		// put the brush at the very end of the list
		BrushList_Remove (BList, m_pCurBrush);
		Brush_SetSubtract(m_pCurBrush, GE_TRUE);

		SelBrushList_RemoveAll (thisFunctionDocVars.m_pSelBrushes);
		BrushList_Append (BList, m_pCurBrush);
	}
	else
	{
		nb	=Brush_Clone(m_pCurBrush);

		SetDefaultBrushTexInfo(nb);
		Brush_Bound (nb);
		BrushList_Append (BList, nb);
	}

	DocVarsUpdate(thisFunctionDocVars);

	UpdateSelected();	//	calls UpdateMainControls)()
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL, TRUE);
	}
}

void CGEditProDoc::CopySelectedBrushes(void)
{
	int		i;
	int		NumSelBrushes;

	NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);

	if( NumSelBrushes )
	{
		// create copies of the selected brushes
		for(i=0;i < NumSelBrushes;i++)
		{
			// clone the brush, add copy to level,
			// add copy to select list, and remove original from
			// select list.
			Brush *pBrush = NULL;
			Brush *pClone = NULL;

			pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, 0);
// changed QD Actors
// don't copy ActorBrushes
			if(strstr(Brush_GetName(pBrush),".act")!=NULL)
				continue;
// end change
			pClone = Brush_Clone (pBrush);
			m_pLevelMgr->AppendBrush (&m_CurrentDocVars.m_Level, pClone);
			SelBrushList_Add (m_CurrentDocVars.m_pSelBrushes, pClone);
			SelBrushList_Remove (m_CurrentDocVars.m_pSelBrushes, pBrush);
		}
	}// Duplicate selected and delesect original Brushes


	CEntity  TEnt;
	CEntityArray *Entities = NULL;
	int cnt;

	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
	if (Entities)
	{
		cnt = Entities->GetSize() ;
		for( i=0 ; i < cnt; i++ )
		{
			CEntity *pEnt = &(*Entities)[i];
			if( pEnt->IsCamera() == GE_FALSE  )	// Exclude Cameras
			{
				if (pEnt->IsSelected ())
				{
					DeselectEntity (pEnt);
					/*
					There really is a reason for this oddity.
					Because m_pLevelMgr->AddEntity might cause the entities array to grow,
					dereferencing the data in the array can cause problems because
					growing the array might cause the dereferenced data to move.
					(Fun, huh?)  So I create a temporary entity, copy the contents
					there, and then use that as cloning material.
					*/
					CEntity WorkingEntity;
					int		Index ;

					WorkingEntity = *pEnt;
					Index = m_pLevelMgr->AddEntity (&m_CurrentDocVars.m_Level, WorkingEntity);

					SelectEntity( &(*Entities)[Index] );
				}
			}
		}

		//	ConPrintf("Cloned %d Brushes, %d Entities.\n", NumSelBrushes, m_iNumSelEntities);	// old Gedit
		m_pMainFrame->ConPrintf("Cloned %d Brushes, %d Entities.\n", NumSelBrushes, m_iNumSelEntities);	// new g3dc
		// Copying items places the new items in the same group, so we must update the UI

		//	m_pMainFrame->m_wndTabControls->GrpTab->UpdateTabDisplay( this ) ;	// old gedit
		//	m_pMainFrame->m_GroupsTab.UpdateTabDisplay( this ) ;	// new g3dc
		m_pMainFrame->UpdateMainControls();
		UpdateSelected();
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		UpdateAllViews( UAV_ALL3DVIEWS, NULL );
	}
}

void CGEditProDoc::MakeSelectedBrushNewest(void)
{
	if(GetSelState()==ONEBRUSHONLY)
	{
		m_pLevelMgr->RemoveBrush (&m_CurrentDocVars.m_Level, m_pCurBrush);
		m_pLevelMgr->AppendBrush (&m_CurrentDocVars.m_Level, m_pCurBrush);
	}
}


BOOL CGEditProDoc::TempDeleteSelected(CGEditProDocVars *pDocVars)
{
	BOOL	ret;
	int		i;
	int		NumTSelBrushes = SelBrushList_GetSize (pDocVars->m_pTempSelBrushes);

	for(i=0;i < NumTSelBrushes;i++)
	{
		Brush *pBrush = NULL;

		pBrush = SelBrushList_GetBrush (pDocVars->m_pTempSelBrushes, i);

		if (pBrush)
		{
			m_pLevelMgr->RemoveBrush (&pDocVars->m_Level, pBrush);
			SelBrushList_Remove (pDocVars->m_pTempSelBrushes, pBrush);
			Brush_Destroy(&pBrush);
			pBrush = NULL;
			ret	=TRUE;
		}
	}
	return	ret;
}

void CGEditProDoc::TempCopySelectedBrushes(void)
{
	int		i, iNumSelBrushes, iNumSelEntities;

	iNumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);

	SelBrushList_RemoveAll (m_CurrentDocVars.m_pTempSelBrushes);

	// make copies of selected brushes
	for(i=0;i < iNumSelBrushes;i++)
	{
		Brush	*pBrush = NULL, *pClone = NULL;

		pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);
		if (pBrush)
		{
			pClone = Brush_Clone (pBrush);
		}
		if (pClone)
		{
			m_pLevelMgr->AppendBrush (&m_CurrentDocVars.m_Level, pClone);
			SelBrushList_Add (m_CurrentDocVars.m_pTempSelBrushes, pClone);
		}
	}
}

void CGEditProDoc::SetDefaultBrushTexInfo(Brush *b)
{
	const char *TexName = NULL;
	BrushTexSetData CallbackData;

	if (b == NULL)	//	post 0.55
	{
		MessageBox(NULL, "Brush == NULL", "GEditProDoc SetDefaultBrushTexInfo", MB_OK);
		return;
	}

	TexName = m_pMainFrame->m_TexturesTab.GetCurrentTexture();
	CallbackData.pDoc = this;
	CallbackData.TexName = TexName;

	Brush_SetName(b, TexName);
	if(Brush_IsMulti(b))
	{
		const BrushList *BList = NULL;	//	post 0.55
		BList = Brush_GetBrushList(b);
		if (BList == NULL)
		{
			MessageBox(NULL, "BrushList == NULL", "GEditProDoc SetDefaultBrushTexInfo", MB_OK);
			return;
		}
		else
			BrushList_EnumLeafBrushes (BList, &CallbackData, ::BrushTexSetCB) ;
	}
	else
	{
		::BrushTexSetCB (b, &CallbackData);
	}
}

// Test BOTH group and brush visiblity--group overrides
// User flags for All, Visible, and Current group must be considered
geBoolean CGEditProDoc::BrushIsVisible( const Brush * pBrush ) const
{
	int			GroupId ;

	if (!Brush_IsVisible (pBrush))
	{
		return GE_FALSE;
	}
	GroupId = Brush_GetGroupId (pBrush);

	CtLevel *pTempLevel = NULL;
	pTempLevel = &(CtLevel)m_CurrentDocVars.m_Level;
	if (pTempLevel)
	{
		switch (m_pLevelMgr->GetGroupVisibility (&m_CurrentDocVars.m_Level))
		{
		case Group_ShowAll :
			return GE_TRUE;

		case Group_ShowCurrent :
			return (GroupId == m_iCurrentGroup);

		case Group_ShowVisible :
			return Group_IsVisible (m_pLevelMgr->GetGroups(pTempLevel), GroupId);

		default :
			assert (0);
			return GE_FALSE;
		}
		return GE_TRUE;
	}
	return GE_FALSE;

}/* CGEditProDoc::BrushIsVisible */


// Test BOTH group and entity visiblity--group overrides
// User flags for All, Visible, and Current group must be considered
geBoolean CGEditProDoc::EntityIsVisible( const CEntity *pEntity ) const
{
	int			GroupId ;

	if (pEntity->IsCamera ())
	{
		return pEntity->IsVisible ();
	}
	if ((m_iShowEntities == GE_FALSE) || !pEntity->IsVisible ())
	{
		return GE_FALSE ;
	}

	GroupId = pEntity->GetGroupId( );

			CtLevel *pTempLevel = NULL;
			pTempLevel = &(CtLevel)m_CurrentDocVars.m_Level;

	switch (m_pLevelMgr->GetGroupVisibility (&m_CurrentDocVars.m_Level))
	{
	    case Group_ShowAll :
		    return GE_TRUE;

		case Group_ShowCurrent :
		    return (GroupId == m_iCurrentGroup);

		case Group_ShowVisible :

		    return Group_IsVisible (m_pLevelMgr->GetGroups (pTempLevel), GroupId);

		default :
		    assert (0);
			return GE_FALSE;
	}
}/* CGEditProDoc::EntityIsVisible */

void CGEditProDoc::CreateNewTemplateBrush( Brush *pBrush)
{
	geVec3d *pTemplatePos = NULL;
	geVec3d MoveVec;
	geVec3d BrushPos;

	assert (pBrush != NULL);

	if (m_pBTemplate != NULL)
	{
		Brush_Destroy (&m_pBTemplate);
	}
	m_pBTemplate = pBrush;
	m_pCurBrush = pBrush;

	m_bTempEnt	= FALSE;
	SetDefaultBrushTexInfo (m_pCurBrush);
	Brush_Bound (m_pCurBrush);
	Brush_Center (m_pCurBrush, &BrushPos);

	pTemplatePos = m_pLevelMgr->GetTemplatePos (&m_CurrentDocVars.m_Level);
	geVec3d_Subtract (pTemplatePos, &BrushPos, &MoveVec);
	Brush_Move (m_pCurBrush, &MoveVec);

	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews (UAV_ALL3DVIEWS, NULL);
	SetModifiedFlag ();
}

void CGEditProDoc::OnUpdateBrushPrimitives (CCmdUI *pCmdUI)
{
	// This function is used by all the primitive UI OnUpdateXXX's
	pCmdUI->Enable( (m_iModeTool == ID_TOOLS_TEMPLATE ) ? TRUE : FALSE ) ;
}

void CGEditProDoc::OnBrushPrimitivesCube()
{
	CreateCube() ;
}


void CGEditProDoc::CreateCube()
{
	if (m_pMainFrame->m_createBoxDialog.m_hWnd != NULL)	//	new g3dc
		return;							//	already showing

	BrushTemplate_Box *pBoxTemplate = NULL;
	pBoxTemplate = m_pLevelMgr->GetBoxTemplate (&m_CurrentDocVars.m_Level);

	if (pBoxTemplate == NULL)	//	post 0.55
		return;

	// create it on the fly and put it as a child on the Propeties panel -- new for g3dc
	if (!m_pMainFrame->m_createBoxDialog.Create(IDD_CREATEBOX, &m_pMainFrame->m_PropertiesPanel))
	{
		TRACE0("Failed to create m_createBoxDialog\n");
		return;
	}

	m_pMainFrame->m_createBoxDialog.ShowDialog((m_pLevelMgr->GetGridType (&m_CurrentDocVars.m_Level) == GridMetric), pBoxTemplate, this); // new g3dc

}/* CGEditProDoc::CreateCube */



void CGEditProDoc::OnBrushPrimitivesSpheroid()
{
	CreateSpheroid() ;
}


void CGEditProDoc::CreateSpheroid()
{
	if (m_pMainFrame->m_createSpheroidDialog.m_hWnd != NULL)	//	new g3dc
		return;									//	already showing

	if (&m_CurrentDocVars.m_Level == NULL)	//	post 0.55
		return;

	BrushTemplate_Spheroid *pTemplate = NULL;
	pTemplate = m_pLevelMgr->GetSpheroidTemplate (&m_CurrentDocVars.m_Level);

	if (pTemplate == NULL)	//	post 0.55
		return;

	if	(!m_pMainFrame->m_createSpheroidDialog.Create(IDD_CREATE_SPHEROID, &m_pMainFrame->m_PropertiesPanel))
	{
		TRACE0("Failed to create m_createSpheroidDialog\n");
		return;
	}

	m_pMainFrame->m_createSpheroidDialog.ShowDialog(m_pLevelMgr->GetGridType (&m_CurrentDocVars.m_Level) == GridMetric, pTemplate, this);

}/* CGEditProDoc::CreateSpheroid */

void CGEditProDoc::OnBrushPrimitivesCylinder()
{
	CreateCylinder() ;
}


void CGEditProDoc::CreateCylinder()
{

	if (m_pMainFrame->m_createCylDialog.m_hWnd != NULL)	//	new g3dc
		return;							//	already showing

	if (&m_CurrentDocVars.m_Level == NULL)	//	post 0.55
		return;


	BrushTemplate_Cylinder *pCylTemplate = NULL;
	pCylTemplate = m_pLevelMgr->GetCylinderTemplate (&m_CurrentDocVars.m_Level);

	if (pCylTemplate == NULL)
		return;

	if	(!m_pMainFrame->m_createCylDialog.Create(IDD_CREATE_CYL, &m_pMainFrame->m_PropertiesPanel))
	{
		TRACE0("Failed to create m_createCylinderDialog\n");
		return;
	}

	m_pMainFrame->m_createCylDialog.ShowDialog(m_pLevelMgr->GetGridType (&m_CurrentDocVars.m_Level) == GridMetric, pCylTemplate, this);

}/* CGEditProDoc::CreateCylinder */

void CGEditProDoc::OnBrushPrimitivesStaircase()
{
	CreateStaircase() ;
}


void CGEditProDoc::CreateStaircase()
{

	if (m_pMainFrame->m_createStaircaseDialog.m_hWnd != NULL)
		return;									//	already showing

	if (&m_CurrentDocVars.m_Level == NULL)
		return;

	BrushTemplate_Staircase *pStairTemplate = NULL;
	pStairTemplate = m_pLevelMgr->GetStaircaseTemplate (&m_CurrentDocVars.m_Level);

	if (pStairTemplate == NULL)
		return;

	if	(!m_pMainFrame->m_createStaircaseDialog.Create(IDD_STAIRCASEDIALOG, &m_pMainFrame->m_PropertiesPanel))
	{
		TRACE0("Failed to create m_createStaircaseDialog\n");
		return;
	}

	m_pMainFrame->m_createStaircaseDialog.ShowDialog(m_pLevelMgr->GetGridType (&m_CurrentDocVars.m_Level) == GridMetric, pStairTemplate, this);

}/* CGEditProDoc::CreateStaircase */



void CGEditProDoc::OnBrushPrimitivesArch()
{
	CreateArch ();
}



void CGEditProDoc::CreateArch()
{
	if (m_pMainFrame->m_createArchDialog.m_hWnd != NULL)
		return;									//	already showing

	if (&m_CurrentDocVars.m_Level == NULL)
		return;

	BrushTemplate_Arch *pArchTemplate = NULL;
	pArchTemplate = m_pLevelMgr->GetArchTemplate (&m_CurrentDocVars.m_Level);

	if (pArchTemplate == NULL)
		return;


	if	(!m_pMainFrame->m_createArchDialog.Create(IDD_CREATE_ARCH, &m_pMainFrame->m_PropertiesPanel))
	{
		TRACE0("Failed to create m_createArchDialog\n");
		return;
	}

	m_pMainFrame->m_createArchDialog.ShowDialog(m_pLevelMgr->GetGridType (&m_CurrentDocVars.m_Level) == GridMetric, pArchTemplate, this);

}/* CGEditProDoc::CreateArch */


void CGEditProDoc::OnBrushPrimitivesCone()
{
	CreateCone ();
}
void CGEditProDoc::CreateCone()
{
	if (m_pMainFrame->m_createConeDialog.m_hWnd != NULL)
		return;									//	already showing

	if (&m_CurrentDocVars.m_Level == NULL)
		return;

	BrushTemplate_Cone *pConeTemplate = NULL;
	pConeTemplate = m_pLevelMgr->GetConeTemplate (&m_CurrentDocVars.m_Level);

	if (pConeTemplate == NULL)
		return;


	if	(!m_pMainFrame->m_createConeDialog.Create(IDD_CREATE_CONE, &m_pMainFrame->m_PropertiesPanel))
	{
		TRACE0("Failed to create m_createConeDialog\n");
		return;
	}

	m_pMainFrame->m_createConeDialog.ShowDialog(m_pLevelMgr->GetGridType (&m_CurrentDocVars.m_Level) == GridMetric, pConeTemplate, this);
}

void CGEditProDoc::BrushSelect(Brush *pBrush)
{
	// if the brush is already selected, then unselect it.
	// if not currently selected, then select it.
	if (!SelBrushList_Remove (m_CurrentDocVars.m_pSelBrushes, pBrush))
	{
		SelBrushList_Add (m_CurrentDocVars.m_pSelBrushes, pBrush);
						//	update all selcetions. Prevent unintended selections
						//	across groups.
		m_pMainFrame->UpdateMainControls();
	}
}

geBoolean CGEditProDoc::BrushIsSelected(Brush const *pBrush)
{
	assert (pBrush != NULL);

	return SelBrushList_Find (m_CurrentDocVars.m_pSelBrushes, pBrush);
}



void CGEditProDoc::OnToolsUsegrid()
{
	GridInfo *pGridInfo = m_pLevelMgr->GetGridInfo (&m_CurrentDocVars.m_Level);

	pGridInfo->UseGrid = !(pGridInfo->UseGrid);

	UpdateGridInformation();
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_GRID_ONLY, NULL);
}

void CGEditProDoc::OnUpdateToolsUsegrid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_pLevelMgr->UseGrid (&m_CurrentDocVars.m_Level));
}



geBoolean CGEditProDoc::CreateEntityFromName(char const *pEntityType, CEntity &NewEnt)
{
	assert (pEntityType != NULL);
	// get all properties for this entity type...
	EntityPropertiesList *pProps = NULL;

	pProps = EntityTable_GetEntityPropertiesFromName (m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level), pEntityType, ET_ALL);
	if (pProps == NULL)
	{
	    return FALSE;
	}

	// Add key/value pairs for all of the properties...
	for (int PropNo = 0; PropNo < pProps->NumProps; ++PropNo)
	{
	    EntityProperty *p = &(pProps->Props[PropNo]);

		NewEnt.SetKeyValue (p->pKey, p->pValue);

	}

	EntityTable_ReleaseEntityProperties (pProps);

	NewEnt.SetGroupId ( 0 );
	NewEnt.UpdateOrigin (m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));
	return TRUE;
}

void CGEditProDoc::CreateEntity(char const *pEntityType)
{
	CEntity NewEnt;
	if (CreateEntityFromName (pEntityType, NewEnt))
	{
		m_bTempEnt = TRUE;

		m_RegularEntity = NewEnt;
		m_iCurrentEntity = -1;
		// set this flag so that doc knows when enter is pressed that user is NOT adding
		// objects to level
		m_bPlaceObjectFlag = FALSE;

		m_iCurrentTool=ID_TOOLS_BRUSH_MOVEROTATEBRUSH;

		//	g3dc tuning comment
		ConfigureCurrentTool();	// already calls UpdateAllViews()

		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		//	g3dc tuning comment
//		UpdateAllViews(UAV_ALL3DVIEWS, NULL);	// already called by Configure CurrentTool
		SetModifiedFlag();
	}
}

// creates a template entity with which the user specifies a location for any
// objects they place
void CGEditProDoc::CreateObjectTemplate()
{
	CEntity NewEnt;

	// make an origin
	if (CreateEntityFromName ("ModelOrigin", NewEnt))
	{
		m_bTempEnt=TRUE;

		NewEnt.SetOrigin (0.0f, 0.0f, 0.0f, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));

		m_RegularEntity=NewEnt;
		m_iCurrentEntity=-1;

		// set this flag so that doc knows when enter is pressed that user is adding objects
		// to level
		m_bPlaceObjectFlag = TRUE;		//	old gedit

		m_iCurrentTool=ID_TOOLS_BRUSH_MOVEROTATEBRUSH;

		// g3dc tuning comment
		// already calls UpdateAllViews()
		ConfigureCurrentTool();

		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up

		// g3dc tuning comment
		// ConfigureCurrentTool() already calls UpdateAllViews()
//		UpdateAllViews(UAV_ALL3DVIEWS, NULL);
		SetModifiedFlag();
	}
}

void CGEditProDoc::ResetAllSelections (void)
{
	ResetAllSelectedFaces();
	ResetAllSelectedBrushes();
	ResetAllSelectedEntities();
}


void CGEditProDoc::OnEntitiesShow(void)
{
	m_iShowEntities = !m_iShowEntities;
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}

void CGEditProDoc::OnUpdateEntitiesShow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_iShowEntities );
}

void CGEditProDoc::OnViewShowAllGroups()
{
	m_pCurBrush	= m_pBTemplate;
	CGlobals::g_iGroupVisibility = Group_ShowAll;

	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}


void CGEditProDoc::OnViewShowCurrentGroup()
{
	m_pCurBrush = m_pBTemplate;

	CGlobals::g_iGroupVisibility = Group_ShowCurrent;

	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}

void CGEditProDoc::OnViewShowVisibleGroups()
{
	m_pCurBrush	= m_pBTemplate;
	CGlobals::g_iGroupVisibility = Group_ShowVisible;

	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}

void CGEditProDoc::OnUpdateViewShowVisibleGroups(CCmdUI* pCmdUI)
{
	BOOL	bEnable ;
	int Setting;

	Setting = CGlobals::g_iGroupVisibility;
	bEnable = ( Group_GetCount( m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level)) ) ? TRUE : FALSE ;

	if (((pCmdUI->m_nID == ID_VIEW_SHOW_ALLGROUPS) && (Setting == Group_ShowAll)) ||
		((pCmdUI->m_nID == ID_VIEW_SHOW_CURRENTGROUP) && (Setting == Group_ShowCurrent)) ||
		((pCmdUI->m_nID == ID_VIEW_SHOW_VISIBLEGROUPS) && (Setting == Group_ShowVisible)))
	{
		pCmdUI->SetCheck();
	}
	else
	{
		pCmdUI->SetCheck(0);
	}

	pCmdUI->Enable( bEnable ) ;
}

void CGEditProDoc::OnUpdateViewShowAllGroups(CCmdUI* pCmdUI)
{
	OnUpdateViewShowVisibleGroups( pCmdUI ) ;
}


void CGEditProDoc::OnUpdateViewShowCurrentGroup(CCmdUI* pCmdUI)
{
	OnUpdateViewShowVisibleGroups( pCmdUI ) ;
}



CEntity *CGEditProDoc::FindCameraEntity (CGEditProDocVars *pDocVars)
{
	CEntityArray *Entities = m_pLevelMgr->GetEntities (&pDocVars->m_Level);
	int i;

	for (i = 0; i < Entities->GetSize (); ++i)
	{
		CEntity *pEnt;

		pEnt = &(*Entities)[i];
		if (pEnt->IsCamera ())
		{
			return pEnt;
		}
	}
	return NULL;
}

//need to put this somewhere
#define	VectorToSUB(a, b)			(*((((geFloat *)(&a))) + (b)))

void CGEditProDoc::RenderOrthoView(ViewVars *v, CDC *pDC)
{
	int				inidx, i;
	GroupIterator	gi;
	int				GroupId;
	BrushDrawData	brushDrawData ;
	BrushList		*BList = NULL;

	if (&m_CurrentDocVars.m_Level == NULL)
	{
		// must not have been loaded.  Probably an assert popping up....
		return;
	}

	/*
	  We could declare these pens so that they have limited scope, but
	  then we'd have to remember to select oldpen back into the DC before
	  exiting the scope.  Otherwise, if the pen remains selected then
	  Windows won't free the resource.  Bad things ensue.

	  new g3dc - see globals.h for color definitions
	*/
	CPen	PenAllItems (PS_SOLID, 1, WHITE);
	CPen	PenCutBrush (PS_SOLID, 1, ORANGE);
	CPen	PenDetailBrush (PS_DASH, 1, WHITE);
	CPen	PenSelected (PS_SOLID, 1, CYAN);
	CPen	PenTemplate (PS_SOLID, 1, TEMPLATEBLUE);
	CPen	PenHintBrush (PS_SOLID, 1, DKGREEN);
	CPen	PenClipBrush (PS_SOLID, 1, DKMAGENTA);
	CPen	PenSheetFaces (PS_SOLID, 1, YELLOW);
	CPen	PenSelectedFaces (PS_SOLID, 1, MAGENTA);

	geVec3d		XTemp;
	Box3d		ViewBox;
	int GroupVis = m_pLevelMgr->GetGroupVisibility (&m_CurrentDocVars.m_Level);

	BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);
	if (BList == NULL)
	{
		MessageBox(NULL, "BList == NULL", "GEditProDoc RenderOrthoView", MB_OK);
		return;
	}

	inidx	=Render_GetInidx(v);

	Box3d_SetBogusBounds (&ViewBox);

	Render_ViewToWorld(v, 0, 0, &XTemp);
	Box3d_AddPoint (&ViewBox, XTemp.X, XTemp.Y, XTemp.Z);

	Render_ViewToWorld(v, Render_GetWidth(v), Render_GetHeight(v), &XTemp);
	Box3d_AddPoint (&ViewBox, XTemp.X, XTemp.Y, XTemp.Z);

	VectorToSUB(ViewBox.Min, inidx)	=-FLT_MAX;
	VectorToSUB(ViewBox.Max, inidx)	=FLT_MAX;

	brushDrawData.pViewBox = &ViewBox ;
	brushDrawData.pDC = pDC ;
	brushDrawData.v = v ;
	brushDrawData.pDoc = this;
	brushDrawData.GroupId = 0;
	brushDrawData.FlagTest = NULL;

	// Initialize oldpen.  Don't initialize it anywhere else!
	CPen BlackPen (PS_SOLID, 1, BLACK);
	CPen * const oldpen = pDC->SelectObject (&BlackPen);

	{
		float GridSize, GridSnapSize;
		const Prefs *pPrefs = GetPrefs ();

		CPen	PenGrid (PS_SOLID, 1, Prefs_GetGridColor (pPrefs));
		CPen	PenSnapGrid (PS_SOLID, 1, Prefs_GetSnapGridColor (pPrefs));

		GridSize = Render_GetFineGrid(v, (m_pLevelMgr->GetGridType (&m_CurrentDocVars.m_Level) == GridTexel) ? GRID_TYPE_TEXEL : GRID_TYPE_METRIC);
		if (m_pLevelMgr->GetGridType (&m_CurrentDocVars.m_Level) == GridMetric)
		{
			GridSize /= 2.54f;
		}

		GridSnapSize = m_pLevelMgr->GetGridSnapSize (&m_CurrentDocVars.m_Level);

		/*
		  If the grid size and the snap size are the same, then just render
		  the snap grid.
		  Otherwise we always want to render the regular grid.  If the
		  snap grid is larger than the regular grid, then render it, too.
		*/
		if (GridSize == GridSnapSize)
		{
			pDC->SelectObject (&PenSnapGrid);
			Render_RenderOrthoGridFromSize (v, GridSize, pDC->m_hDC);
		}
		else
		{
			pDC->SelectObject (&PenGrid);
			Render_RenderOrthoGridFromSize (v, GridSize, pDC->m_hDC);
			if (GridSnapSize > GridSize)
			{
				// render snap grid
				pDC->SelectObject (&PenSnapGrid);
				Render_RenderOrthoGridFromSize (v, GridSnapSize, pDC->m_hDC);
			}
		}
		pDC->SelectObject (oldpen);
	}

	// Step through groups, show items by color
	// All - Show all regardless of visiblity, local invisible attributes still over-ride
	// Vis - Show only groups marked visible, local invisible attributes still over-ride
	// Cur - Show current regardless of visible, local invisible attributes still over-ride

	GroupListType *Groups = m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level);

	GroupId	=Group_GetFirstId(Groups, &gi);
	while(GroupId != NO_MORE_GROUPS )
	{
	    brushDrawData.FlagTest = ::fdocBrushNotDetail;
		brushDrawData.GroupId = GroupId;
		if( (GroupVis == Group_ShowAll) ||
			((GroupVis == Group_ShowCurrent) && (GroupId == m_iCurrentGroup)) ||
			((GroupVis == Group_ShowVisible) && (Group_IsVisible (Groups, GroupId)))
		)
		{
			const GE_RGBA * pRGB ;
			pRGB = Group_GetColor( Groups, GroupId ) ;
			CPen	PenThisGroup(PS_SOLID, 1, RGB(pRGB->r,pRGB->g,pRGB->b));

			pDC->SelectObject (&PenThisGroup);
			m_pLevelMgr->EnumLeafBrushes (&m_CurrentDocVars.m_Level, &brushDrawData, ::BrushDraw);
			if( m_iShowEntities == GE_TRUE )
			{
				m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, &brushDrawData, ::EntityDraw);
			}

			// render cut brushes
			pDC->SelectObject (&PenCutBrush);
			brushDrawData.FlagTest = fdocBrushIsSubtract;
			m_pLevelMgr->EnumLeafBrushes (&m_CurrentDocVars.m_Level, &brushDrawData, ::BrushDraw);

			// details
			if (m_bShowDetailBrushes)
			{
				// detail brushes
				pDC->SelectObject (&PenDetailBrush);
				brushDrawData.FlagTest = Brush_IsDetail;
				m_pLevelMgr->EnumLeafBrushes (&m_CurrentDocVars.m_Level, &brushDrawData, ::BrushDraw);
			}

			// hints
			if(m_bShowHintBrushes)
			{
				// Hint brushes
				pDC->SelectObject (&PenHintBrush);
				brushDrawData.FlagTest = Brush_IsHint;
				m_pLevelMgr->EnumLeafBrushes (&m_CurrentDocVars.m_Level, &brushDrawData, ::BrushDraw);
			}


			// clip
			if(m_bShowClipBrushes)
			{
				// Hint brushes
				pDC->SelectObject (&PenClipBrush);
				brushDrawData.FlagTest = Brush_IsClip;
				m_pLevelMgr->EnumLeafBrushes (&m_CurrentDocVars.m_Level, &brushDrawData, ::BrushDraw);
			}

			pDC->SelectObject (&PenAllItems);
		}
		GroupId	=Group_GetNextId(Groups, &gi);
	}

	// find and render the camera entity
	CEntity *pCameraEntity = FindCameraEntity (&m_CurrentDocVars);
	if ((pCameraEntity != NULL) && pCameraEntity->IsVisible ())
	{
		pDC->SelectObject (&PenAllItems);
		fdocDrawEntity (pCameraEntity, v, pDC, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level), GE_FALSE);
	}

	brushDrawData.GroupId = fdoc_SHOW_ALL_GROUPS;
	brushDrawData.FlagTest = NULL;
	{
		CEntityArray *Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
		int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);

		// render selected brushes and entities
		pDC->SelectObject(&PenSelected);

		for(i=0;i < NumSelBrushes;i++)
		{
			Brush *pBrush;

			pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);

		if (pBrush == NULL)	//	post 0.55
		{
			MessageBox(NULL, "Brush == NULL", "GEditProDoc RenderOrthoView", MB_OK);
			return;
		}


			if (fdocShowBrush (pBrush, &ViewBox))
			{
				if(Brush_IsMulti (pBrush))
				{
					const BrushList *BrushList;	//	post 0.55
					BrushList = Brush_GetBrushList(pBrush);
					if (BrushList == NULL)
					{
						MessageBox(NULL, "BrushList == NULL", "GEditProDoc RenderOrthoView", MB_OK);
						return;
					}
					else
						BrushList_EnumLeafBrushes(BrushList, &brushDrawData, ::BrushDraw);
					}
				else
				{
					Render_RenderBrushFacesOrtho(v, pBrush, pDC->m_hDC);
				}
			}
		}


		for(i=0;i < Entities->GetSize();i++)
		{
			CEntity *pEnt;

			pEnt = &(*Entities)[i];

			if (pEnt->IsSelected ())
			{
				fdocDrawEntity (pEnt, v, pDC, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level), (i==m_iCurrentEntity) ? GE_TRUE : GE_FALSE ) ;
			}
		}
	}

	{
		// render sheet faces
		pDC->SelectObject (&PenSheetFaces);
		BrushList_EnumLeafBrushes(BList, &brushDrawData, ::BrushDrawSheetFacesOrtho);
	}
	{
		// render selected faces
		pDC->SelectObject (&PenSelectedFaces);
		BrushList_EnumLeafBrushes(BList, &brushDrawData, ::BrushDrawSelFacesOrtho);
	}
	{
		// template brush/entity
		pDC->SelectObject (&PenTemplate);

		if((m_iModeTool==ID_TOOLS_TEMPLATE)||
			(m_iModeTool==ID_TOOLS_CAMERA && GetSelState()==NOSELECTIONS))
		{
			if(!m_bTempEnt)
			{
				if (Brush_TestBoundsIntersect(m_pCurBrush, &ViewBox))
				{
					if(Brush_IsMulti(m_pCurBrush))
					{
						const BrushList *CurBrushList;	//	post 0.55
						CurBrushList = Brush_GetBrushList(m_pCurBrush);
						if (CurBrushList == NULL)
						{
							MessageBox(NULL, "CurBrushList == NULL", "GEditProDoc RenderOrthoView", MB_OK);
							return;
						}
					else
						BrushList_EnumLeafBrushes(CurBrushList, &brushDrawData, ::BrushDraw);
					}
					else
					{
						Render_RenderBrushFacesOrtho(v, m_pCurBrush, pDC->m_hDC);
					}
				}
			}
			else
			{
				fdocDrawEntity (&m_RegularEntity, v, pDC, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level), GE_FALSE );
			}
		}
	}

	pDC->SelectObject(oldpen);
}/* CGEditProDoc::RenderOrthoView */

void CGEditProDoc::OnUpdateBrushAdjustmentmode(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( TRUE); //( m_iModeTool == ID_GENERALSELECT ) ? TRUE : FALSE ) ;
	pCmdUI->SetCheck( ( m_AdjustMode == ADJUST_MODE_BRUSH ) ? TRUE : FALSE ) ;
}

void CGEditProDoc::OnToolsBrushAdjustmentmode()
{
	if( m_iCurrentTool == CURTOOL_NONE )
	{
		SetAdjustmentMode( ADJUST_MODE_BRUSH ) ;
	}

	UpdateAllViews(UAV_ALL3DVIEWS, NULL);

}

void CGEditProDoc::OnUpdateFaceAdjustmentmode(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( TRUE); //( m_iModeTool == ID_GENERALSELECT ) ? TRUE : FALSE ) ;
	pCmdUI->SetCheck( (m_AdjustMode==ADJUST_MODE_FACE) ? TRUE : FALSE ) ;
}

void CGEditProDoc::OnToolsFaceAdjustmentmode()
{
		//	0.57	for informing user about this operatin and AutoRebuild
	static int	iCount;

	CString	message = _T("All of your selected faces will appear to be 'Untextured'\n");
			message += _T("while you are NOT in AutoRebuild mode.\n\n");
			message	+= _T("If you need to make adjustments to ALL of the faces\n");
			message += _T("on the selected brushe(s), change to AutoRebuild mode before\n");
			message +=_T("entering Face Adjustment mode.\n\n");
			message += _T("Would you like to change to AutoRebuild mode now?\n");
			message += _T("YES = Change to AutoRebuild mode now so you can see textures on all faces.\n");
			message += _T("NO = Proceed without changing to AutoRebuild mode.\n");
			message += _T("CANCEL = abort this operation.");

	if ((!CGlobals::g_bRebuildAlways) && iCount <1)
	{
		iCount++;
		int result = 0;
		result = AfxMessageBox(message, MB_YESNOCANCEL);
		switch (result)
		{
		case IDYES:
			OnToggleRebuildBspMode();
			break;
		case IDNO:
			break;
		case IDCANCEL:
			return;
		}
	}


	if( m_iCurrentTool == CURTOOL_NONE)
	{
		UpdateAllViews( UAV_ALL3DVIEWS | REBUILD_QUICK, NULL );
		SetAdjustmentMode( ADJUST_MODE_FACE ) ;
	}

			//	post 0.57	for putting 'Rebuild' button on Texture view
	if (!CGlobals::g_bRebuildAlways)
	{
		m_pMainFrame->SendMessageToDescendants(WM_COMMAND, IDC_BTN_REBUILD_COMMAND);
		CGlobals::g_iRebuildClickCount = 0;
	}



}

void CGEditProDoc::OnToolsToggleadjustmode()
{
	if( m_iCurrentTool == CURTOOL_NONE )
	{
		SetAdjustmentMode( ADJUST_MODE_TOGGLE ) ;	// Flip between Brush & face
	}

}

void CGEditProDoc::OnUpdateToolsToggleadjustmode(CCmdUI* pCmdUI)
{
	pCmdUI->Enable (TRUE);//(m_iModeTool == ID_GENERALSELECT) ? TRUE : FALSE);
}


geBoolean CGEditProDoc::FindClosestBrush
	(
	  POINT const *ptFrom,
	  ViewVars *v,
	  Brush **ppFoundBrush,
	  geFloat *pMinEdgeDist
	)
{
	// determine the distance to the closest brush edge in the current view.
	FindClosestInfo	fci;

	*pMinEdgeDist = FLT_MAX;
	*ppFoundBrush = NULL;

	fci.pDoc			=this;
	fci.v				=v;
	fci.ppFoundBrush	=ppFoundBrush;
	fci.pMinEdgeDist	=pMinEdgeDist;
	fci.ptFrom			=ptFrom;

	BrushList_EnumLeafBrushes(m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level), &fci, ::FindClosestBrushCB);

	return	(*ppFoundBrush)? GE_TRUE : GE_FALSE;
}

geBoolean CGEditProDoc::FindClosestEntity
(
 POINT const *ptFrom,
 ViewVars *v,
 CEntity **ppMinEntity,
 geFloat *pMinEntityDist
 )
{
	geBoolean rslt;
	CEntityArray *pEntities = NULL;

	pEntities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
	rslt = GE_FALSE;
	// determine distance to closest entity in the current view
	*pMinEntityDist = FLT_MAX;
	*ppMinEntity = NULL;

	if (pEntities)
	{
		for (int i = 0; i < pEntities->GetSize(); ++i)
		{
			CEntity *pEnt = NULL;
			POINT EntPosView;
			geFloat Dist;
			int dx, dy;

			pEnt = &(*pEntities)[i];

			if (pEnt)
			{
				if (EntityIsVisible( pEnt ))
				{
					EntPosView = Render_OrthoWorldToView (v, &pEnt->mOrigin);
					dx = EntPosView.x - ptFrom->x;
					dy = EntPosView.y - ptFrom->y;

					Dist = (geFloat)((dx*dx) + (dy*dy));
					if (Dist < *pMinEntityDist)
					{
						*pMinEntityDist = Dist;
						*ppMinEntity = pEnt;
						rslt = GE_TRUE;
					}
				}
			}
		}

		if (rslt)
		{
			*pMinEntityDist = (geFloat)sqrt (*pMinEntityDist);
		}
		return rslt;
	}
	return rslt;
}

int CGEditProDoc::FindClosestThing(POINT const *ptFrom, ViewVars *v,
	  Brush **ppMinBrush,		// NULL OK
	  CEntity **ppMinEntity,	// NULL OK
	  geFloat *pDist
	)
{
	int rslt;

	geBoolean FoundBrush;
	geFloat MinEdgeDist;
	Brush *pMinBrush = NULL;

	geBoolean FoundEntity;
	geFloat MinEntityDist;
	CEntity *pMinEntity = NULL;

	rslt = fctNOTHING;
	FoundBrush = FindClosestBrush (ptFrom, v, &pMinBrush, &MinEdgeDist);
	FoundEntity = FindClosestEntity (ptFrom, v, &pMinEntity, &MinEntityDist);


	if (FoundEntity)
	{
		if ((!FoundBrush) || (MinEntityDist < MinEdgeDist))
		{
			*pDist = MinEntityDist;
			if( ppMinEntity != NULL )
				*ppMinEntity = pMinEntity;
			rslt = fctENTITY;
		}
		else
		{
			*pDist = MinEdgeDist;
			if( ppMinBrush != NULL )
				*ppMinBrush = pMinBrush;
			rslt = fctBRUSH;
		}
	}
	else if (FoundBrush)
	{
		*pDist = MinEdgeDist;
		if( ppMinBrush != NULL )
			*ppMinBrush = pMinBrush;
		rslt = fctBRUSH;
	}
	return rslt;
}
					//	brushSelToggle | brushSelAlways
void CGEditProDoc::DoBrushSelection(Brush* pBrush, BrushSel nSelType )
{
	int ModelId = 0;
	geBoolean bModelLocked;
	ModelInfo_Type *pModelInfo = NULL;
	GroupListType *pGroups = NULL;
	int GroupId = 0;
	geBoolean bGroupLocked;
	BrushList *pBList = NULL;
	Brush	*pBParent = NULL;

	pModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);
	pGroups = m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level);
	pBList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);

	if (pModelInfo && pGroups && pBList && pBrush)
	{

		if(Brush_GetParent(pBList, pBrush, &pBParent))
		{
			pBrush	=pBParent;
		}

		bModelLocked = GE_FALSE;
		bGroupLocked = FALSE;
		if(m_AdjustMode != ADJUST_MODE_FACE)
		{
			// don't do this stuff if we're in face mode...
			ModelId = Brush_GetModelId (pBrush);
			if (ModelId != 0)
			{
				Model *pModel = NULL;

				pModel = ModelList_FindById (pModelInfo->Models, ModelId);
				if (pModel)
				{
					bModelLocked = Model_IsLocked (pModel);
				}
			}

			if (!bModelLocked)
			{
				GroupId = Brush_GetGroupId (pBrush);
				if (GroupId != 0)
				{
					bGroupLocked = Group_IsLocked (pGroups, GroupId);
				}
			}
		}

		if( nSelType == brushSelToggle && BrushIsSelected (pBrush) )
		{
			if (bModelLocked)
			{
				// model is locked, so deselect everything in the model
				SelectModelBrushes (FALSE, ModelId);
			}
			else if (bGroupLocked)
			{
				// group is locked, so deselect entire group
				SelectGroupBrushes (FALSE, GroupId);
			}
			else
			{
				SelBrushList_Remove (m_CurrentDocVars.m_pSelBrushes, pBrush);
// changed QD
				if(strstr(Brush_GetName(pBrush),".act")!=NULL)
				{
					CEntityArray *Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

					for(int i=0;i < Entities->GetSize();i++)
					{
						Brush *b=(*Entities)[i].GetActorBrush();
						if(b!=NULL)
							if(SelBrushList_Find (m_CurrentDocVars.m_pSelBrushes, b))
								if ((*Entities)[i].IsSelected())
								{
									(*Entities)[i].DeSelect();
									--m_iNumSelEntities;
								}
					}
				}
// end change
			}
		}
		else
		{
			if (bModelLocked)
			{
				// model is locked, so select everything in the model
				SelectModelBrushes (TRUE, ModelId);
			}
			else if (bGroupLocked)
			{
				// group is locked.  Select everything in the group
				SelectGroupBrushes (TRUE, GroupId);
			}
			else
			{
				SelBrushList_Add (m_CurrentDocVars.m_pSelBrushes, pBrush);
// changed QD
				if(strstr(Brush_GetName(pBrush),".act")!=NULL)
				{
					CEntityArray *Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

					for(int i=0;i < Entities->GetSize();i++)
					{
						Brush *b=(*Entities)[i].GetActorBrush();
						if(b!=NULL)
							if(SelBrushList_Find (m_CurrentDocVars.m_pSelBrushes, b))
								if (!(*Entities)[i].IsSelected())
								{
									(*Entities)[i].Select();
									++m_iNumSelEntities;
								}
					}
				}
// end change
			}
		}
	}
}/* CGEditProDoc::DoBrushSelection */

void CGEditProDoc::SelectEntity(CEntity *pEntity)
{
	if (pEntity)
	{
		if (!pEntity->IsSelected ())
		{
			pEntity->Select ();
// changed QD Actors
			Brush *pBrush;
			pBrush = pEntity->GetActorBrush();
			if(pBrush!=NULL)
			{
				SelBrushList_Add (m_CurrentDocVars.m_pSelBrushes, pBrush);
			}
// end change
			++m_iNumSelEntities;
		}
	}
}

void CGEditProDoc::DeselectEntity(CEntity *pEntity)
{
	if (pEntity)
	{
		if (pEntity->IsSelected ())
		{
			pEntity->DeSelect ();
// changed QD
			Brush *pBrush;
			pBrush = pEntity->GetActorBrush();
			if(pBrush!=NULL)
			{
				SelBrushList_Remove(m_CurrentDocVars.m_pSelBrushes, pBrush);
			}
// end change
			--m_iNumSelEntities;
			if (m_iNumSelEntities < 0)
			{
				m_iNumSelEntities = 0;
			}
		}
	}
}

void CGEditProDoc::DoEntitySelection(CEntity *pEntity)
{
	// an entity is closest.  Select/deselect it.
	int GroupId;
	geBoolean GroupLocked;
	GroupListType *pGroups = NULL;
	pGroups = m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level);

	if (pEntity && pGroups)
	{
		GroupLocked = FALSE;
		GroupId = pEntity->GetGroupId ();
		if (GroupId != 0)
		{
			GroupLocked = Group_IsLocked (pGroups, GroupId);
		}


		if (pEntity->IsSelected ())
		{
			if (GroupLocked)
			{
				// deselect entire group
				SelectGroupBrushes (FALSE, GroupId);
			}
			else
			{
				DeselectEntity (pEntity);
			}
		}
		else
		{
			if (GroupLocked)
			{
				// select entire group
				SelectGroupBrushes (TRUE, GroupId);
			}
			else
			{
				SelectEntity (pEntity);
			}
		}
	}
}

void CGEditProDoc::ResetAllSelectedEntities()
{
	DoGeneralSelect ();	// calls ConfigureCurrentTool()

	m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, this, ::fdocDeselectEntity);
	m_iNumSelEntities = 0;
}


void CGEditProDoc::SelectAll (void)
{
	DoGeneralSelect ();	//	calls ConfigureCurrentTool(), which calls UpdateAllViews(),
						//	Also calls UpdateSelected(), which calls UpdateMainControls()

	m_iNumSelEntities = 0;
	m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, this, ::fdocSelectEntity);
	m_pLevelMgr->EnumBrushes (&m_CurrentDocVars.m_Level, this, ::fdocSelectBrush);

	UpdateSelected();	//	calls UpdateMainControls()
}


BOOL CGEditProDoc::IsEntitySelected(void)
{
	CEntityArray *pEntities = NULL;

	pEntities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	if (pEntities)
	{
		for( int iEnt = 0; iEnt < pEntities->GetSize(); iEnt++ )
		{
			if ((*pEntities)[ iEnt ].IsSelected ())
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//	CHANGE!	04/16/97	John Moore
//	Sets the entity with the designated ID as the current entity...
void CGEditProDoc::SetSelectedEntity( int ID )
{
	CEntityArray *pTempEntities = NULL;

	if (m_pLevelMgr)
	{
		pTempEntities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
		if (pTempEntities)
		{
			ResetAllSelectedEntities();
			m_iCurrentEntity = ID;
			SelectEntity (&(*pTempEntities)[ID]);
			UpdateSelected() ;	//	calls UpdateMainControls()
		}
	}
}
//	End of CHANGE

void CGEditProDoc::DeleteEntity(int EntityIndex)
{
	CEntityArray *Entities = NULL;

	// now delete the entity, we'll do the fixups later
	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	DeselectEntity (&(*Entities)[EntityIndex]);
// changed QD Actors
	Brush *b = (*Entities)[EntityIndex].GetActorBrush();
	if(b!=NULL)
	{
		SelBrushList_Remove(m_CurrentDocVars.m_pSelBrushes, b);
		m_pLevelMgr->RemoveBrush(&m_CurrentDocVars.m_Level, b);
	}
// end change
	Entities->RemoveAt( EntityIndex );
	m_dwSelState&=(~ENTITYCLEAR);
	m_dwSelState|=(m_iNumSelEntities >1)? MULTIENTITY : (m_iNumSelEntities+1)<<7;
}

void CGEditProDoc::AdjustEntityAngle( const ViewVars * v, const geFloat dx )
{
	CEntity *	pEnt = NULL;
	geVec3d		Vec ;
	geVec3d		Angles ;
	geVec3d_Clear(&Vec);
	geVec3d_Clear(&Angles);
	ViewVars *pTempViewVars = NULL;
	CEntityArray *pEntities = NULL;

	pEntities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
	if (pEntities)
	{
		pEnt = &(*pEntities)[m_iCurrentEntity];
		if (pEnt)
		{
			if (pEnt->IsCamera ())
			{
				Render_ViewDeltaToRotation ( v, -dx, &Vec);
				// disallow roll for camera
				Vec.Z = 0.0f;
				// if the camera is upside down, then negate yaw
				if (Render_UpIsDown (v))
				{
					Vec.Y = -Vec.Y;
				}
			}
			else
			{
				Render_ViewDeltaToRotation ( v, dx, &Vec);
			}

			pEnt->GetAngles( &Angles, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) ) ;
			geVec3d_Add (&Angles, &Vec, &Angles);
			pEnt->SetAngles( &Angles, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) ) ;

			if (pEnt->IsCamera())
			{
				SetCameraDocumentCoords(&pEnt->mOrigin, &Angles);

				SetRenderedViewCamera( &(pEnt->mOrigin), &Angles) ;
				//	Be very careful when speccing flags for UpdateAllViews()
				//	The wrong flags at the wrong time will totally screw things up
				UpdateAllViews(UAV_RENDER_ONLY, NULL);
			}


		}
	}

}

void CGEditProDoc::AdjustEntityArc( const ViewVars * v, const geFloat dx )
{
	CEntity *	pEnt ;
	geFloat		fArc ;
	geFloat		fArcDelta ;
	CEntityArray *Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	pEnt = &(*Entities)[m_iCurrentEntity];

	pEnt->GetArc( &fArc, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) ) ;

	fArcDelta = Render_ViewDeltaToRadians( v, dx ) ;
	fArc -= fArcDelta;
	if (fArc > 2*M_PI)
	{
		fArc -= 2*M_PI;
	}
	if (fArc < 0)
	{
		fArc += 2*M_PI;
	}

	pEnt->SetArc( fArc, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) ) ;
}

void  CGEditProDoc::AdjustEntityRadius( const geVec3d *pVec )
{
	CEntity *	pEnt ;
	geFloat		fRadius ;
	CEntityArray *Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	pEnt = &(*Entities)[m_iCurrentEntity];

	pEnt->GetRadius( &fRadius, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) ) ;
	fRadius += geVec3d_Length (pVec);
	if( fRadius < 0.0f )
	{
		fRadius = 0.0f ;
	}
	pEnt->SetRadius( fRadius, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) ) ;
}


void CGEditProDoc::SelectOrtho(CPoint point, ViewVars *v)
{
	Brush *pMinBrush = NULL;
	CEntity *pMinEntity = NULL;
	geFloat Dist;
	int FoundThingType;

	if(IsSelectionLocked())
	{
		return;
	}

	// if Control key isn't pressed, then clear all current selections
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0)
	{
		ResetAllSelections ();
	}

	FoundThingType = FindClosestThing (&point, v, &pMinBrush, &pMinEntity, &Dist);
	if ((FoundThingType != fctNOTHING) && (Dist <= MAX_PIXEL_SELECT_DIST))
	{
		switch (FoundThingType)
		{
			case fctBRUSH :
				DoBrushSelection (pMinBrush, brushSelToggle);
				break;
			case fctENTITY :
				DoEntitySelection (pMinEntity);
				break;
			default :
				// bad value returned from FindClosestThing
				assert (0);
		}
	}
	if (SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes) == 0)
	{
		DeleteBrushAttributes ();
	}

	UpdateSelected ();	//	calls UpdateMainControls)()
}


void CGEditProDoc::SelectOrthoRect(CPoint ptStart, CPoint ptEnd, ViewVars *v)
{
	Brush		*	pBrush = NULL;
	CEntity		*	pEnt = NULL;
	int				i ;
	BrushIterator	bi;
	POINT			Min ;
	POINT			Max ;
	POINT			EntPosView;
	CRect			viewRect( ptStart, ptEnd ) ;		//Selection Rect in view coords
	BrushList	*	BList = NULL;
	BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);

	viewRect.NormalizeRect() ;

	if(IsSelectionLocked())
	{
		return;
	}

	// if Control key isn't pressed, then clear all current selections
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0)
	{
		ResetAllSelections ();
	}

	// Should use ENUM function here
	if (BList)
	{
		pBrush = BrushList_GetFirst (BList, &bi);
		while (pBrush != NULL)
		{
			if (BrushIsVisible (pBrush))
			{
				const Box3d	*bbox	=Brush_GetBoundingBox(pBrush);
				Min = Render_OrthoWorldToView ( v, &bbox->Min ); // Get Brush BB in View Coords
				Max = Render_OrthoWorldToView ( v, &bbox->Max );
				if( viewRect.PtInRect( Min ) && viewRect.PtInRect( Max ) )		// If Brush ENTIRELY in rect...
				{
					DoBrushSelection( pBrush, brushSelAlways ) ;
				}
			}
			pBrush = BrushList_GetNext (&bi);
		}
	}
	CEntityArray *Entities = NULL;
	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
	if (Entities)
	{
		for( i = 0; i < Entities->GetSize(); ++i)
		{
			pEnt = NULL;
			pEnt = &(*Entities)[i];
			if (pEnt)
			{
				if (EntityIsVisible( pEnt) )
				{
					EntPosView = Render_OrthoWorldToView (v, &pEnt->mOrigin);
					if( pEnt->IsSelected () == GE_FALSE )
					{
						if( viewRect.PtInRect( EntPosView ) )
						{
							DoEntitySelection( pEnt );
						}
					}
				}
			}
		}
	}
	UpdateSelected ();	//	calls UpdateMainControls()

}/* CGEditProDoc::SelectOrthoRect */


void CGEditProDoc::PlaceTemplateEntity3D(CPoint point, ViewVars *v)
{
	BrushList	*BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);
	SelectBrush3DCBData	bdat;
	geVec3d ClickPosWorld;
	const Plane	*p;

	if (BList == NULL)	//	post 0.55
	{
		MessageBox(NULL, "BrushList == NULL", "GEditProDoc PlaceTemplateEntity3D", MB_OK);
		return;
	}

	Render_ViewToWorld(v, point.x, point.y, &bdat.vp);
	Render_BackRotateVector(v, &bdat.vp, &bdat.wp);
	Render_GetCameraPos(v, &bdat.vp);

	bdat.MinBDist	=999999.0f;
	bdat.m_pCurBrush	=NULL;
	bdat.pDoc		= this;
	BrushList_EnumCSGBrushes(BList, &bdat, SelectBrush3DCB);

	if(bdat.m_pCurBrush)
	{
		if(bdat.CurFace)
		{
			geFloat	pDist;

			//find the dist ratio from click ray to plane normal
			p		=Face_GetPlane(bdat.CurFace);
			pDist	=geVec3d_DotProduct(&bdat.wp, &p->Normal);
			if(pDist != 0.0f)
			{
				//grab plane distance and move inward by 16
				pDist	=(Face_PlaneDistance(bdat.CurFace, &bdat.vp) - 16.0f) / pDist;
			}

			geVec3d_Scale(&bdat.wp, pDist, &ClickPosWorld);

			//add in distances from the camera, and current entity position
			geVec3d_Subtract(&ClickPosWorld, &bdat.vp, &ClickPosWorld);

			geVec3d_Inverse (&ClickPosWorld);
			m_RegularEntity.mOrigin = ClickPosWorld;
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			UpdateAllViews( UAV_ALL3DVIEWS, NULL );
		}
	}
}

//slow version checking brushes
void CGEditProDoc::SelectRay(CPoint point, ViewVars *v)
{
	int						CurEnt = 0;
	geFloat					MinEDist = 0.0f;
	CEntityArray			*Entities = NULL;
	BrushList				*BList = NULL;
	SelectBrush3DCBData		bdat;
	geVec3d					ClickPosWorld;
	ClickPosWorld.X = ClickPosWorld.Y = ClickPosWorld.Z = 0.0f;
	geBoolean				EntitySelected	= FALSE;

	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
	BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);

	if (BList == NULL)
	{
		MessageBox(NULL, "BrushList == NULL", "RFEditProDoc SelectRay", MB_OK);
		return;
	}

	Render_ViewToWorld(v, point.x, point.y, &bdat.vp);
	Render_BackRotateVector(v, &bdat.vp, &bdat.wp);
	Render_GetCameraPos(v, &bdat.vp);

	Render_ViewToWorld (v, point.x, point.y, &ClickPosWorld);

	MinEDist = bdat.MinBDist = 999999.0f;

	for(int i=0;i < Entities->GetSize();i++)
	{
		CEntity *pEnt = NULL;

		pEnt = &(*Entities)[i];
		if (pEnt)
		{
			if (EntityIsVisible (pEnt))
			{
				bdat.CurDist = pEnt->RayDistance (point, v);
				if ((bdat.CurDist < 900.0f)
					&& (bdat.CurDist < MinEDist)
					&& (bdat.CurDist > MIN_ENTITY_SELECT_DIST))
				{
					MinEDist=bdat.CurDist;
					CurEnt	= i;
				}
			}
		}
	}
	bdat.m_pCurBrush	= NULL;
	bdat.CurFace		= NULL;
	bdat.pDoc			= this;
	BrushList_EnumCSGBrushes(BList, &bdat, SelectBrush3DCB);

	if((bdat.MinBDist < 999999.0f) && (MinEDist > MIN_ENTITY_SELECT_DIST))
	{
		//check the distance of the hit wall
		//see if the closest entity is occluded
		if(bdat.m_pCurBrush)
		{
			if(bdat.CurFace)
			{
				geFloat		fDist = 0.0f;
				const Plane	*pPlane = NULL;

				//find the dist ratio from click ray to plane normal
				pPlane	= Face_GetPlane(bdat.CurFace);

				if (pPlane)
				{
					fDist = geVec3d_DotProduct(&bdat.wp, &pPlane->Normal);

					if (fDist != 0.0f)
					{
						fDist	=(Face_PlaneDistance(bdat.CurFace, &bdat.vp)  / fDist);

						geVec3d_Scale(&bdat.wp, fDist, &ClickPosWorld);
						fDist = geVec3d_Length(&ClickPosWorld);

						if (MinEDist < fDist)
						{
							if((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0)
							{
								ResetAllSelections ();
							}
							DoEntitySelection(&(*Entities)[CurEnt]);
							EntitySelected = GE_TRUE;
						}
					}
				}
			}
		}
	}
	if(!EntitySelected && bdat.m_pCurBrush)
	{
		// if Control key isn't pressed, then clear all current selections
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0)
		{
			ResetAllSelections ();
		}

		//	At this point, bdat.m_pCurBrush points to the CSG brush that contains
		//	the hit face.  We want the leaf brush that contains the CSG face
		//	that was hit.

		//	So, we search the entire brush list looking for a leaf face (non-cut)
		//	that has a face that's coplanar with the hit face, and that contains
		//	the impact point.  If none is found, we then search for a matching
		//	cut brush face (again, in leafs) that contains the point.
		{
			const Plane				*pPlane = NULL;
			geFloat					fDist = 0.0f;
			geVec3d					vecClickPosWorld;
			vecClickPosWorld.X = vecClickPosWorld.Y = vecClickPosWorld.Z = 0.0f;
			FaceSearchCallbackData	fsData;

			if (Brush_GetType (bdat.m_pCurBrush) == BRUSH_LEAF)
			{
				// if the found face is on a leaf brush, then skip the rest of the search
				fsData.pFoundFace = bdat.CurFace;
				fsData.Found = GE_TRUE;
				fsData.pFoundBrush = bdat.m_pCurBrush;
			}

			else
			{
				// determine the impact point
				pPlane	= Face_GetPlane(bdat.CurFace);
				if (pPlane)
				{
					fDist	= geVec3d_DotProduct(&bdat.wp, &pPlane->Normal);
					if (fDist != 0.0f)
					{
						//grab plane distance and move inward by 16
						fDist	= (Face_PlaneDistance(bdat.CurFace, &bdat.vp)) / fDist;
					}

					geVec3d_Scale(&bdat.wp, fDist, &ClickPosWorld);

					//add in distance from the camera
					geVec3d_Subtract(&ClickPosWorld, &bdat.vp, &ClickPosWorld);
					geVec3d_Inverse (&ClickPosWorld);

					// OK, now search list for a face that contains this point and
					// is coplanar with the matched face.
					fsData.pFacePlane = pPlane;
					fsData.Found = GE_FALSE;
					fsData.pFoundFace = NULL;
					fsData.pFoundBrush = NULL;
					fsData.ImpactPoint = ClickPosWorld;

					BrushList_EnumLeafBrushes (BList, &fsData, ::fdocFindLeafFace);
					if (!fsData.Found)
					{
						BrushList_EnumLeafBrushes (BList, &fsData, ::fdocFindAnyFace);
					}
				}
			}
			if (fsData.Found)
			{
				//	reset the click-try count to 0
				CGlobals::g_iRebuildClickCount = 0;

				// We found the hit face.
				if (m_AdjustMode == ADJUST_MODE_BRUSH)
				{
					DoBrushSelection (fsData.pFoundBrush, brushSelToggle);
				}
				else
				{
					DoBrushSelection (fsData.pFoundBrush, brushSelAlways);
					// if the face is already in the list, then remove it
					if (SelFaceList_Remove (m_CurrentDocVars.m_pSelFaces, fsData.pFoundFace))
					{
						Face_SetSelected (fsData.pFoundFace, GE_FALSE);
					}
					else
					{
						Face_SetSelected (fsData.pFoundFace, GE_TRUE);
						SelFaceList_Add (m_CurrentDocVars.m_pSelFaces, fsData.pFoundFace);
					}
					// Deselect any brush that doesn't have selected faces
					int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);
					for (int i = NumSelBrushes-1; i >= 0; --i)
					{
						Brush *pBrush = NULL;

						pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);

						if (pBrush)
						{
							if (Brush_GetSelectedFace (pBrush) == NULL)
							{
								SelBrushList_Remove (m_CurrentDocVars.m_pSelBrushes, pBrush);
							}
						}
					}
				}
			}
			if (!fsData.Found)
			{
				//	keep track of the number of missed hits
				CGlobals::g_iRebuildClickCount++;
				//	need to do this because tools still think there are
				//	brushes selected. This makes it possible to select anew.
				m_iCurrentTool = CURTOOL_NONE;
				if (CGlobals::g_iRebuildClickCount  > 2)
				{
					m_pMainFrame->SendMessage(WM_COMMAND, IDC_BTN_REBUILD_COMMAND);
					CGlobals::g_iRebuildClickCount = 0;
				}
			}
		}
	}

	if ((bdat.CurFace == NULL) ||
		((m_AdjustMode == ADJUST_MODE_FACE) && (SelFaceList_GetSize (m_CurrentDocVars.m_pSelFaces) == 0)))
	{
		DeleteFaceAttributes ();
		DeleteBrushAttributes ();
	}

	UpdateSelected();	//	Repositioned here. calls m_pMainFrame->UpdateMainControls()

}


//selects the texture of the face clicked (doesn't select the face)
void CGEditProDoc::SelectTextureFromFace3D(CPoint point, ViewVars *v)
{
	BrushList	*BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);
	SelectBrush3DCBData	bdat;

	if (BList == NULL)	//	post 0.55
	{
		MessageBox(NULL, "BrushList == NULL", "GEditProDoc SelectTextureFromFace3D", MB_OK);
		return;
	}

	Render_ViewToWorld(v, point.x, point.y, &bdat.vp);
	Render_BackRotateVector(v, &bdat.vp, &bdat.wp);
	Render_GetCameraPos(v, &bdat.vp);

	bdat.MinBDist	=999999.0f;
	bdat.m_pCurBrush	=NULL;
	bdat.pDoc		= this;
	BrushList_EnumCSGBrushes(BList, &bdat, SelectBrush3DCB);

	if(bdat.m_pCurBrush)
	{
		if(bdat.CurFace)
		{
			m_pMainFrame->m_TexturesTab.SelectTexture(Face_GetTextureDibId(bdat.CurFace));
		}
	}
}

void CGEditProDoc::UpdateFaceAttributesDlg (void)
{
	if (m_pMainFrame->m_dPropFaceAttributesDialog)	//	 new g3dc
	{
		m_pMainFrame->UpdateMainControls();

	}
}

void CGEditProDoc::UpdateBrushAttributesDlg (void)
{
	if(m_pMainFrame->m_dPropBrushAttributesDialog)
	{
		m_pMainFrame->UpdateMainControls();
	}

}

void CGEditProDoc::UpdateCameraEntity( const ViewVars *v )
{
	CEntity *	pEnt = NULL ;
	geVec3d		Vec ;

	pEnt = EntityList_FindByClassName( m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level), "Camera" ) ;
	if( pEnt )
	{
		Render_GetCameraPos( v, &Vec ) ;
		pEnt->SetOrigin( Vec.X, Vec.Y, Vec.Z, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) ) ;
		Render_GetPitchRollYaw( v, &Vec ) ;
		pEnt->SetAngles( &Vec, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level) ) ;

		//	set the document's camera coordinates so that the camera doesn't move around
		//	following Undo / Redo activity
		geVec3d	vecOrigin, vecAngles;
		geVec3d_Clear(&vecOrigin);
		geVec3d_Clear(&vecAngles);
		vecOrigin = pEnt->mOrigin;
		pEnt->GetAngles(&vecAngles, m_pLevelMgr->GetEntityDefs(&m_CurrentDocVars.m_Level));
		SetCameraDocumentCoords(&vecOrigin, &vecAngles);

		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		UpdateAllViews( UAV_ALL3DVIEWS, NULL );
	}

}/* CGEditProDoc::MoveCamera */

void CGEditProDoc::SetRenderedViewCamera( const geVec3d * pVec, const geVec3d * pPRY )
{
	POSITION		pos;
	CGEditProView	*	pView;

	pos = GetFirstViewPosition();
	while( pos != NULL )
	{
		pView = (CGEditProView*)GetNextView(pos) ;
		if( Render_GetViewType( pView->VCam ) & (VIEWSOLID|VIEWTEXTURE|VIEWWIRE) )
		{
			Render_SetPitchRollYaw( pView->VCam, pPRY ) ;
			Render_SetCameraPos( pView->VCam, pVec ) ;
			break ;	// Only 1 rendered view for now
		}
	}
}/* CGEditProDoc::SetRenderedViewCamera */

	//	post 0.5 release // this whole function has been reworked
void CGEditProDoc::GetCursorInfo(char *info, int MaxSize)
{
	CGEditProView	*pView = NULL;
	CPoint			CursorPos, ViewCursorPos;
	CRect			ClientRect;
	POSITION		pos;
	geBoolean		ShiftHeld;

	if (m_bPrintingView)
		return;

	pos = GetFirstViewPosition();
	info[0]	= 0;
	ShiftHeld = CGlobals::IsKeyDown(VK_SHIFT);

	GetCursorPos(&CursorPos);

	while(pos != NULL)
	{
		pView = (CGEditProView*)GetNextView(pos);
		if (pView)
		{
			pView->GetClientRect(&ClientRect);
			pView->ClientToScreen(&ClientRect);


		if(ClientRect.PtInRect(CursorPos))
		{
				//	Not testing for active view using the m_iActiveView param
			ViewCursorPos = CursorPos;
			pView->ScreenToClient(&ViewCursorPos);

			if(Render_GetViewType(pView->VCam) < VIEWTOP) //	cursor is in 3d viewport
			{
				if (!ShiftHeld)	//	if we're not trying to shift-click a texture
				{
					SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW));
					return;
				}
			}
				//	otherwise, cursor is in one of the three ortho viewports
			else
			{
				if(!pView->IsPanning && m_iCurrentTool!=ID_TOOLS_BRUSH_SCALEBRUSH
					&& m_iCurrentTool!=ID_TOOLS_BRUSH_SHEARBRUSH)
				{
					int			FoundThingType;
					Brush		*pMinBrush = NULL;
					CEntity		*pMinEntity = NULL;
					geFloat		Dist;

					FoundThingType = FindClosestThing (&ViewCursorPos, pView->VCam, &pMinBrush, &pMinEntity, &Dist);

					if ((FoundThingType == fctNOTHING) || (Dist > MAX_PIXEL_SELECT_DIST) || (m_iCurrentTool != CURTOOL_NONE))
					{
						;;	//	don't do anything
					}
						//	otherwise, we found something, or we're very close to something
					else
					{
						switch (FoundThingType)
						{
						case fctBRUSH :
							SetCursor(AfxGetApp()->LoadCursor(IDC_ARROWBRUSH));
							strncpy (info, Brush_GetName(pMinBrush), MaxSize);
							break;

						case fctENTITY :
							SetCursor(AfxGetApp()->LoadCursor(IDC_ARROWENTITY));
							strncpy (info, pMinEntity->GetName (), MaxSize);
							break;
							default :
								// bad value returned from FindClosestThing
								assert (0);
						}
					}
				}
			}
		}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//	SetPrintingState
//
/////////////////////////////////////////////////////////////////////////////////////
bool	CGEditProDoc::SetPrintingState(bool bState)
{
	m_bPrintingView = bState;	//	set a flag for tracking printing status
	return true;
}



/////////////////////////////////////////////////////////////////////////////////////
//	GetPrintingState
//
/////////////////////////////////////////////////////////////////////////////////////
bool	CGEditProDoc::GetPrintingState()
{
	return m_bPrintingView;
}







void CGEditProDoc::OnSelectedTypeCmdUI(CCmdUI* pCmdUI)
{
	if(m_iModeTool==ID_TOOLS_BRUSH_ADJUSTMENTMODE) pCmdUI->Enable();
	else pCmdUI->Enable(0);
}

// NO UI EXISTS FOR THIS FUNCTION
void CGEditProDoc::OnBrushSelectedCopytocurrent()
{
//	ConPrintf("Temporarily Disabled...\n");	//	old gedit
	m_pMainFrame->ConPrintf("Temporarily Disabled...\n");	//	new g3dc
	return;
}

BOOL CGEditProDoc::DeleteSelectedBrushes(void)
{
	CGEditProDocVars thisFunctionDocVars;
	thisFunctionDocVars = DocVarsCreateUniqueCopy(m_CurrentDocVars);

	geBoolean	bAlteredCurrentGroup = GE_FALSE ;
	CEntityArray *pEntities = NULL;
	pEntities = m_pLevelMgr->GetEntities (&thisFunctionDocVars.m_Level);

	if (pEntities)
	{
		for(int Ent=0;Ent < pEntities->GetSize() && (!(GetSelState() & NOENTITIES)); Ent++)
		{
			BOOL	bIsSelected = (*pEntities)[Ent].IsSelected();
			if((*pEntities)[Ent].IsSelected ())
			{
				if( (*pEntities)[Ent].IsCamera() == GE_FALSE )	// Exclude Cameras
				{
					if( (*pEntities)[Ent].GetGroupId() == m_iCurrentGroup )
					{
						bAlteredCurrentGroup = GE_TRUE ;
					}

//					DeleteEntity(Ent--);

					DeselectEntity (&(*pEntities)[Ent]);
// changed QD 01/04 bug fix
// didn't see that DeleteEntity was commented out above (btw why?)
					Brush *b = (*pEntities)[Ent].GetActorBrush();
					if(b)
					{
						SelBrushList_Remove(m_CurrentDocVars.m_pSelBrushes, b);
						m_pLevelMgr->RemoveBrush (&thisFunctionDocVars.m_Level, b);
					}
// end change
					pEntities->RemoveAt( Ent );
					m_dwSelState&=(~ENTITYCLEAR);
					m_dwSelState|=(m_iNumSelEntities >1)? MULTIENTITY : (m_iNumSelEntities+1)<<7;
					Ent--;


					SetModifiedFlag();
				}
			}
		}
	}

	if(GetSelState() & ANYBRUSH)
	{
		int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);
		for(int i=0;i < NumSelBrushes;i++)
		{
			Brush				*pBrush = NULL;
			int					BrushModelId;	//POST 0.57
			ModelInfo_Type		*pModelInfo = NULL;
			pModelInfo = m_pLevelMgr->GetModelInfo (&thisFunctionDocVars.m_Level);	//	post 0.57

			pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, 0);
// changed QD Actors
// no chance to delete ActorBrushes!!!
			if(strstr(Brush_GetName(pBrush),".act")!=NULL)
				continue;
// end change
			if (pBrush)
			{

			//	post 0.57
			//	Deleting models if they are associated with brushes that are getting deleted
			BrushModelId = Brush_GetModelId (pBrush);//post 0.57

			if (BrushModelId != 0)	// post 0.57
			{
				CString modelName = "Unable to determine name";
				modelName =	Model_GetName(ModelList_FindById(pModelInfo->Models, BrushModelId));
				CString message = _T("The brush that is about to be deleted is associated with the following Model...\n\n");
				if (modelName)
					message += _T(modelName);
				message += _T("\n\nworlds with UNATTACHED MODELS will fail to load .\n\n");
				message += _T("Choose YES to delete this model along with the brush (Recommended).\n");
				message += _T("Choose NO to delete the brush but NOT the model.\n");
				message += _T("Choose CANCEL to cancel this deletion operation.");
				int result = AfxMessageBox(message, MB_YESNOCANCEL);
				switch (result)
				{
				case IDYES:
					ModelList_Remove (pModelInfo->Models, BrushModelId, m_pLevelMgr->GetBrushes (&thisFunctionDocVars.m_Level));
					break;
				case IDNO:
					break;
				case IDCANCEL:
					return FALSE;
				}
			}
			//	end deleting models

			if( Brush_GetGroupId(pBrush) == m_iCurrentGroup )
			{
				bAlteredCurrentGroup = GE_TRUE ;
			}

			m_pLevelMgr->RemoveBrush (&thisFunctionDocVars.m_Level, pBrush);
			SelBrushList_Remove (m_CurrentDocVars.m_pSelBrushes, pBrush);
			Brush_Destroy (&pBrush);
		}
		}

		//turn off any operation tools
		m_iCurrentTool = CURTOOL_NONE ;

		SetModifiedFlag();
	}

	// Deleting items removed group members so we must update the UI
	if( bAlteredCurrentGroup )
	{
		;;
	}


	DocVarsUpdate(thisFunctionDocVars);
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALLVIEWS /*| REBUILD_QUICK*/, NULL, TRUE);
	m_pMainFrame->UpdateMainControls();	// new g3dc

	UpdateSelected();

	return FALSE;
}

void CGEditProDoc::OnBrushSelectedDelete()
{
	DeleteSelectedBrushes();
}

// changed QD 11/03
void CGEditProDoc::OnFileExport3dtv1_32()
{
	static const char FDTitle[] = "Export";
	CFileDialog dlg(TRUE, "3dt", NULL, OFN_OVERWRITEPROMPT,	"rfEDIT Files v1.32 (*.3DT)|*.3DT||");

	dlg.m_ofn.lpstrTitle = FDTitle;
	if (dlg.DoModal () == IDOK)
	{
		if (dlg.m_ofn.nFilterIndex == 1)
		{
			char Name[MAX_PATH];

			::FilePath_SetExt (dlg.GetPathName (), ".3dt", Name);
			ExportTo3dtv1_32(Name);
		}
	}
}

void CGEditProDoc::OnFileExport3ds()
{
// changed QD 12/03
	Export3dsDialog ExpDlg;

	if (ExpDlg.DoModal () == IDOK)
	{
		static const char FDTitle[] = "Export";
		CFileDialog dlg(TRUE, "3ds", NULL, OFN_OVERWRITEPROMPT,	"Autodesk 3DS (*.3DS)|*.3DS||");

		dlg.m_ofn.lpstrTitle = FDTitle;
		if (dlg.DoModal () == IDOK)
		{
			if (dlg.m_ofn.nFilterIndex == 1)
			{
				char Name[MAX_PATH];

				::FilePath_SetExt (dlg.GetPathName (), ".3ds", Name);
				ExportTo3ds(Name, ExpDlg.m_ExportAll, ExpDlg.m_ExportLights, ExpDlg.m_GroupFile);
			}
		}
	}

}
// end change

void CGEditProDoc::SetupDefaultFilename (void)
{
	const CString DocPath = GetPathName ();

	if (DocPath == "")
	{
		// new file...
		char NewFileName[MAX_PATH];
		CString titl = GetTitle ();
		strcpy (NewFileName, LastPath);
		::FilePath_AppendName (NewFileName, titl, NewFileName);
		::FilePath_SetExt (NewFileName, ".3dt", NewFileName);
		strlwr (NewFileName);
		SetPathName (NewFileName, FALSE);
	}
}


void CGEditProDoc::OnFileSave()
{
	if (m_iIsNewDocument)
	{
		OnFileSaveAs ();
	}
	else
	{
		SetupDefaultFilename ();
		CDocument::OnFileSave ();
	}
}

void CGEditProDoc::OnFileSaveAs()
{
	SetupDefaultFilename ();
	CDocument::OnFileSaveAs ();

		//	post 0.57	make prefab instantly available
	CString message = _T("Failed to reinitialize Prefab list.\n");
			message += _T("You must restart GEditPro to list this new Prefab.");
	CString	strCurrentPathName = this->GetPathName();
	if (strCurrentPathName.Find(_T("\\Objects")))
		if(m_pMainFrame->m_TemplatesTab)
			if (m_pMainFrame->m_TemplatesTab.m_prefabList)
				if (!m_pMainFrame->m_TemplatesTab.InitializePrefabList(this))
					AfxMessageBox(message);


}

BOOL CGEditProDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	::FilePath_GetDriveAndDir (lpszPathName, LastPath);

	if (Save( lpszPathName ) == GE_FALSE)
	{
		AfxMessageBox ("Error saving file.\rThe disk may be full.");
		return FALSE;
	}
	m_iIsNewDocument = 0;
	SetModifiedFlag (FALSE);
	return TRUE;
}

BOOL CGEditProDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	::FilePath_GetDriveAndDir (lpszPathName, LastPath);

	if( Load( lpszPathName ) == GE_FALSE )
		return FALSE ;

	UpdateGridInformation();
	m_iIsNewDocument = 0;

	//	Reset ViewMgrDlg
		CMainFrame *pMainFrame = NULL;
		pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
			pMainFrame->ResetViewMgrDialog();

	//	g3dc tuning comment. Don't recommend doing this here. Really slows down the load.
	//	shows textured view in "filled" mode instead of "wireframe" after do load.
	UpdateAllViews( UAV_ALL3DVIEWS | REBUILD_QUICK, NULL );

	return TRUE;
}


//blasts away the OGFaces of the changed trees (makes em draw solid)
void	CGEditProDoc::InvalidateDrawTreeOriginalFaces(void)
{
	ModelIterator	mi;
	int				i;
	Node			*n;
	ModelInfo_Type	*ModelInfo;
	Model			*pMod;

	Node_InvalidateTreeOGFaces(m_pWorldBsp);

	ModelInfo	=m_pLevelMgr->GetModelInfo(&m_CurrentDocVars.m_Level);
	pMod		=ModelList_GetFirst(ModelInfo->Models, &mi);
	n			=NULL;
	for(i=0;i < ModelList_GetCount(ModelInfo->Models);i++, n=NULL)
	{
		n	=Model_GetModelTree(pMod);
		if(n)
		{
			Node_InvalidateTreeOGFaces(n);
		}
		pMod	=ModelList_GetNext(ModelInfo->Models, &mi);
	}
}


//	g3dc tuning comment
//	This function causes the main performance bottleneck if Options,
//	Auto Rebuild is selected.
//	Main slowdown is caused by:
//		BrushList_DoCSG(BList, CurId, ::fdocBrushCSGCallback, this)
//		BrushList_EnumCSGBrushes (BList, &BspCallbackData, ::AddBrushToBspCB)
//

void	CGEditProDoc::RebuildTrees(void)
{
//	AfxMessageBox("Starting to RebuildTrees Now");

	ModelIterator	mi;
	int				i, CurId = 0;
	Node			*n;
	AddBrushCBData	BspCallbackData;
	ModelInfo_Type	*ModelInfo;
	BrushList		*BList;
	Model			*pMod;

	//	Do the progress control on the status bar
	CProgressCtrl	wndProgress;
	m_pMainFrame->CreateProgressBar(&wndProgress, &m_pMainFrame->m_wndStatusBar,
		_T("Rebuilding Trees..."), 0, 10, 200, 1);
	wndProgress.SetRange(0,100);
	wndProgress.SetStep(10);

// changed QD 11/03 Actors
	UpdateEntityActors();
// end change

	BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);

	if (BList == NULL)	//	post 0.55
	{
		MessageBox(NULL, "BrushList == NULL", "GEditProDoc RebuildTrees", MB_OK);
		return;
	}

	SetModifiedFlag();

	//do the world csg list and tree first
	Node_ClearBsp(m_pWorldBsp);
	m_pWorldBsp	=NULL;

			//	increment the progress control by 10%
	wndProgress.StepIt();	//	10

	BrushList_ClearAllCSG (BList);

	wndProgress.StepIt();	// 20

	BrushList_DoCSG(BList, CurId, ::fdocBrushCSGCallback, this);

	wndProgress.StepIt();	// 30

	BspCallbackData.pDoc		=this;
	BspCallbackData.CurId		=CurId;
	BspCallbackData.pTree		=&m_pWorldBsp;
	BspCallbackData.bAddFlocking=GE_FALSE;

	wndProgress.StepIt();	// 40

	BrushList_EnumCSGBrushes (BList, &BspCallbackData, ::AddBrushToBspCB) ;

	wndProgress.StepIt();	// 50

	BspCallbackData.bAddFlocking=GE_TRUE;
	BrushList_EnumCSGBrushes (BList, &BspCallbackData, ::AddBrushToBspCB) ;

	wndProgress.StepIt();	// 60
	wndProgress.StepIt();	// 70
	wndProgress.StepIt();	// 80


	//build individual model mini trees
	ModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);
	pMod = ModelList_GetFirst (ModelInfo->Models, &mi);
	n		=NULL;
	for(i=0;i < ModelList_GetCount(ModelInfo->Models);i++, n=NULL)
	{
		CurId = Model_GetId (pMod);

		BrushList_DoCSG(BList, CurId, ::fdocBrushCSGCallback, this);

		//change pvoid from this to null to skip the BrushIsVisible check
		BspCallbackData.CurId		=CurId;
		BspCallbackData.pTree		=&n;
		BspCallbackData.bAddFlocking=GE_FALSE;
		BrushList_EnumCSGBrushes (BList, &BspCallbackData, ::AddBrushToBspCB) ;

		BspCallbackData.bAddFlocking=GE_TRUE;
		BrushList_EnumCSGBrushes (BList, &BspCallbackData, ::AddBrushToBspCB) ;

		Node_ClearBsp (Model_GetModelTree(pMod));
		Model_SetModelTree(pMod, n);
		pMod = ModelList_GetNext(ModelInfo->Models, &mi);
	}

	wndProgress.StepIt();	// 90

	if(m_AdjustMode==ADJUST_MODE_FACE)
	{
		ResetAllSelectedFaces();	//	post release 0.5. eliminates crash

		m_pMainFrame->UpdateMainControls();

	}

	wndProgress.StepIt();	// 100

			//	destroy the progress control
	wndProgress.DestroyWindow();
			//	return to normal message reporting on the status bar
	m_pMainFrame->m_wndStatusBar.SetPaneText(0, "For Help, press F1");
}

void CGEditProDoc::OnRebuildBSPTree()
{
	//	post 0.57 gets rid of the "Rebuild?" button on the texture view
	this->m_pMainFrame->SendMessageToDescendants(WM_COMMAND, IDC_BTN_REBUILD_HIDE);
	CGlobals::g_iRebuildClickCount = 0;


	RebuildTrees();	//	disabled for ver 0.57.
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}


//==============================================================
// This function will delete our current thing if we
// have something. e.g. entity or brush
//==============================================================
void CGEditProDoc::DeleteCurrentThing()
{
	BOOL	ReBuild;

	if(m_AdjustMode==ADJUST_MODE_BRUSH
		&& m_iModeTool==ID_GENERALSELECT)
	{
		ResetAllSelectedFaces();
		ReBuild	=(GetSelState() & ANYBRUSH);
		DeleteSelectedBrushes();

//		if(ReBuild && m_pLevelMgr->RebuildBspAlways(&m_CurrentDocVars.m_Level))
		if (!CGlobals::g_bRebuildAlways)
		{
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			//	Be sure REBUID_QUICK is absolutely necessary.
			UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL, TRUE );
		}
		else
		{
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			UpdateAllViews(UAV_ALL3DVIEWS, NULL);
		}
	}
}


//===============================================
// this function will update the information on the status bar
// about the grid
//===============================================
void CGEditProDoc::UpdateGridInformation()
{
	if( m_pMainFrame )
	{
		CMDIChildWnd	*pMDIChild	=(CMDIChildWnd *)m_pMainFrame->MDIGetActive();
		if(pMDIChild)
		{
			CGEditProView	*cv	=(CGEditProView *)pMDIChild->GetActiveView();
			if(cv)
			{
				GridInfo *pGridInfo = m_pLevelMgr->GetGridInfo (&m_CurrentDocVars.m_Level);

				m_pMainFrame->UpdateGridSize(
				    Render_GetFineGrid(cv->VCam, (pGridInfo->GridType == GridMetric) ? GRID_TYPE_METRIC : GRID_TYPE_TEXEL),
					pGridInfo->UseGrid,
					((pGridInfo->SnapType == GridMetric) ? pGridInfo->MetricSnapSize : pGridInfo->TexelSnapSize),
					pGridInfo->GridType,
					pGridInfo->SnapType);
			}
		}
	}
}/* CGEditProDoc::UpdateGridInformation */

void CGEditProDoc::RenderWorld(ViewVars *v, CDC *pDC)
{
#define PEN_WHITE_COLOR RGB(255,255,255)
#define PEN_CYAN_COLOR  RGB(0,255,0)
	//#define PEN_BLUE_COLOR  RGB(0,0,255)
#define PEN_BLUE_COLOR TEMPLATEBLUE
#define PEN_PURPLE_COLOR RGB(255,0,255)
#define PEN_YELLOW_COLOR RGB(255,255,0)
#define PEN_HINT_COLOR  RGB(0,100,0)
#define PEN_CLIP_COLOR  RGB(128,0,128)

	int				i;
	Node			*pNode = NULL;	//n;
	ModelIterator	mi;
	Model			*pModel = NULL;
	geBoolean		DoBlit	=GE_FALSE;
	BrushDrawData	brushDrawData;
	BrushList		*BList = NULL;


	// Temporary Hack for not updating
	//#pragma message ("This temporary hack needs to be fixed.  Perhaps a 'force redraw' flag?")
	/*
	if (m_pLevelMgr->RebuildBspAlways (&m_CurrentDocVars.m_Level) == GE_FALSE)
	{
	return ;
	}
	*/
	if (&m_CurrentDocVars.m_Level == NULL)
	{
		// must not be loaded yet...
		return ;
	}

	BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);

	if (BList == NULL)	//	post 0.55
	{
		MessageBox(NULL, "BrushList == NULL", "GEditProDoc RenderWorld", MB_OK);
		return;
	}

	Render_UpdateViewPos(v);
	Render_SetUpFrustum(v);

	brushDrawData.pViewBox = NULL ;
	brushDrawData.pDC = pDC ;
	brushDrawData.v = v ;
	brushDrawData.pDoc = this;
	brushDrawData.GroupId = fdoc_SHOW_ALL_GROUPS;
	brushDrawData.FlagTest = NULL;
	brushDrawData.Color = PEN_WHITE_COLOR;

	if((Render_GetViewType(v) & (VIEWSOLID|VIEWTEXTURE)) && m_pWorldBsp)
	{
		Render_RenderTree(v, m_pWorldBsp, pDC->m_hDC, ZFILL);
		DoBlit	=TRUE;
	}
	else
	{
		Render_ClearViewDib (v);
		brushDrawData.FlagTest = IsBrushNotClipOrHint;
		BrushList_EnumLeafBrushes(BList, &brushDrawData, ::BrushDrawWire3dCB);
	}
	if(m_bShowClipBrushes)
	{
		brushDrawData.Color	=PEN_CLIP_COLOR;
		brushDrawData.FlagTest = Brush_IsClip;
		BrushList_EnumLeafBrushes(BList, &brushDrawData, ::BrushDrawWire3dZBufferCB);
	}
	if(m_bShowHintBrushes)
	{
		brushDrawData.Color	=PEN_HINT_COLOR;
		brushDrawData.FlagTest = Brush_IsHint;
		BrushList_EnumLeafBrushes(BList, &brushDrawData, ::BrushDrawWire3dZBufferCB);
	}

	CEntityArray *Entities = NULL;
	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	if (Entities)
	{
		for(i=0;i < Entities->GetSize();i++)
		{
			CEntity *pEnt = NULL;

			pEnt = &(*Entities)[i];
			if (pEnt)
			{
				if( !Render_PointInFrustum(v, &pEnt->mOrigin ) )
					continue ;

				if( EntityIsVisible( pEnt ) == GE_FALSE )
					continue;

				if (pEnt->IsSelected ())
					continue;

				::DrawEntity (pEnt, v, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));
			}
		}

		if(!(GetSelState() & NOENTITIES))
		{
			for(i=0;i < Entities->GetSize();i++)
			{
				CEntity *pEnt = NULL;
				pEnt = &(*Entities)[i];
				if (pEnt)
				{
					if(!(pEnt->IsSelected () && EntityIsVisible( pEnt )) )
						continue;

					if(!Render_PointInFrustum(v, &pEnt->mOrigin))
						continue;

					const geBitmap *pBitmap = NULL;

					pBitmap = pEnt->GetBitmapPtr (m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));
					if (pBitmap)
					{
						Render_3DTextureZBufferOutline(v, &pEnt->mOrigin, pBitmap, RGB(0,255,255));
					}
				}
			}
		}
	}
	if(DoBlit)
	{
		ModelInfo_Type *ModelInfo = NULL;
		ModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);

		if (ModelInfo)
		{
			//render the models
			pModel = ModelList_GetFirst (ModelInfo->Models, &mi);
			while (pModel != NULL)
			{
				pNode = Model_GetModelTree (pModel);
				if (pNode != NULL)
				{
					Render_RenderTree(v, pNode, pDC->m_hDC, ZBUFFER);
					DoBlit	=TRUE;
				}
				pModel = ModelList_GetNext (ModelInfo->Models, &mi);
			}
		}
	}


	brushDrawData.FlagTest = NULL;
	brushDrawData.Color = PEN_CYAN_COLOR;

	int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);
	for(i=0;i < NumSelBrushes;i++)
	{
		Brush *pBrush = NULL;

		pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);
// changed QD Actors
		if(!m_bShowActors)
			if(strstr(Brush_GetName(pBrush),".act")!=NULL)
				continue;
// end change
		if (pBrush)
		{
			if(Brush_IsMulti(pBrush))
			{
				BrushList_EnumLeafBrushes(Brush_GetBrushList(pBrush), &brushDrawData, ::BrushDrawWire3dCB);
			}
			else
			{
				Render_RenderBrushFaces(v, pBrush, brushDrawData.Color);
			}
		}
	}

	if(!(GetSelState() & NOFACES))
	{
		brushDrawData.Color = PEN_PURPLE_COLOR;
		BrushList_EnumLeafBrushes(BList, &brushDrawData, ::BrushDrawWireSel3dCB);
	}

	if((m_iModeTool==ID_TOOLS_TEMPLATE)||
		(m_iModeTool==ID_TOOLS_CAMERA && GetSelState()==NOSELECTIONS))
	{
		brushDrawData.Color = PEN_BLUE_COLOR;
		if(!m_bTempEnt)
		{
			if(Brush_IsMulti(m_pCurBrush))
			{
				BrushList_EnumLeafBrushes(Brush_GetBrushList(m_pCurBrush), &brushDrawData, ::BrushDrawWire3dCB);
			}
			else
			{
				Render_RenderBrushFaces(v, m_pCurBrush, brushDrawData.Color);
			}
		}
		else
		{
			if(Render_PointInFrustum(v, &m_RegularEntity.mOrigin))
			{
				::DrawEntity (&m_RegularEntity, v, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));
			}
		}
	}

	//	if (DoBlit)
	{
		Render_BlitViewDib(v, pDC->m_hDC);
	}
}

void CGEditProDoc::MoveSelectedBrushList(SelBrushList *pList, geVec3d const *v)
{
	if (pList)
	{
		int NumBrushes;
		int		i;
		CEntityArray *pEntities = NULL;

		m_iLastOp = BRUSH_MOVE;

		geVec3d_Add (&m_vecSelectedGeoCenter, v, &m_vecSelectedGeoCenter);
		geVec3d_Add(v, &m_vecFinalPos, &m_vecFinalPos);

		NumBrushes = SelBrushList_GetSize (pList);
		for(i=0;i < NumBrushes;i++)
		{
			Brush *pBrush = NULL;

			pBrush = SelBrushList_GetBrush (pList, i);
			if (pBrush)
			{
// changed QD Actors
				if(strstr(Brush_GetName(pBrush),".act")!=NULL)
				continue;
// end change
				Brush_Move (pBrush, v);
			}
		}


		pEntities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

		if (pEntities)
		{
			for(i=0;i < pEntities->GetSize();i++)
			{
				if ((*pEntities)[i].IsSelected ())
				{
					(*pEntities)[i].Move (v);
				}
			}
		}
	}
}

void CGEditProDoc::MoveSelectedBrushes(geVec3d const *v)
{
	MoveSelectedBrushList (m_CurrentDocVars.m_pTempSelBrushes, v);
}

void CGEditProDoc::MoveSelectedClone(geVec3d const *v)
{
	MoveSelectedBrushList (m_CurrentDocVars.m_pSelBrushes, v);
}

void CGEditProDoc::MoveTemplateBrush(geVec3d *v)
{
	m_iLastOp	=BRUSH_MOVE;

	geVec3d_Add (&m_vecSelectedGeoCenter, v, &m_vecSelectedGeoCenter);
	geVec3d_Add (v, &m_vecFinalPos, &m_vecFinalPos);

	if(m_bTempEnt)
	{
		m_RegularEntity.Move (v);
	}
	else
	{
		geVec3d *pTemplatePos;

		Brush_Move (m_pCurBrush, v);
		pTemplatePos = m_pLevelMgr->GetTemplatePos (&m_CurrentDocVars.m_Level);
		Brush_Center (m_pCurBrush, pTemplatePos);
	}
}

void CGEditProDoc::RotateTemplateBrush(geVec3d *v)
{
	geXForm3d	rm;

	m_iLastOp	=BRUSH_ROTATE;

	geXForm3d_SetEulerAngles(&rm, v);
	Brush_Rotate (m_pCurBrush, &rm, &m_vecSelectedGeoCenter);
}

void CGEditProDoc::GetRotationPoint
	(
	  geVec3d *pVec
	)
{
	Model *pModel;
	ModelInfo_Type *ModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);

	pModel = ModelList_GetAnimatingModel (ModelInfo->Models);
	if (pModel != NULL)
	{
		CModelsDlg	*ModelTab;		//	new g3dc
		geVec3d Xlate;

		// we're animating a model, so use its current position
		Model_GetCurrentPos (pModel, pVec);
		// We have to add the current move translation
		ModelTab = &m_pMainFrame->m_ModelsTab;	//	new g3dc
		if (ModelTab != NULL)
		{
			ModelTab->GetTranslation (&Xlate);
		}
		else
		{
			geVec3d_Clear (&Xlate);
		}
		geVec3d_Add (pVec, &Xlate, pVec);
	}
	else
	{
		*pVec = m_vecSelectedGeoCenter;
	}
}

void CGEditProDoc::RotateSelectedBrushList(SelBrushList *pList, geVec3d const *v)
{
	int i;
	geXForm3d rm;
	geVec3d RotationPoint;
	int NumBrushes = SelBrushList_GetSize (pList);

	m_iLastOp = BRUSH_ROTATE;

	// if we're animating a model, we want to rotate about the
	// model's current rotation origin--taking into account any
	// current translations.
	GetRotationPoint (&RotationPoint);

	geVec3d_Add(v, &m_vecFinalRot, &m_vecFinalRot);
	geXForm3d_SetEulerAngles(&rm, v);

	for(i=0;i < NumBrushes;i++)
	{
		Brush *pBrush = SelBrushList_GetBrush (pList, i);
// changed QD Actors
// don't rotate ActorBrushes
		if(strstr(Brush_GetName(pBrush),".act")!=NULL)
			continue;
// end change
		Brush_Rotate (pBrush, &rm, &RotationPoint);
	}

	//	rotate selected entities along with brushes
	CEntityArray *pEntities = NULL;
	pEntities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
	if (pEntities)
	{
		for( int iEnt = 0; iEnt < pEntities->GetSize(); iEnt++ )
		{
			CEntity *pTempEntity = NULL;
			pTempEntity = &pEntities->GetAt(iEnt);
			if (pTempEntity)
			{
				if (pTempEntity->IsSelected())
				{
					pTempEntity->Rotate(&rm, &RotationPoint);
				}
			}
		}
	}


}/* CGEditProDoc::RotateSelectedBrushList */

void CGEditProDoc::RotateSelectedBrushes(geVec3d const *v)
{
	RotateSelectedBrushList (m_CurrentDocVars.m_pTempSelBrushes, v);
}

void CGEditProDoc::RotateSelectedBrushesDirect(geVec3d const *v)
{
	int i;
	geXForm3d rm;
	geVec3d RotationPoint;

	int NumBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);

	CGEditProDocVars	thisFunctionDocVars;

	thisFunctionDocVars = DocVarsCreateUniqueCopy(m_CurrentDocVars);
	TempDeleteSelected(&m_CurrentDocVars);

	m_iLastOp = BRUSH_ROTATE;

	// if we're animating a model, we want to rotate about the
	// model's current rotation origin--taking into account any
	// current translations.
	GetRotationPoint (&RotationPoint);


	geVec3d_Add(v, &m_vecFinalRot, &m_vecFinalRot);
	geXForm3d_SetEulerAngles(&rm, v);

	for(i=0;i < NumBrushes;i++)
	{
		Brush *pBrush = NULL;
		pBrush = SelBrushList_GetBrush (thisFunctionDocVars.m_pSelBrushes, i);
// changed QD Actors
// don't rotate ActorBrushes
		if(strstr(Brush_GetName(pBrush),".act")!=NULL)
			continue;
// end change
		if (pBrush)
		{

			Brush_Rotate (pBrush, &rm, &RotationPoint);
		}
	}

//	post 0.5 release	//	rotate selected entities along with brushes
	CEntityArray *Entities = m_pLevelMgr->GetEntities (&thisFunctionDocVars.m_Level);

	for( int Ent = 0; Ent < Entities->GetSize(); Ent++ )
	{
		if ((*Entities)[ Ent ].IsSelected ())
		{
			(*Entities)[Ent].Rotate(&rm, &RotationPoint);
		}
	}

	DocVarsUpdate(thisFunctionDocVars);

	UpdateSelectedModel (BRUSH_ROTATE, v);
	geVec3d_Clear (&m_vecFinalRot);
}


// Sets texture scale on all faces of all selected brushes.
void CGEditProDoc::SetAllFacesTextureScale(geFloat ScaleVal)
{
	if (SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes) > 0)
	{
		SelBrushList_Enum (m_CurrentDocVars.m_pSelBrushes, fdocBrushTextureScaleCallback, &ScaleVal);
		if (CGlobals::g_bRebuildAlways)
		{
			RebuildTrees();
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			UpdateAllViews (UAV_ALL3DVIEWS, NULL);
		}
	}
}


void CGEditProDoc::UpdateSelectedModel
	(
	  int MoveRotate,
	  geVec3d const *pVecDelta
	)
{
	ModelIterator mi;
	Model *pModel;
	ModelInfo_Type *ModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);
	BrushList *BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);

	// notify model dialog so that it can update animation deltas if required
	m_pMainFrame->UpdateSelectedModel (MoveRotate, pVecDelta);

	// For each model that is fully selected, update its reference position.
	pModel = ModelList_GetFirst (ModelInfo->Models, &mi);
	while (pModel != NULL)
	{
		// don't mess with models that are animating
		if (Model_IsAnimating (pModel) != GE_TRUE)
		{
			if (Model_IsSelected (pModel, m_CurrentDocVars.m_pSelBrushes, BList))
			{
				Model_UpdateOrigin (pModel, MoveRotate, pVecDelta);
			}
		}
		pModel = ModelList_GetNext (ModelInfo->Models, &mi);
	}
}



void	CGEditProDoc::SetCameraDocumentCoords(geVec3d *pVecOrigin, geVec3d *pVecAngles)
{
	m_vecCameraOrigin.X = pVecOrigin->X;
	m_vecCameraOrigin.Y = pVecOrigin->Y;
	m_vecCameraOrigin.Z = pVecOrigin->Z;

	m_vecCameraAngles.X = pVecAngles->X;
	m_vecCameraAngles.Y = pVecAngles->Y;
	m_vecCameraAngles.Z = pVecAngles->Z;
}



void	CGEditProDoc::GetCameraDocumentCoords(geVec3d *pVecOrigin, geVec3d *pVecAngles)
{
	pVecOrigin->X = m_vecCameraOrigin.X;
	pVecOrigin->Y = m_vecCameraOrigin.Y;
	pVecOrigin->Z = m_vecCameraOrigin.Z;

	pVecAngles->X = m_vecCameraAngles.X;
	pVecAngles->Y = m_vecCameraAngles.Y;
	pVecAngles->Z = m_vecCameraAngles.Z;
}


void CGEditProDoc::DoneRotate(void)
{
	int			i;
	geFloat	RSnap;
	geXForm3d		rm;
	geVec3d RotationPoint;
	m_iLastOp = BRUSH_ROTATE;
	CGEditProDocVars	thisFunctionDocVars;
	CEntityArray	*pTrueEntities = NULL;

	//	if the camera ALONE is being rotated, let's do it without cycling
	//	the DocVars

	CEntity  *pTrueEnt = NULL;
	if ((GetSelState()==ONEENTITYONLY))
	{
		pTrueEntities = m_CurrentDocVars.m_Level.m_pEntities;
		if (pTrueEntities)
		{
			pTrueEnt = this->FindCameraEntity(&m_CurrentDocVars);

				if (pTrueEnt)
				{
					if (pTrueEnt->IsSelected())
					{
						return;

					}
				}
		}
	}	//	if ((GetSelState()==ONEENTITYONLY))...



	TempDeleteSelected(&m_CurrentDocVars);
	thisFunctionDocVars = DocVarsCreateUniqueCopy(m_CurrentDocVars);

	GetRotationPoint (&RotationPoint);

	if((m_dwSelState & NOENTITIES) && m_pLevelMgr->UseGrid (&thisFunctionDocVars.m_Level))
	{
		RSnap		=Units_DegreesToRadians ((float)m_pLevelMgr->GetRotationSnap (&thisFunctionDocVars.m_Level));
		m_vecFinalRot.X	=((float)((int)(m_vecFinalRot.X / RSnap))) * RSnap;
		m_vecFinalRot.Y	=((float)((int)(m_vecFinalRot.Y / RSnap))) * RSnap;
		m_vecFinalRot.Z	=((float)((int)(m_vecFinalRot.Z / RSnap))) * RSnap;
	}

	geXForm3d_SetEulerAngles(&rm, &m_vecFinalRot);

	int NumSelBrushes = SelBrushList_GetSize (thisFunctionDocVars.m_pSelBrushes);

	for(i=0;i < NumSelBrushes;i++)
	{
		Brush *pBrush = NULL;

		pBrush = SelBrushList_GetBrush (thisFunctionDocVars.m_pSelBrushes, i);
		if (pBrush)
		{
// changed QD Actors
// don't rotate ActorBrushes
			if(strstr(Brush_GetName(pBrush),".act")!=NULL)
				continue;
// end change
			Brush_Rotate (pBrush, &rm, &RotationPoint);
		}
	}


	pTrueEntities = m_pLevelMgr->GetEntities(&thisFunctionDocVars.m_Level);
	if (pTrueEntities)
	{
		int iNumEntities = pTrueEntities->GetSize();

		for (int iZeppo = 0; iZeppo < iNumEntities; iZeppo++)
		{
			CEntity  *pTrueEnt = NULL;
			pTrueEnt = &pTrueEntities->GetAt(iZeppo);
			if (pTrueEnt)
			{
				geVec3d Angles;
				geVec3d_Clear(&Angles);
				pTrueEnt->SetOrigin(pTrueEnt->mOrigin.X, pTrueEnt->mOrigin.Y, pTrueEnt->mOrigin.Z, thisFunctionDocVars.m_Level.m_pEntityDefs);
				pTrueEnt->GetAngles(&Angles, m_pLevelMgr->GetEntityDefs (&thisFunctionDocVars.m_Level));
				pTrueEnt->SetAngles(&Angles,  m_pLevelMgr->GetEntityDefs (&thisFunctionDocVars.m_Level));

				if (pTrueEnt->IsSelected())
				{
					pTrueEnt->DoneRotate(&rm, &m_vecFinalRot, thisFunctionDocVars.m_Level.m_pEntityDefs);

					if (pTrueEnt->IsCamera())
					{
						SetCameraDocumentCoords(&pTrueEnt->mOrigin, &Angles);

						SetRenderedViewCamera( &(pTrueEnt->mOrigin), &Angles) ;
						//	Be very careful when speccing flags for UpdateAllViews()
						//	The wrong flags at the wrong time will totally screw things up
						UpdateAllViews(UAV_RENDER_ONLY, NULL);
					}

				}
			}
		}
	}

	DocVarsUpdate(thisFunctionDocVars);
	UpdateSelectedModel (BRUSH_ROTATE, &m_vecFinalRot);

	UpdateSelected();	//	calls UpdateMainControls
	geVec3d_Clear (&m_vecFinalRot);
}




void CGEditProDoc::ShearSelected(float dx, float dy, int sides, int inidx)
{
	m_iLastOp				=BRUSH_SHEAR;

	if(m_iModeTool == ID_TOOLS_TEMPLATE)
	{
		Brush_Destroy(&m_pCurBrush);
		m_pCurBrush	=m_pBTemplate	=Brush_Clone(m_pTempShearTemplate);
		Brush_ShearFixed(m_pCurBrush, dx, dy, sides, inidx, &m_vecFinalScale, &m_iScaleNum);
	}
	else
	{
		int i;
		int NumSelBrushes;

		TempDeleteSelected(&m_CurrentDocVars);
		TempCopySelectedBrushes();
		NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pTempSelBrushes);
		for (i = 0; i < NumSelBrushes; ++i)
		{
			Brush *pBrush;

			pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pTempSelBrushes, i);
			Brush_ShearFixed(pBrush, dx, dy, sides, inidx, &m_vecFinalScale, &m_iScaleNum);
		}
	}
}

void CGEditProDoc::DoneShear(int sides, int inidx)
{
	CGEditProDocVars	thisFunctionDocVars;

	const Box3d	*bx1 = NULL, *bx2 = NULL;
	int			snapside	=0;
	geFloat		bsnap;
		m_iLastOp	=BRUSH_SHEAR;

	thisFunctionDocVars = DocVarsCreateUniqueCopy(m_CurrentDocVars);
	TempDeleteSelected(&m_CurrentDocVars);

	bsnap = 1.0f;
	if(m_pLevelMgr->UseGrid (&thisFunctionDocVars.m_Level))
	{
		bsnap = m_pLevelMgr->GetGridSnapSize (&thisFunctionDocVars.m_Level);
	}

	if(m_iModeTool==ID_TOOLS_TEMPLATE)
	{
		if(m_pTempShearTemplate)	//can get here without shearing
		{						//by rapid clicking
			Brush_Destroy(&m_pCurBrush);
			m_pCurBrush	=m_pBTemplate	=Brush_Clone(m_pTempShearTemplate);
			Brush_ShearFinal(m_pCurBrush, sides, inidx, &m_vecFinalScale);

			//check which side of the bounds changed
			bx1	=Brush_GetBoundingBox(m_pCurBrush);
			bx2	=Brush_GetBoundingBox(m_pTempShearTemplate);

			if(bx1->Max.X != bx2->Max.X)	snapside	|=2;
			if(bx1->Max.Y != bx2->Max.Y)	snapside	|=8;
			if(bx1->Max.Z != bx2->Max.Z)	snapside	|=32;
			if(bx1->Min.X != bx2->Min.X)	snapside	|=1;
			if(bx1->Min.Y != bx2->Min.Y)	snapside	|=4;
			if(bx1->Min.Z != bx2->Min.Z)	snapside	|=16;
			Brush_SnapShearNearest(m_pCurBrush, bsnap, sides, inidx, snapside);
			Brush_Destroy(&m_pTempShearTemplate);
		}
		return;
	}

	int NumSelBrushes = SelBrushList_GetSize (thisFunctionDocVars.m_pSelBrushes);

	int i;

	for (i = 0; i < NumSelBrushes; ++i)
	{
		Brush *pBrush = NULL;
		Brush *tBrush = NULL;

		pBrush = SelBrushList_GetBrush (thisFunctionDocVars.m_pSelBrushes, i);
		tBrush = SelBrushList_GetBrush (thisFunctionDocVars.m_pTempSelBrushes, i);

		if (pBrush && tBrush)
		{
// changed QD Actors
// don't shear ActorBrushes
		if(strstr(Brush_GetName(pBrush),".act")!=NULL)
			continue;
// end change
		Brush_ShearFinal(pBrush, sides, inidx, &m_vecFinalScale);

		//check which side of the bounds changed
		bx1	=Brush_GetBoundingBox(pBrush);
		bx2	=Brush_GetBoundingBox(tBrush);

		if(bx1->Max.X != bx2->Max.X)	snapside	|=2;
		if(bx1->Max.Y != bx2->Max.Y)	snapside	|=8;
		if(bx1->Max.Z != bx2->Max.Z)	snapside	|=32;
		if(bx1->Min.X != bx2->Min.X)	snapside	|=1;
		if(bx1->Min.Y != bx2->Min.Y)	snapside	|=4;
		if(bx1->Min.Z != bx2->Min.Z)	snapside	|=16;

		Brush_SnapShearNearest(pBrush, bsnap, sides, inidx, snapside);
		}
	}

	CEntityArray	*pTrueEntities = NULL;

	pTrueEntities = m_pLevelMgr->GetEntities(&thisFunctionDocVars.m_Level);
	if (pTrueEntities)
	{
		int iNumEntities = pTrueEntities->GetSize();

		for (int iZeppo = 0; iZeppo < iNumEntities; iZeppo++)
		{
			CEntity  *pTrueEnt = NULL;
			pTrueEnt = &pTrueEntities->GetAt(iZeppo);
			if (pTrueEnt)
			{
				pTrueEnt->SetOrigin(pTrueEnt->mOrigin.X, pTrueEnt->mOrigin.Y, pTrueEnt->mOrigin.Z, thisFunctionDocVars.m_Level.m_pEntityDefs);
			}
		}
	}

	DocVarsUpdate(thisFunctionDocVars);
	UpdateSelected();	//	calls UpdateMainControls)()
}




void CGEditProDoc::ResizeSelected(float dx, float dy, int sides, int inidx)
{
	m_iLastOp				=BRUSH_SCALE;

	if(m_iModeTool == ID_TOOLS_TEMPLATE)
	{
		Brush_Resize(m_pCurBrush, dx, dy, sides, inidx, &m_vecFinalScale, &m_iScaleNum);

		if(Brush_IsMulti(m_pCurBrush))
		{
			BrushList_ClearCSGAndHollows((BrushList *)Brush_GetBrushList(m_pCurBrush), Brush_GetModelId(m_pCurBrush));
			BrushList_RebuildHollowFaces((BrushList *)Brush_GetBrushList(m_pCurBrush), Brush_GetModelId(m_pCurBrush), ::fdocBrushCSGCallback, this);
		}
	}
	else
	{
		int i;
		int NumBrushes;
		Box3d	multiBoundingBox;
		Box3d	oldBoundingBox;
		geVec3d	vecFake;
		multiBoundingBox.Max.X = multiBoundingBox.Max.Y = multiBoundingBox.Max.Z = -10000.0f;
		multiBoundingBox.Min.X = multiBoundingBox.Min.Y = multiBoundingBox.Min.Z = 10000.0f;

		vecFake.X = vecFake.Y = vecFake.Z = 1.0f;

		NumBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pTempSelBrushes);

		if (NumBrushes > 0)
		{
		for (i = 0; i < NumBrushes; ++i)
			{
				Brush *pBrush = NULL;

				pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pTempSelBrushes, i);

				if (pBrush)
				{
					//	set default bounding box to be taht of the first selected
					//	brush ... to get us in range
					if (i == 0)
					{
						int iOffset = 0;
						multiBoundingBox.Min.X = pBrush->BoundingBox.Min.X - iOffset;
						multiBoundingBox.Min.Y = pBrush->BoundingBox.Min.Y - iOffset;
						multiBoundingBox.Min.Z = pBrush->BoundingBox.Min.Z - iOffset;
						multiBoundingBox.Max.X = pBrush->BoundingBox.Max.X + iOffset;
						multiBoundingBox.Max.Y = pBrush->BoundingBox.Max.Y + iOffset;
						multiBoundingBox.Max.Z = pBrush->BoundingBox.Max.Z + iOffset;
					}

					if (pBrush->BoundingBox.Min.X < multiBoundingBox.Min.X)
						multiBoundingBox.Min.X = pBrush->BoundingBox.Min.X;
					if (pBrush->BoundingBox.Min.Y < multiBoundingBox.Min.Y)
						multiBoundingBox.Min.Y = pBrush->BoundingBox.Min.Y;
					if (pBrush->BoundingBox.Min.Z < multiBoundingBox.Min.Z)
						multiBoundingBox.Min.Z = pBrush->BoundingBox.Min.Z;

					if (pBrush->BoundingBox.Max.X > multiBoundingBox.Max.X)
						multiBoundingBox.Max.X = pBrush->BoundingBox.Max.X;
					if (pBrush->BoundingBox.Max.Y > multiBoundingBox.Max.Y)
						multiBoundingBox.Max.Y = pBrush->BoundingBox.Max.Y;
					if (pBrush->BoundingBox.Max.Z > multiBoundingBox.Max.Z)
						multiBoundingBox.Max.Z = pBrush->BoundingBox.Max.Z;
				}
			}
		}

		for (i = 0; i < NumBrushes; ++i)
		{
			Brush *pBrush = NULL;

			pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pTempSelBrushes, i);

			if (pBrush)
			{
				pBrush->BoundingBox.Min.X = multiBoundingBox.Min.X;
				pBrush->BoundingBox.Min.Y = multiBoundingBox.Min.Y;
				pBrush->BoundingBox.Min.Z = multiBoundingBox.Min.Z;

				pBrush->BoundingBox.Max.X = multiBoundingBox.Max.X;
				pBrush->BoundingBox.Max.Y = multiBoundingBox.Max.Y;
				pBrush->BoundingBox.Max.Z = multiBoundingBox.Max.Z;

				//	give the final resize vector only ONCE. Otherwise, if multiple brushes
				//	are being resized, the scale will mulitply for each brush
				if (i == 0)
					Brush_Resize (pBrush, dx, dy, sides, inidx, &m_vecFinalScale, &m_iScaleNum);
				else	//	give it a fake vector
					Brush_Resize (pBrush, dx, dy, sides, inidx, &vecFake, &m_iScaleNum);

				if (Brush_IsMulti(pBrush))
				{
					BrushList_ClearCSGAndHollows((BrushList *)Brush_GetBrushList(pBrush), Brush_GetModelId(pBrush));
					BrushList_RebuildHollowFaces((BrushList *)Brush_GetBrushList(pBrush), Brush_GetModelId(pBrush), ::fdocBrushCSGCallback, this);
				}
			}
		}
	}
}


void CGEditProDoc::DoneResize(int sides, int inidx)
{
	m_iLastOp	=BRUSH_SCALE;

	CGEditProDocVars	thisFunctionDocVars;

	TempDeleteSelected(&m_CurrentDocVars);
	thisFunctionDocVars = DocVarsCreateUniqueCopy(m_CurrentDocVars);

	if (m_iModeTool==ID_TOOLS_TEMPLATE)
	{
		if(Brush_IsMulti(m_pCurBrush))
		{
			BrushList_ClearCSGAndHollows((BrushList *)Brush_GetBrushList(m_pCurBrush), Brush_GetModelId(m_pCurBrush));
			BrushList_RebuildHollowFaces((BrushList *)Brush_GetBrushList(m_pCurBrush), Brush_GetModelId(m_pCurBrush), ::fdocBrushCSGCallback, this);
		}
		return;
	}
	Box3d	multiBoundingBox;

	int NumSelBrushes = SelBrushList_GetSize (thisFunctionDocVars.m_pSelBrushes);

	if (NumSelBrushes > 0)
	{

		for (int i = 0; i < NumSelBrushes; ++i)
		{
			Brush *pBrush = NULL;

			pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);
// changed QD Actors
// don't resize ActorBrushes
			if(strstr(Brush_GetName(pBrush),".act")!=NULL)
				continue;
// end change
			if (pBrush)
			{
				//	set default bounding box to be taht of the first selected
				//	brush ... to get us in range
				if (i == 0)
				{
					multiBoundingBox.Min.X = pBrush->BoundingBox.Min.X;
					multiBoundingBox.Min.Y = pBrush->BoundingBox.Min.Y;
					multiBoundingBox.Min.Z = pBrush->BoundingBox.Min.Z;
					multiBoundingBox.Max.X = pBrush->BoundingBox.Max.X;
					multiBoundingBox.Max.Y = pBrush->BoundingBox.Max.Y;
					multiBoundingBox.Max.Z = pBrush->BoundingBox.Max.Z;
				}

				if (pBrush->BoundingBox.Min.X < multiBoundingBox.Min.X)
					multiBoundingBox.Min.X = pBrush->BoundingBox.Min.X;
				if (pBrush->BoundingBox.Min.Y < multiBoundingBox.Min.Y)
					multiBoundingBox.Min.Y = pBrush->BoundingBox.Min.Y;
				if (pBrush->BoundingBox.Min.Z < multiBoundingBox.Min.Z)
					multiBoundingBox.Min.Z = pBrush->BoundingBox.Min.Z;

				if (pBrush->BoundingBox.Max.X > multiBoundingBox.Max.X)
					multiBoundingBox.Max.X = pBrush->BoundingBox.Max.X;
				if (pBrush->BoundingBox.Max.Y > multiBoundingBox.Max.Y)
					multiBoundingBox.Max.Y = pBrush->BoundingBox.Max.Y;
				if (pBrush->BoundingBox.Max.Z > multiBoundingBox.Max.Z)
					multiBoundingBox.Max.Z = pBrush->BoundingBox.Max.Z;

			}
		}
	}

	for (int i = 0; i < NumSelBrushes; ++i)
	{
		Brush *pBrush = NULL;

		pBrush = SelBrushList_GetBrush (thisFunctionDocVars.m_pSelBrushes, i);
// changed QD Actors
// don't resize ActorBrushes
		if(strstr(Brush_GetName(pBrush),".act")!=NULL)
			continue;
// end change
		if (pBrush)
		{
			pBrush->BoundingBox.Min.X = multiBoundingBox.Min.X;
			pBrush->BoundingBox.Min.Y = multiBoundingBox.Min.Y;
			pBrush->BoundingBox.Min.Z = multiBoundingBox.Min.Z;

			pBrush->BoundingBox.Max.X = multiBoundingBox.Max.X;
			pBrush->BoundingBox.Max.Y = multiBoundingBox.Max.Y;
			pBrush->BoundingBox.Max.Z = multiBoundingBox.Max.Z;

			Brush_ResizeFinal(pBrush, sides, inidx, &m_vecFinalScale);
			if(Brush_IsMulti(pBrush))
			{
				BrushList_ClearCSGAndHollows((BrushList *)Brush_GetBrushList(pBrush), Brush_GetModelId(pBrush));
				BrushList_RebuildHollowFaces((BrushList *)Brush_GetBrushList(pBrush), Brush_GetModelId(pBrush), ::fdocBrushCSGCallback, this);
			}
		}
	}

	CEntityArray	*pTrueEntities = NULL;

	pTrueEntities = m_pLevelMgr->GetEntities(&thisFunctionDocVars.m_Level);
	if (pTrueEntities)
	{
		int iNumEntities = pTrueEntities->GetSize();

		for (int iZeppo = 0; iZeppo < iNumEntities; iZeppo++)
		{
			CEntity  *pTrueEnt = NULL;
			pTrueEnt = &pTrueEntities->GetAt(iZeppo);
			if (pTrueEnt)
			{
				pTrueEnt->SetOrigin(pTrueEnt->mOrigin.X, pTrueEnt->mOrigin.Y, pTrueEnt->mOrigin.Z, thisFunctionDocVars.m_Level.m_pEntityDefs);
			}
		}
	}

	DocVarsUpdate(thisFunctionDocVars);
	UpdateSelected();	//	calls UpdateMainControls)()
}



void CGEditProDoc::DoneMove (void)
{
	CGEditProDocVars	thisFunctionDocVars;
	int	i;
	m_iLastOp = BRUSH_MOVE;

	//	if the camera ALONE is being moved, let's do it without cycling
	//	the DocVars
	CEntityArray	*pTrueEntities = NULL;
	CEntity  *pTrueEnt = NULL;
	if ((GetSelState()==ONEENTITYONLY))
	{
		pTrueEntities = m_CurrentDocVars.m_Level.m_pEntities;
		if (pTrueEntities)
		{
			int iNumberOfEntities = pTrueEntities->GetSize();
			for(i=0;i < iNumberOfEntities;i++)
			{
				pTrueEnt = NULL;
				pTrueEnt = &(*pTrueEntities)[i];

				if (pTrueEnt)
				{
					if (pTrueEnt->IsSelected())
					{
						if( pTrueEnt->IsCamera() == GE_TRUE )	// Camera Entity?
						{
							DoneMoveEntity (&m_CurrentDocVars);
							return;
						}
					}
				}
			}
		}
	}	//	if ((GetSelState()==ONEENTITYONLY))...

	if(m_iModeTool == ID_TOOLS_TEMPLATE)
	{
		if (m_bTempEnt)
		{
			DoneMoveEntity (&m_CurrentDocVars);	//	calls UpdateAllViews()
		}
		else
		{
			Brush_Move (m_pCurBrush, &m_vecFinalPos);
		}
		return;
	}
	else
	{
		TempDeleteSelected(&m_CurrentDocVars);
		thisFunctionDocVars = DocVarsCreateUniqueCopy(m_CurrentDocVars);

		int NumSelBrushes = SelBrushList_GetSize (thisFunctionDocVars.m_pSelBrushes);
		for(i=0;i < NumSelBrushes;i++)
		{
			Brush *pBrush = NULL;

			pBrush = SelBrushList_GetBrush (thisFunctionDocVars.m_pSelBrushes, i);
			if (pBrush)
			{
// changed QD Actors
// ActorBrushes will be moved via their entities
				if(strstr(Brush_GetName(pBrush),".act")!=NULL)
					continue;
// end change
				Brush_Move (pBrush, &m_vecFinalPos);
			}
		}

		if (GetSelState() & ANYENTITY)
		{
			DoneMoveEntity(&thisFunctionDocVars);	//	calls UpdateAllViews()
		}
	}

	DocVarsUpdate(thisFunctionDocVars);
	UpdateSelectedModel (BRUSH_MOVE, &m_vecFinalPos);

	UpdateSelected();	//	calls UpdateMainControls()
	geVec3d_Clear (&m_vecFinalPos);
}


void CGEditProDoc::DoneMoveEntity(CGEditProDocVars	*pDocVars)
{
	int		i;
	float	SnapSize = 0.0f;
	CEntityArray	*pTrueEntities = NULL;
	CEntity *pSelEnt = NULL, *pTrueEnt = NULL;

	pTrueEntities = pDocVars->m_Level.m_pEntities;
	if (/*pSelEntities && */pTrueEntities)
	{
		int iNumberOfEntities = pTrueEntities->GetSize();

		if (m_iCurrentTool==ID_TOOLS_BRUSH_MOVEROTATEBRUSH)
		{
			if ((GetSelState()==ONEENTITYONLY) && m_pLevelMgr->UseGrid (&pDocVars->m_Level))
			{
				SnapSize = m_pLevelMgr->GetGridSnapSize (&pDocVars->m_Level);
			}
			else
			{
				SnapSize = 1.0f;
			}

			for(i=0;i < iNumberOfEntities;i++)
			{
				pSelEnt = NULL, pTrueEnt = NULL;
				pTrueEnt = &(*pTrueEntities)[i];

				if (/*pSelEnt && */pTrueEnt)
				{
					pTrueEnt->SetOrigin(pTrueEnt->mOrigin.X, pTrueEnt->mOrigin.Y, pTrueEnt->mOrigin.Z, pDocVars->m_Level.m_pEntityDefs);
					if (pTrueEnt->IsSelected ())
					{
						pTrueEnt->DoneMove (SnapSize, pDocVars->m_Level.m_pEntityDefs);
						if( pTrueEnt->IsCamera() == GE_TRUE )	// Camera Entity?
						{
							geVec3d	PitchRollYaw ;

							pTrueEnt->GetAngles( &PitchRollYaw, m_pLevelMgr->GetEntityDefs (&pDocVars->m_Level) ) ;
							SetRenderedViewCamera( &(pTrueEnt->mOrigin), &PitchRollYaw ) ;
							SetCameraDocumentCoords(&pTrueEnt->mOrigin, &PitchRollYaw);

							//	Be very careful when speccing flags for UpdateAllViews()
							//	The wrong flags at the wrong time will totally screw things up
							UpdateAllViews(UAV_RENDER_ONLY, NULL);
						}// Camera entity, update camera

					}// Entity Selected

				}// Loop thru Entities
			}
		}	//	if (m_iCurrentTool==ID_TOOLS_BRUSH_MOVEROTATEBRUSH)...
		else
		{
			if (m_iCurrentEntity < 0) //template OR multiple entities selected
			{
				pTrueEnt = NULL;
				pTrueEnt = &m_RegularEntity;
			}
			else
			{
				pTrueEnt = NULL;
				pTrueEnt = &(*pTrueEntities)[m_iCurrentEntity];
			}

			SnapSize = 1.0f;
			if( m_pLevelMgr->UseGrid (&pDocVars->m_Level))
			{
				SnapSize = m_pLevelMgr->GetGridSnapSize (&pDocVars->m_Level);
			}

			//	this makes sure that while the mouse is moving MULTIPLE cloned entities,
			//	the cloned entities STICK  at their new location.
			for(i=0;i < iNumberOfEntities;i++)
			{
				pSelEnt = NULL, pTrueEnt = NULL;
				pTrueEnt = &(*pTrueEntities)[i];

				if (/*pSelEnt && */pTrueEnt)
				{
					if (pTrueEnt->IsSelected ())
					{
						pTrueEnt->DoneMove (SnapSize, m_pLevelMgr->GetEntityDefs (&pDocVars->m_Level));
						if( pTrueEnt->IsCamera() == GE_TRUE )	// Camera Entity?
						{
							geVec3d	PitchRollYaw ;

							pTrueEnt->GetAngles( &PitchRollYaw, m_pLevelMgr->GetEntityDefs (&pDocVars->m_Level)) ;
							SetRenderedViewCamera( &(pTrueEnt->mOrigin), &PitchRollYaw ) ;
							//	Be very careful when speccing flags for UpdateAllViews()
							//	The wrong flags at the wrong time will totally screw things up
							UpdateAllViews(UAV_RENDER_ONLY, NULL);
						}// Camera entity, update camera
					}// Entity Selected
				}
			}// Loop thru Entities
		}
		SetModifiedFlag();
	}

}/* CGEditProDoc::DoneMoveEntity */





void CGEditProDoc::UpdateEntityOrigins()
{
	int	i;
	CEntityArray *pEntities = NULL;

	pEntities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	if (pEntities)
	{
		int iNumberOfEntities =  pEntities->GetSize();
		for(i=0;i < iNumberOfEntities;i++)
		{
			(*pEntities)[i].UpdateOrigin(m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));
		}
	}

	//	updates the Rendered view to reflect the current position of the camera
	CEntity *	pEnt = NULL ;

	pEnt = EntityList_FindByClassName( m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level), "Camera" ) ;
	if( pEnt )
	{
		geVec3d	vecOrigin, vecAngles;
		geVec3d_Clear(&vecOrigin);
		geVec3d_Clear(&vecAngles);

		//	Reposition and Reorient the camera to the DOCUMENT standard
		//	so the camera doesn't change with each Undo/Redo
		GetCameraDocumentCoords(&vecOrigin, &vecAngles);
		pEnt->SetAngles(&vecAngles, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));
		pEnt->SetOrigin(vecOrigin.X, vecOrigin.Y, vecOrigin.Z, m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));

		SetRenderedViewCamera( &(pEnt->mOrigin), &vecAngles ) ;
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		UpdateAllViews(UAV_RENDER_ONLY, NULL);
	}
}


void CGEditProDoc::MoveEntity(geVec3d *v)
{
	assert (v != NULL);

	if(m_iCurrentEntity < 0) //template
	{
		m_RegularEntity.Move (v);
	}
	else
	{
		CEntityArray *pEntities = NULL;
		//		pEntities = m_CurrentDocVars.m_pSelEntities;
		pEntities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

		if (pEntities)
		{

			CEntity	*pEntity = NULL;
			pEntity = &(*pEntities)[m_iCurrentEntity];

			if (pEntity)
			{
				if (pEntity->IsSelected())
				{
					pEntity->Move(v);
					SetModifiedFlag();
				}

				if (pEntity->IsCamera())
				{
					geVec3d	vecOrigin, vecAngles;
					geVec3d_Clear(&vecOrigin);
					geVec3d_Clear(&vecAngles);
					vecOrigin = pEntity->mOrigin;
					pEntity->GetAngles(&vecAngles, this->m_pLevelMgr->GetEntityDefs(&m_CurrentDocVars.m_Level));

					SetCameraDocumentCoords(&vecOrigin, &vecAngles);
				}
			}
		}
	}
}



//	Be very careful when speccing flags for UpdateAllViews()
//	The wrong flags at the wrong time will totally screw things up
void CGEditProDoc::UpdateAllViews(int Mode, CView* pSender, BOOL Override)
{
	if(IsModified() && ((Mode & REBUILD_QUICK ) && (CGlobals::g_bRebuildAlways))||(Override))
	{
		RebuildTrees(); // only place in this function that calls for RebuildTrees()
	}
	else if((Mode & REBUILD_QUICK) && (!CGlobals::g_bRebuildAlways))
	{
		InvalidateDrawTreeOriginalFaces();
	}

	if(Mode & REBUILD_QUICK)
		Mode &= ~REBUILD_QUICK;

	//	Do we want to redraw everything?
	if (Mode & UAV_ALLVIEWS)
	{
		CDocument::UpdateAllViews(pSender);
		return;
	}

	POSITION pos = GetFirstViewPosition();

	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);

		if ( pView->IsKindOf( RUNTIME_CLASS (CGEditProView)) )
		{
			CGEditProView* ptEditView = (CGEditProView*) pView;
			CDC* pDC = ptEditView->GetDC();

			switch(Mode)
			{
			case UAV_ACTIVE3DVIEW_ONLY:

				if( ptEditView->GetParentFrame() == m_pActiveViewFrame )
					ptEditView->Invalidate( TRUE );
				break;

			case UAV_NONACTIVE3DVIEWS_ONLY:

				if( ptEditView->GetParentFrame() != m_pActiveViewFrame )
					ptEditView->Invalidate( TRUE );
				break;

			case UAV_TEXTUREVIEW_ONLY:

				if (ptEditView->mViewType == ID_VIEW_TEXTUREVIEW)
					ptEditView->Invalidate(TRUE);
				break;

			case UAV_RENDER_ONLY:

				switch(ptEditView->mViewType)
				{
				case ID_VIEW_3DWIREFRAME:
				case ID_VIEW_TEXTUREVIEW:

					ptEditView->Invalidate(TRUE);
					break;

				default:
					break;
				}
				break;

			case UAV_GRID_ONLY:

				switch(ptEditView->mViewType)
				{
				case ID_VIEW_TOPVIEW:
				case ID_VIEW_SIDEVIEW:
				case ID_VIEW_FRONTVIEW:

					ptEditView->Invalidate(TRUE);
					break;
				}
				break;


			case UAV_THIS_GRID_ONLY:
				if( ptEditView == pSender )
				{
					switch(ptEditView->mViewType)
					{
					case ID_VIEW_TOPVIEW:
					case ID_VIEW_SIDEVIEW:
					case ID_VIEW_FRONTVIEW:
						ptEditView->Invalidate(TRUE);
						break;
					}
				}
				break;

			case UAV_ALL3DVIEWS:

				ptEditView->Invalidate(TRUE);
				break;

			default:
				break;
			}

			ptEditView->ReleaseDC(pDC);
		}
	}
}

// obsolete old gedit
void CGEditProDoc::FaceAttributesDialog ()
{
	//	Has a face been selected?  Are we selecting faces...
	if ((m_AdjustMode == ADJUST_MODE_FACE) && (SelFaceList_GetSize (m_CurrentDocVars.m_pSelFaces) > 0))
	{
		if (m_pFaceAttributes == NULL)
		{
			;;
		}
	}
}

// obsolete old gedit
void CGEditProDoc::NullFaceAttributes()
{
	m_pFaceAttributes = NULL;
}

//	This function basically sets up everything required
//  for each tool before its use...
void CGEditProDoc::ConfigureCurrentTool(void)
{
	BOOL	Redraw	=FALSE;

	if(m_iModeTool==ID_TOOLS_CAMERA)
	{
		m_iCurrentTool		=CURTOOL_NONE;
		m_iShowSelectedFaces	=(m_AdjustMode==ADJUST_MODE_FACE);
		m_iShowSelectedBrushes=(m_AdjustMode==ADJUST_MODE_BRUSH);
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		UpdateAllViews(UAV_ALL3DVIEWS, NULL);
		return;
	}
	switch(m_AdjustMode)
	{
		case ADJUST_MODE_BRUSH :
			m_iShowSelectedFaces	=FALSE;
			m_iShowSelectedBrushes=TRUE;
			UpdateSelected();
			Redraw	=TRUE;
			break;

		case ADJUST_MODE_FACE :
			m_iShowSelectedFaces	=TRUE;
			m_iShowSelectedBrushes=FALSE;

			UpdateSelected();	//	calls UpdateMainControls()
			Redraw	=TRUE;
			break;

		default :
			assert (0);	// bad adjustment mode
			break;
	}



	switch(m_iCurrentTool)
	{
	case ID_TOOLS_BRUSH_MOVEROTATEBRUSH:
		if(m_iModeTool!=ID_TOOLS_TEMPLATE)
		{
			m_iShowSelectedFaces	=FALSE;
			m_iShowSelectedBrushes=TRUE;
			Redraw				=TRUE;
		}
		else
		{
			m_iShowSelectedFaces	=FALSE;
			m_iShowSelectedBrushes=FALSE;
		}
		break;


	case ID_TOOLS_BRUSH_ROTATEBRUSH:
		if(m_iModeTool!=ID_TOOLS_TEMPLATE)
		{
			m_iShowSelectedFaces	=FALSE;
			m_iShowSelectedBrushes=TRUE;
			Redraw				=TRUE;
		}
		else
		{
			m_iShowSelectedFaces	=FALSE;
			m_iShowSelectedBrushes=FALSE;
		}
		break;


	case ID_TOOLS_BRUSH_SCALEBRUSH:
		if(m_iModeTool!=ID_TOOLS_TEMPLATE)
		{
			Redraw				=TRUE;
			m_iShowSelectedFaces	=FALSE;
			m_iShowSelectedBrushes=TRUE;
		}
		else
		{
			m_iShowSelectedFaces	=FALSE;
			m_iShowSelectedBrushes=FALSE;
		}
		break;

	}


	if(m_iModeTool==ID_TOOLS_TEMPLATE && m_bTempEnt)
		m_iCurrentEntity=-1;

	Redraw	=TRUE;
	if(Redraw)
	{
		//	Be very careful when speccing flags for UpdateAllViews()
		//	The wrong flags at the wrong time will totally screw things up
		UpdateAllViews(UAV_ALL3DVIEWS, NULL);		// old gedit
	}
}

void CGEditProDoc::UpdateSelected()
{
	int		i;

// changed QD 11/03 Actors
// allow entity rotation, while ActorBrush is Selected
	CEntityArray *pEntities = NULL;
	pEntities = m_CurrentDocVars.m_Level.m_pEntities;

	for(i=0;i < pEntities->GetSize();i++)
	{
		Brush *pBrush = (*pEntities)[i].GetActorBrush();

		if(pBrush)
			SelBrushList_Remove(m_CurrentDocVars.m_pSelBrushes, pBrush);
	}

// end change
	int NumSelFaces = SelFaceList_GetSize (m_CurrentDocVars.m_pSelFaces);

	int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);

	m_dwSelState=(NumSelBrushes > 1)? MULTIBRUSH : NumSelBrushes;
	m_dwSelState|=(NumSelFaces >1)? MULTIFACE : (NumSelFaces+1)<<3;
	m_dwSelState|=(m_iNumSelEntities >1)? MULTIENTITY : (m_iNumSelEntities+1)<<7;

	if(m_iModeTool==ID_GENERALSELECT)
	{
		if(GetSelState() & ONEBRUSH)
		{
			m_pCurBrush	=SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, 0);
		}

		else
		{
			m_pCurBrush	= m_pBTemplate;
		}
	}

	geVec3d_Clear (&m_vecSelectedGeoCenter);

	if (m_iModeTool==ID_TOOLS_TEMPLATE)
	{
		Brush_Center (m_pCurBrush, &m_vecSelectedGeoCenter);
	}
	else if((m_dwSelState & MULTIBRUSH) || (m_dwSelState & ONEBRUSH))
	{
		Model *pModel = NULL;
		ModelInfo_Type *ModelInfo = NULL;
		ModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);

		if (ModelInfo)
		{
			pModel = ModelList_GetAnimatingModel (ModelInfo->Models);
			if (pModel)
			{
				// we're animating a model, so use its current position
				Model_GetCurrentPos (pModel, &m_vecSelectedGeoCenter);
			}
			else
			{
				Box3d ViewBox;
				Brush *pBrush = NULL;

				pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, 0);
				if (pBrush)
				{
					ViewBox = *Brush_GetBoundingBox (pBrush);
					for(i = 1; i < NumSelBrushes; i++)
					{
						pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);
						Box3d_Union (Brush_GetBoundingBox(pBrush), &ViewBox, &ViewBox);
					}
					Box3d_GetCenter (&ViewBox, &m_vecSelectedGeoCenter);
				}
			}
		}
	}
	if(m_dwSelState & ONEENTITY)
	{
// changed QD Actors
//		CEntityArray *pEntities = NULL;
//		pEntities = m_CurrentDocVars.m_Level.m_pEntities;

		if (pEntities)
		{
			for(i=0;i < pEntities->GetSize() && !((*pEntities)[i].IsSelected ());i++);
			m_iCurrentEntity	=i;
		}
	}
	else
		m_iCurrentEntity	=-1;

// changed QD 11/03 Actors
	for(i=0;i < pEntities->GetSize();i++)
	{
		Brush *pBrush = (*pEntities)[i].GetActorBrush();

		if(pBrush)
		{
			if((*pEntities)[i].IsSelected())
					SelBrushList_Add(m_CurrentDocVars.m_pSelBrushes, pBrush);
		}
	}

// end change

	m_pMainFrame->UpdateMainControls(); // DoGeneralSelect depends on this here


}/* CGEditProDoc::UpdateSelected */

void CGEditProDoc::NullBrushAttributes(void)
{
	m_pBrushAttributes=NULL;
}



//	called by OnToolsAddToLevel()
void CGEditProDoc::AddBrushToWorld(void)
{
	if(m_bTempEnt || !Brush_IsSubtract (m_pCurBrush))
		OnBrushAddtoworld();
	else
		OnBrushSubtractfromworld();
}

void CGEditProDoc::ResetAllSelectedBrushes(void)
{
	SelBrushList_RemoveAll (m_CurrentDocVars.m_pSelBrushes);
	m_pCurBrush		=m_pBTemplate;
}

void CGEditProDoc::ResetAllSelectedFaces(void)
{
	BrushList_EnumLeafBrushes(m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level), NULL, ::ResetSelectedFacesCB) ;
	SelFaceList_RemoveAll (m_CurrentDocVars.m_pSelFaces);
}

Face *CGEditProDoc::FindSelectedFace (void)
{
	Face *pSelectedFace;

	pSelectedFace = NULL;
	BrushList_EnumLeafBrushes (m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level), &pSelectedFace, ::FindSelectedFaceCB);
	return pSelectedFace;
}

void CGEditProDoc::OnConstrainhollows()
{
	m_bConstrainHollows = ! m_bConstrainHollows ;
}

void CGEditProDoc::OnUpdateConstrainhollows(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_bConstrainHollows );
}


/////////////////////////////////////////////////////////////////////////////////
//	new g3dc -- added by tom
//	added so I can switch modes programmatically

// Called by: OnBrushAddto World(). and CMainFrame::SetCommandPanelTab()
// Calls: ConfigureCurrentTool(), which calls UpdateAllViews(), which calls RebuidTrees()l
// And it calls UpdateMainControls()
////////////////////////////////////////////////////////////////////////////////
void CGEditProDoc::SetMode(int mMode)
{
	m_pMainFrame->HideAllPropDialogsBut((CWnd*)this);

	if (mMode == MAINFRM_COMMANDPANEL_TEMPLATES)
	{
		m_pTempBrush = m_pCurBrush;	//	for restoring later
		m_iModeTool = ID_TOOLS_TEMPLATE;
		m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, this, ::fdocDeselectEntity);
		ResetAllSelectedBrushes();
		ResetAllSelectedFaces();
		m_iCurrentTool = ID_TOOLS_BRUSH_MOVEROTATEBRUSH;
		m_pCurBrush = m_pTempBrush;			//	restore now

		//	post 0.55
		//	ignore if we are placing Prefabs
		if (strcmp(m_currentTemplateName, "PreFab"))
		{
			if (!strcmp(m_currentTemplateName, "Arch"))
				CreateArch();
			if (!strcmp(m_currentTemplateName, "Cube"))
				CreateCube();
			if (!strcmp(m_currentTemplateName, "Cone"))
				CreateCone();
			if (!strcmp(m_currentTemplateName, "Cylinder"))
				CreateCylinder();
			if (!strcmp(m_currentTemplateName, "Sphere"))
				CreateSpheroid();
			if (!strcmp(m_currentTemplateName, "Stairs"))
				CreateStaircase();
		}
	}
	else
	{
		m_iModeTool = ID_GENERALSELECT;
		m_iCurrentTool = CURTOOL_NONE;
	}

	ConfigureCurrentTool();	//	calls UpdateAllViews()

	m_pMainFrame->UpdateMainControls();
}


void CGEditProDoc::OnGeneralselect()
{
	DoGeneralSelect ();
}


void CGEditProDoc::DoGeneralSelect (void)
{
			//	if we're already in ID_GENERALSELECT mode...
	if (m_iModeTool ==ID_GENERALSELECT)	//	save some effort
		return;
					//	clear the properties panel. new g3dc
	m_pMainFrame->HideAllPropDialogsBut((CWnd*)this);

	m_iCurrentTool	=CURTOOL_NONE;
	m_iModeTool		=ID_GENERALSELECT;
	ConfigureCurrentTool();	//	calls UpdateAllViews()
							//	which callsmpMainFrame->UpdateMainControls();
}



void CGEditProDoc::OnUpdateGeneralselect(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck ((m_iModeTool == ID_GENERALSELECT) ? 1 : 0);
}

void CGEditProDoc::SetAdjustmentMode( fdocAdjustEnum nCmdIDMode )
{
	if( m_AdjustMode == nCmdIDMode )
		return ;

	if (nCmdIDMode == ADJUST_MODE_TOGGLE)
	{
		nCmdIDMode = (m_AdjustMode == ADJUST_MODE_BRUSH) ? ADJUST_MODE_FACE : ADJUST_MODE_BRUSH;
	}

	switch (nCmdIDMode)
	{
		case ADJUST_MODE_BRUSH :
			m_AdjustMode = nCmdIDMode;

			// go through brush list and select any brush that has selected faces.
			// Ensure that all brushes in a locked group or model set are selected...
			m_pLevelMgr->EnumLeafBrushes (&m_CurrentDocVars.m_Level, this, ::fdocSelectBrushesFromFaces);

			ResetAllSelectedFaces();
			UpdateSelected();	// calls m_pMainFrame->UpdateMainControls()

			//remove face attributes dialog if present...
			ConfigureCurrentTool();		//	calls UpdateAllViews()
			break;

		case ADJUST_MODE_FACE :
		{
			m_AdjustMode = nCmdIDMode;

			// Select all faces on all selected brushes
			int iBrush;
			int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);

			for (iBrush = 0; iBrush < NumSelBrushes; ++iBrush)
			{
				Brush *pBrush = NULL;

				pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, iBrush);
				if (pBrush)
				{
				if (Brush_IsMulti (pBrush))
				{
					BrushList_EnumLeafBrushes (Brush_GetBrushList (pBrush), this, ::SelAllBrushFaces);
				}
				else
				{
					::SelAllBrushFaces (pBrush, this);
				}
				}
			}
			UpdateSelected ();	// calls m_pMainFrame->UpdateMainControls()
			ConfigureCurrentTool();	//	calls UpdateAllViews()
			break;
		}
		default :
			assert (0);		// bad mode (can't happen?)
			break;
	}
}



void CGEditProDoc::OnCloseDocument()
{
										//	Clear the slate.
										//	still needed here even though
										//	CMainFrame::UpdateMainControls()
										//	does this as well
	int			NO_TABS_DUDE		= -1;
	m_pMainFrame->HideAllPropDialogsBut((CWnd*)m_pMainFrame);
	m_pMainFrame->HideAllTabsBut(NO_TABS_DUDE);
						//	get the latest address for the tab control
	m_pMainFrame->m_CommandTabControl = (CTabCtrl*)m_pMainFrame->m_CommandDialogBar.GetDlgItem( IDC_COMMAND_TAB ) ;
							// Default to TemplatesTab
	m_pMainFrame->m_CommandTabControl->SetCurSel( MAINFRM_COMMANDPANEL_TEMPLATES ) ;
	m_pMainFrame->m_eCurrentTab = MAINFRM_COMMANDPANEL_TEMPLATES ;


	CDocument::OnCloseDocument();
}



void CGEditProDoc::OnThingAttributes()
{
	switch( m_AdjustMode )
	{
		case ADJUST_MODE_BRUSH :
			OnToolsBrushAttributes ();
			break;

		case ADJUST_MODE_FACE :
			OnToolsFaceAttributes ();
			break;

		default :
			assert (0);  // bad...
			break;
	}
}

void CGEditProDoc::OnEditDelete()
{
	DeleteCurrentThing ();
}


int CGEditProDoc::DoCompileDialog
	(
	  void
	)
{
	CompileParamsType *CompileParams;

	CompileParams = m_pLevelMgr->GetCompileParams (&m_CurrentDocVars.m_Level);
	// Build output file if none there...
	if (CompileParams->Filename[0] == '\0')
	{
		FilePath_SetExt (GetPathName (), ".map", CompileParams->Filename);
	}

	CCompileDialog CompileDlg (AfxGetMainWnd (), CompileParams);

	return CompileDlg.DoModal ();
}

// Write level data to map file for compiling
geBoolean CGEditProDoc::WriteLevelToMap(const char *Filename)
{
	static const int Version = 1;
	static const int ftag = 0x4642434E;
	int NumEntities, i;
	FILE *exfile = NULL;
	ModelInfo_Type *ModelInfo = NULL;
	BrushList *BList = NULL;
	fdocEntityCountData	ecnt;
	CompileParamsType *CompileParams = NULL;
	CompileParams = m_pLevelMgr->GetCompileParams (&m_CurrentDocVars.m_Level);

// changed QD Actors
// don't want to compile ActorBrushes
	CEntityArray *Entities;
	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	for(i=0;i < Entities->GetSize();i++)
	{
		Brush *pBrush = (*Entities)[i].GetActorBrush();

		if(pBrush!=NULL)
			m_pLevelMgr->RemoveBrush(&m_CurrentDocVars.m_Level, pBrush);
	}
// end change

	BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);

	if (BList)
	{
		//	assert (BList != NULL);

		exfile = fopen (Filename, "wb");
		if (exfile == NULL)
		{
			return GE_FALSE;
		}
		if (exfile)
		{
			ModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);

			if (ModelInfo)
			{
				// write header information
				TypeIO_WriteInt (exfile, Version);
				TypeIO_WriteInt (exfile, ftag);
				/*
				Number of entities to be written is the sum of:
				World Entity		one
				Regular entities	one for each regular entity that isn't a camera
				Models				one for each model
				Type Information	one entity for each type
				----------------
				*/
				ecnt.Count	= 1;		// world entity
				ecnt.pDoc	= this;
				m_pLevelMgr->EnumEntities(&m_CurrentDocVars.m_Level, &ecnt, ::fdocCountNonCameraEntities);

				NumEntities = ecnt.Count + ModelList_GetCount (ModelInfo->Models) +	// models
					EntityTable_GetTypeCount (m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));			// type information

				TypeIO_WriteInt (exfile, NumEntities);

				//write the world entity first
				{
					BrushList *BList = NULL;

					// count brushes and write count to file
					fdocBrushEnumData EnumData;

					EnumData.SuppressHidden = CompileParams->SuppressHidden;
					EnumData.VisDetail = CompileParams->VisDetailBrushes;
					EnumData.pDoc = this;
					EnumData.BrushCount = 0;
					EnumData.f = exfile;

					BList = m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level);

					if (BList)
					{
						BrushList_EnumCSGBrushes (BList, &EnumData, ::fdocBrushCountCallback);

						TypeIO_WriteInt (exfile, EnumData.BrushCount);

						// and write the model 0 brushes
						BrushList_EnumCSGBrushes (BList, &EnumData, ::fdocBrushWriteCallback);
					}	//	if (BList...
				}

				{
					int nSkyFaces;
					int iFace;
					SkyFaceTexture *pSkyFaces = NULL;
					geVec3d SkyRotationAxis;
					geFloat SkyRotationSpeed, SkyTextureScale;

					pSkyFaces = m_pLevelMgr->GetSkyInfo (&m_CurrentDocVars.m_Level, &SkyRotationAxis, &SkyRotationSpeed, &SkyTextureScale);

					if (pSkyFaces)
					{
						// determine how many sky faces to write...
						nSkyFaces = 0;
						for (iFace = 0; iFace < 6; ++iFace)
						{
							SkyFaceTexture *pFace = NULL;

							pFace = &pSkyFaces[iFace];
							if (pFace)
							{
								if (pFace->Apply && (pFace->TextureName != NULL) && (*pFace->TextureName != '\0'))
								{
									++nSkyFaces;
								}
								pFace = NULL;

							}	//	if (pFace...
						}

						TypeIO_WriteInt (exfile, 0);  // no motion data for this model
						TypeIO_WriteInt (exfile, 4+nSkyFaces);  // numfields = #sky faces + TextureLibrary + SkyAxis + SkyRotation + SkyScaling
						TypeIO_WriteString (exfile, "TextureLib");
						TypeIO_WriteString (exfile, m_pLevelMgr->GetWadPath (&m_CurrentDocVars.m_Level));

						// write sky information
						{
							char s[100];

							sprintf (s, "%f %f %f", SkyRotationAxis.X, SkyRotationAxis.Y, SkyRotationAxis.Z);
							TypeIO_WriteString (exfile, "SkyAxis");
							TypeIO_WriteString (exfile, s);

							sprintf (s, "%f", SkyRotationSpeed);
							TypeIO_WriteString (exfile, "SkyRotation");
							TypeIO_WriteString (exfile, s);

							TypeIO_WriteString (exfile, "SkyDrawScale");
							sprintf (s, "%f", SkyTextureScale);
							TypeIO_WriteString (exfile, s);
						}

						for (iFace = 0; iFace < 6; ++iFace)
						{
							SkyFaceTexture *pFace = NULL;

							pFace = &pSkyFaces[iFace];
							if (pFace)
							{
							if (pFace->Apply && (pFace->TextureName != NULL) && (*pFace->TextureName != '\0'))
							{
								TypeIO_WriteString (exfile, SkyFaceNames[iFace]);
								TypeIO_WriteString (exfile, pFace->TextureName);
							}
							pFace = NULL;
							}
						}
					}	//	if (pSkyFaces...
				}

				// write the models
				ModelList_WriteToMap (ModelInfo->Models, exfile, BList, CompileParams->SuppressHidden, CompileParams->VisDetailBrushes);

				// and the entities
				{
					fdocEntityToMapData etmData;

					etmData.f = exfile;
					etmData.Models = ModelInfo->Models;
					etmData.Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
					etmData.pDoc	=this;

					m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, &etmData, ::fdocEntityToMap);
				}

				EntityTable_WriteTypesToMap (m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level), exfile);
				fclose(exfile);
			}	//	if (ModelInfo...
		}	//	if (exfile...
	}	//	if (BList...
// changed QD Actors
	for(i=0;i < Entities->GetSize();i++)
	{
		Brush *pBrush = (*Entities)[i].GetActorBrush();

		if(pBrush!=NULL)
			m_pLevelMgr->AppendBrush(&m_CurrentDocVars.m_Level, pBrush);
	}
// end change
	return GE_TRUE;
}


void	CGEditProDoc::ShowConsole()
{
	CMainFrame	*pMainFrame = CGlobals::GetActiveDocument()->m_pMainFrame;
	pMainFrame->ShowConsoleBar();
}



void CGEditProDoc::OnCompile()
{
	geBoolean		NoErrors;
	CompilerErrorEnum CompileRslt;
	CompileParamsType *pCompileParams = NULL;
	pCompileParams = m_pLevelMgr->GetCompileParams (&m_CurrentDocVars.m_Level);



	if (Compiler_CompileInProgress ())
	{
		if (AfxMessageBox ("Abort current compile?", MB_YESNO) == IDYES)
		{
			Compiler_CancelCompile ();
		}
		return;
	}

	CMainFrame	*pMainFrame = CGlobals::GetActiveDocument()->m_pMainFrame;	// new g3dc
	pMainFrame->ConClear();		//	new g3dc
	ShowConsole();

	BOOL KeepGoing;
	do
	{
		KeepGoing = FALSE;
		// display the dialog.  Exit if user cancels
		if (DoCompileDialog () != IDOK)
		{
			return;
		}

		NoErrors = GE_TRUE;

		// set wait cursor
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

		//ensure lists and trees are updated
		RebuildTrees();

		NoErrors = WriteLevelToMap (pCompileParams->Filename);
		if (!NoErrors)
		{
			CString MsgString;

			MsgString.LoadString (IDS_MAPERROR);
			int rslt = AfxMessageBox (MsgString, MB_OKCANCEL);
			KeepGoing = (rslt == IDOK);
		}
	} while (KeepGoing);

	/*
	  The idea here is that I want to spawn a thread to do the compile.
	  When the compile is finished, we'll prompt the user and ask if he wants to
	  run preview.
	*/
	if (NoErrors)
	{
		CMDIChildWnd	*pMDIChild	=(CMDIChildWnd *)m_pMainFrame->MDIGetActive();
		CGEditProView	*cv	=(CGEditProView *)pMDIChild->GetActiveView();

		CompileRslt = Compiler_StartThreadedCompile (pCompileParams, cv->m_hWnd);
		NoErrors = (CompileRslt == COMPILER_ERROR_NONE);
	}

	// put cursor back
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	if (!NoErrors)
	{
		AfxMessageBox ("Unable to start compile");
	}
}

void CGEditProDoc::CompileDone (CompilerErrorEnum CompileRslt)
{
	static const char SuccessMessage[] = "Level compile completed successfully.";
	static const char ErrorMessage[] = "Error compiling level.";
	static const char PreviewMessage[] = "Run preview?";
	static const char CancelMessage[] = "Level compile cancelled by user";
	CompileParamsType *CompileParams = m_pLevelMgr->GetCompileParams (&m_CurrentDocVars.m_Level);

	switch (CompileRslt)
	{
		case COMPILER_ERROR_NONE :
		{
			if (CompileParams->RunPreview)
			{
				char BigMessage[sizeof (SuccessMessage) + sizeof (PreviewMessage)];
				sprintf (BigMessage, "%s\r%s", SuccessMessage, PreviewMessage);
				if (AfxMessageBox (BigMessage, MB_YESNO) == IDYES)
				{
					char BspFilename[_MAX_PATH];
					char MotionFilename[_MAX_PATH];

					FilePath_SetExt (CompileParams->Filename, ".bsp", BspFilename);
					FilePath_SetExt (CompileParams->Filename, ".mot", MotionFilename);

					Compiler_RunPreview (BspFilename, MotionFilename, Prefs_GetPreviewPath (GetPrefs ()));
				}
			}
			else
			{
				AfxMessageBox (SuccessMessage);
			}
			break;
		}
		case COMPILER_ERROR_USERCANCEL :
			AfxMessageBox (CancelMessage);
			break;
		default :
			AfxMessageBox (ErrorMessage);
			break;
	}
}

void CGEditProDoc::ScaleWorld(geFloat ScaleFactor)
{
// changed QD Actors 12/03
	if (ScaleFactor <= 0.0f) return;
// don't want to scale ActorBrushes
	int i;
	CEntityArray *Entities;
	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	for(i=0;i < Entities->GetSize();i++)
	{
		Brush *pBrush = (*Entities)[i].GetActorBrush();

		if(pBrush!=NULL)
			m_pLevelMgr->RemoveBrush(&m_CurrentDocVars.m_Level, pBrush);
	}
// end change

	// scale the brushes
	BrushList_Scale (m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level), ScaleFactor);

// changed QD Actors
	for(i=0;i < Entities->GetSize();i++)
	{
		Brush *pBrush = (*Entities)[i].GetActorBrush();

		if(pBrush!=NULL)
			m_pLevelMgr->AppendBrush(&m_CurrentDocVars.m_Level, pBrush);
	}
// end change
	// scale the entity positions
	ScaleEntityInfo ScaleInfo;

	ScaleInfo.pEntityDefs = m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level);
	ScaleInfo.ScaleValue = ScaleFactor;
	m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, &ScaleInfo, ::fdocScaleEntityCallback);


	// and scale the models
	ModelList_ScaleAll (m_pLevelMgr->GetModelInfo(&m_CurrentDocVars.m_Level)->Models, ScaleFactor);

	m_fWorldScaleFactor *= ScaleFactor;	//	post 0.57 fore keeping track of
										//	scaling history

	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews( UAV_ALL3DVIEWS, NULL );
}


float	CGEditProDoc::GetWorldScaleFactor()
{
	return m_fWorldScaleFactor;
}


void CGEditProDoc::SelectModelBrushes
	(
	  BOOL Select,
	  int ModelId
	)
{
	fdocBrushSelectData bsData;

	bsData.Select = Select;
	bsData.ModelId = ModelId;
	bsData.pDoc = this;

	// Go through the brush list and add all brushes that have
	// this model's id to the selection list.
	m_pLevelMgr->EnumBrushes (&m_CurrentDocVars.m_Level, &bsData, ::fdocSelectBrushCallback);

	UpdateSelected ();	//	calls UpdateMainControls)()
}

/******************************************************
     BRUSH GROUP SUPPORT
*******************************************************/
BOOL CGEditProDoc::MakeNewBrushGroup	(CWnd * pParent)
// create a new brush group and add all currently-selected
// brushes and entities to the new group.
{
	CString	szName ;
	CString	szDefault ;
	int		NewGroupId = 0 ;
	int		ModalResult ;
	GroupListType *Groups = m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level);

	szDefault.LoadString( IDS_NEWGROUP ) ;
	CKeyEditDlg * GNameDlg = new CKeyEditDlg( pParent, szDefault, &szName ) ;

	ModalResult = GNameDlg->DoModal() ;
	delete GNameDlg ;

	if( ModalResult == IDOK )
	{
		szName.TrimLeft() ;
		szName.TrimRight() ;
		if( szName.IsEmpty() == FALSE )
		{
			NewGroupId = Group_AddToList( Groups );
			if( NewGroupId != 0 ) // You can't add the default group
			{
				Group_SetName( Groups, NewGroupId, szName ) ;
				m_iCurrentGroup = NewGroupId;
				AddSelToGroup() ;
			}//Good new Group ID
			else
			{
				AfxMessageBox( IDS_CREATEGROUPFAILED ) ;
			}
		}//Not empty name
	}// OK return from name dialog

	return NewGroupId ; // 0 for failed or new non-default group number

}/* CGEditProDoc::MakeNewBrushGroup */



int CGEditProDoc::FillBrushGroupCombo(CComboBox &cb)
{// Fill brush group combo box with group names and IDs
	int CurSel = LB_ERR;
	GroupList_FillCombobox( m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level), &cb, m_iCurrentGroup ) ;
	return CurSel;
}



geBoolean fdocAddEntityToGroupCallback (CEntity &Ent, void *lParam)
{
	if (Ent.IsSelected ())
	{
		fdocAddEntityData *pData;

		pData = (fdocAddEntityData *)lParam;
		Group_AddEntity (pData->Groups, pData->CurrentGroup, &Ent);
	}
	return GE_TRUE;
}


void CGEditProDoc::AddSelToGroup(void)
// add selected brushes/entities to current group, no UI
{
	fdocAddEntityData entData;
	int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);

	entData.Groups = m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level);
	entData.CurrentGroup = m_iCurrentGroup;

	// tag all selected brushes with this group id...
	for (int i = 0; i < NumSelBrushes; i++)
	{
		Brush *pBrush;

		pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);
// changed QD Actors
		if(strstr(Brush_GetName(pBrush),".act")!=NULL)
			continue;
// end change
		Group_AddBrush (entData.Groups, m_iCurrentGroup, pBrush);
	}
	// tag all selected entities with this group id...
	m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, &entData, ::fdocAddEntityToGroupCallback);
}


void CGEditProDoc::RemovesSelFromGroup(void)
{
	fdocAddEntityData entData;
	int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);

	entData.Groups = m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level);
	entData.CurrentGroup = m_iCurrentGroup;

	for(int i = 0; i < NumSelBrushes; i++)
	{
		Brush *pBrush = NULL;

		pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);
		if (pBrush)
		{
		if( Brush_GetGroupId(pBrush) == m_iCurrentGroup )
		{
			Group_RemoveBrush( entData.Groups, m_iCurrentGroup, pBrush );
		}
		}
	}

	m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, &entData, ::fdocRemoveEntityFromGroupCallback);
}


void CGEditProDoc::SelectGroupBrushes(BOOL Select, int WhichGroup) // select/unselect all brushes & entities in the current group
{
	BrushSelectCallbackData SelectData;

	SelectData.Select = Select;
	SelectData.WhichGroup = WhichGroup;
	SelectData.pDoc = this;

	m_pLevelMgr->EnumBrushes (&m_CurrentDocVars.m_Level, &SelectData, ::BrushSelect ) ;
	m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, &SelectData, ::EntitySelect ) ;
	UpdateSelected ();		//	calls UpdateMainControls)()
}


BOOL CGEditProDoc::OneBrushSelectedOnly(void)
{
	return ((m_iModeTool==ID_TOOLS_TEMPLATE) ||
			((SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes)==1) && (m_iNumSelEntities == 0) &&
			 (SelFaceList_GetSize (m_CurrentDocVars.m_pSelFaces) == 0)));
}


void CGEditProDoc::OnUpdateBrushSubtractfromworld(CCmdUI* pCmdUI)
{
	BOOL EnableFlag;

	EnableFlag = ((m_iModeTool==ID_GENERALSELECT) &&
				  (SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes)==1) &&
				  (SelFaceList_GetSize (m_CurrentDocVars.m_pSelFaces) == 0) &&
				  (m_iNumSelEntities == 0));
	pCmdUI->Enable (EnableFlag);
}


void CGEditProDoc::OnUpdateThingAttributes(CCmdUI* pCmdUI)
{
	if(m_iModeTool==ID_GENERALSELECT)
	{
		switch (m_AdjustMode)
		{
			case ADJUST_MODE_BRUSH :
				pCmdUI->Enable (SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes) > 0);
				break;
			case ADJUST_MODE_FACE :
				pCmdUI->Enable (SelFaceList_GetSize (m_CurrentDocVars.m_pSelFaces) > 0);
				break;
			default :
				assert (0);
				break;
		}
	}
	else
	{
		pCmdUI->Enable(0);
	}
}

void CGEditProDoc::OnUpdateToolsBrushShowassociatedentity(CCmdUI* pCmdUI)
{
	if((m_iModeTool==ID_GENERALSELECT) && (m_AdjustMode==ADJUST_MODE_BRUSH))
	{
		pCmdUI->Enable (SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes)==1);
	}
	else
	{
		pCmdUI->Enable(0);
	}
}


void CGEditProDoc::OnUpdateEntitiesEditor(CCmdUI* pCmdUI)
{
	BOOL	bEnable ;

	bEnable = (	m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level)->GetSize() ) ? TRUE : FALSE ;
	pCmdUI->Enable( bEnable ) ;
}

void CGEditProDoc::OnNewLibObject()
{
	// save currently-selected stuff to named lib object.
	CtLevel *NewLevel = NULL;
	char NewObjectName[MAX_PATH];
	char NewObjectFullPath[MAX_PATH];
	int i;
	AddPremadeEnumData EnumData;

	if (!::GetObjectName (NewObjectName, NewObjectFullPath, this))
	{
		return;
	}
// changed QD Actors
	NewLevel = m_pLevelMgr->Create (m_pLevelMgr->GetWadPath (&m_CurrentDocVars.m_Level), m_pLevelMgr->GetHeadersDirectory (&m_CurrentDocVars.m_Level),
									m_pLevelMgr->GetActorsDirectory (&m_CurrentDocVars.m_Level), m_pLevelMgr->GetPawnIniPath (&m_CurrentDocVars.m_Level));
// end change
	if (NewLevel == NULL)
	{
		AfxMessageBox ("Error creating object.");
		return;
	}

	EnumData.NewLevel = NewLevel;
	EnumData.OldLevel = &m_CurrentDocVars.m_Level;

	int NumSelBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes);
	// add all selected brushes and entities to the new level
	for (i = 0; i < NumSelBrushes; ++i)
	{
		Brush *NewBrush = NULL;
		Brush *OldBrush = NULL;

		OldBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pSelBrushes, i);
// changed QD Actors
		if(strstr(Brush_GetName(OldBrush),".act")!=NULL)
			continue;
// end change
		NewBrush = Brush_Clone (OldBrush);
		m_pLevelMgr->AppendBrush (NewLevel, NewBrush);

		// add any group or model that's referenced
		::fdocAddReferencedModel (&EnumData, Brush_GetModelId (NewBrush));
		::fdocAddReferencedGroup (&EnumData, Brush_GetGroupId (NewBrush));
	}

	m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, &EnumData, ::fdocAddSelectedEntities);

	// m_vecSelectedGeoCenter contains center.  We're going to translate
	// the center to the origin.  That means that every brush and
	// entity will be moved so that the m_vecSelectedGeoCenter of the premade
	// object will be the origin.
	{
		geVec3d VecXlate;

		geVec3d_Scale (&m_vecSelectedGeoCenter, -1.0f, &VecXlate);
		m_pLevelMgr->TranslateAll (NewLevel, &VecXlate);
	}

	// ok, everything's added.  Write it to the file.
	if (!m_pLevelMgr->WriteToFile (NewLevel, NewObjectFullPath))
	{
		AfxMessageBox ("Error writing object to file.");
	}
	else
	{
		// add level name to objects list...
//		m_pMainFrame->m_wndTabControls->m_pBrushEntityDialog->SetupObjectListCombo (); // old gedit
		m_pMainFrame->UpdateMainControls();	// new g3dc
	}

	m_pLevelMgr->Destroy (&NewLevel);
}

// -------------------------------------------------------------------------------
// places the specified object down into the level at the specified location
// TODO: later want to make it so that newly added brushes/entities are all placed
// in a single group and selected so the user can immediately begin to move
// and adjust the object within the level

//	called by: OnBrushAddtoWorld()
geBoolean CGEditProDoc::PlaceObject( const char *ObjectName, const geVec3d *location, CGEditProDocVars *pDocVars)
{
	char WorkPath[MAX_PATH];

	::FilePath_AppendName (Prefs_GetObjectsDir (GetPrefs ()), ObjectName, WorkPath);
	::FilePath_SetExt (WorkPath, ".3dt", WorkPath);

	return ImportFile (WorkPath, location, pDocVars);
}



//	called by OnFileImport() and PlaceObject()
geBoolean CGEditProDoc::ImportFile (const char *PathName, const geVec3d *location, CGEditProDocVars *pDocVars)
{
	const char		*ErrMsg = "#ERROR#";
	CtLevel			*NewLevel = NULL;
	EntityViewList	*pEntityView = NULL;
	int				i;
	const Prefs		*pPrefs = NULL;

	pPrefs = GetPrefs ();
	if (pPrefs)
	{
// changed QD Actors
		NewLevel = m_pLevelMgr->CreateFromFile (PathName, &ErrMsg, Prefs_GetHeadersList (pPrefs),
												Prefs_GetActorsList (pPrefs), Prefs_GetPawnIni (pPrefs));
// end change
		if (NewLevel == NULL)
		{
			AfxMessageBox (ErrMsg);
			return GE_FALSE;
		}

		// Unselects everything so that brushes/entities can be added
		// This results in a DoGeneralSelect() call that sets the mMode to ID_GENERALSELECT
		ResetAllSelections ();

		// move the object to the new position
		m_pLevelMgr->TranslateAll (NewLevel, location);

		{
			// copy any groups (except default...)
			int GroupId;
			GroupListType *NewGroups = NULL;
			GroupListType *OldGroups = NULL;
			GroupIterator gi;

			OldGroups = m_pLevelMgr->GetGroups (&pDocVars->m_Level);
			NewGroups = m_pLevelMgr->GetGroups (NewLevel);

			if (OldGroups && NewGroups)
			{
				m_pLevelMgr->CollapseGroups (&pDocVars->m_Level, 1);
				m_pLevelMgr->CollapseGroups (NewLevel, Group_GetCount (OldGroups));

				GroupId = Group_GetFirstId (NewGroups, &gi);
				while (GroupId != NO_MORE_GROUPS)
				{
					if (GroupId != 0)	// don't add default group
					{
						Group *pGroup = NULL;
						Group *NewGroup = NULL;
						char GroupName[MAX_PATH];

						pGroup = GroupList_GetFromId (NewGroups, GroupId);
						if (pGroup)
						{
							strcpy (GroupName, Group_GetName (pGroup));

							// make sure that group name doesn't conflict
							while (Group_GetIdFromName (OldGroups, GroupName) != NO_MORE_GROUPS)
							{
								strcat (GroupName, "x");
							}

							NewGroup = Group_Clone (pGroup);
							if (NewGroup)
							{
								Group_SetGroupName (NewGroup, GroupName);

								GroupList_Add (OldGroups, NewGroup);
							}
						}
					}
					GroupId = Group_GetNextId (NewGroups, &gi);
				}
			}
		}

		{
			// copy models
			ModelList *NewModels = NULL, *OldModels = NULL;
			ModelIterator mi;
			Model *pModel = NULL;

			OldModels = m_pLevelMgr->GetModelInfo (&pDocVars->m_Level)->Models;
			NewModels = m_pLevelMgr->GetModelInfo (NewLevel)->Models;

			if (OldModels && NewModels)
			{
				m_pLevelMgr->CollapseModels (&pDocVars->m_Level, 1);
				m_pLevelMgr->CollapseModels (NewLevel, ModelList_GetCount (OldModels) + 1);

				pModel = ModelList_GetFirst (NewModels, &mi);
				while (pModel != NULL)
				{
					Model *NewModel = NULL;
					char ModelName[MAX_PATH];

					// make sure names don't clash
					strcpy (ModelName, Model_GetName (pModel));
					while (ModelList_FindByName (OldModels, ModelName) != NULL)
					{
						strcat (ModelName, "x");
					}
					NewModel = Model_Clone (pModel);
					if (NewModel)
					{
						Model_SetName (NewModel, ModelName);
						ModelList_AddModel (OldModels, NewModel);
					}
					pModel = ModelList_GetNext (NewModels, &mi);
				}
			}
		}


		{
			fdocAddPremadeData AddPremadeData;

			AddPremadeData.pDoc = this;
			AddPremadeData.pDocVars = pDocVars;
			AddPremadeData.NewLevel = NewLevel;
//changed QD Actors
			CEntityArray *Entities;

			Entities = m_pLevelMgr->GetEntities (NewLevel);

			for(i=0;i < Entities->GetSize();i++)
			{
				Brush *pBrush = (*Entities)[i].GetActorBrush();

				if(pBrush!=NULL)
				{
					SelBrushList_Remove(pDocVars->m_pSelBrushes, pBrush);
					m_pLevelMgr->RemoveBrush(NewLevel, pBrush);
					(*Entities)[i].DeleteActorBrush();
				}
			}
// end change
			// add entities
			m_pLevelMgr->EnumEntities (NewLevel, &AddPremadeData, ::fdocAddPremadeEntity);
		}

		{
			// add brushes
			Brush *pBrush = NULL, *NewBrush = NULL;
			BrushList *NewBrushes = NULL;
			BrushIterator bi;

			// fixup DIB ids on brush faces
			//		m_pLevelMgr->EnumLeafBrushes (NewLevel, &m_CurrentDocVars.m_Level, m_pLevelMgr->FaceFixupCallback);
			m_pLevelMgr->EnumLeafBrushes (NewLevel, NewLevel, FaceFixupCallback);

			// Move brushes from loaded level to doc's level
			NewBrushes = m_pLevelMgr->GetBrushes (NewLevel);
			if (NewBrushes)
			{
				pBrush = BrushList_GetFirst (NewBrushes, &bi);
				while (pBrush != NULL)
				{
					NewBrush = pBrush;
					pBrush = BrushList_GetNext (&bi);
					m_pLevelMgr->RemoveBrush (NewLevel, NewBrush);
					m_pLevelMgr->AppendBrush (&pDocVars->m_Level, NewBrush);
				}
			}
		}

		// update entity visibility info
		pEntityView	=m_pLevelMgr->GetEntityVisibilityInfo (&pDocVars->m_Level);
		for (i = 0; i < pEntityView->nEntries; ++i)
		{
			m_pLevelMgr->EnumEntities (&pDocVars->m_Level, &pEntityView->pEntries[i], ::fdocSetEntityVisibility);
		}

		m_pLevelMgr->Destroy (&NewLevel);
		return GE_TRUE;
	}
	return GE_FALSE;
}


// -----------------------------------------------------------------------------
// insures all of the Dib ID's in the brush are correct
// I would like to have put this in the brush.cpp module,  but fixing up the dib ID's requires
// a function here in the doc and I wanted to avoid coupling brush.cpp to
// GEditProDoc.cpp
// returns 1 on success,  0 on failure
geBoolean CGEditProDoc::FixUpBrushDibIDs(Brush *b)
{
	ASSERT(b);
	geBoolean NoErrors;

	NoErrors = GE_TRUE;
	// flags failure to find a particular texture

	// loop over all the faces in the brush
	for (int i = 0; i < Brush_GetNumFaces(b); i++ )
	{
		Face *pFace;
		const char *pName;
		uint16 DibId;

		pFace = Brush_GetFace (b, i);
		pName = Face_GetTextureName (pFace);
		DibId = m_pLevelMgr->GetDibId (&m_CurrentDocVars.m_Level, pName);
		if (DibId == 0xffff)
		{
			DibId = 1;
			NoErrors = GE_FALSE;
		}
		Face_SetTextureDibId (pFace, DibId);
		if (NoErrors)
		{
			const WadFileEntry * const pbmp = m_pLevelMgr->GetWadBitmap (&m_CurrentDocVars.m_Level, pName);

			Face_SetTextureSize (pFace, pbmp->Width, pbmp->Height);
		}
	}

	return NoErrors;
}

#pragma warning (disable:4100)
void CGEditProDoc::SnapScaleNearest(int sides, int inidx, ViewVars *v)
{
	geFloat	bsnap;

	m_iLastOp		=BRUSH_SCALE;


	bsnap = 1.0f ;
	if (m_pLevelMgr->UseGrid (&m_CurrentDocVars.m_Level))
	{
		bsnap = m_pLevelMgr->GetGridSnapSize (&m_CurrentDocVars.m_Level);
	}

	if(m_iModeTool == ID_TOOLS_TEMPLATE)
	{
		Brush_SnapScaleNearest(m_pCurBrush, bsnap, sides, inidx, &m_vecFinalScale, &m_iScaleNum);
	}
	else
	{
		int i;
		int NumBrushes = SelBrushList_GetSize (m_CurrentDocVars.m_pTempSelBrushes);

		for (i = 0; i < NumBrushes; ++i)
		{
			Brush *pBrush = SelBrushList_GetBrush (m_CurrentDocVars.m_pTempSelBrushes, i);
			Brush_SnapScaleNearest(pBrush, bsnap, sides, inidx, &m_vecFinalScale, &m_iScaleNum);
		}
	}
}
#pragma warning (default:4100)

geBoolean CGEditProDoc::ValidateEntities(CGEditProDocVars *pDocVars)
{
	fdocEntityValidateData evData;

	evData.Groups = m_pLevelMgr->GetGroups (&pDocVars->m_Level);
	evData.AllGood = GE_TRUE;
	evData.pEntityTable = m_pLevelMgr->GetEntityDefs (&pDocVars->m_Level);

	m_pLevelMgr->EnumEntities (&pDocVars->m_Level, &evData, ::fdocValidateEntity);

	return evData.AllGood;
}

geBoolean fdocValidateBrush (Brush *pBrush, void *lParam)
{
	fdocBrushValidateData *pData = NULL;
	int nGroupId;

	pData = (fdocBrushValidateData *)lParam;

	if (pData)
	{
	nGroupId = Brush_GetGroupId( pBrush ) ;
	if( nGroupId != 0 )
	{
		if( GroupList_IsValidId( pData->Groups, nGroupId ) == FALSE )
		{
			char const *pName = NULL;

			pName = Brush_GetName (pBrush);
			if (pName)
			{
//			ConPrintf("Brush %s: Group %d missing--set to no group\n", pName, nGroupId );	// old gedit
			CGlobals::GetActiveDocument()->m_pMainFrame->ConPrintf("Brush %s: Group %d missing--set to no group\n", pName, nGroupId );	// new g3dc
			Brush_SetGroupId( pBrush, 0 ) ;
			pData->AllGood = GE_FALSE ;
			}
		}
	}
	return GE_TRUE;
	}
	return GE_TRUE;
}

geBoolean CGEditProDoc::ValidateBrushes( void )
{
	fdocBrushValidateData bvData;

	bvData.Groups = m_pLevelMgr->GetGroups (&m_CurrentDocVars.m_Level);
	bvData.AllGood = GE_TRUE;

	m_pLevelMgr->EnumBrushes (&m_CurrentDocVars.m_Level, &bvData, ::fdocValidateBrush);

	return bvData.AllGood ;
}


void CGEditProDoc::SelectTab( int nTabIndex )
{
	if( m_pMainFrame )
	{
		m_pMainFrame->SetCommandPanelTab((MAINFRM_COMMANDPANEL_TAB)nTabIndex);
	}
}/* CGEditProDoc::SelectTab */

/* EOF: GEditProDoc.cpp */

void CGEditProDoc::OnFileOpen()
{
	static const char	FDTitle[]	="GEditPro File Opener";

	CFileDialog dlg(TRUE, "3dt", NULL, OFN_HIDEREADONLY	| OFN_OVERWRITEPROMPT,
		"GEDIT Files (*.3DT)|*.3DT|MAP Files (*.MAP)|*.MAP|Leak Files (*.PNT)|*.PNT||");

	dlg.m_ofn.lpstrTitle	=FDTitle;
	dlg.m_ofn.lpstrInitialDir = LastPath;
	if(dlg.DoModal()==IDOK)
	{
		switch(dlg.m_ofn.nFilterIndex)
		{
		case 1 :
			AfxGetApp()->OpenDocumentFile(dlg.GetPathName());
			break;
		case 2 :
			LoadMapFile(dlg.GetPathName());
			break;
		case 3 :
			m_bLeakLoaded	=LoadLeakFile(dlg.GetPathName());
			break;
		default:
			assert(0);
		}
	}
}

void CGEditProDoc::OnFileImport()
{
	static const char FDTitle[] = "GEditPro File Importer";
	CFileDialog dlg(TRUE, "3dt", NULL, OFN_OVERWRITEPROMPT,	"GEditPro Files (*.3DT)|*.3DT||");

	dlg.m_ofn.lpstrTitle = FDTitle;
	if (dlg.DoModal () == IDOK)
	{
		if (dlg.m_ofn.nFilterIndex == 1)
		{
			char Name[MAX_PATH];
			geVec3d loc;

			geVec3d_Clear (&loc);
			::FilePath_SetExt (dlg.GetPathName (), ".3dt", Name);
			ImportFile (Name, &loc, &m_CurrentDocVars);
		}
	}
}

geBoolean	CGEditProDoc::LoadLeakFile(const char *Filename)
{
	CFile	Infile;
	int		sig;
	int		PointsToRead;

	if(!Infile.Open(Filename, CFile::modeRead))
	{
		AfxMessageBox("Error opening the Leak File!", MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	Infile.Read(&sig, sizeof(int));

	if(sig!=0x4b41454c)
	{
		AfxMessageBox("File is not a leakfile!", MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	Infile.Read(&PointsToRead, sizeof(int));

//	ConPrintf("Loaded leak file %s with %d Points...\n", Filename, PointsToRead);	// old gedit
	CGlobals::GetActiveDocument()->m_pMainFrame->ConPrintf("Loaded leak file %s with %d Points...\n", Filename, PointsToRead);	// new g3dc

	m_iNumLeakPoints = (PointsToRead > 1) ? PointsToRead : 2;
	m_pVecLeakPoints	=(geVec3d *)geRam_Allocate(sizeof(geVec3d)*m_iNumLeakPoints);

	if (!m_pVecLeakPoints)
		return FALSE;

	Infile.Read(m_pVecLeakPoints, sizeof(geVec3d)*PointsToRead);
	if (PointsToRead == 1)
	{
		geVec3d_Clear (&m_pVecLeakPoints[1]);
	}

	return TRUE;
}


geBoolean	CGEditProDoc::LoadMapFile(const char *FileName)
{
	FaceList	*fl, *mfl;
	Face		*f;
	Brush		*b;
	FILE		*mf;

	geVec3d		FaceVerts[3];
	int			i, k, bc, sx, sy, contents, surfflags, value; // changed QD
	geFloat		scx, scy, rot; // changed QD
	char		*sp = NULL;
	char		szTex[_MAX_PATH];
	uint16		dibid;

	assert (FileName != NULL);

	mf	=fopen(FileName, "r");
	mfl	=FaceList_Create(64);

	if(!mf)
	{
		AfxMessageBox (IDS_CANT_OPEN_FILE);
		return GE_FALSE;
	}
	//get open brace
	for(bc=fgetc(mf);bc!=EOF && bc!='{';bc=fgetc(mf));
	if(bc=='{')
	{
		for(;;)
		{
			//get open brace
			for(bc=fgetc(mf);bc!=EOF && bc!='{' && bc!='}';bc=fgetc(mf));
			if(bc=='{')
			{
				surfflags	=value=0;
				contents	=CONTENTS_SOLID;	//default
				for(k=0;;)
				{

					for(i=0;i<3;i++)
					{
						//get ( or }
						for(bc=fgetc(mf);bc!=EOF && bc!='}' && bc!='(';bc=fgetc(mf));
						if(bc=='(')
						{
							fscanf(mf, "%f%f%f", &FaceVerts[i].X, &FaceVerts[i].Z, &FaceVerts[i].Y);
							FaceVerts[i].Z	=-FaceVerts[i].Z;
						}
						else
						{
							break;
						}
					}
					if(i==3)
					{
						//get )
						for(bc=fgetc(mf);bc!=EOF && bc!=')';bc=fgetc(mf));
						fscanf(mf, "%s %i %i %i %f %f", szTex, &sx, &sy, &rot, &scx, &scy);
						dibid	=m_pLevelMgr->GetDibId(&m_CurrentDocVars.m_Level, szTex);
						if(dibid == 0xffff)
						{
							//try stripping /
							sp	=strrchr(szTex, '/');
							if(sp)
							{
								sp++;
								dibid	=m_pLevelMgr->GetDibId(&m_CurrentDocVars.m_Level, sp);
							}
						}
						if(!sp)
						{
							sp	=szTex;
						}
						f	=Face_Create(3, FaceVerts, dibid);
						for(;*sp=='#' || *sp=='*' || *sp=='+';sp++);	//strip illegal chars
						if(f)
						{
							Face_SetTextureName(f, sp);
							Face_SetTextureScale(f, scx, scy);
							Face_SetTextureShift(f, sx, sy);
							Face_SetTextureRotate(f, rot); // changed QD
						}
						//look for flags
						for(bc=fgetc(mf);bc!=EOF && bc <=32 && bc && bc!='\n';bc=fgetc(mf));
						if(bc!='\n' && bc)
						{
							ungetc(bc, mf);
							fscanf(mf, "%i %i %i", &contents, &surfflags, &value);
							if(f)
							{
								Face_SetLight(f, (surfflags	&	SURF_LIGHT));
								Face_SetSky(f, (surfflags	&	SURF_SKY));
								Face_SetLightIntensity(f, value);
							}
						}
						if(f)
						{
							if (!fdocCheckAddFace (&mfl, f))
							{
								// this is pretty ugly.
								// If we can't add the face, then set num faces to 0 and exit.
								k = 0;
								break;
							}
//							FaceList_AddFace(mfl, f);
							k++;
						}
					}
					else
					{
						break;
					}
				}
				if(k > 3)
				{
					fl	=FaceList_Create(k);
					for(i=0;i<k;i++)
					{
						FaceList_AddFace(fl, Face_Clone(FaceList_GetFace(mfl, i)));
					}
					FaceList_Destroy(&mfl);
					mfl	=FaceList_Create(64);
					b	=Brush_Create(BRUSH_LEAF, fl, NULL);
					Brush_SealFaces(&b);
					if(b)
					{
						m_pLevelMgr->AppendBrush	(&m_CurrentDocVars.m_Level, b);
						Brush_SetSolid		(b, (contents	&	CONTENTS_SOLID));
						Brush_SetWindow		(b, (contents	&	CONTENTS_WINDOW));
						Brush_SetWavy		(b, (contents	&	CONTENTS_WATER));
						Brush_SetTranslucent(b, (contents	&	CONTENTS_WATER));
						Brush_SetClip		(b, (contents	&	CONTENTS_PLAYERCLIP));
						Brush_SetClip		(b, (contents	&	CONTENTS_MONSTERCLIP));
						Brush_SetDetail		(b, (contents	&	CONTENTS_DETAIL));
						Brush_SetTranslucent(b, (contents	&	CONTENTS_TRANSLUCENT));
					}
				}
			}
			else if(bc=='}')
			{
				break;
			}
		}
	}
	char	Key[255], Value[255];
	//get open brace
	for(bc=fgetc(mf);bc!=EOF && bc!='{';bc=fgetc(mf));
	if(bc=='{')
	{
		for(;;)
		{
			CEntity	NewEnt;
			for(;;)
			{
				for(bc=fgetc(mf);bc!=EOF && bc!='"' && bc!='}' && bc!='{';bc=fgetc(mf));
				if(bc=='"')
				{
					for(i=0;;i++)
					{
						bc		=fgetc(mf);
						if(bc!='"')
						{
							Key[i]	=(char)bc;	//hope theres no high nibble info
						}
						else
						{
							Key[i]	=(char)0;
							break;
						}
					}
					for(bc=fgetc(mf);bc!=EOF && bc!='"' && bc!='}';bc=fgetc(mf));
					if(bc!='"')
					{
						break;
					}
					for(i=0;;i++)
					{
						bc		=fgetc(mf);
						if(bc!='"')
						{
							Value[i]=(char)bc;
						}
						else
						{
							Value[i]=(char)0;
							break;
						}
					}
//					ConPrintf("Key: %s : Value: %s\n", Key, Value);
					if(!stricmp(Key, "_color"))
					{
						strcpy(Key, "color");
						geVec3d	TempVec;
						sscanf((char const *)Value, "%f %f %f", &TempVec.X, &TempVec.Y, &TempVec.Z);
						sprintf(Value, "%d %d %d",
							Units_Round(TempVec.X * 255.0f),
							Units_Round(TempVec.Y * 255.0f),
							Units_Round(TempVec.Z * 255.0f));
					}
					else if(!strnicmp(Value, "info_player", 11))
					{
						strcpy(Value, "PlayerStart");
					}
					else if(!strnicmp(Value, "monster_", 8))
					{
						strcpy(Value, "AIPlayerStart");
					}
					else if(!stricmp(Key, "origin"))
					{
						geVec3d	TempVec;
						sscanf((char const *)Value, "%f %f %f", &TempVec.X, &TempVec.Z, &TempVec.Y);
						TempVec.Z	=-TempVec.Z;
						sprintf(Value, "%d %d %d", Units_Round(TempVec.X), Units_Round(TempVec.Y), Units_Round(TempVec.Z));
					}
					else if(!strnicmp(Value, "weapon_", 7))
					{
						strcpy(Value, "PowerUp");
					}
					else if(!strnicmp(Value, "path_", 5))
					{
						strcpy(Value, "AIMapWayPoint");
					}
					NewEnt.SetKeyValue(Key, Value);
				}
				else if(bc=='{' && NewEnt.GetNumKeyValuePairs())	//brush models
				{
					geXForm3d	XfmDelta;
					for(;;)
					{
						if(bc=='{')
						{
							surfflags	=value=0;
							contents	=CONTENTS_SOLID;	//default
							for(k=0;;)
							{
								for(i=0;i<3;i++)
								{
									//get ( or }
									for(bc=fgetc(mf);bc!=EOF && bc!='}' && bc!='(';bc=fgetc(mf));
									if(bc=='(')
									{
										fscanf(mf, "%f%f%f", &FaceVerts[i].X, &FaceVerts[i].Z, &FaceVerts[i].Y);
										FaceVerts[i].Z	=-FaceVerts[i].Z;
									}
									else
									{
										break;
									}
								}
								if(i==3)
								{
									//get )
									for(bc=fgetc(mf);bc!=EOF && bc!=')';bc=fgetc(mf));
									fscanf(mf, "%s %i %i %i %f %f", szTex, &sx, &sy, &rot, &scx, &scy);
									dibid	=m_pLevelMgr->GetDibId(&m_CurrentDocVars.m_Level, szTex);
									if(dibid == 0xffff)
									{
										//try stripping /
										sp	=strrchr(szTex, '/');
										if(sp)
										{
											sp++;
											dibid	=m_pLevelMgr->GetDibId(&m_CurrentDocVars.m_Level, sp);
										}
									}
									if(!sp)
									{
										sp	=szTex;
									}
									f	=Face_Create(3, FaceVerts, dibid);
									for(;*sp=='#' || *sp=='*' || *sp=='+';sp++);	//strip illegal chars
									Face_SetTextureName(f, sp);
									Face_SetTextureScale(f, scx, scy);
									Face_SetTextureShift(f, sx, sy);
									Face_SetTextureRotate(f, rot); // changed QD

									//look for flags
									for(bc=fgetc(mf);bc!=EOF && bc <=32 && bc && bc!='\n';bc=fgetc(mf));
									if(bc!='\n' && bc)
									{
										ungetc(bc, mf);
										fscanf(mf, "%i %i %i", &contents, &surfflags, &value);
										Face_SetLight(f, (surfflags	&	SURF_LIGHT));
										Face_SetSky(f, (surfflags	&	SURF_SKY));
										Face_SetLightIntensity(f, value);
									}
									if (!fdocCheckAddFace (&mfl, f))
									{
										k = 0;
										break;
									}
									k++;
								}
								else
								{
									break;
								}
							}
							if(k > 3)
							{
								fl	=FaceList_Create(k);
								for(i=0;i<k;i++)
								{
									FaceList_AddFace(fl, Face_Clone(FaceList_GetFace(mfl, i)));
								}
								FaceList_Destroy(&mfl);
								mfl	=FaceList_Create(64);
								b	=Brush_Create(BRUSH_LEAF, fl, NULL);
								Brush_SealFaces(&b);
								Brush_SealFaces(&b);
								if(b)
								{
									m_pLevelMgr->AppendBrush	(&m_CurrentDocVars.m_Level, b);
									Brush_SetSolid		(b, (contents	&	CONTENTS_SOLID));
									Brush_SetWindow		(b, (contents	&	CONTENTS_WINDOW));
									Brush_SetWavy		(b, (contents	&	CONTENTS_WATER));
									Brush_SetTranslucent(b, (contents	&	CONTENTS_WATER));
									Brush_SetClip		(b, (contents	&	CONTENTS_PLAYERCLIP));
									Brush_SetClip		(b, (contents	&	CONTENTS_MONSTERCLIP));
									Brush_SetDetail		(b, (contents	&	CONTENTS_DETAIL));
									Brush_SetTranslucent(b, (contents	&	CONTENTS_TRANSLUCENT));
								}
								SelBrushList_Add (m_CurrentDocVars.m_pSelBrushes, b);
							}
						}
						else if(bc=='}')
						{
							break;
						}
					}
					strcpy(Key, NewEnt.GetClassname());
					i	=0;

					ModelInfo_Type *ModelInfo = m_pLevelMgr->GetModelInfo (&m_CurrentDocVars.m_Level);
					while (ModelList_FindByName (ModelInfo->Models, Key) != NULL)
					{
						sprintf(Key, "%s%d", NewEnt.GetClassname(), i++);
					}
					// add the new model to the list.
					// This will set the model id fields in the model's brushes
					if (ModelList_Add (ModelInfo->Models, Key, m_CurrentDocVars.m_pSelBrushes))
					{
						Model *m;

						// get current model and update its object-to-world transform
						m = ModelList_FindByName (ModelInfo->Models, Key);
						assert (m != NULL);
						ModelInfo->CurrentModel	= Model_GetId (m);

//						UpdateModelsList ();
						geXForm3d_SetIdentity (&XfmDelta);
						// we add the first key (identity)
						// (and don't allow it to be edited/deleted)
						Model_AddKeyframe(m, 0.0f, &XfmDelta);
					}
					SelBrushList_RemoveAll (m_CurrentDocVars.m_pSelBrushes);
				}
				else if(bc==EOF || bc=='}')
				{
					break;
				}
			}
			if(bc==EOF)
			{
				break;
			}
			else if(bc=='}')
			{
				NewEnt.SetGroupId ( 0 );
				NewEnt.UpdateOrigin (m_pLevelMgr->GetEntityDefs (&m_CurrentDocVars.m_Level));
				m_pLevelMgr->AddEntity (&m_CurrentDocVars.m_Level, NewEnt);
			}
		}
	}

	fclose(mf);
	if (mfl != NULL)
	{
		FaceList_Destroy (&mfl);
	}

	return	TRUE;
}

void CGEditProDoc::OnViewShowClip()
{
	m_bShowClipBrushes	=!(m_bShowClipBrushes);
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}

void CGEditProDoc::OnUpdateViewShowClip(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowClipBrushes);
}

void CGEditProDoc::OnViewShowDetail()
{
	m_bShowDetailBrushes	=!(m_bShowDetailBrushes);
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}

void CGEditProDoc::OnUpdateViewShowDetail(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowDetailBrushes);
}

void CGEditProDoc::OnViewShowHint()
{
	m_bShowHintBrushes	=!(m_bShowHintBrushes);
	//	Be very careful when speccing flags for UpdateAllViews()
	//	The wrong flags at the wrong time will totally screw things up
	UpdateAllViews(UAV_ALL3DVIEWS, NULL);
}

void CGEditProDoc::OnUpdateViewShowHint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowHintBrushes);
}

void CGEditProDoc::OnToolsBrushAttributes()
{
	if ((m_iModeTool == ID_GENERALSELECT) && (m_AdjustMode == ADJUST_MODE_BRUSH))
	{
		if ((SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes) > 0) &&
		    (m_AdjustMode == ADJUST_MODE_BRUSH) && (m_pBrushAttributes == NULL))
		{
//			m_pBrushAttributes = new CBrushAttributesDialog (this); // COMMENTED OUT FOR G3DC
		}
		SetModifiedFlag ();
	}
}


//	new for g3dc	NOT NECESSARY TO HAVE HERE.
//	this set of functions now called directly within the CMainFrame::UpdateMainControls()
void CGEditProDoc::ShowBrushAttributesDialog()
{
	;;
}




void CGEditProDoc::OnUpdateToolsBrushAttributes(CCmdUI* pCmdUI)
{
	pCmdUI->Enable ((m_iModeTool == ID_GENERALSELECT) &&
					(m_AdjustMode == ADJUST_MODE_BRUSH) &&
					(SelBrushList_GetSize (m_CurrentDocVars.m_pSelBrushes) > 0));
}


// obsolete old gedit
void CGEditProDoc::OnToolsFaceAttributes()
{
	if ((m_iModeTool == ID_GENERALSELECT) && (m_AdjustMode == ADJUST_MODE_FACE))
	{
		FaceAttributesDialog ();
		SetModifiedFlag ();
	}

}


//	obsolete old gedit
void CGEditProDoc::OnUpdateToolsFaceAttributes(CCmdUI* pCmdUI)
{
	pCmdUI->Enable ((m_iModeTool == ID_GENERALSELECT) &&
					(m_AdjustMode == ADJUST_MODE_FACE) &&
					(SelFaceList_GetSize (m_CurrentDocVars.m_pSelFaces) > 0));

}

void CGEditProDoc::OnEntityVisibility()
{
	EntityViewList *pListCopy;
	EntityViewList *pEntityView;

	pEntityView = m_pLevelMgr->GetEntityVisibilityInfo (&m_CurrentDocVars.m_Level);

	// copy existing...
	pListCopy = EntityViewList_Copy (pEntityView);

	if (pListCopy != NULL)
	{
		CEntityVisDlg Dlg;
		geBoolean Changed;

		Changed = GE_FALSE;

		// call dialog...
		if (Dlg.DoModal (pEntityView) == IDOK)
		{
			int i;

			// update changes
			for (i = 0; i < pEntityView->nEntries; ++i)
			{
				EntityViewEntry *pEntry;

				pEntry = &pEntityView->pEntries[i];
				if ((pEntry->IsVisible ^ pListCopy->pEntries[i].IsVisible) != 0)
				{
					Changed = GE_TRUE;

					m_pLevelMgr->EnumEntities (&m_CurrentDocVars.m_Level, pEntry, ::fdocSetEntityVisibility);
				}
			}
		}

		// get rid of copy
		EntityViewList_Destroy (&pListCopy);

		// redraw views if any changes made
		if (Changed)
		{
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			UpdateAllViews (UAV_ALL3DVIEWS, NULL);
		}
	}
}

void CGEditProDoc::OnToggleRebuildBspMode()
{
	CGlobals::g_bRebuildAlways = !CGlobals::g_bRebuildAlways;
}

void CGEditProDoc::OnUpdateToggleRebuildBspMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CGlobals::g_bRebuildAlways);
}

const Prefs *CGEditProDoc::GetPrefs (void)
{
	return ((CGEditProApp *)AfxGetApp ())->GetPreferences ();
}

void CGEditProDoc::OnLeveloptions()
{
	CLevelOptions  Dlg;

	//	prep for texel and greid info	//	post 0.57
	int	gdex[3]={GridSize_Centimeter, GridSize_Decimeter, GridSize_Meter};
	GridInfo *pGridInfo = NULL;
	static int TexelSnapValues[] = {1, 2, 4, 8, 16, 32};
	static int nSnapValues = sizeof (TexelSnapValues)/sizeof (int);

	pGridInfo = m_pLevelMgr->GetGridInfo (&m_CurrentDocVars.m_Level);
	if (pGridInfo)
	{
		Dlg.m_UseSnap			=pGridInfo->UseGrid;
		Dlg.m_SnapDegrees		=pGridInfo->RotationSnap;

		Dlg.MetricOrTexelSnap	=pGridInfo->SnapType;
		Dlg.MetricOrTexelGrid	=pGridInfo->GridType;

		if( Dlg.MetricOrTexelSnap == 0 )
		{
			switch(pGridInfo->MetricSnapSize)
			{
			case GridSize_Centimeter:
				Dlg.m_GridUnits	=0;
				break;
			case GridSize_Decimeter:
				Dlg.m_GridUnits	=1;
				break;
			case GridSize_Meter:
				Dlg.m_GridUnits	=2;
				break ;
			}
		}
		else
		{
			int i;

			Dlg.m_GridUnits = 0;
			for (i = 0; i < nSnapValues; ++i)
			{
				if (pGridInfo->TexelSnapSize == TexelSnapValues[i])
				{
					Dlg.m_GridUnits = i;
					break;
				}
			}
		}
	}

	if (Dlg.DoModal () == IDOK)
	{
		{
			//	gather grid and texel setting data	//	post 0.57
			pGridInfo->GridType = Dlg.MetricOrTexelGrid;	// Main Visible Grid mode
			// must add here 'cause metric grid is disabled
			Dlg.m_GridUnits += 3;
			switch( Dlg.m_GridUnits ) // SnapGrid RB's 0-2 METRIC 3-5 TEXEL
			{
			case 0:// Centimeter
			case 1:// Decimeter
			case 2:// Meter
				pGridInfo->MetricSnapSize	=gdex[Dlg.m_GridUnits];
				pGridInfo->SnapType = GridMetric;
				break ;
			case 3:// 1
			case 4:// 2
			case 5:// 4
			case 6:
			case 7:
			case 8:
				pGridInfo->TexelSnapSize = TexelSnapValues[Dlg.m_GridUnits - 3];
				pGridInfo->SnapType = GridTexel;
				break ;
			default :
				assert (0);	// unexpected grid units value!
				break;
			}

			pGridInfo->UseGrid = Dlg.m_UseSnap;
			pGridInfo->RotationSnap = (int)(Dlg.m_SnapDegrees);
			UpdateGridInformation();
			//	Be very careful when speccing flags for UpdateAllViews()
			//	The wrong flags at the wrong time will totally screw things up
			UpdateAllViews(UAV_GRID_ONLY, NULL);

			if ((Dlg.m_HeadersChanged) || (Dlg.m_TxlChanged) || (Dlg.m_ActorsChanged) || (Dlg.m_PawnIniChanged))
			{
				if (Dlg.m_HeadersChanged)
				{
					m_pLevelMgr->LoadEntityDefs (&m_CurrentDocVars.m_Level, Dlg.m_HeadersDir);

					if (ValidateEntities(&m_CurrentDocVars) == FALSE)
					{
						ShowConsole();
						AfxMessageBox( IDS_ENTITY_WARNING ) ;
					}
				}

				if (Dlg.m_TxlChanged)
				{
					m_pLevelMgr->SetWadPath (&m_CurrentDocVars.m_Level, Dlg.m_TextureLib);
					if (!m_pLevelMgr->LoadWad (&m_CurrentDocVars.m_Level))
					{
						CString Msg;

						AfxFormatString1 (Msg, IDS_CANTLOADTXL, Dlg.m_TextureLib);
						AfxMessageBox (Msg);
					}
				}
// changed QD
				if (Dlg.m_ActorsChanged)
				{
					m_pLevelMgr->SetActorsDir(&m_CurrentDocVars.m_Level, Dlg.m_ActorsDir);
					UpdateEntityActors();
					UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL);
				}
				if (Dlg.m_PawnIniChanged)
				{
					m_pLevelMgr->SetPawnIniPath (&m_CurrentDocVars.m_Level, Dlg.m_PawnIni);
					UpdateEntityActors();
					UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL);
				}
// end change

				//	set these new level options and resources into all members
				//	of DocVars in the update manager's list
				m_pUpdateMgr->UpdateLevelInfoInAll(&m_CurrentDocVars.m_Level);

				// update textures tab
				m_iCurTextureSelection = 0;

				m_pMainFrame->UpdateTextures();

				// update all brush faces
				BrushList_EnumLeafBrushes (m_pLevelMgr->GetBrushes (&m_CurrentDocVars.m_Level), this, ::fdocUpdateBrushFaceTextures);
				{
					// find the rendered view and set the wad size infos for it
					POSITION		pos = NULL;
					CGEditProView	*	pView = NULL;

					pos = GetFirstViewPosition();
					while( pos != NULL )
					{
						pView = (CGEditProView*)GetNextView(pos) ;
						if( Render_GetViewType( pView->VCam ) & (VIEWSOLID|VIEWTEXTURE|VIEWWIRE) )
						{
							Render_SetWadSizes (pView->VCam, m_pLevelMgr->GetWadSizeInfos (&m_CurrentDocVars.m_Level));
							break ;
						}
					}
				}

				if (CGlobals::g_bRebuildAlways)
				{
					RebuildTrees();
					//	Be very careful when speccing flags for UpdateAllViews()
					//	The wrong flags at the wrong time will totally screw things up
					UpdateAllViews (UAV_ALL3DVIEWS, NULL);
				}
			}
		}
	}
	m_pMainFrame->UpdateMainControls();
}




void	CGEditProDoc::OpenLevelOptionsDialog()
{
	OnLeveloptions();
}



//	VARIABLE MANAGEMENT -- UNDO/REDO


void CGEditProDoc::SaveBrushUndo()
{
}
bool CGEditProDoc::GetUndoAvailability()
{
	return m_bUndoAvailability;
}

bool CGEditProDoc::GetRedoAvailability()
{
	return m_bRedoAvailability;
}


bool CGEditProDoc::SetRedoAvailability(bool bState)
{
	m_bRedoAvailability = bState;
	return true;
}


bool CGEditProDoc::SetUndoAvailability(bool bState)
{
	m_bUndoAvailability = bState;
	return true;
}

bool	CGEditProDoc::DocVarsUndo()
{
	CMainFrame *pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetMainWnd();
	m_iCurrentTool = CURTOOL_NONE;
	ResetAllSelections();
	m_CurrentDocVars = m_pUpdateMgr->DocVarsUndo();
	ResetAllSelections();
	UpdateSelected();	//	calls UpdateMainControls()
	UpdateEntityOrigins();
// changed QD
	UpdateEntityActors();
	UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL, TRUE);
// end change
//	UpdateAllViews(UAV_ALLVIEWS /*| REBUILD_QUICK*/, NULL, TRUE);

	if (pMainFrame)
	{
		pMainFrame->UpdateMainControls();
	}
	return true;
}

bool	CGEditProDoc::DocVarsRedo()
{
	m_iCurrentTool = CURTOOL_NONE;
	ResetAllSelections();
	m_CurrentDocVars = m_pUpdateMgr->DocVarsRedo();
	ResetAllSelections();
	UpdateSelected();	//	calls UpdateMainControls()
	UpdateEntityOrigins();
// changed QD
	UpdateEntityActors();
	UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL, TRUE);
// end change
//	UpdateAllViews(UAV_ALLVIEWS /*| REBUILD_QUICK*/, NULL, TRUE);
	CMainFrame *pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame)
	{
		pMainFrame->UpdateMainControls();
	}
	return true;
}


CGEditProUpdateMgr	*CGEditProDoc::GetUpdateMgr()
{
	return m_pUpdateMgr;
}

CGEditProDocVars	CGEditProDoc::DocVarsGetByValue()
{
	return m_CurrentDocVars;
}

CGEditProDocVars	*CGEditProDoc::DocVarsGetPointer()
{
	return &m_CurrentDocVars;
}


CGEditProDocVars	CGEditProDoc::DocVarsCreateUniqueCopy(CGEditProDocVars theseDocVars)
{
	CEntity	tempEntity;
	CGEditProDocVars thisFunctionDocVars;

// changed QD Actors
// don't want to copy ActorBrushes, so remove them from the BrushList
	CEntityArray *Entities;
	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	for(int i=0;i < Entities->GetSize();i++)
	{
		Brush *pBrush = (*Entities)[i].GetActorBrush();

		if(pBrush!=NULL)
		{
			SelBrushList_Remove(m_CurrentDocVars.m_pSelBrushes, pBrush);
			m_pLevelMgr->RemoveBrush(&m_CurrentDocVars.m_Level, pBrush);
		}
	}
// end change

	thisFunctionDocVars = theseDocVars;
	thisFunctionDocVars.m_pTempSelBrushes = SelBrushList_Clone(theseDocVars.m_pTempSelBrushes);
	thisFunctionDocVars.m_pSelFaces = SelFaceList_Clone(m_CurrentDocVars.m_pSelFaces);

	thisFunctionDocVars.m_Level.m_pBrushes = BrushList_Clone(theseDocVars.m_Level.m_pBrushes);
	thisFunctionDocVars.m_Level.m_ModelInfo.Models = ModelList_Clone(theseDocVars.m_Level.m_ModelInfo.Models);
	thisFunctionDocVars.m_Level.m_pEntityDefs = EntityTable_Clone (theseDocVars.m_Level.m_pEntityDefs);
// this also clones the ActorBrushes
	thisFunctionDocVars.m_Level.m_pEntities = tempEntity.CloneEntityArray(theseDocVars.m_Level.m_pEntities);
	thisFunctionDocVars.m_Level.m_pGroups = Group_CloneList(theseDocVars.m_Level.m_pGroups);

	CString strTempHeaderDir = m_pLevelMgr->CloneHeadersDirectory(&theseDocVars.m_Level);
	thisFunctionDocVars.m_Level.m_pHeadersDir = Util_Strdup (strTempHeaderDir.GetBuffer(strTempHeaderDir.GetLength()));
	CString strTempWadPath = m_pLevelMgr->CloneWadPath(&theseDocVars.m_Level);
	thisFunctionDocVars.m_Level.m_pWadPath = Util_Strdup (strTempWadPath.GetBuffer(strTempWadPath.GetLength()));
// changed QD Actors
	CString strTempActorsDir = m_pLevelMgr->CloneActorsDir(&theseDocVars.m_Level);
	thisFunctionDocVars.m_Level.m_pActorsDir = Util_Strdup (strTempActorsDir.GetBuffer(strTempActorsDir.GetLength()));
	CString strTempPawnIni = m_pLevelMgr->ClonePawnIniPath(&theseDocVars.m_Level);
	thisFunctionDocVars.m_Level.m_pPawnIniPath = Util_Strdup (strTempPawnIni.GetBuffer(strTempPawnIni.GetLength()));

// append the cloned ActorBrushes to the new DocVars
	Entities = m_pLevelMgr->GetEntities (&thisFunctionDocVars.m_Level);
	for(i=0;i < Entities->GetSize();i++)
	{
		Brush *pBrush = (*Entities)[i].GetActorBrush();

		if(pBrush!=NULL)
		{
			m_pLevelMgr->AppendBrush(&thisFunctionDocVars.m_Level, pBrush);

			if((*Entities)[i].IsSelected())
				SelBrushList_Add(thisFunctionDocVars.m_pSelBrushes, pBrush);

			if(!m_bShowActors)
				Brush_SetVisible(pBrush, GE_FALSE);
		}
	}

// add the original ActorBrushes to the CurrentDocVars again
	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
	for(i=0;i < Entities->GetSize();i++)
	{
		Brush *pBrush = (*Entities)[i].GetActorBrush();

		if(pBrush!=NULL)
			m_pLevelMgr->AppendBrush(&m_CurrentDocVars.m_Level, pBrush);

	}
// end change
	return thisFunctionDocVars;
}




CtLevel*	CGEditProDoc::GetLevel()
{
	return &m_CurrentDocVars.m_Level;
}


bool	CGEditProDoc::DocVarsUpdate(CGEditProDocVars DocVars)
{
	//	inform UpdateMgr and get the docVars at the end of it's list
	m_CurrentDocVars = m_pUpdateMgr->DocVarsUpdate(DocVars);

	CMainFrame *pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame)
	{
		pMainFrame->UpdateMainControls();
	}

	return true;
}


// changed QD Actors
void CGEditProDoc::OnViewShowActors()
{
	m_bShowActors	=!(m_bShowActors);
	m_pLevelMgr->SetShowActors(&m_CurrentDocVars.m_Level, m_bShowActors);
	CEntityArray *Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);
	int i;

	if(!m_bShowActors)
	{
		for(i=0;i < Entities->GetSize();i++)
		{
			Brush *pBrush = (*Entities)[i].GetActorBrush();
			if(pBrush!=NULL)
			{
				Brush_SetVisible(pBrush, GE_FALSE);
			}
		}
	}
	else
	{
		for(i=0;i < Entities->GetSize();i++)
		{
			Brush *pBrush = (*Entities)[i].GetActorBrush();
			if(pBrush!=NULL)
			{
				Brush_SetVisible(pBrush, GE_TRUE);
			}
		}
	}
	UpdateAllViews(UAV_ALL3DVIEWS | REBUILD_QUICK, NULL);
}

void CGEditProDoc::OnUpdateViewShowActors(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowActors);
}


void CGEditProDoc::UpdateEntityActors()
{
	int	i;
	CEntityArray *Entities;

	Entities = m_pLevelMgr->GetEntities (&m_CurrentDocVars.m_Level);

	for(i=0;i < Entities->GetSize();i++)
	{

		char ActorFile[256], ActorDir[256], PawnIni[256];
		strcpy(PawnIni, m_pLevelMgr->GetPawnIniPath(&m_CurrentDocVars.m_Level));
		if((*Entities)[i].HasActor(ActorFile, PawnIni))
		{
			Brush *pBrush = (*Entities)[i].GetActorBrush();

			if(pBrush)
			{
				SelBrushList_Remove(m_CurrentDocVars.m_pSelBrushes, pBrush);
				m_pLevelMgr->RemoveBrush(&m_CurrentDocVars.m_Level, pBrush);
				(*Entities)[i].DeleteActorBrush();
			}

			strcpy(ActorDir, m_pLevelMgr->GetActorsDirectory(&m_CurrentDocVars.m_Level));

			pBrush=(*Entities)[i].CreateActorBrush(ActorFile, ActorDir, PawnIni);
			if(pBrush)
			{
				m_pLevelMgr->AppendBrush(&m_CurrentDocVars.m_Level,pBrush);
				if((*Entities)[i].IsSelected())
					SelBrushList_Add(m_CurrentDocVars.m_pSelBrushes, pBrush);
				if(!m_bShowActors)
					Brush_SetVisible(pBrush, GE_FALSE);
			}
		}
		else
		{
			Brush *pBrush = (*Entities)[i].GetActorBrush();
			if(pBrush!=NULL)
			{
				SelBrushList_Remove(m_CurrentDocVars.m_pSelBrushes, pBrush);
				m_pLevelMgr->RemoveBrush(&m_CurrentDocVars.m_Level, pBrush);
				(*Entities)[i].DeleteActorBrush();
			}
		}
	}

}
// end change
