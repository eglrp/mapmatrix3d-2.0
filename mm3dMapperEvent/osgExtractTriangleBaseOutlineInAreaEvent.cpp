// osgExtractTriangleBaseOutlineInAreaEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "InterfaceBaseDrawer.h"
#include "osgExtractTriangleBaseOutlineInAreaEvent.h"
#include "InterfaceCOsgExtractObject.h"
#include "outline_3.h"
#include "OsgTriangleFinder.h"
#include <osgDB/ReadFile>
#include "Geometry.h"

bool COsgExtractTriangleBaseOutLineInAreaEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (isActivated2)
		{

			handleExtractOutline(ea);
		}
		else if (isActivated1)
		{
			handleDrawArea(ea);
		}
	}
	return false;
}

void COsgExtractTriangleBaseOutLineInAreaEvent::handleDrawArea(const osgGA::GUIEventAdapter &ea)
{
	if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
	{
		CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
		worldPt = pickResult.worldPt;
		worldMatrix = pickResult.matrix;
		osg::NodePath np = pickResult.nodePath;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return;
		}
		vecWorldCoord.push_back(worldPt);
	}
	else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
	{
		if (vecWorldCoord.size() < 3)
		{
			return;
		}
	}
	else if (IsKeyPress(ea))
	{
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
		{
			if (vecWorldCoord.size() > 0)
			{
				vecWorldCoord.pop_back();
			}
		}
	}

}

void COsgExtractTriangleBaseOutLineInAreaEvent::handleExtractOutline(const osgGA::GUIEventAdapter &ea)
{
	if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
	{
		CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
		worldPt = pickResult.worldPt;
		worldMatrix = pickResult.matrix;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return;
		}
		std::vector<osg::Vec3d> vecLine;
		extractHeight = worldPt.z();
		if (vecPoints.empty())
		{
			osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
			getTrianglePointsInScope(sceneNode, vecWorldCoord, vecPoints);
			//ISigEventCallBack->EmitSigShowOpsHint("提取失败，请先设置提取范围");

		}
		if (vecPoints.empty())
		{
			ISigEventCallBack->EmitSigShowOpsHint("提取失败，请先设置提取范围");
			return;
		}
		getOutlineByHeight(vecPoints, extractHeight, vecLine);
		if (vecLine.size() > 2)
		{
			InterfaceBaseDrawerFactory IDrawerFactory;
			InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
			osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;

			for (int i = 0; i < vecLine.size(); i += 2)
			{
				IDrawer->BaseDrawLineOperate(vecLine[i], vecLine[i + 1], osg::Vec4(1, 0, 0, 1), 3.5, outlineGroup, worldMatrix);
			}
			outlineGroup->setName("轮廓线");
			mapperGroup->addChild(outlineGroup);

			isOutlineExist = true;
			char strMsg[299];
			sprintf(strMsg, "范围内提取：按BackSpace删除上一个轮廓线，按Z撤销全部，按键盘上下箭头调整提取高度，按ESC退出事件        当前高度：%f米", extractHeight);
			ISigEventCallBack->EmitSigShowOpsHint(strMsg);
		}
	}
	else if (IsKeyPress(ea))
	{
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
		{
			if (mapperGroup->getNumChildren() > 0)
			{
				mapperGroup->removeChild(mapperGroup->getNumChildren() - 1);
				isOutlineExist = false;
				ISigEventCallBack->EmitSigShowOpsHint("范围内提取：请按Ctrl + 鼠标左键选定高度即可自动提取轮廓线");
			}
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
		{
			ClearGroupResult(mapperGroup);
			ISigEventCallBack->EmitSigShowOpsHint("范围内提取：请按Ctrl + 鼠标左键选定高度即可自动提取轮廓线");
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
		{
			if (vecPoints.size() != 0 && isOutlineExist)
			{
				extractHeight += 0.01;
				std::vector<osg::Vec3d> vecLine;
				getOutlineByHeight(vecPoints, extractHeight, vecLine);

				if (vecLine.size() > 2)
				{
					if (mapperGroup->getNumChildren() > 0)
					{
						mapperGroup->removeChild(mapperGroup->getNumChildren() - 1);
					}
					InterfaceBaseDrawerFactory IDrawerFactory;
					InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
					osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;

					for (int i = 0; i < vecLine.size(); i += 2)
					{
						IDrawer->BaseDrawLineOperate(vecLine[i], vecLine[i + 1], osg::Vec4(1, 0, 0, 1), 3.5, outlineGroup, worldMatrix);
					}

					mapperGroup->addChild(outlineGroup);
					char strMsg[299];
					sprintf(strMsg, "范围内提取：按BackSpace删除上一个轮廓线，按Z撤销全部，按键盘上下箭头调整提取高度，按ESC退出事件        当前高度：%f米", extractHeight);
					ISigEventCallBack->EmitSigShowOpsHint(strMsg);
				}
			}
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
		{
			if (vecPoints.size() != 0 && isOutlineExist)
			{
				extractHeight -= 0.01;
				std::vector<osg::Vec3d> vecLine;
				getOutlineByHeight(vecPoints, extractHeight, vecLine);

				if (vecLine.size() > 2)
				{
					if (mapperGroup->getNumChildren() > 0)
					{
						mapperGroup->removeChild(mapperGroup->getNumChildren() - 1);
					}
					InterfaceBaseDrawerFactory IDrawerFactory;
					InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
					osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;

					for (int i = 0; i < vecLine.size(); i += 2)
					{
						IDrawer->BaseDrawLineOperate(vecLine[i], vecLine[i + 1], osg::Vec4(1, 0, 0, 1), 3.5, outlineGroup, worldMatrix);
					}

					mapperGroup->addChild(outlineGroup);
					char strMsg[299];
					sprintf(strMsg, "范围内提取：按BackSpace删除上一个轮廓线，按Z撤销全部，按键盘上下箭头调整提取高度，按ESC退出事件        当前高度：%f米", extractHeight);
					ISigEventCallBack->EmitSigShowOpsHint(strMsg);
				}
			}
		}
	}
}

void COsgExtractTriangleBaseOutLineInAreaEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated1 = isActivate(vecOpState, OPS_EXTRACT_TRIANGLE_OUTLINE_DRAW_AREA);
	isActivated2 = isActivate(vecOpState, OPS_EXTRACT_TRIANGLE_OUTLINE_IN_AREA);

	isActivated = false;
	if (isActivated1 || isActivated2)
	{
		isActivated = true;
	}


	if (isActivated == false && flag == true)//刚不激活
	{
		ISigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{

	}
	if (isActivated1)
	{
		ISigEventCallBack->EmitSigShowOpsHint("范围内提取：请按Ctrl + 鼠标左键圈定提取范围，按BackSpace键回退修改，双击鼠标左键确定范围");
		ClearGroupResult(tempGroup);
		isDrawArea = true;
		vecPoints.clear();
		vecWorldCoord.clear();
	}
	else if (isActivated2)
	{
		if (vecWorldCoord.size() >= 3)
		{
			ISigEventCallBack->EmitSigShowOpsHint("范围内提取：请按Ctrl + 鼠标左键选定高度即可自动提取轮廓线");
			InterfaceBaseDrawerFactory IDrawerFactory;
			InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
			IDrawer->BaseDrawLineLoopOperate(vecWorldCoord, osg::Vec4(1, 0, 1, 1), 2.5, tempGroup, worldMatrix);
		}
		else
		{
			ISigEventCallBack->EmitSigShowOpsHint("范围内提取：提取范围错误");
			isActivated = false;
		}
	}

}


void COsgExtractTriangleBaseOutLineInAreaEvent::getTrianglePointsInScope(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d>& vecWorldCoord, std::vector<osg::Vec3d>& vecPoints)
{
	std::vector<CScope> vecClipScope;
	CScope scope;
	scope.vecCoord = vecWorldCoord;
	vecClipScope.push_back(scope);
	vecPoints.clear();
	osg::ref_ptr<osg::MatrixTransform> accurateNode = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	InterfaceCOsgExtractObjectFactory iOsgExtractObjectFactory;
	InterfaceCOsgExtractObject* iOsgExtractObject = iOsgExtractObjectFactory.create();
	bool isSuccess = iOsgExtractObject->AccurateClipBottomScene(sceneNode, vecClipScope, trans);

	if (!isSuccess)
	{
		return;
	}
	osg::Matrix transMatrix = trans->getMatrix();

	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
	std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(trans);

	for (size_t i = 0; i < vecGeode.size(); ++i)
	{
		if (!vecGeode[i])
		{
			continue;
		}

		int num = vecGeode[i]->getNumDrawables();

		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(vecGeode[i]->getDrawable(j));

			if (!geom)
				continue; //得到可绘制结点的顶点序列 

			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			for (size_t k = 0; k < geom->getNumPrimitiveSets(); k++)
			{
				osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
				osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
				osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
				int cnt = drawElementTriangle->getNumIndices();
				int triCnt = cnt / 3;

				for (int w = 0; w < triCnt; w++)
				{
					int index1 = drawElementTriangle->index(w * 3);
					int index2 = drawElementTriangle->index(w * 3 + 1);
					int index3 = drawElementTriangle->index(w * 3 + 2);

					double x1 = verts->at(index1).x();
					double y1 = verts->at(index1).y();
					double z1 = verts->at(index1).z();
					osg::Vec3d pt1(x1, y1, z1);
					pt1 = pt1 * transMatrix;

					double x2 = verts->at(index2).x();
					double y2 = verts->at(index2).y();
					double z2 = verts->at(index2).z();
					osg::Vec3d pt2(x2, y2, z2);
					pt2 = pt2 * transMatrix;

					double x3 = verts->at(index3).x();
					double y3 = verts->at(index3).y();
					double z3 = verts->at(index3).z();
					osg::Vec3d pt3(x3, y3, z3);
					pt3 = pt3 * transMatrix;

					osg::Vec3d p1 = pt1;
					vecPoints.push_back(p1);
					osg::Vec3d p2 = pt2;
					vecPoints.push_back(p2);
					osg::Vec3d p3 = pt3;
					vecPoints.push_back(p3);

				}
			}
		}
	}
}

void COsgExtractTriangleBaseOutLineInAreaEvent::getOutlineByHeight(std::vector<osg::Vec3d>&vecPoints, double H, std::vector<osg::Vec3d>&vecLine)
{
	for (size_t i = 0; i < vecPoints.size() - 2; i += 3)
	{
		std::vector<osg::Vec3d> vec1;
		vec1.clear();
		if ((vecPoints[i].z() > H && H > vecPoints[i + 1].z()) || (vecPoints[i + 1].z() > H && H > vecPoints[i].z()))
		{
			//求交点
			vec1.push_back(getInsection(vecPoints[i], vecPoints[i + 1], H));
		}
		if ((vecPoints[i].z() > H && H > vecPoints[i + 2].z()) || (vecPoints[i + 2].z() > H && H > vecPoints[i].z()))
		{
			//求交点
			vec1.push_back(getInsection(vecPoints[i], vecPoints[i + 2], H));
		}
		if ((vecPoints[i + 2].z() > H && H > vecPoints[i + 1].z()) || (vecPoints[i + 1].z() > H && H > vecPoints[i + 2].z()))
		{
			//求交点
			vec1.push_back(getInsection(vecPoints[i + 2], vecPoints[i + 1], H));
		}
		if (vec1.size() == 2)
		{
			vecLine.insert(vecLine.end(), vec1.begin(), vec1.end());
		}
	}
}