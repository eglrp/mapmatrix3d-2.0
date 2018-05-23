// DynamicEdit.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyListDlg.h"
#include "DynamicEdit.h"
#include "resource.h"
#include "qcomm.h"

extern CPropertyListDlg* g_mfcPropertyListDlg;

// CDynamicEdit

IMPLEMENT_DYNAMIC(CPropertyDynamicEdit, CEdit)

CPropertyDynamicEdit::CPropertyDynamicEdit()
{

}

CPropertyDynamicEdit::~CPropertyDynamicEdit()
{
}


BEGIN_MESSAGE_MAP(CPropertyDynamicEdit, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CDynamicEdit message handlers




void CPropertyDynamicEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	CString strText;
	GetWindowText(strText);
	int iSaveItem = g_mfcPropertyListDlg->dynamicCtrlList.obtainSaveiTtem();
	int iSubSaveItem = g_mfcPropertyListDlg->dynamicCtrlList.obtainSaveSubiTtem();
	g_mfcPropertyListDlg->dynamicCtrlList.SetItemText(iSaveItem, iSubSaveItem, strText);
	ShowWindow(FALSE);
}



void CPropertyDynamicEdit::OnSetFocus(CWnd* pOldWnd)
{
	CString str;
	GetWindowText(str);
	int length = str.GetLength() - 1;
	SetSel(length, length, FALSE);
	CEdit::OnSetFocus(pOldWnd);
	// TODO: Add your message handler code here
}


BOOL CPropertyDynamicEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
			case VK_TAB:
			{
				TabKeyOperation();
			}
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

void CPropertyDynamicEdit::TabKeyOperation()
{
	int iSaveTtem = g_mfcPropertyListDlg->dynamicCtrlList.iSaveTtem;
	int iSaveSubTtem = g_mfcPropertyListDlg->dynamicCtrlList.iSaveSubTtem;

	CString str;
	GetWindowText(str);
	g_mfcPropertyListDlg->dynamicCtrlList.SetItemText(iSaveTtem, iSaveSubTtem, str);

	if (g_mfcPropertyListDlg->dynamicCtrlList.iSaveSubTtem == 0)
	{
		g_mfcPropertyListDlg->dynamicCtrlList.iSaveSubTtem++;
	}
	else if (g_mfcPropertyListDlg->dynamicCtrlList.iSaveSubTtem == 1)
	{
		g_mfcPropertyListDlg->dynamicCtrlList.iSaveTtem++;
		g_mfcPropertyListDlg->dynamicCtrlList.iSaveSubTtem--;
	}

	g_mfcPropertyListDlg->dynamicCtrlList.ShowEditControl();
}


