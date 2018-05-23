// osgManualExtractTexture.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgManualExtractTextureEvent.h"
#include "InterfaceOsgClip.h"
#include "InterfaceOsgDomGenerator.h"
#include "InterfaceOsgRepair.h"
#include "InterfaceOsgEditor.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgExtractTexture.h"
#include "qcomm.h"
#include "cvLine.h"
#include <numeric> 

bool COsgManualExtractTextureEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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
			pickNormal = pickResult.worldNm;
		}
		else if(IsDoubleClick(ea) && GetKeyState(VK_CONTROL) >= 0)
		{
			/*CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			judgePt = worldPt;*/
		}
		else if(IsKeyPress(ea))
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

void COsgManualExtractTextureEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_MANUAL_EXTRACT_TEXTURE);

	if (isActivated == false && flag == true)																	
	{
		//消息置空
		vecWorldPt.clear();
	}

	if (isActivated && flag == false)																//刚激活
	{
		
	}
}

void COsgManualExtractTextureEvent::StartExtractTexture(double res, double deltaZ)
{
	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	InterfaceOsgExtractTextureFactory iExtractTextureFactory;
	InterfaceOsgExtractTexture* iExtractTexture = iExtractTextureFactory.create();
	osg::ref_ptr<osg::Node> dstOsgNode = iExtractTexture->ExtractTexture(vecWorldPt, sceneNode, res);

	InterfaceOsgEditorFactory iEditorFactory;
	InterfaceOsgEditor* iEditor = iEditorFactory.create();
	iEditor->SetLodFileName(dstOsgNode, "");
	osgDB::writeNodeFile(*dstOsgNode, "E:\\1.osgb");

	vecWorldPt.clear();
}