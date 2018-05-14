// PropertyListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "PropertyListDlg.h"
#include "afxdialogex.h"
#include "qcomm.h"


// CPropertyListDlg dialog
CPropertyListDlg* g_mfcPropertyListDlg = NULL;

IMPLEMENT_DYNAMIC(CPropertyListDlg, CDialogEx)

CPropertyListDlg::CPropertyListDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ID_DIALOG_PROPERTY_LIST, pParent)
{
	g_mfcPropertyListDlg = this;
}

CPropertyListDlg::~CPropertyListDlg()
{
}

void CPropertyListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, dynamicCtrlList);
	DDX_Control(pDX, IDC_EDIT1, dynamicEditContrl);
}


BEGIN_MESSAGE_MAP(CPropertyListDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPropertyListDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPropertyListDlg message handlers

void CPropertyListDlg::CenterScreen()
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


BOOL CPropertyListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CenterScreen();
	CRgn rgnTmp;
	RECT rc;
	GetClientRect(&rc);

	int borderWidth = 8;
	rgnTmp.CreateRoundRectRgn(rc.left + borderWidth, rc.top + borderWidth, rc.right - rc.left - borderWidth, rc.bottom - rc.top - borderWidth + 10, borderWidth * 2, borderWidth * 2);
	SetWindowRgn(rgnTmp, TRUE);

	LONG lStyle;
	lStyle = GetWindowLong(m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = dynamicCtrlList.GetExtendedStyle();			//添加列表框的网格线
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_REPORT;					// | LVS_OWNERDRAWFIXED;
	dynamicCtrlList.SetExtendedStyle(dwStyle);

	CRect rect;
	dynamicCtrlList.GetClientRect(rect);
	dynamicCtrlList.InsertColumn(0, _T("属性"), LVCFMT_LEFT, rect.Width() / 3);
	dynamicCtrlList.InsertColumn(1, _T("值"), LVCFMT_LEFT, rect.Width() / 3 * 2);

	for (int i = 0; i < 20; i++)
	{
		dynamicCtrlList.InsertItem(i, _T(" "));
		dynamicCtrlList.SetItemText(i, 1, _T(" "));
	}

	dynamicEditContrl.ShowWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


bool CPropertyListDlg::ExtractSubPropertyListText(std::string propertyList, std::string &subPropertyList)
{
	int pos = propertyList.find("object");
	
	if (pos == std::string::npos)
	{
		return false;
	}
	
	pos += 7;

	int endPos = propertyList.length();


	int length = endPos - pos;

	if (length <= 0)
	{
		return false;
	}

	subPropertyList = propertyList.substr(pos, length);

	return true;
}

void CPropertyListDlg::SetPropertyListName(std::string strPropertyList)
{	
	if (strPropertyList.size() == 0)
	{
		return;
	}
	
	//去除实际有效文本段
	std::string subPropertyList("");

	if (strPropertyList.size() >= 9)
	{
		ExtractSubPropertyListText(strPropertyList, subPropertyList);
	}

	if (subPropertyList.size() == 0)
	{
		return;
	}

	std::vector<CPropertyListText> vecPropertyName;
	ExtractPropertyListName(subPropertyList, vecPropertyName);
	
	for (int i = 0; i < vecPropertyName.size(); i++)
	{
		std::string firstWord = vecPropertyName[i].strFirstWord;
		std::string secondWord = vecPropertyName[i].strSecondWord;
		CString strFirstWord = firstWord.c_str();
		CString strSecondWord = secondWord.c_str();

		if (strFirstWord != " " && strSecondWord != " ")
		{
			dynamicCtrlList.SetItemText(i, 0, strFirstWord);
			dynamicCtrlList.SetItemText(i, 1, strSecondWord);
		}
	}
}


void CPropertyListDlg::ExtractPropertyListName(std::string propertyListName, std::vector<CPropertyListText> &vecPropertyName)
{
	std::vector<int> vecPos;
	FindSpacePos(propertyListName, vecPos);
	ExtractWord(propertyListName, vecPropertyName, vecPos);
}


void CPropertyListDlg::FindSpacePos(std::string propertyName, std::vector<int> &vecSpacePos)
{
	int pos = propertyName.find(";");
	vecSpacePos.push_back(pos);

	while (pos != std::string::npos)
	{
		int SecondPos = propertyName.find(";", pos + 1);

		if (SecondPos != std::string::npos)
		{
			vecSpacePos.push_back(SecondPos);
		}

		pos = SecondPos;
	}
}

void CPropertyListDlg::ExtractWord(std::string propertyName, std::vector<CPropertyListText> &vecPropertyName, std::vector<int> vecPos)
{
	int firstPos = 0;

	for (int i = 0; i < vecPos.size(); i += 2)
	{
		int midPos = vecPos[i];
		int lasPos = vecPos[i + 1];

		std::string firstWord = propertyName.substr(firstPos, midPos - firstPos);
		std::string lastWord = propertyName.substr(midPos + 1, lasPos - midPos - 1);

		firstPos = lasPos + 1;

		CPropertyListText text;
		text.strFirstWord = firstWord;
		text.strSecondWord = lastWord;
		vecPropertyName.push_back(text);
	}
}

void CPropertyListDlg::OnBnClickedOk()
{
	std::string strPropertyListText("");
	// TODO: Add your control notification handler code here
	int itemCount = dynamicCtrlList.GetItemCount();
	CString strPropertyList = "object ";

	for (int i = 0; i < itemCount; i++)
	{
		CString strItemText = dynamicCtrlList.GetItemText(i, 0);
		CString strItemSubText = dynamicCtrlList.GetItemText(i, 1);

		if (strItemText != _T(" ") && strItemSubText != _T(" "))
		{
			strPropertyList += strItemText + _T(";") + strItemSubText + _T(";");
		}
	}

	strPropertyListText = strPropertyList;
	theApp.IOsgMfcViewer->ObtainMonomerEventSlot()->setObjectProperty(strPropertyListText);

	CDialogEx::OnOK();
}
