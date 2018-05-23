// osgDragPointEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDragPointEvent.h"
#include "InterfaceOsgExtractWall.h"
#include "Geometry.h"

#define EPS 0.00001
void COsgDragPointEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_DRAG_POINT);

	if (isActivated == false && flag == true)
	{
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		std::vector<std::vector<osg::Vec3d> > vecPoins;
		vecPoins.push_back(vecWorldCoord);
		IWall->savePointDataToSqlite3(vecPoins, iSqliteDatabase, "Points");
		ClearGroupResult(pointsGroup);
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecBackup.clear();
		//读取数据
		loadPointsData(vecWorldCoord,iSqliteDatabase,"Points");
		//获得worldMatrix
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		IWall->getWorldMatrix(tempGroup, worldMatrix);
		//画点
		if (vecWorldCoord.size() > 0)
		{
			InterfaceBaseDrawerFactory IDrawFactory;
			InterfaceBaseDrawer* IDrawer = IDrawFactory.create();

			for (size_t i = 0; i < vecWorldCoord.size(); ++i)
			{
				IDrawer->BaseDrawPtOperate(vecWorldCoord[i], osg::Vec4d(0, 0, 1, 1), 3.5, pointsGroup, worldMatrix);
			}
		}
		iSigEventCallBack->EmitSigShowOpsHint("移动点位：按ctrl + 左键选择需移动的点，移动到新位置释放左键完成点位移动");
	}

}

bool COsgDragPointEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();

		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)//单击事件
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			worldMatrix = pickResult.matrix;
			osg::Vec3d normal = pickResult.worldNm;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			osg::Vec3d ptFind;
			if (findPointNearby(worldPt,ptFind,pointsGroup,0.05))
			{
				vecBackup.clear();
				vecBackup.push_back(ptFind);
				deleteItemInVector(ptFind, vecWorldCoord);
				dragflag = true;
			}
			else
				dragflag = false;
			
		}
		if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			if (dragflag)
			{
				ClearGroupResult(tempGroup);
				IDrawer->BaseDrawPtOperate(worldPt, osg::Vec4d(1, 0, 0, 1), 3.5, tempGroup, worldMatrix);
			}
		}
		if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE )
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			if (dragflag)
			{
				ClearGroupResult(tempGroup);
				IDrawer->BaseDrawPtOperate(worldPt, osg::Vec4d(0, 0, 1, 1), 3.5, pointsGroup, worldMatrix);
				vecWorldCoord.push_back(worldPt);
				dragflag = false;
				iSigEventCallBack->EmitSigShowOpsHint("点位移动：按Backspace键可撤销此次移动");
			}
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			
			return false;
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				if (vecBackup.size()>0)
				{
					pointsGroup->removeChild(pointsGroup->getNumChildren() - 1);
					IDrawer->BaseDrawPtOperate(vecBackup.back(), osg::Vec4d(0, 0, 1, 1), 3.5, pointsGroup, worldMatrix);
					vecBackup.pop_back();
					iSigEventCallBack->EmitSigShowOpsHint("移动点位：按ctrl + 左键选择需移动的点，移动到新位置释放左键完成点位移动");
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				ClearGroupResult(pointsGroup);
				//读取数据
				loadPointsData(vecWorldCoord, iSqliteDatabase, "Points");
				//画点
				if (vecWorldCoord.size() > 0)
				{
					InterfaceBaseDrawerFactory IDrawFactory;
					InterfaceBaseDrawer* IDrawer = IDrawFactory.create();

					for (size_t i = 0; i < vecWorldCoord.size(); ++i)
					{
						IDrawer->BaseDrawPtOperate(vecWorldCoord[i], osg::Vec4d(0, 0, 1, 1), 3.5, pointsGroup, worldMatrix);
					}
				}
				iSigEventCallBack->EmitSigShowOpsHint("移动点位：按ctrl + 左键选择需移动的点，移动到新位置释放左键完成点位移动");
			}
		}
	}
	return false;
}

//搜索附近的点
bool COsgDragPointEvent::findPointNearby(osg::Vec3d& worldPt, osg::Vec3d& ptFind, osg::Group *pointsGroup, double dis)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	IFinder->FindTransNode(pointsGroup, vecTrans);
	
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];
		if (mTrans)
		{
			osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
			IFinder->ExtractVerts(mTrans, extracted_verts);
			osg::Matrix matrix = mTrans->getMatrix();

			if (extracted_verts->size() != 1)
			{
				continue;
			}

			osg::Vec3d pt(extracted_verts->at(0).x(), extracted_verts->at(0).y(), extracted_verts->at(0).z());
			pt = pt * matrix;

			if (CL::distance_of_2_Point3D(pt, worldPt) <= dis)
			{
				//删除点
				osg::ref_ptr<osg::Group> parent = mTrans->getParent(0);
				if (parent)
				{
					parent->removeChild(mTrans);
				}
				ptFind = pt;
				return true;
			}

		}
	}
	return false;
}


//双击删除选中点
void COsgDragPointEvent::doubleClickDeletePoint(osg::Vec3d&worldPt, osg::Group *pointsGroup,double dis)
{
	osg::Vec3d pt;
	if (findPointNearby(worldPt, pt, pointsGroup, dis) == true)
	{
		vecBackup.push_back(pt);
	}
}

//从数据库读取点数据
void COsgDragPointEvent::loadPointsData(std::vector<osg::Vec3d>& vecWorldCoord, InterfaceSqliteDatabase* iSqliteDatabase, char* name)
{
	vecWorldCoord.clear();
	InterfaceOsgExtractWallFactory IWallFactory;
	InterfaceOsgExtractWall* IWall = IWallFactory.create();
	IWall->readPointDataFromSqlite3(vecWorldCoord, iSqliteDatabase, name);
}

//删除vector中指定元素
void COsgDragPointEvent::deleteItemInVector(osg::Vec3d& pt, std::vector<osg::Vec3d>& vecWorldCoord)
{
	if (vecWorldCoord.empty())
	{
		return;
	}
	for (std::vector<osg::Vec3d>::iterator ite = vecWorldCoord.begin();ite != vecWorldCoord.end();++ite)
	{
		if (distance2(*ite,pt) <= EPS)
		{
			vecWorldCoord.erase(ite);
			return;
		}
	}
}