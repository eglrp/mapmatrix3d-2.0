// osgDrawEdge_1_Event.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgHandDrawEdge_1_Event.h"
#include "ThreadSafeUpdateCallback.h"
#include "SceneExtractWall.h"
void COsgHandDrawEdge_1_Event::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_DRAW_EDGE_BY_HAND_1);

	if (isActivated == false && flag == true)
	{
		vecWorldCoord.clear();
		ISigEventCallBack->EmitSigShowOpsHint("");

		ClearGroupResult(tempGroup);
	}

	if (isActivated && flag == false)																//刚激活
	{
		isCatch = true;   //关闭捕捉
		ISigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画线，双击结束，按BackSpace回退，按ESC退出事件     捕捉[X]：开启");
	}
}

bool COsgHandDrawEdge_1_Event::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;
			osg::Matrix worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			if (isCatch)
			{
				if (vecPoints.empty())
				{
					return false;
				}
				if (!CatchPointOperate(worldPt, vecPoints))
				{
					return false;
				}
			}
			//画多线
			osg::Vec4d color(1.0, 0, 0.0, 1.0);
			double size = 5.5f;
			LeftClickExtractHandPaintLineOperate(vecWorldCoord, worldPt, color, size, mapperGroup, worldMatrix);					   //启用画多线
			ISigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画线，双击结束，按BackSpace回退，按ESC退出事件，按X切换捕捉状态");
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;
			osg::Matrix worldMatrix = pResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//画多线
			osg::Vec4d color(1.0, 0, 0.0, 1.0);
			double size = 5.5f;
			MouseMoveExtractHandPaintLineOperate(vecWorldCoord, worldPt, color, size, tempGroup, worldMatrix);		//启用鼠标移动画线

		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			DoubleClickDrawExtractHandPaintOperate(vecWorldCoord, tempGroup);
			clearGroupSafety(mRoot, dataGroup);
			vecPoints.clear();
			vecWorldCoord.clear();
			
			ISigEventCallBack->EmitSigShowOpsHint("");
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				int childNum = mapperGroup->getNumChildren();

				if (childNum > 0)
				{
					mapperGroup->removeChild(childNum - 1);
				}

				vecWorldCoord.pop_back();
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
			{
				ClearGroupResult(mapperGroup);
				vecWorldCoord.clear();
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_C)					 //c
			{
				CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
				osg::Vec3d worldPt = pickResult.worldPt;
				osg::NodePath np = pickResult.nodePath;
				normal = pickResult.worldNm;
				worldMatrix = pickResult.matrix;
				mouseMoveEvent(worldPt, np);
				
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_X)					 //x
			{
				if (isCatch == true)
				{
					isCatch = false;
					ISigEventCallBack->EmitSigShowOpsHint("关闭捕捉");
				}
				else
				{
					isCatch = true;
					ISigEventCallBack->EmitSigShowOpsHint("开启捕捉");
				}
			}
		}
	}
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}

void COsgHandDrawEdge_1_Event::mouseMoveEvent(osg::Vec3d& worldPt, osg::NodePath np)
{
	std::vector<std::string> vecChildFileName;

	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	for (int i = np.size() - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> ((np)[i]);
		if (lod)
		{
			IFinder->FindLodBottomLevelFile(lod, vecChildFileName);
			break;
		}
	}
	vecPoints.clear();
	for (size_t j = 0; j < vecChildFileName.size(); ++j)
	{
		getPointsInPolygon(vecChildFileName[j], worldPt, 0.5);
	}
	clearGroupSafety(mRoot, dataGroup);
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
	for (int i = 0; i < vecPoints.size(); ++i)
	{
		IDrawer->BaseDrawPtOperate(vecPoints[i], osg::Vec4(0, 0, 1, 1), 4.5, dataGroup, worldMatrix);
	}
}

bool COsgHandDrawEdge_1_Event::getPointsInPolygon(string& childFileName, osg::Vec3d& worldPt, double r)
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
			CSceneExtractWall sceneExtractWall;
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
					/*osg::Vec3d nm;
					sceneExtractWall.NormalVector(pt1, pt2, pt3, nm);
					if (!sceneExtractWall.checkNomal(normal, nm, distance2(normal,osg::Vec3d(0,0,0)),15))
					{
						continue;
					}
					*/
					if (distance2(pt1, worldPt) <= r)
					{
						vecPoints.push_back(pt1);
					}
					if (distance2(pt2, worldPt) <= r)
					{
						vecPoints.push_back(pt2);
					}
					if (distance2(pt3, worldPt) <= r)
					{
						vecPoints.push_back(pt3);
					}
				}
			}
		}
	}
	return false;
}

bool COsgHandDrawEdge_1_Event::CatchPointOperate(osg::Vec3d &worldPt, std::vector<osg::Vec3d>& vecPoints)
{
	bool flag = false;
	double dis = 99999, d = 0;
	osg::Vec3d pt(0, 0, 0);
	for (size_t i = 0; i < vecPoints.size(); ++i)
	{
		d = distancep2p(worldPt, vecPoints[i]);

		if (d < dis)
		{
			dis = d;
			pt = vecPoints[i];
			flag = true;
		}
	}
	worldPt = pt;
	return flag;
}

void COsgHandDrawEdge_1_Event::clearGroupSafety(osg::Group* father, osg::Group* children)
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