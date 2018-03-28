#pragma once
#include "resource.h"



// CMeasureResultDlg 对话框

class CMeasureResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CMeasureResultDlg)

public:
	CMeasureResultDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMeasureResultDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_Measure_Result };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	CButton m_check_curve;
	CStatic m_Static_Measure_Result;
	std::vector<std::string> vecMeasureResult;
	std::string resultMsg;
	BOOL isSaved;
public:
	void notifyMeasureResult(char* strMsg);
	void closeDlg();
private:
//	afx_msg void OnTcnSelchangeTabMeasurePt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckCatch();
	
protected:
//	afx_msg LRESULT OnChangetext(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonOutMeasureResult();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSavedata();
};
