#pragma once
#include "publicDefined.h"
#include "cv_plus.hpp"
#include <string>
#include "OSG_Plus.h"
#include "InterfaceUserEventCallBack.h"


class CNodeDemCreator :  public osg::NodeVisitor 
{
public:
	CNodeDemCreator::CNodeDemCreator(osg::Matrix MATRIX, CvMat* DEMMAT, GridDem GDem, bool IsRepair) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		matrix = MATRIX;
		demMat = DEMMAT;
		gDem = GDem;
		isRepair = IsRepair;
	}
	void apply(osg::Geode& geode)
	{
		InterpolateDem(geode, matrix, demMat, gDem, isRepair);
	}

private:
	void InterpolateDem(osg::Geode& geode, osg::Matrix matrix, CvMat* demMat, GridDem gDem, bool isRepair);
	
private:
	osg::Matrix matrix;
	CvMat* demMat;
	GridDem gDem;
	bool isRepair;
};

class CSceneDemGenerator
{
public:
	std::vector<std::string> vecFileName;
	GridDem gDem;

public:
	bool StartGeneratingDem(std::vector<osg::MatrixTransform*> vecTransNode, std::string rootModelPath, std::string outfileName, double xRes, double yRes, InterfaceSigEventCallBack* ISigCallBack);

private:
	CvMat* GenerateDem(std::string rootModelPath, osg::ref_ptr<osg::MatrixTransform> transNode, double Xinterval, double Yinterval, InterfaceSigEventCallBack* ISigCallBack);

	void obtainOsgFileName(std::string outputPath);

	void ObtainImageGeoRect(std::string twfFileName, osg::Vec4d &rect, std::string imageFileName);

	bool ObtainGeoRect(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<osg::Vec3d> &minMaxGeode);

	std::vector<osg::Vec3d> findMinMaxNode(std::vector<osg::Vec3d> minMaxNode, std::vector<osg::Vec3d> minMaxNode1);
	
	std::vector<std::string> SearchChildFilePath(std::string searchFilePath);

	std::vector<std::vector<std::string>> SearchChildFileName(std::vector<std::vector<std::string>> vecGroupFilePath);

	std::vector<std::vector<std::string>> SeparateScenenTile(std::vector<std::string> vecChildFilePath, double Xinterval, double Yinterval);

	void SelectHighLevelLodFileName(std::vector<std::vector<std::string>> vecGroupChildName, std::vector<std::vector<std::string>> &vecGroupHighLevelChildName);

	bool GenerateDemForSingleModel(std::string rootModelPath, osg::ref_ptr<osg::MatrixTransform> transNode, std::string outDemFileName, double xRes, double yRes, InterfaceSigEventCallBack* ISigCallBack);

	void CombineImage(osg::Vec4d maxRect, std::string outfileName, std::vector<osg::Vec4d> vecRect, std::vector<std::string> vecDemTiffFileName, 
		std::vector<std::string> vecDemTwfFileName, double xRes, double yRes);

	osg::Vec4d findMaxRect(std::vector<osg::Vec4d> vecRect);

};

