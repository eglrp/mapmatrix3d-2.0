#pragma once
#include "OSG_Plus.h"
#include "InterfaceOsgFinder.h"

class FINDERAPI COsgFinder : public InterfaceOsgFinder
{
public:
	void FindClipNode(osg::ref_ptr<osg::Node> node, std::vector<osg::ClipNode*> &vecTrans);		  //查找ClipNode

	bool FindGeode(osg::ref_ptr<osg::Node> node);												  //查找是否存在Geode

	std::vector<osg::Geode*>  FindAndGetGeode(osg::ref_ptr<osg::Node> node);					  //查找并输出Geode

	void FindLodBlockName(osg::ref_ptr<osg::Node> node, std::string& blockName);				  //查找目录名

	void FindLodBottomLevelFile(osg::ref_ptr<osg::Node> node, std::vector<std::string> &VecBottomLevelPagedLodFileName); 	//查找底层文件名

	void FindPagedLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod);	  //查找pagedLod

	void FindRepairTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecMatTrans);	//查找修复节点的trans节点

	void FindTransFolderName(osg::ref_ptr<osg::Node> node, std::string &folderFileName);			   //查找Trans目录名

	void FindTransMatrix(osg::ref_ptr<osg::Node> node, std::vector<osg::Matrix> &vecMatrix, std::vector<osg::MatrixTransform*> &vecTrans);	 //查找trans节点

	void FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans); //查找trans节点

	void FindTriangleNum(osg::ref_ptr<osg::Node> node, int &triCnt); 							   //查找三角形数量

	void ExtractVerts(osg::ref_ptr<osg::Node> node, osg::ref_ptr<osg::Vec3Array> &extracted_verts);  //查找verts

	void FindEmptyLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod);		//查找空的LOD

	void FindTextureCoord(osg::ref_ptr<osg::Array> nodeArray, osg::ref_ptr<osg::Vec2Array> mTexCoorArray);				//查找纹理坐标

	osg::ref_ptr<osg::Node> FindObjectNodeFromNodePath(osg::NodePath np);										//查找对象节点

	osg::ref_ptr<osg::Node> FindObjectPagedLodFromNodePath(osg::NodePath np);									//查找对象节点

	osg::ref_ptr<osg::Node> FindObjectPropertyNodeFromNodePath(osg::NodePath np);								//查找属性节点

	osg::ref_ptr<osg::Node> FindObjectPropertyNodeByName(osg::ref_ptr<osg::Node> node, std::string ObjectPropertyName, std::string ObjectPropertyValue);	//查找物体属性节点

	bool FindTriangleByPolygon(osg::ref_ptr<osg::Node> node, std::vector<CScope> vecScope, std::map<double, osg::Vec3d> &mapVerts, bool isInside);

	bool FindNodeMinMaxHeight(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, double &minHeight, double &maxHeight);

	void replacePointFound(osg::ref_ptr<osgViewer::Viewer> viewer,osg::ref_ptr<osg::Node> scenceNode,osg::ref_ptr<osg::Node> vertexNode,osg::Matrix matrix,float x,float y,float pixSize,float disErr);
};
