// osgExtractObjectByPolygon.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgExtractObjectByPolygonEvent.h"
#include "InterfaceCOsgExtractObject.h"
#include "InterfaceOsgExtractHouse.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgAlgorithm.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceCOsgExtractArea.h"
#include "InterfaceMFCExcute.h"

bool COsgExtractObjectByPolygonEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated1)
	{
		DrawExtractObjectAreaOperate(ea);
	}
	else if (isActivated2)
	{
		DrawExtractHouseOperate(ea);
	}

	return false;
}

void COsgExtractObjectByPolygonEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated1 == true)
	{
		flag = true;
	}

	isActivated1 = isActivate(vecOpState, OPS_EXTRACT_OBJECT_BY_POLYGON);

	if (isActivated1 == false && flag == true)
	{
		//消息置空
		vecWorldPt.clear();
		vecScope.clear();
		saveClickPt = osg::Vec3d(0, 0, 0);
		ClearGroupResult(editGroup);
		ClearGroupResult(objectGroup);
	}

	if (isActivated1 && flag == false)																//刚激活
	{
		saveClickPt = osg::Vec3d(0, 0, 0);
	}
}

void COsgExtractObjectByPolygonEvent::ExtractObject(std::string inFilePathName)
{
	std::vector<CScope> vecClipScope;
	bool isSuccess = GetClipScope(vecClipScope, vecScope, vecWorldPt);

	if (!isSuccess)
	{
		return;
	}

	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	InterfaceOsgExtractHouseFactory iOsgExtractHouseFactory;
	InterfaceOsgExtractHouse* iOsgExtractHouse = iOsgExtractHouseFactory.create();
	iOsgExtractHouse->extractHouse(sceneNode, vecWorldPt);

	InterfaceMFCExcuteFactory iMfcExcuteFactory;
	InterfaceMFCExcute* iMfcExcute = iMfcExcuteFactory.create();
	iMfcExcute->PopUpMessage("房屋提取完成");
}

void COsgExtractObjectByPolygonEvent::ExtractArea(std::string inFilePathName)
{
	std::vector<CScope> vecClipScope;
	bool isSuccess = GetClipScope(vecClipScope, vecScope, vecWorldPt);

	if (!isSuccess)
	{
		return;
	}

	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	InterfaceCOsgExtractAreaFactory iOsgExtractAreaFactory;
	InterfaceCOsgExtractArea* iOsgExtractArea = iOsgExtractAreaFactory.create();
	iOsgExtractArea->extractArea(sceneNode, inFilePathName, vecClipScope);

	InterfaceMFCExcuteFactory iMfcExcuteFactory;
	InterfaceMFCExcute* iMfcExcute = iMfcExcuteFactory.create();
	iMfcExcute->PopUpMessage("区域提取完成");
}

bool COsgExtractObjectByPolygonEvent::GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord)
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

bool COsgExtractObjectByPolygonEvent::DrawExtractObjectAreaOperate(const osgGA::GUIEventAdapter &ea)
{
	if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
	{
		CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
		osg::Vec3d worldPt = pickResult.worldPt;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return false;
		}

		vecWorldPt.push_back(worldPt);
	}
	else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
	{
		isActivated1 = false;
		isActivated2 = true;
	}
	else if (IsKeyPress(ea))
	{
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
		{
			vecWorldPt.pop_back();
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
		{
			vecWorldPt.clear();
		}
	}
}


bool COsgExtractObjectByPolygonEvent::DrawExtractHouseOperate(const osgGA::GUIEventAdapter &ea)
{
	if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
	{
		CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
		osg::Vec3d worldPt = pickResult.worldPt;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return false;
		}

		saveClickPt = worldPt;
	}
}



bool COsgExtractObjectByPolygonEvent::FindMinHeight(std::vector<osg::Vec3d> vecWorldPt, double &minHeight)
{
	if (vecWorldPt.size() == 0)
	{
		return false;
	}

	double minHeihgt = 0; double maxHeight = 0;
	InterfaceOsgAlgorithmFactory iOsgAlgorithmFactory;
	InterfaceOsgAlgorithm* iOsgAlgorithm = iOsgAlgorithmFactory.create();
	iOsgAlgorithm->FindMinMaxHeight(vecWorldPt, minHeight, maxHeight);

	return true;
}
