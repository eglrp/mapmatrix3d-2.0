#pragma once;
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgManipulator/Command>
#include <osgManipulator/CommandManager>  
#include <osgManipulator/Translate2DDragger>
#include <osgText/Text>
#include <osg/ShapeDrawable>
#include "qcomm.h"

using namespace osgManipulator;

class CDraggerPoint: public osgManipulator::Translate2DDragger
{
public:
	CDraggerPoint(osg::Group* group)
	{
		mGroup = group;
		
	}
	void setScence()
	{
		addChild(mGroup);
	}
	osg::Vec3d& getPoint()
	{
		return point;
	}
	virtual bool handle(const PointerInfo& pointer, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		// Check if the dragger node is in the nodepath.
		if (!pointer.contains(this)) return false;
		switch (ea.getEventType())
		{
			// Pick start.
		case (osgGA::GUIEventAdapter::PUSH):
			{
				// Get the LocalToWorld matrix for this node and set it for the projector.
				osg::Vec3d localPt = pointer.getLocalIntersectPoint();

				//设置点移动平面,点即鼠标与物体交点(世界坐标系),法线为//视线方向
				osg::Plane* pplnPTPush= new osg::Plane(pointer.getEyeDir(),  pointer.getLocalIntersectPoint()/* _projector->getLocalToWorld()*/);
				_projector->setPlane(*pplnPTPush);
				if (_projector->project(pointer, _startProjectedPoint))
				{
					point = _startProjectedPoint;
					osg::Vec3d pt = point;
				}
				return true; 
			}
			// Pick move.
		case (osgGA::GUIEventAdapter::DRAG):
			{
				osg::Vec3d projectedPoint;
				if (_projector->project(pointer, projectedPoint))
				{
					osg::Matrix matrix = _projector->getLocalToWorld();
					double* db = (double*) matrix.ptr();
					point = projectedPoint * _projector->getLocalToWorld();
				}
				return true; 
			}
			// Pick finish.
		case (osgGA::GUIEventAdapter::RELEASE):
			{
				osg::ref_ptr<TranslateInPlaneCommand> cmd = new TranslateInPlaneCommand(_projector->getPlane());
				cmd->setStage(MotionCommand::FINISH);
				cmd->setReferencePoint(_startProjectedPoint);
				cmd->setLocalToWorldAndWorldToLocal(_projector->getLocalToWorld(),_projector->getWorldToLocal());
				setMaterialColor(_color,*this);
				getOrCreateStateSet()->removeAttribute(_polygonOffset.get());
				aa.requestRedraw();
				return true;
			}
		default:
			return false;
		}
	}

protected:
	osg::Group* mGroup;
	osg::Vec3d point;
};