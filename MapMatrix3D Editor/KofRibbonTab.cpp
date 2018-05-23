#include "StdAfx.h"
#include "KofRibbonTab.h"
#include "KofRibbonCategory.h"
#include "KofVisualManagerOffice2007.h"
#include "KofRibbonPanel.h"
#include "KofRibbonButton.h"
#include "qcomm.h"

void CKofRibbonTab::OverrideOnDraw(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT_VALID(m_pParent);
	ASSERT_VALID(m_pParent->GetParentRibbonBar());

	if (m_rect.IsRectEmpty())
	{
		return;
	}

	CMFCVisualManager* pManager = CKofVisualManagerOffice2007::GetInstance();
	CKofVisualManagerOffice2007* pVisManager = DYNAMIC_DOWNCAST(CKofVisualManagerOffice2007, pManager);
	COLORREF clrText = pVisManager->OverrideOnDrawRibbonCategoryTab(pDC, this, m_pParent->IsActive() || GetDroppedDown() != NULL);

	COLORREF clrTextOld = pDC->SetTextColor(clrText);

	CRect rectTab = m_rect;
	CRect rectTabText = m_rect;

	CKofRibbonCategory* pParent = DYNAMIC_DOWNCAST(CKofRibbonCategory, m_pParent);
	ASSERT_VALID(pParent);

	CMFCRibbonPanel* pPanel = m_pParent->GetPanel(0);
	ASSERT_VALID(pPanel);
	CKofRibbonPanel* mPanel = DYNAMIC_DOWNCAST(CKofRibbonPanel, pPanel);
	ASSERT_VALID(mPanel);
	CRect mPanelRect = mPanel->m_rect;
	int center = (mPanelRect.top + mPanelRect.bottom) / 2;

	if ((pParent->m_strName == _T("开始") || pParent->m_strName == _T("Start")) && pParent->IsActive())
	{
		CRect leftRect1;
		CRect rightRect1;
		CRect leftRect2;
		CRect rightRect2;

		for (int i = 0; i < mPanel->GetCount(); i++)
		{
			CMFCRibbonBaseElement* pBaseElement = mPanel->GetElement(i);

			CKofRibbonButton* pRibbonButton = DYNAMIC_DOWNCAST(CKofRibbonButton, pBaseElement);

			if (pRibbonButton)
			{
				CString strName = pRibbonButton->GetText();

				if (strName == "生成索引" || strName ==	"create_Link")
				{
					leftRect1 = pRibbonButton->m_rect;
				}

				if (strName ==	"正视" || strName == "Front_View")
				{
					rightRect1 = pRibbonButton->m_rect;
				}

				if (strName == "仰视" || strName == "Look_Up")
				{
					leftRect2 = pRibbonButton->m_rect;
				}

				if (strName == "网格化" || strName == "Wire")
				{
					rightRect2 = pRibbonButton->m_rect;
				}

			}
		}

		int seperatorPos1 = (leftRect1.right + rightRect1.left) / 2;
		int seperatorPos2 = (leftRect2.right + rightRect2.left) / 2;

		CPen pen(PS_SOLID, 0.4, RGB(76, 79, 85));//设置画笔的样式颜色
		CPen *pOldPen = pDC->SelectObject(&pen);
		pDC->MoveTo(seperatorPos1, center - 30);
		pDC->LineTo(seperatorPos1, center + 30);

		pDC->MoveTo(seperatorPos2, center - 30);
		pDC->LineTo(seperatorPos2, center + 30);

		pDC->SelectObject(pOldPen);

		CPen pen1(PS_SOLID, 0.4, RGB(191, 195, 206));//设置画笔的样式颜色
		pOldPen = pDC->SelectObject(&pen1);
		pDC->MoveTo(seperatorPos1 + 1, center - 30);
		pDC->LineTo(seperatorPos1 + 1, center + 30);

		pDC->MoveTo(seperatorPos2 + 1, center - 30);
		pDC->LineTo(seperatorPos2 + 1, center + 30);

		pDC->SelectObject(pOldPen);

		DeleteObject(pen);
		DeleteObject(pOldPen);

	}
	else if ((pParent->m_strName == _T("定向拼接") || pParent->m_strName == _T("Orient Stitch")) && pParent->IsActive())
	{
		CRect leftRect1;
		CRect rightRect1;

		for (int i = 0; i < mPanel->GetCount(); i++)
		{
			CMFCRibbonBaseElement* pBaseElement = mPanel->GetElement(i);

			CKofRibbonButton* pRibbonButton = DYNAMIC_DOWNCAST(CKofRibbonButton, pBaseElement);

			if (pRibbonButton)
			{
				CString strName = pRibbonButton->GetText();

				if (strName == "颠倒" || strName == "Upside_Down")
				{
					leftRect1 = pRibbonButton->m_rect;
				}

				if (strName == "绝对定向" || strName == "Absolute_Orient")
				{
					rightRect1 = pRibbonButton->m_rect;
				}
			}
		}

		int seperatorPos1 = (leftRect1.right + rightRect1.left) / 2;

		CPen pen(PS_SOLID, 0.4, RGB(76, 79, 85));//设置画笔的样式颜色
		CPen *pOldPen = pDC->SelectObject(&pen);
		pDC->MoveTo(seperatorPos1, center - 30);
		pDC->LineTo(seperatorPos1, center + 30);
		pDC->SelectObject(pOldPen);

		CPen pen1(PS_SOLID, 0.4, RGB(191, 195, 206));//设置画笔的样式颜色
		pOldPen = pDC->SelectObject(&pen1);
		pDC->MoveTo(seperatorPos1 + 1, center - 30);
		pDC->LineTo(seperatorPos1 + 1, center + 30);

		pDC->SelectObject(pOldPen);

		DeleteObject(pen);
		DeleteObject(pOldPen);
	}

	pDC->DrawText(pParent->m_strName, rectTabText, DT_CALCRECT | DT_SINGLELINE | DT_VCENTER);

	const int cxTabText = rectTabText.Width();
	const int cxTabTextMargin = max(4, (rectTab.Width() - cxTabText) / 2);

	rectTab.DeflateRect(cxTabTextMargin, 0);
	rectTab.top += nPanelMarginTop;

	pDC->DrawText(pParent->m_strName, rectTab, DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(clrTextOld);
}

