// osgSelectLineByAreaEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgSelectLineByAreaEvent.h"

void COsgSelectLineByAreaEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_SELECT_LINE_BY_Area);

	if (isActivated == false && flag == true)
	{
		CancelHighLightLineForAll(mapperGroup, vecHighLightTrans);
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，按Delete删除所选线段，按ESC退出事件");
	}
}

bool COsgSelectLineByAreaEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if(IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)				//拖动
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			vecWorldCoord.push_back(worldPt);
			iSigEventCallBack->EmitSigShowOpsHint("按Backspace 取消当前选择，按Z键取消全部选择");
		}
		else if (IsDoubleClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			CtrlDoubleClickSelectLineByArea(mapperGroup, vecWorldCoord, vecHighLightTrans);

			vecWorldCoord.clear();
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				int totalNum = vecHighLightTrans.size();

				if (totalNum == 0)
				{
					return false;
				}

				BackSpaceCancelHighLightLineInRectangle(vecHighLightTrans);

				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，按Delete删除所选线段，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				//取消高亮
				CancelHighLightLineForAll(mapperGroup, vecHighLightTrans);

				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画范围，按Delete删除所选线段，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete)
			{
				COsgDeleteSelectLine(mapperGroup, vecHighLightTrans);
			}
		}
	}

	return false;
}

bool COsgSelectLineByAreaEvent::CtrlDoubleClickSelectLineByArea(osg::ref_ptr<osg::Group> MapperGroup, std::vector<osg::Vec3d> &VecWorldCoord, std::vector<std::vector<osg::MatrixTransform*>> &VecHighLightTrans)
{
	if (VecWorldCoord.size() < 2)
	{
		return false;
	}

	int totalNum = VecWorldCoord.size();
	std::vector<CL::Vec2d> pol;
	
	for (int i = 0; i < totalNum; i++)
	{
		pol.push_back(CL::Vec2d(VecWorldCoord[i].x(), VecWorldCoord[i].y()));
	}

	pol.push_back(CL::Vec2d(VecWorldCoord[0].x(), VecWorldCoord[0].y()));

	//对于线段筛选并进行高亮

	HighLightLineInRectangle(MapperGroup, pol, VecHighLightTrans);
	return true;
}

