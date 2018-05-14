#pragma once
#include "boost/signals2/signal.hpp"
#include "InterfaceUserEventCallBack.h"
#include "qcomm.h"
//返回值处理

class SigEventCallBack : public InterfaceSigEventCallBack
{
public:
	SigEventCallBack(void);
	~SigEventCallBack(void);

public:
	boost::signals2::signal<void ()> sigVoidFunStartProgress;

	virtual boost::signals2::signal<void ()>* ObtainSigStartProgress()
	{
		return &sigVoidFunStartProgress;
	}

	virtual void EmitSigStartProgressFun()
	{
		sigVoidFunStartProgress();
	}

public:
	boost::signals2::signal<void ()> sigVoidFunEndProgress;

	virtual boost::signals2::signal<void ()>* ObtainSigEndProgress()
	{
		return &sigVoidFunEndProgress;
	}

	virtual void EmitSigEndProgressFun()
	{
		sigVoidFunEndProgress();
	}

public:
	boost::signals2::signal<void (int)> sigSetProgress;

	virtual boost::signals2::signal<void (int)>* ObtainSigSetProgress()
	{
		return &sigSetProgress;
	}

	virtual void EmitSigSetProgressFun(int pos)
	{
		sigSetProgress(pos);
	}

public:
	boost::signals2::signal<void (int, double, double, double, bool)> sigObtainRelativePoint;

	virtual boost::signals2::signal<void (int, double, double, double, bool)>* ObtainSigRelativePt()
	{
		return &sigObtainRelativePoint;
	}

	virtual void EmitSigObtainRelativePoint(int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt)
	{
		sigObtainRelativePoint(ptNum, xRelaPt, yRelaPt, zRelaPt, isCheckPt);
	}

public:
	boost::signals2::signal<int (), result_handler<std::vector<int>>> sigGetSelPtNumFromAbsDlg;

	virtual boost::signals2::signal<int (), result_handler<std::vector<int>>>* ObtainSigGetSelPtNumFromAbsDlg()
	{
		return &sigGetSelPtNumFromAbsDlg;
	}

	virtual int EmitSigGetSelPtNumFromAbsDlg()
	{
		std::vector<int> selPtNum = sigGetSelPtNumFromAbsDlg();
		return selPtNum[0];
	}

public:
	boost::signals2::signal<bool (), result_handler<std::vector<bool>> > sigChangeCtrlList;

	virtual boost::signals2::signal<bool (), result_handler<std::vector<bool>>>* ObtainSigChangeCtrlList()
	{
		return &sigChangeCtrlList;
	}

	virtual bool EmitSigChangeCtrlList()
	{
		std::vector<bool> isChangeCtrlPtList = sigChangeCtrlList();
		return isChangeCtrlPtList[0];
	}

public:
	boost::signals2::signal<int  (), result_handler<std::vector<int>>> sigGetSelRowFromAbsDlg;

	virtual boost::signals2::signal<int  (), result_handler<std::vector<int>>>* ObtainSigGetSelRowFromAbsDlg()
	{
		return &sigGetSelRowFromAbsDlg;
	}

	virtual int EmitSigGetSelRowFromAbsDlg()
	{
		std::vector<int> selRow = sigGetSelRowFromAbsDlg();
		return selRow[0];
	}

public:
	boost::signals2::signal<int (), result_handler<std::vector<int>>> sigSelModelIndex;

	virtual boost::signals2::signal<int (), result_handler<std::vector<int>>>* ObtainSigSelModelIndex()
	{
		return &sigSelModelIndex;
	}

	virtual int EmitSigSelModelIndex()
	{
		std::vector<int> selModel = sigSelModelIndex();

		return selModel[0];
	}

public:
	boost::signals2::signal<void  (char*)> sigNotifyMeasureMsg;

	virtual boost::signals2::signal<void  (char*)>* ObtainSigNotifyMeasureMsg()
	{
		return &sigNotifyMeasureMsg;
	}

	virtual void EmitSigNotifyMeasureMsg(char* strMsg)
	{
		sigNotifyMeasureMsg(strMsg);
	}

public:
	boost::signals2::signal<void  (void)> sigCloseMeasureDlg;

	virtual boost::signals2::signal<void  (void)>* ObtainSigCloseMeasureDlg()
	{
		return &sigCloseMeasureDlg;
	}

	virtual void EmitSigCloseMeasureDlg()
	{
		sigCloseMeasureDlg();
	}
public:
	boost::signals2::signal<void  (void)> sigCloseAbsDlg;

	virtual boost::signals2::signal<void  (void)>* ObtainSigCloseAbsDlg()
	{
		return &sigCloseAbsDlg;
	}

	virtual void EmitSigCloseAbsDlg()
	{
		sigCloseAbsDlg();
	}
public:
	boost::signals2::signal<void ()> sigRemoveAllScene;

	virtual boost::signals2::signal<void ()>* ObtainSigRemoveAllScene()
	{
		return &sigRemoveAllScene;
	}

	virtual void EmitSigRemoveAllScene()
	{
		sigRemoveAllScene();
	}

public:
	boost::signals2::signal<void (char*)> sigAddScene;

	virtual boost::signals2::signal<void (char*)>* ObtainSigAddScene()
	{
		return &sigAddScene;
	}

	virtual void EmitSigAddScene(char* strName)
	{
		sigAddScene(strName);
	}

public:
	boost::signals2::signal<void (int, double, double, double)> sigInsertCtrlListRecord;

	virtual boost::signals2::signal<void (int, double, double, double)>* ObtainSigInsertCtrlListRecord()
	{
		return &sigInsertCtrlListRecord;
	}

	virtual void EmitSigInsertControlListRecord(int ptNum, double ptX, double ptY, double ptZ)
	{
		sigInsertCtrlListRecord(ptNum, ptX, ptY, ptZ);
	}

public:
	boost::signals2::signal<void (char*)> sigShowOpsHint;

	virtual boost::signals2::signal<void (char*)>* ObtainSigShowOpsHint()
	{
		return &sigShowOpsHint;
	}

	virtual void EmitSigShowOpsHint(char* strHint)
	{
		sigShowOpsHint(strHint);
	}

public:
	boost::signals2::signal<void (char*)> sigLinkPropertyListDlg;

	virtual boost::signals2::signal<void (char*)>* ObtainSigLinkPropertyListDlg()													//获得单体化关联属性菜单
	{
		return &sigLinkPropertyListDlg;
	}

	virtual void EmitSigLinkPropertyListDlg(char* sigPropertyListName)														//发送关联属性菜单信息
	{
		sigLinkPropertyListDlg(sigPropertyListName);
	}

public:
	boost::signals2::signal<void(int, int)> sigObjectMenuDlg;

	virtual boost::signals2::signal<void(int, int)>* obtainSigObjectMenuDlg()												//获取弹出单体化菜单的对话框
	{
		return &sigObjectMenuDlg;
	}

	virtual void EmitSigObjectMenuDlg(int mouseX, int mouseY)																//发送单体化菜单的信号
	{
		sigObjectMenuDlg(mouseX, mouseY);
	}
};

