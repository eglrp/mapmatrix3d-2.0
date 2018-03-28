#pragma once
#include "boost/signals2/signal.hpp"

#ifdef USEREVENTLIBDLL  
#define USEREVENTAPI _declspec(dllexport)  
#else  
#define USEREVENTAPI  _declspec(dllimport)  
#endif  



template<typename T>
struct result_handler
{
	typedef T result_type;

	template<typename InputIterator>
	T operator() (InputIterator first,InputIterator last) const
	{
		return T(first,last);
	}
};


class USEREVENTAPI InterfaceSigEventCallBack
{
public:
	virtual boost::signals2::signal<void ()>* ObtainSigStartProgress() = 0;														//获得开始进度条信号

	virtual void EmitSigStartProgressFun() = 0;																					//发送开始进度条信号

	virtual boost::signals2::signal<void ()>* ObtainSigEndProgress() = 0;														//获得结束进度条信号

	virtual void EmitSigEndProgressFun() = 0;																					//发送结束进度条信号

	virtual	boost::signals2::signal<int (), result_handler<std::vector<int>>>* ObtainSigSelModelIndex() = 0;					//获取选择模型信号

	virtual int EmitSigSelModelIndex() = 0;																						//发送选择模型信号

	virtual boost::signals2::signal<void (int)>* ObtainSigSetProgress() = 0;													//获得设置进度的信号

	virtual void EmitSigSetProgressFun(int) = 0;																				//发送设置进度信号

	virtual boost::signals2::signal<void (int, double, double, double, bool)>* ObtainSigRelativePt() = 0;						//获得相对刺点信号

	virtual void EmitSigObtainRelativePoint(int ptNum, double xRelaPt, double yRelaPt, double zRelaPt, bool isCheckPt) = 0;		//发送相对点信号
																												
	virtual boost::signals2::signal<int (), result_handler<std::vector<int>>>* ObtainSigGetSelPtNumFromAbsDlg() = 0;			//获得绝对定向对话框的选择点号

	virtual int EmitSigGetSelPtNumFromAbsDlg() = 0;																				//发送获得绝对定向对话框的选择点号

	virtual boost::signals2::signal<int (), result_handler<std::vector<int>>>* ObtainSigGetSelRowFromAbsDlg() = 0;				//获取绝对定向对话框的选择行

	virtual int EmitSigGetSelRowFromAbsDlg() = 0;																				//发送获得绝对定向对话框的选择行

	virtual boost::signals2::signal<void  (char*)>* ObtainSigNotifyMeasureMsg() = 0;											//获得通知测量框信号

	virtual void EmitSigNotifyMeasureMsg(char* strMsg) = 0;																		//发送通知测量框信号

	virtual boost::signals2::signal<void  (void)>* ObtainSigCloseMeasureDlg() = 0;												//获得关闭测量框信号

	virtual void EmitSigCloseMeasureDlg() = 0;																					//发送关闭测量框信号

	virtual boost::signals2::signal<void  (void)>* ObtainSigCloseAbsDlg() = 0;													//获得关闭定向框信号

	virtual void EmitSigCloseAbsDlg() = 0;																						//发送关闭定向框信号

	virtual boost::signals2::signal<bool (), result_handler<std::vector<bool>>>* ObtainSigChangeCtrlList() = 0;					//获得改变控制列表信号

	virtual bool EmitSigChangeCtrlList() = 0;																					//发送改变控制列表信号

	virtual boost::signals2::signal<void ()>* ObtainSigRemoveAllScene() = 0;													//获得清除所有场景信号

	virtual void EmitSigRemoveAllScene() = 0;																					//发送清除所有场景信号

	virtual boost::signals2::signal<void (char*)>* ObtainSigAddScene() = 0;														//获得增加场景信号

	virtual void EmitSigAddScene(char* strName) = 0;																			//发送增加场景信号

	virtual boost::signals2::signal<void (int, double, double, double)>* ObtainSigInsertCtrlListRecord() = 0;					//获取插入控制列表记录信号

	virtual void EmitSigInsertControlListRecord(int ptNum, double ptX, double ptY, double ptZ) = 0;								//发送插入控制列表记录

	virtual boost::signals2::signal<void (char*)>* ObtainSigShowOpsHint() = 0;													//获得显示状态栏的提示的信号

	virtual void EmitSigShowOpsHint(char* strHint) = 0;																			//发送显示状态栏的提示的信号

	virtual boost::signals2::signal<void (char*)>* ObtainSigLinkPropertyListDlg() = 0;											//获得单体化关联属性对话框

	virtual void EmitSigLinkPropertyListDlg(char* sigPropertyListName) = 0;														//发送关联属性菜单对话框

	virtual boost::signals2::signal<void(int, int)>* obtainSigObjectMenuDlg() = 0;												//获取弹出单体化菜单的对话框

	virtual void EmitSigObjectMenuDlg(int mouseX, int mouseY) = 0;																//发送单体化菜单的信号
};

class USEREVENTAPI InterfaceSigEventCallBackFactory
{
public:
	InterfaceSigEventCallBack* create();
};