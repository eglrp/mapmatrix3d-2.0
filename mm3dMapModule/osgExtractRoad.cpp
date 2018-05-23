// osgExtractRoad.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ThreadSafeUpdateCallback.h"
#include "osgExtractRoad.h"
#include "InterfaceOsgExtractWall.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceBaseDrawer.h"
#include "SceneExtractWall.h"
#include "alphaShapes.h"
#include "Geometry.h"
#include <process.h>
#include <mutex>

std::mutex g_lock;

void COsgExtractRoad::toExtractRoad(osg::Group* mRoot_, osg::Group* drawGroup_, osg::NodePath vecNodePath_, osg::Vec3d vecSearchPoint_, std::vector<osg::Vec3d>* vecPtResult_, std::vector<std::string>* vecFileNames_, std::vector<CTriangleRelation>* vecTriangleRelation_, std::vector<osg::Vec3d>* vecNormal_,std::vector<osg::Vec4>* vecColor_, osg::Matrix worldMatrix_, InterfaceSigEventCallBack* ISigEventCallBack_)
{
	//输入
	mRoot = mRoot_;
	drawGroup = drawGroup_;
	vecNodePath = vecNodePath_;
	vecSearchPoint = vecSearchPoint_;

	worldMatrix = worldMatrix_;
	ISigEventCallBack = ISigEventCallBack_;

	//输出
	vecPtResult = vecPtResult_;
	vecTriangleRelation = vecTriangleRelation_;
	vecFileNames = vecFileNames_;
	vecNormal = vecNormal_;
	vecColor = vecColor_;
	
	StartExtractRoad();
}

void COsgExtractRoad::toExtractRoadEdge(osg::Group* mRoot_, osg::Group* drawGroup_,std::vector<osg::Vec3d>* vecPtResult_, std::vector<osg::Vec3d>* vecEdge_, double alpha_, osg::Matrix worldMatrix_, InterfaceSigEventCallBack* ISigEventCallBack_)
{
	mRoot = mRoot_;
	drawGroup = drawGroup_;
	vecPtResult = vecPtResult_;
	vecPtEdge = vecEdge_;
	alpha = alpha_;
	worldMatrix = worldMatrix_;
	ISigEventCallBack = ISigEventCallBack_;

	StartExtractRoadEdge();
}

void COsgExtractRoad::StartExtractRoad()
{
	_beginthread(&extractRoad, 0, this);
}

void COsgExtractRoad::StartExtractRoadEdge()
{
	_beginthread(&extractRoadEdge, 0, this);
}

void COsgExtractRoad::extractRoadEdge(void* ptr)
{
	COsgExtractRoad* roader = (COsgExtractRoad*)ptr;
	osg::Group* mRoot = roader->mRoot;
	osg::Group* drawGroup = roader->drawGroup;
	std::vector<osg::Vec3d>* vecPtResult = roader->vecPtResult;
	std::vector<osg::Vec3d>* vecPtEdge = roader->vecPtEdge;
	osg::Matrix worldMatrix = roader->worldMatrix;
	double alpha = roader->alpha;
	InterfaceSigEventCallBack* ISigEventCallBack = roader->ISigEventCallBack;

	std::vector<osg::Vec3d> vecEdgeTemp;
	roader->ExtractRoadEdge(vecPtResult, &vecEdgeTemp, alpha);
	if (vecEdgeTemp.empty())
	{
		return;
	}

	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
	for (size_t i = 0; i < vecEdgeTemp.size()-1; i+=2)
	{
		IDrawer->BaseDrawLineOperate(vecEdgeTemp[i], vecEdgeTemp[i+1], osg::Vec4(1, 0, 0, 1), 3.5, outlineGroup, worldMatrix);
	}
	osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
	g_lock.lock();
	if (spChildCallback != NULL)
	{
		spChildCallback->AddChild(drawGroup, outlineGroup);
		(*vecPtEdge).insert(vecPtEdge->end(), vecEdgeTemp.begin(), vecEdgeTemp.end());
		ISigEventCallBack->EmitSigShowOpsHint("道路提取：继续提取点云请画线，按Enter键计算道路边缘");
		(*vecPtResult).clear();
	}
	g_lock.unlock();
}

void COsgExtractRoad::extractRoad(void* ptr)
{
	COsgExtractRoad* roader = (COsgExtractRoad*)ptr;
	osg::Group* mRoot = roader->mRoot;
	osg::Group* drawGroup = roader->drawGroup;
	osg::NodePath vecNodePath = roader->vecNodePath;
	osg::Vec3d vecSearchPoint = roader->vecSearchPoint;
	osg::Matrix worldMatrix = roader->worldMatrix;	
	InterfaceSigEventCallBack* ISigEventCallBack = roader->ISigEventCallBack;

	std::vector<osg::Vec3d>* vecPtResult = roader->vecPtResult;
	std::vector<std::string>* vecFileNames = roader->vecFileNames;
	std::vector<CTriangleRelation>* vecTriangleRelation = roader->vecTriangleRelation;
	std::vector<osg::Vec3d>* vecNormal = roader->vecNormal;
	std::vector<osg::Vec4>* vecColor = roader->vecColor;
	
	std::vector<osg::Vec3d> vecPt;
	roader->ExtractRoad(mRoot,vecNodePath,vecSearchPoint,&vecPt,vecFileNames,vecTriangleRelation,vecNormal,vecColor,worldMatrix);
	if (vecPt.empty())
	{
		return;
	}
	g_lock.lock();
	(*vecPtResult).insert(vecPtResult->end(), vecPt.begin(), vecPt.end());
	g_lock.unlock();
	/*InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.Create();
	osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
	osg::ref_ptr<osg::LOD> lod = new osg::LOD();*/
	//double r = mRoot->getChild(0)->getBound().radius();
	//double h = r / 10 / 0.267949;
	/*for (size_t i = 0; i < vecPt.size(); ++i)
	{
		IDrawer->BaseDrawPtOperate(vecPt[i], osg::Vec4(1, 0, 0, 1), 2.5, outlineGroup, worldMatrix);
	}*/
	//lod->addChild(outlineGroup, 0, h);//原始数据
	
	//5层lod
	/*for (int i = 0;i<5;++i)
	{
		osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
		roader->shampPointCloud(vecPt, 0.2*(i+1));
		for (size_t i = 0; i < vecPt.size(); ++i)
		{
			IDrawer->BaseDrawPtOperate(vecPt[i], osg::Vec4(0, 0, 1, 1), 3.5, outlineGroup, worldMatrix);
		}
		lod->addChild(outlineGroup, 20+i*20, 40 + i * 20 );
	}*/
	/*osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
	g_lock.lock();
		if (spChildCallback != NULL)
		{
			spChildCallback->AddChild(drawGroup, lod);	
			ISigEventCallBack->EmitSigShowOpsHint("道路提取：继续提取点云请画线，按Enter键计算道路边缘");
		}
	g_lock.unlock();*/
}

void COsgExtractRoad::ExtractRoad(osg::Group* mRoot, osg::NodePath vecNodePath, osg::Vec3d vecSearchPoint, std::vector<osg::Vec3d>* vecPtResult, std::vector<std::string>* vecFileNames, std::vector<CTriangleRelation>* vecTriangleRelation, std::vector<osg::Vec3d>* vecNormal, std::vector<osg::Vec4>* vecColor, osg::Matrix worldMatrix)
{
	//根据nodepath获得所有涉及到的底层文件名
	std::vector<std::string> vecNames;
	getTileNames(vecNodePath, vecNames);

	std::vector<osg::Vec3d> vecPtFound;
	//按文件来提取三角面片
	std::vector<std::vector<CTriangle> > vecVecTriangle;
	getOriTriangle(vecNames, vecVecTriangle,worldMatrix);

	InterfaceOsgExtractWallFactory IWallFactory;
	InterfaceOsgExtractWall* IWall = IWallFactory.create();
	
	bool finded = false;
	//std::vector<osg::Vec3d> vecPointstop;
	for (size_t i = 0; i < vecVecTriangle.size(); ++i)
	{
		for (size_t j = 0; j < vecVecTriangle[i].size(); ++j)
		{
			if (PtInTriangle(vecSearchPoint, vecVecTriangle[i][j].pt1, vecVecTriangle[i][j].pt2, vecVecTriangle[i][j].pt3))
			{
				g_lock.lock();
					(*vecNormal).push_back(vecVecTriangle[i][j].normal);
					(*vecColor).push_back(vecVecTriangle[i][j].color);
				g_lock.unlock();
				
				//找到点所在数据文件名vecNames[i]
				std::vector<std::string>::iterator ite = find((*vecFileNames).begin(), (*vecFileNames).end(), vecNames[i]);
				if ((*vecFileNames).empty() || ite == (*vecFileNames).end())//该数据没有建立过关系，则先建立关系
				{
					std::vector<CTriangleRelation> vecRelationTemp;
					buildRelation(vecVecTriangle[i],vecRelationTemp);
					g_lock.lock();
						if (vecFileNames->empty() || find(vecFileNames->begin(),vecFileNames->end(),vecNames[i]) == vecFileNames->end())
						{
							(*vecFileNames).push_back(vecNames[i]);
							(*vecTriangleRelation).insert((*vecTriangleRelation).end(), vecRelationTemp.begin(), vecRelationTemp.end());
						}
					
					searchPoints(vecVecTriangle[i][j], vecVecTriangle[i][j], vecPtFound,(*vecPtResult), vecRelationTemp, *vecNormal, *vecColor);
					g_lock.unlock();
				}
				else
				{
					g_lock.lock();
					searchPoints(vecVecTriangle[i][j], vecVecTriangle[i][j], vecPtFound,(*vecPtResult), (*vecTriangleRelation), *vecNormal, *vecColor);
					g_lock.unlock();	
				}
					
				finded = true;
				break;
			}
		}
		if (finded)
		{
			break;
		}
	}
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;

	osg::ref_ptr<osg::LOD> lod = new osg::LOD(); 
	double r = mRoot->getChild(0)->getBound().radius();
	double h = r / 10 / 0.267949;
	for (size_t i = 0; i < (*vecPtResult).size(); ++i)
	{
		IDrawer->BaseDrawPtOperate((*vecPtResult)[i], osg::Vec4(0, 0, 1, 1), 3.5, outlineGroup, worldMatrix);
	}
	lod->addChild(outlineGroup, 0, h);//原始数据	
	
	osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
	g_lock.lock();
	if (spChildCallback != NULL)
	{
		spChildCallback->AddChild(drawGroup, lod);
		//ISigEventCallBack->EmitSigShowOpsHint("道路提取：继续提取点云请画线，按Enter键计算道路边缘");
	}
	g_lock.unlock();
}

void COsgExtractRoad::ExtractRoadEdge( std::vector<osg::Vec3d>*vecPtResult, std::vector<osg::Vec3d>* vecEdge, double alpha)
{
	CAlphaShapes alphaShapes;
	alphaShapes.extractPtCloudEdge(*vecPtResult, *vecEdge, alpha);
}

void COsgExtractRoad::buildRelation(std::vector<CTriangle>& vecTriangle, std::vector<CTriangleRelation>& vecRelationTemp)
{
	for (size_t i = 0; i < vecTriangle.size(); ++i)
	{
		CTriangleRelation tr;
		tr.self = vecTriangle[i];
		vecRelationTemp.push_back(tr);
	}
	for (size_t i = 0; i < vecRelationTemp.size(); ++i)
	{
		for (size_t j = i + 1; j < vecRelationTemp.size(); ++j)
		{
			if (vecRelationTemp[i].self.neighborWith(vecRelationTemp[j].self))
			{
				vecRelationTemp[i].neighbor.push_back(vecRelationTemp[j].self);
				vecRelationTemp[j].neighbor.push_back(vecRelationTemp[i].self);
			}
		}
	}
}

bool COsgExtractRoad::checkNomal(osg::Vec3d nom, osg::Vec3d normal, double err)
{
	double mo = sqrt(nom.x()*nom.x() + nom.y()*nom.y() + nom.z()*nom.z());
	double m_normal = sqrt(normal.x()*normal.x() + normal.y()*normal.y() + normal.z()*normal.z());
	double ji = nom.x() * normal.x() + nom.y() * normal.y() + nom.z() * normal.z();
	double angle = acos(ji / (mo * m_normal));
	angle = angle * 180 / osg::PI;

	if (angle < err || angle >180 - err)
	{
		return true;
	}
	return false;
}

void COsgExtractRoad::insertNodePath(osg::Group* mRoot,osg::Vec3d pt1,osg::Vec3d pt2, std::vector<osg::NodePath>& vecNodePath, std::vector<osg::Vec3d>& vecSearchPoint)
{
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	int numPtInsert = 8;
	osg::Vec3d p1;
	std::vector<osg::Vec3d> vecCoord;
	vecCoord.assign(vecSearchPoint.begin(), vecSearchPoint.end());
	
	double deltax = (pt2.x() - pt1.x()) / (numPtInsert +1);
	double deltay = (pt2.y() - pt1.y()) / (numPtInsert + 1);
	double deltaz = (pt2.z() - pt1.z()) / (numPtInsert + 1);
	for (int j = 0; j < numPtInsert; ++j)
	{
		p1.x() = pt1.x() + (j + 1)*deltax;
		p1.y() = pt1.y() + (j + 1)*deltay;
		p1.z() = pt1.z() + (j + 1)*deltaz + 1;
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(p1, osg::Vec3d(p1.x(), p1.y(), -1000));
		osgUtil::IntersectionVisitor iv(ls);
		mRoot->accept(iv);
		if (ls.valid() && ls->containsIntersections())
		{
			vecNodePath.push_back(ls->getFirstIntersection().nodePath);
			vecSearchPoint.push_back(ls->getFirstIntersection().getWorldIntersectPoint());
			//IDrawer->BaseDrawPtOperate(vecSearchPoint.back(), osg::Vec4(0, 1, 0, 1), 4.5, drawGroup, worldMatrix);
		}
	}	
}

void COsgExtractRoad::getTileNames(osg::NodePath vecNodePath, std::vector<std::string>& vecChildFileName)
{
	vecChildFileName.clear();
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	for (int i = vecNodePath.size() - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> ((vecNodePath)[i]);
		if (lod)
		{
			IFinder->FindLodBottomLevelFile(lod, vecChildFileName);
			break;
		}
	}
}

void COsgExtractRoad::getPointsInPolygon(std::string& childFileName, std::vector<CTriangle>& vecTriangle, osg::Matrix worldMatrix)
{
	if (!childFileName.empty())
	{
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(childFileName);

		if (!node)
		{
			return;
		}
		InterfaceOsgFinderFactory iOsgFinderFactory;
		InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
		std::vector<osg::Geode*> vecGeode = iOsgFinder->FindAndGetGeode(node);
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
				if (verts->empty())
				{
					continue;
				}
				//保存颜色
				osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D*> (geom->getOrCreateStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
				if (!texture)
				{
					continue;
				}
				osg::ref_ptr<osg::Vec2Array> textureArray = dynamic_cast<osg::Vec2Array*> (geom->getTexCoordArray(0));
				osg::ref_ptr<osg::Image> image = texture->getImage(0);
				if (!image)
				{
					continue;
				}
				int imageWidth = image->s();
				int imageHeight = image->t();
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

						//创建三角面片数据
						CTriangle tri;
						//面片顶点坐标
						tri.pt1 = pt1;
						tri.pt2 = pt2;
						tri.pt3 = pt3;
						//面片法向量
						NormalVector(pt1, pt2, pt3, tri.normal);
						//面片颜色
						osg::Vec2d texture1, texture2, texture3;//三个顶点的纹理坐标
						texture1.x() = textureArray->at(index1).x() * imageWidth;
						texture1.y() = textureArray->at(index1).y() * imageHeight;
						texture2.x() = textureArray->at(index2).x() * imageWidth;
						texture2.y() = textureArray->at(index2).y() * imageHeight;
						texture3.x() = textureArray->at(index3).x() * imageWidth;
						texture3.y() = textureArray->at(index3).y() * imageHeight;
						std::vector<osg::Vec2d> vecCoord;
						CSceneExtractWall sceneExtractWall;
						sceneExtractWall.getTextureCoordBy3Points(texture1, texture2, texture3, vecCoord);
						unsigned char* data = (unsigned char*)(image->getDataPointer());
						int R = 0, G = 0, B = 0;
						for (size_t i = 0; i < vecCoord.size(); ++i)
						{
							R += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 0];
							G += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 1];
							B += (int)data[((int)vecCoord[i].y()) * imageWidth * 3 + ((int)vecCoord[i].x()) * 3 + 2];
						}
						tri.color.r() = R / 1.0 / vecCoord.size() / 255;
						tri.color.g() = G / 1.0 / vecCoord.size() / 255;
						tri.color.b() = B / 1.0 / vecCoord.size() / 255;
						tri.color.a() = 1.0;

						vecTriangle.push_back(tri);
					}
				}
			}
		}
	}
}

void COsgExtractRoad::NormalVector(osg::Vec3d& a, osg::Vec3d& b, osg::Vec3d& c, osg::Vec3d& nomal)
{
	osg::Vec3d ab = b - a;
	osg::Vec3d bc = c - b;

	nomal.x() = (-ab.z()*bc.y() + ab.y()*bc.z()) / (ab.x()*bc.y() - bc.x()*ab.y());
	nomal.y() = (-bc.x()*ab.z() + ab.x()*bc.z()) / (bc.x()*ab.y() - ab.x()*bc.y());
	nomal.z() = 1;
}

void COsgExtractRoad::getOriTriangle(std::vector<std::string> & vecNames, std::vector<std::vector<CTriangle> >& vecVecTriangle,osg::Matrix worldMatrix)
{
	//删除重复
	sort(vecNames.begin(), vecNames.end());
	std::vector<string>::iterator pos;
	pos = unique(vecNames.begin(), vecNames.end());
	vecNames.erase(pos, vecNames.end());

	std::vector<CTriangle> vecTempTrinangle;

	for (size_t j = 0; j < vecNames.size(); ++j)
	{
		vecTempTrinangle.clear();
		getPointsInPolygon(vecNames[j], vecTempTrinangle, worldMatrix);
		vecVecTriangle.push_back(vecTempTrinangle);
	}
}

bool COsgExtractRoad::PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3)
{
	std::vector<CL::Vec2d> POL;

	POL.push_back(CL::Vec2d(t1.x(), t1.y()));
	POL.push_back(CL::Vec2d(t2.x(), t2.y()));
	POL.push_back(CL::Vec2d(t3.x(), t3.y()));
	POL.push_back(CL::Vec2d(t1.x(), t1.y()));

	double maxz = t1.z() > t2.z() ? t1.z() : t2.z();
	maxz = maxz > t3.z() ? maxz : t3.z();
	double minz = t1.z() > t2.z() ? t2.z() : t1.z();
	minz = minz > t3.z() ? t3.z() : minz;

	if (Point_In_Polygon_2D(pt.x(), pt.y(), POL) && pt.z() >= minz && pt.z() <= maxz)
	{
		return true;
	}
	return false;
}

void COsgExtractRoad::searchPoints(CTriangle& triangle, CTriangle& triangleFather, std::vector<osg::Vec3d>& vecPoints, std::vector<osg::Vec3d>& vecPointstop, std::vector<CTriangleRelation>& vecTrangleRelation, std::vector<osg::Vec3d>& vecNormal, std::vector<osg::Vec4>& vecColor)
{
	bool finish = false;
	InterfaceOsgExtractWallFactory IWallFactory;
	InterfaceOsgExtractWall* IWall = IWallFactory.create();
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	CSceneExtractWall sceneExtractWall;
	if (!sceneExtractWall.checkNomalEx(triangle.normal, vecNormal, 15))
	{
		//保存当前面片
		if (!IWall->findPointInVector(triangleFather.pt1, vecPointstop))
		{
			vecPointstop.push_back(triangleFather.pt1);
		}
		if (!IWall->findPointInVector(triangleFather.pt2, vecPointstop))
		{
			vecPointstop.push_back(triangleFather.pt2);
		}
		if (!IWall->findPointInVector(triangleFather.pt3, vecPointstop))
		{
			vecPointstop.push_back(triangleFather.pt3);
		}
		
		return;
	}
	if (sceneExtractWall.testColorEx(triangle.color, vecColor, 0.95))
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
	else
	{
		//保存当前面片
		if (!IWall->findPointInVector(triangleFather.pt1, vecPointstop))
		{
			vecPointstop.push_back(triangleFather.pt1);
		}
		if (!IWall->findPointInVector(triangleFather.pt2, vecPointstop))
		{
			vecPointstop.push_back(triangleFather.pt2);
		}
		if (!IWall->findPointInVector(triangleFather.pt3, vecPointstop))
		{
			vecPointstop.push_back(triangleFather.pt3);
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
			searchPoints(*ite,triangle, vecPoints,vecPointstop, vecTrangleRelation, vecNormal, vecColor);
		}
	}
}

void COsgExtractRoad::shampPointCloud(std::vector<osg::Vec3d>&vecPt, double r)
{
	size_t m_sum = vecPt.size();
	for (size_t i = 0; i < m_sum; i++)//删除重复的点
	{
		for (size_t j = i + 1; j < m_sum; j++)
		{
			if (distance2(vecPt[i], vecPt[j]) < r)
			{
				vecPt[j].x() = vecPt[m_sum - 1].x();
				vecPt[j].y() = vecPt[m_sum - 1].y();
				vecPt[j].z() = vecPt[m_sum - 1].z();

				m_sum--;
				j--;
			}
		}
	}
	vecPt.erase(vecPt.begin() + m_sum, vecPt.end());
}