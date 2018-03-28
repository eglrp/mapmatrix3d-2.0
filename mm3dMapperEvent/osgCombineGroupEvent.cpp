// osgCombineGroupEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgCombineGroupEvent.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgExtractWall.h"
#include "Geometry.h"

#define LINECOLOR osg::Vec4d(1,0,0,1)        //线颜色
#define COMBINECOLOR osg::Vec4d(1,0,1,1)      //组合颜色
void COsgCombineGroupEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_COMBINE);

	if (isActivated == false && flag == true)
	{
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecCoord.clear();
		iSigEventCallBack->EmitSigShowOpsHint("组合：按ctrl + 左键选择组合范围，双击左键结束(注意：组合前先进行线段交汇)");
	}

}

bool COsgCombineGroupEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
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
			vecCoord.push_back(worldPt);
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			IWall->combineGroup(numChild,vecCoord,vecLFC,mapperGroup,COMBINECOLOR,worldMatrix,iSigEventCallBack);//返回num记录绘制的墙面数
			vecCoord.clear();
			return false;
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)
			{
				if (vecCoord.size()>0)
				{
					vecCoord.pop_back();
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)//撤销组合结果
			{
				if (numChild > 0)
				{
					for (int i = 0; i < numChild; i++)
					{
						mapperGroup->removeChild(mapperGroup->getNumChildren() - 1);
					}

					numChild = 0;
					for (size_t i = 0; i < vecLFC.size(); ++i)
					{
						osg::ref_ptr<osg::Group> group = new osg::Group;
						IDrawer->BaseDrawLineOperate(vecLFC[i].p1, vecLFC[i].p2, LINECOLOR, 3.5, group, worldMatrix);
						group->setName(vecLFC[i].name.c_str());
						mapperGroup->addChild(group);
					}
					vecLFC.clear();
				}
			}
		}
	}
	return false;
}

