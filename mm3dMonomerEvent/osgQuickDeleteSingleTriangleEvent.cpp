// osgQuickDeleteSingleTriangleEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgQuickDeleteSingleTriangleEvent.h"
#include "InterfaceOsgFinder.h"

bool COsgQuickDeleteSingleTriangleEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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

			//快速删除
			osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
			StartQuickDelete(sceneNode, worldMatrix, worldPt);
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) >= 0)
		{

		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{

			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{

			}
		}
	}

	return false;
}

void COsgQuickDeleteSingleTriangleEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_QUICK_DELETE_SINGLE_TRIANGLE);

	if (isActivated == false && flag == true)
	{
		
	}

	if (isActivated && flag == false)																//刚激活
	{

	}
}


void COsgQuickDeleteSingleTriangleEvent::StartQuickDelete(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, osg::Vec3d worldPt)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
	std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(node);

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
					pt1 = pt1 * worldMatrix;

					double x2 = verts->at(index2).x();
					double y2 = verts->at(index2).y();
					double z2 = verts->at(index2).z();
					osg::Vec3d pt2(x2, y2, z2);
					pt2 = pt2 * worldMatrix;

					double x3 = verts->at(index3).x();
					double y3 = verts->at(index3).y();
					double z3 = verts->at(index3).z();
					osg::Vec3d pt3(x3, y3, z3);
					pt3 = pt3 * worldMatrix;

					if (!PtInTriangle(worldPt, pt1, pt2, pt3))//找到鼠标点击的面片
					{
						newDrawElementsTriangle->push_back(index1);
						newDrawElementsTriangle->push_back(index2);
						newDrawElementsTriangle->push_back(index3);
					}
				}

				geom->setPrimitiveSet(k, newDrawElementsTriangle);
			}
		}
	}
}

//判断点在三角面片内
bool COsgQuickDeleteSingleTriangleEvent::PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3)
{
	osg::Vec3d vec1 = t1 - pt;
	osg::Vec3d vec2 = t2 - pt;
	osg::Vec3d vec3 = t3 - pt;

	double ang1 = getAngleForVector(vec1, vec2);
	double ang2 = getAngleForVector(vec1, vec3);
	double ang3 = getAngleForVector(vec3, vec2);


	if (ang1 + ang2 + ang3 <= 360 + 5 && ang1 + ang2 + ang3 >= 360 - 5)
	{
		return true;
	}
	return false;
}

//计算向量夹角
double COsgQuickDeleteSingleTriangleEvent::getAngleForVector(osg::Vec3d& v1, osg::Vec3d& v2)
{
	double m1 = sqrt(v1.x()*v1.x() + v1.y()*v1.y() + v1.z()*v1.z());
	double m2 = sqrt(v2.x()*v2.x() + v2.y()*v2.y() + v2.z()*v2.z());

	double angle = acos((v1.x()*v2.x() + v1.y() * v2.y() + v1.z()*v2.z()) / (m1*m2)) * 180 / osg::PI;
	return angle;
}