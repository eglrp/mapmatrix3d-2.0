// osgExtractPtCloudEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgExtractPtCloudEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgExtractWall.h"
#define PTCOLOR osg::Vec4d(0, 0, 1, 1)																   //点颜色
																					   //面片角度偏差阈值

void COsgExtractPtCloudEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_EXTRCT_PT_CLOUD);

	if (isActivated == false && flag == true)													 //不激活
	{

		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
		if (spChildCallback != NULL)
		{
			int num = tempGroup->getNumChildren();
			osg::ref_ptr<osg::Node> childNode;
			for (int i = 0; i < num; i++)
			{
				childNode = tempGroup->getChild(i);
				spChildCallback->RemoveChild(tempGroup, childNode);
			}
		}
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecTotalTriangle.clear();
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		IWall->clearPointDataInSqlite3(iSqliteDatabase, "Points");
		vecCorlor.clear();
		vecNormal.clear();
		iSigEventCallBack->EmitSigShowOpsHint("提取点云：按Ctrl + 鼠标左键提取点云");
	}
}


bool COsgExtractPtCloudEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Vec3d normal = pickResult.worldNm;
			osg::NodePath np = pickResult.nodePath;
			osg::Matrix worldMatrix = pickResult.matrix;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			
			iSigEventCallBack->EmitSigShowOpsHint("提取点云：正在提取...");
			std::vector<osg::Vec3d> vecTriangle;
			//自动采样
			InterfaceOsgExtractWallFactory IWallFactory;
			InterfaceOsgExtractWall* IWall = IWallFactory.create();
			vecNormal.push_back(normal);
			if (!IWall->DrawTriangleEx(mRoot, np, worldPt, worldMatrix, vecTriangle, vecNormal,vecCorlor, ANGLE, similarity))
			{
				iSigEventCallBack->EmitSigShowOpsHint("提取点云：提取失败，请重试");
				return false;
			}

			//提取成功，绘制点
			if (vecTriangle.size() > 0)
			{
				vecTotalTriangle.push_back(vecTriangle);
				std::vector<std::vector<osg::Vec3d> > vecTriangleTemp;
				vecTriangleTemp.push_back(vecTriangle);
				IWall->savePointDataToSqlite3(vecTriangleTemp, iSqliteDatabase, "Points", true);//追加保存
																								 			
				osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
				InterfaceBaseDrawerFactory IDrawerFactory;
				InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
				for (int i = 0; i < vecTriangle.size(); ++i)
				{
					IDrawer->BaseDrawPtOperate(vecTriangle[i], PTCOLOR, 3.5, outlineGroup, worldMatrix);
				}
				osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
				if (spChildCallback != NULL)
				{
					spChildCallback->AddChild(tempGroup, outlineGroup);
				}
				iSigEventCallBack->EmitSigShowOpsHint("提取点云：按BackSpace撤销提取点，按Ctrl + 鼠标左键继续提取");
			}
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				if (tempGroup->getNumChildren() > 0)
				{
					tempGroup->removeChild(tempGroup->getNumChildren() - 1);
					vecTotalTriangle.pop_back();
					IWall->savePointDataToSqlite3(vecTotalTriangle, iSqliteDatabase, "Points");//刷新保存
					if (!vecCorlor.empty())
					{
						vecCorlor.pop_back();
					}
					if (!vecNormal.empty())
					{
						vecNormal.pop_back();
					}	
				}
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				ClearGroupResult(tempGroup);
				vecTotalTriangle.clear();
				IWall->clearPointDataInSqlite3(iSqliteDatabase, "Points");//清空数据库
				vecCorlor.clear();
				vecNormal.clear();
				return false;
			}
		}
	}


	return false;
}



