#pragma once
#include "KofToolBarImage.h"
#include "afxvisualmanageroffice2007.h"



class CKofMFCVisualManagerBitmapCacheItem : public CMFCVisualManagerBitmapCache::CMFCVisualManagerBitmapCacheItem
{
public:
	void OverrideDrawY(CDC* pDC, CRect rect, CSize sides, int iImageIndex = 0, BYTE alphaSrc = 255);
	
};

