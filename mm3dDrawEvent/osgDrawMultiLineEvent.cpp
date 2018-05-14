// osgDrawMultiLineEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDrawMultiLineEvent.h"

bool COsgDrawMultiLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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

			//画多线
			osg::Vec4d color(1.0, 0, 1.0, 1.0);
			double size = 3.5f;
			LeftClickDrawMultiLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);					   //启用画多线
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

			//画多线
			osg::Vec4d color(1.0, 0, 1.0, 1.0);
			double size = 3.5f;
			MouseMoveDrawMultiLineOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);		//启用鼠标移动画线

		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			DoubleClickDrawMultiLineOperate(vecWorldCoord);
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				BackSpaceReverseDrawMultiLineOperate(vecWorldCoord, mEditSceneGroup);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
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

void COsgDrawMultiLineEvent::LeftClickDrawMultiLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	//画单点
	bool isInitPoint = true;

	if(vecWorldCoord.size() == 0)
	{
		isInitPoint = true;
	}
	else
	{
		isInitPoint = false;
	}

	if (isInitPoint)
	{
		IBaseDrawer->BaseDrawPtOperate(worldPt, color, size, drawGroup, worldMatrix);
		IBaseDrawer->BaseDrawPtOperate(worldPt, color, size, drawGroup, worldMatrix);
	}
	else
	{
		int childNum = drawGroup->getNumChildren();
		drawGroup->removeChild(childNum - 1, 1U);
		//绘制线
		int totalNum = vecWorldCoord.size();
		IBaseDrawer->BaseDrawLineOperate(vecWorldCoord[totalNum - 1], worldPt, color, size - 2, drawGroup, worldMatrix);
		IBaseDrawer->BaseDrawPtOperate(worldPt, color, size, drawGroup, worldMatrix);
		IBaseDrawer->BaseDrawPtOperate(worldPt, color, size, drawGroup, worldMatrix);
	}

	//保存		
	vecWorldCoord.push_back(worldPt);
}

bool COsgDrawMultiLineEvent::MouseMoveDrawMultiLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	if (vecWorldCoord.size() == 0)
	{
		return false;
	}

	int childNum = drawGroup->getNumChildren();
	drawGroup->removeChild(childNum - 1, 1U);

	//画线
	int totalNum = vecWorldCoord.size();
	osg::Vec3d pt1 = vecWorldCoord[totalNum - 1];
	IBaseDrawer->BaseDrawLineOperate(pt1, worldPt, color, size - 2, drawGroup, worldMatrix);

	return true;
}

void COsgDrawMultiLineEvent::DoubleClickDrawMultiLineOperate(std::vector<osg::Vec3d> &vecWorldCoord)
{

}


void COsgDrawMultiLineEvent::BackSpaceReverseDrawMultiLineOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup)
{
	int childNum = drawGroup->getNumChildren();

	if (childNum >= 3)
	{
		drawGroup->removeChild(childNum - 1, 1U);
		drawGroup->removeChild(childNum - 2, 1U);
		//drawGroup->removeChild(childNum - 3, 1U);
		int totalNum = vecWorldCoord.size();
		IBaseDrawer->DeleteVecChild(totalNum - 1, vecWorldCoord);
	}
	else if (childNum >= 2)
	{
		drawGroup->removeChild(childNum - 1, 1U);
		drawGroup->removeChild(childNum - 2, 1U);
		int totalNum = vecWorldCoord.size();
		IBaseDrawer->DeleteVecChild(totalNum - 1, vecWorldCoord);
	}
	else if (childNum == 1)
	{
		drawGroup->removeChild(childNum - 1, 1U);
		int totalNum = vecWorldCoord.size();
		IBaseDrawer->DeleteVecChild(totalNum - 1, vecWorldCoord);
	}
}

void COsgDrawMultiLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_MULTI_LINE);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(mEditSceneGroup);
		vecWorldCoord.clear();
	}
}
