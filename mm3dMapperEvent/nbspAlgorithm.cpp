#include "stdafx.h"
#include "nbspAlgorithm.h"

using namespace nbsp;

float CNbspAlgorithm::computeAngle(double ux, double uy, double uz, double vx, double vy, double vz, bool bfit)
{
	double m1 = sqrt(ux*ux + uy*uy + uz*uz);
	double m2 = sqrt(vx*vx + vy*vy + vz*vz);

	float angle = acos((ux*vx + uy * vy + uz*vz) / (m1*m2)) * 180 / 3.1415926;
    
	if (bfit && angle > 90)
	{
		return 180 - angle;
	}
	return angle;
}


void CNbspAlgorithm::computePoly(std::vector<pointXYZ> vecPts, std::vector<pointXYZ>& vecOutPoly)
{
	/* 
	1 找到一个边界点:xmax, xmin, ymax, ymin都可以
	2 找第二个边界点，例如：1步骤中找到ymin后，遍历所有点，寻找到一点，使这点与ymin的线段与水平的夹角最小；
	3 针对上两个边界点AB，找下一个边界点C 使脚ABC最大，接近180度；可以使用余弦定理
	4 当找到的边界点为最初的ymin时，结束查找，把以上边界点按次序导出
	*/
	if (vecPts.size()<4)
	{
		vecOutPoly.assign(vecPts.begin(), vecPts.end());
		return;
	}
	double dy = 9999.0;
	int index;
	for (int i = 0;i<vecPts.size();++i)
	{
		if (vecPts[i].y<dy)
		{
			dy = vecPts[i].y;
			index = i;
		}
	}
	pointXYZ ptStart(vecPts[index].x, vecPts[index].y, vecPts[index].z);//获得起始点，y值最小
	vecOutPoly.push_back(ptStart);

	//获得第二个点
	pointXYZ pt;
	float arc = 90.0;
	for (std::vector<pointXYZ>::iterator ite = vecPts.begin();ite != vecPts.end();)
	{
		float ang = computeAngle((*ite).x - ptStart.x, (*ite).y - ptStart.y, 0, 1, 0, 0);
		if (ang < arc)
		{
			pt = *ite;
			arc = ang;
			//ite = vecPts.erase(ite);
			//continue;
			++ite;
		}
		else if (ang == arc)
		{
			//共线  保留最远点
			double d1 = pow(pt.x - ptStart.x, 2) + pow(pt.y - ptStart.y, 2);
			double d2 = pow((*ite).x - ptStart.x, 2) + pow((*ite).y - ptStart.y, 2);
			if (d2 > d1)
			{
				pt = *ite;
			}
			else
				ite = vecPts.erase(ite);
			continue;
		}
		else
			++ite;
	}
	vecOutPoly.push_back(pt);

	//连接其余点

	do{
		arc = 0.0;
		pointXYZ v = vecOutPoly[vecOutPoly.size() - 2] - vecOutPoly.back();
		for (std::vector<pointXYZ>::iterator ite = vecPts.begin(); ite != vecPts.end();)
		{
			pointXYZ vs = *ite - vecOutPoly.back();
			float ang = computeAngle(v.x, v.y, 0, vs.x, vs.y, 0, false);
			if (ang > arc)
			{
				pt = *ite;
				arc = ang;
				//ite = vecPts.erase(ite);
				++ite;
				continue;
			}
			else if (ang == arc && 0.0 != arc)
			{
				double d1 = pow(pt.x - vecOutPoly.back().x, 2) + pow(pt.y - vecOutPoly.back().y, 2);
				double d2 = pow((*ite).x - vecOutPoly.back().x, 2) + pow((*ite).y - vecOutPoly.back().y, 2);
				if (d2 > d1)
				{
					pt = *ite;
				}
				else
					ite = vecPts.erase(ite);
				continue;
			}
			else
				++ite;
		}
		vecOutPoly.push_back(pt);
	} while (!(pt == vecOutPoly.front()));
	vecOutPoly.pop_back();
}