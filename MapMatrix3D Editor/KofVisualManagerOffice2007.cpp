#include "StdAfx.h"
#include "KofMFCControlRenderer.h"
#include "KofVisualManagerOffice2007.h"
#include "KofMFCVisualManagerBitmapCacheItem.h"

void CKofVisualManagerOffice2007::OverrideOnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea)
{
	CRuntimeClass* pBarClass = pBar->GetRuntimeClass();

	if (!CanDrawImage() || pBar->IsDialogControl() || pBarClass->IsDerivedFrom(RUNTIME_CLASS(CMFCColorBar)))
	{
		CMFCVisualManagerOffice2003::OnFillBarBackground(pDC, pBar, rectClient, rectClip, bNCArea);
		return;
	}

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)))
	{
		BOOL bIsHorz = (pBar->GetPaneStyle() & CBRS_ORIENT_HORZ);
		COLORREF clr1 = bIsHorz ? m_clrMenuBarGradientDark : m_clrMenuBarGradientVertLight;
		COLORREF clr2 = bIsHorz ? m_clrMenuBarGradientLight : m_clrMenuBarGradientVertDark;

		CDrawingManager dm(*pDC);
		dm.Fill4ColorsGradient(rectClient, clr1, clr2, clr2, clr1, !bIsHorz);
		return;
	}
	else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
	{
		pDC->FillRect(rectClip, &m_brMenuLight);

		CMFCPopupMenuBar* pMenuBar = DYNAMIC_DOWNCAST(CMFCPopupMenuBar, pBar);
		if (!pMenuBar->m_bDisableSideBarInXPMode)
		{
			CRect rectImages = rectClient;
			rectImages.right = rectImages.left + pMenuBar->GetGutterWidth();
			rectImages.DeflateRect (0, 1);

			pDC->FillRect(rectImages, &m_brBarBkgnd);

			rectImages.left = rectImages.right;
			rectImages.right += 2;
			DrawSeparator(pDC, rectImages, FALSE);
		}

		return;
	}
	else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCStatusBar)))
	{
		CSize szSysBorder(GetSystemBorders(TRUE));

		CRect rect(rectClient);
		CRect rectExt(0, 0, 0, 0);
		BOOL bExtended = ((CMFCStatusBar*)pBar)->GetExtendedArea(rectExt);

		if (bExtended)
		{
			rect.right = rectExt.left;
		}

		CWnd* pWnd = ((CMFCStatusBar*)pBar)->GetParent();
		ASSERT_VALID(pWnd);

		BOOL bActive = IsWindowActive(pWnd);

		rect.InflateRect(szSysBorder.cx, 0, szSysBorder.cx, szSysBorder.cy);
		m_ctrlStatusBarBack.Draw(pDC, rect, bActive ? 0 : 1);

		if (bExtended)
		{
			rectExt.InflateRect(0, 0, szSysBorder.cx, szSysBorder.cy);
			rectExt.left -= m_ctrlStatusBarBack_Ext.GetParams().m_rectCorners.left;
			m_ctrlStatusBarBack_Ext.Draw(pDC, rectExt, bActive ? 0 : 1);
		}

		return;
	}
	else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonStatusBar)))
	{
		CMFCRibbonStatusBar* pRibbonStatusBar = DYNAMIC_DOWNCAST(CMFCRibbonStatusBar, pBar);

		CSize szSysBorder(GetSystemBorders(TRUE));

		CRect rect(rectClient);
		CRect rectExt(0, 0, 0, 0);

		BOOL bExtended    = pRibbonStatusBar->GetExtendedArea(rectExt);
		BOOL bBottomFrame = pRibbonStatusBar->IsBottomFrame();

		//去掉原来的进度条底色
		/*if (bExtended)
		{
			rect.right = rectExt.left;
		}
		*/
		CWnd* pWnd = pBar->GetParent();
		ASSERT_VALID(pWnd);

		BOOL bActive = IsWindowActive(pWnd);

		rect.InflateRect(szSysBorder.cx, 0, szSysBorder.cx, bBottomFrame ? -1 : szSysBorder.cy);
		m_ctrlStatusBarBack.Draw(pDC, rect, bActive ? 0 : 1);

		if (bExtended)
		{
			rectExt.InflateRect(0, 0, szSysBorder.cx, bBottomFrame ? -1 : szSysBorder.cy);
			rectExt.left -= m_ctrlStatusBarBack_Ext.GetParams().m_rectCorners.left;
			//m_ctrlStatusBarBack_Ext.Draw(pDC, rectExt, bActive ? 0 : 1);
		}

		return;
	}
	else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCOutlookBarToolBar)))
	{
		if (m_ctrlOutlookWndBar.IsValid())
		{
			m_ctrlOutlookWndBar.Draw(pDC, rectClient);
		}
		else
		{
			CDrawingManager dm(*pDC);
			dm.FillGradient(rectClient, m_clrToolBarGradientDark, m_clrToolBarGradientLight, TRUE);
		}

		return;
	}

	CMFCVisualManagerOffice2003::OnFillBarBackground(pDC, pBar, rectClient, rectClip, bNCArea);
}


COLORREF CKofVisualManagerOffice2007::OverrideOnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::OnDrawRibbonCategoryTab(pDC, pTab, bIsActive);
	}

	ASSERT_VALID(pDC);
	ASSERT_VALID(pTab);

	CMFCRibbonCategory* pCategory = pTab->GetParentCategory();
	ASSERT_VALID(pCategory);
	CMFCRibbonBar* pBar = pCategory->GetParentRibbonBar();
	ASSERT_VALID(pBar);

	bIsActive = bIsActive && ((pBar->GetHideFlags() & AFX_RIBBONBAR_HIDE_ELEMENTS) == 0 || pTab->GetDroppedDown() != NULL);

	const BOOL bPressed     = pTab->IsPressed();
	const BOOL bIsFocused	= pTab->IsFocused() && (pBar->GetHideFlags() & AFX_RIBBONBAR_HIDE_ELEMENTS);
	const BOOL bIsHighlight = (pTab->IsHighlighted() || bIsFocused) && !pTab->IsDroppedDown();

	CRect rectTab(pTab->GetRect());
	rectTab.bottom++;

	int ratio = 0;
	if (!IsBeta() && m_ctrlRibbonCategoryTabSep.IsValid())
	{
		ratio = pBar->GetTabTruncateRatio();
	}

	if (ratio > 0)
	{
		rectTab.left++;
	}

	int nImage = bIsActive ? 3 : 0;

	if (bPressed)
	{
		if (bIsHighlight)
		{
			nImage = bIsActive ? 2 : 1;
		}
	}

	if (bIsHighlight)
	{
		nImage += 1;
	}

	CMFCControlRenderer* pRenderer = &m_ctrlRibbonCategoryTab;
	COLORREF clrText = m_clrRibbonCategoryText;
	COLORREF clrTextHighlighted = m_clrRibbonCategoryTextHighlighted;

	if (pCategory->GetTabColor() != AFX_CategoryColor_None || pTab->IsSelected())
	{
		CMFCRibbonContextCategory& context = m_ctrlRibbonContextCategory[(pTab->IsSelected() || nImage == 4) ? AFX_CategoryColor_Orange - 1 : pCategory->GetTabColor() - 1];
		pRenderer = &context.m_ctrlTab;
		clrText  = context.m_clrText;
		clrTextHighlighted = context.m_clrTextHighlighted;
	}

	CKofMFCControlRenderer* pKofRender = DYNAMIC_DOWNCAST(CKofMFCControlRenderer, pRenderer);
	pKofRender->OverrideDraw(pDC, rectTab, nImage);

	if (ratio > 0)
	{
		CRect rectSep(rectTab);
		rectSep.left = rectSep.right;
		rectSep.right++;
		rectSep.bottom--;

		m_ctrlRibbonCategoryTabSep.Draw(pDC, rectSep, 0, (BYTE)min(ratio * 255 / 100, 255));
	}

	return bIsActive ? clrTextHighlighted : clrText;
}

void CKofVisualManagerOffice2007::OverrideOnDrawRibbonCategory(CDC* pDC, CMFCRibbonCategory* pCategory, CRect rectCategory)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawRibbonCategory(pDC, pCategory, rectCategory);
		return;
	}

	CMFCControlRenderer* pRenderer = &m_ctrlRibbonCategoryBack;
	CMFCVisualManagerBitmapCache* pCache = &m_cacheRibbonCategoryBack;

	CMFCRibbonBaseElement* pParentButton = pCategory->GetParentButton();

	if (pCategory->GetTabColor() != AFX_CategoryColor_None && (pParentButton == NULL || !pParentButton->IsQATMode()))
	{
		CMFCRibbonContextCategory& context = m_ctrlRibbonContextCategory[pCategory->GetTabColor() - 1];

		pRenderer = &context.m_ctrlBack;
		pCache    = &context.m_cacheBack;
	}

	const CMFCControlRendererInfo& params = pRenderer->GetParams();

	CMFCRibbonPanelMenuBar* pMenuBar = pCategory->GetParentMenuBar();
	if (pMenuBar != NULL)
	{
		if (pMenuBar->GetCategory() != NULL)
		{
			if (rectCategory.left < 0 || rectCategory.top < 0)
			{
				CDrawingManager dm(*pDC);
				dm.FillGradient(rectCategory, m_clrBarGradientDark, m_clrBarGradientLight, TRUE);

				return;
			}
		}
		else if (pMenuBar->GetPanel() != NULL)
		{
			if (IsBeta())
			{
				pRenderer->FillInterior(pDC, rectCategory);
				return;
			}
		}
	}

	int nCacheIndex = -1;
	if (pCache != NULL)
	{
		CSize size(params.m_rectImage.Width(), rectCategory.Height());
		nCacheIndex = pCache->FindIndex(size);
		if (nCacheIndex == -1)
		{
			nCacheIndex = pCache->CacheY(size.cy, *pRenderer);
		}
	}

	if (nCacheIndex != -1)
	{
		CMFCVisualManagerBitmapCache::CMFCVisualManagerBitmapCacheItem* pCacheItem = pCache->Get(nCacheIndex);
		CKofMFCVisualManagerBitmapCacheItem* pKofCacheItem = (CKofMFCVisualManagerBitmapCacheItem *) pCacheItem;

		if (pKofCacheItem != NULL)
		{
			pKofCacheItem->OverrideDrawY(pDC, rectCategory, CSize(params.m_rectInter.left, params.m_rectImage.right - params.m_rectInter.right));
		}
	}
	else
	{
		pRenderer->Draw(pDC, rectCategory);
	}
}
