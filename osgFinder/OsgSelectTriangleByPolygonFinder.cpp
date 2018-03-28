#include "stdafx.h"
#include "OsgSelectTriangleByPolygonFinder.h"
#include "qcomm.h"

osg::Matrix COsgSelectTriangleByPolygonFinder::CalculateNodeMatrix(osg::Geode* geode)
{
	osg::Matrix matrix;
	osg::MatrixList matrixList = geode->getWorldMatrices();
	int matrixNum = matrixList.size();

	if (matrixNum != 0)
	{
		matrix = matrixList[matrixNum - 1];
	}

	return matrix;
}

void COsgSelectTriangleByPolygonFinder::apply(osg::Geode &geode)
{
	int num = geode.getNumDrawables();
	osg::Matrix geodeMatrix = CalculateNodeMatrix(&geode);

	for (int j = num - 1; j >= 0; --j)
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(j));

		if (!geom)
			return; //得到可绘制结点的顶点序列 

		ApplyWithTriangle(geom, geodeMatrix, vecScope);
	}

	traverse(geode);
}

bool COsgSelectTriangleByPolygonFinder::ApplyWithTriangle(osg::Geometry* geom, osg::Matrix worldMatrix, std::vector<CScope> vecScope)
{
	osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

	for (int k = 0; k < geom->getNumPrimitiveSets(); k++)
	{
		osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
		osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
		FindTriangleInPolygon(geom, verts, drawElementTriangle, worldMatrix, vecScope);
	}

	return true;
}

bool COsgSelectTriangleByPolygonFinder::IsTriangleInPolygon(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3, double minH, double maxH, std::vector<CL::Vec2d> pol)
{
	bool isInPolygon = false;

	if (Point_In_Polygon_2D(pt1.x(), pt1.y(), pol) ||
		Point_In_Polygon_2D(pt2.x(), pt2.y(), pol) ||
		Point_In_Polygon_2D(pt3.x(), pt3.y(), pol))
	{
		if (minH >= maxH)
		{
			isInPolygon = true;
		}
		else
		{
			if ((pt1.z() < maxH && pt1.z() > minH) || (pt2.z() < maxH && pt2.z() > minH) || (pt3.z() < maxH && pt3.z() > minH))
			{
				isInPolygon = true;
			}
		}
	}

	return isInPolygon;
}

bool COsgSelectTriangleByPolygonFinder::FindTriangleInPolygon(osg::Geometry* geom, osg::Vec3Array* verts, osg::ref_ptr<osg::DrawElements> drawElementTriangle, osg::Matrix worldMatrix, std::vector<CScope> vecScope)
{
	int cnt = drawElementTriangle->getNumIndices();
	int triCnt = cnt / 3;

	for (int w = 0; w < triCnt; w++)
	{
		//判断是否在范围内
		bool isInPolygon = false;
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

		for (int i = 0; i < vecScope.size(); i++)
		{
			std::vector<CL::Vec2d> pol;

			for (int t = 0; t < vecScope[i].vecCoord.size(); t++)
			{
				double x = vecScope[i].vecCoord[t].x();
				double y = vecScope[i].vecCoord[t].y();
				double z = vecScope[i].vecCoord[t].z();
				pol.push_back(CL::Vec2d(x, y));
			}

			double x = vecScope[i].vecCoord[0].x();
			double y = vecScope[i].vecCoord[0].y();
			double z = vecScope[i].vecCoord[0].z();
			pol.push_back(CL::Vec2d(x, y));

			double minH = vecScope[i].minH;
			double maxH = vecScope[i].maxH;

			if (Point_In_Polygon_2D(pt1.x(), pt1.y(), pol))
			{
				MAP_VERTS::iterator iter = m_MapInsideVerts.find(pt1.x());

				if (iter == m_MapInsideVerts.end())
				{
					if (minH == maxH)
					{
						m_MapInsideVerts[x1] = osg::Vec3d(x1, y1, z1);
					}
					else if(minH < pt1.z() && pt1.z() < maxH)
					{
						m_MapInsideVerts[x1] = osg::Vec3d(x1, y1, z1);
					}
				}
			}

			if (Point_In_Polygon_2D(pt2.x(), pt2.y(), pol))
			{
				MAP_VERTS::iterator iter = m_MapInsideVerts.find(pt2.x());

				if (iter == m_MapInsideVerts.end())
				{
					if (minH == maxH)
					{
						m_MapInsideVerts[x2] = osg::Vec3d(x2, y2, z2);
					}
					else if (minH < pt2.z() && pt2.z() < maxH)
					{
						m_MapInsideVerts[x2] = osg::Vec3d(x2, y2, z2);
					}
				}
			}

			if (Point_In_Polygon_2D(pt3.x(), pt3.y(), pol))
			{
				MAP_VERTS::iterator iter = m_MapInsideVerts.find(pt3.x());

				if (iter == m_MapInsideVerts.end())
				{
					if (minH == maxH)
					{
						m_MapInsideVerts[x3] = osg::Vec3d(x3, y3, z3);
					}
					else if (minH < pt3.z() && pt3.z() < maxH)
					{
						m_MapInsideVerts[x3] = osg::Vec3d(x3, y3, z3);
					}
				}
			}
		}
	}
	return true;
}