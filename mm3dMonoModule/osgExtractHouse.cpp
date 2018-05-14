// osgExtractHouse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgExtractHouse.h"
#include "InterfaceOsgExtractWall.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceCOsgExtractArea.h"
#include "InterfaceOsgAlgorithm.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceOsgEditor.h"
#include "Geometry.h"


void COsgExtractHouse::StartExtractHouse(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, double userDefineGroundHeight)
{
	clipObjectByHeight(node, worldMatrix, userDefineGroundHeight);
}

void COsgExtractHouse::extractHouse(osg::ref_ptr<osg::Node> sceneNode, std::vector<osg::Vec3d> vecWorldPt)
{
	std::vector<osg::MatrixTransform*> vecTrans;
	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	iOsgFinder->FindTransNode(sceneNode, vecTrans);

	if (vecTrans.size() == 0)
	{
		return;
	}

	osg::Matrix matrix = vecTrans[0]->getMatrix();

	//提取对象
	std::vector<CScope> vecClipScope;
	CScope scope;
	scope.vecCoord = vecWorldPt;
	vecClipScope.push_back(scope);
	InterfaceCOsgExtractAreaFactory iOsgExtractAreaFactory;
	InterfaceCOsgExtractArea* iOsgExtractArea = iOsgExtractAreaFactory.create();
	osg::ref_ptr<osg::Node> areaNode = iOsgExtractArea->extractObjectByArea(sceneNode, vecClipScope);
	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	osgDB::writeNodeFile(*areaNode, "D:\\111.OSGB", spOptions);
	double minHeihgt = 0; double maxHeight = 0;
	InterfaceOsgAlgorithmFactory iOsgAlgorithmFactory;
	InterfaceOsgAlgorithm* iOsgAlgorithm = iOsgAlgorithmFactory.create();
	bool isSuccess = iOsgAlgorithm->FindMinMaxHeight(vecWorldPt, minHeihgt, maxHeight);

	if (!isSuccess)
	{
		return;
	}

	double groundHeight = minHeihgt;

	
	
	StartExtractHouse(areaNode, matrix, groundHeight + 0.2);
	
	osg::ref_ptr<osg::MatrixTransform> transAreaNode = dynamic_cast<osg::MatrixTransform*> (areaNode->asTransform());

	if (!transAreaNode)
	{
		return;
	}

	objectAdder.addObjectToScene(sceneNode, transAreaNode);
	
}


void COsgExtractHouse::searchGreen(CTriangle& triangle, std::vector<osg::Vec3d>& vecPoints, std::vector<CTriangleRelation>& vecTrangleRelation)
{
	InterfaceOsgExtractWallFactory IWallFactory;
	InterfaceOsgExtractWall* IWall = IWallFactory.create();
	bool finish = false;
	if ((triangle.color.g() > triangle.color.r() && triangle.color.g() > triangle.color.b()))
	{
		if (!IWall->findPointInVector(triangle.pt1, vecPoints))
		{
			vecPoints.push_back(triangle.pt1);
			finish = true;
		}
		if (!IWall->findPointInVector(triangle.pt2, vecPoints))
		{
			vecPoints.push_back(triangle.pt2);
			finish = true;
		}
		if (!IWall->findPointInVector(triangle.pt3, vecPoints))
		{
			vecPoints.push_back(triangle.pt3);
			finish = true;
		}
	}
	if (!finish)
	{
		return;
	}

	//搜索相邻面片
	std::vector<CTriangle> vecTri;
	if (IWall->FindRelationInVector(triangle, vecTri, vecTrangleRelation))
	{
		for (std::vector<CTriangle>::iterator ite = vecTri.begin(); ite != vecTri.end(); ++ite)
		{
			searchGreen(*ite, vecPoints, vecTrangleRelation);
		}
	}
}
//寻找满足要求的面片
void COsgExtractHouse::searchPoints(CTriangle& triangle, std::vector<osg::Vec3d>& vecPoints, std::vector<CTriangleRelation>& vecTrangleRelation, double userDefineGroundHeight)
{
	bool finish = false;
	InterfaceOsgExtractWallFactory IWallFactory;
	InterfaceOsgExtractWall* IWall = IWallFactory.create();

	osg::Vec3d vertical(0, 0, 1);

	if ((triangle.pt1.z() > userDefineGroundHeight + 1) && (triangle.pt2.z() > userDefineGroundHeight + 1) && (triangle.pt3.z() > userDefineGroundHeight + 1))
	{
		return;
	}

	/*if (isInScope.checkNormalForMono(vertical, triangle.normal, 30))
	{
		return;
	}*/
	
	//if (!(triangle.color.g() > triangle.color.r() && triangle.color.g() > triangle.color.b()))//过滤绿色
	{
		//保存当前面片
		if (!IWall->findPointInVector(triangle.pt1, vecPoints))
		{	
			vecPoints.push_back(triangle.pt1);
			finish = true;
		}
		if (!IWall->findPointInVector(triangle.pt2, vecPoints))
		{
			vecPoints.push_back(triangle.pt2);
			finish = true;
		}
		if (!IWall->findPointInVector(triangle.pt3, vecPoints))
		{
			vecPoints.push_back(triangle.pt3);
			finish = true;
		}
	}

	if (!finish)
	{
		return;
	}

	//搜索相邻面片
	std::vector<CTriangle> vecTri;
	if (IWall->FindRelationInVector(triangle, vecTri, vecTrangleRelation))
	{
		for (std::vector<CTriangle>::iterator ite = vecTri.begin(); ite != vecTri.end(); ++ite)
		{
			searchPoints(*ite, vecPoints, vecTrangleRelation, userDefineGroundHeight);
		}
	}
}

//切除不要的面片
void  COsgExtractHouse::clipObject(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, std::vector<osg::Vec3d>& vecPoints)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
	std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(node);

	for (size_t i = 0; i < vecGeode.size(); ++i)
	{
		if (!vecGeode[i])
		{
			continue;
		}

		int num = vecGeode[i]->getNumDrawables();

		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(vecGeode[i]->getDrawable(j));

			if (!geom)
				continue; //得到可绘制结点的顶点序列 

			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			for (size_t k = 0; k < geom->getNumPrimitiveSets(); k++)
			{
				osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
				osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
				osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
				int cnt = drawElementTriangle->getNumIndices();
				int triCnt = cnt / 3;

				for (int w = 0; w < triCnt; w++)
				{
					int index1 = drawElementTriangle->index(w * 3);
					int index2 = drawElementTriangle->index(w * 3 + 1);
					int index3 = drawElementTriangle->index(w * 3 + 2);

					double x1 = verts->at(index1).x();
					double y1 = verts->at(index1).y();
					double z1 = verts->at(index1).z();
					osg::Vec3d pt1(x1, y1, z1);
					pt1 = pt1 * worldMatrix;

					double x2 = verts->at(index2).x();
					double y2 = verts->at(index2).y();
					double z2 = verts->at(index2).z();
					osg::Vec3d pt2(x2, y2, z2);
					pt2 = pt2 * worldMatrix;

					double x3 = verts->at(index3).x();
					double y3 = verts->at(index3).y();
					double z3 = verts->at(index3).z();
					osg::Vec3d pt3(x3, y3, z3);
					pt3 = pt3 * worldMatrix;

					if (isInScope.PtInVector(pt1, pt2, pt3, vecPoints))//找到鼠标点击的面片
					{
						
					}
					else
					{
						newDrawElementsTriangle->push_back(index1);
						newDrawElementsTriangle->push_back(index2);
						newDrawElementsTriangle->push_back(index3);
					}
				}

				geom->setPrimitiveSet(k, newDrawElementsTriangle);
			}
		}
	}
}

//切除高度以下不要的面片
void  COsgExtractHouse::clipObjectByHeight(osg::ref_ptr<osg::Node> node, osg::Matrix worldMatrix, double height)
{
	InterfaceOsgFinderFactory IOsgFinderFactory;
	InterfaceOsgFinder* IOsgFinder = IOsgFinderFactory.create();
	std::vector<osg::Geode*> vecGeode = IOsgFinder->FindAndGetGeode(node);

	for (size_t i = 0; i < vecGeode.size(); ++i)
	{
		if (!vecGeode[i])
		{
			continue;
		}

		int num = vecGeode[i]->getNumDrawables();

		for (int j = num - 1; j >= 0; --j)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(vecGeode[i]->getDrawable(j));

			if (!geom)
				continue; //得到可绘制结点的顶点序列 

			osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			for (size_t k = 0; k < geom->getNumPrimitiveSets(); k++)
			{
				osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
				osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
				osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
				int cnt = drawElementTriangle->getNumIndices();
				int triCnt = cnt / 3;

				for (int w = 0; w < triCnt; w++)
				{
					int index1 = drawElementTriangle->index(w * 3);
					int index2 = drawElementTriangle->index(w * 3 + 1);
					int index3 = drawElementTriangle->index(w * 3 + 2);

					double x1 = verts->at(index1).x();
					double y1 = verts->at(index1).y();
					double z1 = verts->at(index1).z();
					osg::Vec3d pt1(x1, y1, z1);
					pt1 = pt1 * worldMatrix;

					double x2 = verts->at(index2).x();
					double y2 = verts->at(index2).y();
					double z2 = verts->at(index2).z();
					osg::Vec3d pt2(x2, y2, z2);
					pt2 = pt2 * worldMatrix;

					double x3 = verts->at(index3).x();
					double y3 = verts->at(index3).y();
					double z3 = verts->at(index3).z();
					osg::Vec3d pt3(x3, y3, z3);
					pt3 = pt3 * worldMatrix;

					if (pt1.z() > height && pt2.z() > height && pt3.z() > height)
					{
						newDrawElementsTriangle->push_back(index1);
						newDrawElementsTriangle->push_back(index2);
						newDrawElementsTriangle->push_back(index3);
					}
					else
					{
						osg::Vec3d vertical(0, 0, 1);
						osg::Vec3d normal = normalVector(pt1, pt2, pt3);

						if (isInScope.checkNormalForMono(vertical, normal, 80))
						{

						}
						else
						{
							newDrawElementsTriangle->push_back(index1);
							newDrawElementsTriangle->push_back(index2);
							newDrawElementsTriangle->push_back(index3);
						}
					}
				}

				geom->setPrimitiveSet(k, newDrawElementsTriangle);
			}
		}
	}
}


//计算三角面片的法向量
osg::Vec3d COsgExtractHouse::normalVector(osg::Vec3d a, osg::Vec3d b, osg::Vec3d c)
{
	osg::Vec3d ab = b - a;
	osg::Vec3d bc = c - b;

	osg::Vec3d nomal;
	nomal.x() = (-ab.z()*bc.y() + ab.y()*bc.z()) / (ab.x()*bc.y() - bc.x()*ab.y());
	nomal.y() = (-bc.x()*ab.z() + ab.x()*bc.z()) / (bc.x()*ab.y() - ab.x()*bc.y());
	nomal.z() = 1;

	return nomal;
}