#include "StdAfx.h"
#include "SelfDefTerrainManipulator.h"
#include "qcomm.h"

bool CSelfDefTerrainManipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	// pan model.
	double scale = -0.3f * _distance * getThrowScale( eventTimeDelta );

	osg::Matrixd rotation_matrix;
	rotation_matrix.makeRotate(_rotation);

	// compute look vector.
	osg::Vec3d sideVector = getSideVector(rotation_matrix);

	// CoordinateFrame coordinateFrame = getCoordinateFrame(_center);
	// Vec3d localUp = getUpVector(coordinateFrame);
	osg::Vec3d localUp = _previousUp;

	osg::Vec3d forwardVector = localUp ^ sideVector;
	sideVector = forwardVector^localUp;

	forwardVector.normalize();
	sideVector.normalize();

	osg::Vec3d dv = forwardVector * (dy*scale) + sideVector * (dx*scale);

	_center += dv;

	// need to recompute the intersection point along the look vector.

	bool hitFound = false;

	if (_node.valid())
	{
		// now reorientate the coordinate frame to the frame coords.
		osg::CoordinateFrame coordinateFrame = getCoordinateFrame(_center);

		// need to reintersect with the terrain
		double distance = _node->getBound().radius() * 0.25f;

		osg::Vec3d ip1;
		osg::Vec3d ip2;
		bool hit_ip1 = intersect(_center, _center + getUpVector(coordinateFrame) * distance, ip1);
		bool hit_ip2 = intersect(_center, _center - getUpVector(coordinateFrame) * distance, ip2);

		if (hit_ip1)
		{
			if (hit_ip2)
			{
				_center = (_center-ip1).length2() < (_center-ip2).length2() ?ip1 : ip2;
				hitFound = true;
			}
			else
			{
				_center = ip1;
				hitFound = true;
			}
		}
		else if (hit_ip2)
		{
			_center = ip2;
			hitFound = true;
		}

		if (!hitFound)
		{
			// ??
			OSG_INFO<<"TerrainManipulator unable to intersect with terrain."<<std::endl;
		}

		coordinateFrame = getCoordinateFrame(_center);
		osg::Vec3d new_localUp = getUpVector(coordinateFrame);

		osg::Quat pan_rotation;
		pan_rotation.makeRotate(localUp, new_localUp);

		if (!pan_rotation.zeroRotation())
		{
			_rotation = _rotation * pan_rotation;
			_previousUp = new_localUp;
			//OSG_NOTICE<<"Rotating from "<<localUp<<" to "<<new_localUp<<"  angle = "<<acos(localUp*new_localUp/(localUp.length()*new_localUp.length()))<<std::endl;

			//clampOrientation();
		}
		else
		{
			OSG_INFO<<"New up orientation nearly inline - no need to rotate"<<std::endl;
		}
	}

	return true;
}

bool CSelfDefTerrainManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	if (GetKeyState(VK_CONTROL) < 0)
	{
		return false;
	}
	 //rotate camera
	if( getVerticalAxisFixed() )
		rotateWithFixedVertical( dx, dy );
	else
		rotateTrackball( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
		_ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
		getThrowScale( eventTimeDelta ) );
	return true;
}
