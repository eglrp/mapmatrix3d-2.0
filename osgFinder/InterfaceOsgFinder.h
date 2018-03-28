#pragma once
#include "OSG_Plus.h"
#include "publicDefined.h"

#ifdef FINDERLIBDLL  
#define FINDERAPI _declspec(dllexport)  
#else  
#define FINDERAPI  _declspec(dllimport)  
#endif  


class FINDERAPI InterfaceOsgFinder
{
public:
	virtual void FindClipNode(osg::ref_ptr<osg::Node> node, std::vector<osg::ClipNode*> &vecTrans) = 0;		   //查找ClipNode

	virtual bool FindGeode(osg::ref_ptr<osg::Node> node) = 0;  												   //查找是否存在Geode

	virtual std::vector<osg::Geode*>  FindAndGetGeode(osg::ref_ptr<osg::Node> node) = 0;			     	   //查找并输出Geode

	virtual void FindLodBlockName(osg::ref_ptr<osg::Node> node, std::string& blockName) = 0;				   //查找目录名

	virtual void FindLodBottomLevelFile(osg::ref_ptr<osg::Node> node, std::vector<std::string> &VecBottomLevelPagedLodFileName) = 0; 	//查找底层文件名

	virtual void FindPagedLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod) = 0;	   //查找pagedLod

	virtual void FindRepairTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecMatTrans) = 0;	//查找修复节点的trans节点

	virtual void FindTransFolderName(osg::ref_ptr<osg::Node> node, std::string &folderFileName) = 0;		   //查找Trans目录名

	virtual void FindTransMatrix(osg::ref_ptr<osg::Node> node, std::vector<osg::Matrix> &vecMatrix, std::vector<osg::MatrixTransform*> &vecTrans) = 0;	 //查找trans节点

	virtual void FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans) = 0; //查找trans节点

	virtual void FindTriangleNum(osg::ref_ptr<osg::Node> node, int &triCnt) = 0; 							    //查找三角形数量

	virtual void ExtractVerts(osg::ref_ptr<osg::Node> node, osg::ref_ptr<osg::Vec3Array> &extracted_verts) = 0;  //查找verts

	virtual void FindEmptyLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod) = 0;		//查找空的LOD

	virtual osg::ref_ptr<osg::Node> FindObjectNodeFromNodePath(osg::NodePath np) = 0;										//查找对象节点

	virtual osg::ref_ptr<osg::Node> FindObjectPagedLodFromNodePath(osg::NodePath np) = 0;									//查找对象节点

	virtual osg::ref_ptr<osg::Node> FindObjectPropertyNodeFromNodePath(osg::NodePath np) = 0;								//查找属性节点

	virtual osg::ref_ptr<osg::Node> FindObjectPropertyNodeByName(osg::ref_ptr<osg::Node> node, std::string ObjectPropertyName, std::string ObjectPropertyValue) = 0;	//查找物体属性节点

	virtual bool FindTriangleByPolygon(osg::ref_ptr<osg::Node> node, std::vector<CScope> vecScope, std::map<double, osg::Vec3d> &mapVerts, bool isInside) = 0;

	virtual bool FindNodeMinMaxHeight(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, double &minHeight, double &maxHeight) = 0;

	virtual void replacePointFound(osg::ref_ptr<osgViewer::Viewer> viewer, osg::ref_ptr<osg::Node> scenceNode, osg::ref_ptr<osg::Node> vertexNode, osg::Matrix matrix, float x, float y, float pixSize, float disErr) = 0;
};



class FINDERAPI InterfaceOsgFinderFactory
{
public:		
	InterfaceOsgFinder* create();

};
