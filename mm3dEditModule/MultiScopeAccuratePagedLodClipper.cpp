#include "stdafx.h"
#include "MultiScopeAccuratePagedLodClipper.h"
#include "MultiScopeAccurateHeadGeodeClippper.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgFinder.h"

bool CMultiScopeAccuratePagedLodClipper::ApplyChildNode(osg::ref_ptr<osg::Node> node)
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

	CMultiScopeAccurateHeadGeodeClippper iveClipper(vecScope, matrix, isInside);
	node->accept(iveClipper);
	return true;
}


CHeadPagedLodTraveller* CMultiScopeAccuratePagedLodClipper::clone(std::string inFilePath, std::string outFilePath)
{
	return new CMultiScopeAccuratePagedLodClipper(vecScope, inFilePath, outFilePath, matrix, stProgressSetter, isInside);
}