// osgExtractOutlineEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "InterfaceBaseDrawer.h"
#include "osgExtractOutlineEvent.h"
#include "outline_1.h"
#include "InterfaceMFCExcute.h"
#include "toShpAndDxf.h"

void COsgExtractOutLineEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_EXTRACT_OUTLINE);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(mapperGroup);
		vecWorldCoord.clear();
		vecVec.clear();
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键选点，自动提取轮廓线，按ESC退出事件");
	}
}


bool COsgExtractOutLineEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Vec3d normal = pickResult.worldNm;
			osg::Matrix worldMatrix = pickResult.matrix;

			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}

			vecWorldCoord.push_back(worldPt);

			//提取轮廓线
			std::vector<osg::Vec3d> vecLine;
			outline::ComputeOutline(worldPt, normal, mRoot, vecLine);
			vecVec.push_back(vecLine);
			vecVec1 = vecVec;
			//画轮廓线
			if (vecLine.size() > 0)
			{
				InterfaceBaseDrawerFactory IDrawerFactory;
				InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
				osg::Vec4d color(1.0, 0, 1, 1.0);
				double size = 3.5f;
				IDrawer->BaseDrawLineLoopOperate(vecLine, color, size, mapperGroup, worldMatrix);

				iSigEventCallBack->EmitSigShowOpsHint("按BackSpace删除上一个轮廓线，按Z撤销全部，按ESC退出事件");
			}
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{

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
				int childNum = mapperGroup->getNumChildren();
				if(childNum>0)
				{
					mapperGroup->removeChild(childNum - 1);
					if (!vecVec.empty())
					{
						vecVec.pop_back();
					}

					iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键选点，自动提取轮廓线，按ESC退出事件");
				}
				
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldCoord.clear();
				vecVec.clear();
				ClearGroupResult(mapperGroup);

				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键选点，自动提取轮廓线，按ESC退出事件");
			}
		}
	}
	else
	{
		vecWorldCoord.clear();
	}

	return false;
}

std::vector<std::vector<osg::Vec3d> >& COsgExtractOutLineEvent::getVec()
{
	return vecVec;
}

void COsgExtractOutLineEvent::clearVec()
{
	vecVec.clear();
}

void COsgExtractOutLineEvent::pushBack(std::vector<std::vector<osg::Vec3d> >& vec)
{
	//vecVec1.assign(vecVec.begin(),vecVec.end()) ;
	vecVec.insert(vecVec.end(),vec.begin(),vec.end());
}

bool COsgExtractOutLineEvent::exportOutline()
{
	if(!vecVec.empty())
	{
		//创建文件夹并设置输出路径
		std::string extSelect = "DXF文件 (*.dxf)|*.dxf|SHP文件 (*.shp)|*.shp||";
		std::string outputFileName;
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		bool isSuccess = IExcute->OpenDialogEx(outputFileName, extSelect, false,".dxf",".shp");
		if (!isSuccess)
		{
			return TRUE;
		}

		if (int(outputFileName.find(".shp")) != -1)//如果输出shp
		{
			toShpAndDxf::convertToShp(vecVec1,outputFileName,"outline");
			IExcute->PopUpMessage("SHP文件输出完成！");
			return TRUE;
		}
		else
		{
			//toShpAndDxf::convertToDxf(vecVec,outputFileName,"outline");
			IExcute->PopUpMessage("DXF文件输出完成！");
			return TRUE;
		}
	}
	else
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("未发现轮廓线!");
		return false;
	}
}

