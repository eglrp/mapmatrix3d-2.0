// osgSceneMatchEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "InterfaceOsgOrient.h"
#include "osgSceneMatchEvent.h"
#include "InterfaceMFCExcute.h"
#include "qcomm.h"

COsgSceneMatchEvent::COsgSceneMatchEvent(osgViewer::Viewer *viewer, osg::Group *root, osg::Group* editGroup, InterfaceSigEventCallBack* iSigEventCallBack) : CBaseUserEvent(viewer)
{
	mViewer = viewer;
	mRoot = root;
	mEditGroup = editGroup;
	this->iSigEventCallBack = iSigEventCallBack;
	isActivated = false;
}


bool COsgSceneMatchEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			int totalNum = vecWorldCoord.size();
			vecWorldCoord.push_back(worldPt);

			if (totalNum % 2 == 1)
			{
				vecRelaCoord.push_back(worldPt);
			}
			else
			{
				vecAbsCoord.push_back(worldPt);
			}

			iSigEventCallBack->EmitSigShowOpsHint("按Backspace回退上一个点，右击文件列表的模型进行匹配");
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				//回撤一个点
				int totalNum = vecWorldCoord.size();

				if (totalNum % 2 == 0)
				{
					vecRelaCoord.pop_back();
				}
				else
				{
					vecAbsCoord.pop_back();
				}

				vecWorldCoord.pop_back();
				iSigEventCallBack->EmitSigShowOpsHint("先左击需要定向模型的同名点，然后左击参考模型的同名点，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				osg::ref_ptr<osg::Group> sceneGroup = mRoot->getChild(0)->asGroup();

				int selCnt = iSigEventCallBack->EmitSigSelModelIndex();													//获得选择模型

				if (selCnt < 0)
				{
					return false;
				}

				osg::ref_ptr<osg::Node> sceneNode = sceneGroup->getChild(selCnt);

				InterfaceOsgOrientFactory IOrientFactory;
				InterfaceOsgOrient* IOrient = IOrientFactory.create();
				IOrient->ReverseModelOrientation(sceneNode, absMatrix);

				osg::Matrix matrix;
				absMatrix = matrix;
				iSigEventCallBack->EmitSigShowOpsHint("先左击需要定向模型的同名点，然后左击参考模型的同名点，按ESC退出事件");
			}
		}
	}
	else
	{
		vecAbsCoord.clear();
		vecRelaCoord.clear();
		vecWorldCoord.clear();
	}

	return false;
}

void COsgSceneMatchEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_MODEL_MATCHING);

	if (isActivated == false && flag == true)																	
	{
		//消息置空
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("先左击需要定向模型的同名点，然后左击参考模型的同名点，按ESC退出事件");
	}
}

void COsgSceneMatchEvent::MatchSingleModel()
{
	// TODO: 在此添加命令处理程序代码
	if (vecRelaCoord.size() < 3 && vecAbsCoord.size() < 3)
	{
		InterfaceMFCExcuteFactory IMFCExecuteFactory;
		InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();
		IMFCExecute->PopUpMessage("同名点对数不能小于3");
		return;
	}

	int selModel = iSigEventCallBack->EmitSigSelModelIndex(); //获取选择的场景

	if (selModel < 0)
	{
		return;
	}

	std::vector<CoordDouble3D> vecAbsPt, vecRelaPt;

	for (int i = 0; i < vecAbsCoord.size(); i++)
	{
		vecAbsPt.push_back(CoordDouble3D(vecAbsCoord[i].x(), vecAbsCoord[i].y(), vecAbsCoord[i].z()));
	}

	for (int i = 0; i < vecAbsCoord.size(); i++)
	{
		vecRelaPt.push_back(CoordDouble3D(vecRelaCoord[i].x(), vecRelaCoord[i].y(), vecRelaCoord[i].z()));
	}

	CSevenParameter sevenParamCalulator;
	sevenParamCalulator.LoadControlPoints(vecRelaPt, vecAbsPt);

	sevenParamCalulator.CalculateSevenParameter();

	osg::ref_ptr<osg::Node> oriNode = mRoot->getChild(0)->asGroup()->getChild(selModel);

	InterfaceOsgOrientFactory IOrientFactory;
	InterfaceOsgOrient* IOrient = IOrientFactory.create();
	IOrient->ModelOrientation(sevenParamCalulator, oriNode, absMatrix);

	mViewer->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Space);

	ClearGroupResult(mEditGroup);
	vecAbsCoord.clear();
	vecRelaCoord.clear();
	iSigEventCallBack->EmitSigShowOpsHint("按z键撤销匹配结果，按ESC退出事件");
}


