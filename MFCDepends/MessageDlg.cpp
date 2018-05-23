// MessageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCDepends.h"
#include "MessageDlg.h"
#include "afxdialogex.h"

const UINT_PTR BLAND_IN = 1;  
const UINT_PTR DLG_DELAY = 2;  
const UINT_PTR BLAND_OUT = 3;  

const UINT IN_ELAPSE = 40;  
const UINT DELAY_ELAPSE = 800;  
const UINT OUT_ELAPSE = 40;  

//淡入淡出跨度  
const BYTE BLAND_SPAN = 15;  
//淡入淡出最小值  
const BYTE BLAND_MIN = 0;  
//淡入淡出最大值  
const BYTE BLAND_MAX = 255*0.8;  
//淡入淡出颜色值设置  
const COLORREF BLAND_COLOR = 0;
// CMessageDlg 对话框

IMPLEMENT_DYNAMIC(CMessageDlg, CDialogEx)

CMessageDlg::CMessageDlg(std::string str,CWnd* pParent /*=NULL*/)
	: CDialogEx(CMessageDlg::IDD, pParent)
	, m_strTipInfo(str)  
	, m_bAlpha(0) 
{
	
}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialogEx)
	ON_WM_TIMER()  
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMessageDlg 消息处理程序
BOOL CMessageDlg::OnInitDialog()  
{  
	CDialog::OnInitDialog();  
	
	// TODO:  Add extra initialization here 
	if (m_strTipInfo.length()<=36)
	{
		m_strTipInfo = "\n"+m_strTipInfo;
	}
	CString cstr(m_strTipInfo.c_str());
	SetDlgItemText(IDC_STATIC_MESSAGE, cstr);  


	CenterScreen();
	//InitDlgPosition();  

	//设置窗口可淡入淡出  
	ModifyStyleEx(NULL, WS_EX_LAYERED);  

	//消息渐入渐出效果  
	SetTimer(BLAND_IN, IN_ELAPSE, NULL);  

	static CFont m_font;

	m_font.CreateFont(
		20,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("微软雅黑"));                 // lpszFacename
	GetDlgItem(IDC_STATIC_MESSAGE)->SetFont(&m_font);

	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);
	SetLayeredWindowAttributes(0,0,LWA_ALPHA);
	return TRUE;  
}  


void CMessageDlg::OnTimer(UINT_PTR nIDEvent)  
{  
	RECT rect;  
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);  
	int cy = rect.bottom-rect.top;  
	int cx = rect.right-rect.left;  

	CRect rectTip;  
	GetWindowRect(&rectTip);  

	switch (nIDEvent)  
	{  
	case BLAND_IN:  
		{  
			if (m_bAlpha > (BLAND_MAX - BLAND_SPAN))  
			{  
				m_bAlpha = BLAND_MAX;  
			}  
			else  
			{  
				m_bAlpha += BLAND_SPAN;  
			}  

			SetLayeredWindowAttributes(BLAND_COLOR, m_bAlpha, LWA_ALPHA);  

			if (BLAND_MAX == m_bAlpha)  
			{  
				KillTimer(BLAND_IN);  

				SetTimer(DLG_DELAY, DELAY_ELAPSE, NULL);  
			}  

			break;  
		}  
	case DLG_DELAY:  
		{  
			KillTimer(DLG_DELAY);  
			SetTimer(BLAND_OUT, OUT_ELAPSE, NULL);  

			break;  
		}  
	case BLAND_OUT:  
		{  
			if (m_bAlpha < BLAND_SPAN)  
			{  
				m_bAlpha = BLAND_MIN;  
			}  
			else  
			{  
				m_bAlpha -= BLAND_SPAN;  
			}  

			SetLayeredWindowAttributes(BLAND_COLOR, m_bAlpha, LWA_ALPHA);  

			if (BLAND_MIN == m_bAlpha)  
			{  
				KillTimer(BLAND_OUT);  
				PostMessage(WM_CLOSE);  
			}  

			break;  
		}  
	}  

	CDialog::OnTimer(nIDEvent);  
}

BOOL CMessageDlg::PreTranslateMessage(MSG* pMsg)  
{  
	//对话框屏蔽Enter和ESC键  
	if (WM_KEYDOWN == pMsg->message)  
	{  
		if (    (VK_RETURN == pMsg->wParam)  
			|| (VK_ESCAPE == pMsg->wParam))  
		{  
			return TRUE;  
		}  
	}  

	return CDialog::PreTranslateMessage(pMsg);  
}  

void CMessageDlg::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CPaintDC   dc(this);   
	CRect   rect;   
	GetClientRect(&rect);   
	CDC   dcMem;   
	dcMem.CreateCompatibleDC(&dc);   
	CBitmap   bmpBackground;   
	bmpBackground.LoadBitmap(IDB_BITMAP1);   //IDB_BITMAP是你自己的图对应的ID   ，由于我刚刚加入的位图资源 
	//被我命名成了IDB_Bg，因而我这句就是bmpBackground.LoadBitmap(IDB_Bg);  

	BITMAP   bitmap;   
	bmpBackground.GetBitmap(&bitmap);   
	CBitmap   *pbmpOld=dcMem.SelectObject(&bmpBackground);   
	dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,   
		bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);   
}


HBRUSH CMessageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if(nCtlColor == CTLCOLOR_STATIC ) 
	{ 
		pDC->SetBkMode(TRANSPARENT);//设置背景透明 
		pDC->SetTextColor(RGB(255,255,255));//设置字体为黄色 

		return (HBRUSH)::GetStockObject(NULL_BRUSH); 
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CMessageDlg::CenterScreen()
{
	//ShowWindow(SW_NORMAL);  
	CRect rtDesk;  
	CRect rtDlg;  
	::GetWindowRect(::GetDesktopWindow(), &rtDesk);  
	GetWindowRect(&rtDlg);  
	int iXpos = rtDesk.Width() / 2 - rtDlg.Width() / 2;  
	int iYpos = rtDesk.Height() / 2 - rtDlg.Height() / 2;  
	SetWindowPos(NULL, iXpos, iYpos, 0, 0, SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOZORDER);  
}