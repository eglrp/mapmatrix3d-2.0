#pragma once
#include "selfdefterrainmanipulator.h"
class CSelfDefTrackBallManipulator : public CSelfDefTerrainManipulator
{
public:
	bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);

};

