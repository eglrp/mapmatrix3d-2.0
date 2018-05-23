// osgMeasurePtEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "qcomm.h"
#include "osgMeasurePtEvent.h"

bool COsgMeasurePtEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if(IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)											//左键
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//发送消息
			char strMsg[99];
			sprintf_s(strMsg, " XYZ = %lf  %lf %lf", worldPt.x(), worldPt.y(), worldPt.z());
			iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);

			//iSigEventCallBack->EmitSigShowOpsHint("按Backspace回退上一个点，按Z键撤销");
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace || ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				//消息置空
				char strMsg[99] = "";
				iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);
				//iSigEventCallBack->EmitSigShowOpsHint("按住ctrl键，点击鼠标左键测点，按ESC退出事件");
			}
		}
	}
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}

void COsgMeasurePtEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_MEASURE_POINT);

	if (isActivated == false && flag == true)														//刚不激活
	{
		
		//消息置空
		char strMsg[99] = "";
		iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);
		iSigEventCallBack->EmitSigCloseMeasureDlg();
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecWorldCoord.clear();
		iSigEventCallBack->EmitSigShowOpsHint("按住ctrl键，点击鼠标左键测点，按ESC退出事件");
	}
}