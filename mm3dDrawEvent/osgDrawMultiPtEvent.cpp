// osgDrawMultiPtEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDrawMultiPtEvent.h"

void COsgDrawMultiPtEvent::CtrlLeftClickDrawMultiPtOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, 
	osg::Matrix worldMatrix)
{
	ClickDrawSinglePtOperate(vecWorldCoord, worldPt, color, size, drawGroup, worldMatrix);

	vecWorldCoord.clear();
}

void COsgDrawMultiPtEvent::BackSpaceReverseMultiPtOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup)
{
	BackSpaceReverseSinglePtOperate(vecWorldCoord, drawGroup);
}

bool COsgDrawMultiPtEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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

			osg::Vec4d color(1.0, 0, 0, 1.0);
			double size = 3.5f;
			CtrlLeftClickDrawMultiPtOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					//BackSpace	 
			{
				BackSpaceReverseMultiPtOperate(vecWorldCoord, mEditSceneGroup);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)						//z
			{
				ClearGroupResult(mEditSceneGroup);
				vecWorldCoord.clear();
			}
		}
	}
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}

void COsgDrawMultiPtEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_MULTI_PT);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(mEditSceneGroup);
		vecWorldCoord.clear();
	}
}
