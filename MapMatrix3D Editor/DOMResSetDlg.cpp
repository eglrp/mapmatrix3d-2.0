// DOMResSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "DOMResSetDlg.h"
#include "afxdialogex.h"
#include "qcomm.h"

// CDOMResSetDlg 对话框

IMPLEMENT_DYNAMIC(CDOMResSetDlg, CDialogEx)

CDOMResSetDlg::CDOMResSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDOMResSetDlg::IDD, pParent)
	, res1(_T(""))
{
	res = 0;
}

CDOMResSetDlg::~CDOMResSetDlg()
{
}

void CDOMResSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, res1);
}


BEGIN_MESSAGE_MAP(CDOMResSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDOMResSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDOMResSetDlg 消息处理程序


void CDOMResSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	

	//CDialogEx::OnOK();
	UpdateData(TRUE);
	if (res1.IsEmpty() )
	{
		return;
	}
	res = atof(res1);
	UpdateData(FALSE);
	CDialogEx::OnOK();
}
