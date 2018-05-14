#pragma once
#include "afxcontrolrenderer.h"
class CKofMFCControlRenderer :
	public CMFCControlRenderer
{
public:
	CKofMFCControlRenderer(void);
	~CKofMFCControlRenderer(void);

	void OverrideDraw(CDC* pDC, CRect rect, UINT index, BYTE alphaSrc = 255)
	{
		CRect rectInter(rect);
		rectInter.left   += m_Params.m_rectSides.left;
		rectInter.top    += m_Params.m_rectSides.top;
		rectInter.right  -= m_Params.m_rectSides.right;
		rectInter.bottom -= m_Params.m_rectSides.bottom;

		alphaSrc = 150;
		FillInterior(pDC, rectInter, index, alphaSrc);

		DrawFrame(pDC, rect, index, alphaSrc);
	}
};

