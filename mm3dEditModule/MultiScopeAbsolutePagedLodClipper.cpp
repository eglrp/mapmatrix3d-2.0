#include "stdafx.h"
#include "MultiScopeAbsolutePagedLodClipper.h"

bool CMultiScopeAbsolutePagedLodClipper::ApplyChildNode(osg::ref_ptr<osg::Node> node)
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

	CMultiScopeAbsoluteHeadGeodeClipper iveClipper(vecScope, matrix, isInside);
	node->accept(iveClipper);

	return true;
}

CHeadPagedLodTraveller* CMultiScopeAbsolutePagedLodClipper::clone(std::string inFilePath, std::string outFilePath)
{
	return new CMultiScopeAbsolutePagedLodClipper(vecScope, inFilePath, outFilePath, matrix, stProgressSetter, isInside);
}