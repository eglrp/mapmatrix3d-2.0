#include "stdafx.h"
#include "ChildPagedLodTraveller.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceMFCExcute.h"

void CChildPagedLodTraveller::apply(osg::PagedLOD& pagedLod)
{
	std::string fileName = pagedLod.getFileName(1);

	osg::ref_ptr<osg::Node> node = osgPagedLodReader.readPagedLodNode(pagedLod);

	if (!node)
	{
		return;
	}

	bool isSuccess = ApplyChildNode(node);

	if (!isSuccess)
	{
		return;
	}

	isSuccess = osgPagedLodOutputer.outputPagedLodNode(pagedLod, node);

	if (isSuccess)
	{
		travelChildNode(pagedLod, node);
	}

}

bool CChildPagedLodTraveller::ApplyChildNode(osg::ref_ptr<osg::Node> node)
{
	return true;
}

void CChildPagedLodTraveller::travelChildNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node)
{
	//¼ÌÐø·ÃÎÊ
	std::string childFileTitle = pagedLod.getFileName(1);
	std::string childFileName = inFilePath + childFileTitle;

	int pos = childFileName.rfind('\\') + 1;
	std::string childFilePath = childFileName.substr(0, pos);

	CChildPagedLodTraveller ive(childFilePath, outFilePath);
	node->accept(ive);
}
