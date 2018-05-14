#pragma once
#include "resource.h"

// CDOMResSetDlg 对话框

class CDOMResSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDOMResSetDlg)

public:
	CDOMResSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDOMResSetDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SET_DOM_RES };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString res1;
	double res;
};
