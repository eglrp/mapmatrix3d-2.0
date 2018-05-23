#include "stdafx.h"
#include "MultiScopePagedLodFlatter.h"
#include "MultiScopeHeadGeodeflatter.h"
#include "InterfaceOsgFinder.h"

bool CMultiScopePagedLodFlatter::ApplyChildNode(osg::ref_ptr<osg::Node> node)
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

	CMultiScopeHeadGeodeflatter iveClipper(vecScope, matrix, flattenHeight);
	node->accept(iveClipper);

	return true;
}


CHeadPagedLodTraveller* CMultiScopePagedLodFlatter::clone(std::string inFilePath, std::string outFilePath)
{
	return new CMultiScopePagedLodFlatter(vecScope, inFilePath, outFilePath, matrix, flattenHeight, stProgress);
}
