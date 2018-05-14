#include "stdafx.h"
#include "OsgDrawClosedLineDBWithPointEvent.h"



void COsgDrawClosedLineDBWithPointEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated1 == true)
	{
		flag = true;
	}

	isActivated1 = isActivate(vecOpState, OPS_DRAW_CLOSED_LINE_WITH_POINT);

	if (isActivated1 == false && flag == true)											//改变状态
	{
		ClearGroupResult(mEditSceneGroup);
		ClearGroupResult(mTempGroup);
		vecWorldCoord.clear();
	}
}

bool COsgDrawClosedLineDBWithPointEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated1)
	{
		DrawClosedLineEventOperate(ea);
	}
	else if(isActivated2)
	{
		DrawSinglePtEventOperate(ea);
	}
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}


void COsgDrawClosedLineDBWithPointEvent::DoubleClickDrawClosedLineOperate(osg::Vec3d worldPt, std::vector<osg::Vec3d> VecWorldCoord, osg::ref_ptr<osg::Group> editSceneGroup, osg::Matrix worldMatrix)
{
	//激活第二个事件
	isActivated2 = true;
	isActivated1 = false;
}

bool COsgDrawClosedLineDBWithPointEvent::DrawClosedLineEventOperate(const osgGA::GUIEventAdapter &ea)
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
	else if (IsDoubleClick(ea))
	{
		CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
		osg::Vec3d worldPt = pResult.worldPt;
		osg::Matrix worldMatrix = pResult.matrix;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return false;
		}

		DoubleClickDrawClosedLineOperate(worldPt, vecWorldCoord, mEditSceneGroup, worldMatrix);
	}
	else if (IsKeyPress(ea))
	{
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
		{
			if (vecWorldCoord.size() > 0)
			{
				BackSpaceReverseDrawClosedLineOperate(vecWorldCoord, mEditSceneGroup, mTempGroup);
			}
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
		{
			ClearGroupResult(mEditSceneGroup);
			ClearGroupResult(mTempGroup);
			vecWorldCoord.clear();
		}
	}
}

bool COsgDrawClosedLineDBWithPointEvent::DrawSinglePtEventOperate(const osgGA::GUIEventAdapter &ea)
{
	if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)				//左键
	{
		CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
		osg::Vec3d worldPt = pickResult.worldPt;
		osg::Matrix worldMatrix = pickResult.matrix;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return false;
		}

		osg::Vec4d color(1.0, 0, 1.0, 1.0);
		double size = 3.5f;

		ClickDrawSinglePtOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);
	}
	else if (IsKeyPress(ea))
	{
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
		{
			BackSpaceReverseSinglePtOperate(vecWorldCoord, mEditSceneGroup);
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
		{
			ClearGroupResult(mEditSceneGroup);
			vecWorldCoord.clear();
		}
	}
}
