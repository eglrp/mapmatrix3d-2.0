// osgHighLightObjectByClickEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgHighLightObjectByClickEvent.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceMFCExcute.h"
#include "osgUserDefineEffect.h"
#include <osg/ComputeBoundsVisitor>
#include "qcomm.h"

void COsgHighLightObjectByClickEvent::outputModelWithoutTexture(std::string outModelWithoutFileName)
{
	osg::ref_ptr<osg::Node> copyNode = dynamic_cast<osg::Node*> (selPagedLod->clone(osg::CopyOp::DEEP_COPY_ALL));
	osg::Matrix worldMatrix = highLightOutputer.obtainMatrix(selPagedLod);
	highLightOutputer.outputModelWithoutTexture(outModelWithoutFileName, copyNode, worldMatrix);
}

void COsgHighLightObjectByClickEvent::outputTextureModel(std::string outModelFileName)
{
	osg::ref_ptr<osg::Node> copyNode = dynamic_cast<osg::Node*> (selPagedLod->clone(osg::CopyOp::DEEP_COPY_ALL));
	osg::Matrix worldMatrix = highLightOutputer.obtainMatrix(selPagedLod);
	highLightOutputer.outputTextureModel(outModelFileName, copyNode, worldMatrix);
}

void COsgHighLightObjectByClickEvent::removeSelPagedLod()
{
	highLightOutputer.removeSelPagedLod(selPagedLod);
}


void COsgHighLightObjectByClickEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_HIGHLIGHT_OBJECT_BY_CLICK);

	if (isActivated == false && flag == true)
	{
		//消息置空
		
	}

	if (isActivated && flag == false)																//刚激活
	{

	}
}

bool COsgHighLightObjectByClickEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;
			osg::Vec3d localPt = pickResult.localPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//高亮
			osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
			osg::ref_ptr<osg::Node> pagedLod = selector.findSelPagedLod(sceneNode, worldPt, worldMatrix);

			if (pagedLod)
			{
				//高亮单体
				highLighter.highLightByColor(pagedLod);
			}
		}
		else if (IsLeftClick(ea) && GetKeyState(VK_SHIFT) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;
			osg::Vec3d localPt = pickResult.localPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//取消高亮
			osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
			osg::ref_ptr<osg::Node> pagedLod = selector.findSelPagedLod(sceneNode, worldPt, worldMatrix);

			if (pagedLod)
			{
				highLighter.cancelHighLightByColor(pagedLod);
			}
		}
		else if (IsRightClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			//选中单体并高亮
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
			selPagedLod = selector.findSelPagedLod(sceneNode, worldPt, worldMatrix);

			if (!selPagedLod)
			{
				return false;
			}

			//高亮
			bool isSuccess = highLighter.highLightByColor(selPagedLod);

			//弹出对话框
			if (isSuccess)
			{
				mViewer->getEventQueue()->mouseButtonRelease(ea.getX(), ea.getY(), 3);
				Sleep(100);

				int windowX = ea.getWindowX() + ea.getX();
				int windowY = ea.getWindowY() + ea.getWindowHeight() - ea.getY();
				iSigEventCallBack->EmitSigObjectMenuDlg(windowX, windowY);
			}
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) >= 0)
		{

		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{

			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{

			}
		}
	}

	return false;
}




