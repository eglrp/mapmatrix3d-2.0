#pragma once
#include <math.h>
#include <vector>
namespace nbsp {

	class pointXYZ {

	public:
		pointXYZ(double a = 0, double b = 0, double c = 0)
		{
			x = a; y = b; z = c;
		}
		pointXYZ operator-(pointXYZ other)
		{
			pointXYZ t;
			t.x = x - other.x;
			t.y = y - other.y;
			t.z = z - other.z;
			return t;
		}
		pointXYZ& operator=(pointXYZ other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}
		bool operator==(const pointXYZ &p1)const {
			if (x == p1.x && y == p1.y && z == p1.z)
				return true;
			return false;
		}
		double x;
		double y;
		double z;
	};


	class CNbspAlgorithm
	{
	public:
		void computePoly(std::vector<pointXYZ> vecPts, std::vector<pointXYZ>& vecOutPoly);

	private:
		float computeAngle(double ux, double uy, double uz, double vx, double vy, double vz,bool bfit = true);
		
	};
}