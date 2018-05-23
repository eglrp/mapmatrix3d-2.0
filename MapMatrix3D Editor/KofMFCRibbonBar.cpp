#include "stdafx.h"  
#include "resource.h"
#include "KofMFCRibbonBar.h"  
#include "RibbonCaptionButton.h"
#include "KofRibbonQuickAccessToolBar.h"
#include "KofRibbonTab.h"
#include "KofRibbonCategory.h"
#include "KofRibbonButton.h"
#include "KofMfcRibbonBaseElement.h"

static const int nXTabMargin = 14;  
static const int nYTabMargin = 5;  

static const int nMinRibbonWidth = 300;

static const int nXMargin = 2;  
static const int nYMargin = 2;  
static const UINT IdAutoCommand = 1;


UINT AFX_WM_POSTRECALCLAYOUT = ::RegisterWindowMessage(_T("AFX_WM_POSTRECALCLAYOUT"));
//////////////////////////////////////////////////////////////////////////  
// CKofMFCRibbonBar  

IMPLEMENT_DYNAMIC(CKofMFCRibbonBar, CMFCRibbonBar)  

	CKofMFCRibbonBar::CKofMFCRibbonBar(BOOL bReplaceFrameCaption /*= TRUE*/)  
	: CMFCRibbonBar(bReplaceFrameCaption)  
{  
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}  

CKofMFCRibbonBar::~CKofMFCRibbonBar()  
{  
}  

BEGIN_MESSAGE_MAP(CKofMFCRibbonBar, CMFCRibbonBar)  
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()  
BOOL CKofMFCRibbonBar::OnShowRibbonQATMenu(CWnd* pWnd,int x,int y,CMFCRibbonBaseElement* pHit)
{
	return TRUE;
}

CSize CKofMFCRibbonBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )  
{  
	ASSERT_VALID(this);  

	CClientDC dc(this);  

	CFont* pOldFont = dc.SelectObject(GetFont());  
	ENSURE(pOldFont != NULL);  

	TEXTMETRIC tm;  
	dc.GetTextMetrics(&tm);  

	//“标题”的高度  
	m_nCaptionHeight = 0;  
	
	if (m_bReplaceFrameCaption)  
	{  
		m_nCaptionHeight = GetSystemMetrics(SM_CYCAPTION) ;  

		if (afxGlobalData.IsDwmCompositionEnabled())  
		{  
			m_nCaptionHeight += GetSystemMetrics(SM_CYSIZEFRAME);  
		}  
	}  

	int cy = 0;  

	CSize sizeMainButton = m_sizeMainButton;  
	double scale = afxGlobalData.GetRibbonImageScale();  
	if (scale > 1.)  
	{  
		sizeMainButton.cx = (int)(.5 + scale * sizeMainButton.cx);  
		sizeMainButton.cy = (int)(.5 + scale * sizeMainButton.cy);  
	}  

	if (m_dwHideFlags & AFX_RIBBONBAR_HIDE_ALL)  
	{  
		cy = m_nCaptionHeight;  
	}  
	else  
	{  
		if (m_bRecalcCategoryHeight)  
		{  
			m_nCategoryHeight = 0;  
		}  

		//“选项卡”的高度  
		m_nTabsHeight = tm.tmHeight + 2 * nYTabMargin;  

		if (m_bRecalcCategoryHeight)  
		{  
			for (int i = 0; i < m_arCategories.GetSize(); i++)  
			{  
				CMFCRibbonCategory* pCategory = m_arCategories [i];  
				ASSERT_VALID(pCategory);  

				m_nCategoryHeight = max(m_nCategoryHeight, pCategory->GetMaxHeight(&dc))-15;  
			}  
			m_bRecalcCategoryHeight = FALSE;  
		}  

		CKofRibbonQuickAccessToolBar *pQAToolbar = (CKofRibbonQuickAccessToolBar *)&m_QAToolbar;  
		const CSize sizeAQToolbar = pQAToolbar->GetRegularSize(&dc);  

		if (IsQuickAccessToolbarOnTop())  
		{  
			m_nCaptionHeight = max(m_nCaptionHeight, sizeAQToolbar.cy + (IsWindows7Look() ? 0 : (2 * nYMargin)));  
		}  

		//“快速访问工具栏”的高度  
		const int nQuickAceesToolbarHeight = IsQuickAccessToolbarOnTop() ? 0 : sizeAQToolbar.cy;  
		
		//“分类”的高度  
		const int nCategoryHeight = (m_dwHideFlags & AFX_RIBBONBAR_HIDE_ELEMENTS) ? 0 : m_nCategoryHeight;  

		cy = nQuickAceesToolbarHeight + nCategoryHeight + max( m_nCaptionHeight + m_nTabsHeight, m_sizeMainButton.cy + nYMargin);  
	}  

	if (afxGlobalData.IsDwmCompositionEnabled())  
	{  
		if (GetParent()->IsZoomed() && m_bReplaceFrameCaption)  
		{  
			cy += ::GetSystemMetrics(SM_CYSIZEFRAME) - 2;  
		}  
	}  

	dc.SelectObject(pOldFont);  


	ReleaseDC(&dc);

	return CSize(32767, cy);  
}  

void CKofMFCRibbonBar::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
	return;
}


void CKofMFCRibbonBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CMFCRibbonBar::OnPaint()
	int i = 0;

	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rectClip;
	dc.GetClipBox(rectClip);

	CRgn rgnClip;

	if (!rectClip.IsRectEmpty())
	{
		rgnClip.CreateRectRgnIndirect(rectClip);
		pDC->SelectClipRgn(&rgnClip);
	}

	pDC->SetBkMode(TRANSPARENT);

	CRect rectClient;
	GetClientRect(rectClient);

	OnFillBackground(pDC, rectClient);

	//mark
	CFont* pFont = GetFont();
	CFont* pOldFont = pDC->SelectObject(pFont);
	ENSURE(pOldFont != NULL);

	// Draw caption bar:
	if (!m_rectCaption.IsRectEmpty())
	{
		CRect rectFill = m_rectCaption;
		rectFill.top = 0;

		if (m_bIsTransparentCaption)
		{
			pDC->FillSolidRect(rectFill, RGB(0, 0, 0));
			CMFCToolBarImages::m_bIsDrawOnGlass = TRUE;
		}

		CRect clientRect;
		GetClientRect(clientRect);
		m_rectCaptionText.MoveToXY(20, 0);

		//CMFCVisualManager* pVisualManager = CMFCVisualManager::GetInstance();
		//CKofVisualManager* pKofVisualManager = DYNAMIC_DOWNCAST(CKofVisualManager, pVisualManager);
		//CMFCVisualManager::GetInstance()->OnDrawRibbonCaption(pDC, this, m_rectCaption, m_rectCaptionText);
		int center = (m_rectCaptionText.top + m_rectCaptionText.bottom) / 2;
		//重画字体
		CString strCaption = _T("航天远景MapMatrix3D图阵三维智能测图系统");
		CRect rectText;
		COLORREF clrText = RGB(255, 255, 255);
		rectText.left = 22;
		rectText.right = rectText.left + 400;
		rectText.top = center - 8;
		rectText.bottom = rectText.top + m_rectCaptionText.Height();
		COLORREF clrOldText = pDC->SetTextColor(clrText);
		pDC->DrawText(strCaption, rectText, DT_LEFT);
		pDC->SetTextColor(clrOldText);

		//重画图标
		CRect rtWnd, rtTitle, rtButtons;
		GetWindowRect(&rtWnd); 
		CRect m_rtIcon;
		m_rtIcon.right = 20;
		m_rtIcon.left = m_rtIcon.right - 16;
		m_rtIcon.top = center - 8;
		m_rtIcon.bottom = m_rtIcon.top + 16;
		::DrawIconEx(pDC->m_hDC, m_rtIcon.left, m_rtIcon.top, m_hIcon, 
			m_rtIcon.Width(), m_rtIcon.Height(), 0, NULL, DI_NORMAL);
		m_rtIcon.OffsetRect(rtWnd.TopLeft());

		////重画字体
		//int nOldMode = pDC->SetBkMode(TRANSPARENT);
		//COLORREF clOldText=pDC->SetTextColor(RGB(255, 255, 255));
		//pDC->SelectStockObject(SYSTEM_FIXED_FONT);

		//CRect captionRect = m_rectCaptionText;
		//captionRect.left = m_rtIcon.right;
		//captionRect.right = captionRect.left + m_rectCaptionText.Width();
		//captionRect.top = center - m_rectCaptionText.Height() / 2;
		//captionRect.bottom = center + m_rectCaptionText.Height() / 2;

		//CString m_strTitle;
		//GetWindowText(m_strTitle);
		//pDC->DrawText(m_strTitle, &captionRect, DT_LEFT);
		//pDC->SetBkMode(nOldMode);
		//pDC->SetTextColor(clOldText);

		for (i = 0; i < AFX_RIBBON_CAPTION_BUTTONS; i++)
		{
			if (!m_CaptionButtons [i].GetRect().IsRectEmpty ())
			{
				CKofRibbonCaptionButton* pCaptionBtn = DYNAMIC_DOWNCAST(CKofRibbonCaptionButton, &m_CaptionButtons[i]);
				pCaptionBtn->OnDraw(pDC);
			}
		}

		for (i = 0; i < m_arContextCaptions.GetSize(); i++)
		{
			ASSERT_VALID(m_arContextCaptions [i]);
			//m_arContextCaptions [i]->OnDraw(pDC);
		}

		CMFCToolBarImages::m_bIsDrawOnGlass = FALSE;
	}

	if (m_bIsTransparentCaption && m_bQuickAccessToolbarOnTop)
	{
		CMFCToolBarImages::m_bIsDrawOnGlass = TRUE;
	}

	// Draw quick access toolbar:
	COLORREF cltTextOld = (COLORREF)-1;
	COLORREF cltQATText = CMFCVisualManager::GetInstance()->GetRibbonQuickAccessToolBarTextColor();

	if (cltQATText != (COLORREF)-1)
	{
		cltTextOld = pDC->SetTextColor(cltQATText);
	}

	//m_QAToolbar.OnDraw(pDC);

	if (cltTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor(cltTextOld);
	}

	CMFCToolBarImages::m_bIsDrawOnGlass = FALSE;

	// Draw active category:
	if (m_pActiveCategory != NULL && m_dwHideFlags == 0)
	{
		ASSERT_VALID(m_pActiveCategory);

		CKofRibbonCategory* pActiveCategory = DYNAMIC_DOWNCAST(CKofRibbonCategory, m_pActiveCategory);
		ASSERT_VALID(pActiveCategory);
		pActiveCategory->OverrideOnDraw(pDC);
	}

	// Draw tabs:
	CRect rectTabs = rectClient;
	rectTabs.top = m_rectCaption.IsRectEmpty() ? rectClient.top : m_rectCaption.bottom;
	rectTabs.bottom = rectTabs.top + m_nTabsHeight;

	COLORREF clrTextTabs = CMFCVisualManager::GetInstance()->OnDrawRibbonTabsFrame(pDC, this, rectTabs);

	for (i = 0; i < m_arCategories.GetSize(); i++)
	{
		CKofRibbonCategory* pCategory = DYNAMIC_DOWNCAST(CKofRibbonCategory, m_arCategories[i]);
		ASSERT_VALID(pCategory);

		if (pCategory->IsVisible())
		{
			CKofRibbonTab* pTab = DYNAMIC_DOWNCAST(CKofRibbonTab, &(pCategory->m_Tab));
			pTab->OverrideOnDraw(pDC);
		}
	}

	// Draw elements on right of tabs:
	COLORREF clrTextOld = (COLORREF)-1;
	if (clrTextTabs != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor(clrTextTabs);
	}

	//m_TabElements.OnDraw(pDC);

	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor(clrTextOld);
	}

	// Draw main button:

	if (m_pMainButton != NULL)
	{
		ASSERT_VALID(m_pMainButton);

		if (!m_pMainButton->GetRect().IsRectEmpty())
		{
			//CMFCVisualManager::GetInstance()->OnDrawRibbonApplicationButton(pDC, m_pMainButton);
			CRect rect = m_pMainButton->GetRect();
			CRect clientRect;
			GetClientRect(clientRect);
			rect.MoveToXY(0, -10);
			m_pMainButton->SetRect(rect);
			//m_pMainButton->OnDraw(pDC);
		}
	}

	pDC->SelectObject(pOldFont);
	pDC->SelectClipRgn(NULL);

	//pFont->DeleteObject();
	ReleaseDC(pDC);
}





void CKofMFCRibbonBar::OnSize(UINT nType, int cx, int cy)
{
	CPane::OnSize(nType, cx, cy);

	const BOOL bHideAll = cx < nMinRibbonWidth;
	const BOOL bIsHidden = (m_dwHideFlags & AFX_RIBBONBAR_HIDE_ALL) != 0;

	BOOL bWasMaximized = m_bIsMaximized;

	m_bIsMaximized = GetParent()->IsZoomed();

	if (bHideAll != bIsHidden)
	{
		if (bHideAll)
		{
			m_dwHideFlags |= AFX_RIBBONBAR_HIDE_ALL;
		}
		else
		{
			m_dwHideFlags &= ~AFX_RIBBONBAR_HIDE_ALL;
		}

		if (m_pMainButton != NULL && bHideAll)
		{
			ASSERT_VALID(m_pMainButton);
			m_pMainButton->SetRect(CRect(0, 0, 0, 0));
		}

		PostMessage(AFX_WM_POSTRECALCLAYOUT);
	}
	else if (bWasMaximized != m_bIsMaximized)
	{
		m_bForceRedraw = TRUE;
		RecalcLayout();
	}
	else
	{
		RecalcLayout();
	}

	UpdateToolTipsRect();
	// TODO: 在此处添加消息处理程序代码
}


void CKofMFCRibbonBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CPane::OnLButtonDown(nFlags, point);

	DeactivateKeyboardFocus();

	CMFCRibbonBaseElement* pDroppedDown = GetDroppedDown();
	if (pDroppedDown != NULL)
	{
		ASSERT_VALID(pDroppedDown);
		pDroppedDown->ClosePopupMenu();
	}

	if ((m_dwHideFlags & AFX_RIBBONBAR_HIDE_ALL) == AFX_RIBBONBAR_HIDE_ALL || IsWindows7Look())
	{
		CRect rectIcon = m_rectCaption;
		rectIcon.right = rectIcon.left + rectIcon.Height();

		if (rectIcon.PtInRect(point))
		{
			CPoint ptMenu(m_rectCaption.left, m_rectCaption.bottom);
			ClientToScreen(&ptMenu);

			ShowSysMenu(ptMenu);
			return;
		}
	}

	OnMouseMove(nFlags, point);

	CMFCRibbonBaseElement* ptempHit = HitTest(point);
	CKofMfcRibbonBaseElement* pHit = DYNAMIC_DOWNCAST(CKofMfcRibbonBaseElement, ptempHit);

	if (pHit != NULL)
	{
		ASSERT_VALID(pHit);

		pHit->OnLButtonDown(point);
		pHit->m_bIsPressed = TRUE;

		CRect rectHit = pHit->GetRect();
		rectHit.InflateRect(1, 1);

		RedrawWindow(rectHit);

		m_pPressed = pHit;
	}
	else
	{
		if (m_rectCaption.PtInRect(point))
		{
			if (CMFCPopupMenu::GetActiveMenu() != NULL)
			{
				CMFCPopupMenu::GetActiveMenu()->SendMessage(WM_CLOSE);
			}

			if (!m_rectSysButtons.PtInRect(point))
			{
				GetParent()->SendMessage(WM_NCLBUTTONDOWN, (WPARAM) HTCAPTION, MAKELPARAM(point.x, point.y));
			}
			return;
		}

		if (m_pActiveCategory != NULL &&(m_dwHideFlags & AFX_RIBBONBAR_HIDE_ELEMENTS) == 0)
		{
			ASSERT_VALID(m_pActiveCategory);
			CKofRibbonCategory* pActiveCategory = DYNAMIC_DOWNCAST(CKofRibbonCategory, m_pActiveCategory);

			if (pActiveCategory)
			{
				m_pPressed = pActiveCategory->OverrideOnLButtonDown(point);
			}
		}
	}

	if (m_pPressed != NULL)
	{
		ASSERT_VALID(m_pPressed);

		int nDelay = 250;

		if (m_pPressed->IsAutoRepeatMode(nDelay))
		{
			SetTimer(IdAutoCommand, nDelay, NULL);
			m_bAutoCommandTimer = TRUE;
		}
	}
	
}
