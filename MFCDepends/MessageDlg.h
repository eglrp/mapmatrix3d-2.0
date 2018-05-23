#pragma once
#include <string>
#include "resource.h"
// CMessageDlg 对话框

class CMessageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(std::string strTipInfo,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMessageDlg();

	// 对话框数据
	enum { IDD = IDD_DIALOG_MESSAGE };
	void ShowMsgWindow(); 
	void CenterScreen();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();  
	virtual BOOL PreTranslateMessage(MSG* pMsg);  

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

private:  
	std::string m_strTipInfo;    
	BYTE m_bAlpha;//淡入淡出透明效果 
public:
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
