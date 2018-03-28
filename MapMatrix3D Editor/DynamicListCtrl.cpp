// DynamicListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AbsoluteOrientationDlg.h"
#include "DynamicListCtrl.h"
#include "MapMatrix3D Editor.h"
#include "InterfaceMFCExcute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	IDC_DYNAMIC_EDIT 1
#define IDC_DYNAMIC_COMBO 2
UINT WM_DYNAMIC_END = ::RegisterWindowMessage( "Dynamic_End" );
UINT WM_DYNAMIC_START = ::RegisterWindowMessage( "Dynamic_Start" );



/////////////////////////////////////////////////////////////////////////////
// CDynamicListCtrl
extern CAbsoluteOrientationDlg* g_mfcAbsoluteOrientationDlg;

CDynamicListCtrl::CDynamicListCtrl()
	: m_iNumColumns( 0 )
	, m_iSortColumn( -1 )
	, m_bSortAscending( TRUE )
{
	m_bEdit	= FALSE;
	m_bCheckData = FALSE;
//	for( int i = 0; i < DYNAMIC_MAX_COL; i ++ )
//	{
//		m_dmMode[i] = modeReadOnly;
//	}
	m_iItem = -1;
	m_iSubItem = -1;

	m_crNormal[0] = ::GetSysColor(COLOR_WINDOWTEXT);
	m_crNormal[1] = ::GetSysColor(COLOR_WINDOW);

	m_crSelected[0] = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crSelected[1] = ::GetSysColor(COLOR_HIGHLIGHT);

	m_crDisabled[0] = ::GetSysColor(COLOR_GRAYTEXT);
	m_crDisabled[1] = ::GetSysColor(COLOR_WINDOW);
}

CDynamicListCtrl::~CDynamicListCtrl()
{
}

BOOL CDynamicListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
	UINT nID)
{
	dwStyle |= LVS_REPORT;// | LVS_OWNERDRAWFIXED;

	if( !CListCtrl::Create( dwStyle, rect, pParentWnd, nID ) )
	{
		TRACE0( "Failed to Create list control\n" );
		return FALSE;
	};

//	VERIFY( m_ctlHeader.SubclassWindow( GetHeaderCtrl()->GetSafeHwnd() ) );

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDynamicListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CDynamicListCtrl)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT_EX(NM_DBLCLK, OnDblclk)
	ON_REGISTERED_MESSAGE( WM_DYNAMIC_END, OnDynamicEnd )
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomdraw )

	//}}AFX_MSG_MAP
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicListCtrl message handlers
// 移动列表的滚动条时，使它拥有角点
// 这样动态编辑框就是隐藏
void CDynamicListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if (GetFocus() != this) SetFocus();

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDynamicListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if (GetFocus() != this) SetFocus();

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}
void CDynamicListCtrl::OnCustomdraw ( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	
	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.
		
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{
		// This is the beginning of an item's paint cycle.
		int			nSubItem = pLVCD->iSubItem;
//		if( modeText == m_im[nSubItem] )return;

		int			nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );
		CDC*		pDC   = CDC::FromHandle ( pLVCD->nmcd.hdc );
		CRect		rcItem;
		CString		strText;
		UINT		nStyle = ILD_TRANSPARENT;
		UINT		uState = GetItemState( nItem, LVIS_SELECTED );

		GetSubItemRect( nItem,nSubItem,LVIR_LABEL,rcItem );

		// 绘制背景
		COLORREF crBack;
		if( m_bCheckData && NULL == pLVCD->nmcd.lItemlParam )
		{
			pDC->SetTextColor( m_crDisabled[0] );
			crBack = m_crDisabled[1];
		}
		else
		{
			// ( CDIS_SELECTED & pLVCD->nmcd.uItemState ) can not express selections
			// when the flag LVS_SHOWSELALWAYS is setted!
			// what does this mean?
			// use LVIS_SELECTED instead
			if( LVIS_SELECTED == (LVIS_SELECTED&uState) )
			{
				nStyle |= ILD_FOCUS;
				pDC->SetTextColor( m_crSelected[0] );
				crBack = m_crSelected[1];
			}
			else
			{
				pDC->SetTextColor( m_crNormal[0] );
				crBack = m_crNormal[1];
			}
		}

		if( 0 == pLVCD->iSubItem )
		{
			LVITEM item;
			item.iItem = nItem;
			item.iSubItem = 0;
			item.mask = LVIF_IMAGE;
			if( GetItem( &item ) )
			{
				CImageList *pImageList = GetImageList( LVSIL_SMALL );
				if( pImageList )
				{
					CRect rcIcon;
					GetSubItemRect( nItem,0,LVIR_ICON,rcIcon );
					POINT pt;
					pt.x = rcIcon.left;
					pt.y = rcIcon.top;
					pImageList->Draw( pDC,item.iImage,pt,nStyle );
				}
			}

		}

		// 如果只显示图标，把图标显示在中间
		// 否则把图标显示在左侧
		CRect rcFill = rcItem;
		rcFill.left += 1;
		pDC->FillSolidRect( &rcFill, crBack );

		// 绘制文字
		strText = GetItemText( nItem,nSubItem );
		rcItem.left += 3;
		rcItem.top += 1;
		if( modeColor!=m_dmMode[nSubItem].mode )
		{
			pDC->SetBkMode ( TRANSPARENT );
			pDC->DrawText ( strText, rcItem, DT_VCENTER | DT_SINGLELINE );
		}
		else
		{
			rcItem.left += 1;
			rcItem.right -= 2;
			rcItem.top += 1;
			rcItem.bottom -= 2;
			DWORD dwData = atoi(strText);
			pDC->FillSolidRect( &rcItem,dwData );
		}

		*pResult = CDRF_SKIPDEFAULT;    // We've painted everything.
	}
}

////////////////////////////////////////////////////////////////////////////
// CDynamicEdit
// 在失去角点的时候通知 Parent
// 截获Esc消息
/*
CDynamicEdit::CDynamicEdit()
{
	m_bEdit	= FALSE;
}

CDynamicEdit::~CDynamicEdit()
{
}*/
#define CTRL_C	0x3
#define CTRL_V	0x16
#define CTRL_X	0x18

BEGIN_MESSAGE_MAP(CDynamicEdit, CEdit)
	//{{AFX_MSG_MAP(CDynamicEdit)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDynamicEdit::PreTranslateMessage(MSG* pMsg) 
{
	if( WM_KEYDOWN == pMsg->message )
	{
		switch( pMsg->wParam )
		{
		case VK_ESCAPE:
			SetWindowText( m_strBack );
		case VK_RETURN:
			GetParent()->SetFocus();
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void CDynamicEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

 	if ( (m_strMask.IsEmpty()) || ((-1 != m_strMask.Find(static_cast<TCHAR> (nChar))) || 
		(nChar == VK_BACK) || (nChar == CTRL_C) || (nChar == CTRL_V) || (nChar == CTRL_X)))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
}

void CDynamicEdit::OnSetFocus( CWnd *pOldWnd )
{
	GetWindowText( m_strBack );
	m_strMask.Empty();
	CEdit::OnSetFocus(pOldWnd);
}
void CDynamicEdit::OnKillFocus( CWnd *pNewWnd )
{
	ShowWindow(SW_HIDE);

	int nModify = 1;
	CString strText;
	GetWindowText( strText );

	// 判断内容是否被修改过
	if( strText.IsEmpty() && m_strBack.IsEmpty() )
		nModify = 0;
	else if( !strText.IsEmpty() && !m_strBack.IsEmpty() )
	{
		if( 0 == m_strBack.Compare( strText ) )
			nModify = 0;
	}
	m_strBack = strText;

	DynamicInfo di;
	di.dwId = GetDlgCtrlID();
	di.pWnd = this;
	di.sMode = modeEdit;
	di.iFlag = -1;
	di.iModify = nModify;
	GetParent()->SendMessage( WM_DYNAMIC_END,(WPARAM)&di,0 );
	CEdit::OnKillFocus(pNewWnd);
}

///////////////////////////////////////////////////////////////////////////////////
// CDynamicComboBox

BEGIN_MESSAGE_MAP(CDynamicComboBox, CComboBox)
	//{{AFX_MSG_MAP(CDynamicEdit)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CDynamicComboBox::OnCbnSelchange)
END_MESSAGE_MAP()

CDynamicComboBox::CDynamicComboBox()
{
	
}

BOOL CDynamicComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if( WM_KEYDOWN == pMsg->message )
	{
		switch( pMsg->wParam )
		{
		case VK_ESCAPE:
			SetCurSel( m_iSelected );
		case VK_RETURN:
			GetParent()->SetFocus();
			return TRUE;
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CDynamicComboBox::OnSetFocus( CWnd *pOldWnd )
{
	m_iSelected = GetCurSel();
	CComboBox::OnSetFocus(pOldWnd);
}

void CDynamicComboBox::OnKillFocus( CWnd *pNewWnd )
{
	ShowWindow(SW_HIDE);

	int nModify = 1;
	int nSel = GetCurSel();
	if( nSel == m_iSelected )
		nModify = 0;

	DynamicInfo di;
	di.dwId = GetDlgCtrlID();
	di.pWnd = this;
	di.sMode = modeCombo;
	di.iFlag = -1;
	di.iModify = nModify;
	GetParent()->SendMessage( WM_DYNAMIC_END,(WPARAM)&di,0 );
	ResetContent();
	CComboBox::OnKillFocus(pNewWnd);
}

void CDynamicComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ((long)lpDrawItemStruct->itemID == -1)
		return;
	
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	CString strItem;
	GetLBText(lpDrawItemStruct->itemID, strItem);
	dc.SetBkMode(TRANSPARENT);
	BOOL bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) ? TRUE : FALSE;
	
	if (bSelected)
	{
		dc.FillSolidRect(rcItem, RGB(238, 238, 238));
		dc.SetTextColor(RGB(0, 0, 0));
	}
	else
	{
		dc.FillSolidRect(rcItem, RGB(218, 218, 218));
		dc.SetTextColor(RGB(255, 255, 255));
	}
	
	strItem = _T("是");
	dc.DrawText(strItem, strItem.GetLength(), rcItem, DT_VCENTER | DT_SINGLELINE);
	// TODO:  添加您的代码以绘制指定项
}

BOOL CDynamicComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CComboBox::Create(dwStyle, rect, pParentWnd, nID);
}

void CDynamicComboBox::OnCbnSelchange()
{
	// TODO: 在此添加控件通知处理程序代码
	int item = g_mfcAbsoluteOrientationDlg->m_list_control_point.m_iItem;
	CString strCompleteStatus = g_mfcAbsoluteOrientationDlg->m_list_control_point.GetItemText(item, 5);
	int numCol = g_mfcAbsoluteOrientationDlg->m_list_control_point.GetItemCount();

	DWORD id = GetThreadLocale();

	InterfaceMFCExcuteFactory IMfcExcuteFactory;
	InterfaceMFCExcute* IMfcExcute = IMfcExcuteFactory.create();

	if (id != 1033)
	{
		if (strCompleteStatus != _T("完成"))
		{		
			IMfcExcute->PopUpMessage("请先刺点，再选择检查点");

			return;
		}
	}
	else
	{
		if (strCompleteStatus != _T("Finish"))
		{		
			IMfcExcute->PopUpMessage("Please Click Pt, then select check pt");

			return;
		}
	}

	int index = GetCurSel();
	CString sz;
	this->GetLBText(index, sz);
	
	int subItem = g_mfcAbsoluteOrientationDlg->m_list_control_point.m_iSubItem;
	g_mfcAbsoluteOrientationDlg->m_list_control_point.SetItemText(item, subItem, sz);

	CString strNum = g_mfcAbsoluteOrientationDlg->m_list_control_point.GetItemText(item, 0);
	int ptNum = atoi(strNum);

	if (ptNum < 0)
	{
		return;
	}

	if (id != 1033)
	{
		if (sz == _T("是"))
		{
			theApp.IOsgMfcViewer->ObtainOrientEventSlot()->reSetCtrlPtCheckStatus(ptNum, true);
		}
		else if (sz == _T("否"))
		{
			theApp.IOsgMfcViewer->ObtainOrientEventSlot()->reSetCtrlPtCheckStatus(ptNum, false);
		}
	}
	else
	{
		if (sz == _T("Yes"))
		{
			theApp.IOsgMfcViewer->ObtainOrientEventSlot()->reSetCtrlPtCheckStatus(ptNum, true);
		}
		else if (sz == _T("No"))
		{
			theApp.IOsgMfcViewer->ObtainOrientEventSlot()->reSetCtrlPtCheckStatus(ptNum, false);
		}
	}
	
	ShowWindow(SW_HIDE);
}

///////////////////////////////////////////////////////////////////////////////////
//
// 启动动态窗口
BOOL CDynamicListCtrl::EditLabel(int iItem,int iSubItem)
{
	if (iSubItem !=  1)
	{
		return FALSE;
	}


	if(m_bCheckData && NULL == GetItemData(iItem))
		return FALSE;

	CRect rc;
	if(!GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rc))
		return FALSE;

	CString strText = GetItemText(iItem, iSubItem);

	DWORD id = GetThreadLocale();

	if (id != 1033)
	{
		g_mfcAbsoluteOrientationDlg->m_ComboBox1.InsertString(0, "是");
		g_mfcAbsoluteOrientationDlg->m_ComboBox1.InsertString(1, "否");
	}
	else
	{
		g_mfcAbsoluteOrientationDlg->m_ComboBox1.InsertString(0, "Yes");
		g_mfcAbsoluteOrientationDlg->m_ComboBox1.InsertString(1, "No");
	}

	CRect listRect;
	GetWindowRect(listRect);
	g_mfcAbsoluteOrientationDlg->ScreenToClient(&listRect);
	g_mfcAbsoluteOrientationDlg->m_ComboBox1.MoveWindow(rc.left + listRect.left, rc.top + listRect.top, rc.Width(), rc.Height());
	g_mfcAbsoluteOrientationDlg->m_ComboBox1.ShowWindow(SW_SHOW);
	g_mfcAbsoluteOrientationDlg->m_ComboBox1.SetFocus();

	if (id != 1033)
	{
		if (strText == _T("是"))
		{
			g_mfcAbsoluteOrientationDlg->m_ComboBox1.SetCurSel(0);
		}
		else if (strText == _T("否"))
		{
			g_mfcAbsoluteOrientationDlg->m_ComboBox1.SetCurSel(1);
		}
	}
	else
	{
		if (strText == _T("Yes"))
		{
			g_mfcAbsoluteOrientationDlg->m_ComboBox1.SetCurSel(0);
		}
		else if (strText == _T("No"))
		{
			g_mfcAbsoluteOrientationDlg->m_ComboBox1.SetCurSel(1);
		}
	}
	

	m_iItem = iItem;
	m_iSubItem = iSubItem;

	return TRUE;
}

//改变颜色
BOOL CDynamicListCtrl::EditRowColor(int iItem)
{
	return TRUE;
}

// 搜索当前点击的项，启动编辑功能
BOOL CDynamicListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
		return FALSE;
}
// 搜索当前点击的项，启动编辑功能
BOOL CDynamicListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nIndex;
	UINT uFlags;
	LPNMLISTVIEW pNmListView = (LPNMLISTVIEW)pNMHDR;
	nIndex = HitTest(pNmListView->ptAction, &uFlags);

	if( -1!=nIndex && uFlags==LVHT_ONITEMICON )
	{
		return FALSE;
	}

	*pResult = 0;
	LPNMITEMACTIVATE pNIA = (LPNMITEMACTIVATE)pNMHDR;

	if(-1 != pNIA->iItem)
	{
		/*if(pNIA->iItem == m_iItem)
		{*/
			//EditRowColor(pNIA->iItem);
		return EditLabel(pNIA->iItem, pNIA->iSubItem);
		/*	}
			else
			m_iItem = pNIA->iItem;*/
	}
	return FALSE;
}

void CDynamicListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( GetFocus() != this && m_bEdit )
	{
		SetFocus();
	}
	
	CListCtrl::OnLButtonDown(nFlags, point);
}

// 转发 WM_DYNAMIC_EDIT_END 消息
LRESULT CDynamicListCtrl::OnDynamicEnd(WPARAM wParam, LPARAM lParam)
{
	m_bEdit	= FALSE;
	PDynamicInfo pDI = (PDynamicInfo)wParam;
	if( 0 == pDI->iModify )return 0;

	// wParam : DynamicInfo
	pDI->dwId = GetDlgCtrlID();
	pDI->iRow = m_iItem;
	pDI->iColumn = m_iSubItem;
	GetParent()->SendMessage( WM_DYNAMIC_END,(WPARAM)pDI,0 );

	// 在上层没有处理的情况下，默认更改列表内容
	if( -1==pDI->iFlag )
	{
		CString strText;
		pDI->pWnd->GetWindowText(strText);

		// process
		if( 0<m_dmMode[pDI->iColumn].iMax && strText.GetLength()>m_dmMode[pDI->iColumn].iMax )
		{
			strText.Format( "请输入长度在%d之内的字符！",m_dmMode[pDI->iColumn].iMax );
			AfxMessageBox(strText);
			return 0;
		}

		BOOL bNumber = TRUE;
		BOOL bDot = FALSE;
		TCHAR chNumber;
		if( m_dmMode[pDI->iColumn].bNumber )
		{
			chNumber = strText.GetAt(0);
			if( chNumber=='-' || (chNumber>='0'&&chNumber<='9') )
			{
				int iLength = strText.GetLength();
				for( int i = 1; i < iLength; i ++ )
				{
					chNumber = strText.GetAt(i);
					if( (chNumber>='0'&&chNumber<='9') )
						continue;
					bNumber = FALSE;
					break;
				}
			}
			else
				bNumber = FALSE;
			if( !bNumber )
			{
				AfxMessageBox("请输入一个整数！");
				return 0;
			}
		}
		if( m_dmMode[pDI->iColumn].bFloat )
		{
			chNumber = strText.GetAt(0);
			if( chNumber=='-' || chNumber=='.' || (chNumber>='0'&&chNumber<='9') )
			{
				if( chNumber=='.' )
					bDot = TRUE;
				int iLength = strText.GetLength();
				for( int i = 1; i < iLength; i ++ )
				{
					chNumber = strText.GetAt(i);
					if( (chNumber>='0'&&chNumber<='9') )
						continue;
					else if( chNumber=='.' )
					{
						if( !bDot )
							bDot = TRUE;
						else
						{
							bNumber = FALSE;
							break;
						}
					}
					bNumber = FALSE;
					break;
				}
			}
			else
				bNumber = FALSE;
			if( !bNumber )
			{
				AfxMessageBox("请输入一个浮点数字！");
				return 0;
			}
		}


		SetItemText( m_iItem,m_iSubItem,strText );
	}
	return 0;
}

void CDynamicListCtrl::EnableEdit(int nColumn, DynamicDDV mode)
{
	if( 0 > nColumn || DYNAMIC_MAX_COL < nColumn )
		return;
	m_dmMode[nColumn] = mode;
}

void CDynamicListCtrl::SetNormalColor(COLORREF crText,COLORREF crBack)
{
	m_crNormal[0] = crText;
	m_crNormal[1] = crBack;
	if( ::IsWindow( m_hWnd ) )RedrawWindow();
}

void CDynamicListCtrl::SetSelectedColor(COLORREF crText,COLORREF crBack)
{
	m_crSelected[0] = crText;
	m_crSelected[1] = crBack;
	if( ::IsWindow( m_hWnd ) )RedrawWindow();
}

void CDynamicListCtrl::SetDisalbedColor(COLORREF crText,COLORREF crBack)
{
	m_crDisabled[0] = crText;
	m_crDisabled[1] = crBack;
	if( ::IsWindow( m_hWnd ) )RedrawWindow();
}

void CDynamicListCtrl::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int iColumn = pNMListView->iSubItem;

	// if it's a second click on the same column then reverse the sort order,
	// otherwise sort the new column in ascending order.
	m_bSortAscending = iColumn == m_iSortColumn ? !m_bSortAscending : TRUE;
	m_iSortColumn = iColumn;

	if( SortItems( CompareFunction, reinterpret_cast<DWORD>( this ) ) )
		;
//		m_ctlHeader.SetSortArrow( m_iSortColumn, m_bSortAscending );

	*pResult = 0;
}

int CALLBACK CDynamicListCtrl::CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData )
{
	CDynamicListCtrl* pListCtrl = reinterpret_cast<CDynamicListCtrl*>( lParamData );
	ASSERT( pListCtrl->IsKindOf( RUNTIME_CLASS( CListCtrl ) ) );

	// 要求 ItemData = index + 1
	// 连续点击两次，lParam1,lParam2的大小关系是相反的
	if( lParam1 == lParam2 )return 0;

	CString strItem1,strItem2;

	int nIndex;
	LVFINDINFO info;
	info.flags = LVFI_PARAM;

	if( lParam1 )
	{
		info.lParam = lParam1;
		if ( (nIndex=pListCtrl->FindItem(&info)) != -1) 
		   strItem1 = pListCtrl->GetItemText(nIndex, pListCtrl->m_iSortColumn);
	}
	if( lParam2 )
	{
		info.lParam = lParam2;
		if ( (nIndex=pListCtrl->FindItem(&info)) != -1) 
		   strItem2 = pListCtrl->GetItemText(nIndex, pListCtrl->m_iSortColumn);
	}
	nIndex = strcmp( strItem2,strItem1 );
	if( pListCtrl->m_bSortAscending )
		nIndex = - nIndex;
	return nIndex;
}

int CDynamicListCtrl::GetSelectedEnabledCount()
{
	int nCount = GetSelectedCount();
	if( 0 == nCount )return 0;

	int nIndex = -1;
	do
	{
		nIndex = GetNextItem( nIndex,LVNI_SELECTED );
		if( -1 == nIndex )break;
		if( m_bCheckData && NULL == GetItemData( nIndex ) )
			nCount --;
	}while( true );

	return nCount;
}

int CDynamicListCtrl::GetNextEnabledItem(int index)
{
	do
	{
		index = GetNextItem( index,LVNI_SELECTED );
		if( -1 == index )break;
		if( m_bCheckData && NULL == GetItemData( index ) )
			continue;
		else
			break;
	}while( true );
	return index;
}

BOOL CDynamicListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( WM_KEYDOWN == pMsg->message )
	{
		switch( pMsg->wParam )
		{
		case 'A':
			{
				// 选中所有
				DWORD dwStyle;
				dwStyle = GetStyle();
				if( LVS_SINGLESEL != (dwStyle&LVS_SINGLESEL) )
				{
					short state = GetKeyState( VK_CONTROL );
					if( 0x01 & HIBYTE( state ) )
					{
						int i;
						for( i = GetItemCount()-1; i >= 0; i -- )
						{
							if( m_bCheckData && NULL==GetItemData(i) )
								continue;
							SetItemState( i,LVIS_SELECTED,LVIS_SELECTED );
						}
					}
				}
			}
			break;

		case VK_TAB:
			if( IsEditing() )
			{
				// 如果处于编辑中，跳到下一个编辑
				// SHIFT按下的话，反跳
				BOOL bBack = FALSE;
				short state = GetKeyState( VK_SHIFT );
				if( 0x01 & HIBYTE( state ) )
					bBack = TRUE;

				int iCount,iSubCount;
				iCount = GetItemCount();
				iSubCount = GetHeaderCtrl()->GetItemCount();

				int iItem,iSubItem;
				iItem = m_iItem;
				iSubItem = m_iSubItem;
				while( true )
				{
					if( TRUE==bBack )
					{
						iSubItem --;
						if( iSubItem<0 )
						{
							iItem --;
							if( iItem < 0 )
								iItem = iCount-1;
							iSubItem = iSubCount-1;
						}
					}
					else
					{
						iSubItem ++;
						if( iSubItem>iSubCount-1 )
						{
							iItem ++;
							if( iItem>iCount-1 )
								iItem = 0;
							iSubItem = 0;
						}
					}

					if( ( modeReadOnly != m_dmMode[iSubItem].mode ) && 
						( FALSE==m_bCheckData || NULL!=GetItemData(iItem) ) )
					{
						if( EditLabel(iItem,iSubItem) )
							return TRUE;
					}
					if( iItem==m_iItem && iSubItem==m_iSubItem )
						break;
				}
			}
			break;
		}
	}
	if( m_bEdit )
	{
		if( !CListCtrl::PreTranslateMessage(pMsg) )
			return PreTranslateInput(pMsg);
		return TRUE;
	}
	
	return CListCtrl::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl window
CSortHeaderCtrl::CSortHeaderCtrl()
	: m_iSortColumn( -1 )
	, m_bSortAscending( TRUE )
{
}

CSortHeaderCtrl::~CSortHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortHeaderCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl message handlers

void CSortHeaderCtrl::SetSortArrow( const int iSortColumn, const BOOL bSortAscending )
{
	m_iSortColumn = iSortColumn;
	m_bSortAscending = bSortAscending;

	// change the item to owner drawn.
	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;
	VERIFY( GetItem( iSortColumn, &hditem ) );
	hditem.fmt |= HDF_OWNERDRAW;
	VERIFY( SetItem( iSortColumn, &hditem ) );

	// invalidate the header control so it gets redrawn
	Invalidate();
}


void CSortHeaderCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	// attath to the device context.
	CDC dc;
	VERIFY( dc.Attach( lpDrawItemStruct->hDC ) );

	// save the device context.
	const int iSavedDC = dc.SaveDC();

	// get the column rect.
	CRect rc( lpDrawItemStruct->rcItem );

	// draw the background,
	CBrush brush( GetSysColor( COLOR_3DFACE ) );
	dc.FillRect( rc, &brush );

	// get the column text and format.
	TCHAR szText[ 256 ];
	HD_ITEM hditem;

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = szText;
	hditem.cchTextMax = 255;

	VERIFY( GetItem( lpDrawItemStruct->itemID, &hditem ) );

	// determine the format for drawing the column label.
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS ;

	if( hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if( hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	// adjust the rect if the mouse button is pressed on it.
	if( lpDrawItemStruct->itemState == ODS_SELECTED )
	{
		rc.left++;
		rc.top += 2;
		rc.right++;
	}

	CRect rcIcon( lpDrawItemStruct->rcItem );
	const int iOffset = ( rcIcon.bottom - rcIcon.top ) / 4;

	// adjust the rect further if the sort arrow is to be displayed.
	if( lpDrawItemStruct->itemID == (UINT)m_iSortColumn )
		rc.right -= 3 * iOffset;

	rc.left += iOffset;
	rc.right -= iOffset;

	// draw the column label.
	if( rc.left < rc.right )
		(void)dc.DrawText( szText, -1, rc, uFormat );

	// draw the sort arrow.
	if( lpDrawItemStruct->itemID == (UINT)m_iSortColumn )
	{
		// set up the pens to use for drawing the arrow.
		CPen penLight( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );
		CPen penShadow( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );
		CPen* pOldPen = dc.SelectObject( &penLight );

		if( m_bSortAscending )
		{
			// draw the arrow pointing upwards.
			dc.MoveTo( rcIcon.right - 2 * iOffset, iOffset);
			dc.LineTo( rcIcon.right - iOffset, rcIcon.bottom - iOffset - 1 );
			dc.LineTo( rcIcon.right - 3 * iOffset - 2, rcIcon.bottom - iOffset - 1 );
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo( rcIcon.right - 3 * iOffset - 1, rcIcon.bottom - iOffset - 1 );
			dc.LineTo( rcIcon.right - 2 * iOffset, iOffset - 1);
		}
		else
		{
			// draw the arrow pointing downwards.
			dc.MoveTo( rcIcon.right - iOffset - 1, iOffset );
			dc.LineTo( rcIcon.right - 2 * iOffset - 1, rcIcon.bottom - iOffset );
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo( rcIcon.right - 2 * iOffset - 2, rcIcon.bottom - iOffset );
			dc.LineTo( rcIcon.right - 3 * iOffset - 1, iOffset );
			dc.LineTo( rcIcon.right - iOffset - 1, iOffset );		
		}

		// restore the pen.
		(void)dc.SelectObject( pOldPen );
	}

	// restore the previous device context.
	VERIFY( dc.RestoreDC( iSavedDC ) );

	// detach the device context before returning.
	(void)dc.Detach();
}

void CDynamicListCtrl::Refresh()
{
	SortItems( CompareFunction, reinterpret_cast<DWORD>( this ) );
}


void CDynamicListCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// TODO: Add your code to draw the specified item   
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTVIEW);  
	CDC dc;   
	dc.Attach(lpDrawItemStruct->hDC);  
	ASSERT(NULL != dc.GetSafeHdc());  
	 // Save these value to restore them when done drawing.   
	COLORREF crOldTextColor = dc.GetTextColor();  
	COLORREF crOldBkColor = dc.GetBkColor();  
	   
	// If this item is selected, set the background color    
	// and the text color to appropriate values. Also, erase   
	// rect by filling it with the background color.   
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&  (lpDrawItemStruct->itemState & ODS_SELECTED))  
	{  
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));  
		dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));  
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));  
	}  
	else  
	{  
	    if(lpDrawItemStruct->itemID%2)  
		   dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(128,128,128));  
		else  
		   dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(255,128,255));  
	}  
	
	 // If this item has the focus, draw a red frame around the   
	 // item's rect.   
	 if ((lpDrawItemStruct->itemAction | ODA_FOCUS) && (lpDrawItemStruct->itemState & ODS_FOCUS))  
	 {  
		  CBrush br(RGB(0, 0, 128));  
		  dc.FrameRect(&lpDrawItemStruct->rcItem, &br);  
	 }    
	 // Draw the text.   
	 CString strText(_T(""));  
	 CRect rcItem;  
	  
	for(int i=0; i<GetHeaderCtrl()->GetItemCount(); i++)  
	{  
		strText = GetItemText(lpDrawItemStruct->itemID, i);  
		GetSubItemRect(lpDrawItemStruct->itemID, i, LVIR_LABEL, rcItem);  
		rcItem.left += 5;  
		dc.DrawText(strText, strText.GetLength(), &rcItem, DT_LEFT|DT_SINGLELINE|DT_VCENTER);  
	}     
	// Reset the background color and the text color back to their   
	// original values.   
	dc.SetTextColor(crOldTextColor);  
	dc.SetBkColor(crOldBkColor);    
	dc.Detach();  
	
	// 调用   
	  
	#define IDC_LIST 0x1101   
	Create(WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|LVS_OWNERDRAWFIXED, CRect(0, 0, 280, 280), this, IDC_LIST);  
	ModifyStyle(0, LVS_REPORT|LVS_SINGLESEL);  
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
	InsertColumn(0, _T("AAA"), LVCFMT_LEFT, 100);  
	InsertColumn(1, _T("BBB"), LVCFMT_LEFT, 100);  
	
	for(int i=0; i<20; i++)  
	{  
	  InsertItem(i, _T(""));  
	  strText.Format(_T("%d - Hello, World!"), i+1);  
	  SetItemText(i, 0, strText);  
	  strText.Format(_T("%d - ABCDEFG"), i+1);  
	  SetItemText(i, 1, strText);  
	  CListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}

}


