#include "stdafx.h"
#include "osgBreakoutLineEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceBaseDrawer.h"

void COsgBreakoutLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_BREAKOUT_LINE);

	if (isActivated == false && flag == true)
	{
		
		ISigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecPoints.clear();
		vecBackup.clear();
		ISigEventCallBack->EmitSigShowOpsHint("曲线打断");
	}
}

bool COsgBreakoutLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;
			worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			if (vecPoints.empty())
			{
				indexGroup = editLine.getGroupPoints(worldPt, 0.1, mapperGroup, vecPoints);
				if (indexGroup != -1 )//成功获得特征线点集
				{
					ptStart = worldPt;
					//标记点
					IDrawer->BaseDrawPtOperate(ptStart, osg::Vec4(0, 0, 1, 1), 3.5, tempGroup, worldMatrix);
					vecAddPoints.clear();
				}
			}
			else
			{
				editLine.getTerminal(worldPt, vecPoints);
				ptEnd = worldPt;
				std::vector < osg::Vec3d > vecReserveL, vecReserveS;
				vecAddPoints.insert(vecAddPoints.begin(), ptStart);
				vecAddPoints.push_back(ptEnd);
				editLine.cutLine(ptStart, ptEnd, vecPoints,vecAddPoints, vecReserveL, vecReserveS);
				//删除选中的特征线
				ClearGroupResult(tempGroup);
				osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
				if (spChildCallback != NULL)
				{
					osg::ref_ptr<osg::Node> childNode = mapperGroup->getChild(indexGroup);
					spChildCallback->RemoveChild(mapperGroup, childNode);
				}
				//画保留的特征线
				IDrawer->BaseDrawLineLoopOperate(vecReserveL, osg::Vec4(1, 0, 0, 1), 2.5, mapperGroup, worldMatrix);
				vecBackup.assign(vecPoints.begin(), vecPoints.end());
				vecPoints.clear();
			}
		}
		else if (IsRightClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;
			worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			//保证在两点之间才能插点
			if (vecPoints.size() > 0)
			{
				vecAddPoints.push_back(worldPt);
			}
			
			if (vecAddPoints.size() == 1)
			{
				IDrawer->BaseDrawLineOperate(worldPt, ptStart, osg::Vec4(0, 0, 1, 1), 2.5, tempGroup, worldMatrix);
			}
			else
				IDrawer->BaseDrawLineOperate(worldPt, vecAddPoints[vecAddPoints.size() - 2], osg::Vec4(0, 0, 1, 1), 2.5, tempGroup, worldMatrix);			
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
				if (spChildCallback != NULL)
				{
					int num = mapperGroup->getNumChildren();
					if (num>0)
					{
						osg::ref_ptr<osg::Node> childNode;
						childNode = mapperGroup->getChild(num - 1);
						spChildCallback->RemoveChild(mapperGroup, childNode);
					}
					
				}
				if (vecBackup.size()>0)
				{
					IDrawer->BaseDrawLineLoopOperate(vecBackup, osg::Vec4(1, 0, 0, 1), 2.5, mapperGroup, worldMatrix);
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				if (vecAddPoints.size()>0)
				{
					vecAddPoints.pop_back();
					tempGroup->removeChild(tempGroup->getNumChildren() - 1);
				}
			}
		}
	}
	return false;
}


