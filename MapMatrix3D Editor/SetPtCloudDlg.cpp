// SetPtCloudDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "SetPTCloudDlg.h"
#include "afxdialogex.h"


// CSetPtCloudDlg 对话框

IMPLEMENT_DYNAMIC(CSetPtCloudDlg, CDialogEx)

CSetPtCloudDlg::CSetPtCloudDlg(double angle, double alpha, double color, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SET_PTCLOUD, pParent)
	, m_angle(angle)
	, m_color(color)
	, m_alpha(alpha)
{

}

CSetPtCloudDlg::~CSetPtCloudDlg()
{
}

void CSetPtCloudDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_ANGLE, m_Slider_Angle);
	DDX_Control(pDX, IDC_SLIDER_COLOR, m_Slider_Color);
	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_Slider_Alpha);
	DDX_Text(pDX, IDC_STATIC_ANGLE, m_angle);
	DDX_Text(pDX, IDC_STATIC_COLOR, m_color);
	DDX_Text(pDX, IDC_STATIC_ALPHA, m_alpha);
}


BEGIN_MESSAGE_MAP(CSetPtCloudDlg, CDialogEx)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CSetPtCloudDlg 消息处理程序


BOOL CSetPtCloudDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Slider_Angle.SetRange(0, 900);//设置滑动范围为1到20
	m_Slider_Angle.SetTicFreq(10);//每1个单位画一刻度
	m_Slider_Angle.SetPos(m_angle * 10);//设置滑块初始位置为1 

	m_Slider_Alpha.SetRange(0, 49);
	m_Slider_Alpha.SetTicFreq(10);
	m_Slider_Alpha.SetPos(m_alpha * 10 - 1);

	m_Slider_Color.SetRange(0, 100);
	m_Slider_Color.SetTicFreq(10);
	m_Slider_Color.SetPos(m_color * 100);
	UpdateData(TRUE);
	CSliderCtrl   *pSlidCtrlAlpha = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ALPHA);
	CSliderCtrl   *pSlidCtrlAngle = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE);
	CSliderCtrl   *pSlidCtrlSimilarity = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_COLOR);
	//m_int 即为当前滑块的值。
	int k;
	m_alpha = 0.1*pSlidCtrlAlpha->GetPos() + 0.1;//取得当前位置值
	k = static_cast<int>(m_alpha / 5 * 1000.0);
	m_alpha = k / 10.0;
	m_angle = 0.1*pSlidCtrlAngle->GetPos();
	k = static_cast<int>(m_angle / 90 * 1000.0);
	m_angle = k / 10.0;
	m_color = 0.01*pSlidCtrlSimilarity->GetPos();
	k = static_cast<int>(m_color / 1 * 1000.0);
	m_color = k / 10.0;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSetPtCloudDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(TRUE);
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl   *pSlidCtrlAlpha = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ALPHA);
	CSliderCtrl   *pSlidCtrlAngle = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE);
	CSliderCtrl   *pSlidCtrlSimilarity = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_COLOR);
	//m_int 即为当前滑块的值。
	int k;
	m_alpha = 0.1*pSlidCtrlAlpha->GetPos() + 0.1;//取得当前位置值
	k = static_cast<int>(m_alpha / 5 * 1000.0);
	m_alpha = k / 10.0;
	m_angle = 0.1*pSlidCtrlAngle->GetPos();
	k = static_cast<int>(m_angle / 90 * 1000.0);
	m_angle = k / 10.0;
	m_color = 0.01*pSlidCtrlSimilarity->GetPos();
	k = static_cast<int>(m_color / 1 * 1000.0);
	m_color = k / 10.0;
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateData(FALSE);
}
