#include "stdafx.h"
#include "Selector.h"
#include "InterfaceOsgFinder.h"
#include <osg/ComputeBoundsVisitor>

osg::ref_ptr<osg::Node> CSelector::findSelPagedLod(osg::ref_ptr<osg::Node> node, osg::Vec3d worldPt, osg::Matrix worldMatrix)
{
	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	std::vector<osg::PagedLOD*> vecPagedLod;
	iOsgFinder->FindPagedLod(node, vecPagedLod);

	if (vecPagedLod.size() == 0)
	{
		return false;
	}

	for (int i = 0; i < vecPagedLod.size(); i++)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = vecPagedLod[i];
		std::string name = pagedLod->getName();

		if (name.find("object") == std::string::npos)
		{
			continue;
		}

		osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
		trans->setMatrix(worldMatrix);
		trans->addChild(pagedLod);

		if (isPtInScope(worldPt, trans))
		{
			return pagedLod;
		}
	}

	return NULL;
}

bool CSelector::isPtInScope(osg::Vec3d worldPt, osg::ref_ptr<osg::Node> node)
{
	osg::ComputeBoundsVisitor ive;
	node->accept(ive);
	osg::BoundingBox bb = ive.getBoundingBox();
	return bb.contains(worldPt);
}

