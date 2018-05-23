#include "stdafx.h"
#include "OsgObjectCancelHighLighter.h"


void COsgObjectCancelHighLighter::apply(osg::Geode& geode)
{
	int num = geode.getNumDrawables();

	for (int j = num - 1; j >= 0; --j)
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(j));
		apply(geom);
	}
}


void COsgObjectCancelHighLighter::apply(osg::Geometry* geom)
{
	geom->setColorBinding(osg::Geometry::BIND_OFF);
}
