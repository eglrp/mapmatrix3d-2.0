// DlgSetContour.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "DlgSetContour.h"
#include "afxdialogex.h"


// DlgSetContour 对话框

IMPLEMENT_DYNAMIC(DlgSetContour, CDialogEx)

DlgSetContour::DlgSetContour(double zmax,double zmin,CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SET_CONTOUR, pParent)
	, m_zmax(zmax)
	, m_zmin(zmin)
	, m_zstart(zmin)
	, m_zend(zmax)
	, m_zspan(0.1)
{

}

DlgSetContour::~DlgSetContour()
{
}

void DlgSetContour::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ZMAX, m_zmax);
	DDX_Text(pDX, IDC_EDIT_ZMIN, m_zmin);
	DDX_Text(pDX, IDC_EDIT_ZSTART, m_zstart);
	DDX_Text(pDX, IDC_EDIT_ZEND, m_zend);
	DDX_Text(pDX, IDC_EDIT_ZSPAN, m_zspan);
}


BEGIN_MESSAGE_MAP(DlgSetContour, CDialogEx)
END_MESSAGE_MAP()


// DlgSetContour 消息处理程序
