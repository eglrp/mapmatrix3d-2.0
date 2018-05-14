#pragma once
class CKofRibbonCategoryScrool : public CRibbonCategoryScroll
{
	friend class CKofMFCRibbonCategory;
public:
	void OverrideOnDraw(CDC* pDC);

};

