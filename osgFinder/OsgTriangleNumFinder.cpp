#include "StdAfx.h"
#include "OsgTriangleNumFinder.h"

void COsgTriangleNumFinder::apply(osg::Geode& geode)
{
	for (int i = 0; i < geode.getNumDrawables(); i++)
	{
		osg::ref_ptr<osg::Geometry> geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));

		if (!geom)
			continue;

		for (int k = 0; k < geom->getNumPrimitiveSets(); k++)
		{
			osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
			osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
			osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
			int cnt = drawElementTriangle->getNumIndices();
			int triCnt = cnt / 3;
			numTri += triCnt;
		}
	}
}
