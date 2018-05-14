// DLGHatchSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "DLGHatchSetting.h"
#include "afxdialogex.h"


// DLGHatchSetting 对话框

IMPLEMENT_DYNAMIC(DLGHatchSetting, CDialogEx)

DLGHatchSetting::DLGHatchSetting(double step, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SET_HATCH, pParent)
	, mStep(step)
{

}

DLGHatchSetting::~DLGHatchSetting()
{
}

void DLGHatchSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HATCH_STEP, mStep);
}


BEGIN_MESSAGE_MAP(DLGHatchSetting, CDialogEx)
END_MESSAGE_MAP()


// DLGHatchSetting 消息处理程序
