#include "stdafx.h"
#include "osgToolFunc.h"
#include "InterfaceOsgFinder.h"
#include <osgViewer/Renderer>
#include "osgUtil/RayIntersector"

bool COsgToolFunc::getLodBottomLevelFile(float x, float y, osg::NodePath& np, osgViewer::Viewer *mViewer, std::string& m_tableName)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();

	//²éÕÒµ×²ãÊý¾Ý
	std::vector<std::string> vecChildFileName;

	for (int i = np.size() - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> (np[i]);

		if (lod)
		{
			IFinder->FindLodBottomLevelFile(lod, vecChildFileName);
			break;
		}
	}
	if (vecChildFileName.size() == 0)
		return false;
	if (vecChildFileName.size() > 1)
	{
		osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();
		osgViewer::Renderer *render = dynamic_cast<osgViewer::Renderer *>(camera->getRenderer());
		osgUtil::SceneView *sceneView = render->getSceneView(0);
		osg::Vec3 vScreen1(x, y, 0);
		osg::Vec3 vScreen2(x, y, 1);
		osg::Vec3 window1, window2;
		sceneView->projectWindowIntoObject(vScreen1, window1);
		sceneView->projectWindowIntoObject(vScreen2, window2);

		for (int j = 0; j < vecChildFileName.size(); ++j)
		{
			osg::Node* node = osgDB::readNodeFile(vecChildFileName[j]);
			if (node)
			{
				osg::ref_ptr<osgUtil::RayIntersector> ri = new osgUtil::RayIntersector(window1, window2 - window1);
				osgUtil::IntersectionVisitor iv1(ri);
				node->accept(iv1);
				if (ri.valid() && ri->containsIntersections())
				{
					m_tableName = vecChildFileName[j];
					break;
				}
			}
		}
	}
	else
	{
		m_tableName = vecChildFileName[0];
	}
	if (m_tableName.empty())
	{
		return false;
	}
	m_tableName = getTableName(m_tableName);
	replace_all(m_tableName, "+", "");
	transform(m_tableName.begin(), m_tableName.end(), m_tableName.begin(), ::tolower);
	return true;
}

std::string COsgToolFunc::getTableName(std::string fileName)
{
	int pos = fileName.rfind("\\");
	int pos1 = fileName.rfind("\/");
	int pos2 = fileName.rfind(".osgb");

	int posT = (pos > pos1) ? pos : pos1;
	return fileName.substr(posT + 1, pos2 - posT - 1);
}

void COsgToolFunc::replace_all(std::string& s, std::string const & t, std::string const & w)
{
	string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
	while (pos != string::npos) { // found   
		s.replace(pos, t_size, w);
		pos = s.find(t, pos + r_size);
	}
}