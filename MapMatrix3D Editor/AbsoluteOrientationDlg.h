#pragma once
#include "InterfaceOsgOrient.h"
#include <vector>
#include "resource.h"
#include "DynamicListCtrl.h"
#include "afxwin.h"


// CAbsoluteOrientationDlg 对话框

class CAbsoluteOrientationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAbsoluteOrientationDlg)

public:
	CAbsoluteOrientationDlg(CWnd* pParent, std::vector<CString> VecSceneFileName);   // 标准构造函数
	virtual ~CAbsoluteOrientationDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ABSOLUTE_ORIENTATION };

public:
	int getNumColumn();																								//获取列表行数				
	void setRelativePoint(int ptNum, double xCtrlPt, double yCtrlPt, double zCtrlPt, bool isCheckPt);				//设置相对坐标点
	void insertNewColumn(int ptNum, double xClickPt, double yClickPt, double zClickPt);								//插入新行
	int  ObtainSelectedRow();																						//获取选择行
	//void ObtainImageControlPoint(double xImageCtrlPt, double yImageCtrlPt, double zImageCtrlPt, bool isCheckPt);  //获取影像控制点
	void ObtainModelRelaPoint(int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt);			//获取模型控制点
	CString ObtainSelectedPtNum();																					//获取选择点号
	bool InsertControlListCompleteStatus();																			//插入选择项完成状态	
	bool isAlreadyClicked();																							//是否已经刺过

	void InsertControlListRecord(int ptNum, double ptX, double ptY, double ptZ);									//列表插入记录
	void closeDlg();  //关闭对话框
public:
	//QyManipulatorMode qMode; 
	CDynamicComboBox m_ComboBox1;
	CDynamicListCtrl m_list_control_point;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	//int ObtainControlListRowText(std::vector<CoordDouble3D> &vecAbsoluteCoord, std::vector<CoordDouble3D> &vecRelaCoord);
	
	bool ChangeCompleteStatusByRow(int row);
	void ChangeCompleteStatusByPtNum(int ptNum);
	void ClearCompleteStatusInList();
private:
	afx_msg void OnBnClickedLoadControlPoint();
	afx_msg void OnBnClickedAbsoluteOrientaton();
	afx_msg void OnClose();
	afx_msg void OnSetAsCheckPoint();
	afx_msg void OnRclickListControlPoint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInsertRecord();
	afx_msg void OnReclickControlPoint();
	afx_msg void OnClickListControlPoint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSaveClickPtResult();
	afx_msg void OnBnClickedLoadClickPtResult();



public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
