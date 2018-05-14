// osgExtractWallInsectEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgExtractWallInsectEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgExtractWall.h"
#include <osgViewer/Renderer>


#define WALLSELECTCOLOR osg::Vec4d(0,1,1,0.8)//墙面选中颜色
#define LINECOLOR osg::Vec4d(1,0,0,1)        //线颜色
extern std::vector<WallInfo> gVecWall;
extern std::vector<CPointWall> gVecPW;
bool neiborTest(std::vector<int>& vec1, std::vector<int>& vec2)
{
	if (vec1.size()!= 3 || vec2.size()!= 3)
	{
		return false;
	}
	int k = 0;
	for (int i = 0;i<vec1.size();++i)
	{
		bool flag = false;
		for (int j = 0; j < vec2.size(); ++j)
		{
			if (vec1[i] == vec2[j])
			{
				flag = true;
				break;
			}
		}
		if (flag)
		{
			k++;
		}
	}
	return k == 2;

}

void COsgExtractWallInsectEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_WALLSPACE_INSECT);

	if (isActivated == false && flag == true)
	{
		if (conn)
		{
			PQfinish(conn);
			conn = NULL;
		}
		ClearGroupResult(tempGroup);
		for (int i = 0;i<editWallGroup->getNumChildren();i++)
		{
			if (editWallGroup->getChild(i)->getName() == "tempEdit")
			{
				editWallGroup->removeChildren(i,1);
				break;
			}
		}
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		if (!conn)
		{
			CPGController pgControl;
			conn = pgControl.connectToDB();
		}
		if (editWallGroup->getNumChildren() > 0)
		{
			InterfaceBaseDrawerFactory IDrawerFactory;
			InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
			InterfaceOsgExtractWallFactory IWallFactory;
			InterfaceOsgExtractWall* IWall = IWallFactory.create();
			osg::Matrix matrix;
			IWall->getWorldMatrix(mRoot, matrix);
			vecRoofPoint.clear();
			vecDropPoints.clear();
			float e = 0.2;
			for (std::vector<CPointWall>::iterator ite = gVecPW.begin(); ite != gVecPW.end();)
			{
				osg::Vec3d pt = (*ite).pt;
				//front
				osg::Vec3d p1(pt.x() - e,pt.y() - e,pt.z() - e);
				osg::Vec3d n1(0,1,0);
				//back
				osg::Vec3d p2(pt.x() - e, pt.y() + e, pt.z() - e);
				osg::Vec3d n2(0, -1, 0);
				//top
				osg::Vec3d p3(pt.x() - e, pt.y() - e, pt.z() + e);
				osg::Vec3d n3(0, 0, -1);
				//bottom
				osg::Vec3d p4(pt.x() - e, pt.y() - e, pt.z() - e);
				osg::Vec3d n4(0, 0, 1);
				//left
				osg::Vec3d p5(pt.x() - e, pt.y() - e, pt.z() - e);
				osg::Vec3d n5(1,0 , 0);
				//right
				osg::Vec3d p6(pt.x() + e, pt.y() - e, pt.z() - e);
				osg::Vec3d n6(-1, 0, 0);

				std::vector<osg::Plane> planeList;
				planeList.push_back(osg::Plane(n1, p1));
				planeList.push_back(osg::Plane(n2, p2));
				planeList.push_back(osg::Plane(n3, p3));
				planeList.push_back(osg::Plane(n4, p4));
				planeList.push_back(osg::Plane(n5, p5));
				planeList.push_back(osg::Plane(n6, p6));

				osg::Polytope poly(planeList);
				osgUtil::PolytopeIntersector* pI = new osgUtil::PolytopeIntersector(poly);
				/** 构造一个交集访问器*/
				osgUtil::IntersectionVisitor iv(pI);
				/** 根节点请求访问操作*/
				(mRoot->getChild(0)->asGroup()->getChild(0))->accept(iv);
				if (pI->containsIntersections())
				{
					 ++ite;
				}
				else
				{
					vecDropPoints.push_back(*ite);
					ite = gVecPW.erase(ite);
				}
					
			}


			if (gVecPW.size() > 1)
			{
				ClearGroupResult(editWallGroup);
				ClearGroupResult(tempGroup);
				for (auto pw : gVecPW)
				{
					IDrawer->BaseDrawPtOperate(pw.pt, osg::Vec4(0, 1, 0, 1), 5.0, tempGroup, matrix);
					vecRoofPoint.push_back(pw.pt);
				}
				if (tempGroup->getNumChildren() > 0)
				{
					isEditPoint = true;
					mvecBackCoord.clear();
				}
				gVecWall.clear();
				char msg[99];
				sprintf_s(msg, "鼠标点击可删除错误特征点，按C键可绘制特征线   特征点数：%d", tempGroup->getNumChildren());
				iSigEventCallBack->EmitSigShowOpsHint(msg);
			}
			else
			{
				iSigEventCallBack->EmitSigShowOpsHint("未得到有效房屋特征点，请重试");
				isActivated = false;
				flag = false;
			}	
		}
		else
		{
			iSigEventCallBack->EmitSigShowOpsHint("未找到房屋面，请重试");
			isActivated = false;
			flag = false;
		}
		
	}
}

bool COsgExtractWallInsectEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)//单击事件
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::Vec3d normal = pickResult.worldNm;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			if (isEditPoint)
			{
				float x = ea.getX();
				float y = ea.getY();
				osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();
				osgViewer::Renderer *render = dynamic_cast<osgViewer::Renderer *>(camera->getRenderer());
				osgUtil::SceneView *sceneView = render->getSceneView(0);
				osg::Vec3 vScreen;
				for (int i = 0; i < gVecPW.size(); ++i)
				{
					sceneView->projectObjectIntoWindow(gVecPW[i].pt, vScreen);
					if (vScreen.x() <= x + 3 && vScreen.x() >= x - 3 && vScreen.y() <= y + 3 && vScreen.y() >= y - 3)
					{
						tempGroup->removeChildren(i,1);
						ClearGroupResult(tempEditGroup);
						mvecBackCoord.push_back(gVecPW[i]);//删除前保存 可撤销
						gVecPW.erase(gVecPW.begin() + i);
						vecRoofPoint.erase(vecRoofPoint.begin() + i);
						char msg[99];
						sprintf_s(msg, "鼠标点击可删除错误特征点，按C键可绘制特征线   特征点数：%d", tempGroup->getNumChildren());
						iSigEventCallBack->EmitSigShowOpsHint(msg);
						return false;
					}
				}
			}
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			if (bRoofReady && vecBuildingDLG.size()>0)
			{
				osg::ref_ptr<osg::Group> group = new osg::Group;
				CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
				osg::Vec3d worldPt = pickResult.worldPt;
				osg::Matrix worldMatrix = pickResult.matrix;
				int n = vecBuildingDLG.size();
				for (int i = 0;i<n;++i)
				{
					osg::Vec3d p1(vecBuildingDLG[i].p1.x(), vecBuildingDLG[i].p1.y(), worldPt.z());
					osg::Vec3d p2(vecBuildingDLG[i].p2.x(), vecBuildingDLG[i].p2.y(), worldPt.z());
					IDrawer->BaseDrawLineOperate(p1, p2, osg::Vec4(1, 0, 0, 1), 3.5, group, worldMatrix);
					vecBuildingDLG.push_back(CLine(p1,p2));
				}
				for (int i = 0;i<vecRoofPoint.size();++i)
				{
					osg::Vec3d p(vecRoofPoint[i].x(), vecRoofPoint[i].y(), worldPt.z());
					IDrawer->BaseDrawLineOperate(p,vecRoofPoint[i],osg::Vec4(1,0,0,1),3.5,group,worldMatrix);
					vecBuildingDLG.push_back(CLine(p, vecRoofPoint[i]));
				}
				if (group->getNumChildren() > 0)
				{
					mapperGroup->addChild(group);
					bRoofReady = false;
					CPGController pgControl;
					pgControl.insertBuilding(conn, vecBuildingDLG);
					vecBuildingDLG.clear();
					//vecRoofLine.clear();
					vecRoofPoint.clear();
				}
			}
		}
		else if (IsMouseMove(ea))
		{
			if (isEditPoint)
			{
				float x = ea.getX();
				float y = ea.getY();
				CPickResult pickResult = PickResult(x, y, mViewer);
				osg::Vec3d worldPt = pickResult.worldPt;
				osg::Matrix worldMatrix = pickResult.matrix;
				osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();
				osgViewer::Renderer *render = dynamic_cast<osgViewer::Renderer *>(camera->getRenderer());
				osgUtil::SceneView *sceneView = render->getSceneView(0);
				osg::Vec3 vScreen;
				bool flag = false;
				for (int i = 0;i<gVecPW.size();++i)
				{
					sceneView->projectObjectIntoWindow(gVecPW[i].pt,vScreen);
					if (vScreen.x() <= x+3 && vScreen.x()>=x-3 && vScreen.y()<=y+3 && vScreen.y()>=y-3)
					{
						IDrawer->BaseDrawPtOperate(gVecPW[i].pt, osg::Vec4(0, 1, 0, 1), 6.0, tempEditGroup, worldMatrix);
						//return false;
						flag = true;
						break;
					}
				}
				for (int i = 0; i < vecDropPoints.size(); ++i)
				{
					sceneView->projectObjectIntoWindow(vecDropPoints[i].pt, vScreen);
					if (vScreen.x() <= x + 30 && vScreen.x() >= x - 30 && vScreen.y() <= y + 30 && vScreen.y() >= y - 30)
					{
						IDrawer->BaseDrawPtOperate(vecDropPoints[i].pt, osg::Vec4(1, 1, 0, 1), 4.0, tempEditGroup, worldMatrix);
						flag = true;
					}
				}
				if (!flag)
				{
					ClearGroupResult(tempEditGroup);
				}
			}
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				if (isEditPoint)
				{
					if (mvecBackCoord.size() > 0)
					{
						gVecPW.push_back(mvecBackCoord.back());
						osg::Matrix matrix;
						IWall->getWorldMatrix(mRoot, matrix);
						IDrawer->BaseDrawPtOperate(mvecBackCoord.back().pt, osg::Vec4(0, 1, 0, 1), 5.0, tempGroup, matrix);
						
						vecRoofPoint.push_back(mvecBackCoord.back().pt);
						mvecBackCoord.pop_back();
						char msg[99];
						sprintf_s(msg, "鼠标点击可删除错误特征点，按C键可绘制特征线   特征点数：%d", tempGroup->getNumChildren());
						iSigEventCallBack->EmitSigShowOpsHint(msg);
					}
					return false;
				}
				else
				{
					size_t num = mapperGroup->getNumChildren();
					if (num > 0)
					{
						mapperGroup->removeChild(num - 1);
						return false;
					}
				}
			}
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_C)
			{
				osg::Matrix matrix;
				IWall->getWorldMatrix(mRoot, matrix);
				if (isEditPoint)
				{
					osg::ref_ptr<osg::Group> group = new osg::Group;
					for (int i = 0; i < gVecPW.size() - 1; ++i)
					{
						for (int j = i + 1; j < gVecPW.size(); ++j)
						{
							if (neiborTest(gVecPW[i].vecWallIndex, gVecPW[j].vecWallIndex))
							{
								IDrawer->BaseDrawLineOperate(gVecPW[i].pt, gVecPW[j].pt, osg::Vec4(1, 0, 0, 1), 3.5, group, matrix);
								vecBuildingDLG.push_back(CLine(gVecPW[i].pt, gVecPW[j].pt));
								//vecRoofLine.push_back(CLine(gVecPW[i].pt, gVecPW[j].pt));
							}
						}
					}
					if (group->getNumChildren()>0)
					{
						mapperGroup->addChild(group);
						bRoofReady = true;
					}
					gVecPW.clear();
					mvecBackCoord.clear();
					ClearGroupResult(tempGroup);
					ClearGroupResult(tempEditGroup);
					isEditPoint = false;
					iSigEventCallBack->EmitSigShowOpsHint("");
					return false;
				}
				return false;
			}
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				ClearGroupResult(mapperGroup);
				ClearGroupResult(tempGroup);
				ClearGroupResult(tempEditGroup);
				ClearGroupResult(editWallGroup);	
				gVecWall.clear();
				gVecPW.clear();
				return false;
			}
		}
	}
	return false;
}

