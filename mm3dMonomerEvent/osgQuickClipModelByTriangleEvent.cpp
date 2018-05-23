// osgQuickClipModelByTriangleEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgQuickClipModelByTriangleEvent.h"
#include "InterfaceOsgClip.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgEditor.h"

bool COsgQuickClipModelByTriangleEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			vecWorldPt.push_back(worldPt);
		}
		else if (IsDoubleClick(ea))
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			selectSceneByTriangle(vecWorldPt);
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{


			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldPt.clear();
				ClearGroupResult(editGroup);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete)
			{
				StartQuickClipSceneByTriangle();
			}
		}
	}

	return false;
}

void COsgQuickClipModelByTriangleEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_QUICK_CLIP_MODEL_BY_TRIANGLE);

	if (isActivated == false && flag == true)
	{
		//œ˚œ¢÷√ø’
		vecWorldPt.clear();
	}

	if (isActivated && flag == false)																//∏’º§ªÓ
	{

	}
}

void COsgQuickClipModelByTriangleEvent::selectSceneByTriangle(std::vector<osg::Vec3d> vecWorldPt)
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	std::vector<osg::PagedLOD*> vecPagedLod;
	iOsgFinder->FindPagedLod(sceneNode, vecPagedLod);

	CScope scope;
	scope.vecCoord = vecWorldPt;
	std::vector<CScope> vecScope;
	vecScope.push_back(scope);

	for (int i = 0; i < vecPagedLod.size(); i++)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = vecPagedLod[i];
		std::string name = pagedLod->getName();

		if (name.find("object") != std::string::npos)
		{
			osg::MatrixList matrixList = pagedLod->getWorldMatrices();
			int size = matrixList.size();

			if (size == 0)
			{
				continue;;
			}

			osg::Matrix worldMatrix = matrixList[size - 1];

			osg::ref_ptr<osg::MatrixTransform> mTrans = new osg::MatrixTransform;
			mTrans->setMatrix(worldMatrix);
			mTrans->addChild(pagedLod);

			std::map<double, osg::Vec3d> mapVerts;
			InterfaceOsgFinderFactory iOsgFinderFactory;
			InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
			iOsgFinder->FindTriangleByPolygon(pagedLod, vecScope, mapVerts, true);

			InterfaceBaseDrawerFactory iBaseDrawerFactory;
			InterfaceBaseDrawer* iBaseDrawer = iBaseDrawerFactory.create();

			for (std::map<double, osg::Vec3d>::iterator iter = mapVerts.begin(); iter != mapVerts.end(); iter++)
			{
				osg::Vec3d selPt = iter->second;
				selPt = selPt * worldMatrix;
				iBaseDrawer->BaseDrawPtOperate(selPt, osg::Vec4d(1.f, 0.f, 0.0f, 1.0f), 3.5f, editGroup, worldMatrix);
			}
		}
	}
}

void COsgQuickClipModelByTriangleEvent::StartQuickClipSceneByTriangle()
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	std::vector<osg::PagedLOD*> vecPagedLod;
	iOsgFinder->FindPagedLod(sceneNode, vecPagedLod);

	CScope scope;
	scope.vecCoord = vecWorldPt;
	std::vector<CScope> vecScope;
	vecScope.push_back(scope);

	for (int i = 0; i < vecPagedLod.size(); i++)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = vecPagedLod[i];
		std::string name = pagedLod->getName();

		if (name.find("object") != std::string::npos)
		{
			osg::MatrixList matrixList = pagedLod->getWorldMatrices();
			int size = matrixList.size();

			if (size == 0)
			{
				continue;;
			}

			osg::Matrix worldMatrix = matrixList[size - 1];

			osg::ref_ptr<osg::MatrixTransform> mTrans = new osg::MatrixTransform;
			mTrans->setMatrix(worldMatrix);
			mTrans->addChild(pagedLod);

			InterfaceOsgClipFactory iOsgClipFactory;
			InterfaceOsgClip* iOsgClip = iOsgClipFactory.create(BY_POLYGON);
			iOsgClip->clipOsgHeader(mTrans, vecScope, false);

			InterfaceOsgEditorFactory iOsgEditFactory;
			InterfaceOsgEditor* iOsgEdit = iOsgEditFactory.create();
			iOsgEdit->ReCalPagedLodCenter(pagedLod);
		}
	}

	vecWorldPt.clear();
	ClearGroupResult(editGroup);
}