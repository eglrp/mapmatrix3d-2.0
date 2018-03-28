#pragma once
#include "OSG_Plus.h"

class COsgPropertyNodeFinder : public osg::NodeVisitor
{
public:
	COsgPropertyNodeFinder(std::string ObjectPropertyName, std::string ObjectPropertyValue) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		objectPropertyName = ObjectPropertyName;
		objectPropertyValue = ObjectPropertyValue;
		objectNode = NULL;	
	}

	osg::ref_ptr<osg::Node> getObjectNode();

protected:
	virtual void apply(osg::Group &group)
	{
		std::string name = group.getName();

		if (name.find("Property:") != std::string::npos)
		{
			if (name.find(objectPropertyName) != std::string::npos && name.find(objectPropertyValue) != std::string::npos)
			{
				objectNode = &group;
			}
		}

		traverse(group);
	}

protected:
	std::string objectPropertyName;

	std::string objectPropertyValue;

	osg::ref_ptr<osg::Node> objectNode;
};

