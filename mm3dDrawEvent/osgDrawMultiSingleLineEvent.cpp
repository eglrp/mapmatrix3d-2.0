// osgDrawMultiSingleLineEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDrawMultiSingleLineEvent.h"


bool COsgDrawMultiSingleLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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

			osg::Vec4d color(0.0, 1.0, 0.0, 1.0);

			int totalNum = vecWorldCoord.size();

			if (totalNum % 2 == 1)
			{
				osg::Vec4d newColor(1.0, 0.0, 0.0, 1.0);
				color = newColor;
			}

			double size = 3.5f;
			LeftClickDrawMultiSingleLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);
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

			osg::Vec4d color(1.0, 0.0, 0, 1.0);
			double size = 3.5f;
			MouseMoveDrawMultiSingleLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					//BackSpace	 
			{
				BackSpaceReverseDrawMultiSingleLineOperate(vecWorldCoord, mEditSceneGroup);
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

void COsgDrawMultiSingleLineEvent::LeftClickDrawMultiSingleLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	LeftClickDrawSingleLineOperate(vecWorldCoord, worldPt, color, size, drawGroup, worldMatrix, false);
}

void COsgDrawMultiSingleLineEvent::MouseMoveDrawMultiSingleLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	MouseMoveDrawSingleLineOperate(vecWorldCoord, worldPt, color, size, drawGroup, worldMatrix);
}

void COsgDrawMultiSingleLineEvent::BackSpaceReverseDrawMultiSingleLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup)
{
	int totalNum = vecWorldCoord.size();

	if (totalNum % 2 == 1)
	{
		int childNum = drawGroup->getNumChildren();
		drawGroup->removeChild(childNum - 1, 1U);
		drawGroup->removeChild(childNum - 2, 1U);
		int totalNum = vecWorldCoord.size();
		IBaseDrawer->DeleteVecChild(totalNum - 1, vecWorldCoord);
	}
	else if (totalNum % 2 == 0)
	{
		int childNum = drawGroup->getNumChildren();
		drawGroup->removeChild(childNum - 1, 1U);
		int totalNum = vecWorldCoord.size();
		IBaseDrawer->DeleteVecChild(totalNum - 1, vecWorldCoord);
	}
}

void COsgDrawMultiSingleLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_MULTI_SINGLE_LINE);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(mEditSceneGroup);
		vecWorldCoord.clear();
	}
}
