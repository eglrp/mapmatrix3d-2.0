#pragma once


// DLGHatchSetting 对话框

class DLGHatchSetting : public CDialogEx
{
	DECLARE_DYNAMIC(DLGHatchSetting)

public:
	DLGHatchSetting(double step = 10, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DLGHatchSetting();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET_HATCH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double mStep;
};
