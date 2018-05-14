#pragma once
#include "afxribboncategory.h"
class CKofRibbonTab : public CMFCRibbonTab
{
	friend class CKofMFCRibbonBar;
public:
	void OverrideOnDraw(CDC* pDC);

};

