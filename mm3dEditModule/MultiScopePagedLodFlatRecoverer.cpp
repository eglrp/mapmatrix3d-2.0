#include "stdafx.h"
#include "MultiScopePagedLodFlatRecoverer.h"
#include "InterfaceOsgFinder.h"
#include "MultiScopeHeadGeodeFlatRecoverer.h"

bool CMultiScopePagedLodFlatRecoverer::ApplyChildNode(osg::ref_ptr<osg::Node> node)
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

	CMultiScopeHeadGeodeFlatRecoverer iveClipper(vecScope, vecPreVecScope, matrix, sceneMaxH, flattenHeight);
	node->accept(iveClipper);

	return true;
}

CHeadPagedLodTraveller* CMultiScopePagedLodFlatRecoverer::clone(std::string inFilePath, std::string outFilePath)
{
	return new CMultiScopePagedLodFlatRecoverer(vecScope, vecPreVecScope, inFilePath, outFilePath, matrix, flattenHeight, sceneMaxH, stProgressSetter);
}