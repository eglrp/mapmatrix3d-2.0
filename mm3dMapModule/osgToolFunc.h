#pragma once
#include <string>
#include <osg\Node>
#include <osgViewer\Viewer>

class _declspec(dllexport) COsgToolFunc
{
public:
	bool getLodBottomLevelFile(float x, float y, osg::NodePath& np, osgViewer::Viewer *mViewer, std::string& m_tableName);

protected:
	std::string getTableName(std::string fileName);

	void replace_all(std::string& s, std::string const & t, std::string const & w);
};