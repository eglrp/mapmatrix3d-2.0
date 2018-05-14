// osgAbsOrientEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "InterfaceMFCExcute.h"
#include "osgAbsOrientEvent.h"
#include "InterfaceBaseDrawer.h"

bool COsgAbsOrientEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//获得选择行
			int selRow = iSigEventCallBack->EmitSigGetSelRowFromAbsDlg();																//获得选择行

			if (selRow < 0)
			{
				InterfaceMFCExcuteFactory IExcuteFactory;
				InterfaceMFCExcute* IExcute = IExcuteFactory.create();
				IExcute->PopUpMessage("未选择控制点，请点击控制点列表进行选择");
				return false; 
			}

			if (iSigEventCallBack->EmitSigChangeCtrlList() == false)			//判断是否完成刺点
			{
				InterfaceMFCExcuteFactory IMfcExcuteFactory;
				InterfaceMFCExcute* IExcute = IMfcExcuteFactory.create();
				IExcute->PopUpMessage("该点已经刺过，请选择其他点号");
				return false;
			}

			//模型上画点
			InterfaceBaseDrawerFactory IDrawerFactory;
			InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
			osg::Vec4d color(1.0, 0, 0, 1.0);
			double size = 3.5f;
			IDrawer->BaseDrawPtOperate(worldPt,color,size,mEditGroup,worldMatrix);
			//获取模型上点
			bool isCheckPt = false;

			int ptNum = iSigEventCallBack->EmitSigGetSelPtNumFromAbsDlg();
			IAbsOrient->ObtainModelRelativePoint(ptNum, worldPt.x(), worldPt.y(), worldPt.z(), isCheckPt);
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{

		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				//回撤一个点
				
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
				IAbsOrient->ReverseModelOrientation(sceneNode, absMatrix);
				iSigEventCallBack->EmitSigShowOpsHint("先鼠标点击列表选择刺点行，然后按住ctrl键，左键选点，按ESC退出事件");
				mViewer->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);
			}
		}
	}
	else
	{
		vecWorldCoord.clear();
		osg::Matrix matrix;
		absMatrix = matrix;
	}

	return false;
}

void COsgAbsOrientEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_ABSOLUTE_ORIENTATION);

	if (isActivated == false && flag == true)																	
	{
		ClearGroupResult(mEditGroup);
		IAbsOrient->ClearVecRelativePt();
		IAbsOrient->ClearVecAbsolutePt();
		//消息置空
		iSigEventCallBack->EmitSigShowOpsHint("");
		iSigEventCallBack->EmitSigCloseAbsDlg();
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("先鼠标点击列表选择刺点行，然后按住ctrl键，左键选点，按ESC退出事件");
	}
}

BOOL COsgAbsOrientEvent::AbsOrient(std::string inFilePath)
{
	//创建文件夹并设置输出路径
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	//把精度报告的输出路径设置为索引位置
	if(!IAbsOrient->AbsOrient(sceneNode, inFilePath, absMatrix))
	{
		return FALSE;
	};

	mViewer->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);

	iSigEventCallBack->EmitSigShowOpsHint("按z键撤销定向结果 按ESC退出事件");

	osg::ref_ptr<osg::Group> editGroup = mRoot->getChild(1)->asGroup();
	ClearGroupResult(editGroup);
	return TRUE;
}

void COsgAbsOrientEvent::LoadControlPt()
{
	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();

	if (IAbsOrient->GetVecCtrlPt().size() > 0)
	{
		IExcute->PopUpMessage("已经加载过控制点了，不允许多次加载,请关闭后再重新加载");
		return;
	}

	std::string ctrlPtFileName;
	std::string extName = "Control Point Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	bool isSuccess = IExcute->OpenDialog(ctrlPtFileName, extName, true);

	if (isSuccess)
	{
		FILE* fpCtrlPt = fopen(ctrlPtFileName.c_str(), "r");

		if (fpCtrlPt == NULL)
		{
			IExcute->PopUpMessage("选择路径不正确");
			return;
		}

		//加载控制点
		IAbsOrient->LoadControlPoint(ctrlPtFileName);				

		std::vector<CControlPoint> vecCtrlPt = IAbsOrient->GetVecCtrlPt();
		//插入列表中
		for (int i = 0; i < vecCtrlPt.size(); i++)
		{
			iSigEventCallBack->EmitSigInsertControlListRecord(vecCtrlPt[i].ptSerialNum, vecCtrlPt[i].ptX, vecCtrlPt[i].ptY, vecCtrlPt[i].ptZ);
		}
	}
}

void COsgAbsOrientEvent::ReclickCtrlPt(int ptNum)
{
	std::vector<CControlPoint> vecRelaPt = IAbsOrient->GetVecRelativePt();

	for (int i = 0; i < vecRelaPt.size(); i++)
	{
		if (ptNum == vecRelaPt[i].ptSerialNum)
		{
			IAbsOrient->EraseRelativePtIndex(i);							//清除相应点index

			int childNum = mEditGroup->getNumChildren();
			osg::ref_ptr<osg::Node> node = mEditGroup->getChild(i);
			mEditGroup->removeChild(node);
		}
	}
}

void COsgAbsOrientEvent::ReSetCtrlPtCheckStatus(int ptNum, bool isCheckPt)
{
	IAbsOrient->SetControlPtStatus(ptNum, isCheckPt);							//清除相应点index
}

