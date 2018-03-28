// osgExtractHatchingEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Hatching.h"
#include "osgExtractHatchingEvent.h"
#include "InterfaceBaseDrawer.h"
#include "InterfaceMFCExcute.h"
#include "toShpAndDxf.h"

void COsgExtractHatchingEvent::RegisterJScripts()
{
	//获得exe路径
	char szapipath[MAX_PATH];
	memset(szapipath, 0, MAX_PATH);
	GetModuleFileNameA(NULL, szapipath, MAX_PATH);
	mEXEPath = string(szapipath);
	int pos = mEXEPath.rfind('\\');
	mEXEPath = mEXEPath.substr(0, pos);
	//初始化temp_found 
	std::ifstream in0(mEXEPath + "\\scripts\\initializeTempFound.sql");
	string s0((istreambuf_iterator<char>(in0)), istreambuf_iterator<char>());
	PQexec(conn, s0.c_str());

	std::ifstream in1(mEXEPath + "\\scripts\\deleteFromTempFound.sql");
	string s1((istreambuf_iterator<char>(in1)), istreambuf_iterator<char>());
	PQexec(conn, s1.c_str());//加载脚本

	std::ifstream in2(mEXEPath + "\\scripts\\getBottomTile.sql");
	string s2((istreambuf_iterator<char>(in2)), istreambuf_iterator<char>());
	PQexec(conn, s2.c_str());//加载脚本

	//检查三角面片是否搜索过  未搜索过可选择是否插入
	std::ifstream in3(mEXEPath + "\\scripts\\checkTheTriFound.sql");
	string s3((istreambuf_iterator<char>(in3)), istreambuf_iterator<char>());
	PQexec(conn, s3.c_str());//加载脚本

	std::ifstream in4(mEXEPath + "\\scripts\\getRootTilebyPoint.sql");
	string s4((istreambuf_iterator<char>(in4)), istreambuf_iterator<char>());
	PQexec(conn, s4.c_str());//加载脚本

	std::ifstream in5(mEXEPath + "\\scripts\\getSecondTilebyRoot.sql");
	string s5((istreambuf_iterator<char>(in5)), istreambuf_iterator<char>());
	PQexec(conn, s5.c_str());//加载脚本

	std::ifstream in6(mEXEPath + "\\scripts\\checkBottomTile.sql");
	string s6((istreambuf_iterator<char>(in6)), istreambuf_iterator<char>());
	PQexec(conn, s6.c_str());//加载脚本

	std::ifstream in7(mEXEPath + "\\scripts\\insertGeometry.sql");
	string s7((istreambuf_iterator<char>(in7)), istreambuf_iterator<char>());
	PQexec(conn, s7.c_str());//加载脚本
}

void COsgExtractHatchingEvent::UnRegisterJScripts()
{
	std::ifstream in(mEXEPath + "\\scripts\\UninstallHatchScripts.sql");
	string s((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	PQexec(conn, s.c_str());
	PQexec(conn, "begin;select UninstallHatchScripts();drop function UninstallHatchScripts();commit;");
}

bool COsgExtractHatchingEvent::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if (isActivated)
	{
		InterfaceBaseDrawerFactory IDrawerFactory;
		InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
		if (IsLeftClick(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
			osg::Vec3d worldPt = pickResult.worldPt;
			worldMatrix = pickResult.matrix;
			np = pickResult.nodePath;
			if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
			{
				return false;
			}
			vecWorldCoord.push_back(worldPt);
			if (isBatch)//批量提取模式
			{
				return false;
			}
		
			int totalNum = vecWorldCoord.size();

			if (totalNum <= 1)
			{
				if (totalNum == 1)
				{
					FX1 = ea.getX();
					FY1 = ea.getY();
					NP1 = np;
				}
				return false;
			}
			
			/*************调试用************/
			PS = vecWorldCoord[totalNum - 1];
			PE = vecWorldCoord[totalNum - 2];
			/******************************/

			//开始提取剖面线
			if (!bLocal)
			{
				if(startComputeHatching(ea.getX(), ea.getY(), vecWorldCoord[totalNum - 1], vecWorldCoord[totalNum - 2], np))
					iSigEventCallBack->EmitSigShowOpsHint("按BackSpace删除上一条剖面线，按Z撤销所有，按B键激活批量提取模式，按ESC退出事件");
				else
					iSigEventCallBack->EmitSigShowOpsHint("剖面线提取失败，数据库中可能不存在有效数据");
			}
			else
			{
				std::vector<osg::Vec3d> vecRes;
				computeHatching(vecWorldCoord[totalNum - 1], vecWorldCoord[totalNum - 2],mRoot, vecRes, worldMatrix);
				//画剖面线
				if (vecRes.size() > 0)
				{
					InterfaceBaseDrawerFactory IDrawerFactory;
					InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
					osg::Vec4d color(1.0, 0, 0, 1.0);
					double size = 2.5f;
					IDrawer->BaseDrawCurLineOperate(vecRes, color, size, mapperGroup, worldMatrix);
					iSigEventCallBack->EmitSigShowOpsHint("按BackSpace删除上一条剖面线，按Z撤销所有，按B键激活批量提取模式，按ESC退出事件");

					if (bConnect)
					{
						std::vector<point3D> vec3d;
						for (auto pt : vecRes)
						{
							vec3d.push_back(point3D(pt.x(), pt.y(), pt.z()));
						}
						CPGController  pgControl;
						pgControl.insertGeometry(conn, vec3d, EXTRACT_TYPE::HATCH);
					}
						
				}
			}
			
		}
		else if (IsMouseMove(ea) && GetKeyState(VK_CONTROL) < 0)
		{
			if (isBatch)//批量提取模式 临时绘制路径
			{
				CPickResult pickResult = PickResult(ea.getX(), ea.getY(), mViewer);
				osg::Vec3d worldPt = pickResult.localPt;
				ClearGroupResult(tempGroup);
				if (vecWorldCoord.size()>1)
				{
					IDrawer->BaseDrawCurLineOperate(vecWorldCoord, osg::Vec4(1, 0, 1, 1), 2.5, tempGroup, worldMatrix);
				}
				IDrawer->BaseDrawLineOperate(worldPt, vecWorldCoord.back(), osg::Vec4(1, 0, 1, 1), 2.5, tempGroup, worldMatrix);
			}
		}
		else if(IsKeyPress(ea))
		{
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_BackSpace)					 //BackSpace
			{
				if (isBatch && vecWorldCoord.size() == 1)
				{
					return false;
				}
				if (vecWorldCoord.size() > 0)
				{
					//撤销上个点
					vecWorldCoord.pop_back();
				}

				int childNum = mapperGroup->getNumChildren();

				if (childNum > 0)
				{
					mapperGroup->removeChild(childNum - 1);
					

					iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画两个点，即可自动提取剖面线，按P键进入批量提取模式，按ESC退出事件");
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Escape)
			{
				ClearGroupResult(mapperGroup);
				ClearGroupResult(tempGroup);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_N)//非数据库模式
			{
				bLocal = !bLocal;
				if (bLocal)
				{
					iSigEventCallBack->EmitSigShowOpsHint("激活非数据库提取模式");
				}
				else
					iSigEventCallBack->EmitSigShowOpsHint("激活数据库提取模式");
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_P)//激活批量模式
			{
				isBatch = !isBatch;
				if (isBatch)
				{
					iSigEventCallBack->EmitSigShowOpsHint("激活批量提取模式");
					vecWorldCoord.clear();
					//路径起点默认设置为最后一条剖面线的中点
					vecWorldCoord.push_back((PE + PS) / 2);
				}
				else
				{
					iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画两个点，即可自动提取剖面线，按P键进入批量提取模式，按ESC退出事件");
				}
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Z)
			{
				vecWorldCoord.clear();
				ClearGroupResult(mapperGroup);
				ClearGroupResult(tempGroup);
				isBatch = false;
				iSigEventCallBack->EmitSigShowOpsHint("按Ctrl + 左键画两个点，即可自动提取剖面线，按P键进入批量提取模式，按ESC退出事件");
			}
		}
		else if (GetKeyState(VK_CONTROL) >= 0)
		{
			std::vector<osg::Vec3d> vec;
			if (isBatch)//批量模式松开ctrl键开始提取
			{
				if (vecWorldCoord.size()<2)
				{
					return false;
				}
				PE.z() = PS.z();
				double len = (PE - PS).length();
				//计算按给定宽度和间隔的所有剖面线的两个端点坐标
				getHatchPathMultiline(vecWorldCoord, len,mStep, vec);
				if (!vec.empty() && !vecWorldCoord.empty())
				{
					startComputeBatchHatching(vec);//批量提取
					
					while (vecWorldCoord.size()>1)
					{
						vecWorldCoord.pop_back();
					}
				}
			}
			else
			{
				vecWorldCoord.clear();
			}
			ClearGroupResult(tempGroup);
		}
	}
	else
	{
		vecWorldCoord.clear();
	}
	return false;
}

void COsgExtractHatchingEvent::UpdateActivatedStatus(std::vector<OperatingState> vecOpState)
{
	bool flag = false; 

	if (isActivated == true)
	{
		flag = true;
	}

	isActivated = isActivate(vecOpState, OPS_EXTRACT_HATCH);

	if (isActivated == false && flag == true)
	{
		//ClearGroupResult(mapperGroup);
		if (conn)
		{
			UnRegisterJScripts();
			PQfinish(conn);
		}
		vecWorldCoord.clear();
		
		iSigEventCallBack->EmitSigShowOpsHint("");
	}

	if (isActivated && flag == false)																//刚激活
	{
		CPGController pgControl;
		conn = pgControl.connectToDB();
		if (conn)
		{
			RegisterJScripts();
			bConnect = true;
			iSigEventCallBack->EmitSigShowOpsHint("PostGis连接成功！按Ctrl + 左键画两个点，即可自动提取剖面线，按ESC退出事件");
		}
		else
		{
			iSigEventCallBack->EmitSigShowOpsHint("PostGis连接失败，数据库不可用");
			bConnect = false;
		}	
	}
}


void COsgExtractHatchingEvent::getHatchPathMultiline(std::vector<osg::Vec3d>& vecPtIn, double HatchLenth, double step, std::vector<osg::Vec3d>& vecPtOut)
{
	if (vecPtIn.size()<2)
	{
		return;
	}
	double diff = 0;
	double defaultH = vecPtIn[0].z();
	for (int i = 0;i<vecPtIn.size() - 1;++i)
	{
		osg::Vec3d ps = vecPtIn[i];
		osg::Vec3d pe = vecPtIn[i + 1];
		ps.z() = 0;
		pe.z() = 0;

		int nstep = 0;
		if (ps.x() == pe.x())
		{
			int k = 0;
			if (ps.y() > pe.y())
			{
				while (ps.y() - k*step >= pe.y())
				{
					osg::Vec3d p1(ps.x() - HatchLenth / 2, ps.y() - k*step - diff, defaultH);
					osg::Vec3d p2(ps.x() + HatchLenth / 2, ps.y() - k*step - diff, defaultH);
					vecPtOut.push_back(p1);
					vecPtOut.push_back(p2);
					k++;
				}
			}
			else
			{
				while (ps.y() + k*step <= pe.y())
				{
					osg::Vec3d p1(ps.x() - HatchLenth / 2, ps.y() + k*step - diff, defaultH);
					osg::Vec3d p2(ps.x() + HatchLenth / 2, ps.y() + k*step - diff, defaultH);
					vecPtOut.push_back(p1);
					vecPtOut.push_back(p2);
					k++;
				}
			}			
		}
		else if(ps.y() == pe.y())
		{
			int k = 0;
			if (ps.x() < pe.x())
			{
				while (ps.x() + k*step<=pe.x())
				{
					osg::Vec3d p1(ps.x() + k*step, ps.y() + HatchLenth / 2, defaultH);
					osg::Vec3d p2(ps.x() + k*step, ps.y() - HatchLenth / 2, defaultH);
					vecPtOut.push_back(p1);
					vecPtOut.push_back(p2);
					k++;
				}
			}
			else
			{
				while (ps.x() - k*step >= pe.x())
				{
					osg::Vec3d p1(ps.x() - k*step, ps.y() + HatchLenth / 2, defaultH);
					osg::Vec3d p2(ps.x() - k*step, ps.y() - HatchLenth / 2, defaultH);
					vecPtOut.push_back(p1);
					vecPtOut.push_back(p2);
					k++;
				}
			}
		}
		else
		{
			float k = (ps.y() - pe.y()) / (ps.x() - pe.x());
			float b = ps.y() - k*ps.x();
			float cosa = abs(ps.x() - pe.x()) / (ps - pe).length();
			int m = 0;
			if (ps.x()<pe.x())
			{
				while (ps.x() + m*(step*cosa)<=pe.x())
				{	
					double cx = ps.x() + m*(step*cosa);
					double cy = k*cx + b;

					double y1 = cy + cosa*HatchLenth / 2;
					double y2 = cy - cosa*HatchLenth / 2;
					float nk = -1 / k;
					float nb = cy - nk*cx;
					osg::Vec3d p1((y1 - nb) / nk, y1, defaultH);
					osg::Vec3d p2((y2 - nb) / nk, y2, defaultH);
					vecPtOut.push_back(p1);
					vecPtOut.push_back(p2);
					m++;
				}
			}
			else
			{
				while (ps.x() - m*(step*cosa) >= pe.x())
				{
					double cx = ps.x() - m*(step*cosa);
					double cy = k*cx + b;

					double y1 = cy + cosa*HatchLenth / 2;
					double y2 = cy - cosa*HatchLenth / 2;
					float nk = -1 / k;
					float nb = cy - nk*cx;
					osg::Vec3d p1((y1 - nb) / nk, y1, defaultH);
					osg::Vec3d p2((y2 - nb) / nk, y2, defaultH);
					vecPtOut.push_back(p1);
					vecPtOut.push_back(p2);
					m++;
				}
			}
		}
	}
}

void COsgExtractHatchingEvent::stretchPointToPoint(osg::Vec3d& ps, osg::Vec3d& pe, double len)
{
	//斜率不存在
	if (ps.x() == pe.x())
	{
		if (ps.y() < pe.y())
			len *= -1;
		ps.y() += len;
		return;
	}
	//斜率为0
	if (ps.y() == pe.y())
	{
		if (ps.x() < pe.x())

			ps.x() += len;
		return;
	}
	//斜率存在 且不为0
	double k = (pe.y() - ps.y()) / (pe.x() - ps.x());
	double b = ps.y() - k*ps.x();
	len += (ps - pe).length();
	double sina = abs(ps.y() - pe.y()) / (ps - pe).length();
	double cosa = abs(ps.x() - pe.x()) / (ps - pe).length();
	if (k>0)
	{
		if (ps.x()<pe.x())
		{
			ps.x() = pe.x() - len*cosa;
			ps.y() = pe.y() - len*sina;
		}
		else
		{
			ps.x() = pe.x() + len*cosa;
			ps.y() = pe.y() + len*sina;
		}
	}
	else
	{
		if (ps.x() < pe.x())
		{
			ps.x() = pe.x() - len*cosa;
			ps.y() = pe.y() + len*sina;
		}
		else
		{
			ps.x() = pe.x() + len*cosa;
			ps.y() = pe.y() - len*sina;
		}
	}
	return;
}

bool COsgExtractHatchingEvent::findStartFile(osg::Vec3d& ps, osg::Vec3d& pe, std::string& tableName)
{
	bool br = true;
	osg::Vec3d ps1 = ps + osg::Vec3d(0, 0, 1000);
	osg::Vec3d ps2 = ps + osg::Vec3d(0, 0, -1000);
	osg::Vec3d pe1 = pe + osg::Vec3d(0, 0, 1000);
	osg::Vec3d pe2 = pe + osg::Vec3d(0, 0, -1000);
	osg::NodePath np;
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(ps1, ps2);
	osgUtil::IntersectionVisitor iv(ls);
	mRoot->accept(iv);
	if (ls.valid() && ls->containsIntersections())
	{
		np = ls->getFirstIntersection().nodePath;
	}
	else
	{
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(pe1, pe2);
		osgUtil::IntersectionVisitor iv(ls);
		mRoot->accept(iv);
		if (ls.valid() && ls->containsIntersections())
		{
			np = ls->getFirstIntersection().nodePath;
			osg::Vec3d p = ps;
			ps = pe;
			pe = p;
			br = false;
		}
		else
		{
			return true;
		}
	}
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();

	//查找底层数据
	std::vector<std::string> vecChildFileName;

	for (int i = np.size() - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> (np[i]);

		if (lod)
		{
			IFinder->FindLodBottomLevelFile(lod, vecChildFileName);
			break;
		}
	}
	if (vecChildFileName.size() == 0)
		return true;
	if (vecChildFileName.size() > 1)
	{
		for (int j = 0; j < vecChildFileName.size(); ++j)
		{
			osg::Node* node = osgDB::readNodeFile(vecChildFileName[j]);
			if (node)
			{
				osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(ps + osg::Vec3d(0, 0, 1000), ps + osg::Vec3d(0, 0, -1000));
				osgUtil::IntersectionVisitor iv(ls);
				node->accept(iv);
				if (ls.valid() && ls->containsIntersections())
				{
					tableName = vecChildFileName[j];
					break;
				}
			}
		}
	}
	else
	{
		tableName = vecChildFileName.front();
	}
	tableName = getTableName(tableName);
	replace_all(tableName, "+", "");
	transform(tableName.begin(), tableName.end(), tableName.begin(), ::tolower);
	return br;
}

bool COsgExtractHatchingEvent::startComputeBatchHatching(std::vector<osg::Vec3d>& vecIn)
{
	if (vecIn.size()<2)
	{
		return false;
	}
	char msg[199];
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	CPGController pgControl;
	std::vector<std::string> vecTile;
	for (int i = 2; i<vecIn.size() - 1; i += 2)
	{
		sprintf_s(msg, "批量模式提取剖面线，间隔：%f  当前进度：%d / %d", mStep, i / 2, vecIn.size() / 2 - 1);
		iSigEventCallBack->EmitSigShowOpsHint(msg);
		vecIn[i].z() = 0;
		vecIn[i + 1].z() = 0;
		osg::Vec3d ps = vecIn[i];
		osg::Vec3d pe = vecIn[i + 1];
		if (bLocal)
		{
			osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(ps - osg::Vec3d(0,0,-1000), ps - osg::Vec3d(0,0,1000));
			osgUtil::IntersectionVisitor iv(ls);
			mRoot->accept(iv);
			if (ls.valid() && ls->containsIntersections())
			{
				ps = ls->getFirstIntersection().getWorldIntersectPoint();
			}
			ls = new osgUtil::LineSegmentIntersector(pe - osg::Vec3d(0, 0, -1000), pe - osg::Vec3d(0, 0, 1000));
			osgUtil::IntersectionVisitor iv1(ls);
			mRoot->accept(iv1);
			if (ls.valid() && ls->containsIntersections())
			{
				pe = ls->getFirstIntersection().getWorldIntersectPoint();
			}
			std::vector<osg::Vec3d> vecOsg3d;
			computeHatching(ps, pe, mRoot, vecOsg3d, worldMatrix);
			if (vecOsg3d.size()>0)
			{
				IDrawer->BaseDrawCurLineOperate(vecOsg3d, osg::Vec4(1, 1, 0, 1), 2.5, mapperGroup, worldMatrix);
				if (bConnect)
				{
					std::vector<point3D> vecLineRes;
					pgControl.insertGeometry(conn, vecLineRes, EXTRACT_TYPE::HATCH);//结果保存数据库
				}	
			}
			continue;
		}
		std::string startTable;
		bool br = findStartFile(ps, pe, startTable);

		if (!startTable.empty())
		{
			vecTile.push_back(startTable);
			CTri3D tri;
			int idSel = -1;
			std::vector<int> ivecNeighbor;
			
			int numTri = pgControl.getHatchStartTri(conn, point3D(ps.x(), ps.y(), ps.z()), point3D(pe.x(), pe.y(), pe.z()), startTable, tri, idSel, ivecNeighbor, 1);
			if (numTri<1)
			{
				return false;
			}
			std::vector<point3D> vecLineRes;
			osg::Vec3d pt = ps;
			stretchPointToPoint(pt, pe, 10);
			pgControl.DoSearchHatchPostGis(conn, tri, idSel, startTable, ivecNeighbor, vecLineRes, point3D(pt.x(), pt.y(), pt.z()), point3D(pe.x(), pe.y(), pe.z()));
			if (vecLineRes.size()>0)
			{
				//判断搜索遇到空洞
				if (br && (vecLineRes.back() - point3D(0, 0, vecLineRes.back().z) - point3D(pe.x(), pe.y(), 0)).length() > 3)
				{
					startTable.clear();
					br = findStartFile(pe, ps, startTable);
					if (br && !startTable.empty())
					{
						CTri3D tri;
						int idSel = -1;
						std::vector<int> ivecNeighbor;

						int numTri = pgControl.getHatchStartTri(conn, point3D(pe.x(), pe.y(), pe.z()), point3D(ps.x(), ps.y(), ps.z()), startTable, tri, idSel, ivecNeighbor, 1);
						if (numTri >= 1)
						{
							std::vector<point3D> vecLineRes1;
							stretchPointToPoint(pe, ps, 10);
							pgControl.DoSearchHatchPostGis(conn, tri, idSel, startTable, ivecNeighbor, vecLineRes1, point3D(pe.x(), pe.y(), pe.z()), point3D(ps.x(), ps.y(), ps.z()));
							if (vecLineRes1.size()>0)
							{
								double d1 = (point3D(vecLineRes.back().x, vecLineRes.back().y, 0) - point3D(ps.x(), ps.y(), 0)).length();
								double d2 = (point3D(vecLineRes1.back().x, vecLineRes1.back().y, 0) - point3D(pe.x(), pe.y(), 0)).length();
								double d = (ps - pe).length();
								if (d1 + d2 <= d + 1)
								{
									vecLineRes.insert(vecLineRes.end(), vecLineRes1.rbegin(), vecLineRes1.rend());
								}
							}
						}
					}
				}
				std::vector<osg::Vec3d> vec;
				for (auto pt : vecLineRes)
				{
					vec.push_back(osg::Vec3d(pt.x, pt.y, pt.z));
				}
				IDrawer->BaseDrawCurLineOperate(vec, osg::Vec4(1, 1, 0, 1), 2.5, mapperGroup, worldMatrix);
				pgControl.insertGeometry(conn, vecLineRes, EXTRACT_TYPE::HATCH);//结果保存数据库
			}
		}

	}

	sprintf_s(msg, "批量提取完成，失败：%d", vecIn.size() / 2 - 1 - vecTile.size());
	iSigEventCallBack->EmitSigShowOpsHint(msg);
	return true;
}

bool COsgExtractHatchingEvent::startComputeHatching(float x, float y, osg::Vec3d p1, osg::Vec3d p2, osg::NodePath& np1, bool repeat)
{
	InterfaceBaseDrawerFactory IDrawerFactory;
	InterfaceBaseDrawer* IDrawer = IDrawerFactory.create();
	char msg[99];
	std::vector<point3D> vecLineRes;
	std::vector<osg::Vec3d> vecOsg3d;
	CPGController pgControl;
	//首先 找底层文件
	std::string tableName;
	if (!getLodBottomLevelFile(x, y, np1, mViewer, tableName))//如果搜索底层文件失败
	{
		iSigEventCallBack->EmitSigShowOpsHint("搜索最底层文件失败!");
		return false;
	}
	else
	{//搜索种子面片
		CTri3D tri;
		int idSel;
		std::vector<int> ivecNeighbor;
		int numTri = pgControl.getHatchStartTri(conn, point3D(p1.x(), p1.y(), p1.z()), point3D(p2.x(), p2.y(), p2.z()),
			tableName, tri, idSel, ivecNeighbor);
		if (numTri < 1)
		{
			iSigEventCallBack->EmitSigShowOpsHint("获取种子面片失败，请重新提取剖面线");
			return 0;
		}
		else
		{
			sprintf_s(msg, "成功搜索到种子面片，开始提取剖面线");
			iSigEventCallBack->EmitSigShowOpsHint(msg);
			stretchPointToPoint(p1, p2, 10);
			pgControl.DoSearchHatchPostGis(conn, tri, idSel, tableName, ivecNeighbor, vecLineRes, point3D(p1.x(), p1.y(), p1.z()), point3D(p2.x(), p2.y(), p2.z()));
		}
	}

	if (vecLineRes.size() > 0)
	{
		if ((vecLineRes.back() - point3D(0, 0, vecLineRes.back().z) - point3D(p2.x(), p2.y(), 0)).length() > 3)
		{
			std::string tableName;
			if (getLodBottomLevelFile(FX1, FY1, NP1, mViewer, tableName))
			{
				CTri3D tri;
				int idSel;
				std::vector<int> ivecNeighbor;
				std::vector<point3D> vecLineRes1;
				int numTri = pgControl.getHatchStartTri(conn, point3D(p2.x(), p2.y(), p2.z()), point3D(p1.x(), p1.y(), p1.z()),
					tableName, tri, idSel, ivecNeighbor);
				if (numTri >= 1)
				{
					stretchPointToPoint(p2, p1, 10);
					pgControl.DoSearchHatchPostGis(conn, tri, idSel, tableName, ivecNeighbor, vecLineRes1, point3D(p2.x(), p2.y(), p2.z()), point3D(p1.x(), p1.y(), p1.z()));
					if (vecLineRes1.size() > 0)
					{
						double d1 = (point3D(vecLineRes.back().x, vecLineRes.back().y, 0) - point3D(p1.x(), p1.y(), 0)).length();
						double d2 = (point3D(vecLineRes1.back().x, vecLineRes1.back().y, 0) - point3D(p2.x(), p2.y(), 0)).length();
						double d = (p1 - p2).length();
						if (d1 + d2 <= d + 1)
						{
							vecLineRes.insert(vecLineRes.end(), vecLineRes1.rbegin(), vecLineRes1.rend());
						}

					}
				}
			}
		}
		std::vector<osg::Vec3d> vec1;
		for (auto pt : vecLineRes)
		{
			vec1.push_back(osg::Vec3d(pt.x, pt.y, pt.z));
		}

		IDrawer->BaseDrawCurLineOperate(vec1, osg::Vec4(1, 0, 0, 1), 2.5, mapperGroup, worldMatrix);
		pgControl.insertGeometry(conn, vecLineRes, EXTRACT_TYPE::HATCH);//保存数据库	
	}
	
	return true;
}

bool COsgExtractHatchingEvent::getLodBottomLevelFile(float x, float y, osg::NodePath& np, osgViewer::Viewer *mViewer, std::string& m_tableName)
{
	InterfaceOsgFinderFactory IFinderFactory;
	InterfaceOsgFinder* IFinder = IFinderFactory.create();

	//查找底层数据
	std::vector<std::string> vecChildFileName;

	for (int i = np.size() - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*> (np[i]);

		if (lod)
		{
			IFinder->FindLodBottomLevelFile(lod, vecChildFileName);
			break;
		}
	}
	if (vecChildFileName.size() == 0)
		return false;
	if (vecChildFileName.size() > 1)
	{
		osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();
		osgViewer::Renderer *render = dynamic_cast<osgViewer::Renderer *>(camera->getRenderer());
		osgUtil::SceneView *sceneView = render->getSceneView(0);
		osg::Vec3 vScreen1(x, y, 0);
		osg::Vec3 vScreen2(x, y, 1);
		osg::Vec3 window1, window2;
		sceneView->projectWindowIntoObject(vScreen1, window1);
		sceneView->projectWindowIntoObject(vScreen2, window2);

		for (int j = 0; j < vecChildFileName.size(); ++j)
		{
			osg::Node* node = osgDB::readNodeFile(vecChildFileName[j]);
			if (node)
			{
				osg::ref_ptr<osgUtil::RayIntersector> ri = new osgUtil::RayIntersector(window1, window2 - window1);
				osgUtil::IntersectionVisitor iv1(ri);
				node->accept(iv1);
				if (ri.valid() && ri->containsIntersections())
				{
					m_tableName = vecChildFileName[j];
					break;
				}
			}
		}
	}
	else
	{
		m_tableName = vecChildFileName[0];
	}
	if (m_tableName.empty())
	{
		return false;
	}
	m_tableName = getTableName(m_tableName);
	replace_all(m_tableName, "+", "");
	transform(m_tableName.begin(), m_tableName.end(), m_tableName.begin(), ::tolower);
	return true;
}

string COsgExtractHatchingEvent::getTableName(string fileName)
{
	int pos = fileName.rfind("\\");
	int pos1 = fileName.rfind("\/");
	int pos2 = fileName.rfind(".osgb");

	int posT = (pos > pos1) ? pos : pos1;
	return fileName.substr(posT + 1, pos2 - posT - 1);
}

void COsgExtractHatchingEvent::replace_all(std::string& s, std::string const & t, std::string const & w)
{
	string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
	while (pos != string::npos) { // found   
		s.replace(pos, t_size, w);
		pos = s.find(t, pos + r_size);
	}
}

