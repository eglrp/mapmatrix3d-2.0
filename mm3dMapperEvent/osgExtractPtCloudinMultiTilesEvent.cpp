// osgExtractPtCloudinMultiTilesEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgExtractPtCloudinMultiTilesEvent.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgExtractWall.h"
#include "SceneExtractWall.h"
#include "ThreadSafeUpdateCallback.h"
#include "qcomm.h"
#include "Geometry.h"
#include "alphaShapes.h"
#include "InterfaceOsgExtractRoad.h"

void COsgExtractPtCloudinMultiTilesEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_EXTRACT_ROAD);

	if (isActivated == false && flag == true)													 //不激活
	{
		clearGroupSafety(mRoot, tempGroup);
		vecSearchPoint.clear();
		vecFileNames.clear();
		vecTriangleRelation.clear();
		
		if (vecPtResult.size() > 0)
		{
			InterfaceOsgExtractWallFactory IWallFactory;
			InterfaceOsgExtractWall* IWall = IWallFactory.create();
			std::vector<std::vector<osg::Vec3d> > vecVec;
			vecVec.push_back(vecPtResult);
			IWall->savePointDataToSqlite3(vecVec, iSqliteDatabase, "Points", true);//追加保存
		}
		ISigEventCallBack->EmitSigShowOpsHint("");
		vecPtResult.clear();
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecColor.clear();
		vecNormal.clear();
		vecSearchPoint.clear();
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		IWall->clearPointDataInSqlite3(iSqliteDatabase, "Points", "runData.db");
		ISigEventCallBack->EmitSigShowOpsHint("道路提取：请沿道路画线提取点云，双击结束提取");
	}
}

bool COsgExtractPtCloudinMultiTilesEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		InterfaceOsgExtractRoadFactory IExtractRoadFactory;
		InterfaceOsgExtractRoad* IExtractRoad = IExtractRoadFactory.create();
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Vec3d normal = pickResult.worldNm;
			osg::NodePath np = pickResult.nodePath;
			worldMatrix = pickResult.matrix;
			pickResult = PickResult(ea.getX() + 20, ea.getY(), mViewer);
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			
			//碰撞有效 开线程提取点云
			IExtractRoad->toExtractRoad( mRoot, tempGroup, np, worldPt, &vecPtResult, &vecFileNames, &vecTriangleRelation, &vecNormal, &vecColor, worldMatrix, ISigEventCallBack);
			
			if (vecSearchPoint.size() > 0)
			{
				std::vector<osg::Vec3d> vecCoord;
				std::vector<osg::NodePath> vecNodePath;
				IExtractRoad->insertNodePath(mRoot, vecSearchPoint.back(),worldPt, vecNodePath, vecCoord);
				if (!vecNodePath.empty())
				{
					for (size_t i = 0;i<vecNodePath.size();++i)
					{
						IExtractRoad->toExtractRoad( mRoot, tempGroup, vecNodePath[i], vecCoord[i], &vecPtResult, &vecFileNames, &vecTriangleRelation,&vecNormal,&vecColor, worldMatrix, ISigEventCallBack);
					}
					//保存碰撞点（搜索起点）
					vecSearchPoint.push_back(worldPt);
				}
			}
			else
				vecSearchPoint.push_back(worldPt);

			ISigEventCallBack->EmitSigShowOpsHint("道路提取：按ctrl + 左键提取点云，提取完毕后手动绘制边缘");
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			
		}
		else if (IsDoubleClick(ea))
		{
			//ISigEventCallBack->EmitSigShowOpsHint("道路提取：正在提取点云，请稍候...");
			vecSearchPoint.clear();
			vecNormal.clear();
			vecColor.clear();
			vecTriangleRelation.clear();
			vecFileNames.clear();
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{				
				clearGroupSafety(mRoot, mapperGroup);
				clearGroupSafety(mRoot, tempGroup);
				vecSearchPoint.clear();
				
				vecPtResult.clear();
				vecFileNames.clear();
				vecTriangleRelation.clear();
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				if (vecSearchPoint.size()>0)
				{
					vecSearchPoint.pop_back();
					
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_KP_Enter || ea.getKey() == osgGA::GUIEventAdapter::KEY_Return)
			{
				ISigEventCallBack->EmitSigShowOpsHint("道路提取：正在计算道路边缘，请稍候...");
				//计算边缘
				
				//多线程计算
				
				IExtractRoad->toExtractRoadEdge(mRoot, mapperGroup, &vecPtResult,  &vecEdge, 0.5, worldMatrix,ISigEventCallBack);
				
				//删除点云图形	
				clearGroupSafety(mRoot, tempGroup);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_C )
			{
				CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
				osg::Vec3d worldPt = pickResult.worldPt;
				osg::NodePath np = pickResult.nodePath;
				worldMatrix = pickResult.matrix;
				mouseMoveEvent(worldPt, np);
			}
		}
	}
	return false;
}

void COsgExtractPtCloudinMultiTilesEvent::clearGroupSafety(osg::Group* father,osg::Group* children)
{
	osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(father->getUpdateCallback());
	if (spChildCallback != NULL)
	{
		int num = children->getNumChildren();
		osg::ref_ptr<osg::Node> childNode;
		for (int i = 0; i < num; i++)
		{
			childNode = children->getChild(i);
			spChildCallback->RemoveChild(children, childNode);
		}
	}
}

void COsgExtractPtCloudinMultiTilesEvent::mouseMoveEvent(osg::Vec3d& worldPt, osg::NodePath np)
{
	std::vector<std::string> vecChildFileName;
	
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	for (int i =np.size() - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> ((np)[i]);
		if (lod)
		{
			IFinder->FindLodBottomLevelFile(lod, vecChildFileName);
			break;
		}
	}
	vecSearchPoint.clear();
	for (size_t j = 0; j < vecChildFileName.size(); ++j)
	{
		
		getPointsInPolygon(vecChildFileName[j], worldPt, 0.5);
		/*{
			clearGroupSafety(mRoot, tempGroup);
			InterfaceBaseDrawerFactory IDrawerFactory;
			InterfaceBaseDrawer* IDrawer = IDrawerFactory.Create();
			osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
			for (int i = 0;i<vecSearchPoint.size();++i)
			{
				IDrawer->BaseDrawPtOperate(vecSearchPoint[i], osg::Vec4(1, 0, 0, 1), 4.5, tempGroup, worldMatrix);
			}

			return;
		}*/
			
	}
	clearGroupSafety(mRoot, tempGroup);
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
	for (int i = 0; i < vecSearchPoint.size(); ++i)
	{
		IDrawer->BaseDrawPtOperate(vecSearchPoint[i], osg::Vec4(0, 0, 1, 1), 4.5, tempGroup, worldMatrix);
	}
}

bool COsgExtractPtCloudinMultiTilesEvent::getPointsInPolygon(string& childFileName, osg::Vec3d& worldPt,double r)
{
	if (childFileName.empty())
	{
		return false;
	}
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(childFileName);

	if (!node)
	{
		return false;
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
					if (distance2(pt1,worldPt)<=r)
					{
						vecSearchPoint.push_back(pt1);
					}
					if (distance2(pt2, worldPt) <= r)
					{
						vecSearchPoint.push_back(pt2);
					}
					if (distance2(pt3, worldPt) <= r)
					{
						vecSearchPoint.push_back(pt3);
					}
				}
			}
		}
	}
	return false;
}



bool COsgExtractPtCloudinMultiTilesEvent::PtInTriangle(osg::Vec3d pt, osg::Vec3d& t1, osg::Vec3d& t2, osg::Vec3d& t3)
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
