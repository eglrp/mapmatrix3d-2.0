#pragma once
#include <osgGA/TerrainManipulator>
#include <osg/Node>

class CSelfDefTerrainManipulator : public osgGA::TerrainManipulator
{
public:
	

protected:
	virtual bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy);

	virtual bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);

};

