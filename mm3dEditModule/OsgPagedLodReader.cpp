#include "stdafx.h"
#include "OsgPagedLodReader.h"
#include "InterfaceOsgEditor.h"

osg::ref_ptr<osg::Node> COsgPagedLodReader::readPagedLodNode(osg::PagedLOD& pagedLod)
{
	//读取照片
	std::string childFileTitle = pagedLod.getFileName(1);
	std::string childFileName = inFilePath + childFileTitle;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(childFileName);
	return node;
}

osg::ref_ptr<osg::Group> COsgPagedLodReader::combinePagedLod(std::vector<std::string> vecPagedLodFileName)
{
	//把所有的pagedlod加入group
	osg::ref_ptr<osg::Group> group = new osg::Group;

	for (int i = 0; i < vecPagedLodFileName.size(); i++)
	{
		std::string fileFolder = vecPagedLodFileName[i];
		std::string fileName = inFilePath + fileFolder;
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*>(osgDB::readNodeFile((fileName)));

		if (!lod)
		{
			continue;
		}

		char strMsg[99];
		sprintf_s(strMsg, "%d\n", i);
		lod->setName(strMsg);
		lod->setFileName(1, fileFolder);
		lod->setDatabasePath("");
		group->addChild(lod);
	}

	return group;
}
