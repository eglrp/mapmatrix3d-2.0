// osgSimplifyObjectByPolygonEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgSimplifyObjectByPolygonEvent.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgClip.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgAlgorithm.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgExtractTexture.h"
#include "ThreadSafeUpdateCallback.h"
#include "cvLine.h"
#include <osgDB/WriteFile>

bool COsgSimplifyObjectByPolygonEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			vecWorldPt.push_back(worldPt);
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) <= 0)
		{
			//按多边形进行选中三角面
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
			std::vector<CScope> vecClipScope;
			bool isSuccess = GetClipScope(vecClipScope, vecScope, vecWorldPt);

			if (!isSuccess)
			{
				return false;
			}

			AccurateClipTriangleByPolygon(sceneNode, vecClipScope, worldMatrix);
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				vecWorldPt.pop_back();
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldPt.clear();
			}
		}
	}

	return false;
}

void COsgSimplifyObjectByPolygonEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_SIMPLIFY_OBJECT);

	if (isActivated == false && flag == true)
	{
		//消息置空
		vecWorldPt.clear();
	}

	if (isActivated && flag == false)																//刚激活
	{

	}
}

void COsgSimplifyObjectByPolygonEvent::AccurateClipTriangleByPolygon(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, osg::Matrix worldMatrix)
{
	//按多边形面积选择三角面
	double a = 0;
	double b = 0;
	double c = 0;
	double d = 0;
	CVPlane(vecWorldPt, a, b, c, d);
	osg::Vec3d beforeNormal(a, b, c);
	beforeNormal.normalize();
	osg::Vec3d afterNormal(0, 0, -1);

	osg::Vec3d rotationAxis = beforeNormal ^ afterNormal;
	double axisAngle = acos(beforeNormal * afterNormal);
	rotationAxis.normalize();
	osg::Quat quat(axisAngle, rotationAxis);
	osg::Matrix rotationMatrix;
	rotationMatrix.setRotate(quat);

	//根据范围选择三角面
	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->MultiplyTransMatrix(sceneNode, rotationMatrix);

	std::vector<osg::Vec3d> scopeRect = vecScope[0].vecCoord;

	for (int i = 0; i < scopeRect.size(); i++)
	{
		osg::Vec3d pt = scopeRect[i] * rotationMatrix;
		scopeRect[i] = pt;
	}

	vecScope[0].vecCoord = scopeRect;

	double minHeight = 0; double maxHeight = 0;
	InterfaceOsgAlgorithmFactory iOsgAlgorithmFactory;
	InterfaceOsgAlgorithm* iOsgAlgorithm = iOsgAlgorithmFactory.create();
	iOsgAlgorithm->FindMinMaxHeight(scopeRect, minHeight, maxHeight);

	vecScope[0].minH = minHeight - 1;
	vecScope[0].maxH = maxHeight + 1;

	//查找需要合并点的信息
	std::map<double, osg::Vec3d> mapSelectVerts;
	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	iOsgFinder->FindTriangleByPolygon(sceneNode, vecScope, mapSelectVerts, true);
	osg::Vec3d sumPt(0, 0, 0);

	for (std::map<double, osg::Vec3d>::iterator iter = mapSelectVerts.begin(); iter != mapSelectVerts.end(); iter++)
	{
		osg::Vec3d selectPt = iter->second;
		selectPt = selectPt;
		sumPt.x() = sumPt.x() + selectPt.x();
		sumPt.y() = sumPt.y() + selectPt.y();
		sumPt.z() = sumPt.z() + selectPt.z();
	}

	osg::Vec3d simplifyPt = sumPt / mapSelectVerts.size();

	//进行精确裁切
	InterfaceOsgClipFactory IClipFactory;
	InterfaceOsgClip* IClip = IClipFactory.create(BY_ACCURATE_POLYGON);
	IClip->clipAccurateOsgHeader(sceneNode, vecScope, false);
	std::vector<osg::Vec3d> insideVerts = IClip->getInsideVecVerts();
	osg::Matrix iRotationMatrix = rotationMatrix.inverse(rotationMatrix);

	//构建新的几何节点
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geode->addDrawable(geom);
	osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
	verts->push_back(simplifyPt);
	osg::Matrix iWorldMatrix = worldMatrix.inverse(worldMatrix);

	for (int i = 0; i < insideVerts.size(); i++)
	{
		insideVerts[i] = insideVerts[i] * iRotationMatrix * iWorldMatrix;
		verts->push_back(insideVerts[i]);
	}

	for (int i = 0; i < scopeRect.size(); i++)
	{
		osg::Vec3d pt = scopeRect[i] * iRotationMatrix * iWorldMatrix;
		verts->push_back(pt);
	}

	//创建Delaunay三角网对象
	osg::ref_ptr<osgUtil::DelaunayTriangulator> dt = new osgUtil::DelaunayTriangulator(verts.get());
	dt->setInputPointArray(verts);
	//生成三角网
	dt->triangulate();
	geom->addPrimitiveSet(dt->getTriangles());
	geom->setVertexArray(verts);
}


bool COsgSimplifyObjectByPolygonEvent::GetClipScope(std::vector<CScope> &vecClipScope, std::vector<CScope> vecScope, std::vector<osg::Vec3d> vecWorldCoord)
{
	if (vecScope.size() == 0)
	{
		if (vecWorldCoord.size() < 3)
		{
			InterfaceMFCExcuteFactory IExcuteFactory;
			InterfaceMFCExcute* IExcute = IExcuteFactory.create();
			IExcute->PopUpMessage("请先选择裁切范围");
			return false;
		}
		else
		{
			CScope scope;
			scope.vecCoord = vecWorldCoord;
			vecClipScope.push_back(scope);
		}
	}
	else
	{
		if (vecWorldCoord.size() >= 3)
		{
			vecClipScope = vecScope;
			CScope scope;
			scope.vecCoord = vecWorldCoord;
			vecClipScope.push_back(scope);
		}
		else
		{
			vecClipScope = vecScope;
		}
	}

	return true;
}

void COsgSimplifyObjectByPolygonEvent::StartSimplifyObjectByPolygon()
{
	
}