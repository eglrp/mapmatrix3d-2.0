#include "stdafx.h"
#include "osgHorizontalizationAccurateEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceOsgExtractWall.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgEditor.h"

void COsgHorizontalizationAccurateEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false;

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_HORIZON_ACCURATE);

	if (isActivated == false && flag == true)													 //不激活
	{
		//清除editGoup
		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
		if (spChildCallback != NULL)
		{
			int num = editGroup->getNumChildren();
			osg::ref_ptr<osg::Node> childNode;
			for (int i = 0; i < num; i++)
			{
				childNode = editGroup->getChild(i);
				spChildCallback->RemoveChild(editGroup, childNode);
			}
		}
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		vecTotalTriangle.clear();
		planePara.clear();
		iSigEventCallBack->EmitSigShowOpsHint("精确置平");
	}
}

bool COsgHorizontalizationAccurateEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceOsgExtractWallFactory IWallFactory;
		InterfaceOsgExtractWall* IWall = IWallFactory.create();
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			osg::Vec3d normal = pickResult.worldNm;
			osg::NodePath np = pickResult.nodePath;
			worldMatrix = pickResult.matrix;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			std::vector<osg::Vec3d> vecTriangle;
			//自动采样
			InterfaceOsgExtractWallFactory IWallFactory;
			InterfaceOsgExtractWall* IWall = IWallFactory.create();
			if (!IWall->DrawTriangle(mRoot, np, worldPt, worldMatrix, vecTriangle, normal, 15, 0.0))
			{
				return false;
			}

			//提取成功，绘制点
			if (vecTriangle.size() > 0)
			{
				vecTotalTriangle.push_back(vecTriangle);
				osg::ref_ptr<osg::Group> outlineGroup = new osg::Group;
				InterfaceBaseDrawerFactory IDrawerFactory;
				InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
				for (int i = 0; i < vecTriangle.size(); ++i)
				{
					IDrawer->BaseDrawPtOperate(vecTriangle[i], osg::Vec4(0,0,1,1), 3.5, outlineGroup, worldMatrix);
				}
				osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
				if (spChildCallback != NULL)
				{
					outlineGroup->setName("points");
					spChildCallback->AddChild(editGroup, outlineGroup);
				}
				iSigEventCallBack->EmitSigShowOpsHint("精确置平：按BackSpace撤销采样点，按Ctrl + 鼠标左键继续采样，按 E 键提取墙面");
			}
		}
		else if (IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				if (editGroup->getNumChildren() > 0)
				{
					editGroup->removeChild(editGroup->getNumChildren() - 1);
					vecTotalTriangle.pop_back();
				}
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				ClearGroupResult(editGroup);
				vecTotalTriangle.clear();
				return false;
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_E)
			{
				if (vecTotalTriangle.empty())
				{
					return false;
				}
				std::vector<osg::Vec3d> vecPt;
				for (size_t i = 0;i<vecTotalTriangle.size();++i)
				{
					for (size_t j = 0;j<vecTotalTriangle[i].size();++j)
					{
						vecPt.push_back(vecTotalTriangle[i][j]);
					}
				}
				vecTotalTriangle.clear();
				if (IWall->fittingWallSpace(vecPt, planePara, osg::Vec3d(0,0,0), worldMatrix))//拟合平面
				{
					osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
					//提取了2个面，计算交线向量
					if (planePara.size() == 2)
					{
						if (spChildCallback != NULL)
						{
							int num = editGroup->getNumChildren();
							osg::ref_ptr<osg::Node> childNode;
							for (int i = 0; i < num; i++)
							{
								childNode = editGroup->getChild(i);
								spChildCallback->RemoveChild(editGroup, childNode);
							}
						}
						calcHorizonVector(planePara, hVector);
						return false;
					}
					InterfaceBaseDrawerFactory IDrawerFactory;
					InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
					//先删除点图形
					
					if (spChildCallback != NULL)
					{
						int num = editGroup->getNumChildren();
						osg::ref_ptr<osg::Node> childNode;
						for (int i = 0; i < num; i++)
						{
							childNode = editGroup->getChild(i);
							if (childNode->getName() == "points")
							{
								spChildCallback->RemoveChild(editGroup, childNode);
							}	
						}
					}
					IDrawer->BaseDrawPolygon(planePara.back().coord, osg::Vec4d(0, 1, 1, 0.4), editGroup, worldMatrix);
					
					char strMsg[299];
					sprintf(strMsg, "精确置平：墙面倾斜角：%f ° 墙面拟合误差：%lf", planePara.back().angle, planePara.back().err);
					iSigEventCallBack->EmitSigShowOpsHint(strMsg);	
				}
			}
		}
	}


	return false;
}

void COsgHorizontalizationAccurateEvent::calcHorizonVector(std::vector<WallInfo>& planePara, osg::Vec3d& hVector)
{
	if (planePara[0].angle == planePara[1].angle && planePara[0].err == planePara[1].err)//同一墙面
	{
		return ;
	}
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	double a1 = planePara[0].pl->A;
	double b1 = planePara[0].pl->B;
	double c1 = planePara[0].pl->C;
	double d1 = planePara[0].pl->D;

	double a2 = planePara[1].pl->A;
	double b2 = planePara[1].pl->B;
	double c2 = planePara[1].pl->C;
	double d2 = planePara[1].pl->D;

	double x1, x2, y1, y2, z1, z2;
	
	double minz1 = planePara[0].coord[0].z();
	double maxz1 = planePara[0].coord[0].z();
	double minz2 = planePara[1].coord[0].z();
	double maxz2 = planePara[1].coord[0].z();
	for (int i = 1; i < planePara[0].coord.size(); ++i)
	{
		if (planePara[0].coord[i].z() < minz1)
		{
			minz1 = planePara[0].coord[i].z();
		}
		else if (planePara[0].coord[i].z() > maxz1)
		{
			maxz1 = planePara[0].coord[i].z();
		}
	}
	for (int i = 1; i < planePara[1].coord.size(); ++i)
	{
		if (planePara[1].coord[i].z() < minz2)
		{
			minz2 = planePara[1].coord[i].z();
		}
		else if (planePara[1].coord[i].z() > maxz2)
		{
			maxz2 = planePara[1].coord[i].z();
		}
	}
	z1 = minz1 > minz2 ? minz1 : minz2;
	x1 = ((-d1 - c1*z1)*b2 - (-d2 - c2*z1)*b1) / (b2*a1 - b1*a2);
	y1 = ((-d1 - c1*z1)*a2 - (-d2 - c2*z1)*a1) / (a2*b1 - a1*b2);
	z2 = maxz1 > maxz2 ? maxz2 : maxz1;
	x2 = ((-d1 - c1*z2)*b2 - (-d2 - c2*z2)*b1) / (b2*a1 - b1*a2);
	y2 = ((-d1 - c1*z2)*a2 - (-d2 - c2*z2)*a1) / (a2*b1 - a1*b2);
	osg::ref_ptr<osg::Group> group = new osg::Group;
	//IDrawer->BaseDrawLineOperate(osg::Vec3d(x1, y1, z1), osg::Vec3d(x2, y2, z2), osg::Vec4(1,0,0,1), 3.5, group, worldMatrix);	
	//editGroup->addChild(group);
	
	char strMsg[99];
	
	sprintf(strMsg, "精确置平：交线向量（%f,%f,%f） ", x2-x1,y2-y1,z2-z1);
	iSigEventCallBack->EmitSigShowOpsHint(strMsg);
	hVector.x() = x2 - x1;
	hVector.y() = y2 - y1;
	hVector.z() = z2 - z1;
	hVector.z() = 0;
	hVector.normalize();
	osg::Vec3 axis = osg::Y_AXIS ^ hVector;
	float angle = atan2(axis.length(), osg::Y_AXIS*hVector);
	axis.normalize();
	osg::Matrix matrix = osg::Matrix::rotate(angle, axis);
	osg::ref_ptr<osg::Group> sceneGroup = mRoot->getChild(0)->asGroup();

	int childNum = sceneGroup->getNumChildren();
	InterfaceOsgEditorFactory IEditorFactory;
	InterfaceOsgEditor* IEditor = IEditorFactory.create();
	
	for (int i = 0; i < childNum; i++)
	{
		osg::ref_ptr<osg::Node> sceneNode = sceneGroup->getChild(i);
		if (sceneNode)
		{
			IEditor->MultiplyTransMatrix(sceneNode, matrix);
		}
	}

	planePara.clear();
	return ;
}