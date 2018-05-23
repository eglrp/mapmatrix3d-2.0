#include "StdAfx.h"
#include "KofMFCVisualManagerBitmapCacheItem.h"


void CKofMFCVisualManagerBitmapCacheItem::OverrideDrawY(CDC* pDC, CRect rect, CSize sides, int iImageIndex, BYTE alphaSrc)
{
	CRect rectImage(CPoint(0, 0), m_Images.GetImageSize());

	ASSERT(rect.Height() == rectImage.Height());

	if (sides.cx > 0)
	{
		CRect rt(rectImage);
		if (m_bMirror)
		{
			rt.left = rectImage.right - sides.cx;
			rectImage.right = rt.left;
		}
		else
		{
			rt.right = rt.left + sides.cx;
			rectImage.left = rt.right;
		}

		CMFCToolBarImages* pImage = &m_Images;
		CKofToolBarImage* ckofToolBarImage = DYNAMIC_DOWNCAST(CKofToolBarImage, pImage);
		ASSERT_VALID(ckofToolBarImage);

		ckofToolBarImage->OverrideDrawEx(pDC, rect, iImageIndex, CMFCToolBarImages::ImageAlignHorzLeft, CMFCToolBarImages::ImageAlignVertTop, rt, alphaSrc);
	}

	if (sides.cy > 0)
	{
		CRect rt(rectImage);
		if (m_bMirror)
		{
			rt.right = rectImage.left + sides.cy;
			rectImage.left = rt.right;
		}
		else
		{
			rt.left = rectImage.right - sides.cy;
			rectImage.right = rt.left;
		}

		CMFCToolBarImages* pImage = &m_Images;
		CKofToolBarImage* ckofToolBarImage = DYNAMIC_DOWNCAST(CKofToolBarImage, pImage);
		ASSERT_VALID(ckofToolBarImage);
		ckofToolBarImage->OverrideDrawEx(pDC, rect, iImageIndex, CMFCToolBarImages::ImageAlignHorzRight, CMFCToolBarImages::ImageAlignVertTop, rt, alphaSrc);
	}

	if (rectImage.Width() > 0)
	{
		rect.DeflateRect(sides.cx, 0, sides.cy, 0);

		CMFCToolBarImages* pImage = &m_Images;
		CKofToolBarImage* ckofToolBarImage = DYNAMIC_DOWNCAST(CKofToolBarImage, pImage);
		ASSERT_VALID(ckofToolBarImage);
		ckofToolBarImage->OverrideDrawEx(pDC, rect, iImageIndex, CMFCToolBarImages::ImageAlignHorzStretch, CMFCToolBarImages::ImageAlignVertTop, rectImage, alphaSrc);
	}
}