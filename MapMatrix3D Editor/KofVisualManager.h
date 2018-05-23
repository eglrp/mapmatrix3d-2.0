#pragma once
#include "afxvisualmanager.h"
#include "KofRibbonCategory.h"
#include "KofMfcOutlookBarPane.h"

class CKofVisualManager : public CMFCVisualManager
{
public:
	static CKofVisualManager* __stdcall GetInstance()
	{
		if (m_pVisManager != NULL)
		{
			ASSERT_VALID(m_pVisManager);
			return (CKofVisualManager*) m_pVisManager;
		}

		if (m_pRTIDefault == NULL)
		{
			m_pVisManager = new CKofVisualManager;
		}
		else
		{
			m_pVisManager = (CKofVisualManager*) m_pRTIDefault->CreateObject();
			ASSERT_VALID(m_pVisManager);
		}

		//CKofVisualManager* pVisManager = DYNAMIC_DOWNCAST(CKofVisualManager, m_pVisManager);
		//pVisManager->m_bAutoDestroy = TRUE;
		m_pVisManager->OnUpdateSystemColors();

		return (CKofVisualManager*)m_pVisManager;
	}

	void OverrideOnDrawRibbonCaption(CDC* pDC, CMFCRibbonBar* pBar, CRect rect, CRect rectText)
	{
		ASSERT_VALID(pBar);

		CWnd* pWnd = pBar->GetParent();
		ASSERT_VALID(pWnd);

		const BOOL bGlass = pBar->IsTransparentCaption();
		const DWORD dwStyleEx  = pWnd->GetExStyle();
		const BOOL bIsRTL = (dwStyleEx & WS_EX_LAYOUTRTL) == WS_EX_LAYOUTRTL;
		BOOL bTextCenter = TRUE;

		ASSERT_VALID(pDC);

		if ((pBar->GetHideFlags() & AFX_RIBBONBAR_HIDE_ALL) == AFX_RIBBONBAR_HIDE_ALL)
		{
			HICON hIcon = afxGlobalUtils.GetWndIcon(pWnd);

			if (hIcon != NULL)
			{
				CSize szIcon(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
				CRect rectIcon(rect.TopLeft(), CSize(min(::GetSystemMetrics(SM_CYCAPTION), rect.Height()), rect.Height()));

				long x = rect.left + max(0, (rectIcon.Width()  - szIcon.cx) / 2);
				long y = rect.top  + max(0, (rectIcon.Height() - szIcon.cy) / 2);

				::DrawIconEx(pDC->GetSafeHdc(), x, y, hIcon, szIcon.cx, szIcon.cy, 0, NULL, DI_NORMAL);

				if (rectText.left < rectIcon.right)
				{
					rectText.left = rectIcon.right;
				}
			}

			bTextCenter = TRUE;
		}

		CFont* pOldFont = pDC->SelectObject(&afxGlobalData.fontBold);
		ENSURE(pOldFont != NULL);

		int nOldMode = pDC->SetBkMode(TRANSPARENT);

		CString strCaption;
		pWnd->GetWindowText(strCaption);

		DWORD dwTextStyle = DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | (bIsRTL ? DT_RTLREADING | DT_RIGHT : 0);

		COLORREF clrText = RGB(0, 0, 0);

		int widthFull = rectText.Width();
		int width = pDC->GetTextExtent(strCaption).cx;

		if (bTextCenter && width < widthFull)
		{
			rectText.left += (widthFull - width) / 2;
		}

		rectText.right = min(rectText.left + width, rectText.right);

		if (rectText.right > rectText.left)
		{
			if (bGlass)
			{
				DrawTextOnGlass(pDC, strCaption, rectText, dwTextStyle, 10);
			}
			else
			{
				COLORREF clrOldText = pDC->SetTextColor(clrText);
				pDC->DrawText(strCaption, rectText, dwTextStyle);
				pDC->SetTextColor(clrOldText);
			}
		}

		pDC->SetBkMode(nOldMode);
		pDC->SelectObject(pOldFont);
	}

	void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip /*bNCArea*/)
	{
		ASSERT_VALID(pDC);
		ASSERT_VALID(pBar);

		if (DYNAMIC_DOWNCAST(CReBar, pBar) != NULL || DYNAMIC_DOWNCAST(CReBar, pBar->GetParent()))
		{
			FillReBarPane(pDC, pBar, rectClient);
			return;
		}

		if (pBar->IsKindOf(RUNTIME_CLASS(CKofMfcOutlookBarPane)))
		{
			((CKofMfcOutlookBarPane*) pBar)->OnEraseWorkArea(pDC, rectClient);
			return;
		}

		if (pBar->IsKindOf(RUNTIME_CLASS(CMFCCaptionBar)))
		{
			CMFCCaptionBar* pCaptionBar = (CMFCCaptionBar*) pBar;

			if (pCaptionBar->IsMessageBarMode())
			{
				pDC->FillRect(rectClip, &afxGlobalData.brBarFace);
			}
			else
			{
				pDC->FillSolidRect(rectClip, pCaptionBar->m_clrBarBackground == -1 ? afxGlobalData.clrBarShadow : pCaptionBar->m_clrBarBackground);
			}
			return;
		}

		if (pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
		{
			CMFCPopupMenuBar* pMenuBar = (CMFCPopupMenuBar*) pBar;

			if (pMenuBar->IsDropDownListMode())
			{
				pDC->FillRect(rectClip, &afxGlobalData.brWindow);
				return;
			}
		}

		// By default, control bar background is filled by
		// the system 3d background color

		//pDC->FillRect(rectClip.IsRectEmpty() ? rectClient : rectClip, pBar->IsDialogControl() ? &afxGlobalData.brBtnFace : &afxGlobalData.brBarFace);
		CBrush* pBrush = new CBrush(RGB(59, 59, 59));
		pDC->FillRect(rectClip.IsRectEmpty() ? rectClient : rectClip, pBar->IsDialogControl() ? pBrush : pBrush);
		DeleteObject(pBrush);
	}
};

