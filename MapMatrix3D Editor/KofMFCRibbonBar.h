#pragma once  
#include "KofRibbonCategory.h"
#include<afxribbonbar.h>
//////////////////////////////////////////////////////////////////////////  
// CKofMFCRibbonQuickAccessToolBar  


//////////////////////////////////////////////////////////////////////////  
// CKofMFCRibbonBar  

class CKofMFCRibbonBar : public CMFCRibbonBar  
{  
	DECLARE_DYNAMIC(CKofMFCRibbonBar)  
public:  
	CKofMFCRibbonBar(BOOL bReplaceFrameCaption = TRUE);  
	virtual ~CKofMFCRibbonBar();  

protected:
	HICON m_hIcon;
protected:  
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);  
	virtual BOOL OnShowRibbonQATMenu(CWnd* pWnd,int x,int y,CMFCRibbonBaseElement* pHit);
	DECLARE_MESSAGE_MAP() 
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};  