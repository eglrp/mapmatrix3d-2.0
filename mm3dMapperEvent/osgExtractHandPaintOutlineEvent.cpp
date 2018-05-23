// osgExtractHandPaintEvent.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include "InterfaceBaseDrawer.h"
#include "osgExtractHandPaintOutlineEvent.h"
#include "InterfaceMFCExcute.h"
#include "toShpAndDxf.h"

void COsgExtractHandPaintOutLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_EXTRACT_HAND_PAINT_RING);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(mapperGroup);
		vecWorldCoord.clear();
		vecVec.clear();
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键手动画轮廓线，按ESC退出事件");
	}
}





bool COsgExtractHandPaintOutLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			worldPt = pickResult.worldPt;
			//osg::Vec3d normal = pickResult.worldNm;
			worldMatrix = pickResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			vecWorldCoord.push_back(worldPt);
			iSigEventCallBack->EmitSigShowOpsHint("按BackSpace回退上一个点，按Z撤销全部，按ESC退出事件");
		}
		else if (IsDoubleClick(ea))
		{
			if (vecWorldCoord.size()<3)
			{
				return false;
			}
			InterfaceBaseDrawerFactory IDrawerFactory;
			InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
			osg::Vec4d color(1.0, 0, 1, 1.0);
			double size = 3.5f;
			IDrawer->BaseDrawLineLoopOperate(vecWorldCoord, color, size, mapperGroup, worldMatrix);
			vecVec.push_back(vecWorldCoord);
			vecWorldCoord.clear();
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				if (vecWorldCoord.size() > 0)
				{
					//撤销上个点
					vecWorldCoord.pop_back();
				}
				else
				{
					int childNum = mapperGroup->getNumChildren();//删除上一条轮廓线
					if(childNum>0)
					{
						mapperGroup->removeChild(childNum - 1);//视觉上删除
						if (!vecVec.empty())
						{
							vecVec.pop_back();//数据上删除
						}
					}
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldCoord.clear();
				vecVec.clear();
				ClearGroupResult(mapperGroup);
				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画轮廓线，双击左键完成绘制，按ESC退出事件");
			}
		}
	}
	else
	{
		vecWorldCoord.clear();
	}
	return false;
}

void COsgExtractHandPaintOutLineEvent::clearVec()
{
	vecVec.clear();
}

std::vector<std::vector<osg::Vec3d> >& COsgExtractHandPaintOutLineEvent::getVec()
{
	return vecVec;
}