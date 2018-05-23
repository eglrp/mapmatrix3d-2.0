// ChildDomFrm.cpp : 实现文件
//


#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "ChildDomFrm.h"


// CChildDomFrm

IMPLEMENT_DYNAMIC(CChildDomFrm, CMDIChildWndEx)

CChildDomFrm::CChildDomFrm()
{

}

CChildDomFrm::~CChildDomFrm()
{
}


BEGIN_MESSAGE_MAP(CChildDomFrm, CMDIChildWndEx)
END_MESSAGE_MAP()



// CChildDomFrm 消息处理程序




BOOL CChildDomFrm::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (!CMDIChildWndEx::PreCreateWindow(cs))
	{
		return FALSE;
	}
	
	CRect rect;
	AfxGetMainWnd()->GetClientRect(rect);
	cs.x = 0;
	cs.y = 0;
	cs.cx = rect.Width();
	cs.cy = rect.Height();
	return TRUE;
}
