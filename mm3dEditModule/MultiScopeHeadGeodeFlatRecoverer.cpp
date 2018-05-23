#include "stdafx.h"
#include "MultiScopeHeadGeodeFlatRecoverer.h"
#include "IsInAreaJudger.h"

bool CMultiScopeHeadGeodeFlatRecoverer::ApplyWithTriangle(osg::Geometry* geom)
{
	bool isEmpty = true;

	osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
	osg::ref_ptr<osg::Vec3Array> newVerts = new osg::Vec3Array;
	osg::Matrix iMatrix = matrix.inverse(matrix);

	for (int i = 0; i < verts->size(); i++)
	{
		osg::Vec3d pt = verts->at(i);
		pt = pt * matrix;

		isInAreaJudger.setScope(vecScope);
		bool bIsInPPolygon = isInAreaJudger.isPtInArea(pt);

		if (!bIsInPPolygon)
		{
			newVerts->push_back(verts->at(i));
			continue;
		}

		isInAreaJudger.setScope(vecPreFlatArea);
		bool bIsInPrePolygon = isInAreaJudger.isPtInArea(pt);

		if (!bIsInPrePolygon)
		{
			newVerts->push_back(verts->at(i));
			continue;
		}

		double reCoverHeight = (pt.z() - flattenHeight) / 0.5 * (sceneMaxHeight - flattenHeight) + flattenHeight;

		if (reCoverHeight > flattenHeight)
		{
			pt.z() = reCoverHeight;
		}

		osg::Vec3d iPt = pt * iMatrix;
		newVerts->push_back(iPt);
	}

	geom->setVertexArray(newVerts);
	return false;
}

