#include "StdAfx.h"
#include "qcomm.h"
#include "OsgPagedLodBottomLevelFinder.h"
#include "InterfaceMFCExcute.h"


COsgPagedLodBottomLevelFinder::COsgPagedLodBottomLevelFinder(std::vector<std::string>* VecBottomLevelPagedLodFileName) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	vecBottomLevelPagedLodFileName = VecBottomLevelPagedLodFileName;
}

COsgPagedLodBottomLevelFinder::~COsgPagedLodBottomLevelFinder(void)
{

}

void COsgPagedLodBottomLevelFinder::apply(osg::PagedLOD& pagedLod)
{
	InterfaceMFCExcuteFactory IMfcExcuteFactory;
	InterfaceMFCExcute* IMfcExcute = IMfcExcuteFactory.create();
	
	int numFileName = pagedLod.getNumFileNames();

	for (int i = 0; i < numFileName; i++)
	{
		std::string pagedLodFileTitle = pagedLod.getFileName(i);

		IMfcExcute->MFCPeekMessage();			//刷新进度条

		if (pagedLodFileTitle.size() > 0)
		{
			std::string databasePath = pagedLod.getDatabasePath();
			std::string pagedLodFileName = databasePath + pagedLodFileTitle;

			if (pagedLodFileName.size() == 0)
			{
				continue;
			}

			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(pagedLodFileName);

			if (node)					//找到下一层
			{
				node->setName(pagedLodFileTitle.c_str());
				osg::ref_ptr<osg::Geode> geode = node->asGeode();
				osg::ref_ptr<osg::PagedLOD> childPagedLod = dynamic_cast<osg::PagedLOD*>(node.get());
				osg::ref_ptr<osg::Group> childGroup = node->asGroup();

				if (geode)
				{
					vecBottomLevelPagedLodFileName->push_back(pagedLodFileName);
				}
				else if (childPagedLod)					//继续往下一层
				{
					COsgPagedLodBottomLevelFinder iveFinder(vecBottomLevelPagedLodFileName);
					childPagedLod->accept(iveFinder);
				}
				else if (childGroup)
				{
					for (int j = 0; j < childGroup->getNumChildren(); j++)
					{
						osg::ref_ptr<osg::PagedLOD> childPagedLod = dynamic_cast<osg::PagedLOD*> (childGroup->getChild(j));

						if (childPagedLod)
						{
							COsgPagedLodBottomLevelFinder iveFinder(vecBottomLevelPagedLodFileName);
							childPagedLod->accept(iveFinder);
						}
					}
				}
			}
			else
			{
				std::string pagedLodTitle = pagedLod.getName();

				if (pagedLodTitle.size() > 0)
				{
					std::string databasePath = pagedLod.getDatabasePath();
					std::string pagedLodFileName = databasePath + pagedLodTitle;

					if (pagedLodFileName.size() == 0)
					{
						continue;
					}

					vecBottomLevelPagedLodFileName->push_back(pagedLodFileName);
				}
				
			}
		}
	}
}
