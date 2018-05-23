// osgHollowOutSuspendEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "qcomm.h"
#include "InterfaceOsgClip.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceOsgFinder.h"
#include "InterfaceOsgEditor.h"
#include <osg/ComputeBoundsVisitor>
#include "osgHollowOutSuspendEvent.h"
#include "InterfaceMFCExcute.h"
#include "ThreadSafeUpdateCallback.h"
#include "InterfaceCOsgClipByScope.h"

void COsgHollowOutSuspendEvent::HollowOutSuspend(std::string inFilePath)
{
	if (vecScope.empty())
	{
		InterfaceMFCExcuteFactory IExcuteFactory;
		InterfaceMFCExcute* IExcute = IExcuteFactory.create();
		IExcute->PopUpMessage("请先选择悬空物");
		return;
	}

	osg::ref_ptr<osg::Node> sceneNode = mRoot->getChild(0)->asGroup()->getChild(0);

	_logop("开始进行悬空物裁切");
	iSigEventCallBack->EmitSigShowOpsHint("正在裁切悬空物...");
	StartHollowOutSuspendedByScope(sceneNode, inFilePath, false, iSigEventCallBack);
}

bool COsgHollowOutSuspendEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && GetKeyState(VK_CONTROL) < 0)
		{
			CtrlReleaseOperate(mRoot, mEditScopeGroup, vecScope);
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && GetKeyState(VK_CONTROL) < 0)
		{
			CtrlDragOperate(mViewer, ea, aa);
		}
		else if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			bool isSuccess = CtrlLeftClickOperate(ea, aa);

			if (!isSuccess)
			{
				return false;
			}
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				BackSpaceOperate(vecScope, mEditScopeGroup);
				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键点击悬空物自动生成范围，可多次点击生成范围，然后进行裁切，按ESC退出事件");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				KeyZOperate(vecScope, mEditScopeGroup);
				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键点击悬空物自动生成范围，可多次点击生成范围，然后进行裁切，按ESC退出事件");
			}
		}
	}
	else
	{
		vecScope.clear();
	}
	return false;
}

void COsgHollowOutSuspendEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}
	bool isActivated1 = isActivate(vecOpState, OPS_HOLLOW_OUT_SUSPENDED);
	bool isActivated2 = isActivate(vecOpState, OPS_HOLLOW_OUT_SUSPENDED_UP);
	if (isActivated1 || isActivated2)
	{
		isActivated = true;
	}
	else
		isActivated =false;

	if (isActivated2)
	{
		isUp = true;
	}
	if (isActivated1)
	{
		isUp = false;
	}
	if (isActivated == false && flag == true)																	
	{
		//消息置空
		iSigEventCallBack->EmitSigShowOpsHint("");
		vecScope.clear();
		ClearGroupResult(mEditScopeGroup);
		ClearGroupResult(ScopeGroup);
		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
		if (spChildCallback != NULL)
		{
			int num = mRoot->getNumChildren();
			osg::ref_ptr<osg::Node> childNode;
			for (int i = 0;i<num;i++)
			{
				childNode = mRoot->getChild(i);
				if (childNode->getName() == "ScopeGroup")
				{
					spChildCallback->RemoveChild(mRoot, childNode);
					break;
				}
			}
		}
	}

	if (isActivated && flag == false)																//刚激活
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		IDrawer->AddDraggerToScene(mRoot,ScopeGroup,mEditScopeGroup);
		iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键点击悬空物自动生成范围，可多次点击生成范围，然后进行裁切，按ESC退出事件");
	}
}

bool COsgHollowOutSuspendEvent::StartHollowOutSuspendedByScope(osg::ref_ptr<osg::Node> sceneNode, std::string inFilePath, bool isInside, InterfaceSigEventCallBack* iSigEventCallBack)
{
	//按范围悬空物裁切
	InterfaceCOsgClipByScopeFactory iOsgClipByScopeFactory;
	InterfaceCOsgClipByScope* iOsgClipByScope = iOsgClipByScopeFactory.create(ACCURATE_HOLLOW_OUT_BY_SUSPEND_SCOPE_1);
	bool isSuccess = iOsgClipByScope->hollowOutByScope(sceneNode, inFilePath, vecScope, false, iSigEventCallBack,false);

	if (isSuccess)
	{
		vecScope.clear();
		ClearGroupResult(mEditScopeGroup);
	}

	return isSuccess;
}

bool COsgHollowOutSuspendEvent::CtrlLeftClickOperate(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();

	CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
	osg::Vec3d worldPt = pickResult.worldPt;
	osg::Vec3d localPt = pickResult.localPt;
	osg::Matrix worldMatrix = pickResult.matrix;
	osg::NodePath np = pickResult.nodePath;

	if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
	{
		return false;
	}
	int _i = 0;

	for (vector<CScope>::iterator ite = vecScope.begin(); ite != vecScope.end(); ++ite)
	{
		if (inScope(worldPt, *ite, scopeInfo))//点在scope上
		{
			atScope = true;
			//点在scope上激活单击事件
			pushEvent(ea, aa);
			//让节点透明
			mEditScopeGroup->removeChild(_i);
			double height = (*ite).maxH - (*ite).minH;
			IDrawer->BaseDrawSolidOperate((*ite).vecCoord, height, osg::Vec4d(1, 0, 1, 0.3), 1.0f, mTempScopeGroup, worldMatrix);
			IDrawer->BaseDrawSolidOperate((*ite).vecCoord, height, osg::Vec4d(1, 0, 1, 0.0), 1.0f, mEditScopeGroup, worldMatrix);
			vecScope.erase(ite);
			return false;
		}
		++_i;
	}

	CScope scope;
	std::string fileName;

	if (!ObtainScope(scope, fileName, worldPt, localPt, worldMatrix, np))
	{
		return false;
	}

	vecScope.push_back(scope);
	double height = scope.maxH - scope.minH;
	IDrawer->BaseDrawSolidOperate(scope.vecCoord, height, osg::Vec4d(1, 0, 1, 0.3), 1.0f, mEditScopeGroup, worldMatrix);
	//根据范围画
	iSigEventCallBack->EmitSigShowOpsHint("按BackSpace回退上一个范围，按Z全部撤销，按ESC退出事件");
}

bool COsgHollowOutSuspendEvent::CtrlReleaseOperate(osg::ref_ptr<osg::Group> root, osg::ref_ptr<osg::Group> group, std::vector<CScope> &VecScope)
{
	if (atScope == true)
	{
		//关闭拖动事件
		osg::ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(root->getUpdateCallback());

		if (spChildCallback != NULL)
		{
			int num = group->getNumChildren();
			osg::ref_ptr<osg::Node> childNode = group->getChild(num - 1);
			spChildCallback->RemoveChild(group, childNode);
		}

		CScope scope = ReDrawScope(endPoint, scopeInfo, mEditScopeGroup, worldMatrix);
		VecScope.push_back(scope);
		atScope = false;
		_activeDragger = 0;
	}

	return true;
}

bool COsgHollowOutSuspendEvent::CtrlDragOperate(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (_activeDragger)
	{
		_pointer._hitIter = _pointer._hitList.begin();
		_pointer.setCamera(viewer->getCamera());
		_pointer.setMousePosition(ea.getX(), ea.getY());
		_activeDragger->handle(_pointer, ea, aa);
		endPoint = _activeDragger->getPoint();
		ReDrawScope(endPoint, scopeInfo, mTempScopeGroup, worldMatrix);
		return false;
	}
}

bool COsgHollowOutSuspendEvent::BackSpaceOperate(std::vector<CScope> &vecScope, osg::ref_ptr<osg::Group> group)
{
	vecScope.pop_back();
	int childNum = group->getNumChildren();
	group->removeChild(childNum - 1);
	return true;
}

bool COsgHollowOutSuspendEvent::KeyZOperate(std::vector<CScope> &vecScope, osg::ref_ptr<osg::Group> group)
{
	vecScope.clear();
	ClearGroupResult(group);
	return true;
}

bool COsgHollowOutSuspendEvent::ObtainScope(CScope &scope, std::string &fileName, osg::Vec3d worldPt, osg::Vec3d localPt, osg::Matrix worldMatrix, osg::NodePath np)
{
	if (np.size() == 0)
	{
		return false;
	}

	for(int i = 0; i < np.size(); i++)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> (np[i]);

		if (lod)
		{
			osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
			InterfaceOsgFinderFactory IFinderFactory;
			InterfaceOsgFinder* IFinder = IFinderFactory.create();
			IFinder->ExtractVerts(lod, extracted_verts);

			std::vector <osg::Vec3>::iterator iter = extracted_verts.get()->begin();
			std::vector<osg::Vec3d> coord;

			if (extracted_verts->size() == 0)
			{
				return false;
			}

			for (int i = 0; i < extracted_verts->size(); i++)
			{
				double x = iter->x();
				double y = iter->y();
				double z = iter->z();
				osg::Vec3d pt(x, y, z);
				pt = pt * worldMatrix;
				coord.push_back(pt);
				iter++;
			}

			std::vector<osg::Vec3d> vecMinMax;
			findMinMax(coord, vecMinMax);
			if (!isUp)
			{
				osg::Vec3d minXYZ(vecMinMax[0].x(), vecMinMax[0].y(), vecMinMax[0].z());
				osg::Vec3d maxXYZ(vecMinMax[1].x(), vecMinMax[1].y(), vecMinMax[1].z());

				std::vector<osg::Vec3d> vecCoord;
				double height = maxXYZ.z() - minXYZ.z();
				vecCoord.push_back(minXYZ);
				vecCoord.push_back(osg::Vec3d(minXYZ.x(), maxXYZ.y(), minXYZ.z()));
				vecCoord.push_back(osg::Vec3d(maxXYZ.x(), maxXYZ.y(), minXYZ.z()));
				vecCoord.push_back(osg::Vec3d(maxXYZ.x(), minXYZ.y(), minXYZ.z()));
				scope.vecCoord = vecCoord;
				scope.minH = minXYZ.z();
				scope.maxH = worldPt.z();
			}
			else
			{
				osg::Vec3d minXYZ(vecMinMax[0].x(), vecMinMax[0].y(), worldPt.z());
				osg::Vec3d maxXYZ(vecMinMax[1].x(), vecMinMax[1].y(), vecMinMax[1].z());

				std::vector<osg::Vec3d> vecCoord;
				double height = maxXYZ.z() - minXYZ.z();
				vecCoord.push_back(minXYZ);
				vecCoord.push_back(osg::Vec3d(minXYZ.x(), maxXYZ.y(), worldPt.z()));
				vecCoord.push_back(osg::Vec3d(maxXYZ.x(), maxXYZ.y(), worldPt.z()));
				vecCoord.push_back(osg::Vec3d(maxXYZ.x(), minXYZ.y(), worldPt.z()));
				scope.vecCoord = vecCoord;
				scope.minH = worldPt.z();
				scope.maxH = maxXYZ.z();
			}
			fileName = lod->getFileName(1);

			break;
		}
		else if (i == np.size() - 1)
		{
			return false;
		}

	}

	return true;
}

bool COsgHollowOutSuspendEvent::findMinMax(std::vector<osg::Vec3d> vecMinMax, std::vector<osg::Vec3d> &coord)
{
	int size = vecMinMax.size();

	if (size <= 0)
	{
		return false;
	}

	double xMin = vecMinMax[0].x();
	double yMin = vecMinMax[0].y();
	double zMin = vecMinMax[0].z();
	double xMax = vecMinMax[0].x();
	double yMax = vecMinMax[0].y();
	double zMax = vecMinMax[0].z();

	for (int i = 0; i < vecMinMax.size(); i++)
	{
		if (xMin > vecMinMax[i].x())
		{
			xMin =  vecMinMax[i].x();
		}

		if (yMin >  vecMinMax[i].y())
		{
			yMin =  vecMinMax[i].y();
		}

		if (zMin >  vecMinMax[i].z())
		{
			zMin =  vecMinMax[i].z();
		}

		if (xMax <  vecMinMax[i].x())
		{
			xMax =  vecMinMax[i].x();
		}

		if (yMax <  vecMinMax[i].y())
		{
			yMax =  vecMinMax[i].y();
		}

		if (zMax < vecMinMax[i].z())
		{
			zMax = vecMinMax[i].z();
		}
	}

	coord.push_back(osg::Vec3d(xMin, yMin, zMin));
	coord.push_back(osg::Vec3d(xMax, yMax, zMax));
	return TRUE;
}

bool COsgHollowOutSuspendEvent::inScope(osg::Vec3d& pt,CScope& scope,SCOPEINFO& scopeInfo)
{
	std::vector<osg::Vec3d> coord;
	coord = scope.vecCoord;
	osg::Vec3d p(0,0,0);

	if (pt.z() < scope.maxH+0.01 && pt.z() > scope.minH-0.01 )
	{
		double mxx = coord[0].x();double mnx = coord[0].x();
		double mxy = coord[0].y();double mny = coord[0].y();

		for(int i = 1;i < coord.size();i++)
		{
			if (coord[i].x()>mxx)
			{
				mxx = coord[i].x();
			}
			else if (coord[i].x()<mnx)
			{
				mnx = coord[i].x();
			}
			if (coord[i].y()>mxy)
			{
				mxy = coord[i].y();
			}
			else if (coord[i].y()<mny)
			{
				mny = coord[i].y();
			}
		}
		
		if (pt.x() < mxx+0.01 && pt.x() > mnx-0.01 && pt.y() > mny-0.01 && pt.y() < mxy+0.01)
		{
			p = pt;

			double Dis = 99999,dis;
			scopeInfo.pointIndex = 0;
			for (int i = 0;i<coord.size();i++)
			{
				dis = distance2(p,coord[i]);
				if (dis<Dis)
				{
					Dis = dis;
					pt = coord[i];
					scopeInfo.pointIndex = i;
				}
			}
			int j;
			switch(scopeInfo.pointIndex)
			{
			case 0:j = 2;break;
			case 1:j = 3;break;
			case 2:j = 0;break;
			case 3:j = 1;break;
			default:return false;
			}
			scopeInfo.pt = scope.vecCoord[j];//保存所选点的对角点
			if (scope.maxH-p.z() > p.z() - scope.minH)
			{
				pt.z() = scope.minH;
				scopeInfo.point_up = false;
				scopeInfo.pt.z() = scope.maxH;
			} 
			else
			{
				pt.z() = scope.maxH;
				scopeInfo.point_up = true;
				scopeInfo.pt.z() = scope.minH;
			}
			scopeInfo.scope = scope;
			return true;
		}
	}
	return false;
}

void COsgHollowOutSuspendEvent::pushEvent(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;
	_pointer.reset();
	if (mViewer->computeIntersections(ea.getX(),ea.getY(),intersections))
	{
		_pointer.setCamera(mViewer->getCamera());
		_pointer.setMousePosition(ea.getX(), ea.getY());
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin(); hitr != intersections.end(); ++hitr)
		{
			_pointer.addIntersection(hitr->nodePath, hitr->getWorldIntersectPoint());
		}
		for (osg::NodePath::iterator itr = _pointer._hitList.front().first.begin(); itr != _pointer._hitList.front().first.end(); ++itr)
		{
			CDraggerPoint* dragger = dynamic_cast<CDraggerPoint*>(*itr);
			if (dragger)
			{
				dragger->handle(_pointer, ea, aa);
				_activeDragger = dragger;
				endPoint = _activeDragger->getPoint();
			}                    
		}
	}
}
//重画scope
CScope COsgHollowOutSuspendEvent::ReDrawScope(osg::Vec3d &pt,SCOPEINFO &scopeInfo,osg::Group* EditScopeGroup,osg::Matrix worldMatrix)
{
	CScope scope;
	if (scopeInfo.point_up)//选择的上方点   对角下方点固定不动
	{
		if (pt.z()<=scopeInfo.scope.minH)
		{
			pt.z() = scopeInfo.scope.minH + 0.1;
		}
		osg::Vec3d p0 = scopeInfo.pt;
		osg::Vec3d p2 = pt;p2.z() = p0.z();
		osg::Vec3d p1 = osg::Vec3d(p0.x(),p2.y(),p0.z());
		osg::Vec3d p3 = osg::Vec3d(p2.x(),p0.y(),p0.z());

		vector<osg::Vec3d> coord;
		coord.push_back(p0);
		coord.push_back(p1);
		coord.push_back(p2);
		coord.push_back(p3);

		double H = pt.z() - scopeInfo.pt.z();
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		ClearGroupResult(mTempScopeGroup);
		IDrawer->BaseDrawSolidOperate(coord,H,osg::Vec4(1,0,1,0.3),1.0f,EditScopeGroup,worldMatrix);

		scope.vecCoord = coord;
		scope.minH = scopeInfo.pt.z();
		scope.maxH = pt.z();
		scope.matrix = scopeInfo.scope.matrix;
		return scope;
	}
	else//选择的下方点  
	{
		if (pt.z()>=scopeInfo.scope.maxH)
		{
			pt.z() = scopeInfo.scope.maxH - 0.1;
		}
		osg::Vec3d p0 = scopeInfo.pt;p0.z() = pt.z();
		osg::Vec3d p2 = pt;
		osg::Vec3d p1 = osg::Vec3d(p0.x(),p2.y(),pt.z());
		osg::Vec3d p3 = osg::Vec3d(p2.x(),p0.y(),pt.z());

		vector<osg::Vec3d> coord;
		coord.push_back(p0);
		coord.push_back(p1);
		coord.push_back(p2);
		coord.push_back(p3);

		double H = scopeInfo.pt.z() - pt.z()  ;
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		ClearGroupResult(mTempScopeGroup);
		IDrawer->BaseDrawSolidOperate(coord,H,osg::Vec4(1,0,1,0.3),1.0f,EditScopeGroup,worldMatrix);
		scope.vecCoord = coord;
		scope.minH = pt.z();
		scope.maxH = scopeInfo.pt.z();
		scope.matrix = scopeInfo.scope.matrix;
		return scope;
	}

}
