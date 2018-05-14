#include "stdafx.h"
#include "OsgDrawClosedLineForMeasureAreaEvent.h"


void COsgDrawClosedLineForMeasureAreaEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_MULTI_CLOSED_LINE_FOR_AREA);

	if (isActivated == false && flag == true)											//改变状态
	{
		ClearGroupResult(mEditSceneGroup);
		ClearGroupResult(mTempGroup);
		vecWorldCoord.clear();
	}
}

void COsgDrawClosedLineForMeasureAreaEvent::DoubleClickDrawClosedLineOperate(osg::Vec3d worldPt, std::vector<osg::Vec3d> VecWorldCoord, osg::ref_ptr<osg::Group> editSceneGroup, osg::Matrix worldMatrix)
{
	vecWorldCoord.clear();
}

bool COsgDrawClosedLineForMeasureAreaEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			if (vecWorldCoord.empty())
			{
				ClearGroupResult(mEditSceneGroup);
				ClearGroupResult(mTempGroup);
			}
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
				else
				{
					ClearGroupResult(mEditSceneGroup);
					ClearGroupResult(mTempGroup);
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
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}