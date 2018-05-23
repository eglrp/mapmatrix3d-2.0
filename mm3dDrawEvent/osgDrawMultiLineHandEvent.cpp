#include "stdafx.h"
#include "osgDrawMultiLineHandEvent.h"

bool COsgDrawMultiLineHandEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;
			worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			
			osg::Vec4d color(1.0, 0, 0, 1.0);
			double size = 5.5f;
			LeftClickDrawClosedLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);					//启用双击画闭合线模式

		}
		else if (IsDoubleClick(ea))
		{
			osg::Vec4d color(1.0, 0, 0, 1.0);
			double size = 5.5f;
			DoubleClickDrawClosedLineOperate(vecWorldCoord,  color, size, mEditSceneGroup, worldMatrix);					//启用画闭合线模式
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
			double size = 5.5f;
			MouseMoveDrawClosedLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);					//启用画闭合线模式
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				BackSpaceReverseDrawClosedLineOperate(vecWorldCoord, mEditSceneGroup);
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

void COsgDrawMultiLineHandEvent::DoubleClickDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord,  osg::Vec4d color,double size,  osg::ref_ptr<osg::Group> mEditSceneGroup,osg::Matrix worldMatrix)
{
	if (vecWorldCoord.size() >= 2)								//头尾相连
	{
		ClearGroupResult(mEditSceneGroup);
		/*int totalNum = vecWorldCoord.size();
		IBaseDrawer->BaseDrawLineOperate(vecWorldCoord[totalNum-1], vecWorldCoord[0], color, size - 2, mEditSceneGroup, worldMatrix);*/
		vecWorldCoord.clear();
	}
}

void COsgDrawMultiLineHandEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_MULTI_LINE_HAND);

	if (isActivated == false && flag == true)											//改变状态
	{
		ClearGroupResult(mEditSceneGroup);
		ClearGroupResult(mTempGroup);
		vecWorldCoord.clear();
	}
}






bool COsgDrawMultiLineForMultiLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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
			worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::Vec4d color(1.0, 0, 1.0, 1.0);
			double size = 3.5f;
			LeftClickDrawClosedLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);					//启用双击画闭合线模式

		}
		else if (IsDoubleClick(ea))
		{
			osg::Vec4d color(1.0, 0, 1.0, 1.0);
			double size = 3.5f;
			DoubleClickDrawClosedLineOperate(vecWorldCoord,  color, size, mEditSceneGroup, worldMatrix);					//启用画闭合线模式
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
			MouseMoveDrawClosedLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);					//启用画闭合线模式
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				if (vecWorldCoord.empty())
				{
					ClearGroupResult(mEditSceneGroup);
					ClearGroupResult(mTempGroup);
				}
				else
					BackSpaceReverseDrawClosedLineOperate(vecWorldCoord, mEditSceneGroup);
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


void COsgDrawMultiLineForMultiLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_MULTI_LINE_FOR_MULTI_LINE);

	if (isActivated == false && flag == true)											//改变状态
	{
		ClearGroupResult(mEditSceneGroup);
		ClearGroupResult(mTempGroup);
		vecWorldCoord.clear();
	}
}

void COsgDrawMultiLineForMultiLineEvent::DoubleClickDrawClosedLineOperate(std::vector<osg::Vec3d> &vecWorldCoord,  osg::Vec4d color,double size,  osg::ref_ptr<osg::Group> mEditSceneGroup,osg::Matrix worldMatrix)
{
	if (vecWorldCoord.size() >= 2)								//头尾相连
	{
		//ClearGroupResult(mEditSceneGroup);
		/*int totalNum = vecWorldCoord.size();
		IBaseDrawer->BaseDrawLineOperate(vecWorldCoord[totalNum-1], vecWorldCoord[0], color, size - 2, mEditSceneGroup, worldMatrix);*/
		vecWorldCoord.clear();
	}
}