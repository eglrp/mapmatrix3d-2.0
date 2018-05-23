// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// ChildFrm.cpp : CChildFrame 类的实现
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "qcomm.h"
#include "ChildFrm.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame
CChildFrame* g_mfcChildFrame = NULL;

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()

// CChildFrame 构造/析构

CChildFrame::CChildFrame()
{
	// TODO: 在此添加成员初始化代码
	g_mfcChildFrame = this;
	WindowMove = (double) GetSystemMetrics(SM_CYCAPTION);
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;
	
	
	double widthFileView = 0;
	
	/*cs.x = rect.left - 3 * GetSystemMetrics(SM_CXDLGFRAME);
	cs.y = rect.top - GetSystemMetrics(SM_CYCAPTION) - 3 * GetSystemMetrics(SM_CXDLGFRAME);
	cs.cx = rect.Width() + 3 * GetSystemMetrics(SM_CXDLGFRAME) + 200;
	cs.cy = rect.Height() - 2 * GetSystemMetrics(SM_CYCAPTION);*/


	/*cs.x = rect.left -  3*GetSystemMetrics(SM_CXDLGFRAME);
	cs.y = rect.top - GetSystemMetrics(SM_CYCAPTION) - 3*GetSystemMetrics(SM_CXDLGFRAME);
	cs.cx = rect.Width()+6*GetSystemMetrics(SM_CXDLGFRAME);
	cs.cy = rect.Height() - GetSystemMetrics(SM_CYCAPTION) - 3*GetSystemMetrics(SM_CXDLGFRAME);*/

	/*cs.x = 0;
	cs.y=0;
	cs.cx= rect.Width();
	cs.cy= rect.Height();*/
	cs.style =WS_MAXIMIZE ;
	return TRUE;
}

// CChildFrame 诊断

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 消息处理程序