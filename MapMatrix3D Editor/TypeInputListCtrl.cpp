// TypeInputListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyListDlg.h"
#include "TypeInputListCtrl.h"
#include "resource.h"
#include "qcomm.h"

// CTypeInputListCtrl
extern CPropertyListDlg* g_mfcPropertyListDlg;

IMPLEMENT_DYNAMIC(CTypeInputListCtrl, CListCtrl)

CTypeInputListCtrl::CTypeInputListCtrl()
{
	iSaveTtem = 0;
	iSaveSubTtem = 0;
}

CTypeInputListCtrl::~CTypeInputListCtrl()
{
}


BEGIN_MESSAGE_MAP(CTypeInputListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_TAB_CTRL_LIST, &CTypeInputListCtrl::OnTabCtrlList)
END_MESSAGE_MAP()



// CTypeInputListCtrl message handlers

BOOL CTypeInputListCtrl::EditLabel(int iItem, int iSubItem)
{
	CRect rc;
	if (!GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rc))
		return FALSE;

	CString strText = GetItemText(iItem, iSubItem);

	//取消焦点
	g_mfcPropertyListDlg->dynamicCtrlList.SetFocus();

	//保存 iTtem 和iSubItem
	iSaveTtem = iItem;
	iSaveSubTtem = iSubItem;

	//把edit control 显示过来
	ShowEditControl();
	
	return TRUE;
}

void CTypeInputListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	lvinfo.flags = LVHT_ABOVE;

	int nItem = SubItemHitTest(&lvinfo);

	if (nItem != -1)
	{
		EditLabel(lvinfo.iItem, lvinfo.iSubItem);
	}
}

afx_msg LRESULT CTypeInputListCtrl::OnChangeListText(WPARAM wParam, LPARAM lParam)
{
	CString* str = (CString*) lParam;

	AfxMessageBox(_T("ok"));

	return 0;
}


void CTypeInputListCtrl::ShowEditControl()
{
	CString strText = GetItemText(iSaveTtem, iSaveSubTtem);

	CRect rc;
	if (!GetSubItemRect(iSaveTtem, iSaveSubTtem, LVIR_LABEL, rc))
		return;

	CRect listRect;
	GetWindowRect(listRect);

	g_mfcPropertyListDlg->ScreenToClient(&listRect);
	g_mfcPropertyListDlg->dynamicEditContrl.MoveWindow(rc.left + listRect.left, rc.top + listRect.top, rc.Width() + 3, rc.Height() + 3);
	g_mfcPropertyListDlg->dynamicEditContrl.ShowWindow(TRUE);
	g_mfcPropertyListDlg->dynamicEditContrl.SetWindowTextA(strText);
	g_mfcPropertyListDlg->dynamicEditContrl.SetFocus();
}


afx_msg LRESULT CTypeInputListCtrl::OnTabCtrlList(WPARAM wParam, LPARAM lParam)
{
	if (iSaveTtem == 0)
	{
		iSaveSubTtem++;

		CString strText = GetItemText(iSaveTtem, iSaveSubTtem);

		CRect rc;
		if (!GetSubItemRect(iSaveTtem, iSaveSubTtem, LVIR_LABEL, rc));

		CRect listRect;
		GetWindowRect(listRect);

		g_mfcPropertyListDlg->ScreenToClient(&listRect);
		g_mfcPropertyListDlg->dynamicEditContrl.MoveWindow(rc.left + listRect.left, rc.top + listRect.top, rc.Width() + 3, rc.Height() + 3);
		g_mfcPropertyListDlg->dynamicEditContrl.ShowWindow(TRUE);
		g_mfcPropertyListDlg->dynamicEditContrl.SetWindowTextA(strText);
		g_mfcPropertyListDlg->dynamicEditContrl.SetFocus();
	}
	else if (iSaveSubTtem == 1)
	{
		iSaveTtem++;
		iSaveSubTtem--;

		CString strText = GetItemText(iSaveTtem, iSaveSubTtem);

		CRect rc;
		if (!GetSubItemRect(iSaveTtem, iSaveSubTtem, LVIR_LABEL, rc));

		CRect listRect;
		GetWindowRect(listRect);

		g_mfcPropertyListDlg->ScreenToClient(&listRect);
		g_mfcPropertyListDlg->dynamicEditContrl.MoveWindow(rc.left + listRect.left, rc.top + listRect.top, rc.Width() + 3, rc.Height() + 3);
		g_mfcPropertyListDlg->dynamicEditContrl.ShowWindow(TRUE);
		g_mfcPropertyListDlg->dynamicEditContrl.SetWindowTextA(strText);
		g_mfcPropertyListDlg->dynamicEditContrl.SetFocus();
	}

	return 0;
}
