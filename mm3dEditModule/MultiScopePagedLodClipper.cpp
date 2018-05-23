#include "stdafx.h"
#include "MultiScopePagedLodClipper.h"
#include "MultiScopeHeadGeodeClipper.h"
#include "InterfaceOsgFinder.h"

bool CMultiScopePagedLodClipper::ApplyChildNode(osg::ref_ptr<osg::Node> node)
{
	//查找三角形个数
	int numTri = 0;
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	IFinder->FindTriangleNum(node, numTri);

	if (numTri == 0)
	{
		return false;
	}

	CMultiScopeHeadGeodeClipper iveClipper(vecScope, matrix, isInside);
	node->accept(iveClipper);

	return true;
}


CHeadPagedLodTraveller* CMultiScopePagedLodClipper::clone(std::string inFilePath, std::string outFilePath)
{
	return new CMultiScopePagedLodClipper(vecScope, inFilePath, outFilePath, matrix, stProgressSetter, isInside);
}