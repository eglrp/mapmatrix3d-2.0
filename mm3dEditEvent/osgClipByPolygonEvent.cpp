// osgClipByPolygonEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "InterfaceMFCExcute.h"
#include "osgClipByPolygonEvent.h"
#include "InterfaceOsgClip.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceCOsgClipByScope.h"
#include "ThreadSafeUpdateCallback.h"

bool COsgClipByPolygonEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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
			iSigEventCallBack->EmitSigShowOpsHint("按BackSpace回退上一个点，按Z全部撤销，按ESC退出事件");
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{

		}
		else if (IsDoubleClick(ea))
		{
			//保存数据
			CScope scope;
			scope.vecCoord = vecWorldCoord;
			vecScope.push_back(scope);

			//开始新的画闭合线
			vecWorldCoord.clear();
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				if (vecWorldCoord.size() > 0)
				{
					//撤销上个点
					vecWorldCoord.pop_back();
					iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，完成画范围，双击画多个范围，然后进行裁切，按ESC退出事件");
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldCoord.clear();
				vecScope.clear();
				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，完成画范围，双击画多个范围，然后进行裁切，按ESC退出事件");
			}
		}
	}
	else
	{

	}

	return false;
}

void COsgClipByPolygonEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_CLIP_BY_POLYGON);

	if (isActivated == false && flag == true)																	
	{
		//消息置空
		iSigEventCallBack->EmitSigShowOpsHint("");
		vecWorldCoord.clear();
		vecScope.clear();
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，完成画范围，然后进行裁切，双击画多个范围，按ESC退出事件");
	}
}

bool COsgClipByPolygonEvent::GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord)
{
	if (vecScope.size() == 0)
	{
		if (vecWorldCoord.size() < 3)
		{
			InterfaceMFCExcuteFactory IExcuteFactory;
			InterfaceMFCExcute* IExcute = IExcuteFactory.create();
			IExcute->PopUpMessage("请先选择裁切范围");
			return false;
		}
		else
		{
			CScope scope;
			scope.vecCoord = vecWorldCoord;
			vecClipScope.push_back(scope);
		}
	}
	else
	{
		if (vecWorldCoord.size() >= 3)
		{
			vecClipScope = vecScope;
			CScope scope;
			scope.vecCoord = vecWorldCoord;
			vecClipScope.push_back(scope);
		}
		else
		{
			vecClipScope = vecScope;
		}
	}

	return true;
}

void COsgClipByPolygonEvent::ClipByPolygon(std::string inFilePath)
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
	std::vector<CScope> vecClipScope;
	bool isSuccess = GetClipScope(vecClipScope, vecScope, vecWorldCoord);

	if (!isSuccess)
	{
		return;
	}

	StartClipSceneByPolygon(sceneNode, inFilePath, vecClipScope, true, iSigEventCallBack);
}

bool COsgClipByPolygonEvent::StartClipSceneByPolygon(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack)
{
	InterfaceCOsgClipByScopeFactory iOsgClipByScopeFactory;
	InterfaceCOsgClipByScope* iOsgClipByScope = iOsgClipByScopeFactory.create(ACCURATE_CLIP_BY_POLYGON_1);
	bool isSuccess = iOsgClipByScope->hollowOutByScope(sceneNode, inFilePath, vecClipScope, isInside, iSigEventCallBack);

	return isSuccess;
}
