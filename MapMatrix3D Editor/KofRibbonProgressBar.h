#pragma once
#include "afxribbonprogressbar.h"
#include "OPStatusBar.h"

class CKofRibbonProgressBar : public CMFCRibbonProgressBar
{
	friend class OPStatusBar;
public:
	CKofRibbonProgressBar(UINT nID, int nWidth = 90, int nHeight = 22) : CMFCRibbonProgressBar(nID, nWidth, nHeight)
	{

	}

	void OnDrawRibbonProgressBar(CDC* pDC, CMFCRibbonProgressBar* /*pProgress*/, CRect rectProgress, CRect rectChunk, BOOL /*bInfiniteMode*/);

protected:
	void OverrideOnDraw(CDC* pDC);
};

