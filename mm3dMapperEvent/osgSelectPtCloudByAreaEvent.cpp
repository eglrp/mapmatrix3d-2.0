// osgSelectPtCloudByAreaEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgSelectPtCloudByAreaEvent.h"
#include "InterfaceOsgExtractWall.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceBaseDrawer.h"
void COsgSelectPtCloudByAreaEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_SELECT_PTCLOUD_BY_Area);

	if (isActivated == false && flag == true)
	{
		CancelHighLightLineForAll(pointsGroup, vecHighLightTrans);
		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
		if (spChildCallback != NULL)
		{
			int num = pointsGroup->getNumChildren();
			osg::ref_ptr<osg::Node> childNode;
			for (int i = 0; i < num; i++)
			{
				childNode = pointsGroup->getChild(i);
				spChildCallback->RemoveChild(pointsGroup, childNode);
			}
		}
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		
		IWall->readPointDataFromSqlite3(vecTotalPoints, iSqliteDatabase, "Points");

		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
		IWall->getWorldMatrix(mRoot, worldMatrix);
		for (int i = 0; i < vecTotalPoints.size(); ++i)
		{
			IDrawer->BaseDrawPtOperate(vecTotalPoints[i], osg::Vec4d(0,0,1,1), 3.5, outlineGroup, worldMatrix);
		}
		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
		if (spChildCallback != NULL)
		{
			spChildCallback->AddChild(pointsGroup, outlineGroup);
		}
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，按Delete删除所选点，按ESC退出事件");
	}
}

bool COsgSelectPtCloudByAreaEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)				//拖动
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			vecWorldCoord.push_back(worldPt);
			iSigEventCallBack->EmitSigShowOpsHint("按Backspace 取消当前选择，按Z键取消全部选择");
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			CtrlDoubleClickSelectLineByArea(pointsGroup, vecWorldCoord, vecHighLightTrans);

			vecWorldCoord.clear();
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				int totalNum = vecHighLightTrans.size();

				if (totalNum == 0)
				{
					return false;
				}

				BackSpaceCancelHighLightLineInRectangle(vecHighLightTrans);

				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，按Delete删除所选线段，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				//取消高亮
				CancelHighLightLineForAll(pointsGroup, vecHighLightTrans);

				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，按Delete删除所选线段，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete)
			{
				COsgDeleteSelectPoints(pointsGroup, vecHighLightTrans,vecDelete);
				for (size_t i = 0;i<vecDelete.size();++i)
				{
					deleteItemInVector(vecDelete[i],vecTotalPoints);
				}
				InterfaceOsgExtractWallFactory IWallFactory;
				InterfaceOsgExtractWall* IWall = IWallFactory.create();
				std::vector<std::vector<osg::Vec3d> > vecVec;
				vecVec.push_back(vecTotalPoints);
				IWall->savePointDataToSqlite3(vecVec, iSqliteDatabase, "Points");
			}
		}
	}

	return false;
}

void COsgSelectPtCloudByAreaEvent::COsgDeleteSelectPoints(osg::ref_ptr<osg::Group> MapperGroup, std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans,std::vector<osg::Vec3d>& vecDelete)
{
	if (MapperGroup->getNumChildren() == 0)
	{
		return;
	}
	vecDelete.clear();
	for (int i = 0; i < MapperGroup->getNumChildren(); i++)
	{
		std::vector<osg::MatrixTransform*> vecMatrixTrans;
		InterfaceOsgFinderFactory IFinderFactory;
		InterfaceOsgFinder* IFinder = IFinderFactory.create();
		IFinder->FindTransNode(MapperGroup, vecMatrixTrans);

		for (int j = 0; j < vecMatrixTrans.size(); j++)
		{
			osg::ref_ptr<osg::MatrixTransform> mTrans = vecMatrixTrans[j];

			if (mTrans && mTrans->getName() == "highLight")
			{
				if (mTrans->getNumParents() == 0)
				{
					continue;
				}
				osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
				IFinder->ExtractVerts(mTrans, extracted_verts);
				osg::Matrix matrix = mTrans->getMatrix();
				if (extracted_verts->size() > 0)
				{
					osg::Vec3d pt(extracted_verts->at(0).x(), extracted_verts->at(0).y(), extracted_verts->at(0).z());
					pt = pt * matrix;
					vecDelete.push_back(pt);
				}
				osg::ref_ptr<osg::Group> parent = mTrans->getParent(0);

				if (parent)
				{
					parent->removeChild(mTrans);
				}
			}
		}
	}

	VecHighLightTrans.pop_back();
	
}



void COsgSelectPtCloudByAreaEvent::deleteItemInVector(osg::Vec3d& pt, std::vector<osg::Vec3d>& vecWorldCoord)
{
	if (vecWorldCoord.empty())
	{
		return;
	}
	for (std::vector<osg::Vec3d>::iterator ite = vecWorldCoord.begin(); ite != vecWorldCoord.end(); ++ite)
	{
		if (distance2(*ite, pt) <= 0.00001)
		{
			vecWorldCoord.erase(ite);
			return;
		}
	}
}