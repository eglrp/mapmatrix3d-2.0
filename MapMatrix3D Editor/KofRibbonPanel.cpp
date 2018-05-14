#include "StdAfx.h"
#include "KofRibbonPanel.h"
#include "KofRibbonButton.h"

void CKofRibbonPanel::OverideDoPaint(CDC* pDC)
{
	ASSERT_VALID(pDC);
	if (m_rect.IsRectEmpty())
	{
		return;
	}

	CRect rectClip;
	pDC->GetClipBox(rectClip);

	CRect rectInter;

	if (!rectInter.IntersectRect(m_rect, rectClip))
	{
		return;
	}

	COLORREF clrTextOld = pDC->GetTextColor();

	COLORREF clrText = m_pParent == NULL || m_pPaletteButton != NULL ? afxGlobalData.clrBarText : OnDrawRibbonPanel(pDC, this, m_rect)/* CMFCVisualManager::GetInstance()->OnDrawRibbonPanel(pDC, this, m_rect, m_rectCaption)*/;

	// Draw panel caption:
	if (!m_rectCaption.IsRectEmpty() && rectInter.IntersectRect(m_rectCaption, rectClip))
	{
		//CMFCVisualManager::GetInstance()->OnDrawRibbonPanelCaption(pDC, this, m_rectCaption);
	}

#ifdef ENABLE_RIBBON_LAUNCH_BUTTON
	// Draw launch button:
	if (rectInter.IntersectRect(m_btnLaunch.GetRect(), rectClip))
	{
		m_btnLaunch.OnDraw(pDC);
	}
#endif // ENABLE_RIBBON_LAUNCH_BUTTON

	//字体颜色
	clrText = RGB(89, 64, 42);

	pDC->SetTextColor(clrText);

	if (!m_btnDefault.GetRect().IsRectEmpty())
	{
		// Panel is collapsed, draw default button only:
		if (rectInter.IntersectRect(m_btnDefault.GetRect(), rectClip))
		{
			m_btnDefault.OnDraw(pDC);
		}
	}
	else if (m_pPaletteButton != NULL)
	{
		OnDrawPaletteMenu(pDC);
	}
	else
	{
		if (m_bIsDefaultMenuLook && m_pParentMenuBar != NULL)
		{
			ASSERT_VALID(m_pParentMenuBar);

			BOOL bDisableSideBarInXPMode = m_pParentMenuBar->m_bDisableSideBarInXPMode;
			m_pParentMenuBar->m_bDisableSideBarInXPMode = FALSE;

			CMFCVisualManager::GetInstance()->OnFillBarBackground(pDC, m_pParentMenuBar, m_rect, m_rect);

			m_pParentMenuBar->m_bDisableSideBarInXPMode = bDisableSideBarInXPMode;
		}

		// Draw panel elements:
		for (int i = 0; i < m_arElements.GetSize(); i++)
		{
			CMFCRibbonBaseElement* pElem = m_arElements [i];
			ASSERT_VALID(pElem);

			if (rectInter.IntersectRect(pElem->GetRect(), rectClip))
			{
				pDC->SetTextColor(clrText);

				CKofRibbonButton* pRibbonElem = DYNAMIC_DOWNCAST(CKofRibbonButton, pElem);		//只对按钮绘制

				if (pRibbonElem != NULL)
				{
					BOOL bIsHighlighted = pRibbonElem->m_bIsHighlighted;

					if (IsMenuMode() && pRibbonElem->IsDroppedDown() && m_pHighlighted == NULL)
					{
						pRibbonElem->m_bIsHighlighted = TRUE;
					}

					pRibbonElem->OverrideOnDraw(pDC);

					pRibbonElem->m_bIsHighlighted = bIsHighlighted;
				}	
			}
		}
	}

	pDC->SetTextColor(clrTextOld);
}



COLORREF CKofRibbonPanel::OnDrawRibbonPanel(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectPanel)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pPanel);

	COLORREF clrText = afxGlobalData.clrBarText;

	if (pPanel->IsCollapsed() && pPanel->GetDefaultButton().IsFocused())
	{
		pDC->FillRect(rectPanel, &afxGlobalData.brHilite);
		clrText = afxGlobalData.clrTextHilite;
	}
	else if (pPanel->IsHighlighted())			//不绘制闪光
	{
		/*CDrawingManager dm(*pDC);
		dm.HighlightRect(rectPanel);*/
	}

	//pDC->Draw3dRect(rectPanel, RGB(0, 0, 255), RGB(0, 0, 255));
	//pDC->Draw3dRect(rectPanel, afxGlobalData.clrBarHilite, afxGlobalData.clrBarHilite);
	rectPanel.OffsetRect(-1, -1);
	//pDC->Draw3dRect(rectPanel, afxGlobalData.clrBarShadow, afxGlobalData.clrBarShadow);
	return clrText;
}