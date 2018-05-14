// osgFlatSceneByPolygonEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgFlatSceneByPolygonEvent.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgFlatByScope.h"
#include "InterfaceOsgRecoverFlatByScope.h"
#include "InterfaceOsgFinder.h"
#include "publicDefined.h"


void COsgFlatSceneByPolygonEvent::FlatByPolygon(std::string inFilePath)
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
	std::vector<CScope> vecClipScope;
	bool isSuccess = GetClipScope(vecClipScope, vecScope, vecWorldCoord);

	if (!isSuccess)
	{
		return;
	}

	this->inFilePath = inFilePath;
	vecPreScope = vecClipScope;
	StartFlatSceneByPolygon(sceneNode, inFilePath, vecClipScope, true, iSigEventCallBack);
}

bool COsgFlatSceneByPolygonEvent::StartFlatSceneByPolygon(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, std::vector<CScope> vecClipScope, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack)
{
	InterfaceOsgFlatByScopeFactory iOsgFlatByScopeFactory;
	InterfaceOsgFlatByScope* iOsgFlatByScope = iOsgFlatByScopeFactory.create(FLAT_BY_NORMALIZE);
	bool isSuccess = iOsgFlatByScope->flatSceneByScope(sceneNode, inFilePath, vecClipScope, iSigEventCallBack);

	return isSuccess;
}

bool COsgFlatSceneByPolygonEvent::GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord)
{
	if (vecScope.size() == 0)
	{
		if (vecWorldCoord.size() < 3)
		{
			InterfaceMFCExcuteFactory IExcuteFactory;
			InterfaceMFCExcute* IExcute = IExcuteFactory.create();
			IExcute->PopUpMessage("«Îœ»—°‘Ò≤√«–∑∂Œß");
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

bool COsgFlatSceneByPolygonEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			vecWorldCoord.push_back(worldPt);
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) >= 0)
		{

		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				vecWorldCoord.pop_back();
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldCoord.clear();
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_R)
			{
				osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
				InterfaceOsgRecoverFlatByScopeFactory iOsgRecoverFlatByScopeFactory;
				InterfaceOsgRecoverFlatByScope* iOsgRecoverFlatByScope = iOsgRecoverFlatByScopeFactory.create();
				iOsgRecoverFlatByScope->recoverFlatSceneByScope(sceneNode, inFilePath, vecPreScope, vecPreScope, iSigEventCallBack);
			}
		}
	}

	return false;
}

void COsgFlatSceneByPolygonEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_FLAT_SCENE_BY_POLYGON);

	if (isActivated == false && flag == true)
	{
		//œ˚œ¢÷√ø’
		vecWorldCoord.clear();
	}

	if (isActivated && flag == false)																//∏’º§ªÓ
	{

	}
}