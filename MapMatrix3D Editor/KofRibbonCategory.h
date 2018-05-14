#pragma once
#include "KofRibbonPanel.h"

// CKofRibbonCategory ÃüÁîÄ¿±ê
static const int nPanelMarginLeft = 2;
static const int nPanelMarginRight = 2;
static const int nPanelMarginTop = 3;
static const int nPanelMarginBottom = 4;

class CKofRibbonCategory : public CMFCRibbonCategory
{
	friend class CKofMFCRibbonBar;
	friend class CKofRibbonTab;
public:
	void OverrideOnDraw(CDC* pDC);

	CMFCRibbonBaseElement* OverrideOnLButtonDown(CPoint point)
	{
		CMFCRibbonBaseElement* pBtnScroll = HitTestScrollButtons(point);
		if (pBtnScroll != NULL)
		{
			ASSERT_VALID(pBtnScroll);
			pBtnScroll->OnAutoRepeat();

			if (HitTestScrollButtons(point) == pBtnScroll)
			{
				return pBtnScroll;
			}
			else
			{
				return NULL;
			}
		}

		CMFCRibbonPanel* pPanel = GetPanelFromPoint(point);
		if (pPanel == NULL)
		{
			return NULL;
		}

		m_bMouseIsPressed = TRUE;

		ASSERT_VALID(pPanel);
		CKofRibbonPanel* pKofPanel = DYNAMIC_DOWNCAST(CKofRibbonPanel, pPanel);
		ASSERT_VALID(pKofPanel);
		return pKofPanel->OverrideMouseButtonDown(point);
	}
};


