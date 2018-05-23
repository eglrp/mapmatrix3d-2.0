#include "StdAfx.h"
#include "SelfDefTrackBallManipulator.h"

bool CSelfDefTrackBallManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	//rotate camera
	//if( getVerticalAxisFixed() )
	//	rotateWithFixedVertical( dx, dy );
	//else
	rotateTrackball( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
		_ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
		getThrowScale( eventTimeDelta ) );
	return true;
}