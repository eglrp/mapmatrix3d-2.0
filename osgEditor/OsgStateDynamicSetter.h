#pragma once
#include<osg/Geode>
#include<osg/NodeVisitor>
#include<osg/Geometry>
#include<osg/Texture2D>

class COsgStateDynamicSetter : public osg::NodeVisitor
{
public:
	COsgStateDynamicSetter::COsgStateDynamicSetter() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
	}
	
protected:
	virtual void apply(osg::Geode& geode);

	void apply(osg::StateSet* state);
};

