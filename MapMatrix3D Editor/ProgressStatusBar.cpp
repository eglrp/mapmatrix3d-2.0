// ProgressStatusBar.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "ProgressStatusBar.h"


// CProgressStatusBar

IMPLEMENT_DYNAMIC(CProgressStatusBar, CStatusBar)

CProgressStatusBar::CProgressStatusBar()
{

}

CProgressStatusBar::~CProgressStatusBar()
{
}


BEGIN_MESSAGE_MAP(CProgressStatusBar, CStatusBar)
END_MESSAGE_MAP()



// CProgressStatusBar 消息处理程序

int CProgressStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	VERIFY(CStatusBar::OnCreate(lpCreateStruct) ==0);
	VERIFY(m_Progress.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, IDC_PROGRESS));
	return 0;
}


void CProgressStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);
	CRect rect;
	GetItemRect(1, &rect);
	m_Progress.MoveWindow(rect); 
}




