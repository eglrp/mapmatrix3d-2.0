#include "stdafx.h"
#include "PGControl.h"
#include <windows.h>
#include "json/reader.h"
#include "json/value.h"
#include "json/writer.h"
#include <string>

using namespace std;
PGconn* CPGController::connectToDB(char *pghost, char *pgport, char *dbname, char *user, char *pwd)
{
	char *pgoptions, *pgtty;
	pgoptions = NULL;
	pgtty = NULL;
	PGconn *conn = PQsetdbLogin(pghost, pgport, pgoptions, pgtty, dbname, user, pwd);
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		return nullptr;
	}
	else
	{
		return conn;
	}
}

//void CPGController::checkPGTableTable(PGconn* conn,std::string table)
//{
//	//检查存放搜索路径的表，无则创建，有则清空
//	if (table == "temp_found")
//	{
//		string updateSql = "select tablename from pg_tables where tablename = 'temp_found'";
//		PGresult *res = PQexec(const_cast<PGconn *>(conn), updateSql.c_str());
//		if (res != NULL)
//		{
//			int tuple_num = PQntuples(res);
//			if (tuple_num > 0)
//			{
//				PQexec(const_cast<PGconn *>(conn), "TRUNCATE TABLE  temp_found");
//			}
//			else
//			{
//				PQexec(const_cast<PGconn *>(conn), "create TABLE  temp_found(id integer,tile_name VARCHAR(132))");
//			}
//		}
//		else
//		{
//			PQexec(const_cast<PGconn *>(conn), "create TABLE  temp_found(id integer,tile_name VARCHAR(132))");
//		}
//	}
//	else if(table == "object_found")
//	{
//		string updateSql = "select tablename from pg_tables where tablename = 'object_found'";
//		PGresult *res = PQexec(const_cast<PGconn *>(conn), updateSql.c_str());
//		if (res != NULL)
//		{
//			int tuple_num = PQntuples(res);
//			if (tuple_num > 0)
//			{
//				PQexec(const_cast<PGconn *>(conn), "TRUNCATE TABLE  object_found");
//			}
//			else
//			{
//				PQexec(const_cast<PGconn *>(conn), "create TABLE  object_found(id integer,tile_name VARCHAR(132),geom geometry,maxh double precision)");
//			}
//		}
//		else
//		{
//			PQexec(const_cast<PGconn *>(conn), "create TABLE  object_found(id integer,tile_name VARCHAR(132),geom geometry,maxh double precision)");
//		}
//	}
//	
//}

double CPGController::getAngle(point3D& p1, point3D& p2, bool flag)
{
	double m_normal = p2.length();
	double mo = p1.length();
	double ji = p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
	double angle = acos(ji / (mo * m_normal));
	angle = angle * 180 / osg::PI;
	if (flag)
	{
		if (angle > 90)
		{
			return 180 - angle;
		}
	}
	return angle;
}

void CPGController::replace_all(std::string& s, std::string const & t, std::string const & w)
{
	string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
	while (pos != string::npos) { // found   
		s.replace(pos, t_size, w);
		pos = s.find(t, pos + r_size);
	}
}

string CPGController::getTableName(string fileName)
{
	int pos = fileName.rfind("\\");
	int pos1 = fileName.rfind("\/");
	int pos2 = fileName.rfind(".osgb");

	int posT = (pos > pos1) ? pos : pos1;
	return fileName.substr(posT + 1, pos2 - posT - 1);
}

void CPGController::getBottomTile(PGconn* conn, std::vector<string>&vecName, std::vector<string>& vecNeed)
{
	char sql[1024];
	while (!vecName.empty())
	{
		std::vector<std::string> vec1(vecName.begin(),vecName.end());
		vecName.clear();
		for (int i = 0; i < vec1.size(); ++i)
		{
			sprintf_s(sql, "select getBottomTile('%s')", vec1[i].c_str());
			PGresult* res = PQexec(conn, sql);//执行脚本 搜索最底层表
			if (PQntuples(res) > 0)	
				for (int j = 0; j < PQntuples(res); ++j)
					vecName.push_back(PQgetvalue(res, j, 0));			
			else
				vecNeed.push_back(vec1[i]);
		}
	}
}

int CPGController::getNeighborTile(PGconn* conn,CTri3D& tri, point3D ps,point3D pe, point3D pt, std::string curTileName, std::string& sRes, std::vector<int>& vecint)
{
	std::vector<std::string> vecSeemLikeName;
	CTri3D tri1;
	tri1.p1 = tri.p1;
	tri1.p2 = tri.p2;
	tri1.p3 = tri.p3;
	int id = -1;
	char sql[1024];
	sRes = "";
	double dis = 999;
	double geomDis = 999;
	sprintf_s(sql, "select getRootTilebyPoint('{\"x\":%f,\"y\":%f,\"z\":%f}')", pt.x, pt.y, pt.z);
	PGresult* res = PQexec(conn, sql);//执行脚本 搜索所有根表

	for (int w = 0; w < PQntuples(res); ++w)
	{
		string rootTableName = PQgetvalue(res, w, 0);//获得根节点tile
		sprintf_s(sql, "select getSecondTilebyRoot('{\"x\":%f,\"y\":%f,\"z\":%f,\"tb\":\"%s\"}')", pt.x, pt.y, pt.z, rootTableName.c_str());
		PGresult* res1 = PQexec(conn, sql);//执行脚本 获得了所有根节点下的二级节点
		std::vector<string> vecName, vecNeed;
		int iRes = -1;
		//获得了所有根节点下的二级节点
		for (int i = 0; i < PQntuples(res1); ++i)
		{
			vecName.push_back(PQgetvalue(res1, i, 0));
		}
		//查找最底层tile
		getBottomTile(conn, vecName, vecNeed);
		vecName.clear();
		//筛选获得包含末尾点的底层tile,不包括当前tile
		if (vecNeed.size() > 0)
		{
			for (int i = 0; i < vecNeed.size(); ++i)
			{
				sprintf_s(sql, "select checkBottomTile('{\"testTB\":\"%s\",\"curTB\":\"%s\",\"x\":%f,\"y\":%f,\"z\":%f}')", vecNeed[i].c_str(), curTileName.c_str(), pt.x, pt.y, pt.z);
				res1 = PQexec(conn, sql);//执行脚本
				if (PQntuples(res1) > 0)
					vecName.push_back(PQgetvalue(res1, 0, 0));
			}
		}
		else
		{
			continue;
		}
		if (vecName.empty())
		{
			continue;
		}
		std::ifstream in(mEXEPath + "\\scripts\\getHatchIntersectTri.sql");
		string sJS((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
		Json::Reader reader;
		Json::Value root;
		for (int i = 0; i < vecName.size(); ++i)
		{
			string sData = sJS;
			replace_all(sData, "para_tablename", vecName[i]);
			PQexec(conn, sData.c_str());
			sprintf_s(sql, "select getHatchIntersectTri(%f,'linestring(%f %f,%f %f)','polygon((%f %f %f,%f %f %f,%f %f %f,%f %f %f))')",
				pt.z, ps.x, ps.y, pe.x, pe.y, tri1.p1.x, tri1.p1.y, tri1.p1.z, tri1.p2.x,tri1.p2.y,tri1.p2.z, tri1.p3.x, tri1.p3.y, tri1.p3.z, tri1.p1.x, tri1.p1.y, tri1.p1.z);
			res1 = PQexec(conn, sql);
			if (PQntuples(res1)>0)
			{
				reader.parse(PQgetvalue(res1, 0, 0), root);
				sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", root["f2"].asInt(), vecName[i].c_str());
				PGresult* res2 = PQexec(conn, sql);
				if (string(PQgetvalue(res2,0,0)) == string("t"))
				{
					continue;
				}
				if (root["f1"].asDouble() < dis)
				{
					if (dis < 0.01)
					{
						if (root["f15"].asDouble() < geomDis)
						{
							geomDis = root["f15"].asDouble();
							sRes = vecName[i];
							id = root["f2"].asInt();
							tri.p1 = point3D(root["f3"].asDouble(), root["f4"].asDouble(), root["f5"].asDouble());
							tri.p2 = point3D(root["f6"].asDouble(), root["f7"].asDouble(), root["f8"].asDouble());
							tri.p3 = point3D(root["f9"].asDouble(), root["f10"].asDouble(), root["f11"].asDouble());
							vecint.clear();
							if (root["f12"].asInt() != 0)
							{
								vecint.push_back(root["f12"].asInt());
							}
							if (root["f13"].asInt() != 0)
							{
								vecint.push_back(root["f13"].asInt());
							}
							if (root["f14"].asInt() != 0)
							{
								vecint.push_back(root["f14"].asInt());
							}
						}
					}
					else
					{
						dis = root["f1"].asDouble();
						geomDis = root["f15"].asDouble();
						sRes = vecName[i];
						transform(sRes.begin(), sRes.end(), sRes.begin(), ::tolower);
						id = root["f2"].asInt();
						tri.p1 = point3D(root["f3"].asDouble(), root["f4"].asDouble(), root["f5"].asDouble());
						tri.p2 = point3D(root["f6"].asDouble(), root["f7"].asDouble(), root["f8"].asDouble());
						tri.p3 = point3D(root["f9"].asDouble(), root["f10"].asDouble(), root["f11"].asDouble());
						vecint.clear();
						if (root["f12"].asInt() != 0)
						{
							vecint.push_back(root["f12"].asInt());
						}
						if (root["f13"].asInt() != 0)
						{
							vecint.push_back(root["f13"].asInt());
						}
						if (root["f14"].asInt() != 0)
						{
							vecint.push_back(root["f14"].asInt());
						}
					}	
				}
			}
		}
	}
	return id;
}

int CPGController::getNeighborTile(PGconn* conn, CTri3D& tri, point3D pt, std::string curTileName, std::string& sRes, std::vector<int>& vecint)
{
	Json::Reader reader;
	Json::Value root;
	std::vector<std::string> vecSeemLikeName;
	CTri3D tri1;
	tri1.p1 = tri.p1;
	tri1.p2 = tri.p2;
	tri1.p3 = tri.p3;
	int id = -1;
	char sql[1024];
	sRes = "";
	
	sprintf_s(sql, "select getRootTilebyPoint('{\"x\":%f,\"y\":%f,\"z\":%f}')", pt.x, pt.y, pt.z);
	PGresult* res = PQexec(conn, sql);//执行脚本 搜索所有根表
	for (int w = 0;w<PQntuples(res);++w)
	{
		string rootTableName = PQgetvalue(res, w, 0);//获得根节点tile
		sprintf_s(sql, "select getSecondTilebyRoot('{\"x\":%f,\"y\":%f,\"z\":%f,\"tb\":\"%s\"}')",pt.x,pt.y,pt.z, rootTableName.c_str());
		PGresult* res1 = PQexec(conn, sql);//执行脚本 获得了所有根节点下的二级节点
		std::vector<string> vecName, vecNeed;
		int iRes = -1;
		for (int i = 0; i <  PQntuples(res1); ++i)
		{
			vecName.push_back(PQgetvalue(res1, i, 0));
		}

		//查找最底层tile
		getBottomTile(conn, vecName, vecNeed);
		vecName.clear();
		//筛选获得包含末尾点的底层tile,不包括当前tile
		if (vecNeed.size() > 0)
		{
			for (int i = 0; i < vecNeed.size(); ++i)
			{
				sprintf_s(sql, "select checkBottomTile('{\"testTB\":\"%s\",\"curTB\":\"%s\",\"x\":%f,\"y\":%f,\"z\":%f}')", vecNeed[i].c_str(), curTileName.c_str(), pt.x, pt.y, pt.z);
				res1 = PQexec(conn, sql);//执行脚本
				if (PQntuples(res1) > 0)
					vecName.push_back(PQgetvalue(res1, 0, 0));
			}
		}
		else
			continue;
		if (vecName.empty())
			continue;
		bool skip = false;

		std::ifstream in(mEXEPath + "\\scripts\\getContourIntersectTri.sql");
		string sJS((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
		for (int i = 0; i < vecName.size(); ++i)
		{
			std::string sData = sJS;
			replace_all(sData, "para_tablename", vecName[i]);
			PQexec(conn, sData.c_str());
			sprintf_s(sql, "select getContourIntersectTri('polygon((%f %f,%f %f,%f %f,%f %f))',%f)", tri1.p1.x, tri1.p1.y, tri1.p2.x, tri1.p2.y, tri1.p3.x, tri1.p3.y, tri1.p1.x, tri1.p1.y, pt.z);
			res1 = PQexec(conn, sql);//执行脚本，检测当前底层文件中是否有临界面片
			int n = PQntuples(res1);
			int nNeed = -1;
			if (n == 0)
			{
				vecSeemLikeName.push_back(vecName[i]);//如果没有相交面片，保存为备选tile
				continue;
			}
			for (int j = 0; j < n; ++j)
			{
				reader.parse(PQgetvalue(res1, j, 0), root);

				sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", root["f1"].asInt(), vecName[i].c_str());
				PGresult* res2 = PQexec(conn, sql);
				if (PQgetvalue(res2,0,0) == "t")//如果跨过的tile已搜索过
				{
					skip = true;
					break;
				}
				if (root["f14"].asInt() == 1)
					nNeed = j;
			}
			if (skip)
			{
				skip = false;
				continue;
			}
			if (nNeed != -1)
				n = nNeed;
			else
				n = 0;
	
			sRes = vecName[i];
			reader.parse(PQgetvalue(res1, n, 0), root);
			tri.p1.x = root["f2"].asDouble();
			tri.p1.y = root["f3"].asDouble();
			tri.p1.z = root["f4"].asDouble();
			tri.p2.x = root["f5"].asDouble();
			tri.p2.y = root["f6"].asDouble();
			tri.p2.z = root["f7"].asDouble();
			tri.p3.x = root["f8"].asDouble();
			tri.p3.y = root["f9"].asDouble();
			tri.p3.z = root["f10"].asDouble();
			if (root["f11"].asInt() != 0)
			{
				vecint.push_back(root["f11"].asInt());
			}
			if (root["f12"].asInt() != 0)
			{
				vecint.push_back(root["f12"].asInt());
			}
			if (root["f13"].asInt() != 0)
			{
				vecint.push_back(root["f13"].asInt());
			}
			return root["f1"].asInt();
		}
	}
	if (!vecSeemLikeName.empty())//如果有可能的tile,找距离最近的面片
	{
		double dis = 9999;
		std::ifstream in(mEXEPath + "\\scripts\\getTribyDistance.sql");
		string sJS((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
		for (auto file:vecSeemLikeName)
		{
			string sData = sJS;
			replace_all(sData, "para_tablename", file);
			PQexec(conn, sData.c_str());
			sprintf_s(sql, "select getTribyDistance('point(%f %f %f)',%f)",pt.x,pt.y,pt.z,pt.z);
			res = PQexec(conn, sql);//执行脚本
			if (PQntuples(res) != 0)
			{
				reader.parse(PQgetvalue(res, 0, 0), root);//解析第一条记录
				sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", root["f2"].asInt(), file.c_str());
				PGresult* res1 = PQexec(conn, sql);
				if (string(PQgetvalue(res1, 0, 0)) == string("t") )
					continue;
				if (root["f1"].asDouble() < dis)
				{
					dis = root["f1"].asDouble();
					sRes = file;
					tri.p1.x = root["f3"].asDouble();
					tri.p1.y = root["f4"].asDouble();
					tri.p1.z = root["f5"].asDouble();
					tri.p2.x = root["f6"].asDouble();
					tri.p2.y = root["f7"].asDouble();
					tri.p2.z = root["f8"].asDouble();
					tri.p3.x = root["f9"].asDouble();
					tri.p3.y = root["f10"].asDouble();
					tri.p3.z = root["f11"].asDouble();
					if (root["f12"].asInt() != 0)
						vecint.push_back(root["f12"].asInt());
					if (root["f13"].asInt() != 0)
						vecint.push_back(root["f13"].asInt());
					if (root["f14"].asInt() != 0)
						vecint.push_back(root["f14"].asInt());
				
					id = root["f2"].asInt();
				}
			}
		}
		return id;
	}
	return -1;
}

void CPGController::computeOutlinebyPoint(std::vector<point3D>& vecpt, double H, std::vector<point3D>& vec)
{
	computePoints(vecpt[0], vecpt[1],vecpt[2], H, vec);
}

void CPGController::computeHatchbyPoint(CTri3D& tri, point3D p1, point3D p2, std::vector<point3D>& vec)
{
	computePoints(tri.p1, tri.p2, tri.p3, p1, p2, vec);
}

bool CPGController::getCross(point3D& line11, point3D& line12, double pa, double pb, double pc,point3D& pCross)
{
	double a = line11.y - line12.y;
	double b = line12.x - line11.x;
	double c = line11.x * line12.y - line12.x * line11.y;

	double D = a*pb - pa*b;
	if (D == 0)
	{
		return false;
	}
	pCross.x = (b * pc - pb * c) / D;
	pCross.y = (pa * c - a * pc) / D;
	if (line12.x != line11.x)
	{
		pCross.z = (line12.z - line11.z)*(pCross.x - line11.x) / (line12.x - line11.x) + line11.z;
	}
	else
	{
		pCross.z = (line12.z - line11.z)*(pCross.y - line11.y) / (line12.y - line11.y) + line11.z;
	}
	return true;
}

point3D CPGController::getCross(point3D& line11, point3D& line12, double pa,double pb)//输入的两个点已排除x y均相等的情况
{
	point3D CrossP;
	//y = a * x + b;
	//x相等 斜率不存在的情况
	if (line11.x == line12.x)
	{
		CrossP.x = line11.x;
		CrossP.y = pa * CrossP.x + pb;
		CrossP.z = (CrossP.y - line11.y)*(line12.z - line11.z) / (line12.y - line11.y) + line11.z;
		return CrossP;
	}

	//两直线做交 求交点
	double a1 = (line11.y - line12.y) / (line11.x - line12.x);
	double b1 = line11.y - a1 * (line11.x);

	CrossP.x = (b1 - pb) / (pa - a1);
	CrossP.y = a1 * CrossP.x + b1;

	//是端点情况
	if (line11.x == CrossP.x )
	{
		CrossP.z = line11.z;
		return CrossP;
	}
	if (line12.x == CrossP.x)
	{
		CrossP.z = line12.z;
		return CrossP;
	}
	//线段是水平的情况
	if (line11.z == line12.z)
	{
		CrossP.z = line12.z;
		return CrossP;
	}
	
	CrossP.z = (line12.z - line11.z)*(CrossP.x - line11.x) / (line12.x - line11.x) + line11.z;
	return CrossP;
}

void CPGController::computePoints(point3D& Pt1, point3D& Pt2, point3D& Pt3, point3D& pStart, point3D& pEnd, std::vector<point3D>& vec)
{
	if (pStart.x == pEnd.x)//防止路径斜率不存在 人为偏移
	{
		pStart.x += 0.000001;
		pEnd.x -= 0.000001;
	}
	//计算路径线段参数
	double a = (pStart.y - pEnd.y) / (pStart.x - pEnd.x);
	double b = pStart.y - a * (pStart.x);
	
	point3D pt;
	if (Pt1.x == Pt2.x && Pt1.y == Pt2.y);//与水平面垂直的边不考虑
	else
	{
		pt = getCross(Pt1, Pt2, a, b);//先求直线交点，再判断交点是否在线段内
		
		//if (getCross(Pt1, Pt2, a, b, c, pt))
		{
			if (((pt.z >= Pt2.z && pt.z <= Pt1.z) || (pt.z >= Pt1.z && pt.z <= Pt2.z)) &&
				((pt.y >= Pt2.y && pt.y <= Pt1.y) || (pt.y >= Pt1.y && pt.y <= Pt2.y)) &&
				((pt.x >= Pt2.x && pt.x <= Pt1.x) || (pt.x >= Pt1.x && pt.x <= Pt2.x)) &&
				(abs(pt.x - pStart.x) + abs(pt.x - pEnd.x) <= abs(pEnd.x - pStart.x)) + 0.000001)
			{
				vec.push_back(pt);
			}
		}
		
	}
	if (Pt1.x == Pt3.x && Pt1.y == Pt3.y);//与水平面垂直的边不考虑
	else
	{
		pt = getCross(Pt1, Pt3, a, b);//先求直线交点，再判断交点是否在线段内
		
		//if (getCross(Pt1, Pt3, a, b, c, pt))
		{
			if (((pt.z >= Pt1.z && pt.z <= Pt3.z) || (pt.z >= Pt3.z && pt.z <= Pt1.z)) &&
				((pt.y >= Pt1.y && pt.y <= Pt3.y) || (pt.y >= Pt3.y && pt.y <= Pt1.y)) &&
				((pt.x >= Pt1.x && pt.x <= Pt3.x) || (pt.x >= Pt3.x && pt.x <= Pt1.x)) &&
				(abs(pt.x - pStart.x) + abs(pt.x - pEnd.x) <= abs(pEnd.x - pStart.x)) + 0.000001)
			{
				vec.push_back(pt);
			}
		}
		
	}
	if (Pt2.x == Pt3.x && Pt2.y == Pt3.y);//与水平面垂直的边不考虑
	else
	{
		pt = getCross(Pt3, Pt2, a, b);//先求直线交点，再判断交点是否在线段内
		
		//if (getCross(Pt2, Pt3, a, b, c, pt))
		{
			if (((pt.z >= Pt2.z && pt.z <= Pt3.z) || (pt.z >= Pt3.z && pt.z <= Pt2.z)) &&
				((pt.y >= Pt2.y && pt.y <= Pt3.y) || (pt.y >= Pt3.y && pt.y <= Pt2.y)) &&
				((pt.x >= Pt2.x && pt.x <= Pt3.x) || (pt.x >= Pt3.x && pt.x <= Pt2.x)) &&
				(abs(pt.x - pStart.x) + abs(pt.x - pEnd.x) <= abs(pEnd.x - pStart.x)) + 0.000001)
			{
				vec.push_back(pt);
			}
		}	
	}
}

void CPGController::computePoints(point3D Pt1, point3D Pt2, point3D Pt3, double H, std::vector<point3D>& vec)
{
	osg::Vec3d p;
	std::vector<point3D> vec1;
	vec1.clear();
	if ((Pt1.z > H && H > Pt2.z) || (Pt2.z > H && H > Pt1.z))
	{
		//求交点
		vec1.push_back(getInsection(Pt1, Pt2, H));

	}
	if ((Pt1.z > H && H > Pt3.z) || (Pt3.z > H && H > Pt1.z))
	{
		//求交点
		vec1.push_back(getInsection(Pt1, Pt3, H));

	}
	if ((Pt3.z > H && H > Pt2.z) || (Pt2.z > H && H > Pt3.z))
	{
		//求交点
		vec1.push_back(getInsection(Pt3, Pt2, H));
	}
	if (vec1.size() == 2)
	{
		vec.insert(vec.end(), vec1.begin(), vec1.end());
	}
}

void CPGController::split(std::string sNeib, char * s, std::vector<int>& vecInt)
{
	int i = sNeib.find(s);
	while (i != -1)
	{
		vecInt.push_back(atoi(sNeib.substr(0, i).c_str()));
		sNeib = sNeib.substr(i + 1);
		i = sNeib.find(s);
	}
	vecInt.push_back(atoi(sNeib.c_str()));
}

void CPGController::split(std::string sNeib, char * s, std::vector<double>& vecDoub)
{
	int i = sNeib.find(s);
	while (i != -1)
	{
		vecDoub.push_back(atof(sNeib.substr(0, i).c_str()));
		sNeib = sNeib.substr(i + 1);
		i = sNeib.find(s);
	}
	vecDoub.push_back(atof(sNeib.c_str()));
}

bool CPGController::vec3dEqWithEPS(point3D& p1, point3D& p2)
{
	if (abs(p1.x - p2.x) <= 0.0000001 &&
		abs(p1.y - p2.y) <= 0.0000001 )
	{
		return true;
	}
	return false;
}

void CPGController::findContourByNeighbor(PGconn* conn, string m_tableName, std::vector<int>& vecInt, CTri3D &tri, std::vector<point3D>& vecPt, double h,  std::vector<int>& vecRec, bool start)
{
	char sql[1024];
	Json::Reader reader;
	Json::Value root;
	
	//遍历邻域
	for (int i = 0; i < vecInt.size(); ++i)
	{	
		sprintf_s(sql, "select 	checkContourTriangle('{\"id\":%d ,\"h\":%f}')",vecInt[i], h);
		PGresult* res = PQexec(conn, sql);
		std::string sres = PQgetvalue(res, 0, 0);

		if (!reader.parse(sres, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
			continue;	
		else
		{
			if (root["ok"].asInt() == 0)//当前面片不满足要求，搜索下一个
				continue;	
		}
		//面片满足要求
		point3D v1(root["x1"].asDouble(), root["y1"].asDouble(), root["z1"].asDouble());
		point3D v2(root["x2"].asDouble(), root["y2"].asDouble(), root["z2"].asDouble());
		point3D v3(root["x3"].asDouble(), root["y3"].asDouble(), root["z3"].asDouble());

		std::vector<point3D> vecSel, vecres;
		vecSel.push_back(v1);
		vecSel.push_back(v2);
		vecSel.push_back(v3);
		
		computeOutlinebyPoint(vecSel, h, vecres);
		if (vecres.size() == 2)
		{
			if (!start)//如果不是种子点 直接往末尾存
			{
				if (vec3dEqWithEPS(vecPt.back(), vecres[0]))
					vecPt.push_back(vecres[1]);
				else if (vec3dEqWithEPS(vecPt.back(), vecres[1]))
					vecPt.push_back(vecres[0]);
				else
				{
					//在高程范围 但不连续 则从搜索历史中撤销
					sprintf_s(sql, "select deleteFromTempFound(%d,'%s') ", vecInt[i], m_tableName.c_str());
					PQexec(const_cast<PGconn *>(conn), sql);
					continue;
				}
			}
			else//否则  两头判断
			{
				start = false;
				if (vec3dEqWithEPS(vecPt.back(), vecres[0]))
					vecPt.push_back(vecres[1]);
				else if (vec3dEqWithEPS(vecPt.back(), vecres[1]))
					vecPt.push_back(vecres[0]);
				else if (vec3dEqWithEPS(vecPt[vecPt.size() - 2],vecres[0]))
				{
					reverse(vecPt.begin(), vecPt.end());
					vecPt.push_back(vecres[1]);
				}
				else if (vec3dEqWithEPS(vecPt[vecPt.size() - 2], vecres[1]))
				{
					reverse(vecPt.begin(), vecPt.end());
					vecPt.push_back(vecres[0]);
				}
				else
					continue;
			}
			//更新搜索起点 迭代邻域搜索
			tri.p1 = v1;
			tri.p2 = v2;
			tri.p3 = v3;
			std::vector<int> vec1;
			if (root["neibor1"].asInt() != 0)
				vec1.push_back(root["neibor1"].asInt());
			if (root["neibor2"].asInt() != 0)
				vec1.push_back(root["neibor2"].asInt());
			if (root["neibor3"].asInt() != 0)
				vec1.push_back(root["neibor3"].asInt());
			findContourByNeighbor(conn,m_tableName, vec1, tri, vecPt, h, vecRec);
			return;
		}
		else
		{
			continue;
		}
	}
}

int CPGController::getHatchStartTri(PGconn* conn,  point3D ps, point3D pe, std::string bottomfile, CTri3D& tri, int& idSel, std::vector<int>& ivecNeibor, int doBatch)
{
	char sql[1024];
	Json::Reader reader;
	Json::Value root;
	std::ifstream in(mEXEPath + "\\scripts\\getHatchStartTri.sql");
	string s((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	replace_all(s, "para_tablename", bottomfile);
	PQexec(conn, s.c_str());
	sprintf_s(sql, "select getHatchStartTri('{\"doBatch\":%d,\"sx\":%f,\"sy\":%f,\"sz\":%f,\"ex\":%f,\"ey\":%f}')", doBatch,ps.x, ps.y, ps.z, pe.x, pe.y);
	PGresult *res = PQexec(conn, sql);
	if (PQntuples(res)<1)
	{
		return 0;
	}
	reader.parse(PQgetvalue(res, 0, 0), root);
	double x1 = root["f1"].asDouble();
	double y1 = root["f2"].asDouble();
	double z1 = root["f3"].asDouble();
	double x2 = root["f4"].asDouble();
	double y2 = root["f5"].asDouble();
	double z2 = root["f6"].asDouble();
	double x3 = root["f7"].asDouble();
	double y3 = root["f8"].asDouble();
	double z3 = root["f9"].asDouble();
	idSel = root["f15"].asInt();
	tri.p1 = point3D(x1,y1,z1);
	tri.p2 = point3D(x2, y2, z2);
	tri.p3 = point3D(x3, y3, z3);

	ivecNeibor.clear();
	if (root["f12"].asInt() != 0)
		ivecNeibor.push_back(root["f12"].asInt());
	if (root["f13"].asInt() != 0)
		ivecNeibor.push_back(root["f13"].asInt());
	if (root["f14"].asInt() != 0)
		ivecNeibor.push_back(root["f14"].asInt());

	return 1;
}

int CPGController::getContourStartTri(PGconn* conn, point3D worldPt, point3D nm, std::string bottomfile, CTri3D& tri, int& idSel, std::vector<int>& ivecNeighbor)
{
	char sql[256];
	int numTri = 0;
	std::ifstream in(mEXEPath + "\\scripts\\getContourStartTriangle.sql");
	string s((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	replace_all(s, "para_tablename", bottomfile);

	PQexec(conn, s.c_str());
	//执行脚本
	sprintf_s(sql, "select 	getContourStartTri('{\"ptx\":%f ,\"pty\":%f,\"ptz\":%f,\"nmx\":%f,\"nmy\":%f,\"nmz\":%f}')",
		worldPt.x, worldPt.y, worldPt.z, nm.x, nm.y, nm.z);
	PGresult* res = PQexec(conn, sql);

	if (PQntuples(res) > 0)
	{
		std::string sres = PQgetvalue(res, 0, 0);
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(sres, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
		{
			return 0;
		}
		else
		{
			numTri = root["num"].asInt();  // 访问节点
			idSel = root["idSel"].asInt();    // 访问节点
			tri.p1.x = root["tri_p1x"].asDouble();
			tri.p1.y = root["tri_p1y"].asDouble();
			tri.p1.z = root["tri_p1z"].asDouble();
			tri.p2.x = root["tri_p2x"].asDouble();
			tri.p2.y = root["tri_p2y"].asDouble();
			tri.p2.z = root["tri_p2z"].asDouble();
			tri.p3.x = root["tri_p3x"].asDouble();
			tri.p3.y = root["tri_p3y"].asDouble();
			tri.p3.z = root["tri_p3z"].asDouble();
			int n1 = root["neibor1"].asInt();
			int n2 = root["neibor2"].asInt();
			int n3 = root["neibor3"].asInt();

			if (n1 != 0)
			{
				ivecNeighbor.push_back(n1);
			}
			if (n2 != 0)
			{
				ivecNeighbor.push_back(n2);
			}
			if (n3 != 0)
			{
				ivecNeighbor.push_back(n3);
			}
			return numTri;
		}
	}
	else
		return 0;
}

void CPGController::analysisGeometryDataString(std::string& sData, std::vector<point3D>& vecPt3D, std::vector<std::vector<point3D> >& vecVecPt3D, GEOMETRY_TYPE type)
{
	switch (type)
	{
	case PG_POLYGON:
	{
		int pos = sData.find("((");
		std::string str = sData.substr(pos + 2);
		str = str.substr(0, str.size() - 2);
		pos = str.find(",");
		while (pos != -1)
		{
			std::string s = str.substr(0, pos);
			str = str.substr(pos + 1);
			pos = str.find(",");
			int pos1 = s.find_first_of(" ");
			int pos2 = s.find_last_of(" ");
			double x = atof(s.substr(0, pos1).c_str());
			double y = atof(s.substr(pos1 + 1, pos2 - (pos1 + 1)).c_str());
			double z = atof(s.substr(pos2 + 1).c_str());
			vecPt3D.push_back(point3D(x, y, z));
		}
		if (vecPt3D.front() == vecPt3D.back())
		{
			vecPt3D.pop_back();
		}
		break;
	}
		
	case PG_MULTIPOLYGON:
	{
		replace_all(sData, "MULTIPOLYGON(", "");
		replace_all(sData, "MULTIPOLYGON Z (", "");
		replace_all(sData, ")))", "))");
		int pos = sData.find(")),");
		while (pos != -1)
		{
			std::string spoly = sData.substr(0, pos + 2);
			sData = sData.substr(pos + 3);
			replace_all(spoly, "((", "");
			replace_all(spoly, "))", "");

			std::vector<point3D> vPt;
			int pos1 = spoly.find(",");
			while (pos1 != -1)
			{
				std::string s1 = spoly.substr(0, pos1);
				spoly = spoly.substr(pos1 + 1);
				int posSpace1 = s1.find(" ");
				int posSpace2 = s1.rfind(" ");
				double x = atof(s1.substr(0, posSpace1).c_str());
				double y = atof(s1.substr(posSpace1 + 1, posSpace2 - (posSpace1 + 1)).c_str());
				double z = atof(s1.substr(posSpace2 + 1).c_str());
				vPt.push_back(point3D(x,y,z));
				pos1 = spoly.find(",");
			}
			if (vPt.size()>=3)
			{
				vecVecPt3D.push_back(vPt);
			}
			pos = sData.find(")),");
		}
		replace_all(sData, "((", "");
		replace_all(sData, "))", "");
		std::vector<point3D> vPt;
		int pos1 = sData.find(",");
		while (pos1 != -1)
		{
			std::string s1 = sData.substr(0, pos1);
			sData = sData.substr(pos1 + 1);
			int posSpace1 = s1.find(" ");
			int posSpace2 = s1.rfind(" ");
			double x = atof(s1.substr(0, posSpace1).c_str());
			double y = atof(s1.substr(posSpace1 + 1, posSpace2 - (posSpace1 + 1)).c_str());
			double z = atof(s1.substr(posSpace2 + 1).c_str());
			vPt.push_back(point3D(x, y, z));
			pos1 = sData.find(",");
		}
		if (vPt.size() >= 3)
		{
			vecVecPt3D.push_back(vPt);
		}
		break;
	}
	case PG_LINESTRING:
	{
		replace_all(sData, "LINESTRING Z (", "");
		replace_all(sData, ")", "");
		int pos = sData.find(",");
		while (pos != -1)
		{
			std::string s = sData.substr(0, pos);
			sData = sData.substr(pos + 1);
			pos = sData.find(",");
			int pos1 = s.find_first_of(" ");
			int pos2 = s.find_last_of(" ");
			double x = atof(s.substr(0, pos1).c_str());
			double y = atof(s.substr(pos1 + 1, pos2 - (pos1 + 1)).c_str());
			double z = atof(s.substr(pos2 + 1).c_str());
			vecPt3D.push_back(point3D(x, y, z));
		}
		int pos1 = sData.find_first_of(" ");
		int pos2 = sData.find_last_of(" ");
		double x = atof(sData.substr(0, pos1).c_str());
		double y = atof(sData.substr(pos1 + 1, pos2 - (pos1 + 1)).c_str());
		double z = atof(sData.substr(pos2 + 1).c_str());
		vecPt3D.push_back(point3D(x, y, z));
		break;
	}
	default:
		break;
	}
}

int CPGController::deleteGeomFromPG(PGconn* conn, double x, double y)
{
	int k = -1;
	char sql[1024];
	sprintf_s(sql, "select id from tb_contour where geometry_contains(line,'point(%f %f 0)') = 't'",x,y);
	PGresult* res = PQexec(conn, sql);
	if (PQntuples(res)>0)
	{
		k = atoi(PQgetvalue(res, 0, 0));
		sprintf_s(sql, "delete from tb_contour where id = %d",k);
		PQexec(conn, sql);
		sprintf_s(sql, "delete from tb_contour_simple where id = %d", k);
		PQexec(conn, sql);
	}
	return k;
}

void CPGController::getObject(PGconn* conn, double x, double y, float dist, std::string& sPoly, std::vector<CTri3D>& vecTri3D,std::string& sContour,double& mZ)
{
	sPoly.clear();
	vecTri3D.clear();
	char sql[9999];
	//获得选中的轮廓线
	sprintf_s(sql, "select * from tb_contour where st_contains(line,'point(%f %f 0)')", x, y);
	PGresult* res = PQexec(conn, sql);
	if (res == NULL || PQntuples(res) == 0)
	{
		return;
	}
	int id = atoi(PQgetvalue(res, 0, 0));//得到选择的轮廓线编号
	sContour = (PQgetvalue(res, 0, 1));//轮廓线
	mZ = atof(PQgetvalue(res, 0, 2));
	sprintf_s(sql,"delete from tb_house_object where id = %d",id);
	PQexec(conn, sql);
	//sprintf_s(sql, "select id,st_astext(obj) from tb_house_object where id = %d", id);
	//res = PQexec(conn, sql);

	//if (res == NULL)
	//{
	//	return;
	//}
	////如果数据库存在单体成果，返回结果
	//if (PQntuples(res) > 0)
	//{
	//	sPoly = PQgetvalue(res, 0, 1);
	//	return;
	//}
	//不存在单体，则构建
	
	//获得所有的表名

	////////直接获取和轮廓线有交集的表//////
	PQexec(conn, "truncate table temp_object");
	
	std::vector<std::string> vecTile;
	getIntersecTable(conn, sContour, vecTile);
	for (auto tile : vecTile)
	{
		std::string sSql = "select *,v1_v2[1],v2_v3[1],v1_v3[1] from " + tile + " where st_distance(geom,'" +
			sContour + "') <=" + to_string(dist);
		res = PQexec(conn, sSql.c_str());
		int n = PQntuples(res);
		for (int j = 0; j < n; ++j)
		{
			int idx = atoi(PQgetvalue(res, j, 0));
			double x1 = atof(PQgetvalue(res, j, 2));
			double y1 = atof(PQgetvalue(res, j, 3));
			double z1 = atof(PQgetvalue(res, j, 4));
			double x2 = atof(PQgetvalue(res, j, 5));
			double y2 = atof(PQgetvalue(res, j, 6));
			double z2 = atof(PQgetvalue(res, j, 7));
			double x3 = atof(PQgetvalue(res, j, 8));
			double y3 = atof(PQgetvalue(res, j, 9));
			double z3 = atof(PQgetvalue(res, j, 10));
			double nx = atof(PQgetvalue(res, j, 11));
			double ny = atof(PQgetvalue(res, j, 12));
			double nz = atof(PQgetvalue(res, j, 13));
			string sTri = PQgetvalue(res, j, 14);
			int n1 = atoi(PQgetvalue(res, j, 19));
			int n2 = atoi(PQgetvalue(res, j, 20));
			int n3 = atoi(PQgetvalue(res, j, 21));
			CTri3D tri;
			tri.p1 = point3D(x1, y1, z1);
			tri.p2 = point3D(x2, y2, z2);
			tri.p3 = point3D(x3, y3, z3);
			vecTri3D.push_back(tri);
			
			sprintf_s(sql, "insert into temp_object values(%d,'%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,'%s')", idx, tile.c_str(), x1, y1, z1, x2, y2, z2, x3, y3, z3,nx,ny,nz,n1,n2,n3,sTri.c_str());
			PQexec(conn,sql );
		}
	}
	//按距离修补
	//repairObjectTri(conn, dist, vecTri3D);
	//插入数据库
	insertObject(conn, vecTri3D, id, sPoly);
	return;
}

void CPGController::getObjectTest1(PGconn* conn, double x, double y, std::string& s, std::vector<CTri3D>& vecTri)
{
	char sql[9999];
	//获得选中的轮廓线
	sprintf_s(sql, "select id,line from tb_contour where st_contains(line,'point(%f %f 0)')", x, y);
	PGresult* res = PQexec(conn, sql);
	if (res == NULL || PQntuples(res) == 0)
	{
		return;
	}
	int id = atoi(PQgetvalue(res, 0, 0));//得到选择的轮廓线编号
	std::string sContour(PQgetvalue(res, 0, 1));//轮廓线
	double height = atof(PQgetvalue(res, 0, 2));
	std::vector<std::string> vecTile,vecTileNull;
	getIntersecTable(conn, sContour, vecTile);
	std::vector<ID_TILE_PAIR> vecItp;
	for (auto tile:vecTile)
	{
		sprintf_s(sql, "select * from %s where st_intersects(geom,'%s') and v1_z <= %f and v3_z >= %f limit 1",tile.c_str(),sContour.c_str(),height,height);
		res = PQexec(conn, sql);
		if (PQntuples(res) > 0)
		{
			ID_TILE_PAIR itp;
			itp.id = atoi(PQgetvalue(res, 0, 0));
			itp.tilename = tile;
			vecItp.push_back(itp);
		}
		else
			vecTileNull.push_back(tile);
	}
}

void CPGController::insertObject(PGconn* conn, std::vector<CTri3D>& vecTri3D,int id,std::string& sPoly)
{
	sPoly = "MULTIPOLYGON(";
	for (auto tri : vecTri3D)
	{
		sPoly = sPoly + "((" + std::to_string(tri.p1.x) + " " + std::to_string(tri.p1.y) + " " + std::to_string(tri.p1.z)
			+ "," + std::to_string(tri.p2.x) + " " + std::to_string(tri.p2.y) + " " + std::to_string(tri.p2.z)
			+ "," + std::to_string(tri.p3.x) + " " + std::to_string(tri.p3.y) + " " + std::to_string(tri.p3.z)
			+ "," + std::to_string(tri.p1.x) + " " + std::to_string(tri.p1.y) + " " + std::to_string(tri.p1.z)
			+ ")),";
	}
	sPoly = sPoly.substr(0, sPoly.size() - 1);
	sPoly += ")";
	std::string sSql = "insert into tb_house_object values(" + std::to_string(id) +",'" + sPoly + "')";
	PQexec(conn, sSql.c_str());
	return;
}

void CPGController::insertGeometry(PGconn* conn, std::vector<point3D>& vecPt3D, EXTRACT_TYPE type,int id)
{
	char sql[9999];
	
	string sgeom;
	if (type == EXTRACT_TYPE::CONTOUR)
	{
		sgeom = "polygon((";
		for (int i = 0; i < vecPt3D.size(); ++i)
		{
			sgeom += (std::to_string(vecPt3D[i].x) + std::string(" ") + std::to_string(vecPt3D[i].y)
				+ std::string(" ") + std::to_string(vecPt3D[i].z) + std::string(","));
		}
		sgeom += (std::to_string(vecPt3D[0].x) + std::string(" ") + std::to_string(vecPt3D[0].y)
			+ std::string(" ") + std::to_string(vecPt3D[0].z) + "))");
		sprintf_s(sql, "select insertGeometry(0,%d,'", id);//, sgeom.c_str(), vecPt3D[0].z
		string sSQL(sql);
		sSQL = sSQL + sgeom + "'," + to_string(vecPt3D[0].z) + ")";
		
		PQexec(conn, sSQL.c_str());
		return;
	}
	if (type == EXTRACT_TYPE::HATCH)
	{
		sgeom = "linestring(";
		for (int i = 0; i < vecPt3D.size() - 1; ++i)
		{
			sgeom += (std::to_string(vecPt3D[i].x) + std::string(" ") + std::to_string(vecPt3D[i].y)
				+ std::string(" ") + std::to_string(vecPt3D[i].z) + std::string(","));
		}
		sgeom += (std::to_string(vecPt3D.back().x) + std::string(" ") + std::to_string(vecPt3D.back().y)
			+ std::string(" ") + std::to_string(vecPt3D.back().z) + ")");
		//sprintf_s(sql, "select insertGeometry(1,%d,'%s',%f)", id, sgeom.c_str(), vecPt3D[0].z);
		sprintf_s(sql, "select insertGeometry(1,%d,'", id);//, sgeom.c_str(), vecPt3D[0].z
		string sSQL(sql);
		sSQL = sSQL + sgeom + "'," + to_string(vecPt3D[0].z) + ")";
		PQexec(conn, sSQL.c_str());
		return;
	}
}

void CPGController::insertBuilding(PGconn* conn, const std::vector<CLine>& vecBuildingDLG)
{
	if (vecBuildingDLG.empty())
	{
		return;
	}
	std::string sql("'MULTILINESTRING(");
	char msg[99];
	for (int i = 0;i<vecBuildingDLG.size();++i)
	{
		sprintf_s(msg,"(%f %f %f,%f %f %f),", vecBuildingDLG[i].p1.x(), vecBuildingDLG[i].p1.y(), vecBuildingDLG[i].p1.z(),
			vecBuildingDLG[i].p2.x(), vecBuildingDLG[i].p2.y(), vecBuildingDLG[i].p2.z());
		sql += std::string(msg);
	}
	sql = sql.substr(0, sql.size() - 1);
	sql += ")'";

	sql = "insert into tb_house_dlg(geom) values(" + sql + ")";
	PQexec(conn, sql.c_str());
}

void CPGController::getIntersecTable(PGconn* conn, std::string& sGeometry, std::vector<std::string>& vectile)
{
	char sql[1024];
	Json::Reader reader;
	Json::Value root;
	vectile.clear();
	std::ifstream in0(mEXEPath + "\\scripts\\getIntersecTable.sql");
	string s0((istreambuf_iterator<char>(in0)), istreambuf_iterator<char>());
	PQexec(conn, s0.c_str());
	std::string sSql("select getIntersecTable('");
	sSql += (sGeometry + "')");
	//执行脚本
	replace_all(sSql, "''", "'");
	PGresult* res = PQexec(conn, sSql.c_str());
	for (int i = 0;i<PQntuples(res);++i)
	{
		std::string s(PQgetvalue(res, i, 0));
		if (!reader.parse(s, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
			continue;
		else
		{
			vectile.push_back(root["tile"].asString());
		}
	}
}

void CPGController::DoSearchContourPostGis(PGconn* conn,CTri3D& tri, int& idSel, std::string m_tableName, std::vector<int>& ivecNeighbor, std::vector<point3D>& vecLineRes,double h, int clearTemp)
{
	char sql[1024];
	CPGController pgControl;
	vecLineRes.clear();
	std::vector<point3D> vecLineResTemp;
	std::vector<int> iVecRec;//保存当前tile搜索路径，用于撤销搜索
	std::vector<point3D> vecTemp0;//临时容器
	vecTemp0.push_back(tri.p1);
	vecTemp0.push_back(tri.p2);
	vecTemp0.push_back(tri.p3);
	//获取种子面片的两个交点
	pgControl.computeOutlinebyPoint(vecTemp0, h, vecLineResTemp);
	if (vecLineResTemp.size() == 2)//如果交于两点 开始执行搜索
	{
		vecLineRes.insert(vecLineRes.end(), vecLineResTemp.begin(), vecLineResTemp.end());//保存初始的两个点
		
		PQexec(conn, "select checktempRecTable()");
		sprintf_s(sql, "select 	initializeTempFound('{\"id\":%d ,\"clearTemp\":%d,\"save\":1,\"tb\":\"%s\"}')", idSel, clearTemp,m_tableName.c_str());
		PQexec(conn, sql);//执行脚本 清空temp_found 并将起始面片标记为已搜索
		//加载脚本 
		std::ifstream in(mEXEPath + "\\scripts\\checkContourTriangle.sql");
		string sJS((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
		string sData = sJS;
		replace_all(sData, "para_tablename", m_tableName);
		PQexec(conn, sData.c_str());
		//对当前tile进行邻域搜索
		pgControl.findContourByNeighbor(conn,  m_tableName, ivecNeighbor, tri, vecLineRes, h, iVecRec, true);

		string stableNew;//跨tile获得新表名
		while (1)
		{
			ivecNeighbor.clear();
			stableNew.clear();
			//搜索相邻tile
			if (pgControl.vec3dEqWithEPS(vecLineRes.front(), vecLineRes.back()))
				break;
			
			idSel = pgControl.getNeighborTile(conn, tri, vecLineRes.back(), m_tableName, stableNew, ivecNeighbor);

			if (!stableNew.empty() && idSel != -1)//如果找到邻域表
			{
				vecTemp0.clear();
				vecTemp0.push_back(tri.p1);
				vecTemp0.push_back(tri.p2);
				vecTemp0.push_back(tri.p3);
				vecLineResTemp.clear();
				pgControl.computeOutlinebyPoint(vecTemp0, h, vecLineResTemp);//计算新表种子面片两个交点

				sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":1}')", idSel, stableNew.c_str());
				PGresult* res = PQexec(const_cast<PGconn *>(conn), sql);//判断该面片是否搜索过
				string ss(PQgetvalue(res, 0, 0));
				if (string(PQgetvalue(res, 0, 0)) == string("t"))//搜索过  结束搜索
					break;
				
				m_tableName = stableNew;
				//更新脚本函数
				string sData = sJS;
				replace_all(sData, "para_tablename", m_tableName);
				PQexec(conn, sData.c_str());
				
				if (vecLineResTemp.size() == 2)//如果新种子点有两个交点 执行搜索
				{
					vecLineRes.insert(vecLineRes.end(), vecLineResTemp.begin(), vecLineResTemp.end());
					//备份
					std::vector<point3D> vec2;
					vec2.push_back(vecLineRes.back());
					CTri3D triTemp;
					triTemp.p1 = tri.p1;
					triTemp.p2 = tri.p2;
					triTemp.p3 = tri.p3;
					iVecRec.clear();
					//按默认方向邻域搜索一次
					PQexec(conn, "select checktempRecTable()");
					pgControl.findContourByNeighbor(conn, m_tableName, ivecNeighbor, triTemp, vec2, h, iVecRec);

					if (vec2.size() < 2)//如果搜索到少于1 个面片 推测搜索方向错误，进行反向搜索
					{
						vecLineRes.pop_back();
						vecLineRes.pop_back();
						vecLineRes.insert(vecLineRes.end(), vecLineResTemp.rbegin(), vecLineResTemp.rend());
						//撤销搜索记录
						
						sprintf_s(sql, "select clearTempRec('%s')", m_tableName.c_str());
						PQexec(conn, sql);
						
						pgControl.findContourByNeighbor(conn,  m_tableName, ivecNeighbor, tri, vecLineRes, h, iVecRec);
					}
					else//如果搜索到大于一个面片 继续判断
					{
						//判断两个方向向量的夹角
						point3D vv1 = vecLineRes[vecLineRes.size() - 3] - vecLineRes[vecLineRes.size() - 4];
						point3D vv2 = vecLineResTemp[1] - vecLineResTemp[0];
						if (pgControl.getAngle(vv1, vv2) >= 90)//如果是钝角，推测方向错误，反向搜索
						{
							vecLineRes.pop_back();
							vecLineRes.pop_back();
							vecLineRes.insert(vecLineRes.end(), vecLineResTemp.rbegin(), vecLineResTemp.rend());
							//撤销搜索记录
							sprintf_s(sql, "select clearTempRec('%s')", m_tableName.c_str());
							PQexec(conn, sql);
						
							pgControl.findContourByNeighbor(conn,  m_tableName, ivecNeighbor, tri, vecLineRes, h, iVecRec);
						}
						else//如果是锐角，推测方向正确，保存结果
						{
							vec2.erase(vec2.begin());
							vecLineRes.insert(vecLineRes.end(), vec2.begin(), vec2.end());
							tri.p1 = triTemp.p1;
							tri.p2 = triTemp.p2;
							tri.p3 = triTemp.p3;
						}
					}
					continue;//继续跨tile
				}
			}
			break;//跨tile失败则结束搜索
		}
	}
}

void CPGController::DoSearchHatchPostGis(PGconn* conn, CTri3D& tri, int& idSel, std::string m_tableName, std::vector<int>& ivecNeighbor, std::vector<point3D>& vecLineRes, point3D pStart, point3D pEnd, bool clearTemp )
{
	char sql[1024];
	CPGController pgControl;
	vecLineRes.clear();
	std::vector<point3D> vecTemp;//临时容器
	//执行脚本初始化temp_found
	sprintf_s(sql, "select initializeTempFound('{\"id\":%d ,\"clearTemp\":1,\"save\":1,\"tb\":\"%s\"}')", idSel, m_tableName.c_str());
	PQexec(conn,sql );
	
	//获取种子面片的两个交点
	pgControl.computeHatchbyPoint(tri, pStart,pEnd, vecTemp);

	//如果交于两点 开始执行搜索
	if(vecTemp.empty() || vecTemp.size() > 2)
		return;

	if (vecTemp.size() == 2)
	{
		double d1 = (pEnd - point3D(0,0,pEnd.z) - vecTemp[0] - point3D(0,0,-vecTemp[0].z)).length();
		double d2 = (pEnd - point3D(0, 0, pEnd.z) - vecTemp[1] - point3D(0, 0, -vecTemp[1].z)).length();
		if (d1>d2)
		{
			vecLineRes.insert(vecLineRes.end(), vecTemp.begin(), vecTemp.end());
		}
		else
		{
			vecLineRes.insert(vecLineRes.end(), vecTemp.rbegin(), vecTemp.rend());
		}
	}
	if (vecTemp.size() == 1)
	{
		vecLineRes.push_back(pStart);
		vecLineRes.insert(vecLineRes.end(), vecTemp.begin(), vecTemp.end());
	}
	std::ifstream in(mEXEPath + "\\scripts\\checkHatchTriangle.sql");
	string sJS((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	string sData = sJS;
	replace_all(sData, "para_tablename", m_tableName);
	PQexec(conn, sData.c_str());
	pgControl.findHatchByNeighbor(conn, idSel, m_tableName, ivecNeighbor, tri, vecLineRes, pStart, pEnd);
	string stableNew;//跨tile获得新表名
	while (1)
	{
		ivecNeighbor.clear();
		stableNew.clear();
		//搜索相邻tile
		idSel = pgControl.getNeighborTile(conn,tri, pStart,pEnd, vecLineRes.back(), m_tableName, stableNew, ivecNeighbor);
		if (idSel != -1)
		{
			sprintf_s(sql, "select checkTheTriFound('{\"id\":%d ,\"save\":1,\"tb\":\"%s\"}')", idSel, stableNew.c_str());
			PGresult* res = PQexec(const_cast<PGconn *>(conn), sql);
			if (string(PQgetvalue(res, 0, 0)) == string("t"))
				break;
			
			m_tableName = stableNew;
			sData = sJS;
			replace_all(sData, "para_tablename", m_tableName);
			PQexec(conn, sData.c_str());
			vecTemp.clear();
			computeHatchbyPoint(tri,pStart,pEnd,vecTemp);
			if (vecTemp.size() == 1)
			{
				vecLineRes.push_back(vecTemp[0]);
				findHatchByNeighbor(conn, idSel, m_tableName, ivecNeighbor, tri, vecLineRes, pStart, pEnd);
				continue;
			}
			if (vecTemp.size() == 2)
			{
				point3D vv1 = vecLineRes[vecLineRes.size() - 2] - vecLineRes[vecLineRes.size() - 1];
				point3D vv2 = vecTemp[1] - vecTemp[0];
				if (pgControl.getAngle(vv1, vv2) <= 90)//如果是钝角，推测方向错误，反向baocun 
				{
					vecLineRes.insert(vecLineRes.end(), vecTemp.rbegin(), vecTemp.rend());
				}
				else
				{
					vecLineRes.insert(vecLineRes.end(), vecTemp.begin(), vecTemp.end());	
				}
				findHatchByNeighbor(conn, idSel, m_tableName, ivecNeighbor, tri, vecLineRes, pStart, pEnd);
				continue;
			}
		}
		break;
	}
}

void CPGController::findHatchByNeighbor(PGconn* conn, int& idSel, string m_tableName, std::vector<int>& vecInt, CTri3D &tri, std::vector<point3D>& vecPt, point3D pStart, point3D pEnd)
{
	char sql[1024];

	//邻域空 返回
	if (vecInt.empty())
		return;
	Json::Reader reader;
	Json::Value root;
	//遍历邻域
	for (int i = 0; i < vecInt.size(); ++i)
	{
		sprintf_s(sql, "select checkHatchTriangle('{\"id\":%d,\"line\":\"linestring(%f %f,%f %f)\"}')", vecInt[i], pStart.x, pStart.y, pEnd.x, pEnd.y);
		PGresult* res = PQexec(const_cast<PGconn *>(conn), sql);//执行脚本  判断该面片是否符合要求
		string sRes = PQgetvalue(res, 0, 0);
		reader.parse(sRes, root);
		if (!root["empty"].empty())//不符合要求，判断下一个
		{
			continue;
		}
		//符合要求  取面片信息
		std::vector<int> vec1;
		if (root["f10"].asInt() != 0)
			vec1.push_back(root["f10"].asInt());
		if (root["f11"].asInt() != 0)
			vec1.push_back(root["f11"].asInt());
		if (root["f12"].asInt() != 0)
			vec1.push_back(root["f12"].asInt());
		
		std::vector<point3D> vecSel, vecres;
		CTri3D tri1(point3D(root["f1"].asDouble(), root["f2"].asDouble(), root["f3"].asDouble()),
				    point3D(root["f4"].asDouble(), root["f5"].asDouble(), root["f6"].asDouble()),
				    point3D(root["f7"].asDouble(), root["f8"].asDouble(), root["f9"].asDouble()));
		computeHatchbyPoint(tri1,pStart,pEnd,vecres);
		if (vecres.size() >= 2)
		{
			if (vecres.size() == 3)//如果有三个交点 必有两个交点相同，删除一个点
			{
				if (vecres.front() == vecres[1] || vecres.front() == vecres[2])
				{
					vecres.erase(vecres.begin());
				}
				else if (vecres[2] == vecres[1] || vecres[2] == vecres[0])
				{
					vecres.pop_back();
				}
				else if (vecres[1] == vecres[0] || vecres[1] == vecres[2])
				{
					vecres.erase(vecres.begin() + 1);
				}
				else
					continue;
			}
			if (vec3dEqWithEPS(vecres[0],vecPt.back()))
			{
				vecPt.push_back(vecres[1]);
			}
			else if (vec3dEqWithEPS(vecres[1], vecPt.back()))
			{
				vecPt.push_back(vecres[0]);
			}
			else
			{
				sprintf_s(sql, "select deleteFromTempFound(%d,'%s')", vecInt[i], m_tableName.c_str());
				PQexec(const_cast<PGconn *>(conn), sql);
				continue;
			}
			idSel = vecInt[i];
			tri = tri1;
			findHatchByNeighbor(conn,vecInt[i],m_tableName,vec1,tri,vecPt,pStart,pEnd);
			return;
		}
		else
		{
			continue;
		}
	}
}

void CPGController::DoSearchSurfaceFeatures(PGconn* conn, CTri3D& tri, int& idSel, std::string m_tableName, std::vector<int>& ivecNeighbor, point3D normal, float err,  std::vector<CTri3D>& vecTriRes,  int times)
{
	char sql[1024];
	char msg[1024];
	sprintf_s(sql, "select initializeTempFound('{\"id\":%d,\"tb\":\"%s\",\"clearTemp\":1,\"save\":1}')", idSel, m_tableName.c_str());
	//sprintf_s(sql, "select insertIntoTempFound(%d,'%s')", idSel, m_tableName.c_str());
	PQexec(conn, sql);
	CPGController pgControl;
	std::vector<CTri3D> vecTri;//边缘面片
	
	//vecTriRes.push_back(tri);
	std::vector<int> vecEdgeId;

	std::ifstream in0(mEXEPath + "\\scripts\\checkSurfaceTriangle.sql");
	string sJS((istreambuf_iterator<char>(in0)), istreambuf_iterator<char>());
	string sData = sJS;
	replace_all(sData, "para_tablename", m_tableName);
	PQexec(conn, sData.c_str());
	pgControl.findSurfaceByNeighbor(conn, vecTri, vecEdgeId, m_tableName, ivecNeighbor, tri, vecTriRes, normal, err);

	std::vector<std::vector<CTri3D> > vecVecTri;
	std::vector<std::string> vecTableName;

	vecTableName.push_back(m_tableName);
	vecVecTri.push_back(vecTri);
	time_t start, stop;
	start = time(NULL);
	while (vecTableName.size() > 0)
	{
		stop = time(NULL);
		if (stop - start > times && times != 0)
		{
			break;
		}
		std::vector<std::vector<CTri3D> > vecVecTriTemp;
		std::vector<std::string> vecTableNameTemp;
		std::vector<CTri3D> vecTriTotal;
		std::vector<std::string> vecTableNew;
		std::vector<std::vector<int> > vecVecNeiborID;
		std::vector<CTri3D> vecNeiborTri;
		for (int j = 0; j < vecVecTri.size(); ++j)
		{
			for (auto tri1 : vecVecTri[j])
			{
				std::string stableNew;
				idSel = getSurfaceNeighbor(conn, tri1, normal, err, vecTableName[j], stableNew, ivecNeighbor);//跨tile
				if (idSel != -1)
				{
					std::vector<std::string>::iterator result = std::find(vecTableNew.begin(), vecTableNew.end(), stableNew); //查找3
					if (result == vecTableNew.end() || vecTableNew.empty())
					{
						vecTableNew.push_back(stableNew);
						vecVecNeiborID.push_back(ivecNeighbor);
						vecNeiborTri.push_back(tri1);
					}
				}
			}
			for (int i = 0; i < vecTableNew.size(); ++i)
			{
				sData = sJS;
				replace_all(sData, "para_tablename", vecTableNew[i]);
				PQexec(conn, sData.c_str());
				vecTriTotal.clear();
				pgControl.findSurfaceByNeighbor(conn, vecTriTotal, vecEdgeId, vecTableNew[i], vecVecNeiborID[i], vecNeiborTri[i], vecTriRes, normal, err);
				if (vecTriTotal.size() > 0)
				{
					vecVecTriTemp.push_back(vecTriTotal);
					vecTableNameTemp.push_back(vecTableNew[i]);
				}
			}
		}
		vecVecTri.assign(vecVecTriTemp.begin(), vecVecTriTemp.end());
		vecTableName.assign(vecTableNameTemp.begin(), vecTableNameTemp.end());
	}
}

/*



*/
void CPGController::DoSearchPlanePostGis(PGconn* conn, CTri3D& rootTri, int& idSel, std::string m_tableName, std::vector<int>& ivecNeighbor, std::vector<point3D>& vecPtRes, point3D normal,float err, std::vector<CTri3D>& vectri, int clearTemp)
{
	char sql[1024];
	sprintf_s(sql, "select initializeTempFound('{\"id\":%d,\"tb\":\"%s\",\"clearTemp\":%d,\"save\":1}')", idSel, m_tableName.c_str(), clearTemp);
	PQexec(conn, sql);
	CPGController pgControl;
	std::vector<CTri3D> vecEdgeTri;//边缘面片
	std::vector<int> vecEdgeIDs;//边缘面片对应序号
	vecPtRes.push_back(rootTri.p1);//保存结果点集
	vecPtRes.push_back(rootTri.p2);
	vecPtRes.push_back(rootTri.p3);
	std::ifstream in(mEXEPath + "\\scripts\\checkPlaneTriangle.txt");
	string sJS((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	string sData = sJS;
	replace_all(sData, "para_tablename", m_tableName);
	PQexec(conn, sData.c_str());
	
	std::vector<std::vector<int> > vecVecEdgeIDs;
	pgControl.findPlaneByNeighbor(conn, vecEdgeTri, m_tableName, ivecNeighbor, rootTri, vecPtRes, vecEdgeIDs,normal, err);

	//return;
	std::vector<std::vector<CTri3D> > vecVecEdgeTri;
	std::vector<std::string> vecTableName;

	vecTableName.push_back(m_tableName);
	vecVecEdgeTri.push_back(vecEdgeTri);
	vecVecEdgeIDs.push_back(vecEdgeIDs);
	
	while (vecTableName.size()>0)
	{
		std::vector<std::vector<CTri3D> > vecVecTriTemp;
		std::vector<std::string> vecTableNameTemp;
		std::vector<CTri3D> vecTriTotal;
		std::vector<std::string> vecTableNew;
		std::vector<std::vector<int> > vecVecNeiborID;
		std::vector<CTri3D> vecNeiborTri;
		std::vector<std::vector<int> > vecVecEdgeIDsTemp;
		for (int j = 0;j<vecVecEdgeTri.size();++j)
		{
			for (int m = 0;m<vecVecEdgeTri[j].size();++m)
			{
				std::string stableNew;
				int inds = vecVecEdgeIDs[j][m];
				sprintf_s(sql, "select neibortile,neiborid from %s where id = %d", vecTableName[j].c_str(), inds);
				PGresult* res2 = PQexec(conn, sql);
				idSel = atoi(PQgetvalue(res2, 0, 1));
				stableNew = std::string(PQgetvalue(res2, 0, 0));
				
				if (idSel != -1)
				{
					std::vector<std::string>::iterator result = std::find(vecTableNew.begin(), vecTableNew.end(), stableNew); //查找3
					if (result == vecTableNew.end() || vecTableNew.empty())
					{
						sprintf_s(sql, "select v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2[1],v2_v3[1],v1_v3[1],normx,normy,normz from %s where id = %d and testvectorangleinerror(normx,normy,normz,%f,%f,%f,%f) ", stableNew.c_str(), idSel,normal.x,normal.y,normal.z,20.0);
						res2 = PQexec(conn, sql);
						if (PQntuples(res2) == 0)
						{	
							continue;
						}
						double nx = atof(PQgetvalue(res2, 0, 12));
						double ny = atof(PQgetvalue(res2, 0, 13));
						double nz = atof(PQgetvalue(res2, 0, 14));
						if (getAngle(point3D(nx,ny,nz),normal,true)>err)
						{
							continue;
						}
						ivecNeighbor.clear();
						if (atoi(PQgetvalue(res2,0,9))!=0)
						{
							ivecNeighbor.push_back(atoi(PQgetvalue(res2, 0, 9)));
						}
						if (atoi(PQgetvalue(res2, 0, 10)) != 0)
						{
							ivecNeighbor.push_back(atoi(PQgetvalue(res2, 0, 10)));
						}
						if (atoi(PQgetvalue(res2, 0, 11)) != 0)
						{
							ivecNeighbor.push_back(atoi(PQgetvalue(res2, 0, 11)));
						}
						double x1 = atof(PQgetvalue(res2, 0, 0));
						double y1 = atof(PQgetvalue(res2, 0, 1));
						double z1 = atof(PQgetvalue(res2, 0, 2));
						double x2 = atof(PQgetvalue(res2, 0, 3));
						double y2 = atof(PQgetvalue(res2, 0, 4));
						double z2 = atof(PQgetvalue(res2, 0, 5));
						double x3 = atof(PQgetvalue(res2, 0, 6));
						double y3 = atof(PQgetvalue(res2, 0, 7));
						double z3 = atof(PQgetvalue(res2, 0, 8));
						vecVecEdgeTri[j][m] = CTri3D(point3D(x1,y1,z1),point3D(x2,y2,z2),point3D(x3,y3,z3));
						vecTableNew.push_back(stableNew);
						vecVecNeiborID.push_back(ivecNeighbor);
						vecNeiborTri.push_back(vecVecEdgeTri[j][m]);
					}
				}
				PQclear(res2);
			}
			for (int i = 0; i < vecTableNew.size(); ++i)
			{
				sData = sJS;
				replace_all(sData, "para_tablename", vecTableNew[i]);
				PQexec(conn, sData.c_str());//更新脚本
				vecTriTotal.clear();
				vecEdgeIDs.clear();
				pgControl.findPlaneByNeighbor(conn, vecTriTotal, vecTableNew[i], vecVecNeiborID[i], vecNeiborTri[i], vecPtRes, vecEdgeIDs,normal, err);
				if (vecTriTotal.size()>0)
				{
					vecVecTriTemp.push_back(vecTriTotal);
					vecVecEdgeIDsTemp.push_back(vecEdgeIDs);
					vecTableNameTemp.push_back(vecTableNew[i]);
				}
			}
		}
		vecVecEdgeTri.assign(vecVecTriTemp.begin(), vecVecTriTemp.end());
		vecVecEdgeIDs.assign(vecVecEdgeIDsTemp.begin(), vecVecEdgeIDsTemp.end());
		vecTableName.assign(vecTableNameTemp.begin(), vecTableNameTemp.end());
	}
}

void CPGController::findSurfaceByNeighbor(PGconn* conn, std::vector<CTri3D>& vecTriEdge,std::vector<int>& vecEdgeId, string m_tableName, std::vector<int>& vecInt, CTri3D &tri, std::vector<CTri3D>& vecTriRes,  point3D& normal, float err)
{
	char sql[1024];
	std::vector<std::vector<int> > vecVecId;
	std::vector<CTri3D> vecTri;
	vecVecId.push_back(vecInt);
	std::vector<CTri3D> vecTriTemp;
	vecTri.push_back(tri);

	Json::Reader reader;
	Json::Value root;
	while (vecVecId.size() > 0)//n 个三角邻域ID
	{
		std::vector<std::vector<int> > vecVecIdTotal;
		std::vector<int> vecIdTemp;
		
		for (int j = 0; j < vecVecId.size();++j)//遍历n个三角
		{
			bool flag = false;
			for (int i = 0; i < vecVecId[j].size();++i)//遍历三角的n个邻域
			{
				sprintf_s(sql, "select checkSurfaceTriangle('{\"id\":%d,\"nx\":%f,\"ny\":%f,\"nz\":%f,\"err\":%f}')", vecVecId[j][i], normal.x, normal.y, normal.z, err);
				PGresult* res1 = PQexec(const_cast<PGconn *>(conn), sql);
				string s1 = PQgetvalue(res1,0,0);
				reader.parse(s1, root);
				if (root["isOk"].asInt() == 0)
				{
					continue;
				}
				int v12 = root["n1"].asInt();
				int v23 = root["n2"].asInt();
				int v13 = root["n3"].asInt();

				double x1 = root["x1"].asDouble();
				double y1 = root["y1"].asDouble();
				double z1 = root["z1"].asDouble();
				double x2 = root["x2"].asDouble();
				double y2 = root["y2"].asDouble();
				double z2 = root["z2"].asDouble();
				double x3 = root["x3"].asDouble();
				double y3 = root["y3"].asDouble();
				double z3 = root["z3"].asDouble();
				int edge = root["edge"].asInt();
				point3D v1(x1, y1, z1);
				point3D v2(x2, y2, z2);
				point3D v3(x3, y3, z3);
				
				if (v12 != 0)
				{
					sprintf_s(sql,"select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", v12, m_tableName.c_str());
					//sprintf_s(sql, "select * from temp_found where id = %d and tile_name = '%s'", v12, m_tableName);
					PGresult* ress = PQexec(conn, sql);
					if (string(PQgetvalue(ress, 0, 0)) == string("f"))
					{
						vecIdTemp.push_back(v12);
					}
				}
				if (v23 != 0)
				{
					sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", v23, m_tableName.c_str());
					//sprintf_s(sql, "select * from temp_found where id = %d and tile_name = '%s'", v23, m_tableName);
					PGresult* ress = PQexec(conn, sql);
					if (string(PQgetvalue(ress, 0, 0)) == string("f"))
					{
						vecIdTemp.push_back(v23);
					}
				}
				if (v13 != 0)
				{
					sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", v13, m_tableName.c_str());
					//sprintf_s(sql, "select * from temp_found where id = %d and tile_name = '%s'", v13, m_tableName);
					PGresult* ress = PQexec(conn, sql);
					if (string(PQgetvalue(ress, 0, 0)) == string("f"))
					{
						vecIdTemp.push_back(v13);
					}
				}
				if (vecIdTemp.size() > 0)
				{
					vecVecIdTotal.push_back(vecIdTemp);
					vecIdTemp.clear();
					vecTriTemp.push_back(CTri3D(v1,v2,v3));
				}
				if (edge == 1)
				{
					vecTriEdge.push_back(CTri3D(v1, v2, v3));
					vecEdgeId.push_back(vecVecId[j][i]);
				}
			}
		}
		vecVecId.assign(vecVecIdTotal.begin(), vecVecIdTotal.end());
		vecTri.assign(vecTriTemp.begin(),vecTriTemp.end());
		vecTriTemp.clear();
	}
}

void CPGController::findPlaneByNeighbor(PGconn* conn, std::vector<CTri3D>& vecTri, string m_tableName, std::vector<int>& vecInt, CTri3D &tri, std::vector<point3D>& vecPt, std::vector<int>& vecID, point3D& normal, float err)
{
	char sql[1024];
	std::vector<std::vector<int> > vecVecId;
	vecVecId.push_back(vecInt);

	while (vecVecId.size()>0)//n 个三角邻域ID
	{
		Json::Reader reader;
		Json::Value root;
		std::vector<std::vector<int> > vecVecIdTotal;
		std::vector<int> vecIdTemp;
		for (auto vecId: vecVecId)//遍历n个三角
		{
			for (auto id: vecId)//遍历三角的n个邻域
			{
				sprintf_s(sql, "select checkPlaneTriangle('{\"id\":%d,\"nx\":%f,\"ny\":%f,\"nz\":%f,\"err\":%f}')",id, normal.x, normal.y, normal.z, err);
				PGresult* res = PQexec(const_cast<PGconn *>(conn), sql);
				
				string sRes = PQgetvalue(res, 0, 0);
				reader.parse(sRes, root);
				if (root["isOk"].asInt() == 0)
				{
					continue;
				}
				double x1 = root["x1"].asDouble();
				double y1 = root["y1"].asDouble();
				double z1 = root["z1"].asDouble();
				double x2 = root["x2"].asDouble();
				double y2 = root["y2"].asDouble();
				double z2 = root["z2"].asDouble();
				double x3 = root["x3"].asDouble();
				double y3 = root["y3"].asDouble();
				double z3 = root["z3"].asDouble();
				
				int edge = root["edge"].asInt();
			
				point3D v1(x1, y1, z1);
				point3D v2(x2, y2, z2);
				point3D v3(x3, y3, z3);
				//保存结果
				vecPt.push_back(v1);

				vecPt.push_back(v2);

				vecPt.push_back(v3);
							
				if (root["n1"].asInt()!=0)
				{
					sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", root["n1"].asInt(), m_tableName.c_str());
					PGresult* ress = PQexec(conn, sql);
					if (string(PQgetvalue(ress, 0, 0)) == string("f"))
					{
						vecIdTemp.push_back(root["n1"].asInt());
					}
				}
				if (root["n2"].asInt() != 0)
				{
					sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", root["n2"].asInt(), m_tableName.c_str());
					PGresult* ress = PQexec(conn, sql);
					if (string(PQgetvalue(ress, 0, 0)) == string("f"))
					{
						vecIdTemp.push_back(root["n2"].asInt());
					}
				}
				if (root["n3"].asInt() != 0)
				{
					sprintf_s(sql, "select checkTheTriFound('{\"id\":%d,\"tb\":\"%s\",\"save\":0}')", root["n3"].asInt(), m_tableName.c_str());
					PGresult* ress = PQexec(conn, sql);
					if (string(PQgetvalue(ress, 0, 0)) == string("f"))
					{
						vecIdTemp.push_back(root["n3"].asInt());
					}
				}
				if (vecIdTemp.size()>0)
				{
					vecVecIdTotal.push_back(vecIdTemp);
					vecIdTemp.clear();
				}
				if (edge == 1)
				{
					vecTri.push_back(CTri3D(v1, v2, v3));
					vecID.push_back(id);
				}
			}	
		}
		vecVecId.assign(vecVecIdTotal.begin(), vecVecIdTotal.end());
	}
	std::sort(vecPt.begin(), vecPt.end());
	vecPt.erase(unique(vecPt.begin(), vecPt.end()), vecPt.end());
}

int CPGController::getSurfaceNeighbor(PGconn* conn, CTri3D& tri, point3D& normal, float err, std::string curTileName, std::string& sRes, std::vector<int>& vecint)
{
	char sql[9999];
	sprintf(sql, "'polygon((%f %f,%f %f,%f %f,%f %f))'", tri.p1.x, tri.p1.y, tri.p2.x, tri.p2.y, tri.p3.x, tri.p3.y, tri.p1.x, tri.p1.y);
	std::vector<std::string> vecTile;
	//获得与边缘面片相交的tile
	std::string sGeom(sql);
	getIntersecTable(conn, std::string(sql), vecTile);
	//筛选
	for (auto tile : vecTile)
	{
		if (tile == curTileName)
		{
			continue;
		}
		//geometry_distance_centroid
		sprintf(sql, "select st_distance(geom,%s) as dis,id,v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z, \
					  array_to_string(v1_v2, '&'), array_to_string(v2_v3, '&'),array_to_string(v1_v3, '&') \
					  from %s where st_intersects(geom,%s) and testvectorangleinerror(normx,normy,normz,%f,%f,%f,%f) order by dis limit 1", sGeom.c_str(), tile.c_str(), sGeom.c_str(),
			 normal.x, normal.y, normal.z, err);
		PGresult* res = PQexec(conn, sql);
		if (res && PQntuples(res) > 0)
		{
			double x1 = atof(PQgetvalue(res, 0, 2));
			double y1 = atof(PQgetvalue(res, 0, 3));
			double z1 = atof(PQgetvalue(res, 0, 4));
			double x2 = atof(PQgetvalue(res, 0, 5));
			double y2 = atof(PQgetvalue(res, 0, 6));
			double z2 = atof(PQgetvalue(res, 0, 7));
			double x3 = atof(PQgetvalue(res, 0, 8));
			double y3 = atof(PQgetvalue(res, 0, 9));
			double z3 = atof(PQgetvalue(res, 0, 10));
			std::string v12(PQgetvalue(res, 0, 11));
			std::string v23(PQgetvalue(res, 0, 12));
			std::string v31(PQgetvalue(res, 0, 13));
			
			if (abs(z1+z3 - tri.p1.z - tri.p3.z) > 2)
			{
				continue;
			}
			sRes = tile;
			tri.p1 = point3D(x1, y1, z1);
			tri.p2 = point3D(x2, y2, z2);
			tri.p3 = point3D(x3, y3, z3);
			vecint.clear();
			if (!v12.empty())
				vecint.push_back(atoi(v12.c_str()));
			if (!v23.empty())
				vecint.push_back(atoi(v23.c_str()));
			if (!v31.empty())
				vecint.push_back(atoi(v31.c_str()));
			
			return atoi(PQgetvalue(res, 0, 1));
		}
	}
	return -1;
}

int CPGController::getNeighborTile(PGconn* conn, CTri3D& tri, point3D normal,float err, std::string curTileName, std::string& sRes, std::vector<int>& vecint)
{
	char sql[9999];
	sprintf(sql, "'polygon((%f %f,%f %f,%f %f,%f %f))'", tri.p1.x, tri.p1.y, tri.p2.x, tri.p2.y, tri.p3.x, tri.p3.y, tri.p1.x, tri.p1.y);
	std::vector<std::string> vecTile;
	//获得与边缘面片相交的tile
	std::string sGeom(sql);
	getIntersecTable(conn, std::string(sql), vecTile);
	//筛选
	std::ifstream in(mEXEPath + "\\scripts\\getPlaneIntersectTri.sql");
	string sJS((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	
	Json::Reader reader;
	Json::Value root;
	for (auto tile:vecTile)
	{
		if (tile == curTileName)
		{
			continue;
		}
		string sData = sJS;
		replace_all(sData, "para_tablename", tile);
		PQexec(conn, sData.c_str());
		//geometry_distance_centroid
		sprintf_s(sql, "select getPlaneIntersectTri(%s,%f,%f,%f,%f,%f,%f,%f)", sGeom.c_str(), tri.p1.z, tri.p2.z, tri.p3.z, normal.x, normal.y, normal.z, err);
		PGresult* res = PQexec(conn, sql);
		if (res && PQntuples(res)>0)
		{
			sRes = tile;
			string s = PQgetvalue(res, 0, 0);
			reader.parse(s, root);

			double x1 = root["f3"].asDouble();
			double y1 = root["f4"].asDouble();
			double z1 = root["f5"].asDouble();
			double x2 = root["f6"].asDouble();
			double y2 = root["f7"].asDouble();
			double z2 = root["f8"].asDouble();
			double x3 = root["f9"].asDouble();
			double y3 = root["f10"].asDouble();
			double z3 = root["f11"].asDouble();
		
			tri.p1 = point3D(x1, y1, z1);
			tri.p2 = point3D(x2, y2, z2);
			tri.p3 = point3D(x3, y3, z3);
			vecint.clear();
			int v12 = root["f12"].asInt();
			int v23 = root["f13"].asInt();
			int v13 = root["f14"].asInt();
			if (v12!=0)
				vecint.push_back(v12);
			if (v23 != 0)
				vecint.push_back(v23);
			if (v13 != 0)
				vecint.push_back(v13);
			return root["f2"].asInt();
		}
	}
	return -1;	
}
