// osgMoveWholeSceneXY.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgMoveWholeSceneXY.h"
#include "InterfaceOsgStitcher.h"

bool COsgMoveWholeSceneXYEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		int selCnt = iSigEventCallBack->EmitSigSelModelIndex();										 //获得选择模型

		if (selCnt < 0)
		{
			return false;
		}
		
		osg::ref_ptr<osg::Group> sceneGroup = mRoot->getChild(0)->asGroup();
		osg::ref_ptr<osg::Node> sceneNode = sceneGroup->getChild(selCnt);

		if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
			{
				//整体向前移动
				double decimal = 0.1;
				KeyPressAdjustWholeScene(0, decimal, 0, sceneNode);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
			{
				//整体向后移动
				double decimal = -0.1;
				KeyPressAdjustWholeScene(0, decimal, 0, sceneNode);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
			{
				//整体向左移动
				double decimal = -0.1;
				KeyPressAdjustWholeScene(decimal, 0, 0, sceneNode);
			}
			else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
			{
				//整体向右移动
				double decimal = 0.1;
				KeyPressAdjustWholeScene(decimal, 0, 0, sceneNode);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
			{
				//撤销移动

			}
		}
	}
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}

void COsgMoveWholeSceneXYEvent::KeyPressAdjustWholeScene(double xDecimal, double yDecimal, double zDecimal, osg::ref_ptr<osg::Node> sceneNode)
{
	//获取选择的场景
	InterfaceOsgStitcherFactory IStitchFactory;
	InterfaceOsgStitcher* IStitch = IStitchFactory.create();

	if (zDecimal == 0)
	{
		IStitch->MoveWholeSceneXY(sceneNode, xDecimal, yDecimal);
	}
	else if (xDecimal == 0 && yDecimal == 0)
	{
		IStitch->MoveWholeSceneZ(sceneNode, zDecimal);
	}

}

void COsgMoveWholeSceneXYEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	isActivated = isActivate(vecOpState, OPS_EDIT_WHOLE_SCENE_XY);
}

