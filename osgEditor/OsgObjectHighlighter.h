#pragma once
#include<osg/Geode>
#include<osg/NodeVisitor>
#include<osg/Geometry>

class COsgObjectHighlighter : public osg::NodeVisitor
{
public:
	COsgObjectHighlighter(osg::Vec4d HighLightColor) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		highLightColor = HighLightColor;
	}	

public:
	std::vector<osg::ref_ptr<osg::Geometry>> vecHighLightGeometry;

protected:
	virtual void apply(osg::Geode& geode);

	virtual	void apply(osg::Geometry* geom);

protected:
	osg::Vec4d highLightColor;
};

