// osgDrawSinglePt.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "qcomm.h"
#include "osgDrawSinglePtHandler.h"

bool COsgDrawSinglePtHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if(IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)				//左键
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
		else if(IsKeyPress(ea))
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
	else
	{
		vecWorldCoord.clear();
	}
	
	return false;
}

void COsgDrawSinglePtHandler::ClickDrawSinglePtOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::Vec3d worldPt, osg::Vec4d color, double size, osg::ref_ptr<osg::Group> drawGroup, osg::Matrix worldMatrix)
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
		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();
		IBaseDrawer->BaseDrawPtOperate(worldPt, color, size, drawGroup, worldMatrix);
		//保存		
		vecWorldCoord.push_back(worldPt);
	}
	else
	{
		vecWorldCoord.pop_back();
		//清除最后绘制的一个点
		int totalNum = drawGroup->getNumChildren();
		drawGroup->removeChild(totalNum - 1, 1);
		//绘制点
		InterfaceBaseDrawerFactory IBaseDrawerFactory;
		IBaseDrawer = IBaseDrawerFactory.create();
		IBaseDrawer->BaseDrawPtOperate(worldPt, color, size, drawGroup, worldMatrix);
		//保存
		vecWorldCoord.push_back(worldPt);
	}
}

void COsgDrawSinglePtHandler::BackSpaceReverseSinglePtOperate(std::vector<osg::Vec3d> &vecWorldCoord, osg::ref_ptr<osg::Group> drawGroup)
{
	int childNum = drawGroup->getNumChildren();

	if (childNum > 0)
	{
		drawGroup->removeChild(childNum - 1);
	}

	int totalNum = vecWorldCoord.size();
	IBaseDrawer->DeleteVecChild(totalNum - 1, vecWorldCoord);
}

void COsgDrawSinglePtHandler::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_SINGLE_PT);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(mEditSceneGroup);
		vecWorldCoord.clear();
	}
}