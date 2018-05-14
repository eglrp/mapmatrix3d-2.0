#pragma once
#include "InterfaceMFCExcute.h"
#include <osg/MatrixTransform>
#include "InterfaceOsgFinder.h"

class CFindTrans
{
public:
	bool findTrans(osg::ref_ptr<osg::Node> sceneNode);

	std::vector<osg::MatrixTransform*> getTrans() const;

private:
	std::vector<osg::MatrixTransform*> vecTrans;
};

