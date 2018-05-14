// osgQuickDeleteTriangleBySolidEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgQuickDeleteTriangleBySolidEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgClip.h"
//
//void COsgQuickDeleteSingleTriangleBySolidEvent::StartQuickClipSceneBySolid()
//{
//	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
//
//	InterfaceOsgClipFactory iOsgClipFactory;
//	InterfaceOsgClip* iOsgClip = iOsgClipFactory.create(BY_SUSPEND_SCOPE);
//	iOsgClip->clipOsgHeader(sceneNode, vecScope, false);
//
//	vecScope.clear();
//}
//
//void COsgQuickDeleteSingleTriangleBySolidEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
//{
//	bool flag = false;
//
//	if (isActivated == true)
//	{
//		flag = true;
//	}
//
//	isActivated = isActivate(vecOpState, OPS_QUICK_DELETE_SINGLE_TRIANGLE_BY_SOLID);
//
//	if (isActivated == false && flag == true)
//	{
//		//消息置空
//		iSigEventCallBack->EmitSigShowOpsHint("");
//		vecScope.clear();
//		ClearGroupResult(mEditScopeGroup);
//		ClearGroupResult(ScopeGroup);
//		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
//		if (spChildCallback != NULL)
//		{
//			int num = mRoot->getNumChildren();
//			osg::ref_ptr<osg::Node> childNode;
//			for (int i = 0; i < num; i++)
//			{
//				childNode = mRoot->getChild(i);
//				if (childNode->getName() == "ScopeGroup")
//				{
//					spChildCallback->RemoveChild(mRoot, childNode);
//					break;
//				}
//			}
//		}
//	}
//
//	if (isActivated && flag == false)																//刚激活
//	{
//		InterfaceBaseDrawerFactory IDrawerFactory;
//		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
//		IDrawer->AddDraggerToScene(mRoot, ScopeGroup, mEditScopeGroup);
//		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键点击悬空物自动生成范围，可多次点击生成范围，然后进行裁切，按ESC退出事件");
//	}
//}
//
//bool COsgQuickDeleteSingleTriangleBySolidEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
//{
//	if (isActivated)
//	{
//		if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && GetKeyState(VK_CONTROL) < 0)
//		{
//			CtrlReleaseOperate(mRoot, mEditScopeGroup, vecScope);
//		}
//		else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && GetKeyState(VK_CONTROL) < 0)
//		{
//			CtrlDragOperate(mViewer, ea, aa);
//		}
//		else if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
//		{
//			CtrlLeftClickOperate(ea, aa);
//		}
//		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) >= 0)
//		{
//
//		}
//		else if (IsKeyPress(ea))
//		{
//			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
//			{
//				BackSpaceOperate(vecScope, mEditScopeGroup);
//			}
//			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
//			{
//				KeyZOperate(vecScope, mEditScopeGroup);
//			}
//		}
//	}
//
//	return false;
//}
//
//bool COsgQuickDeleteSingleTriangleBySolidEvent::ObtainScope(CScope &scope, std::string &fileName, osg::Vec3d worldPt, osg::Vec3d localPt, osg::Matrix worldMatrix, osg::NodePath np)
//{
//	if (np.size() == 0)
//	{
//		return false;
//	}
//
//	osg::Vec3d minXYZ(worldPt.x() - 3, worldPt.y() - 3, worldPt.z() - 3);
//	osg::Vec3d maxXYZ(worldPt.x() + 3, worldPt.y() + 3, worldPt.z() + 3);
//
//	std::vector<osg::Vec3d> vecCoord;
//	double height = maxXYZ.z() - minXYZ.z();
//	vecCoord.push_back(minXYZ);
//	vecCoord.push_back(osg::Vec3d(minXYZ.x(), maxXYZ.y(), minXYZ.z()));
//	vecCoord.push_back(osg::Vec3d(maxXYZ.x(), maxXYZ.y(), minXYZ.z()));
//	vecCoord.push_back(osg::Vec3d(maxXYZ.x(), minXYZ.y(), minXYZ.z()));
//	scope.vecCoord = vecCoord;
//	scope.minH = minXYZ.z();
//	scope.maxH = worldPt.z();
//
//	return true;
//}
