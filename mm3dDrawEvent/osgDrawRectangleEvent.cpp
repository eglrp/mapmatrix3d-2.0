// osgDrawRectangleEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDrawRectangleEvent.h"
#include "Geometry.h"
#include "qcomm.h"

void COsgDrawRectangleEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_RECTANGLE);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(mEditSceneGroup);
		vecWorldCoord.clear();
	}
}

bool COsgDrawRectangleEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if(ea.getEventType() == osgGA::GUIEventAdapter::DRAG && GetKeyState(VK_CONTROL) < 0)				//拖动
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			if (isFirstPoint)
			{
				isFirstPoint = false;
				vecWorldCoord.push_back(worldPt);
			}
			else
			{
				osg::Vec4d color(1.0, 0, 1.0, 1.0);
				double size = 3.5f;
				osg::Matrix viewMatrix = mViewer->getCamera()->getViewMatrix();

				DragDrawRectangleOperate(vecWorldCoord, worldPt, color, size, mEditSceneGroup, worldMatrix, viewMatrix);
			}
			
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && GetKeyState(VK_CONTROL) >= 0)
		{
			isFirstPoint = true;
			vecWorldCoord.clear();
			ClearGroupResult(mEditSceneGroup);
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			ClearGroupResult(mEditSceneGroup);
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				BackSpaceReverseDrawRectangleOperate(mEditSceneGroup);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
			{
				ClearGroupResult(mEditSceneGroup);
				vecWorldCoord.clear();
			}
		}
		else if (GetKeyState(VK_CONTROL) >= 0)
		{
			isFirstPoint = true;
			vecWorldCoord.clear();
		}
	}
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}

void COsgDrawRectangleEvent::DragDrawRectangleOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix, osg::Matrix viewMatrix)
{
	//画矩形框
	ClearGroupResult(drawGroup);

	int totalNum = vecWorldCoord.size();

	if (totalNum == 0)
	{
		return;
	}

	double z = vecWorldCoord[totalNum - 1].z();
	//osg::Vec3 northVec = osg::Y_AXIS * viewMatrix;
	//northVec.z() = 0.0f;
	//northVec.normalize();
	//osg::Vec3 axis = osg::Y_AXIS ^ northVec;
	//float angle = atan2(axis.length(), osg::X_AXIS * northVec);
	//angle *= -1;

	double x1 = vecWorldCoord[totalNum - 1].x();
	double y1 = vecWorldCoord[totalNum - 1].y();
	double x2 = worldPt.x();
	double y2 = worldPt.y();

	//新坐标系
	//double nx1 = x1 * cos(angle) + y1 * sin(angle);
	//double ny1 = y1 * cos(angle) - x1 * sin(angle);
	//double nx2 = x2 * cos(angle) + y2 * sin(angle);
	//double ny2 = y2 * cos(angle) - x2 * sin(angle);
	//double nx3 = nx1;
	//double ny3 = ny2;
	//double nx4 = nx2;
	//double ny4 = ny1;
	double x3 = x1/*nx3 * cos(-angle) + ny3 * sin(-angle)*/;
	double y3 = y2/*ny3 * cos(-angle) - nx3 * sin(-angle)*/;
	double x4 = x2/*nx4 * cos(-angle) + ny4 * sin(-angle)*/;
	double y4 = y1/*ny4 * cos(-angle) - nx4 * sin(-angle)*/;

	std::vector<osg::Vec3d> rectangleCoord;
	rectangleCoord.push_back(osg::Vec3d(x1, y1, z));
	rectangleCoord.push_back(osg::Vec3d(x3, y3, z));
	rectangleCoord.push_back(osg::Vec3d(x2, y2, z));
	rectangleCoord.push_back(osg::Vec3d(x4, y4, z));
	IBaseDrawer->BaseDrawLineLoopOperate(rectangleCoord, color, size, drawGroup, worldMatrix);
}

void COsgDrawRectangleEvent::BackSpaceReverseDrawRectangleOperate(osg::ref_ptr<osg::Group> drawGroup)
{
	int childNum = drawGroup->getNumChildren();

	if (childNum > 0)
	{
		drawGroup->removeChild(childNum - 1);
	}

	vecWorldCoord.clear();
}