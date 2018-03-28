#pragma once

#include "publicDefined.h"
#include "InterfaceUserEventCallBack.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgFinder.h"
#include "Geometry.h"
#include "osgClip.h"
#include "MultiScopePagedLodFlatter.h"
#include "InterfaceOsgFlat.h"
#include "SqliteSaver.h"

class COsgFlatter : public COsgClipper, public InterfaceOsgFlat
{
public: 

public:
	//½Ó¿Ú
	virtual bool flatSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
		CStProgressSetter stPrgress);

	virtual bool NormalizeflatSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
		CStProgressSetter stPrgress);

	virtual bool RecoverFlatSceneByArea(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string InFilePath, std::string OutputFilePath, std::vector<CScope> VecScope,
		std::vector<CScope> VecPreFlatScope, CStProgressSetter stProgress);

	virtual bool flatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope);

	virtual bool normalizeflatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope);

	virtual bool recoverFlatOsgHeader(osg::ref_ptr<osg::Node> sceneNode, std::vector<CScope> vecScope, std::vector<CScope> preVecScope, double preSceneMaxHeight, double preFlattenHeight);

	bool ClipSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<CScope> vecScope,
		std::vector<std::string> &vecHalfInPolygonFileName, std::vector<std::string> &vecTotalInPolygonFileName, CStProgressSetter stPrgress, bool isInside)
	{
		return true;
	}

protected:
	virtual void FlatTotalInPolygonTile(osg::ref_ptr<osg::Node> node, osg::Matrix Matrix);

	virtual void NormalizeFlatTotalInPolygonTile(osg::ref_ptr<osg::Node> node, osg::Matrix Matrix);

	virtual void RecoverPreFlatTotalInPolygonTile(osg::ref_ptr<osg::Node> node, std::vector<CScope> vecPreScope, osg::Matrix matrix);

	virtual void ObtainSceneMaxHeight(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d> vecWorldPt, osg::Matrix matrix, double &sceneMaxHeight);

	virtual void obtainFlattenHeight(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d> vecWorldPt, osg::Matrix matrix, double &flattenHeight);

	virtual void InitOsgFlatter(const std::string inFilePath, const std::string outputFilePath, const std::vector<CScope> vecScope, CStProgressSetter stPrgress);

private:
	std::string inFilePath;
	std::string outputFilePath;
	std::vector<CScope> vecScope;
	CStProgressSetter stPrgress;
	CSqliteSaver sqliteSaveer;
};