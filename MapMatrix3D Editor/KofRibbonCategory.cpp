// KofRibbonCategory.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "KofRibbonCategory.h"
#include "KofRibbonPanel.h"
#include "KofRibbonCategoryScrool.h"
#include "KofVisualManagerOffice2007.h"

// CKofRibbonCategory


void CKofRibbonCategory::OverrideOnDraw(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	if (m_rect.IsRectEmpty())
	{
		return;
	}

	CMFCVisualManager* pManager = CKofVisualManagerOffice2007::GetInstance();
	CKofVisualManagerOffice2007* pVisManager = DYNAMIC_DOWNCAST(CKofVisualManagerOffice2007, pManager);
	pVisManager->OverrideOnDrawRibbonCategory(pDC, this, m_rect);

	/*CMFCVisualManager::GetInstance()->OnDrawRibbonCategory(pDC, this, m_rect);*/

	BOOL bClip = FALSE;

	CRgn rgnClip;

	if (!m_ScrollLeft.GetRect().IsRectEmpty() ||
		!m_ScrollRight.GetRect().IsRectEmpty())
	{
		CRect rectClient = m_rect;
		rectClient.DeflateRect(nPanelMarginLeft, nPanelMarginTop, nPanelMarginRight, nPanelMarginBottom);

		rgnClip.CreateRectRgnIndirect(rectClient);
		pDC->SelectClipRgn(&rgnClip);

		bClip = TRUE;
	}

	for (int i = 0; i < m_arPanels.GetSize(); i++)
	{
		CKofRibbonPanel* prPanel = DYNAMIC_DOWNCAST(CKofRibbonPanel, m_arPanels[i]);
		ASSERT_VALID(prPanel);

		prPanel->OverideDoPaint(pDC);
	}

	if (bClip)
	{
		pDC->SelectClipRgn(NULL);
	}

	CRibbonCategoryScroll* pLeft = &m_ScrollLeft;
	CRibbonCategoryScroll* pRight = &m_ScrollRight;
	CKofRibbonCategoryScrool* pScrollLeft = DYNAMIC_DOWNCAST(CKofRibbonCategoryScrool, pLeft);
	CKofRibbonCategoryScrool* pScrollRight = DYNAMIC_DOWNCAST(CKofRibbonCategoryScrool, pRight);
	pScrollLeft->OverrideOnDraw(pDC);
	pScrollRight->OverrideOnDraw(pDC);
}


