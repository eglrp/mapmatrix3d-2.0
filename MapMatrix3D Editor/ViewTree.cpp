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

#include "stdafx.h"
#include "ViewTree.h"
//#include "UseEventHandler.h"
#include "FileView.h"
#include "MapMatrix3D Editor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFileView* g_mfcFileView;
//extern CPickHandler* g_mfcPickHander;
/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CViewTree::OnNMClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 消息处理程序

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}


void CViewTree::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	//TODO: 在此添加控件通知处理程序代码
	CPoint point;
	GetCursorPos(&point);					       //获取屏幕鼠标坐标   
	ScreenToClient(&point);						   //转化成客户坐标   
	UINT uFlag;									   //接收有关点击测试的信息的整数   
	HTREEITEM hTree = HitTest(point, &uFlag);	   //返回与CtreeTtrl关联的光标的当前位置和句柄   
	
	if((TVHT_NOWHERE & uFlag))   
	{     
		return;   
	}   
	
	if (hTree && (TVHT_ONITEMSTATEICON & uFlag))   //点中复选框   
	{     
		SelectItem(hTree);      
		BOOL bCheck = GetCheck(hTree);   //获取当前复选状态  
		HTREEITEM hParentTree = GetParentItem(hTree);
		HTREEITEM hChildTree = GetChildItem(hTree);

		if (hParentTree)
		{
			SetCheck(hParentTree, !bCheck);
		}
		
		if (hChildTree)
		{
			SetCheck(hChildTree, !bCheck);
		}

		//进行隐藏或显示
		int selectCol = g_mfcFileView->getSelectedItemCount();

		if (selectCol < 0)
		{
			return;
		}

		theApp.ShowOrHideSceneGroup(selectCol, !bCheck);
	}   

	 *pResult = 0; 
}
