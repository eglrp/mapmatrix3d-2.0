// LookUpObjectPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "LookUpObjectPropertyDlg.h"
#include "afxdialogex.h"


// CLookUpObjectPropertyDlg dialog

IMPLEMENT_DYNAMIC(CLookUpObjectPropertyDlg, CDialogEx)

CLookUpObjectPropertyDlg::CLookUpObjectPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_LOOKUP_PROPERTY, pParent)
{

}

CLookUpObjectPropertyDlg::~CLookUpObjectPropertyDlg()
{
}

void CLookUpObjectPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLookUpObjectPropertyDlg, CDialogEx)
	ON_BN_CLICKED(IDLOOKUP, &CLookUpObjectPropertyDlg::OnBnClickedLookup)
END_MESSAGE_MAP()


// CLookUpObjectPropertyDlg message handlers


void CLookUpObjectPropertyDlg::OnBnClickedLookup()
{
	// TODO: Add your control notification handler code here
	CString strPropertyName, strPropertyValue;
	GetDlgItem(IDC_EDIT_PROPERTY_NAME)->GetWindowTextA(strPropertyName);
	GetDlgItem(IDC_EDIT_PROPERTY_VALUE)->GetWindowText(strPropertyValue);

	std::string propertyName = strPropertyName;
	std::string propertyValue = strPropertyValue;
	theApp.IOsgMfcViewer->ObtainMonomerEventSlot()->lookUpOjbectProperty(propertyName, propertyValue);
}


void CLookUpObjectPropertyDlg::CenterScreen()
{
	//ShowWindow(SW_NORMAL);  
	CRect rtDesk;
	CRect rtDlg;
	::GetWindowRect(::GetDesktopWindow(), &rtDesk);
	GetWindowRect(&rtDlg);
	int iXpos = rtDesk.Width() * 2 / 3 - rtDlg.Width() / 2;
	int iYpos = rtDesk.Height() / 2 - rtDlg.Height() / 2;
	SetWindowPos(NULL, iXpos, iYpos, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
}



BOOL CLookUpObjectPropertyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CenterScreen();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
