#include "stdafx.h"
#include "MultiScopePagedLodNormalizeFlatter.h"
#include "MultiScopeHeadGeodeNormalizeFlatter.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"

bool CMultiScopePagedLodNormalizeFlatter::ApplyChildNode(osg::ref_ptr<osg::Node> node)
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

	CMultiScopeHeadGeodeNormalizeFlatter iveClipper(vecScope, matrix, sceneMaxH, flattenHeight);
	node->accept(iveClipper);

	InterfaceOsgEditorFactory iOsgEditorFactory;
	InterfaceOsgEditor* iOsgEditor = iOsgEditorFactory.create();
	iOsgEditor->ReCalPagedLodCenter(node);

	return true;
}

CHeadPagedLodTraveller* CMultiScopePagedLodNormalizeFlatter::clone(std::string inFilePath, std::string outFilePath)
{
	return new CMultiScopePagedLodNormalizeFlatter(vecScope, inFilePath, outFilePath, matrix, flattenHeight, sceneMaxH, stProgress);
}
