#include "stdafx.h"
#include "MultiScopeHeadGeodeNormalizeFlatter.h"


bool CMultiScopeHeadGeodeNormalizeFlatter::ApplyWithTriangle(osg::Geometry* geom)
{
	bool isEmpty = true;

	osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
	osg::ref_ptr<osg::Vec3Array> newVerts = new osg::Vec3Array;
	osg::Matrix iMatrix = matrix.inverse(matrix);

	for (int i = 0; i < verts->size(); i++)
	{
		osg::Vec3d pt = verts->at(i);
		pt = pt * matrix;

		bool isInPolygon = isInAreaJudger.isPtInArea(pt);

		if (!isInPolygon)
		{
			newVerts->push_back(verts->at(i));
			continue;
		}

		if (pt.z() > flattenHeight)
		{
			pt.z() = (pt.z() - flattenHeight) / (sceneMaxHeight - flattenHeight) * 0.5 + flattenHeight;
		}

		osg::Vec3d iPt = pt * iMatrix;
		newVerts->push_back(iPt);
	}

	geom->setVertexArray(newVerts);
	return false;
}