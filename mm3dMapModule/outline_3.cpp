#include "stdafx.h"
#include "outline_3.h"
#include "InterfaceOsgFinder.h"


namespace outline_3
{
	//输入构成三条线段 Pt1-Pt2,Pt2-Pt3,Pt3-Pt1,依次判断与高程H的交点
	DLL_API int computePoints(osg::Vec3d Pt1, osg::Vec3d Pt2, osg::Vec3d Pt3, double H, std::vector<osg::Vec3d>& vec)
	{
		int index = 0;
		osg::Vec3d p;
		std::vector<osg::Vec3d> vec1;
		vec1.clear();
		if ((Pt1.z() >= H && H >= Pt2.z()) || (Pt2.z() >= H && H >= Pt1.z()))
		{
			//求交点
			vec1.push_back(getInsection(Pt1, Pt2, H));
			if (vec1.size() == 2)
			{
				index = 1;
			}	
		}
		if ((Pt1.z() >= H && H >= Pt3.z()) || (Pt3.z() >= H && H >= Pt1.z()))
		{
			//求交点
			vec1.push_back(getInsection(Pt1, Pt3, H));
			if (vec1.size() == 2)
			{
				index = 2;
			}
		}
		if ((Pt3.z() >= H && H >= Pt2.z()) || (Pt2.z() >= H && H >= Pt3.z()))
		{
			//求交点
			vec1.push_back(getInsection(Pt3, Pt2, H));
			if (vec1.size() == 2)
			{
				index = 3;
			}
		}
		if (vec1.size() == 2)
		{
			vec.insert(vec.end(), vec1.begin(), vec1.end());
		}
		return index;
	}

	void computePointsInArea(osg::Vec3d Pt1, osg::Vec3d Pt2, osg::Vec3d Pt3, double H, std::vector<osg::Vec3d>& vec, std::vector<osg::Vec3d>& area)
	{
		int totalNum = area.size();
		std::vector<CL::Vec2d> pol;

		for (int i = 0; i < totalNum; i++)
		{
			pol.push_back(CL::Vec2d(area[i].x(), area[i].y()));
		}

		pol.push_back(CL::Vec2d(area[0].x(), area[0].y()));

		if (pol.size() == 0)
		{
			return;
		}
		osg::Vec3d p;
		std::vector<osg::Vec3d> vec1;
		vec1.clear();
		if ((Pt1.z() > H && H > Pt2.z()) || (Pt2.z() > H && H > Pt1.z()))
		{
			//求交点
			vec1.push_back(getInsection(Pt1, Pt2, H));
		}
		if ((Pt1.z() > H && H > Pt3.z()) || (Pt3.z() > H && H > Pt1.z()))
		{
			//求交点
			vec1.push_back(getInsection(Pt1, Pt3, H));
		}
		if ((Pt3.z() > H && H > Pt2.z()) || (Pt2.z() > H && H > Pt3.z()))
		{
			//求交点
			vec1.push_back(getInsection(Pt3, Pt2, H));
		}
		if (vec1.size() == 2)
		{
			if (Point_In_Polygon_2D(vec1[0].x(), vec1[0].y(), pol) && Point_In_Polygon_2D(vec1[1].x(), vec1[1].y(), pol))
			{
				vec.insert(vec.end(), vec1.begin(), vec1.end());
			}
		}
	}

	DLL_API int computeOutlinebyPoint(std::vector<osg::Vec3d>& vecpt, double H, osg::Matrix matrix, std::vector<osg::Vec3d>& vec)
	{
		return computePoints(vecpt[0], vecpt[1], vecpt[2], H, vec);
	}

	DLL_API void computeOutline(osg::Geode* geode, double H, osg::Matrix matrix, std::vector<osg::Vec3d>& vec)
	{
		if (!geode)
		{
			return;
		}
		int num = geode->getNumDrawables();



		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(j));

			if (!geom)
				continue; //得到可绘制结点的顶点序列 

			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			for (int k = 0; k < geom->getNumPrimitiveSets(); k++)
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
					pt1 = pt1 * matrix;

					double x2 = verts->at(index2).x();
					double y2 = verts->at(index2).y();
					double z2 = verts->at(index2).z();
					osg::Vec3d pt2(x2, y2, z2);
					pt2 = pt2 * matrix;

					double x3 = verts->at(index3).x();
					double y3 = verts->at(index3).y();
					double z3 = verts->at(index3).z();
					osg::Vec3d pt3(x3, y3, z3);
					pt3 = pt3 * matrix;
					computePoints(pt1, pt2, pt3, H, vec);
				}
			}
		}
	}


	DLL_API void computeOutlineInArea(osg::Geode* geode, double H, osg::Matrix matrix, std::vector<osg::Vec3d>& vec, std::vector<osg::Vec3d>& area)
	{
		if (!geode)
		{
			return;
		}
		int num = geode->getNumDrawables();



		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(j));

			if (!geom)
				continue; //得到可绘制结点的顶点序列 

			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			for (int k = 0; k < geom->getNumPrimitiveSets(); k++)
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
					pt1 = pt1 * matrix;

					double x2 = verts->at(index2).x();
					double y2 = verts->at(index2).y();
					double z2 = verts->at(index2).z();
					osg::Vec3d pt2(x2, y2, z2);
					pt2 = pt2 * matrix;

					double x3 = verts->at(index3).x();
					double y3 = verts->at(index3).y();
					double z3 = verts->at(index3).z();
					osg::Vec3d pt3(x3, y3, z3);
					pt3 = pt3 * matrix;
					computePointsInArea(pt1, pt2, pt3, H, vec, area);
				}
			}
		}
	}
}