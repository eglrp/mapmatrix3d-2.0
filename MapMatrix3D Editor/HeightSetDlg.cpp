// HeightSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "HeightSetDlg.h"
#include "afxdialogex.h"


// CHeightSetDlg 对话框

IMPLEMENT_DYNAMIC(CHeightSetDlg, CDialogEx)

CHeightSetDlg::CHeightSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHeightSetDlg::IDD, pParent)
	, res1(_T(""))
{

}

CHeightSetDlg::~CHeightSetDlg()
{
}

void CHeightSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, res1);
}


BEGIN_MESSAGE_MAP(CHeightSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CHeightSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CHeightSetDlg 消息处理程序


void CHeightSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (res1.IsEmpty() )
	{
		return;
	}
	res = atof(res1);
	UpdateData(FALSE);
	CDialogEx::OnOK();
}
