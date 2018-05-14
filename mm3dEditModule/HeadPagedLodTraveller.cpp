#include "stdafx.h"
#include "HeadPagedLodTraveller.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceMFCExcute.h"

void CHeadPagedLodTraveller::apply(osg::PagedLOD& pagedLod)
{
	//统计百分比
	if (pagedLod.getNumParents() > 0)
	{
		int childNum = pagedLod.getParent(0)->getNumChildren();

		std::string Name = pagedLod.getName();

		if (Name != "")
		{
			int lodNum = atoi(Name.c_str());
			stProgressSetter.SetProgress(lodNum + 1, childNum);
		}
	}
	
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
		travelChildNode(pagedLod, node);			//访问下级
	}												

	traverse(pagedLod);
}

bool CHeadPagedLodTraveller::ApplyChildNode(osg::ref_ptr<osg::Node> node)
{
	return true;
}


void CHeadPagedLodTraveller::travelChildNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node)
{
	//继续访问
	std::string childFileTitle = pagedLod.getFileName(1);
	int posF = childFileTitle.find_last_of("\\");

	if (posF > 2 && posF != std::string::npos)												//Lod头
	{
		//访问下一级
		std::string fileName = pagedLod.getFileName(1);
		int posEnd = fileName.find_last_of('\\');
		std::string outFolder = fileName.substr(0, posEnd - 0 + 1);				//输出目录
																							//判断是否有clip
		std::string outFilePath1;
		osgPagedLodOutputer.CheckClipFolder(outFilePath, outFolder, outFilePath1);

		std::string inChildFileName = inFilePath + fileName;
		int pos = inChildFileName.rfind('\\') + 1;
		std::string inChildFilePath = inChildFileName.substr(0, pos);

		osg::ref_ptr<osg::Node> childNode = osgDB::readNodeFile(inChildFileName);

		//使用深拷贝复制保持递归
		osg::ref_ptr<CHeadPagedLodTraveller> pagedLodTravel = this->clone(inChildFilePath, outFilePath1);
		childNode->accept(*pagedLodTravel);
	}
	else
	{
		std::string childFileName = inFilePath + childFileTitle;

		int pos = childFileName.rfind('\\') + 1;
		std::string childFilePath = childFileName.substr(0, pos);
		osg::ref_ptr<CHeadPagedLodTraveller> pagedLodTravel = this->clone(childFilePath, outFilePath);
		node->accept(*pagedLodTravel);
	}
	
}

CHeadPagedLodTraveller* CHeadPagedLodTraveller::clone(std::string inFilePath, std::string outFilePath)
{
	return new CHeadPagedLodTraveller(inFilePath, outFilePath);
}