// osgRepairHoleEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgRepairHoleEvent.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgRepair.h"

void COsgRepairHoleEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_REPAIR_HOLE);

	if (isActivated == false && flag == true)																	
	{
		mTrans = NULL;
		vecWorldCoord.clear();
		//消息置空
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，完成画范围，然后进行修补，按ESC退出事件");
	}
}

bool COsgRepairHoleEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if(IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			osg::NodePath nodePath = pickResult.nodePath;
			bool isFindTransNode = ObtainTransNodeFromNodePath(pickResult.nodePath, mTrans);

			vecWorldCoord.push_back(worldPt);
			iSigEventCallBack->EmitSigShowOpsHint("按BackSpace回退上一个点，按Z全部撤销，按ESC退出事件");
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				if (vecWorldCoord.size() > 0)
				{
					//撤销上个点
					vecWorldCoord.pop_back();
					iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，完成画范围，然后进行修补，按ESC退出事件");
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)					 //z
			{
				vecWorldCoord.clear();
				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，完成画范围，然后进行修补，按ESC退出事件");

				if (vecChildNode.size() > 0)
				{
					osg::ref_ptr<osg::Node> saveRepairNode = vecChildNode.back();

					if (saveRepairNode->getNumParents() > 0)
					{
						osg::ref_ptr<osg::Group> parent = saveRepairNode->getParent(0);
						parent->removeChild(saveRepairNode);
						vecChildNode.pop_back();
					}
				}
			}
		}
	}
	else
	{
		vecWorldCoord.clear();
	}
			
	return false;
}

bool COsgRepairHoleEvent::StartRepairHole()
{
	if (vecWorldCoord.size() < 3)
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("需要至少三个点进行构网, 请继续刺点");
		return false;
	}

	std::string imageFileName;
	std::string extName = "png (*.png)|*.png|jpg (*.jpg)|*.jpg|All Files (*.*)|*.*||";
	InterfaceMFCExcuteFactory IExcuteFactory;
	InterfaceMFCExcute* IExcute = IExcuteFactory.create();
	bool isSuccess = IExcute->OpenDialog(imageFileName, extName, true);

	if (isSuccess)
	{
		osg::Matrix worldMatrix = mTrans->getMatrix();
		//创建补洞节点
		InterfaceOsgRepairFactory IRepairFactory;
		InterfaceOsgRepair* IRepair = IRepairFactory.create();
		osg::ref_ptr<osg::Node> repairNode = IRepair->createRepairPatch(vecWorldCoord, worldMatrix, imageFileName);
		osg::ref_ptr<osg::Group> parent = mTrans->getChild(0)->asGroup();

		if (!repairNode)
		{
			return false;
		}
			
		vecParentNode.push_back(parent);
		vecChildNode.push_back(repairNode);
		
		//插入节点
		parent->insertChild(0, repairNode);
		vecWorldCoord.clear();
		std::vector<OperatingState> vecOPS;
		vecOPS.push_back(OPS_None);
		AddViewerOps(vecOPS);

		//重新刺点
		vecOPS.clear();
		vecOPS.push_back(OPS_DRAW_CLOSED_LINE);
		vecOPS.push_back(OPS_REPAIR_HOLE);
		AddViewerOps(vecOPS);

		return true;
	}
	else
	{
		return false;
	}
}

void COsgRepairHoleEvent::AddViewerOps(std::vector<OperatingState> vecOps)
{
	for(osgViewer::View::EventHandlers::iterator hitr = mViewer->getEventHandlers().begin(); hitr != mViewer->getEventHandlers().end(); ++hitr)
	{
		osgGA::EventHandler* pEvent = *hitr;

		osg::ref_ptr<CBaseUserEvent> ph = dynamic_cast<CBaseUserEvent*> (pEvent);

		if (!ph)
		{
			continue;
		}

		ph->UpdateActivatedStatus(vecOps);
	}

}

bool COsgRepairHoleEvent::ObtainTransNodeFromNodePath(osg::NodePath nodePath,  osg::ref_ptr<osg::MatrixTransform> &mTrans)
{
	//裁切
	bool isFindTransNode = false;

	for (int i = 0; i < nodePath.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> node = dynamic_cast<osg::MatrixTransform*> (nodePath[i]);

		if (!node)
		{
			continue;
		}
			
		if (node->getName() == "repairMatrix")
		{
			continue;
		}

		if (mTrans != NULL)
		{
			InterfaceOsgFinderFactory iFinderFactory;
			InterfaceOsgFinder* iFinder = iFinderFactory.create();

			std::vector<osg::MatrixTransform*> vecMatrixTransform;
			osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0);
			iFinder->FindTransNode(sceneNode, vecMatrixTransform);
			int nodeIndex = 9999; int transIndex = 0;

			for (int k = 0; k < vecMatrixTransform.size(); k++)
			{
				if (vecMatrixTransform[k] == node)
				{
					nodeIndex = k;
					break;
				}
			}

			for (int k = 0; k < vecMatrixTransform.size(); k++)
			{
				if (vecMatrixTransform[k] == mTrans)
				{
					transIndex = k;
					break;
				}
			}

			if (nodeIndex < transIndex)
			{
				mTrans = node;
			}
		}
		else
		{
			mTrans = node;
		}

		isFindTransNode = true;
		break;
	}

	if (isFindTransNode == false)
	{
		return false; 
	}

	return true;
}