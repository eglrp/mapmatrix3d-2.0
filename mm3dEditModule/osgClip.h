#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"
#include "InterfaceOsgClip.h"
#include "StProgressSetter.h"
#include "LogShower.h"
#include "ObtainSelTileFileName.h"

class COsgClipper : public InterfaceOsgClip
{
public:
	virtual bool clipSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope, 
		CStProgressSetter stProgressSetter, bool isInside);		//按范围删除

	virtual bool clipAccurateOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, bool isInside,bool bAccurate = true);
	
	virtual bool clipOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, bool isInside);																		//裁剪头结点

	virtual void obtainSelTileName(std::vector<CScope> vecScope, osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix, std::vector<std::string> &vecHalfInPagedLodFileName, std::vector<std::string> &vecTotalInPagedLodFileName);

	virtual void obtainSelTileNamebyScope(std::vector<CScope> vecScope, osg::ref_ptr<osg::Group> mGroup, osg::Matrix matrix, std::vector<std::string> &vecHalfInPagedLodFileName, std::vector<std::string> &vecTotalInPagedLodFileName);

	virtual std::vector<osg::Vec3d> getInsideVecVerts();

protected:
	virtual void clipHalfInPolygonTile(osg::ref_ptr<osg::Node> node, std::vector<CScope> VecScope, std::string InFilePath, std::string OutFilePath, osg::Matrix Matrix,
		CStProgressSetter StProgressSetter, bool IsInside) {}

	virtual void changePercentageBarInterval(int &halfPercentInterval, int &PercentInterval, bool isInside);

protected:
	std::vector<osg::Vec3d> vecInsideVerts;

protected:
	CLogShower lodShower;

};