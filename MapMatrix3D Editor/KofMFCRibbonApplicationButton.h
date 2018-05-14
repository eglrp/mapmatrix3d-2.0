#pragma once
#include "afxribbonbar.h"
class CKofMFCRibbonApplicationButton :
	public CMFCRibbonApplicationButton
{
public:
	CKofMFCRibbonApplicationButton(void);
	//~CKofMFCRibbonApplicationButton(void);

protected:
	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonDblClk(CPoint point);
};

