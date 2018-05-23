// OPStatusBar.cpp : 实现文件
//

#include "stdafx.h"
#include "OPStatusBar.h"
#include "resource.h"
#include "qcomm.h"
#include "KofRibbonProgressBar.h"
#include "KofVisualManager.h"
#include "KofVisualManagerOffice2007.h"

#define DEVICE_SPEED_PANE 1
#define GROUND_COORDINATE_PANE 2

OPStatusBar* g_mfcOpsStatusBar = NULL;

IMPLEMENT_DYNAMIC(OPStatusBar, CMFCRibbonStatusBar)

OPStatusBar::OPStatusBar()
{
	g_mfcOpsStatusBar = this;
	pos = 0;
}

OPStatusBar::~OPStatusBar()
{
}


BEGIN_MESSAGE_MAP(OPStatusBar, CMFCRibbonStatusBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// OPStatusBar 消息处理程序


void OPStatusBar::ShowCoordinate(double gx, double gy, float ele)
{
	char str[100];
	sprintf_s(str, "X: %.3lf Y: %.3lfZ: %.1lf\n", gx, gy, ele);

	CRect rect;
	GetClientRect(rect);
	FindByID(GROUND_COORDINATE_PANE)->SetRect(rect);
	FindByID(GROUND_COORDINATE_PANE)->SetText(str);

	Invalidate(TRUE);
}

void OPStatusBar::ShowDeviceCoordinate(double x, double y, double z)
{
	//char str[100];
	//sprintf(str, "X: %.6fl Y: %.6lf Z: %.6f", x, y, z);
	//SetPaneText(DEVICE_SPEED_PANE, CString(str));
}

void OPStatusBar::ShowHint(char* strHint)
{
	std::string str = strHint;

	DWORD id = GetThreadLocale();

	if (id != 1033)
	{
		CRect rect;
		GetClientRect(rect);
		FindByID(GROUND_COORDINATE_PANE)->SetRect(rect);
		FindByID(GROUND_COORDINATE_PANE)->SetText(str.c_str());

		Invalidate(TRUE);
	}
}

void OPStatusBar::ShowDeviceCoordinate(LPCTSTR strDevciecoordinate)
{
	/*SetPaneText(DEVICE_SPEED_PANE, CString(strDevciecoordinate));*/

}

void OPStatusBar::StartProgress(int elapseTime)
{
	//开始进度条不停变化
	SetProgressBarPos(0);
	/*SetTimer1(elapseTime);*/
}

void OPStatusBar::EndProgress()
{
	EndTimer();
	SetProgressBarPos(1000);
}

void OPStatusBar::SetProgressBarPos(int POS)
{
	/*OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);*/

	if (POS - pos < 1 && POS - pos >= 0)
	{
		return;
	}

	char strMsg[99];
	sprintf_s(strMsg, "POS %d pos %d\n", POS, pos);
	_logop(strMsg);

	m_wndProgressBar->SetPos(POS, 1);
	ShowPercentage(POS);

	Sleep(50);
	pos = POS;
}

int OPStatusBar::OnCreate(LPCREATESTRUCT LPCREATESTRUCT)
{
	if (CMFCRibbonStatusBar::OnCreate(LPCREATESTRUCT) == -1)
		return -1;

	AddElement(new CMFCRibbonStatusBarPane(GROUND_COORDINATE_PANE, "", TRUE), "");
	// TODO:  在此添加您专用的创建代码
	progress_bar.Create(CString(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 65539);
	AddSeparator();

	DWORD dwStyle = progress_bar.GetStyle();
	SetWindowLong(progress_bar.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_BITMAP);
	//
	//CSize str_size = GetDC()->GetTextExtent(CString("X: 00000000.0000 Y: 00000000.0000 Z: 0000000.0000"));
	m_wndProgressBar = new CMFCRibbonProgressBar(IDC_PROGRESSBAR, 500, 18);
	m_wndProgressBar->SetRange(0, 1000);
	m_wndProgressBar->SetPos(0);
	
	//m_wndProgressBar->SetInfiniteMode(TRUE);
	AddExtendedElement(m_wndProgressBar, "");
	//SetPaneInfo(DEVICE_SPEED_PANE, ID_SEPARATOR, SBPS_POPOUT, str_size.cx);
	//SetPaneInfo(GROUND_COORDINATE_PANE, ID_SEPARATOR, SBPS_POPOUT, str_size.cx);
	AddSeparator();
	double pos = 0;
	char percentStr[100];
	sprintf_s(percentStr, "%.1lf%%", pos);
	AddExtendedElement(new CMFCRibbonStatusBarPane(3, percentStr, TRUE), percentStr);
	AddSeparator();
	AddSeparator();
	AddSeparator();
	AddSeparator();
	AddSeparator();
	AddSeparator();
	//
	return 0;
}

CWnd* OPStatusBar::GetProgressBarWindow()
{
	return &progress_bar;
}

void OPStatusBar::OnSize(UINT nType, int cx, int cy)
{
	CMFCRibbonStatusBar::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//CRect rect;
	//GetItemRect(0, &rect);
	//progress_bar.MoveWindow(rect);

}

void OPStatusBar::OnTimer(UINT_PTR nIDEvent)
{
	//switch(nIDEvent)  
	//{  
	//case ID_TIMER_1:
	//	{
	//		if (pos < 990)
	//		{
	//			pos++;
	//			SetProgressBarPos(pos);
	//		}
	//		
	//	}
	//}  

	//CMFCRibbonStatusBar::OnTimer(nIDEvent);
}

void OPStatusBar::SetTimer1(int elapseTime)
{
	this->SetTimer(ID_TIMER_1, elapseTime, NULL);
}

void OPStatusBar::EndTimer()
{
	this->SetTimer(ID_TIMER_2, 20, NULL);
}

void OPStatusBar::ShowPercentage(int percentage)
{
	double percent = (double) percentage / 10.0;
	char str[100];

	if (percentage != 1000)
	{
		sprintf_s(str, "%.1lf%%", percent);
	}
	else
	{
		DWORD id = GetThreadLocale();

		if (id == 1033)
		{
			sprintf_s(str, "Finish");
		}
		else
		{
			sprintf_s(str, "完成");
		}
		
	}

	FindByID(3)->SetText(str);
	Invalidate(TRUE);
}

void OPStatusBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CMFCRibbonStatusBar::OnPaint()
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

	// draw the size box in the bottom right corner
	if (!m_rectSizeBox.IsRectEmpty())
	{
		CRect rectSizeBox = m_rectSizeBox;

		if (m_bBottomFrame)
		{
			rectSizeBox.OffsetRect(-2, -2);
		}

		CMFCVisualManager::GetInstance()->OnDrawStatusBarSizeBox(pDC, NULL, rectSizeBox);
	}

	CFont* pFont = GetFont();
	CFont* pOldFont = pDC->SelectObject(pFont);
	ENSURE(pOldFont != NULL);

	int i = 0;

	if (IsInformationMode())
	{
		OnDrawInformation(pDC, m_strInfo, m_rectInfo);
	}
	else
	{
		for (i = 0; i <(int) m_arElements.GetSize(); i++)
		{
			ASSERT_VALID(m_arElements [i]);
			m_arElements [i]->OnDraw(pDC);
		}
	}

	for (i = 0; i <(int) m_arExElements.GetSize(); i++)
	{
		ASSERT_VALID(m_arExElements [i]);
		/*m_arExElements [i]->OnDraw(pDC);*/
		CKofRibbonProgressBar* pProgressBar = DYNAMIC_DOWNCAST(CKofRibbonProgressBar, m_arExElements[i]);

		if (pProgressBar != NULL)
		{
			pProgressBar->OverrideOnDraw(pDC);
		}
		else
		{
			m_arExElements [i]->OnDraw(pDC);
		}
	}

	pDC->SelectObject(pOldFont);
	pDC->SelectClipRgn(NULL);

	DeleteObject(rgnClip);
	DeleteObject(pOldFont);
	DeleteObject(pFont);
	ReleaseDC(pDC);
}

void OPStatusBar::OnFillBackground(CDC* pDC, CRect rectClient)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	if (m_bIsTransparentCaption)
	{
		rectClient.top = m_rectCaption.bottom;
	}

	CMFCVisualManager* pManager = CKofVisualManagerOffice2007::GetInstance();
	CKofVisualManagerOffice2007* pVisManager = DYNAMIC_DOWNCAST(CKofVisualManagerOffice2007, pManager);

	pVisManager->OverrideOnFillBarBackground(pDC, this, rectClient, rectClient);

	//DeleteObject(pManager);
	//delete pManager;

}

