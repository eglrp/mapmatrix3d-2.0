// osgFinder.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "COsgFinder.h"
#include "OsgClipNodeFinder.h"
#include "OsgTriangleFinder.h"
#include "OsgPagedLodBlockNameFinder.h"
#include "OsgPagedLodBottomLevelFinder.h"
#include "OsgPagedLodFinder.h"
#include "OsgRepairTransNodeFinder.h"
#include "OsgTransFolderFileNameFinder.h"
#include "OsgTransMatrixFinder.h"
#include "OsgTransNodeFinder.h"
#include "OsgTriangleNumFinder.h"
#include "OsgVertextExtract.h"
#include "OsgGeometryFinder.h"
#include "OsgTextureCoordFinder.h"
#include "OsgPropertyNodeFinder.h"
#include "OsgSelectTriangleByPolygonFinder.h"
#include "qcomm.h"
#include <iostream>

void COsgFinder::FindClipNode(osg::ref_ptr<osg::Node> node, std::vector<osg::ClipNode*> &vecTrans)		  //查找ClipNode
{
	COsgClipNodeFinder ive;
	node->accept(ive);
	vecTrans = ive.vecTrans;

	return;
}

bool COsgFinder::FindGeode(osg::ref_ptr<osg::Node> node)
{
	COsgGeodeFinder ive;
	node->accept(ive);
	return ive.isFindGeometry;
}

std::vector<osg::Geode*>  COsgFinder::FindAndGetGeode(osg::ref_ptr<osg::Node> node)
{
	COsgGeodeFinder_ ive;
	node->accept(ive);
	return ive.vec_ptr;
}

void COsgFinder::FindLodBlockName(osg::ref_ptr<osg::Node> node, std::string& blockName)					  //查找目录名
{
	COsgPagedLodBlockNameFinder ive;
	node->accept(ive);
	blockName = ive.blockName;

	return;
}

void COsgFinder::FindLodBottomLevelFile(osg::ref_ptr<osg::Node> node, std::vector<std::string> &VecBottomLevelPagedLodFileName) 	//查找底层文件名
{
	COsgPagedLodBottomLevelFinder ive(&VecBottomLevelPagedLodFileName);
	node->accept(ive);

	return;
}

void COsgFinder::FindPagedLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod)	  //查找pagedLod
{
	COsgPagedLodFinder ive;
	node->accept(ive);
	vecPagedLod = ive.vecPagedLod;

	return;
}

void COsgFinder::FindRepairTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecMatTrans)		//查找修复节点的trans节点
{
	COsgRepairTransNodeFinder ive;
	node->accept(ive);
	vecMatTrans = ive.vecMatTrans;

	return;
}

void COsgFinder::FindTransFolderName(osg::ref_ptr<osg::Node> node, std::string &folderFileName)			   //查找Trans目录名
{
	COsgTransFolderNameFinder ive;
	node->accept(ive);
	folderFileName = ive.FolderFileName;

	return;
}

void COsgFinder::FindTransMatrix(osg::ref_ptr<osg::Node> node, std::vector<osg::Matrix> &vecMatrix, std::vector<osg::MatrixTransform*> &vecTrans)	 //查找trans节点
{
	COsgTransMatrixFinder ive;
	node->accept(ive);
	vecMatrix = ive.vecMatrix;
	vecTrans = ive.vecTrans;

	return;
}

void COsgFinder::FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans) //查找trans节点
{
	COsgTransNodeFinder ive;
	node->accept(ive);
	vecTrans = ive.vecTrans;

	return;
}

void COsgFinder::FindTriangleNum(osg::ref_ptr<osg::Node> node, int &triCnt) 							   //查找三角形数量
{
	COsgTriangleNumFinder ive;
	node->accept(ive);
	triCnt = ive.numTri;

	return;
}

void COsgFinder::ExtractVerts(osg::ref_ptr<osg::Node> node, osg::ref_ptr<osg::Vec3Array> &extracted_verts)  //查找verts
{
	COsgVertextExtract ive;
	node->accept(ive);
	extracted_verts = ive.extracted_verts;
	return;
}

void COsgFinder::replacePointFound(osg::ref_ptr<osgViewer::Viewer> viewer,osg::ref_ptr<osg::Node> scenceNode, osg::ref_ptr<osg::Node> vertexNode, osg::Matrix matrix, float x, float y, float pixSize, float disErr)
{
	COsgVertextReplace ive(viewer,scenceNode,matrix,x,y,pixSize,disErr);
	vertexNode->accept(ive);
	return;
}

void COsgFinder::FindEmptyLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod)
{

}

void COsgFinder::FindTextureCoord(osg::ref_ptr<osg::Array> nodeArray, osg::ref_ptr<osg::Vec2Array> mTexCoorArray)
{
	/*CTextureCoordFinder ive;
	nodeArray->accept(ive);
	mTexCoorArray = ive.GetTexCoorArray();*/
}

osg::ref_ptr<osg::Node> COsgFinder::FindObjectNodeFromNodePath(osg::NodePath np)
{
	for (int i = 0; i < np.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> trans = dynamic_cast<osg::MatrixTransform*> (np[i]);

		if (!trans)
		{
			continue;
		}

		std::string name = trans->getName();

		if (name != "ObjectMatrix")
		{
			continue;
		}

		return trans.get();
	}

	return NULL;
}

osg::ref_ptr<osg::Node> COsgFinder::FindObjectPagedLodFromNodePath(osg::NodePath np)
{
	for (int i = 0; i < np.size(); i++)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*> (np[i]);

		if (!pagedLod)
		{
			continue;
		}

		std::string name = pagedLod->getName();

		if (name.find("object") != std::string::npos)
		{
			return pagedLod;
		}
	}

	return NULL;
}


osg::ref_ptr<osg::Node> COsgFinder::FindObjectPropertyNodeFromNodePath(osg::NodePath np)
{
	for (int i = 0; i < np.size(); i++)
	{
		osg::ref_ptr<osg::Node> node = np[i];

		if (!node)
		{
			continue;
		}

		std::string name = node->getName();

		if (name.find("Property:") != std::string::npos)   //如果找到
		{
			return node.get();
		}
	}

	return NULL;
}

osg::ref_ptr<osg::Node> COsgFinder::FindObjectPropertyNodeByName(osg::ref_ptr<osg::Node> node, std::string ObjectPropertyName, std::string ObjectPropertyValue)
{
	COsgPropertyNodeFinder ive(ObjectPropertyName, ObjectPropertyValue);
	node->accept(ive);
	
	return ive.getObjectNode();
}


bool COsgFinder::FindTriangleByPolygon(osg::ref_ptr<osg::Node> node, std::vector<CScope> vecScope, std::map<double, osg::Vec3d> &mapVerts, bool isInside)
{
	COsgSelectTriangleByPolygonFinder ive(vecScope, isInside);
	node->accept(ive);
	mapVerts = ive.m_MapInsideVerts;

	return true;
}

bool COsgFinder::FindNodeMinMaxHeight(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, double &minHeight, double &maxHeight)
{
	COsgVertextExtract ive;
	node->accept(ive);
	osg::ref_ptr<osg::Vec3Array> extracted_verts = ive.extracted_verts;
	std::list<double> listHeight;


	for (int j = 0; j < extracted_verts->size(); j++)
	{
		osg::Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
		osg::Vec3d pt1 = pt * matrix;
		listHeight.push_back(pt1.z());
	}

	listHeight.sort();

	minHeight = listHeight.front();
	maxHeight = listHeight.back();
	return true;
}