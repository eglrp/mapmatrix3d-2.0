// osgExtractLineInsectEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgExtractLineInsectEvent.h"
#include "InterfaceOsgExtractWall.h"
#include "InterfaceBaseDrawer.h"

#define PTCOLOR osg::Vec4d(0,0,1,1)          //点颜色
#define LINECOLOR osg::Vec4d(1,0,0,1)        //线颜色
void COsgExtractLineInsectEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_LINE_INSECT);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(tempGroup);
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecCoord.clear();
		iSigEventCallBack->EmitSigShowOpsHint("直线求交：按ctrl + 左键选择线段，Tab键切换端点");
	}
	
}

bool COsgExtractLineInsectEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)//单击事件
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			worldMatrix = pickResult.matrix;
			osg::Vec3d normal = pickResult.worldNm;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			
			IWall->lineInsectOprate(vecLineforInsect, vecCoord, worldPt, PointInLine1, PointInLine2, transName1, transName2, selectTrans1, selectTrans2, mapperGroup, tempGroup, worldMatrix, iSigEventCallBack);
			
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_KP_Enter || ea.getKey()==osgGA::GUIEventAdapter::KEY_Return)
			{
				IWall->calcIntersectOfLine(vecLineforInsect,vecCoord,mapperGroup,tempGroup,PointInLine1,PointInLine2,transName1,transName2,selectTrans1,selectTrans2,worldMatrix);
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				if (vecLineforInsect.empty())
				{
					return false;
				}
				int num = tempGroup->getNumChildren();
				if (num > 0)
				{
					ClearGroupResult(tempGroup);
					vecLineforInsect.clear();
					vecCoord.clear();
					return false;
				}
				num = mapperGroup->getNumChildren();
				if (num < 2)
				{
					return false;
				}

				mapperGroup->removeChild(num - 1);
				mapperGroup->removeChild(num - 2);
				InterfaceBaseDrawerFactory IDrawerFactory;
				InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
				for (size_t i = 0; i < 2; ++i)
				{
					osg::ref_ptr<osg::Group> group = new osg::Group;
					IDrawer->BaseDrawLineOperate(vecLineforInsect[i].p1, vecLineforInsect[i].p2, LINECOLOR, 3.5, group, worldMatrix);
					if (i == 0)
					{
						group->setName(transName1);
					}
					else
						group->setName(transName2);
					mapperGroup->addChild(group);
				}
				vecLineforInsect.clear();
				vecCoord.clear();
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Tab)
			{
				int num = tempGroup->getNumChildren();
				if (num > 0)
				{
					InterfaceBaseDrawerFactory IDrawerFactory;
					InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
					tempGroup->removeChild(num - 1);
					if (vecLineforInsect.size() == 1)
					{
						if (vecLineforInsect.back().p1 == PointInLine1)
						{
							IDrawer->BaseDrawPtOperate(vecLineforInsect.back().p2, PTCOLOR, 4.5, tempGroup, worldMatrix);
							PointInLine1 = vecLineforInsect.back().p2;
						}
						else
						{
							IDrawer->BaseDrawPtOperate(vecLineforInsect.back().p1, PTCOLOR, 4.5, tempGroup, worldMatrix);
							PointInLine1 = vecLineforInsect.back().p1;
						}
					}
					if (vecLineforInsect.size() == 2)
					{
						if (vecLineforInsect.back().p1 == PointInLine2)
						{
							IDrawer->BaseDrawPtOperate(vecLineforInsect.back().p2, PTCOLOR, 4.5, tempGroup, worldMatrix);
							PointInLine2 = vecLineforInsect.back().p2;
						}
						else
						{
							IDrawer->BaseDrawPtOperate(vecLineforInsect.back().p1, PTCOLOR, 4.5, tempGroup, worldMatrix);
							PointInLine2 = vecLineforInsect.back().p1;
						}
					}
				}
				return false;
			}
		}
	}
	return false;
}
