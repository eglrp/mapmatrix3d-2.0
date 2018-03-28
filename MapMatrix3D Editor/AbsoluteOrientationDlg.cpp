
// AbsoluteOrientationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapMatrix3D Editor.h"
#include "AbsoluteOrientationDlg.h"
#include "SevenParameter.h"
#include <vector>
#include "ChildFrm.h"
#include "OSG_Plus.h"

CAbsoluteOrientationDlg* g_mfcAbsoluteOrientationDlg = NULL;


IMPLEMENT_DYNAMIC(CAbsoluteOrientationDlg, CDialogEx)

CAbsoluteOrientationDlg::CAbsoluteOrientationDlg(CWnd* pParent, std::vector<CString> VecSceneFileName)
	: CDialogEx(CAbsoluteOrientationDlg::IDD, pParent)
{
	g_mfcAbsoluteOrientationDlg = this;
}

CAbsoluteOrientationDlg::~CAbsoluteOrientationDlg()
{
}

void CAbsoluteOrientationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTROL_POINT, m_list_control_point);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox1);
}

BEGIN_MESSAGE_MAP(CAbsoluteOrientationDlg, CDialogEx)
	ON_BN_CLICKED(ID_LOAD_CONTROL_POINT, &CAbsoluteOrientationDlg::OnBnClickedLoadControlPoint)
	ON_BN_CLICKED(ID_ABSOLUTE_ORIENTATON, &CAbsoluteOrientationDlg::OnBnClickedAbsoluteOrientaton)
	ON_COMMAND(ID_SET_AS_CHECK_POINT, &CAbsoluteOrientationDlg::OnSetAsCheckPoint)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CONTROL_POINT, &CAbsoluteOrientationDlg::OnRclickListControlPoint)
	ON_COMMAND(ID_INSERT_RECORD, &CAbsoluteOrientationDlg::OnInsertRecord)
	ON_COMMAND(ID_Reclick_Control_Point, &CAbsoluteOrientationDlg::OnReclickControlPoint)
	ON_BN_CLICKED(ID_SAVE_CLICK_PT_RESULT, &CAbsoluteOrientationDlg::OnBnClickedSaveClickPtResult)
	ON_BN_CLICKED(ID_LOAD_CLICK_PT_RESULT, &CAbsoluteOrientationDlg::OnBnClickedLoadClickPtResult)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONTROL_POINT, &CAbsoluteOrientationDlg::OnClickListControlPoint)
END_MESSAGE_MAP()


// CAbsoluteOrientationDlg 消息处理程序

BOOL CAbsoluteOrientationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	LONG lStyle;
	lStyle = GetWindowLong(m_list_control_point.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_list_control_point.m_hWnd, GWL_STYLE, lStyle);//设置style


	DWORD dwStyle = m_list_control_point.GetExtendedStyle();			//添加列表框的网格线
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_list_control_point.SetExtendedStyle(dwStyle);

	DWORD id = GetThreadLocale();

	if (id != 1033)
	{
		// TODO:  在此添加额外的初始化
		m_list_control_point.InsertColumn(0, "控制点编号", LVCFMT_CENTER, 80);
		m_list_control_point.InsertColumn(1, "检查点", LVCFMT_CENTER, 60);
		m_list_control_point.InsertColumn(2, "坐标x", LVCFMT_CENTER, 100);
		m_list_control_point.InsertColumn(3, "坐标y", LVCFMT_CENTER, 100);
		m_list_control_point.InsertColumn(4, "坐标z", LVCFMT_CENTER, 100);
		m_list_control_point.InsertColumn(5, "刺点完成", LVCFMT_CENTER, 60);
	}
	else
	{
		m_list_control_point.InsertColumn(0, "CtrlPtNum", LVCFMT_CENTER, 80);
		m_list_control_point.InsertColumn(1, "CheckPt", LVCFMT_CENTER, 60);
		m_list_control_point.InsertColumn(2, "CoordX", LVCFMT_CENTER, 100);
		m_list_control_point.InsertColumn(3, "CoordY", LVCFMT_CENTER, 100);
		m_list_control_point.InsertColumn(4, "CoordZ", LVCFMT_CENTER, 100);
		m_list_control_point.InsertColumn(5, "ClickPtStatus", LVCFMT_CENTER, 60);
	}

	CRect rect;
	GetWindowRect(rect);
	ClientToScreen(rect); 
	int yMove = GetSystemMetrics(SM_CYMENU);
	CRect mainFrameRect;
	AfxGetMainWnd()->GetClientRect(mainFrameRect);
	MoveWindow(mainFrameRect.Width() - rect.Width(), yMove + 200, rect.Width(), rect.Height());

	DynamicDDV ddv;
	ddv.mode = modeEdit;
	m_list_control_point.EnableEdit(1, ddv);
	m_list_control_point.EnableEdit(2, ddv);
	m_ComboBox1.ShowWindow(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

int CAbsoluteOrientationDlg::getNumColumn()
{
	int numCol = m_list_control_point.GetItemCount();
	return numCol;
}

void CAbsoluteOrientationDlg::setRelativePoint(int ptNum, double xCtrlPt, double yCtrlPt, double zCtrlPt, bool isCheckPt)
{
	//for (int i = 0; i < absOrientor.vecRelaPt.size(); i++)
	//{
	//	if (ptNum == absOrientor.vecRelaPt[i].ptSerialNum)		//替换
	//	{
	//		absOrientor.vecRelaPt[i].ptSerialNum = ptNum;
	//		absOrientor.vecRelaPt[i].ptX = xCtrlPt;
	//		absOrientor.vecRelaPt[i].ptY = yCtrlPt;
	//		absOrientor.vecRelaPt[i].ptZ = zCtrlPt;
	//		absOrientor.vecRelaPt[i].isCheckPoint = isCheckPt;
	//		return;
	//	}
	//}

	////没有加入过，则加入
	//absOrientor.vecRelaPt.push_back(CControlPoint(ptNum, xCtrlPt, yCtrlPt, zCtrlPt, isCheckPt));
}

void CAbsoluteOrientationDlg::insertNewColumn(int ptNum, double xClickPt, double yClickPt, double zClickPt)
{
	/*std::vector<CControlPoint> vecCtrlPt = absOrientor.vecCtrlPt;

	for (int i = 0; i < vecCtrlPt.size(); i++)
	{
		if (ptNum == vecCtrlPt[i].ptSerialNum)
		{
			int numCol = m_list_control_point.GetItemCount();
			char isCheckPtStr[20] = "否";
			char number[20];
			sprintf_s(number, "%d", ptNum);
			char xClickStr[20];
			sprintf_s(xClickStr, "%lf", xClickPt);
			char yClickStr[20];
			sprintf_s(yClickStr, "%lf", yClickPt);
			char zClickStr[20];
			sprintf_s(zClickStr, "%lf", zClickPt);

			int nRow = m_list_control_point.InsertItem(numCol, number);
			m_list_control_point.SetItemText(nRow, 1, isCheckPtStr);
			m_list_control_point.SetItemText(nRow, 2, xClickStr);
			m_list_control_point.SetItemText(nRow, 3, yClickStr);
			m_list_control_point.SetItemText(nRow, 4, zClickStr);

			char numCtrlptStr[20];
			sprintf_s(numCtrlptStr, "%d", vecCtrlPt[i].ptSerialNum);
			char xCtrlPtStr[20];
			sprintf_s(xCtrlPtStr, "%lf", vecCtrlPt[i].ptX);
			char yCtrlPtStr[20];
			sprintf_s(yCtrlPtStr, "%lf", vecCtrlPt[i].ptY);
			char zCtrlPtStr[20];
			sprintf_s(zCtrlPtStr, "%lf", vecCtrlPt[i].ptZ);

			m_list_control_point.SetItemText(nRow, 5, numCtrlptStr);
			m_list_control_point.SetItemText(nRow, 6, xCtrlPtStr);
			m_list_control_point.SetItemText(nRow, 7, yCtrlPtStr);
			m_list_control_point.SetItemText(nRow, 8, zCtrlPtStr);
		}
	}*/
}

//void CAbsoluteOrientationDlg::ObtainImageControlPoint(double xImageCtrlPt, double yImageCtrlPt, double zImageCtrlPt, bool isCheckPt)
//{
//	/*int num = vecAbsPt.size();
//	vecAbsPt.push_back(CControlPoint(num, xImageCtrlPt, yImageCtrlPt, zImageCtrlPt, isCheckPt));*/
//}
//
void CAbsoluteOrientationDlg::ObtainModelRelaPoint(int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt)
{
	//IAbsOrient->ObtainModelRelativePoint(ptNum, xRelaPt, yRelaPt, zRelaPt, isCheckPt);
}

void CAbsoluteOrientationDlg::OnBnClickedLoadControlPoint()
{
	// TODO: 在此添加控件通知处理程序代码
	theApp.IOsgMfcViewer->ObtainOrientEventSlot()->loadControlPt();
}

void CAbsoluteOrientationDlg::OnBnClickedAbsoluteOrientaton()
{
	//把精度报告的输出路径设置为索引位置
	std::string outPrecisionFileName = theApp.vecSceneFileName[0];
	int pos = outPrecisionFileName.rfind('\\') + 1;
	std::string outPrecisionFilePath = outPrecisionFileName.substr(0, pos);
	//定向失败则返回
	if(!theApp.IOsgMfcViewer->ObtainOrientEventSlot()->absOrient(outPrecisionFilePath))
	{
		return;
	}
	//CDialogEx::OnOK();
}

void CAbsoluteOrientationDlg::closeDlg()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SendMessage(WM_CLOSE);
}

void CAbsoluteOrientationDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	std::vector<OperatingState> vecOPS;
	vecOPS.push_back(OPS_ABS_NONE);
	theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
	
	CDialogEx::OnClose();
}

void CAbsoluteOrientationDlg::OnSetAsCheckPoint()
{
	// TODO: 在此添加命令处理程序代码
	/*int row = ObtainSelectedRow();

	if(row < 0)
	{
		InterfaceMFCExcuteFactory IMfcExcuteFactory;
		InterfaceMFCExcute* IMfcExcute = IMfcExcuteFactory.create();
		IMfcExcute->PopUpMessage("设置检查点不成功");
		return;
	}

	CString selPtNumStr = ObtainSelectedPtNum();
	int selPtNum = atoi(selPtNumStr);

	static int i = 1; 

	if (i % 2 == 1)
	{
		IAbsOrient->SetControlPtStatus(selPtNum, true);
		m_list_control_point.SetItemText(row, 1, "是");
	}

	i++;*/
}

int CAbsoluteOrientationDlg::ObtainSelectedRow()
{
	int selectCol = -999;
	int colCount = m_list_control_point.GetItemCount();

	//寻找选中的点
	for(int i = 0; i < colCount; i++) 
	{ 
		if(m_list_control_point.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) 
		{ 
			selectCol = i;
		} 
	} 

	return selectCol;
}

CString CAbsoluteOrientationDlg::ObtainSelectedPtNum()
{
	int colCount = m_list_control_point.GetItemCount();
	CString strText;
	//寻找选中的点
	for(int i = 0; i < colCount; i++) 
	{ 
		if(m_list_control_point.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) 
		{ 
			strText = m_list_control_point.GetItemText(i, 0);
		} 
	} 

	return strText;
}

void CAbsoluteOrientationDlg::OnRclickListControlPoint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	DWORD dwPos = GetMessagePos(); 
	CPoint point(LOWORD(dwPos), HIWORD(dwPos)); 

	CMenu menu;
	menu.LoadMenuA(IDR_MENU4);				//加载菜单资源
	CMenu *pPopup=menu.GetSubMenu(0);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CAbsoluteOrientationDlg::OnInsertRecord()
{
	
}

void CAbsoluteOrientationDlg::InsertControlListRecord(int ptNum, double ptX, double ptY, double ptZ)
{													  
	int numCol = m_list_control_point.GetItemCount();
	DWORD id = GetThreadLocale();
	//控制点信息
	char numCtrlptStr[20];
	char isCheckPtStr[20];

	if (id != 1033)
	{
		sprintf_s(isCheckPtStr, "否");
	}
	else
	{
		sprintf_s(isCheckPtStr, "No");
	}

	sprintf_s(numCtrlptStr, "%d", ptNum);
	char xCtrlPtStr[20];
	sprintf_s(xCtrlPtStr, "%lf", ptX);
	char yCtrlPtStr[20];
	sprintf_s(yCtrlPtStr, "%lf", ptY);
	char zCtrlPtStr[20];
	sprintf_s(zCtrlPtStr, "%lf", ptZ);

	int nRow = m_list_control_point.InsertItem(numCol, numCtrlptStr);
	m_list_control_point.SetItemText(nRow, 1, isCheckPtStr);
	m_list_control_point.SetItemText(nRow, 2, xCtrlPtStr);
	m_list_control_point.SetItemText(nRow, 3, yCtrlPtStr);
	m_list_control_point.SetItemText(nRow, 4, zCtrlPtStr);
}	

bool CAbsoluteOrientationDlg::InsertControlListCompleteStatus()		//回调函数
{
	int row = ObtainSelectedRow();
	return ChangeCompleteStatusByRow(row);
}

bool CAbsoluteOrientationDlg::ChangeCompleteStatusByRow(int row)
{
	int numCol = m_list_control_point.GetItemCount();
	DWORD id = GetThreadLocale();

	char completeStatus[20];

	if (id != 1033)
	{
		sprintf_s(completeStatus, "完成");
	}
	else
	{
		sprintf_s(completeStatus, "Finish");
	}

	if (m_list_control_point.GetItemText(row, 5) == _T(""))
	{
		m_list_control_point.SetItemText(row, 5, completeStatus);
		return true;
	}
	else
	{
		return false;
	}
}

void CAbsoluteOrientationDlg::ClearCompleteStatusInList()
{
	int totalNum = m_list_control_point.GetItemCount();
	char completeStatus[20] = "";

	for (int i = 0; i < totalNum; i++)
	{
		m_list_control_point.SetItemText(i, 5, completeStatus);
	}
}

void CAbsoluteOrientationDlg::ChangeCompleteStatusByPtNum(int ptNum)
{
	int totalNum = m_list_control_point.GetItemCount();

	int numCol = m_list_control_point.GetItemCount();
	DWORD id = GetThreadLocale();
	char completeStatus[20];

	if (id != 1033)
	{
		 sprintf_s(completeStatus, "完成");
	}
	else
	{
		sprintf_s(completeStatus, "Finish");
	}

	for (int i = 0; i < totalNum; i++)
	{
		CString ptStr = m_list_control_point.GetItemText(i, 0);
		int Num = atoi(ptStr);

		if (Num == ptNum)
		{
			m_list_control_point.SetItemText(i, 5, completeStatus);
		}
	}
}

bool CAbsoluteOrientationDlg::isAlreadyClicked()
{
	int row = ObtainSelectedRow();

	if (m_list_control_point.GetItemText(row, 5) == _T(""))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CAbsoluteOrientationDlg::OnReclickControlPoint()
{
	osg::ref_ptr<osg::Group> root = theApp.IOsgMfcViewer->ObtainRoot();
	osg::ref_ptr<osg::Group> drawGroup = root->getChild(4)->asGroup();
	// TODO: 在此添加命令处理程序代码
	int row = ObtainSelectedRow();
	CString strNum = ObtainSelectedPtNum();
	int ptNum = atoi(strNum);

	theApp.IOsgMfcViewer->ObtainOrientEventSlot()->reClickCtrlPt(ptNum);

	char completeStatus[20] = "";

	DWORD id = GetThreadLocale();

	if (id != 1033)
	{
		if (m_list_control_point.GetItemText(row, 5) == _T("完成"))
		{
			m_list_control_point.SetItemText(row, 5, completeStatus);
		}
	}
	else
	{
		if (m_list_control_point.GetItemText(row, 5) == _T("Finish"))
		{
			m_list_control_point.SetItemText(row, 5, completeStatus);
		}
	}

	
	
	return;
}

void CAbsoluteOrientationDlg::OnBnClickedSaveClickPtResult()
{
	

}

void CAbsoluteOrientationDlg::OnBnClickedLoadClickPtResult()
{
	
}

void CAbsoluteOrientationDlg::OnClickListControlPoint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	DWORD dwPos = GetMessagePos(); 
	CPoint point(LOWORD(dwPos), HIWORD(dwPos)); 

	int row = ObtainSelectedRow();

	char completeStatus[20] = "";

	if (m_list_control_point.GetItemText(row, 5) == _T("完成"))
	{
		//std::vector<OperatingState> vecOPS;
		//vecOPS.push_back(OPS_None);
		//theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);
		//g_mfcOSG->mOpState->opState = OPS_None;
	}
	else
	{
		/*std::vector<OperatingState> vecOPS;
		vecOPS.push_back(OPS_ABSOLUTE_ORIENTATION);
		theApp.IOsgMfcViewer->AddViewerOPS(vecOPS);*/
		//g_mfcOSG->mOpState->opState = OPS_ABSOLUTE_ORIENTATION;
	}

}


BOOL CAbsoluteOrientationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE ) 
	{
		std::vector<OperatingState> vecOps;
		vecOps.push_back(OPS_None);
		theApp.IOsgMfcViewer->AddViewerOPS(vecOps);
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
