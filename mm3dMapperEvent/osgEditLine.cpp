#include "stdafx.h"
#include "osgEditLine.h"

int COsgEditLine::getGroupPoints(osg::Vec3d& worldPt, double dis, osg::Group * mapperGroup, std::vector<osg::Vec3d>& vecPoints)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();
	std::vector<osg::MatrixTransform*> vecTrans;
	vecPoints.clear();
	for (int n = 0; n < mapperGroup->getNumChildren(); ++n)
	{
		IFinder->FindTransNode(mapperGroup->getChild(n), vecTrans);

		for (int i = 0; i < vecTrans.size(); i++)
		{
			osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];
			if (mTrans)
			{
				osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
				IFinder->ExtractVerts(mTrans, extracted_verts);
				osg::Matrix matrix = mTrans->getMatrix();

				if (extracted_verts->size() == 0)
				{
					continue;
				}

				for (int j = 0; j < extracted_verts->size(); j++)
				{
					osg::Vec3d pt(extracted_verts->at(j).x(), extracted_verts->at(j).y(), extracted_verts->at(j).z());
					pt = pt * matrix;

					if (distance2(worldPt, pt) <= dis)
					{
						worldPt = pt;
						for (int kk = 0; kk < vecTrans.size(); ++kk)
						{
							osg::ref_ptr<osg::Vec3Array> extracted_verts = new osg::Vec3Array;
							IFinder->ExtractVerts(vecTrans[kk], extracted_verts);
							osg::Matrix matrix = vecTrans[kk]->getMatrix();

							if (extracted_verts->size() == 0)
							{
								continue;
							}

							for (int k = 0; k < extracted_verts->size(); k++)
							{
								osg::Vec3d pt(extracted_verts->at(k).x(), extracted_verts->at(k).y(), extracted_verts->at(k).z());
								pt = pt * matrix;
								vecPoints.push_back(pt);
							}
						}

						if (vecPoints.size() > 3)//É¾³ýÖØ¸´µã
						{
							for (std::vector<osg::Vec3d>::iterator ite = vecPoints.begin(); ite != vecPoints.end() - 2;)
							{
								if (*ite == *(ite + 1))
								{
									ite = vecPoints.erase(ite);
								}
								else
									++ite;
							}
						}
						return n;
					}
				}
			}
		}
	}
	return -1;
}

void COsgEditLine::getTerminal(osg::Vec3d& worldPt, std::vector<osg::Vec3d>& vecPoints)
{
	double dis = 99999;
	double k;
	osg::Vec3d ptSel;
	for (size_t i = 0; i < vecPoints.size(); ++i)
	{
		if ((k = distance2(worldPt, vecPoints[i])) < dis)
		{
			dis = k;
			ptSel = vecPoints[i];
		}
	}
	worldPt = ptSel;
}

void COsgEditLine::cutLine(osg::Vec3d p1, osg::Vec3d p2, std::vector<osg::Vec3d>& vecPoints, std::vector<osg::Vec3d>& vecAddPoints, std::vector<osg::Vec3d>& vecReserveL, std::vector<osg::Vec3d>& vecReserveS)
{
	std::vector<osg::Vec3d> vec1, vec2;
	size_t index;
	for (size_t i = 0; i < vecPoints.size(); ++i)
	{
		vec1.push_back(vecPoints[i]);
		if (vecPoints[i] == p1 || vecPoints[i] == p2)
		{
			index = i;
			vec2.push_back(vecPoints[i]);
			break;
		}
	}
	for (size_t i = index + 1; i < vecPoints.size(); ++i)
	{
		vec2.push_back(vecPoints[i]);
		if (vecPoints[i] == p1 || vecPoints[i] == p2)
		{
			index = i;
			break;
		}
	}
	if (vecAddPoints.size()>2)
	{
		if (vec1.back() == p1)
		{
			for (size_t i = 1; i < vecAddPoints.size() - 1; ++i)
			{
				vec1.push_back(vecAddPoints[i]);
			}
		}
		else
		{
			for (size_t i = vecAddPoints.size() - 1; i > 0; --i)
			{
				vec1.push_back(vecAddPoints[i]);
			}
		}
	}
	for (size_t i = vecPoints.size()-1; i >= index ; --i)
	{
		vec1.insert(vec1.begin(), vecPoints[i]);
	}

	if (vec1.size()>=vec2.size())
	{
		vecReserveL.assign(vec1.begin(), vec1.end());
		vecReserveS.assign(vec2.begin(), vec2.end());
	}
	else
	{
		if (vecAddPoints.size()>2)
		{
			if (vec2.back() == p1)
			{
				vec2.insert(vec2.end(), vecAddPoints.begin() + 1, vecAddPoints.end() - 1);
			}
			else
			{
				for (size_t i = vecAddPoints.size() - 2;i>0;++i)
				{
					vec2.push_back(vecAddPoints[i]);
				}
			}
		}
		vecReserveS.assign(vec1.begin(), vec1.end());
		vecReserveL.assign(vec2.begin(), vec2.end());
	}
	return;
}

void COsgEditLine::linkVector(std::vector<osg::Vec3d>& vec1L, std::vector<osg::Vec3d>& vec2L, std::vector<osg::Vec3d>& vecPtSel)
{
	if (vec1L.back() == vecPtSel[0])
	{
		if (vec2L.front() == vecPtSel[1])
		{
			vec1L.insert(vec1L.end(), vec2L.begin(), vec2L.end());
		}
		else
		{
			for (size_t i = vec2L.size() - 1; i >= 0; --i)
			{
				vec1L.push_back(vec2L[i]);
			}
		}
	}
	else if (vec1L.back() == vecPtSel[2])
	{
		if (vec2L.front() == vecPtSel[3])
		{
			vec1L.insert(vec1L.end(), vec2L.begin(), vec2L.end());
		}
		else
		{
			for (size_t i = vec2L.size() - 1; i >= 0; --i)
			{
				vec1L.push_back(vec2L[i]);
			}
		}
	}
}