// osgExtractAccuraOutlineEvent.cpp : 定义 DLL 应用程序的导出函数。
//



#include "stdafx.h"
#include "InterfaceBaseDrawer.h"
#include "osgExtractAccuraOutlineEvent.h"
#include "outline_2.h"
#include "InterfaceMFCExcute.h"
#include "toShpAndDxf.h"

void COsgExtractAccuraOutLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 
	int numChild = tempGroup->getNumChildren();

	if (isStart == false )
	{
		ClearGroupResult(tempGroup);
	}
	
	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_EXTRACT_ACCURA_OUTLINE);

	if (isActivated == false && flag == true)
	{
		vecWorldCoord.clear();
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画房屋范围，然后选点，即可提取轮廓线，按ESC退出事件");
	}
}


bool COsgExtractAccuraOutLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Matrix worldMatrix = pickResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			if (isStart == true)
			{
				DrawOutline(worldPt, vecWorldCoord, mRoot, worldMatrix);
				return true;
			}

			vecWorldCoord.push_back(worldPt);
			iSigEventCallBack->EmitSigShowOpsHint("按BackSpace回退上一个点，按Z撤销全部，按ESC退出事件");
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{

		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				if (isArea == true)//画范围状态
				{
					if (vecWorldCoord.size() > 0)
					{
						//撤销上个点
						vecWorldCoord.pop_back();
					}
					else
					{
						int childNum = mapperGroup->getNumChildren();//删除上一条轮廓线

						if (childNum > 0)
						{
							mapperGroup->removeChild(childNum - 1);//视觉上删除	
						}
					}	
				}
				if (isStart == true)//画轮廓状态
				{
					int childNum = mapperGroup->getNumChildren();//删除上一条轮廓线

					if(childNum>0)
					{
						mapperGroup->removeChild(childNum - 1);//视觉上删除
					}

					iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画房屋范围，然后选点，即可提取轮廓线，按ESC退出事件");
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldCoord.clear();
				reSet();
				ClearGroupResult(mapperGroup);
				ClearGroupResult(tempGroup);
				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画房屋范围，然后选点，即可提取轮廓线，按ESC退出事件");
			}
		}
	}
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}

bool COsgExtractAccuraOutLineEvent::reSet()//开始画范围
{
	isStart = false;
	isArea = true;
	vecWorldCoord.clear();
	return true;
}

bool COsgExtractAccuraOutLineEvent::startDrawOutline(osg::Vec3d worldPt, std::vector<osg::Vec3d> vecWorldCoord, osg::ref_ptr<osg::Group> root, osg::Matrix WorldMatrix)
{
	if (vecWorldCoord.size() < 3)
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("请先画范围！");
		return false;
	}

	isStart = true;
	isArea = false;
	osg::Vec4d color(1.0, 0, 0, 1.0);
	double size = 3.5f;
	
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	
	IDrawer->BaseDrawLineLoopOperate(vecWorldCoord, color, size - 2, tempGroup, WorldMatrix);
	return true;
}

bool COsgExtractAccuraOutLineEvent::DrawOutline(osg::Vec3d worldPt, std::vector<osg::Vec3d> vecWorldCoord, osg::ref_ptr<osg::Group> root, osg::Matrix WorldMatrix)
{
	std::vector<osg::Vec3d> vecLine;
	outline_2::ComputeOutline(vecWorldCoord, worldPt, root, vecLine);
	
	//画轮廓线
	if (vecLine.size() > 0)
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		osg::Vec4d color(1.0, 0, 1, 1.0);
		double size = 3.5f;
		IDrawer->BaseDrawLineLoopOperate(vecLine, color, size, mapperGroup, WorldMatrix);
	}

	return true;
}

