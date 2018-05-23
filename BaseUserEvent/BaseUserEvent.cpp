// BaseUserEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "BaseUserEvent.h"


bool CBaseUserEvent::IsKeyPress(const osgGA::GUIEventAdapter &ea)
{
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
	{
		return true;
	}

	return false;
}

bool CBaseUserEvent::IsKeyUp(const osgGA::GUIEventAdapter &ea)
{
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP)
	{
		return true;
	}

	return false;
}

bool CBaseUserEvent::IsLeftClick(const osgGA::GUIEventAdapter &ea)
{
	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
	{
		if(ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)//鼠标左键
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool CBaseUserEvent::IsDoubleClick(const osgGA::GUIEventAdapter &ea)
{
	if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
	{
		return true;
	}

	return false;
}

bool CBaseUserEvent::IsRightClick(const osgGA::GUIEventAdapter &ea)
{
	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
	{
		if (ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)//鼠标双击
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	return false;
}

bool CBaseUserEvent::IsMouseMove(const osgGA::GUIEventAdapter &ea)
{
	if(ea.getEventType() == osgGA::GUIEventAdapter::MOVE)//鼠标移动
	{
		return true;
	}
	else
	{
		return false;
	}
}

CPickResult CBaseUserEvent::PickResult(float x, float y, osgViewer::Viewer *mViewer)
{
	osgUtil::LineSegmentIntersector::Intersections intersections; 
	mViewer->computeIntersections(x, y, intersections);
	int totalNum = intersections.size();
	osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();

	while( hitr != intersections.end() &&
		( strcmp( hitr->drawable->className(), "Widget")==0 || 
		strcmp( hitr->drawable->className(), "EmbeddedWindow")==0 || 
		strcmp( hitr->drawable->className(), "Corner")==0 || 
		strcmp( hitr->drawable->className(), "Border")==0 )  )
	{
		totalNum -= 1;
		++hitr;
	}

	CPickResult pickResult;

	if(totalNum > 0)
	{
		osg::ref_ptr<osg::Node> parent = ((osg::Node*)(hitr->nodePath.back()))->getParent(0);

		pickResult.localPt =  hitr->localIntersectionPoint;
		pickResult.nodePath = hitr ->nodePath;
		pickResult.worldPt = hitr->getWorldIntersectPoint();
		pickResult.worldNm = hitr->getWorldIntersectNormal();
		pickResult.modelIndex = -1;
		pickResult.parent = parent;
		pickResult.matrix = computeLocalToWorld(hitr ->nodePath);

		for (int i = 0; i < pickResult.nodePath.size(); i++)
		{
			osg::ref_ptr<osg::Node> node = dynamic_cast<osg::Node*> (pickResult.nodePath[i]);
			std::string name = node->getName();
			int pos = name.find("model");

			if (pos != -1)
			{
				int posL =name.find("l");
				std::string subStr = name.substr(posL + 1, name.length());
				int count = atoi(subStr.c_str());
				pickResult.modelIndex = count - 1;
				break;
			}
		}
	}
	else
	{
		osg::Matrix matrix;
		osg::NodePath np;
		pickResult.localPt = osg::Vec3d(0, 0, 0);
		pickResult.nodePath = np;
		pickResult.worldPt = osg::Vec3d(0, 0, 0);
		pickResult.modelIndex = -1;
		pickResult.matrix = matrix;
	}

	return pickResult;
}

void CBaseUserEvent::ClearGroupResult(osg::ref_ptr<osg::Group> group)
{
	while(group->getNumChildren()>0)
	{
		osg::ref_ptr<osg::Group> childGroup = group->getChild(0)->asGroup();
		//如果是Group，先删除其子节点，再删除自身
		if(childGroup)
		{
			while( childGroup->getNumChildren() >0)
				childGroup->removeChildren(0,1);
		}

		group->removeChildren(0,1);
	}
}


double CBaseUserEvent::CalLength(std::vector<osg::Vec3d> coord)
{
	if (coord.size() < 2)
	{
		return 0;
	}

	double Length = 0;

	for (int i = 0; i < coord.size() - 1; i++)
	{
		Length += CalculateDist(coord[i], coord[i + 1]);
	}

	return Length;
};

double CBaseUserEvent::CalculateDist(osg::Vec3d pt1, osg::Vec3d pt2)
{
	double distance = sqrt((pt1.x() - pt2.x()) * (pt1.x() - pt2.x()) + (pt1.y() - pt2.y()) * (pt1.y() - pt2.y()) + (pt1.z() - pt2.z()) * (pt1.z() - pt2.z()));
	return distance;
}

double CBaseUserEvent::CalculateArea(std::vector<osg::Vec3d> coord)
{
	if(coord.size() < 3)
	{
		return 0;
	}

	double area = 0.0;
	//每后两个点与第一个点形成的三角形面积之和
	for(unsigned int i = 2; i < coord.size(); i++)
	{
		float a = CalculateDist(coord[0], coord[i-1]);      //第0个点到第i-1个点的距离
		float b = CalculateDist(coord[i-1], coord[i]);		//第i-1个点到第i个点的距离
		float c = CalculateDist(coord[i], coord[0]);		//第i个点到第0个点的距离
		area += sqrt( ( a + b + c ) * ( a + b - c ) * ( a + c - b ) * ( b + c - a ) )/4;
	}

	return area;
}

 bool CBaseUserEvent::isActivate(std::vector<OperatingState> vecOpState, OperatingState ops)
 {
	 for(int i = 0; i < vecOpState.size(); i++)
	 {
		 if (ops == vecOpState[i])
		 {
			 return true;
		 }
	 }

	 return false;
 }