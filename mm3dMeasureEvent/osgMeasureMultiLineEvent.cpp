// osgMeasureMultiLineEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgMeasureMultiLineEvent.h"


bool COsgMeasureMultiLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			vecWorldCoord.push_back(worldPt);

			//计算距离
			double length = CalLength(vecWorldCoord);
			//发送消息
			char strMsg[99];
			sprintf_s(strMsg, " 长度： %lf米", length);
			iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);

			iSigEventCallBack->EmitSigShowOpsHint("按Backspace回退上一个点，双击左键结束，按Z键撤销");
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//计算距离
			int totalNum = vecWorldCoord.size();

			if (totalNum == 0)
			{
				return false;
			}

			double length = CalLength(vecWorldCoord);
			length += CalculateDist(worldPt, vecWorldCoord[totalNum - 1]);

			//发送消息
			char strMsg[99];
			sprintf_s(strMsg, " 长度： %lf米", length);
			iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);
		}
		else if (IsDoubleClick(ea))
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//计算距离
			int totalNum = vecWorldCoord.size();

			if (totalNum == 0)
			{
				return false;
			}

			double length = CalLength(vecWorldCoord);
			length += CalculateDist(worldPt, vecWorldCoord[totalNum - 1]);

			//发送消息
			char strMsg[99];
			sprintf_s(strMsg, " 长度： %lf米", length);
			iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);
			vecWorldCoord.clear();
			iSigEventCallBack->EmitSigShowOpsHint("按住ctrl键，点击鼠标左键选点，然后移动鼠标测线，按ESC退出事件");
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				vecWorldCoord.pop_back();
				//消息置空
				char strMsg[99] = "";
				iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);

				iSigEventCallBack->EmitSigShowOpsHint("按住ctrl键，点击鼠标左键选点，然后移动鼠标测线，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
			{
				vecWorldCoord.clear();

				//消息置空
				char strMsg[99] = "";
				iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);

				iSigEventCallBack->EmitSigShowOpsHint("按住ctrl键，点击鼠标左键选点，然后移动鼠标测线，按ESC退出事件");
			}
		}
	}

	return false;
}

void COsgMeasureMultiLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_MEASURE_MULTIlINE);

	if (isActivated == false && flag == true)
	{
		//消息置空
		char strMsg[99] = "";
		iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);
		iSigEventCallBack->EmitSigCloseMeasureDlg();
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按住ctrl键，点击鼠标左键选点，然后移动鼠标测线，按ESC退出事件");
	}
}