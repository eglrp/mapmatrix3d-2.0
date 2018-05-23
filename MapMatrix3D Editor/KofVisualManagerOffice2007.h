#pragma once
#include "afxvisualmanageroffice2007.h"


class CKofVisualManagerOffice2007 : public CMFCVisualManagerOffice2007
{
public:
	static CMFCVisualManager* __stdcall GetInstance()
	{
		if (m_pVisManager != NULL)
		{
			ASSERT_VALID(m_pVisManager);
			return  m_pVisManager;
		}

		if (m_pRTIDefault == NULL)
		{
			m_pVisManager = new CMFCVisualManager;
		}
		else
		{
			m_pVisManager = (CMFCVisualManager*) m_pRTIDefault->CreateObject();
			ASSERT_VALID(m_pVisManager);
		}

		CKofVisualManagerOffice2007* pVisManager = DYNAMIC_DOWNCAST(CKofVisualManagerOffice2007, m_pVisManager);

		//pVisManager->m_bAutoDestroy = TRUE;
		m_pVisManager->OnUpdateSystemColors();

		return m_pVisManager;
	}

	void OverrideOnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);

	COLORREF OverrideOnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive);

	void OverrideOnDrawRibbonCategory(CDC* pDC, CMFCRibbonCategory* pCategory, CRect rectCategory);
};

