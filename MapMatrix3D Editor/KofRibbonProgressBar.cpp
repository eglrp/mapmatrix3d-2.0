#include "StdAfx.h"
#include "KofRibbonProgressBar.h"


void CKofRibbonProgressBar::OverrideOnDraw(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	if (m_rect.IsRectEmpty() || m_nMax - m_nMin <= 0)
	{
		return;
	}

	CRect rectProgress = m_rect;
	rectProgress.DeflateRect(5, 5);

	CRect rectChunk = rectProgress;
	rectChunk.right = rectChunk.left + (m_nPos - m_nMin) * rectChunk.Width() /(m_nMax - m_nMin);
	rectChunk.DeflateRect(2, 2);

	OnDrawRibbonProgressBar(pDC, this, rectProgress, rectChunk, m_bInfiniteMode);
}

void CKofRibbonProgressBar::OnDrawRibbonProgressBar(CDC* pDC, CMFCRibbonProgressBar* /*pProgress*/, CRect rectProgress, CRect rectChunk, BOOL /*bInfiniteMode*/)
{
	ASSERT_VALID(pDC);

	if (CMFCToolBarImages::m_bIsDrawOnGlass)
	{
		CDrawingManager dm(*pDC);

		if (!rectChunk.IsRectEmpty())
		{
			dm.DrawRect(rectChunk, afxGlobalData.clrHilite, (COLORREF)-1);
		}

		dm.DrawRect(rectProgress, (COLORREF)-1, afxGlobalData.clrBarShadow);
	}
	else
	{
		if (!rectChunk.IsRectEmpty())
		{
			//进度条颜色
			CBrush* pBrush = new CBrush(RGB(255, 181, 93));
			pDC->FillRect(rectChunk, pBrush);
			pBrush->DeleteObject();
		}
		else
		{
			//进度条的底色
			rectProgress.DeflateRect(1, 1);
			CBrush* pBrush = new CBrush(RGB(27, 27, 27));
			pDC->FillRect(rectProgress, pBrush);
			pBrush->DeleteObject();
		}

		//进度条的边框色
		pDC->Draw3dRect(rectProgress, RGB(104, 104, 104), RGB(104, 104, 104));
	}
}