// osgHandExractWallSpaceEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgHandExtractWallSpaceEvent.h"
#include "InterfaceOsgExtractWall.h"
#include "ThreadSafeUpdateCallback.h"

extern std::vector<WallInfo> gVecWall;
extern std::vector<CPointWall> gVecPW;
void COsgHandExtractWallSpaceEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}
	isActivated = isActivate(vecOpState, OPS_SELECT_WALLSPACE_HAND);

	if (isActivated == false && flag == true)
	{
		ClearGroupResult(tempGroup);
		vecCoord.clear();
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		(vecTotalTriangle).clear();
		vecCoord.clear();
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		IWall->addDraggerToScene(mRoot, mSlotGroup, mEditGroup);
		iSigEventCallBack->EmitSigShowOpsHint("手动采样：按Ctrl + 鼠标左键采样");
	}
}

bool COsgHandExtractWallSpaceEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
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
			normal = pickResult.worldNm;
			worldMatrix = pickResult.matrix;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			//画点
			IDrawer->BaseDrawPtOperate(worldPt, osg::Vec4d(0, 0, 1, 1), 3.5, tempGroup, worldMatrix);
			vecCoord.push_back(worldPt);
			iSigEventCallBack->EmitSigShowOpsHint("手动采样：按BackSpace撤销前一次采样，按Z键撤销所有采样，按Ctrl + 鼠标左键继续采样，单击 <提取墙面> 或 Alt + E 键提取墙面");
			return false;
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)//删除上一个点图形及数据
			{
				size_t num = tempGroup->getNumChildren();
				if (vecCoord.size() > 0)
				{
					vecCoord.pop_back();
					tempGroup->removeChild(num - 1);
					iSigEventCallBack->EmitSigShowOpsHint("提示：删除一个手动采样点");
					return false;
				}
				if (mEditGroup->getNumChildren() > 0)
				{
					mEditGroup->removeChild(mEditGroup->getNumChildren() - 1);
					gVecWall.pop_back();
					iSigEventCallBack->EmitSigShowOpsHint("提示：删除一个房屋面");
					
					if (gVecPW.size() > 0)
					{
						int index = mEditGroup->getNumChildren();//待删除墙面的序号
																 //删除和已删除面相关的特征点
						for (std::vector<CPointWall>::iterator ite = gVecPW.begin(); ite != gVecPW.end();)
						{
							if (std::find(((*ite).vecWallIndex).begin(), ((*ite).vecWallIndex).end(), index) != ((*ite).vecWallIndex).end())
							{
								ite = gVecPW.erase(ite);
							}
							else
							{
								ite++;
							}
						}
					}
				}
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_E)//拟合墙面
			{
				if (vecCoord.size()<3)
				{
					iSigEventCallBack->EmitSigShowOpsHint("采样点不足3个，无法拟合平面");
					return false;
				}
				ClearGroupResult(tempGroup);
				IWall->fittingWallSpace(vecCoord, gVecWall, normal, worldMatrix);
				vecCoord.clear();
				char msg[299];
				IDrawer->BaseDrawPolygon(gVecWall.back().coord, osg::Vec4d(0, 1, 1, 0.4), mEditGroup, worldMatrix);
				sprintf(msg, "手动提取墙面：墙面倾斜角：%f ° 墙面拟合误差：%lf", gVecWall.back().angle, gVecWall.back().err);
				iSigEventCallBack->EmitSigShowOpsHint(msg);
				if (gVecWall.size() >= 3)
				{
					std::vector<WallInfo> vecwall;
					vecwall.assign(gVecWall.begin(), gVecWall.end() - 1);
					WallInfo wall = gVecWall.back();

					for (int i = 0; i < vecwall.size() - 1; ++i)
					{
						for (int j = i + 1; j < vecwall.size(); ++j)
						{
							osg::Vec3d ptInsect;
							if (IWall->GetInsectof3Plane(vecwall[i], vecwall[j], wall, ptInsect))
							{
								CPointWall pw;
								pw.pt = ptInsect;
								pw.vecWallIndex.push_back(i);
								pw.vecWallIndex.push_back(j);
								pw.vecWallIndex.push_back(gVecWall.size() - 1);
								gVecPW.push_back(pw);
							}
						}
					}
				}
				
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)//删除所有点图形
			{
				ClearGroupResult(tempGroup);
				ClearGroupResult(mEditGroup);
				gVecWall.clear();
				gVecPW.clear();
				vecCoord.clear();
				iSigEventCallBack->EmitSigShowOpsHint("手动采样：按Ctrl + 鼠标左键采样");
				return false;
			}
		}
	}
	return false;
}

