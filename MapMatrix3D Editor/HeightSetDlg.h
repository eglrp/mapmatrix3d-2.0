#pragma once


// CHeightSetDlg 对话框

class CHeightSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHeightSetDlg)

public:
	CHeightSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHeightSetDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SET_HEIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString res1;
	double res;
	afx_msg void OnBnClickedOk();
};
