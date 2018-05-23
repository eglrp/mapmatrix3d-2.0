#ifndef COORD_HPP
#define COORD_HPP

#include <iostream>
#include <math.h>

using namespace std;

struct CoordInt
{
public:
	int x, y;

	CoordInt() {}
	CoordInt(int a, int b) { x = a; y = b; }
	//friend istream& operator>>(istream& os, CoordInt& c);
	//friend ostream& operator<<(ostream& os, CoordInt& c);

	CoordInt operator- ()        { return CoordInt(-x, -y); }
	CoordInt operator+ (CoordInt a) { return CoordInt(x + a.x, y + a.y); }
	CoordInt operator- (CoordInt a) { return CoordInt(x - a.x, y - a.y); }
	bool  operator< (CoordInt a) { return (x <  a.x) && (y <  a.y); }
	bool  operator<=(CoordInt a) { return (x <= a.x) && (y <= a.y); }
	bool  operator> (CoordInt a) { return (x >  a.x) && (y >  a.y); }
	bool  operator>=(CoordInt a) { return (x >= a.x) && (y >= a.y); }
	bool  operator==(CoordInt a) { return (x == a.x) && (y == a.y); }
	bool  operator!=(CoordInt a) { return (x != a.x) || (y != a.y); }
	void operator-=(CoordInt a) { x-=a.x; y-=a.y;}
	CoordInt& operator=(const CoordInt& a) {
		if (this == &a)
		{
			return *this;
		}
		else
		{
			x = a.x;
			y = a.y;
			return *this;
		}  
	}

};

//istream& operator>>(istream& os, Coord& c)
//{
//	os>>c.x>>c.y;
//	return os;
//}

//ostream& operator<<(ostream& os, Coord& c)
//{
//	os<<"("<<c.x<<","<<c.y<<")"<<endl;
//	return os;
//}
//#define IMREF(im, p, c) (im[3*((p.y)*_w + (p.x)) + (c)]);


struct CoordLong
{
public:
	long x, y;

	CoordLong() {}
	CoordLong(long a, long b) { x = a; y = b; }
	//friend istream& operator>>(istream& os, CoordLong& c);
	//friend ostream& operator<<(ostream& os, CoordLong& c);

	CoordLong operator- ()        { return CoordLong(-x, -y); }
	CoordLong operator+ (CoordLong a) { return CoordLong(x + a.x, y + a.y); }
	CoordLong operator- (CoordLong a) { return CoordLong(x - a.x, y - a.y); }
	bool  operator< (CoordLong a) { return (x <  a.x) && (y <  a.y); }
	bool  operator<=(CoordLong a) { return (x <= a.x) && (y <= a.y); }
	bool  operator> (CoordLong a) { return (x >  a.x) && (y >  a.y); }
	bool  operator>=(CoordLong a) { return (x >= a.x) && (y >= a.y); }
	bool  operator==(CoordLong a) { return (x == a.x) && (y == a.y); }
	bool  operator!=(CoordLong a) { return (x != a.x) || (y != a.y); }
	void operator-=(CoordLong a) { x-=a.x; y-=a.y;}
	CoordLong& operator=(const CoordLong& a) {
		if (this == &a)
		{
			return *this;
		}
		else
		{
			x = a.x;
			y = a.y;
			return *this;
		}  
	}

};

struct CoordLong3D
{
public:
	int x, y, z;

	CoordLong3D() {}
	CoordLong3D(int a, int b, int c) { x = a; y = b; z = c; }
	//friend istream& operator>>(istream& os, CoordLong3D& c);
	//friend ostream& operator<<(ostream& os, CoordLong3D& c);

	CoordLong3D operator- ()        { return CoordLong3D(-x, -y, -z); }
	CoordLong3D operator+ (CoordLong3D a) { return CoordLong3D(x + a.x, y + a.y, z + a.z); }
	CoordLong3D operator- (CoordLong3D a) { return CoordLong3D(x - a.x, y - a.y, z - a.z); }
	bool  operator< (CoordLong3D a) { return (x <  a.x) && (y <  a.y) && (z < a.z); }
	bool  operator<=(CoordLong3D a) { return (x <= a.x) && (y <= a.y) && (z <= a.z); }
	bool  operator> (CoordLong3D a) { return (x >  a.x) && (y >  a.y) && (z > a.z); }
	bool  operator>=(CoordLong3D a) { return (x >= a.x) && (y >= a.y) && (z >= a.z); }
	bool  operator==(CoordLong3D a) { return (x == a.x) && (y == a.y) && (z == a.z); }
	bool  operator!=(CoordLong3D a) { return (x != a.x) || (y != a.y) || (z != a.z); }
	void operator-=(CoordLong3D a) { x-=a.x; y-=a.y; z-=a.z; }
	CoordLong3D& operator=(const CoordLong3D& a) {
		if (this == &a)
		{
			return *this;
		}
		else
		{
			x = a.x;
			y = a.y;
			z = a.z;
			return *this;
		}  
	}

};

struct CoordFloat
{
public:
	float x, y;

	CoordFloat() {}
	CoordFloat(float a, float b) { x = a; y = b; }

	CoordFloat operator- ()        { return CoordFloat(-x, -y); }
	CoordFloat operator+ (CoordFloat a) { return CoordFloat(x + a.x, y + a.y); }
	CoordFloat operator- (CoordFloat a) { return CoordFloat(x - a.x, y - a.y); }
	bool  operator< (CoordFloat a) { return (x <  a.x) && (y <  a.y); }
	bool  operator<=(CoordFloat a) { return (x <= a.x) && (y <= a.y); }
	bool  operator> (CoordFloat a) { return (x >  a.x) && (y >  a.y); }
	bool  operator>=(CoordFloat a) { return (x >= a.x) && (y >= a.y); }
	bool  operator==(CoordFloat a) { return (x == a.x) && (y == a.y); }
	bool  operator!=(CoordFloat a) { return (x != a.x) || (y != a.y); }
	void operator-=(CoordFloat a) { x-=a.x; y-=a.y;}
	CoordFloat& operator=(const CoordFloat& a) {
		if (this == &a)
		{
			return *this;
		}
		else
		{
			x = a.x;
			y = a.y;
			return *this;
		}  
	}

};

struct CoordDouble
{
public:
	double x, y;

	CoordDouble() {}
	CoordDouble(double a, double b) { x = a; y = b; }

	CoordDouble operator- ()        { return CoordDouble(-x, -y); }
	CoordDouble operator+ (CoordDouble a) { return CoordDouble(x + a.x, y + a.y); }
	CoordDouble operator- (CoordDouble a) { return CoordDouble(x - a.x, y - a.y); }
	bool  operator< (CoordDouble a) { return (x <  a.x) && (y <  a.y); }
	bool  operator<=(CoordDouble a) { return (x <= a.x) && (y <= a.y); }
	bool  operator> (CoordDouble a) { return (x >  a.x) && (y >  a.y); }
	bool  operator>=(CoordDouble a) { return (x >= a.x) && (y >= a.y); }
	bool  operator==(CoordDouble a) { return (x == a.x) && (y == a.y); }
	bool  operator!=(CoordDouble a) { return (x != a.x) || (y != a.y); }
	void operator-=(CoordDouble a) { x-=a.x; y-=a.y;}
	CoordDouble& operator=(const CoordDouble& a) {
		if (this == &a)
		{
			return *this;
		}
		else
		{
			x = a.x;
			y = a.y;
			return *this;
		}  
	}

};

struct CoordDouble2D
{
public:
	double x, y;

	CoordDouble2D() {}
	CoordDouble2D(double a, double b) { x = a; y = b; }

	friend istream& operator>>(istream& os, CoordDouble2D& c);
	friend ostream& operator<<(ostream& os, CoordDouble2D& c);

	CoordDouble2D operator- ()        { return CoordDouble2D(-x, -y); }
	CoordDouble2D operator+ (CoordDouble2D a) { return CoordDouble2D(x + a.x, y + a.y); }
	CoordDouble2D operator- (CoordDouble2D a) { return CoordDouble2D(x - a.x, y - a.y); }
	bool  operator< (CoordDouble2D a) { return (x <  a.x) && (y <  a.y); }
	bool  operator<=(CoordDouble2D a) { return (x <= a.x) && (y <= a.y); }
	bool  operator> (CoordDouble2D a) { return (x >  a.x) && (y >  a.y); }
	bool  operator>=(CoordDouble2D a) { return (x >= a.x) && (y >= a.y); }
	bool  operator==(CoordDouble2D a) { return (x == a.x) && (y == a.y); }
	bool  operator!=(CoordDouble2D a) { return (x != a.x) || (y != a.y); }
	void operator-=(CoordDouble2D a) { x-=a.x; y-=a.y;}
	CoordDouble2D& operator=(const CoordDouble2D& a) {
		if (this == &a)
		{
			return *this;
		}
		else
		{
			x = a.x;
			y = a.y;
			return *this;
		}  
	}

};

extern istream& operator>>(istream& os, CoordDouble2D& c);
extern ostream& operator<<(ostream& os, CoordDouble2D& c);

struct CoordFloat3D
{
public:
	float x, y, z;

	CoordFloat3D() {}
	CoordFloat3D(float a, float b, float c) { x = a; y = b; z = c; }

	friend istream& operator>>(istream& os, CoordFloat3D& c);
	friend ostream& operator<<(ostream& os, CoordFloat3D& c);

	CoordFloat3D operator- ()        { return CoordFloat3D(-x, -y, -z); }
	CoordFloat3D operator+ (CoordFloat3D a) { return CoordFloat3D(x + a.x, y + a.y, z + a.z); }
	CoordFloat3D operator- (CoordFloat3D a) { return CoordFloat3D(x - a.x, y - a.y, z - a.z); }
	bool  operator< (CoordFloat3D a) { return (x <  a.x) && (y <  a.y) && (z < a.z); }
	bool  operator<=(CoordFloat3D a) { return (x <= a.x) && (y <= a.y) && (z <= a.z); }
	bool  operator> (CoordFloat3D a) { return (x >  a.x) && (y >  a.y) && (z > a.z); }
	bool  operator>=(CoordFloat3D a) { return (x >= a.x) && (y >= a.y) && (z >= a.z); }
	bool  operator==(CoordFloat3D a) { return (x == a.x) && (y == a.y) && (z == a.z); }
	bool  operator!=(CoordFloat3D a) { return (x != a.x) || (y != a.y) || (z != a.z); }
	void operator-=(CoordFloat3D a) { x-=a.x; y-=a.y; z-=a.z;}
	CoordFloat3D& operator=(const CoordFloat3D& a) {
		if (this == &a)
		{
			return *this;
		}
		else
		{
			x = a.x;
			y = a.y;
			z = a.z;
			return *this;
		}  
	}
};

extern istream& operator>>(istream& os, CoordFloat3D& c);

extern ostream& operator<<(ostream& os, CoordFloat3D& c);

struct CoordDouble3D
{
public:
	double x, y, z;

	CoordDouble3D() {}
	CoordDouble3D(double a, double b, double c) { x = a; y = b; z = c; }

	friend istream& operator>>(istream& os, CoordDouble3D& c);
	friend ostream& operator<<(ostream& os, CoordDouble3D& c);

	CoordDouble3D operator- ()        { return CoordDouble3D(-x, -y, -z); }
	CoordDouble3D operator+ (CoordDouble3D a) { return CoordDouble3D(x + a.x, y + a.y, z + a.z); }
	CoordDouble3D operator- (CoordDouble3D a) { return CoordDouble3D(x - a.x, y - a.y, z - a.z); }
	bool  operator< (CoordDouble3D a) { return (x <  a.x) && (y <  a.y) && (z < a.z); }
	bool  operator<=(CoordDouble3D a) { return (x <= a.x) && (y <= a.y) && (z <= a.z); }
	bool  operator> (CoordDouble3D a) { return (x >  a.x) && (y >  a.y) && (z > a.z); }
	bool  operator>=(CoordDouble3D a) { return (x >= a.x) && (y >= a.y) && (z >= a.z); }
	bool  operator==(CoordDouble3D a) { return (x == a.x) && (y == a.y) && (z == a.z); }
	bool  operator!=(CoordDouble3D a) { return (x != a.x) || (y != a.y) || (z != a.z); }
	void operator-=(CoordDouble3D a) { x-=a.x; y-=a.y; z-=a.z;}
	void operator+=(CoordDouble3D a) { x+=a.x; y+=a.y; z+=a.z;}
	CoordDouble3D& operator=(const CoordDouble3D& a) {
		if (this == &a)
		{
			return *this;
		}
		else
		{
			x = a.x;
			y = a.y;
			z = a.z;
			return *this;
		}  
	}

	double norm()
	{
		return sqrt(x * x + y * y + z * z);
	}
	double norm2()
	{
		return x * x + y * y + z * z;
	}
	double norm(CoordDouble3D& p)
	{
		double dx = x - p.x;
		double dy = y - p.y;
		double dz = z - p.z;
		return sqrt(dx * dx + dy * dy + dz * dz);
	}
	double norm2(CoordDouble3D& p)
	{
		double dx = x - p.x;
		double dy = y - p.y;
		double dz = z - p.z;
		return (dx * dx + dy * dy + dz * dz);
	}
};

extern istream& operator>>(istream& os, CoordDouble3D& c);

extern ostream& operator<<(ostream& os, CoordDouble3D& c);

#define IMREF(im, p) (imRef((im), (p).x, (p).y))


#endif
