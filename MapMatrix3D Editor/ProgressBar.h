#pragma once
#include "resource.h"
#include "afxcmn.h"
// CProgressBar 对话框


class CProgressBar : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressBar)
public:
	CProgressBar(CWnd* pParent = NULL);   // 标准构造函数
	void ChangeStaticText(CString posStr);
	virtual ~CProgressBar();

// 对话框数据
	enum { IDD = IDD_DIALOG_PROGRESS_BAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	int pos;
	CProgressCtrl m_ProgressBar;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
