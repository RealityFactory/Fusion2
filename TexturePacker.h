// TexturePacker.h: interface for the CTexturePacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREPACKER_H__06916000_A64E_11D4_A527_006008BFCEBD__INCLUDED_)
#define AFX_TEXTUREPACKER_H__06916000_A64E_11D4_A527_006008BFCEBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTexturePackBmp;

typedef CArray<CTexturePackBmp, CTexturePackBmp&> CTexturePackBmpArray;

class CTexturePacker  
{
public:
	CTexturePacker();
	virtual ~CTexturePacker();

	geBoolean Save( const char *FileName );
	geBoolean Load( const char *FileName );
	geBoolean AddBitmap( const char *BitmapFileName );
	geBoolean RemoveTexture( const char *TextureName );

private:
	CTexturePackBmpArray *Textures;
	void DestroyBmps();
	geBoolean AddTexture(geVFile *VFS, const char *TextureName);
	int FindTexture( const char *TextureName );

};

#endif // !defined(AFX_TEXTUREPACKER_H__06916000_A64E_11D4_A527_006008BFCEBD__INCLUDED_)
