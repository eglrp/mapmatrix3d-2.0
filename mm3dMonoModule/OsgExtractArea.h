#pragma once
#include "InterfaceCOsgExtractArea.h"
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "ObjectAdder.h"

class COsgExtractArea : public InterfaceCOsgExtractArea
{
public:
	virtual void extractArea(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePathName, std::vector<CScope> vecScope);

	virtual osg::ref_ptr<osg::Node> extractObjectByArea(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope);

protected:
	CObjectAdder objectAdder;
};

