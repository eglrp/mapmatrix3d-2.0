#pragma once
#include "resource.h"
#include "afxcmn.h"

// CSetPtCloudDlg 对话框

class CSetPtCloudDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetPtCloudDlg)

public:
	CSetPtCloudDlg(double angle, double alpha, double color, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetPtCloudDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET_PTCLOUD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_Slider_Angle;
	CSliderCtrl m_Slider_Color;
	CSliderCtrl m_Slider_Alpha;
	double m_angle;
	double m_color;
	double m_alpha;
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
