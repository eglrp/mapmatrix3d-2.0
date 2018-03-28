#pragma once


// DlgSetContour 对话框

class DlgSetContour : public CDialogEx
{
	DECLARE_DYNAMIC(DlgSetContour)

public:
	DlgSetContour(double zmax = 0,double zmin = 0, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgSetContour();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET_CONTOUR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_zmax;
	double m_zmin;
	double m_zstart;
	double m_zend;
	double m_zspan;
};
