// osgDrawClosedLineEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDrawClosedLineEvent.h"

bool COsgDrawClosedLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;
			osg::Matrix worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::Vec4d color(1.0, 0, 1.0, 1.0);
			double size = 3.5f;
			LeftClickDrawClosedLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, mTempGroup, worldMatrix);					//启用画闭合线模式
			
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;
			osg::Matrix worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::Vec4d color(1.0, 0, 1.0, 1.0);
			double size = 3.5f;
			MouseMoveDrawClosedLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, mTempGroup, worldMatrix);					//启用画闭合线模式
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				BackSpaceReverseDrawClosedLineOperate(vecWorldCoord, mEditSceneGroup, mTempGroup);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
			{
				ClearGroupResult(mEditSceneGroup);
				ClearGroupResult(mTempGroup);
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

void COsgDrawClosedLineEvent::LeftClickDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::ref_ptr<osg::Group> tempGroup, osg::Matrix worldMatrix)
{
	ClearGroupResult(mTempGroup);

	LeftClickDrawMultiLineOperate(vecWorldCoord, worldPt, color, size, drawGroup, worldMatrix);	

	if (vecWorldCoord.size() >= 3)								//头尾相连
	{
		int totalNum = vecWorldCoord.size();
		IBaseDrawer->BaseDrawLineOperate(worldPt, vecWorldCoord[0], color, size - 2, mTempGroup, worldMatrix);
	}
}

bool COsgDrawClosedLineEvent::MouseMoveDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::ref_ptr<osg::Group> tempGroup, osg::Matrix worldMatrix)
{
	if (vecWorldCoord.size() == 0)
	{
		return false;
	}

	ClearGroupResult(mTempGroup);
	MouseMoveDrawMultiLineOperate(vecWorldCoord, worldPt, color, size, drawGroup, worldMatrix);

	if (vecWorldCoord.size() >= 2)								//头尾相连
	{
		int totalNum = vecWorldCoord.size();
		IBaseDrawer->BaseDrawLineOperate(worldPt, vecWorldCoord[0], color, size - 2, mTempGroup, worldMatrix);
	}
	return true;
}

void COsgDrawClosedLineEvent::BackSpaceReverseDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup, osg::ref_ptr<osg::Group> tempGroup)
{
	if (vecWorldCoord.size() == 0)
	{
		return;
	}

	BackSpaceReverseDrawMultiLineOperate(vecWorldCoord, drawGroup);
	ClearGroupResult(mTempGroup);
}

void COsgDrawClosedLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_CLOSED_LINE);

	if (isActivated == false && flag == true)											//改变状态
	{
		ClearGroupResult(mEditSceneGroup);
		ClearGroupResult(mTempGroup);
		vecWorldCoord.clear();
	}
}
