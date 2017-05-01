#ifndef _MEMDC_H_
#define _MEMDC_H_

// Definition of CBufferMemDC. It is an object which does the management
// of a buffer for CMemDC further down. This buffer can live much longer 
// than a CMemDC.

class CBufferMemDC : public CObject
{
public:

	CBufferMemDC();		// constructor

	~CBufferMemDC();	// destructor

	void	Reset();	// empty the buffer

						//	returns TRUE if the buffer has not been initialized 
	BOOL	IsDirty();	//	and if it should be filled by drawing operations

						//	set dirty flag of the buffer, all subsequent IsDirty()
	void	Dirty();	//	will return TRUE after this call

						//	tell the buffer everything is drawn, all subsequent 
						//	IsDirty() will return FALSE after this call. 
	void	EndModify();//	The buffer can be used now

// functions used by CMemDC:

	BOOL	SetSizeBuffer( CDC *pDC, CRect rect );
	// does allocate a suitable bitmap with the size of rect. Is for example used by CMemDC for the case where
	// the size of the window has been changed.
	//
	// Note: If a buffer of 1024*768 pixel is defined, this consumes with a 32 bit video card 3 byte per pixel =
	// 2.4 MByte memory of the main memory of the computer. There are still computers (laptops) which have
	// only 8 MByte memory.
	//
	// If the bitmap could not be created, for example when there is too little memory or when the bitmap
	// is too large, the user is notified about it and FALSE is returned. In this case
	// the object does never try again to create a bitmap after this event, because we assume that we have
	// limited system resources. CMemDC can bypass the buffers in this case.

	CRect   GetSizeBuffer() const;		// get the size of the bitmap
	CBitmap *GetBitmap();				// get the bitmap assoziated with the buffer
	void    OnScroll( int dx, int dy);	// does do the same as CWnd::ScrollWindow()

private:

	CBitmap *m_pBitmap;		//	offscreen bitmap
	CRect	m_SizeBitmap;	//	the size of the bitmap
	BOOL	m_IsDirty;		//	TRUE if the buffer is empty

							//	TRUE when the creation of a bitmap has failed.
	BOOL    m_LowMemory;	//	A flag for low system resources
};

//--------------------------------------------------------------
// Definition of CMemDC. This object is a CDC which does send all drawing
// commands to the buffer the user supplies in the constructor of the object. The
// object is capable to copy the contents of the buffer to the screen and to copy
// to and from other buffers. During printing all buffer operations are bypassed
// to prevent too big bitmaps and other problems with special printer contexts.
//
// See also the important notes at the member function GetSafeCDC()

class CMemDC : public CDC
{
    DECLARE_DYNAMIC( CMemDC );

public:

	// constructor. Creates a CDC which is based on pDC and which uses pBufferMemDC as buffer.
	// 'rect' is the total area which should be buffered in pBufferMemDC. clipRect is the area
	// which should be redrawn in the buffer
	CMemDC(CDC* pDC, CRect rect, CRect clipRect, CBufferMemDC* pBufferMemDC);

	// destructor. This does not automatically make a copy to the screen because the user has
	// to decide when to do this. See CopyToScreen for this
	~CMemDC();

	// copy the current contents of the buffer to the screen if more than 'interval'
	// milliseconds have been passed by since the last CopyToScreen. Does return
	// if a copy has be done
	BOOL			CopyToScreen( DWORD interval );

	// copy the contents of the clip area (see clipRect of the constructor) from another buffer to this buffer
	void			CopyFrom(CBufferMemDC* pBufferMemDC);

	// copy the contents of clip area (see clipRect of the constructor) from this to another buffer
	void			CopyTo(CBufferMemDC* pBufferMemDC);

	CDC*			GetSafeCDC();
	// an important helper function which does eliminate problems if we are previewing 
	// or printing.
	//
	// if we are printing or previewing the original pDC from the constructor is
	// returned. If we are drawing on the screen <this> is returned which does use
	// the buffer.
	//
	// It is not possible to use always CMemDC as replacement for pDC which is supplied in the
	// constructor. The reason is that sometimes pDC may be a class which is derived from CDC.
	// For example CPreviewDC from MFC. If these classes have overloaded functions which are
	// not virtual the wrong function will be called if we have only a pointer to CDC in the
	// constructor. If one of these functions will be called, it will not call the appropriate
	// function in CPreviewDC, but the same function of CDC.
	//
	// The CMemDC of Codeguru have this problem.

// it would be possible to simplify the operations on CMemDC with the following operators which
// would return the value of GetSaveCDC:
//
//	CDC* operator->();
//	operator		CDC*();
//
// we don't do this because everybody thinks that a pointer of the object returns the object 
// itself. The code is easier to read if GetSafeCDC() is written.

private:

						//	a pointer to the bitmap buffer which is used for all operations
	CBufferMemDC	*m_pBufferMemDC;
	CBitmap			*m_oldBitmap;	// bitmap originaly found in pBufferMemDC
	CDC				*m_pDC;			// The CDC passed in the constructor
	CRect			m_ClipRect;		// The clipping rect we are using
	BOOL			m_bMemDC;		// TRUE if the CDC is a memory DC
	DWORD			m_LastCopyToScreen;	// GetTickCount() of last CopyToScreen
};

#endif
