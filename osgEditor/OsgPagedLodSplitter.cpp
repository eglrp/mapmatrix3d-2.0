#include "StdAfx.h"
#include "OsgPagedLodSplitter.h"
#include "OsgPageLodSetter.h"

COsgPagedLodSplitter::COsgPagedLodSplitter(std::string InFileName) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	inFileName = InFileName;
}


COsgPagedLodSplitter::~COsgPagedLodSplitter(void)
{
}

void COsgPagedLodSplitter::SplitSceneLod(osg::ref_ptr<osg::Node> node, std::string inFileName)
{
	COsgPagedLodSplitter ive(inFileName);
	node->accept(ive);
}

std::string COsgPagedLodSplitter::ObtainTileFileName(std::string fileName)
{
	int pos;

	if (fileName.find("Clip") != -1)
	{
		pos = fileName.find("Clip");
	}
	else
	{
		pos = fileName.find("Tile");
	}

	int length = fileName.length();
	std::string tileFileName = ".\\" + fileName.substr(pos, length - pos);
	return tileFileName;
}

void COsgPagedLodSplitter::apply(osg::MatrixTransform& node)
{
	osg::ref_ptr<osg::Group> group = node.getChild(0)->asGroup();
	std::string blockName;

	if (group)
	{
		int numChild = group->getNumChildren();

		for (int i = 0; i < numChild; i++)
		{
			osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (group->getChild(i));

			if (pagedLod)
			{
				for (int j = 0; j < pagedLod->getNumFileNames(); j++)
				{
					std::string fileName = pagedLod->getFileName(j);

					if (fileName.size() > 0)
					{
						blockName = ObtainBlockName(fileName);
						std::string tileFileName = ObtainTileFileName(fileName);
						pagedLod->setFileName(j, tileFileName);
					}
				}
			}
		}
	}

	int pos = inFileName.rfind('\\');
	std::string inFilePath = inFileName.substr(0, pos);
	std::string transNodeFileName = inFilePath + blockName + "scene.osgb";

	osg::ref_ptr<osg::Node> sceneNode = &node;
	COsgPageLodSetter iveSetter("");
	sceneNode->accept(iveSetter);
	osgDB::writeNodeFile(*sceneNode, transNodeFileName);
}

std::string COsgPagedLodSplitter::ObtainBlockName(std::string fileName)
{
	int pos;

	if (fileName.find("Clip") != -1)
	{
		pos = fileName.find("Clip");
	}
	else
	{
		pos = fileName.find("Tile");
	}

	std::string blockName = fileName.substr(0, pos);

	return blockName;
}
