#include "StdAfx.h"
#include "KofRibbonCategoryScrool.h"

void CKofRibbonCategoryScrool::OverrideOnDraw(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	if (m_rect.IsRectEmpty())
	{
		return;
	}

	CMFCVisualManager::GetInstance()->OnDrawRibbonCategoryScroll(pDC, this);
}