// osgClipByHeightEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgClipByHeightEvent.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgClipByHeight.h"

void COsgClipByHeightEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 
	reFactor = 1;
	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_CLIP_BY_HEIGHT);

	if (isActivated == false && flag == true)																	
	{
		//消息置空
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按上下键调整裁切高度，按ESC退出事件");
	}
}

bool COsgClipByHeightEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsKeyPress(ea))
		{
			osg::ref_ptr<osg::Group> sceneGroup = mRoot->getChild(0)->asGroup();

			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
			{
				//整体向上移动 并裁切
				double increaseH = 0.5;
				KeyDownMoveClipByHeight(increaseH, reFactor, sceneGroup);
				iSigEventCallBack->EmitSigShowOpsHint("按Z键撤销");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
			{
				//整体向下移动 并裁切
				double increaseH = -0.5;
				KeyDownMoveClipByHeight(increaseH, reFactor, sceneGroup);
				iSigEventCallBack->EmitSigShowOpsHint("按Z键撤销");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z 撤销
			{
				osg::ref_ptr<osg::Group> sceneGroup = mRoot->getChild(0)->asGroup();
				KeyCtrlZReverseClipByHeight(sceneGroup);
				iSigEventCallBack->EmitSigShowOpsHint("");
				isActivated = false;
			}
		}
	}

	return false;
}

void COsgClipByHeightEvent::ClipHeight()
{
	//根据计算结果画高度基准面
	osg::BoundingSphere bound = mRoot->getChild(0)->getBound();
	osg::Vec3f newCenter = bound.center();
	osg::Vec4f color(0.f, 1.f, 1.f, 0.8f);
	double radius = bound.radius();
	osg::ref_ptr<osg::Group> sceneGroup = mRoot->getChild(0)->asGroup();

	clipHeight = newCenter.z();

	InterfaceOsgClipByHeightFactory iOsgClipByHeightFactory;
	InterfaceOsgClipByHeight* iOsgClipByHeight = iOsgClipByHeightFactory.create();
	iOsgClipByHeight->StartClipSceneByHeight(newCenter.z(), reFactor, sceneGroup);										//开始按高度裁切场景
}

void COsgClipByHeightEvent::InvertClipByHeight()
{
	if (!isActivated)
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("未设置高度裁切线");
		return;
	}

	reFactor = reFactor * -1;

	osg::ref_ptr<osg::Group> sceneGroup = mRoot->getChild(0)->asGroup();
	osg::ref_ptr<osg::ClipNode> clipNode = dynamic_cast<osg::ClipNode*>(sceneGroup->getChild(0));
	InterfaceOsgClipByHeightFactory iOsgClipByHeightFactory;
	InterfaceOsgClipByHeight* iOsgClipByHeight = iOsgClipByHeightFactory.create();
	iOsgClipByHeight->ResetClipHeightAndRefactor(clipHeight, reFactor, clipNode);
}

void COsgClipByHeightEvent::KeyCtrlZReverseClipByHeight(osg::ref_ptr<osg::Group> mSceneGroup)
{
	osg::ref_ptr<osg::ClipNode> clipNode = dynamic_cast<osg::ClipNode*>(mSceneGroup->getChild(0));

	if (clipNode)
	{
		int numClipPlane = clipNode->getNumClipPlanes();

		if (numClipPlane > 1)
		{
			for (int i = numClipPlane;i>1;i--)
			{
				clipNode->removeClipPlane(i - 1);
			}

			osg::ref_ptr<osg::Node> oriNode = clipNode->getChild(0);
			mSceneGroup->replaceChild(clipNode, oriNode);
		}
		else if (numClipPlane == 1)
		{
			osg::ref_ptr<osg::Node> oriNode = clipNode->getChild(0);
			mSceneGroup->replaceChild(clipNode, oriNode);
		}
	}
}

void COsgClipByHeightEvent::KeyDownMoveClipByHeight(double moveY, double reFactor, osg::ref_ptr<osg::Group> mSceneGroup)
{
	//移动过程进行裁切
	osg::ref_ptr<osg::ClipNode> clipNode = dynamic_cast<osg::ClipNode*>(mSceneGroup->getChild(0));
	InterfaceOsgClipByHeightFactory iOsgClipByHeightFactory;
	InterfaceOsgClipByHeight* iOsgClipByHeight = iOsgClipByHeightFactory.create();

	if (clipNode)
	{
		iOsgClipByHeight->ResetClipHeightAndRefactor(clipHeight, reFactor, clipNode);
		clipHeight += moveY;
	}
}
