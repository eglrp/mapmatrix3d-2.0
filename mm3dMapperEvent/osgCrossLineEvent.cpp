#include "stdafx.h"
#include "osgCrossLineEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceBaseDrawer.h"

void COsgCrossLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_CROSS_LINE);

	if (isActivated == false && flag == true)
	{

		ISigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecPoints1.clear();
		vecPoints2.clear();
		vecPtSel.clear();
		ISigEventCallBack->EmitSigShowOpsHint("曲线连通");
	}
}

bool COsgCrossLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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
			pickOperate(worldPt);
		}
	}	
	return false;
}

void COsgCrossLineEvent::pickOperate(osg::Vec3d& worldPt)
{
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	if (vecPtSel.empty())
	{
		int k = editLine.getGroupPoints(worldPt, 0.1, mapperGroup, vecPoints1);
		if (k != -1)
		{
			indexGroup1 = k;
			vecPtSel.push_back(worldPt);
			IDrawer->BaseDrawPtOperate(worldPt, osg::Vec4(0, 0, 1, 1), 3.5, tempGroup, worldMatrix);
		}
		return;
	}
	if (vecPtSel.size() == 1)
	{
		int k = editLine.getGroupPoints(worldPt, 0.1, mapperGroup, vecPoints2);
		if (k != -1)
		{
			indexGroup2 = k;
			vecPtSel.push_back(worldPt);
			ClearGroupResult(tempGroup);
			IDrawer->BaseDrawLineOperate(vecPtSel[0], worldPt, osg::Vec4(0, 0, 1, 1), 2.5, tempGroup, worldMatrix);

		}
		return;
	}
	if (vecPtSel.size() == 2)
	{
		editLine.getTerminal(worldPt, vecPoints1);
		vecPtSel.push_back(worldPt);

		IDrawer->BaseDrawPtOperate(worldPt, osg::Vec4(0, 0, 1, 1), 3.5, tempGroup, worldMatrix);
		return;
	}
	if (vecPtSel.size() == 3)
	{
		editLine.getTerminal(worldPt, vecPoints2);
		vecPtSel.push_back(worldPt);
		ClearGroupResult(tempGroup);

		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
		if (spChildCallback != NULL)
		{
			osg::ref_ptr<osg::Node> childNode1 = mapperGroup->getChild(indexGroup1);
			osg::ref_ptr<osg::Node> childNode2 = mapperGroup->getChild(indexGroup2);
			spChildCallback->RemoveChild(mapperGroup, childNode1);
			spChildCallback->RemoveChild(mapperGroup, childNode2);
		}

		std::vector < osg::Vec3d > vec1L, vec2L, vec1S, vec2S;
		std::vector<osg::Vec3d> vecNull;
		vecNull.clear();
		editLine.cutLine(vecPtSel[0], vecPtSel[2], vecPoints1, vecNull,vec1L, vec1S);
		editLine.cutLine(vecPtSel[1], worldPt, vecPoints2, vecNull, vec2L, vec2S);
		editLine.linkVector(vec1L, vec2L, vecPtSel);
		IDrawer->BaseDrawLineLoopOperate(vec1L, osg::Vec4(1, 0, 0, 1), 2.5, mapperGroup, worldMatrix);
		vecPtSel.clear();
	}
}