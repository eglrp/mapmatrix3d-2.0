// osgFittingWallSpaceEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceBaseDrawer.h"
#include "osgFittingWallSpaceEvent.h"
#include "InterfaceOsgFinder.h"

#include "Geometry.h"
#include "cvLine.h"

#define WALLCOLOR osg::Vec4d(0,1,1,0.4)      //墙面颜色

void COsgFittingWallSpaceEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_FITTING_WALLSPACE);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(tempGroup);
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		IWall->addDraggerToScene(mRoot, slotGroup, editWallGroup);
		//读取面数据
		IWall->readWallDataFromSqlite3(planePara, iSqliteDatabase, "runData.db");
		
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		
		
		ClearGroupResult(editWallGroup);
		IWall->getWorldMatrix(mRoot, worldMatrix);
		for (size_t i = 0; i < planePara.size(); ++i)
		{
			IDrawer->BaseDrawPolygon(planePara[i].coord, WALLCOLOR, editWallGroup, worldMatrix);
		}
		
		
		//读取点数据
		IWall->readPointDataFromSqlite3(vecTotalTriangle,iSqliteDatabase,"Points");

		if (vecTotalTriangle.size()>0)
		{
			std::vector<osg::Vec3d> vecNormal;
			IWall->readPointDataFromSqlite3(vecNormal, iSqliteDatabase, "Normal");
			normal = vecNormal[0];
			if (IWall->fittingWallSpace(vecTotalTriangle, planePara, normal, worldMatrix))//拟合平面
			{
				InterfaceBaseDrawerFactory IDrawerFactory;
				InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
				IDrawer->BaseDrawPolygon(planePara.back().coord, WALLCOLOR, editWallGroup, worldMatrix);

				vecTotalTriangle.clear();
				IWall->clearPointDataInSqlite3(iSqliteDatabase,"Points");
				char strMsg[299];
				sprintf(strMsg, "提取墙面：墙面倾斜角：%f ° 墙面拟合误差：%lf", planePara.back().angle, planePara.back().err);
				iSigEventCallBack->EmitSigShowOpsHint(strMsg);
				IWall->saveWallDataToSqlite3(planePara, iSqliteDatabase, "runData.db");
			}
		}
		else
		{
			iSigEventCallBack->EmitSigShowOpsHint("提取墙面：找不到采样信息");
		}
	}
}

bool COsgFittingWallSpaceEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();

		//执行拖动相关事件
		if (IWall->draggerEvent(atWall, ea, aa, mViewer, _activeDragger, editWallGroup, tempGroup, partnerPoint, wallSelected, _pointer, endPoint, planePara, worldMatrix, WALLCOLOR))
		{
			//保存墙面信息
			IWall->saveWallDataToSqlite3(planePara, iSqliteDatabase, "runData.db");
		}
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)//单击事件
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			worldPt = pickResult.worldPt;
			
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			int _i = 0;
			for (std::vector<WallInfo>::iterator ite = (planePara).begin(); ite != (planePara).end(); ++ite)
			{
				if (CL::Point_In_Rectangle_3D(worldPt, partnerPoint, (*ite).coord))//点在墙面上
				{
					//点在wall上激活单击事件
					if (!pushEvent(ea, aa))
					{
						return false;
					}
					atWall = true;
					editWallGroup->removeChild(_i);
					IDrawer->BaseDrawPolygon((*ite).coord, WALLCOLOR, tempGroup, worldMatrix);
					wallSelected = *ite;
					(planePara).erase(ite);
					IWall->saveWallDataToSqlite3(planePara, iSqliteDatabase, "runData.db");
					return false;
				}
				++_i;
			}
			return false;
		}
		else if(IsKeyPress(ea))
		{ 
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_M)
			{
				char strMsg[99];
				sprintf(strMsg, "节点数:%d", mRoot->getNumChildren());
				iSigEventCallBack->EmitSigShowOpsHint(strMsg);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				int num = editWallGroup->getNumChildren();
				if (num>0)
				{
					editWallGroup->removeChild(num - 1);
					planePara.pop_back();
					//保存墙面信息
					IWall->saveWallDataToSqlite3(planePara, iSqliteDatabase, "runData.db");
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				ClearGroupResult(editWallGroup);
				ClearGroupResult(tempGroup);
				planePara.clear();
				//保存墙面信息
				IWall->saveWallDataToSqlite3(planePara, iSqliteDatabase, "runData.db");
			}
		}
		
	}
	return false;
}





bool COsgFittingWallSpaceEvent::pushEvent(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;
	_pointer.reset();
	if (mViewer->computeIntersections(ea.getX(), ea.getY(), intersections))
	{
		_pointer.setCamera(mViewer->getCamera());
		_pointer.setMousePosition(ea.getX(), ea.getY());
		for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin(); hitr != intersections.end(); ++hitr)
		{
			_pointer.addIntersection(hitr->nodePath, hitr->getWorldIntersectPoint());
		}
		for (osg::NodePath::iterator itr = _pointer._hitList.front().first.begin(); itr != _pointer._hitList.front().first.end(); ++itr)
		{
			CDraggerPoint* dragger = dynamic_cast<CDraggerPoint*>(*itr);
			if (dragger)
			{
				dragger->handle(_pointer, ea, aa);
				_activeDragger = dragger;
				return true;
			}
		}
	}
	return false;
}


