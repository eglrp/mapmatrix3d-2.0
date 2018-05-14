#include "StdAfx.h"
#include "KofToolBarImage.h"


CKofToolBarImage::CKofToolBarImage(void)
{
}


CKofToolBarImage::~CKofToolBarImage(void)
{
}

BOOL CKofToolBarImage::OverrideDrawEx(CDC* pDC, CRect rect, int iImageIndex, ImageAlignHorz horzAlign, ImageAlignVert vertAlign, CRect rectSrc, BYTE alphaSrc)
{
	ASSERT_VALID(pDC);

	if (rectSrc.IsRectEmpty())
	{
		rectSrc = CRect(CPoint(0, 0), m_sizeImage);
	}

	if (rectSrc.IsRectEmpty())
	{
		return FALSE;
	}

	CRect rectDst(rect);

	if (horzAlign != ImageAlignHorzStretch)
	{
		BOOL bUpdate = TRUE;

		if (horzAlign == ImageAlignHorzLeft)
		{
			rectDst.right = rectDst.left + rectSrc.Width();
		}
		else if (horzAlign == ImageAlignHorzRight)
		{
			rectDst.left = rectDst.right - rectSrc.Width();
		}
		else if (horzAlign == ImageAlignHorzCenter)
		{
			rectDst.left += (rectDst.Width() - rectSrc.Width()) / 2;
			rectDst.right = rectDst.left + rectSrc.Width();
		}
		else
		{
			bUpdate = FALSE;
		}

		if (bUpdate)
		{
			CRect rt(rectDst);
			rectDst.IntersectRect(rectDst, rect);

			if (0 < rectDst.Width() && rectDst.Width() !=  rectSrc.Width())
			{
				rectSrc.left += rectDst.left - rt.left;
				rectSrc.right = rectSrc.left + min(rectDst.Width(), rectSrc.Width());
			}
		}
	}

	if (vertAlign != ImageAlignVertStretch)
	{
		BOOL bUpdate = TRUE;

		if (vertAlign == ImageAlignVertTop)
		{
			rectDst.bottom = rectDst.top + rectSrc.Height();
		}
		else if (vertAlign == ImageAlignVertBottom)
		{
			rectDst.top = rectDst.bottom - rectSrc.Height();
		}
		else if (vertAlign == ImageAlignVertCenter)
		{
			rectDst.top += (rectDst.Height() - rectSrc.Height()) / 2;
			rectDst.bottom = rectDst.top + rectSrc.Height();
		}
		else
		{
			bUpdate = FALSE;
		}

		if (bUpdate)
		{
			CRect rt(rectDst);
			rectDst.IntersectRect(rectDst, rect);

			if (0 < rectDst.Height() && rectDst.Height() !=  rectSrc.Height())
			{
				rectSrc.top += rectDst.top - rt.top;
				rectSrc.bottom = rectSrc.top + min(rectDst.Height(), rectSrc.Height());
			}
		}
	}

	if (rectSrc.IsRectEmpty() || rectDst.IsRectEmpty())
	{
		return FALSE;
	}

	HBITMAP hbmOldGlyphs = (HBITMAP)SelectObject (hDCGlyphs, m_hbmImageWell);

	const int xOffset = rectSrc.left;
	const int yOffset = rectSrc.top;

	const int nWidth = rectSrc.IsRectEmpty () ? m_sizeImage.cx : rectSrc.Width ();
	const int nHeight = rectSrc.IsRectEmpty () ? m_sizeImage.cy : rectSrc.Height ();

	BOOL bRes = FALSE;

	if (m_nBitsPerPixel == 32)
	{
		BLENDFUNCTION pixelblend = { AC_SRC_OVER, 0, alphaSrc, AC_SRC_ALPHA };

		bRes = pDC->AlphaBlend(rectDst.left, rectDst.top,
			rectDst.Width (), rectDst.Height (), 
			CDC::FromHandle (hDCGlyphs), iImageIndex * m_sizeImage.cx + xOffset, yOffset, 
			nWidth, nHeight, pixelblend);
	}
	else if (m_clrTransparent == -1)
	{
		bRes = ::StretchBlt (pDC->m_hDC, rectDst.left, rectDst.top,
			rectDst.Width (), rectDst.Height (), 
			hDCGlyphs, iImageIndex * m_sizeImage.cx + xOffset, yOffset, 
			nWidth, nHeight, SRCCOPY);
	}

	SelectObject (hDCGlyphs, hbmOldGlyphs);

	if (bRes)
	{
		return TRUE;
	}

	BOOL bcreateMonoDC = m_bCreateMonoDC;
	m_bCreateMonoDC = FALSE;

	CAfxDrawState ds;
	if (!PrepareDrawImage(ds, rectDst.Size()))
	{
		m_bCreateMonoDC = bcreateMonoDC;
		return FALSE;
	}

	m_rectSubImage = rectSrc;

	//设置面板的明暗度0-255
	alphaSrc = 150;

	bRes = Draw(pDC, rectDst.left, rectDst.top, iImageIndex, FALSE, FALSE, FALSE, FALSE, FALSE, alphaSrc);

	m_rectSubImage.SetRectEmpty();

	EndDrawImage(ds);
	m_bCreateMonoDC = bcreateMonoDC;

	DeleteObject(hDCGlyphs);
	DeleteObject(hbmOldGlyphs);
	return bRes;
}
