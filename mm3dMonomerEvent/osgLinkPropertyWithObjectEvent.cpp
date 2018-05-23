// osgLinkPropertyWithObjectEvent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "osgLinkPropertyWithObjectEvent.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceOsgFinder.h"
#include "publicDefined.h"
#include <osg/ComputeBoundsVisitor>
#include "qcomm.h"

void COsgLinkPropertyWithObjectEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_LINK_PROPERTY_WITH_OBJECT);

	if (isActivated == false && flag == true)
	{
		//消息置空

	}

	if (isActivated && flag == false)																//刚激活
	{

	}
}

bool COsgLinkPropertyWithObjectEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			//选中单体并取消高亮
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;
			osg::NodePath np = pickResult.nodePath;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			//取消高亮
			osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);
			InterfaceOsgFinderFactory iOsgFinderFactory;
			InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();

			propertyNode = selector.findSelPagedLod(sceneNode, worldPt, worldMatrix);

			if (!propertyNode)
			{
				return false;
			}

			highLighter.cancelHighLightByColor(propertyNode);
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
			propertyNode = selector.findSelPagedLod(sceneNode, worldPt, worldMatrix);

			if (!propertyNode)
			{
				return false;
			}
			
			//高亮
			bool isSuccess = highLighter.highLightByColor(propertyNode);
			std::string propertyListName = propertyNode->getName();

			//弹出对话框
			if (isSuccess)
			{
				int len = propertyListName.length();

				char strMsg[MAX_PATH];
				sprintf_s(strMsg, "len: %d\n", len);
				_logop(strMsg);

				char* strPropertyListName = (char *)malloc((len) * sizeof(char));
				propertyListName.copy(strPropertyListName, len, 0);
				strPropertyListName += '\0';
				iSigEventCallBack->EmitSigLinkPropertyListDlg(strPropertyListName);
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

void COsgLinkPropertyWithObjectEvent::SetObjectProperty(std::string strObjectProperty)
{
	if (!propertyNode)
	{
		return;
	}

	propertyNode->setName(strObjectProperty.c_str());
}

void COsgLinkPropertyWithObjectEvent::LookUpOjbectProperty(std::string objectPropertyName, std::string objectPropertyValue)
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	if (!sceneNode)
	{
		return;
	}
		
	InterfaceOsgFinderFactory iOsgFinderFactory;
	InterfaceOsgFinder* iOsgFinder = iOsgFinderFactory.create();
	osg::ref_ptr<osg::Node> selectPropertyNode = iOsgFinder->FindObjectPropertyNodeByName(sceneNode, objectPropertyName, objectPropertyValue);

	if (!selectPropertyNode)
	{
		return;
	}

	int numParent = selectPropertyNode->getNumParents();

	if (numParent == 0)
	{
		return;
	}

	osg::ref_ptr<osg::Node> parentPropertyNode = selectPropertyNode->getParent(0);

	if (!parentPropertyNode)
	{
		return;
	}

	int grandNumParent = parentPropertyNode->getNumParents();

	if (grandNumParent == 0)
	{
		return;
	}

	osg::ref_ptr<osg::Node> grandPropertyNode = parentPropertyNode->getParent(0);

	if (!grandPropertyNode)
	{
		return;
	}

	//飞行过去
	FlyToPropertyNode(grandPropertyNode);

	//高亮节点
	highLighter.StartHighLight(grandPropertyNode);
}

void COsgLinkPropertyWithObjectEvent::FlyToPropertyNode(osg::ref_ptr<osg::Node> grandPropertyNode)
{
	osg::ComputeBoundsVisitor cbbv;
	grandPropertyNode->accept(cbbv);
	osg::BoundingBox bb = cbbv.getBoundingBox();
	osg::Vec3f bp = bb.center();
	double radius = bb.radius();
	// 创建矩阵，指定到视点的距离。
	osg::Matrix trans;
	trans.makeTranslate(bp.x(), bp.y(), bp.z() + radius * 2);
	// 旋转一定角度（弧度值）。
	double angle(0);
	// 创建旋转矩阵。
	osg::Matrix rot;
	rot.makeRotate(osg::DegreesToRadians(angle), osg::Vec3(1., 0., 0.));

	// 设置视口矩阵（旋转矩阵和平移矩阵连乘）。
	mViewer->getCameraManipulator()->setByMatrix(rot * trans);
}