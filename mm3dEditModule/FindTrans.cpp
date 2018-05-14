#include "stdafx.h"
#include "FindTrans.h"

bool CFindTrans::findTrans(osg::ref_ptr<osg::Node> sceneNode)
{
	//²éÕÒtrans½Úµã
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	IFinder->FindTransNode(sceneNode, vecTrans);

	if (vecTrans.size() == 0)
	{
		return false;
	}

	return true;
}

std::vector<osg::MatrixTransform*> CFindTrans::getTrans() const
{
	return vecTrans;
}
