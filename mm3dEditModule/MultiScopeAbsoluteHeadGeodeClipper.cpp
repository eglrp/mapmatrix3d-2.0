#include "stdafx.h"
#include "MultiScopeAbsoluteHeadGeodeClipper.h"

bool CMultiScopeAbsoluteHeadGeodeClipper::ApplyWithTriangle(osg::Geometry* geom)
{
	bool isEmpty = true;

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

			CVertsTriangle vertsTriangle;
			vertsTriangle.pt1 = pt1;
			vertsTriangle.pt2 = pt2;
			vertsTriangle.pt3 = pt3;
			//ÅÐ¶ÏÊÇ·ñÔÚ·¶Î§ÄÚ
			bool isInPolygon = isInAreaJudger.isTriangleInAbsoluteScope(vertsTriangle);

			if (isInPolygon ^ isInside)
			{

			}
			else
			{
				newDrawElementsTriangle->push_back(index1);
				newDrawElementsTriangle->push_back(index2);
				newDrawElementsTriangle->push_back(index3);

				isEmpty = false;
			}
		}

		geom->setPrimitiveSet(k, newDrawElementsTriangle);
	}

	if (isEmpty)
	{
		return true;
	}
	else
	{
		return false;
	}

}
