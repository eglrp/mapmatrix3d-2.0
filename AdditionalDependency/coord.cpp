#include "stdafx.h"
#include "coord.hpp"

istream& operator>>(istream& os, CoordDouble2D& c)
{
	os>>c.x>>c.y;
	return os;
}

ostream& operator<<(ostream& os, CoordDouble2D& c)
{
	os<<"("<<c.x<<","<<c.y<<")"<<endl;
	return os;
}

istream& operator>>(istream& os, CoordFloat3D& c)
{
	os>>c.x>>c.y>>c.z;
	return os;
}

ostream& operator<<(ostream& os, CoordFloat3D& c)
{
	os<<"("<<c.x<<","<<c.y<<","<<c.z<<")"<<endl;
	return os;
}

istream& operator>>(istream& os, CoordDouble3D& c)
{
	os>>c.x>>c.y>>c.z;
	return os;
}

ostream& operator<<(ostream& os, CoordDouble3D& c)
{
	os<<"("<<c.x<<","<<c.y<<","<<c.z<<")"<<endl;
	return os;
}