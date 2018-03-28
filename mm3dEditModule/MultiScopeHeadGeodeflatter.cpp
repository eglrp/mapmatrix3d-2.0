#include "stdafx.h"
#include "MultiScopeHeadGeodeflatter.h"
#include "IsInAreaJudger.h"

bool CMultiScopeHeadGeodeflatter::ApplyWithTriangle(osg::Geometry* geom)
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

			//ÅÐ¶ÏÊÇ·ñÔÚ·¶Î§ÄÚ
			CVertsTriangle vertsTri;
			vertsTri.pt1 = pt1;
			vertsTri.pt2 = pt2;
			vertsTri.pt3 = pt3;
			bool isInPolygon = isInAreaJudger.isTriangleInScope(vertsTri);

			osg::Matrix iMatrix = matrix.inverse(matrix);

			if (isInPolygon ^ true)
			{
				
			}
			else
			{
				newDrawElementsTriangle->push_back(index1);
				newDrawElementsTriangle->push_back(index2);
				newDrawElementsTriangle->push_back(index3);

				pt1.z() = flattenHeight;
				pt2.z() = flattenHeight;
				pt3.z() = flattenHeight;

				osg::Vec3d iPt1 = pt1 * iMatrix;
				osg::Vec3d iPt2 = pt2 * iMatrix;
				osg::Vec3d iPt3 = pt3 * iMatrix;

				float* pIndex1 = verts->at(index1).ptr();
				float* pIndex2 = verts->at(index2).ptr();
				float* pIndex3 = verts->at(index3).ptr();

				pIndex1[0] = iPt1.x();
				pIndex1[1] = iPt1.y();
				pIndex1[2] = iPt1.z();

				pIndex2[0] = iPt2.x();
				pIndex2[1] = iPt2.y();
				pIndex2[2] = iPt2.z();

				pIndex3[0] = iPt3.x();
				pIndex3[1] = iPt3.y();
				pIndex3[2] = iPt3.z();
			}
		}

		geom->setPrimitiveSet(k, newDrawElementsTriangle);
	}

	return false;
}
