#pragma once
#include "afxtoolbarimages.h"
#include "qcomm.h"

static HDC hDCGlyphs = NULL;
static HDC hDCMono = NULL;
#define AFX_ROP_DSPDxax  0x00E20746L
#define AFX_ROP_PSDPxax  0x00B8074AL

// Internal images:
#define AFX_IMAGE_LIGHT  0
#define AFX_IMAGE_SHADOW 1


class CKofToolBarImage : public CMFCToolBarImages
{
public:
	CKofToolBarImage(void);
	~CKofToolBarImage(void);

	BOOL OverrideDrawEx(CDC* pDC, CRect rect, int iImageIndex, ImageAlignHorz horzAlign = ImageAlignHorzLeft, ImageAlignVert vertAlign = ImageAlignVertTop, CRect rectSrc = CRect(0, 0, 0, 0), BYTE alphaSrc = 255);

};




