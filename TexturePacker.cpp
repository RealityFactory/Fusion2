// TexturePacker.cpp: implementation of the CTexturePacker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fusion.h"
#include "TexturePacker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CTexturePackBmp
{
public:
	CTexturePackBmp(){};
	CTexturePackBmp(LPCTSTR NewName, geBitmap* pNewBmp)
	{
		Name = NewName; 
		pBmp = pNewBmp;
	};

	CString	Name;
	geBitmap *pBmp;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexturePacker::CTexturePacker()
{
	Textures = new CTexturePackBmpArray;
}

CTexturePacker::~CTexturePacker()
{
	DestroyBmps();

	delete Textures;
}

void CTexturePacker::DestroyBmps()
{
	for (int i = 0; i < Textures->GetSize(); i++)
	{
		CTexturePackBmp &TextureBmp = Textures->ElementAt(i);
		
		if(TextureBmp.pBmp)
			geBitmap_Destroy(&TextureBmp.pBmp);
	}

	Textures->RemoveAll();
}

geBoolean CTexturePacker::Load(const char *FileName)
{
	DestroyBmps();

	CString Msg;

	geVFile *VFS = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_VIRTUAL, FileName, NULL, GE_VFILE_OPEN_READONLY | GE_VFILE_OPEN_DIRECTORY);
	if	(!VFS)
	{
		DestroyBmps();
		return GE_FALSE;
	}

	geVFile_Finder *Finder = geVFile_CreateFinder(VFS, "*.*");
	if	(!Finder)
	{
		geVFile_Close(VFS);
		DestroyBmps();
		return GE_FALSE;
	}
	
	while	(geVFile_FinderGetNextFile(Finder) != GE_FALSE)
	{
		geVFile_Properties	Properties;

		geVFile_FinderGetProperties(Finder, &Properties);
		if	(!AddTexture(VFS, Properties.Name))
		{
			geVFile_Close(VFS);
			DestroyBmps();
			return GE_FALSE;
		}
	}

	geVFile_Close(VFS);

	return GE_TRUE;
}

geBoolean CTexturePacker::Save(const char *FileName)
{
	geVFile *pVFS = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_VIRTUAL, FileName, NULL, GE_VFILE_OPEN_CREATE | GE_VFILE_OPEN_DIRECTORY);

	if	(!pVFS)
		return GE_FALSE;

	for (int i=0; i<Textures->GetSize(); i++)
	{
		geVFile *	pTextureFile;

		CTexturePackBmp &CurrentTexture = Textures->ElementAt(i);

		pTextureFile = geVFile_Open(pVFS, CurrentTexture.Name, GE_VFILE_OPEN_CREATE);
		if	(!pTextureFile)
		{
			geVFile_Close(pVFS);
			return GE_FALSE;
		}

		if(!geBitmap_WriteToFile(CurrentTexture.pBmp, pTextureFile))
		{
			geVFile_Close(pTextureFile);
			geVFile_Close(pVFS);
			return GE_FALSE;
		}

		geVFile_Close(pTextureFile);
	}

	geVFile_Close(pVFS);

	return GE_TRUE;
}

geBoolean CTexturePacker::AddTexture(geVFile *pVFS, const char *TextureName)
{
	geBitmap_Info	PInfo;
	geBitmap_Info	SInfo;
	geBitmap *		pBitmap;
	geVFile *		pTextureFile;

	if(pVFS)
		pTextureFile = geVFile_Open(pVFS, TextureName, GE_VFILE_OPEN_READONLY);
	else
		return GE_FALSE;

	if (!pTextureFile)
	{
		return GE_FALSE;
	}

	pBitmap = geBitmap_CreateFromFile(pTextureFile);
	geVFile_Close(pTextureFile);

	if (!pBitmap)
	{
		return GE_FALSE;
	}

	geBitmap_GetInfo(pBitmap, &PInfo, &SInfo);
	
	//Support several pixelformats. JJT
	if(	PInfo.Format != GE_PIXELFORMAT_8BIT_GRAY
		&& PInfo.Format != GE_PIXELFORMAT_8BIT 
		&& PInfo.Format != GE_PIXELFORMAT_16BIT_555_RGB
		&& PInfo.Format != GE_PIXELFORMAT_16BIT_565_RGB
		&& PInfo.Format != GE_PIXELFORMAT_16BIT_4444_ARGB
		&& PInfo.Format != GE_PIXELFORMAT_24BIT_RGB
		&& PInfo.Format != GE_PIXELFORMAT_24BIT_BGR

		)
	{
		//Default
		if(!geBitmap_SetFormat(pBitmap, GE_PIXELFORMAT_16BIT_555_RGB, GE_FALSE, 0, NULL))
		{
			geBitmap_Destroy(&pBitmap);
			return GE_FALSE;
		}
	}

	CTexturePackBmp NewTexture(TextureName, pBitmap);
	Textures->Add(NewTexture);

	return GE_TRUE;
}

geBoolean CTexturePacker::AddBitmap(const char *BitmapFileName)
{
	geBitmap_Info	PInfo;
	geBitmap_Info	SInfo;
	geBitmap *pBitmap = 0;
	geVFile *pBMPFile =  0;
	char FileName[MAX_PATH];
	CString Msg;

	_splitpath(BitmapFileName, NULL, NULL, FileName, NULL);

	int DuplicateTexture = FindTexture(FileName);
	if (DuplicateTexture != -1)
	{
		AfxFormatString1 (Msg, IDS_REPLACEBMPINWAD, FileName);
		if (AfxMessageBox (Msg, MB_YESNO | MB_ICONQUESTION) != IDYES)
			return GE_FALSE;

		CTexturePackBmp& CurrentTexture = Textures->ElementAt(DuplicateTexture);
		geBitmap_Destroy(&CurrentTexture.pBmp);
		Textures->RemoveAt(DuplicateTexture);
	}

	pBMPFile = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, BitmapFileName, NULL, GE_VFILE_OPEN_READONLY);

	if	(!pBMPFile)
	{
		AfxFormatString1 (Msg, IDS_CANTOPENBMP, FileName);
		AfxMessageBox (Msg, MB_OK | MB_ICONEXCLAMATION);

		return GE_FALSE;
	}

	pBitmap = geBitmap_CreateFromFile(pBMPFile);
	geVFile_Close(pBMPFile);

	if	(!pBitmap)
	{
		AfxFormatString1 (Msg, IDS_INVALIDBITMAPFILE, FileName);
		AfxMessageBox (Msg, MB_OK | MB_ICONEXCLAMATION);

		return GE_FALSE;
	}

	geBitmap_GetInfo(pBitmap, &PInfo, &SInfo);
	
	if ( (PInfo.Width != PInfo.Height) || 
			 (
				 (PInfo.Width != 1) &&
				 (PInfo.Width != 2) &&
				 (PInfo.Width != 4) &&
				 (PInfo.Width != 8) &&
				 (PInfo.Width != 16) &&
				 (PInfo.Width != 32) &&
				 (PInfo.Width != 64) &&
				 (PInfo.Width != 128) &&
				 (PInfo.Width != 256)
			 )
		 )
	{
		CString SizeString;
		SizeString.Format("%d (width) by %d (height)", PInfo.Width, PInfo.Height);
		AfxFormatString2 (Msg, IDS_BMPSIZEPROBLEM, FileName, SizeString);
		if (AfxMessageBox (Msg, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES)
			return GE_FALSE;
	}

	//Support several pixelformats. JJT
	if(	 PInfo.Format != GE_PIXELFORMAT_8BIT_GRAY
		&& PInfo.Format != GE_PIXELFORMAT_8BIT 
		&& PInfo.Format != GE_PIXELFORMAT_16BIT_555_RGB
		&& PInfo.Format != GE_PIXELFORMAT_16BIT_565_RGB
		&& PInfo.Format != GE_PIXELFORMAT_16BIT_4444_ARGB
		&& PInfo.Format != GE_PIXELFORMAT_24BIT_RGB
		&& PInfo.Format != GE_PIXELFORMAT_24BIT_BGR
		)
	{
		//Default
		if(!geBitmap_SetFormat(pBitmap, GE_PIXELFORMAT_16BIT_555_RGB, GE_FALSE, 0, NULL))
		{
			AfxFormatString1 (Msg, IDS_FAILED16555RGB, FileName);
			AfxMessageBox (Msg, MB_OK | MB_ICONEXCLAMATION);

			geBitmap_Destroy(&pBitmap);
			return GE_FALSE;
		}
	}

	CTexturePackBmp NewTexture(FileName, pBitmap);
	Textures->Add(NewTexture);

	return GE_TRUE;
}

int CTexturePacker::FindTexture(const char *TextureName)
{
	for (int i=0; i < Textures->GetSize(); i++)
	{
		CTexturePackBmp& CurrentTexture = Textures->ElementAt(i);

		if (CurrentTexture.Name == TextureName)
		{
			return i;
		}
	}

	return -1;
}

geBoolean CTexturePacker::RemoveTexture(const char *TextureName)
{
	int TextureIndex = FindTexture(TextureName);
	
	if (TextureIndex != -1)
	{
		CTexturePackBmp& CurrentTexture = Textures->ElementAt(TextureIndex);
		geBitmap_Destroy(&CurrentTexture.pBmp);
		Textures->RemoveAt(TextureIndex);
		return GE_TRUE;
	}
	else
	{
		return GE_FALSE;
	}
}
