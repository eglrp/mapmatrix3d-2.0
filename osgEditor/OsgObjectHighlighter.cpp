#include "stdafx.h"
#include "OsgObjectHighlighter.h"


void COsgObjectHighlighter::apply(osg::Geode& geode)
{
	int num = geode.getNumDrawables();

	for (int j = num - 1; j >= 0; --j)
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(j));
		apply(geom);

		vecHighLightGeometry.push_back(geom);
	}
	
}

void COsgObjectHighlighter::apply(osg::Geometry* geom)
{
	osg::Vec4dArray* colorArray = new osg::Vec4dArray;
	colorArray->push_back(highLightColor);
	geom->setColorArray(colorArray);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
}