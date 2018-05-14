// osgMeasureSingleLineEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgMeasureSingleLineEvent.h"

bool COsgMeasureLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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

			vecWorldCoord.push_back(worldPt);

			if (vecWorldCoord.size() % 2 == 1)														//奇数时候			
			{
				return false;
			}

			//计算距离
			int totalNum = vecWorldCoord.size();

			if (totalNum <= 1)
			{
				return false;
			}

			osg::Vec3d pt1 = vecWorldCoord[totalNum - 1];
			osg::Vec3d pt2 = vecWorldCoord[totalNum - 2];
			double dist = CalculateDist(pt1, pt2);
			double height = abs(pt1.z() - pt2.z());
			//发送消息
			char strMsg[99];
			sprintf_s(strMsg, " 距离： %lf米  高度: %lf米", dist, height);
			iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);

			iSigEventCallBack->EmitSigShowOpsHint("按Backspace回退上一个点，按Z键撤销");
		}
		else if (IsMouseMove(ea))
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			if (vecWorldCoord.size() % 2 == 0)														//偶数时候			
			{
				return false;
			}

			//计算距离
			int totalNum = vecWorldCoord.size();
			osg::Vec3d pt1 = vecWorldCoord[totalNum - 1];
			double dist = CalculateDist(pt1, worldPt);
			double height = abs(pt1.z() - worldPt.z());

			//发送消息
			char strMsg[99];
			sprintf_s(strMsg, " 距离： %lf米  高度: %lf米", dist, height);
			iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);
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
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldCoord.clear();

				//消息置空
				char strMsg[99] = "";
				iSigEventCallBack->EmitSigNotifyMeasureMsg(strMsg);

				iSigEventCallBack->EmitSigShowOpsHint("按住ctrl键，点击鼠标左键选点，然后移动鼠标测线，按ESC退出事件");
			}
		}
	}
	else 
	{
		vecWorldCoord.clear();
	}

	return false;
}

void COsgMeasureLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_MEASURE_LINE);

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