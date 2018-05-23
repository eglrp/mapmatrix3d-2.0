// ProgressBar.cpp : 实现文件
//

#include "stdafx.h"
#include "ProgressBar.h"
#include "afxdialogex.h"
#include "publicDefined.h"

// CProgressBar 对话框
CProgressBar* g_mfcProgressBar = NULL;

IMPLEMENT_DYNAMIC(CProgressBar, CDialogEx)

CProgressBar::CProgressBar(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgressBar::IDD, pParent)
{
	g_mfcProgressBar = this;
	pos = 0;
}

CProgressBar::~CProgressBar()
{
}

void CProgressBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
}

BEGIN_MESSAGE_MAP(CProgressBar, CDialogEx)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// CProgressBar 消息处理程序


BOOL CProgressBar::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	GetWindowRect(rect);
	ClientToScreen(rect); 
	MoveWindow(1920 / 2 - rect.Width() /2, 1080 /2 - rect.Height() / 2, rect.Width(), rect.Height());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CProgressBar::ChangeStaticText(CString posStr)
{
	int posPercent = atoi(posStr);
	GetDlgItem(IDC_STATIC)->SetWindowText(posStr);
	m_ProgressBar.SetPos(posPercent);
}

void CProgressBar::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnTimer(nIDEvent);
	m_ProgressBar.SetPos(pos);

	if(pos == -1)
	{
		this->EndDialog(true);
	}
}


void CProgressBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	// TODO: 在此处添加消息处理程序代码
	this->KillTimer(1);
	this->SetTimer(1, 20, NULL);
	m_ProgressBar.SetRange(1, 1000);

}
