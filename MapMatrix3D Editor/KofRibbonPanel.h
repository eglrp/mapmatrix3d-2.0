#pragma once
#include "KofMfcRibbonBaseElement.h"

class CKofRibbonPanel : public CMFCRibbonPanel
{
	friend class CKofMFCRibbonBar;
	friend class CKofMFCRibbonCategory;
	friend class CKofRibbonTab;

	
public:
	void OverideDoPaint(CDC* pDC);

	CMFCRibbonBaseElement* OverrideMouseButtonDown(CPoint point)
	{
		ASSERT_VALID(this);

		if (m_pHighlighted != NULL)
		{
			ASSERT_VALID (m_pHighlighted);

			BOOL bSetPressed = TRUE;

			if (m_pHighlighted->HasMenu())
			{
				CMFCRibbonButton* pButton = DYNAMIC_DOWNCAST (CMFCRibbonButton, m_pHighlighted);
				if (pButton != NULL)
				{
					ASSERT_VALID (pButton);

					const CRect rectCmd = pButton->GetCommandRect();
					bSetPressed = !rectCmd.IsRectEmpty() && rectCmd.PtInRect(point);
				}
			}

			CKofMfcRibbonBaseElement* pHighlighted = DYNAMIC_DOWNCAST(CKofMfcRibbonBaseElement, m_pHighlighted);
			ASSERT_VALID(pHighlighted);

			CMFCRibbonDefaultPanelButton* pButtonHighlighted = DYNAMIC_DOWNCAST(CMFCRibbonDefaultPanelButton, m_pHighlighted);

			if (bSetPressed)
			{
				if (!pButtonHighlighted)
				{
					pHighlighted->m_bIsPressed = TRUE;
					RedrawElement(pHighlighted);
				}
				else
				{
					pHighlighted->m_bIsPressed = FALSE;
				}

			}

			HWND hwndMenu = m_pParentMenuBar->GetSafeHwnd();

			if (!pButtonHighlighted)
			{
				pHighlighted->OnLButtonDown(point);
			}


			if (hwndMenu != NULL && !::IsWindow(hwndMenu))
			{
				return NULL;
			}
		}

		return m_pHighlighted;
	}
	
protected:
	COLORREF OnDrawRibbonPanel(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectPanel);

};

