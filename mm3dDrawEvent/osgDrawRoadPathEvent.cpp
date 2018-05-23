// osgDrawRoadPathEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDrawRoadPathEvent.h"


void COsgDrawRoadPathEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_ROAD_PATH);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(mEditSceneGroup);
		vecWorldCoord.clear();
	}
}

bool COsgDrawRoadPathEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)				//鼠标左键
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::Vec4d color(1.0, 0, 1, 1.0);
			double size = 3.5f;
			LeftClickDrawRoadPathOperate(vecWorldCoord, pickResult.worldPt, color, size, mEditSceneGroup, worldMatrix, true);
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)		//鼠标移动
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//画单线
			osg::Vec4d color(1.0, 0, 1, 1.0);
			double size = 3.5f;
			MouseMoveDrawRoadPathOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix);			//启用画单点模式
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			ClearGroupResult(mEditSceneGroup);
			vecWorldCoord.clear();
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				//BackSpaceReverseDrawSingleLineOperate(vecWorldCoord, mEditSceneGroup);
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

void COsgDrawRoadPathEvent::LeftClickDrawRoadPathOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix, bool isOnlyOneLine)
{
	ClearGroupResult(drawGroup);
	if (vecWorldCoord.size()%2 == 0)
	{
		IBaseDrawer->BaseDrawPtOperate(worldPt, color, size, drawGroup, worldMatrix);	
	}
	else
	{
		std::vector<osg::Vec3d> vecCoord;
		insertNodePath(vecWorldCoord.back(), worldPt, vecCoord);
		IBaseDrawer->BaseDrawLineOperate(vecWorldCoord.back(), worldPt, color, size - 2, drawGroup, worldMatrix);
		for (size_t i = 0;i<vecCoord.size();++i)
		{
			IBaseDrawer->BaseDrawPtOperate(vecCoord[i], color, size, drawGroup, worldMatrix);
		}
	}
	vecWorldCoord.push_back(worldPt);
}


void COsgDrawRoadPathEvent::MouseMoveDrawRoadPathOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
{
	if (vecWorldCoord.empty())
	{
		return;
	}
	ClearGroupResult(drawGroup);
	std::vector<osg::Vec3d> vecCoord;
	insertNodePath(vecWorldCoord.back(), worldPt, vecCoord);
	IBaseDrawer->BaseDrawLineOperate(vecWorldCoord.back(), worldPt, color, size - 2, drawGroup, worldMatrix);
	for (size_t i = 0; i < vecCoord.size(); ++i)
	{
		IBaseDrawer->BaseDrawPtOperate(vecCoord[i], color, size, drawGroup, worldMatrix);
	}
}

void COsgDrawRoadPathEvent::insertNodePath(osg::Vec3d& pt1, osg::Vec3d& pt2, std::vector<osg::Vec3d>& vecWorldCoord)
{
	int numPtInsert = 8;
	osg::Vec3d p1;
	

	double deltax = (pt2.x() - pt1.x()) / (numPtInsert + 1);
	double deltay = (pt2.y() - pt1.y()) / (numPtInsert + 1);
	double deltaz = (pt2.z() - pt1.z()) / (numPtInsert + 1);
	for (int j = 0; j < numPtInsert; ++j)
	{
		p1.x() = pt1.x() + (j + 1)*deltax;
		p1.y() = pt1.y() + (j + 1)*deltay;
		p1.z() = pt1.z() + (j + 1)*deltaz;
		vecWorldCoord.push_back(p1);
	}
}