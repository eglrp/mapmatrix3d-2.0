#include "StdAfx.h"
#include "SceneDemGenerator.h"
#include "InterfaceOsgFinder.h"
#include "GDAL_PLUS.h"
#include "InterfaceMFCExcute.h"
#include "InterfaceOsgAlgorithm.h"
#include "qcomm.h"

void CNodeDemCreator::InterpolateDem(osg::Geode& geode, osg::Matrix matrix, CvMat* demMat, GridDem gDem, bool isRepair)
{
	int childGeomNum = geode.getNumDrawables();

	for (int i = 0; i < childGeomNum; i++)
	{	
		osg::ref_ptr<osg::Geometry> geom = dynamic_cast<osg::Geometry*> (geode.getDrawable(i));
		int iAverageSize = 0;

		if (geom)
		{
			osg::ref_ptr<osg::Vec3Array> verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray()); 
			//获取三角形序号
			osg::PrimitiveSet* priSet = geom->getPrimitiveSet(0);
			osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
			int cnt = drawElementTriangle->getNumIndices();
			int triCnt = cnt / 3;

			for (int w = 0; w < triCnt; w++)
			{
				int index1 = drawElementTriangle->index(w * 3);
				int index2 = drawElementTriangle->index(w * 3 + 1);
				int index3 = drawElementTriangle->index(w * 3 + 2);

				//顶点坐标对应新影像坐标
				std::vector<CoordDouble3D> pt;
				double vertX1 = verts->at(index1).x();
				double vertY1 = verts->at(index1).y();
				double vertZ1 = verts->at(index1).z();
				osg::Vec3d tempPt1(vertX1, vertY1, vertZ1);
				tempPt1 = tempPt1 * matrix;
				pt.push_back(CoordDouble3D(tempPt1.x(), tempPt1.y(), tempPt1.z()));
				vertX1 = tempPt1.x();
				vertY1 = tempPt1.y();
				vertZ1 = tempPt1.z();

				double vertX2 = verts->at(index2).x();
				double vertY2 = verts->at(index2).y();
				double vertZ2 = verts->at(index2).z();
				osg::Vec3d tempPt2(vertX2, vertY2, vertZ2);
				tempPt2 = tempPt2 * matrix;
				pt.push_back(CoordDouble3D(tempPt2.x(), tempPt2.y(), tempPt2.z()));
				vertX2 = tempPt2.x();
				vertY2 = tempPt2.y();
				vertZ2 = tempPt2.z();

				double vertX3 = verts->at(index3).x();
				double vertY3 = verts->at(index3).y();
				double vertZ3 = verts->at(index3).z();
				osg::Vec3d tempPt3(vertX3, vertY3, vertZ3);
				tempPt3 = tempPt3 * matrix;
				pt.push_back(CoordDouble3D(tempPt3.x(), tempPt3.y(), tempPt3.z()));
				vertX3 = tempPt3.x();
				vertY3 = tempPt3.y();
				vertZ3 = tempPt3.z();

				double lfTmp=max(pt[0].x, max(pt[1].x, pt[2].x)) - gDem.x0;
				int iCol_max = ceil(lfTmp / gDem.dx);

				lfTmp = max(pt[0].y, max(pt[1].y, pt[2].y)) - gDem.y0;
				int iRow_max = ceil(lfTmp / gDem.dy);

				lfTmp = min(pt[0].x, min(pt[1].x, pt[2].x)) - gDem.x0;
				int iCol_min = floor(lfTmp / gDem.dx);

				lfTmp = min(pt[0].y, min(pt[1].y, pt[2].y)) - gDem.y0;
				int iRow_min = floor(lfTmp / gDem.dy);

				iRow_min--;
				iRow_max++;
				iCol_min--;
				iCol_max++;

				if(w == 0)
				{
					iAverageSize = max((iRow_max - iRow_min), (iCol_max - iCol_min)); 
				}
				else
				{
					if((iRow_max - iRow_min) > iAverageSize * 10||(iCol_max - iCol_min) > iAverageSize * 10)
					{
						continue; //非正常三角形。。。不处理
					}
				}

				std::vector<CL::Vec2d> pol;
				pol.push_back(CL::Vec2d(vertX1, vertY1));
				pol.push_back(CL::Vec2d(vertX2, vertY2));
				pol.push_back(CL::Vec2d(vertX3, vertY3));
				pol.push_back(CL::Vec2d(vertX1, vertY1));

				for(int k = iRow_min; k <= iRow_max; k++)
				{
					for(int s = iCol_min; s <= iCol_max; s++)
					{
						if(k < 0)
							k = 0;
						if(s < 0)
							s = 0;
						if(k > (gDem.rows - 1))
							continue;
						if(s > (gDem.cols - 1))
							continue;

						double x = gDem.x0 + gDem.dx * s;
						double y = gDem.y0 + gDem.dy * k;
						//利用反距离权重进行插值
						double d1 = sqrt((vertX1 - x) * (vertX1 - x) + (vertY1 - y) * (vertY1 - y));
						double d2 = sqrt((vertX2 - x) * (vertX2 - x) + (vertY2 - y) * (vertY2 - y));
						double d3 = sqrt((vertX3 - x) * (vertX3 - x) + (vertY3 - y) * (vertY3 - y));
						double z = (1 / d1) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ1 + (1 / d2) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ2 + (1 / d3) / (1 / d1 + 1 / d2 + 1 / d3) * vertZ3;

						if (Point_In_Polygon_2D(x, y, pol))
						{
							double oriZ = CV_MAT_ELEM(*demMat, double, k, s);

							if (oriZ == 0)
							{
								CV_MAT_ELEM(*demMat, double, k, s) = z;
							}
							else if (!isRepair)
							{
								if (oriZ < z)
								{
									CV_MAT_ELEM(*demMat, double, k, s) = z;
								}
							}
						}

					}
				}
			}
		}
	}
}

CvMat* CSceneDemGenerator::GenerateDem(std::string rootModelPath, osg::ref_ptr<osg::MatrixTransform> transNode, double Xinterval, double Yinterval, InterfaceSigEventCallBack* ISigCallBack)	//生成DEM
{
	int pos = 0;
	//ISigCallBack->EmitSigSetProgressFun(pos);
	ISigCallBack->EmitSigStartProgressFun();
	InterfaceMFCExcuteFactory IMFCExecuteFactory;
	InterfaceMFCExcute* IMFCExecute = IMFCExecuteFactory.create();

	//查找pagedlod并赋值
	std::vector<osg::PagedLOD*> vecPagedLod;
	InterfaceOsgFinderFactory IFactory;
	InterfaceOsgFinder* IOsgFinder = IFactory.create();
	IOsgFinder->FindPagedLod(transNode, vecPagedLod);

	if (vecPagedLod.size() == 0)
	{
		return NULL;
	}

	std::vector<std::string> vecGroupHighLevelChildName;

	for (int i = 0; i < vecPagedLod.size(); i++)
	{
		std::vector<std::string> vecTileHighLevelChildName;
		IOsgFinder->FindLodBottomLevelFile(vecPagedLod[i], vecTileHighLevelChildName);

		if (vecTileHighLevelChildName.size() > 0)
		{
			vecGroupHighLevelChildName.insert(vecGroupHighLevelChildName.end(), vecTileHighLevelChildName.begin(), vecTileHighLevelChildName.end());

			pos = (int)((double) (i + 1) / (double) vecPagedLod.size() * 150);
			ISigCallBack->EmitSigSetProgressFun(pos);
		}
	}

	_logop("start find bottom level file");
	//查找最底层的文件
	pos = 150;
	ISigCallBack->EmitSigSetProgressFun(pos);

	//获取各底层文件范围
	_logop("start find bottom level rect");

	std::vector<osg::Vec3d> minMaxNode;

	osg::Matrix matrix = transNode->getMatrix();

	for (int i = 0; i < vecGroupHighLevelChildName.size(); i++)
	{
		if (i == 0 ||  minMaxNode.size() == 0)
		{
			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecGroupHighLevelChildName[i]);

			if (!node)
			{
				continue;
			}

			 ObtainGeoRect(node, matrix, minMaxNode);
		}
		else
		{
			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(vecGroupHighLevelChildName[i]);

			if (!node)
			{
				continue;
			}

			std::vector<osg::Vec3d> minMaxNode1;
			bool isSuccess = ObtainGeoRect(node, matrix, minMaxNode1);

			if (isSuccess)
			{
				minMaxNode = findMinMaxNode(minMaxNode, minMaxNode1);
			}
		}

		pos = (int)((double) (i + 1) / (double) vecGroupHighLevelChildName.size() * 300) + 150;
		ISigCallBack->EmitSigSetProgressFun(pos);
	}

	gDem.dx = Xinterval;
	gDem.dy = Yinterval;
	gDem.x0 = minMaxNode[0].x();
	gDem.y0 = minMaxNode[0].y();
	gDem.x1 = minMaxNode[1].x();
	gDem.y1 = minMaxNode[1].y();
	gDem.rows = (gDem.y1 -gDem.y0) / gDem.dy;
	gDem.cols = (gDem.x1 - gDem.x0) / gDem.dx;

	CvMat* demMat = cvCreateMat(gDem.rows, gDem.cols, CV_64FC1);
	cvSetZero(demMat);

	_logop("start produce node DSM");

	for (int i = 0; i < vecGroupHighLevelChildName.size(); i++)
	{
		std::string fileName = vecGroupHighLevelChildName[i];
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(fileName);

		if (!node)
		{
			continue;
		}

		CNodeDemCreator ive(matrix, demMat, gDem, false);
		node->accept(ive);

		pos = (int)((double) (i + 1) / (double) vecGroupHighLevelChildName.size() * 500) + 450;
		ISigCallBack->EmitSigSetProgressFun(pos);
	}

	_logop("start produce repair node DSM");
	//修补空洞dem生成
	std::vector<osg::MatrixTransform*> mRepairMTrans;
	IOsgFinder->FindRepairTransNode(transNode, mRepairMTrans);			//查找修补的trans节点
	
	for (int i = 0; i < mRepairMTrans.size(); i++)
	{
		osg::ref_ptr<osg::Node> repairNode = mRepairMTrans[i]->getChild(0);
		osg::Matrix worldMatrix;
		osg::MatrixList matrixList = repairNode->getWorldMatrices();

		if (matrixList.size() != 0)
		{
			osg::Matrix newMatrix;

			for (int i = 0; i < matrixList.size(); i++)
			{
				newMatrix.postMult(matrixList[i]);
			}

			worldMatrix = newMatrix;
		}

		CNodeDemCreator ive(worldMatrix, demMat, gDem, true);
		repairNode->accept(ive);

		pos = (int)((double) (i + 1) / (double) vecGroupHighLevelChildName.size() * 49) + 950;
		ISigCallBack->EmitSigSetProgressFun(pos);
	}	

	//ISigCallBack->EmitSigEndProgressFun();

	return demMat;
}

void CSceneDemGenerator::obtainOsgFileName(std::string outputPath)						//获取目录下所有带有subtext的osgb文件
{
	//WIN32_FIND_DATA FindFileData; 
	//char nameRule[6] = {0};     
	//char tempname[100] = {0};   
	//int i = 0;  
	//std::string fileRule = outputPath + "*.*";
	//HANDLE hfind = FindFirstFile(fileRule.c_str(), &FindFileData);

	//while( hfind != INVALID_HANDLE_VALUE )      
	//{          
	//	memset(tempname, 0, sizeof(tempname));         
	//	strcpy(tempname, FindFileData.cFileName);         
	//	size_t namelen = strlen(tempname);         
	//	tempname[namelen] = '\0';         
	//	std::string fileTitle = tempname;
	//	i++;         

	//	if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //is directory 
	//	{
	//		memset(tempname, 0, sizeof(tempname));         
	//		strcpy(tempname, FindFileData.cFileName);

	//		if (strcmp(tempname, ".") != 0 && strcmp(tempname, "..") != 0)
	//		{
	//			std::string childFolder = outputPath + tempname + "\\";
	//			obtainOsgFileName(childFolder);
	//		}
	//	}
	//	else
	//	{
	//		int posStart = fileTitle.rfind('.');
	//		int posEnd = fileTitle.length() - 1;
	//		std::string fileExt = fileTitle.substr(posStart + 1, posEnd);

	//		if (fileExt == "osgb")
	//		{
	//			std::string osgFileName = outputPath + fileTitle;
	//			vecFileName.push_back(osgFileName);
	//		}
	//	}

	//	if(!FindNextFile(hfind, &FindFileData))          
	//	{              
	//		hfind = INVALID_HANDLE_VALUE;          
	//	}      
	//}     

	//CloseHandle(hfind); 
}

bool CSceneDemGenerator::ObtainGeoRect(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<osg::Vec3d> &minMaxGeode)    //获取osgb节点
{
	osg::ref_ptr<osg::Vec3Array> extracted_verts;
	InterfaceOsgFinderFactory IFactory;
	InterfaceOsgFinder* IOsgFinder = IFactory.create();
	IOsgFinder->ExtractVerts(node, extracted_verts);
	int size_t = extracted_verts.get()->size() ; 

	if (size_t == 0)
	{
		return false;
	}

	std::vector <osg::Vec3>::iterator iter = extracted_verts.get()->begin();
	double x = iter->x();
	double y = iter->y();
	double z = iter->z();
	osg::Vec3 pt(x, y, z);
	pt = pt * matrix;
	double xMin = pt.x();
	double yMin = pt.y(); 
	double zMin = pt.z();
	double xMax = pt.x();
	double yMax = pt.y();
	double zMax = pt.z();

	for (int i = 0 ; i < size_t; i++)
	{
		double x = iter->x();
		double y = iter->y();
		double z = iter->z();
		osg::Vec3 pt(x, y, z);
		pt = pt * matrix;

		if (xMin > pt.x())
		{
			xMin = pt.x();
		}

		if (yMin > pt.y())
		{
			yMin = pt.y();
		}

		if (zMin > pt.z())
		{
			zMin = pt.z();
		}

		if (xMax < pt.x())
		{
			xMax = pt.x();
		}

		if (yMax < pt.y())
		{
			yMax = pt.y();
		}

		if (zMax < pt.z())
		{
			zMax = pt.z();
		}

		iter++ ; 
	}

	minMaxGeode.push_back(osg::Vec3d(xMin, yMin, zMin));
	minMaxGeode.push_back(osg::Vec3d(xMax, yMax, zMax));
	return true;
}				

std::vector<osg::Vec3d> CSceneDemGenerator::findMinMaxNode(std::vector<osg::Vec3d> minMaxNode, std::vector<osg::Vec3d> minMaxNode1)  //找到两个范围的合并集合
{
	std::vector<osg::Vec3d> minMaxNode2;
	double minX = min(minMaxNode[0].x(), minMaxNode1[0].x());
	double minY = min(minMaxNode[0].y(), minMaxNode1[0].y());
	double minZ = min(minMaxNode[0].z(), minMaxNode1[0].z());
	double maxX = max(minMaxNode[1].x(), minMaxNode1[1].x());
	double maxY = max(minMaxNode[1].y(), minMaxNode1[1].y());
	double maxZ = max(minMaxNode[1].z(), minMaxNode1[1].z());
	minMaxNode2.push_back(osg::Vec3d(minX, minY, minZ));
	minMaxNode2.push_back(osg::Vec3d(maxX, maxY, maxZ));
	return minMaxNode2;
}

std::vector<std::string> CSceneDemGenerator::SearchChildFilePath(std::string searchFilePath)
{
	std::vector<std::string> vecChildFilePath;
	//WIN32_FIND_DATA FindFileData;    
	//char tempname[100] = {0};   
	//int i = 0;  
	//std::vector<std::string> vecChildFilePath;
	//std::string fileRule = searchFilePath + "*.*";
	//HANDLE hfind = FindFirstFile(fileRule.c_str(), &FindFileData);

	//while( hfind != INVALID_HANDLE_VALUE )      
	//{          
	//	memset(tempname, 0, sizeof(tempname));         
	//	strcpy(tempname, FindFileData.cFileName);			
	//	size_t namelen = strlen(tempname);         
	//	tempname[namelen] = '\0';         
	//	std::string fileTitle = tempname;
	//	i++;         

	//	if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //is directory 
	//	{
	//		memset(tempname, 0, sizeof(tempname));         
	//		strcpy(tempname, FindFileData.cFileName);

	//		if (strcmp(tempname, ".") != 0 && strcmp(tempname, "..") != 0)
	//		{
	//			std::string childFolder = searchFilePath + tempname + "\\";
	//			vecChildFilePath.push_back(childFolder);
	//		}
	//	}

	//	if(!FindNextFile(hfind, &FindFileData))          
	//	{              
	//		hfind = INVALID_HANDLE_VALUE;          
	//	}      
	//}     

	//CloseHandle(hfind); 

	return vecChildFilePath;
}

std::vector<std::vector<std::string>> CSceneDemGenerator::SearchChildFileName(std::vector<std::vector<std::string>> vecGroupFilePath)
{
	//WIN32_FIND_DATA FindFileData;    
	//char tempname[100] = {0};   

	std::vector<std::vector<std::string>> vecGroupFileName;

	//for (int i = 0 ; i < vecGroupFilePath.size(); i++)
	//{
	//	std::vector<std::string> vecChildFileName;

	//	for (int j = 0; j < vecGroupFilePath[i].size(); j++)
	//	{
	//		std::string searchChildFilePath = vecGroupFilePath[i][j];
	//		int w = 0;  
	//		std::string fileRule = searchChildFilePath + "*.*";
	//		HANDLE hfind = FindFirstFile(fileRule.c_str(), &FindFileData);

	//		while( hfind != INVALID_HANDLE_VALUE )      
	//		{          
	//			memset(tempname, 0, sizeof(tempname));         
	//			strcpy(tempname, FindFileData.cFileName);			
	//			size_t namelen = strlen(tempname);         
	//			tempname[namelen] = '\0';         
	//			std::string fileTitle = tempname;
	//			w++;         

	//			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //is directory 
	//			{

	//			}
	//			else
	//			{
	//				int posStart = fileTitle.rfind('.');
	//				int posEnd = fileTitle.length() - 1;
	//				std::string fileExt = fileTitle.substr(posStart + 1, posEnd);

	//				if (fileExt == "osgb")
	//				{
	//					std::string osgFileName = searchChildFilePath + fileTitle;
	//					vecChildFileName.push_back(osgFileName);
	//				}
	//			}

	//			if(!FindNextFile(hfind, &FindFileData))          
	//			{              
	//				hfind = INVALID_HANDLE_VALUE;          
	//			}      
	//		}     

	//		CloseHandle(hfind); 

	//	}

	//	vecGroupFileName.push_back(vecChildFileName);
	//}

	return vecGroupFileName;
}

std::vector<std::vector<std::string>> CSceneDemGenerator::SeparateScenenTile(std::vector<std::string> vecChildFilePath, double Xinterval, double Yinterval)
{
	int rowMin = 999999, rowMax = -9999, colMin = 999999, colMax = -99999;

	for (int i = 0; i < vecChildFilePath.size(); i++)
	{
		int rPos = vecChildFilePath[i].rfind('_+');
		int pos = vecChildFilePath[i].find('_+');
		int length = vecChildFilePath[i].length() - 1;
		std::string rowStr = vecChildFilePath[i].substr(pos + 2, rPos);
		std::string colStr = vecChildFilePath[i].substr(rPos + 2, length);
		int row = atoi(rowStr.c_str());
		int col = atoi(colStr.c_str());

		if (rowMin > row)
		{
			rowMin = row;
		}

		if (rowMax < row)
		{
			rowMax = row;
		}

		if (colMin > col)
		{
			colMin = col;
		}

		if (colMax < col)
		{
			colMax = col;
		}
	}

	int rowNum = (int) ceil((double)(rowMax - rowMin + 1) / Yinterval); 
	int colNum = (int) ceil((double)(colMax - colMin + 1) / Xinterval);
	int groupNum = rowNum * colNum;

	std::vector<std::vector<std::string>> vecGroupFilePath;

	for (int i = 0; i < groupNum; i++)
	{
		std::vector<std::string> groupFilePath;
		vecGroupFilePath.push_back(groupFilePath);
	}

	for (int i = 0; i < vecChildFilePath.size(); i++)
	{
		int rPos = vecChildFilePath[i].rfind('_+');
		int pos = vecChildFilePath[i].find('_+');
		int length = vecChildFilePath[i].length() - 1;
		std::string rowStr = vecChildFilePath[i].substr(pos + 2, rPos);
		std::string colStr = vecChildFilePath[i].substr(rPos + 2, length);
		int row = atoi(rowStr.c_str());
		int col = atoi(colStr.c_str());
		int rowIndex = (row - rowMin) / Xinterval;
		int colIndex = (col - colMin) / Yinterval;
		vecGroupFilePath[rowIndex * colNum + colIndex].push_back(vecChildFilePath[i]);
	}

	return vecGroupFilePath;
}

void CSceneDemGenerator::SelectHighLevelLodFileName(std::vector<std::vector<std::string>> vecGroupChildName, std::vector<std::vector<std::string>> &vecGroupHighLevelChildName)
{
	for (int i = 0; i < vecGroupChildName.size(); i++)		
	{
		int maxLevelNum = 0;

		for(int j = 0; j < vecGroupChildName[i].size(); j++)
		{
			int pos = vecGroupChildName[i][j].find('L');
			int rPos = vecGroupChildName[i][j].rfind('_');

			if (pos == -1)
			{
				continue;
			}

			if (rPos == -1)
			{
				continue;
			}

			std::string levelStr = vecGroupChildName[i][j].substr(pos + 1, rPos - pos - 1);

			int levelNum = atoi(levelStr.c_str());

			if (levelNum > maxLevelNum)
			{
				maxLevelNum = levelNum;
			}
		}

		if (maxLevelNum == 0)
		{
			continue;
		}

		std::vector<std::string> vecHighLevelChildName;

		for(int j = 0; j < vecGroupChildName[i].size(); j++)
		{
			int pos = vecGroupChildName[i][j].find('L');
			int rPos = vecGroupChildName[i][j].rfind('_');

			if (pos == -1)
			{
				continue;
			}

			if (rPos == -1)
			{
				continue;
			}

			if (vecGroupChildName[i][j] == "")
			{
				continue;
			}

			std::string levelStr = vecGroupChildName[i][j].substr(pos + 1, rPos - pos - 1);
			int levelNum = atoi(levelStr.c_str());

			if (levelNum == maxLevelNum)
			{
				vecHighLevelChildName.push_back(vecGroupChildName[i][j]);

			}
		}

		vecGroupHighLevelChildName.push_back(vecHighLevelChildName);
	}
}

void CSceneDemGenerator::ObtainImageGeoRect(std::string twfFileName, osg::Vec4d &rect, std::string imageFileName)
{
	//获取范围
	double bottom = 0;
	double left = 0;
	double xRes = 0;
	double yRes = 0;
	double xRotation = 0;
	double yRotation = 0;
	FILE* fp = fopen(twfFileName.c_str(), "r");
	fscanf(fp, "%lf %lf %lf %lf %lf %lf", &xRes, &xRotation, &yRotation, &yRes, &left, &bottom);
	fclose(fp);

	//获取影像
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	GDALDataset* poDataSet = (GDALDataset *)GDALOpen(imageFileName.c_str(), GA_ReadOnly);
	
	int width = poDataSet->GetRasterXSize();
	int height = poDataSet->GetRasterYSize();

	double right = left + width * xRes;
	double top = bottom + height * yRes;

	rect.x() = left;
	rect.y() = right;
	rect.z() = bottom;
	rect.w() = top;
	GDALClose(poDataSet);
}

bool CSceneDemGenerator::GenerateDemForSingleModel(std::string rootModelPath, osg::ref_ptr<osg::MatrixTransform> transNode, std::string outDemFileName, double xRes, double yRes, InterfaceSigEventCallBack* ISigCallBack)
{
	CvMat* demMat = GenerateDem(rootModelPath, transNode, xRes, yRes, ISigCallBack);

	if (demMat == NULL)
	{
		return false;
	}

	_logop("start output DSM");

	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	int height = demMat->height;
	int width = demMat->width;
	int bandCount = 1;
	double *data = new double[height * width * bandCount];
	memset(data, 0, sizeof(double) * height * width * bandCount);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			data[i * width + j] = CV_MAT_ELEM(*demMat, double, i, j);
		}
	}

	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName("Gtiff"); 
	GDALDataset* poDataset = poDriver->Create(outDemFileName.c_str(), width, height, bandCount, GDT_Float64, NULL);

	poDataset->RasterIO(GF_Write, 0, 0, width, height, data, width, height, GDT_Float64, bandCount, NULL, 0, 0, 0);
	GDALClose(poDataset); 
	poDataset = NULL;
	delete []data;  
	data = NULL;
	cvReleaseMat(&demMat);

	_logop("start output dsm rect");

	//输出范围
	int pos = outDemFileName.rfind('.');
	std::string twfFileName = outDemFileName.substr(0, pos) + ".tfw";
	FILE* fp = fopen(twfFileName.c_str(), "w");
	fprintf(fp, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", gDem.dx, 0, 0, -gDem.dy, gDem.x0, gDem.y0 - gDem.dy * height);
	fclose(fp);

	return true;
}

bool CSceneDemGenerator::StartGeneratingDem(std::vector<osg::MatrixTransform*> vecTransNode, std::string rootModelPath, std::string outfileName, double xRes, double yRes, InterfaceSigEventCallBack* ISigCallBack)
{
	bool isSuccess = false;

	if (vecTransNode.size() == 1)
	{
		_logop("start producing Single DSM");
		ISigCallBack->EmitSigShowOpsHint("正在生成DSM...当前数据为第1块，共1块");
		isSuccess = GenerateDemForSingleModel(rootModelPath, vecTransNode[0], outfileName, xRes, yRes, ISigCallBack);
	}
	else if (vecTransNode.size() > 1)
	{
		std::vector<std::string> vecDemTiffFileName;
		std::vector<std::string> vecDemTwfFileName;

		//生成多个block的Dem
		for (int i = 0; i < vecTransNode.size(); i++)
		{
			char strMsg[MAX_PATH];
			sprintf_s(strMsg, "正在生成DSM...当前数据为第%d块，共%d块", i+1,vecTransNode.size());
			ISigCallBack->EmitSigShowOpsHint(strMsg);
			osg::ref_ptr<osg::MatrixTransform> matrixTrans = vecTransNode[i];

			if (!matrixTrans)
			{
				continue;
			}

			//查找trans目录名
			std::string blockFolderName;
			InterfaceOsgFinderFactory IFactory;
			InterfaceOsgFinder* IOsgFinder = IFactory.create();
			IOsgFinder->FindTransFolderName(matrixTrans, blockFolderName);
			
			std::string blockDemFilePath = rootModelPath + blockFolderName + "\\";
			std::string outBlockDemFileName = blockDemFilePath + "dem.tif";
			osg::Matrix matrix = matrixTrans->getMatrix();			

			_logop("start producing Multi-DSM");
			bool isBlockSuccess = GenerateDemForSingleModel(blockDemFilePath, vecTransNode[i], outBlockDemFileName, xRes, yRes, ISigCallBack);

			int pos = outBlockDemFileName.rfind('.') + 1;
			std::string twfBlockFileName = outBlockDemFileName.substr(0, pos) + "tfw";

			if (isBlockSuccess)
			{
				vecDemTiffFileName.push_back(outBlockDemFileName);
				vecDemTwfFileName.push_back(twfBlockFileName);
				isSuccess = true;
			}

			if (i < vecTransNode.size()-1)
			{
				ISigCallBack->EmitSigEndProgressFun();
			}
		}

		if (isSuccess)
		{
			ISigCallBack->EmitSigShowOpsHint("正在合并数据...请稍等");
			std::vector<osg::Vec4d> vecRect;

			//计算出合并后的范围
			for (int i = 0; i < vecDemTwfFileName.size(); i++)
			{
				osg::Vec4d rect;
				ObtainImageGeoRect(vecDemTwfFileName[i], rect, vecDemTiffFileName[i]);
				vecRect.push_back(rect);
			}

			osg::Vec4d maxRect = findMaxRect(vecRect);

			//合并tif
			CombineImage(maxRect, outfileName, vecRect, vecDemTiffFileName, vecDemTwfFileName, xRes, yRes);

			//输出twf
			int pos = outfileName.rfind('.');
			std::string outTwfFileName = outfileName.substr(0, pos) + ".tfw";

			double xRot = 0; double yRot = 0;
			FILE* fpOut = fopen(outTwfFileName.c_str(), "w");
			fprintf(fpOut, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", xRes, xRot, yRot, yRes, maxRect.x(), maxRect.z());
			fclose(fpOut);
		}
	}
	ISigCallBack->EmitSigShowOpsHint("");
	ISigCallBack->EmitSigEndProgressFun();
	return isSuccess;
}

osg::Vec4d CSceneDemGenerator::findMaxRect(std::vector<osg::Vec4d> vecRect)
{
	double xMin = vecRect[0].x();
	double xMax = vecRect[0].y();
	double yMin = vecRect[0].z();
	double yMax = vecRect[0].w();

	for (int i = 1; i < vecRect.size(); i++)
	{
		if (xMin > vecRect[i].x())
		{
			xMin = vecRect[i].x();
		}

		if (xMax < vecRect[i].y())
		{
			xMax = vecRect[i].y();
		}

		if (yMin > vecRect[i].z())
		{
			yMin = vecRect[i].z();
		}

		if (yMax < vecRect[i].w())
		{
			yMax = vecRect[i].w();
		}
	}

	return osg::Vec4d(xMin, xMax, yMin, yMax);
}

void CSceneDemGenerator::CombineImage(osg::Vec4d maxRect, std::string outfileName, std::vector<osg::Vec4d> vecRect, std::vector<std::string> vecDemTiffFileName, std::vector<std::string> vecDemTwfFileName, double xRes, double yRes)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");

	int width = (int) ((maxRect.y() - maxRect.x()) / xRes);
	int height = (int) ((maxRect.w() - maxRect.z()) / yRes);
	int bandCount = 1;

	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName("Gtiff"); 
	GDALDataset* poDataset = poDriver->Create(outfileName.c_str(), width, height, bandCount, GDT_Float64, NULL);
	double* pCombineBuffer =  new double[width * height * bandCount];
	memset(pCombineBuffer, 0, sizeof(double) * width * height * bandCount);

	for (int i = 0; i < vecDemTiffFileName.size(); i++)
	{
		GDALDataset* po = (GDALDataset*) GDALOpen(vecDemTiffFileName[i].c_str(), GA_ReadOnly);
		int xRasterSize = po->GetRasterXSize();
		int yRasterSize = po->GetRasterYSize();
		int bandCount = po->GetRasterCount();
		double* pBuffer = new double[xRasterSize * yRasterSize * bandCount];
		po->RasterIO(GF_Read, 0, 0, xRasterSize, yRasterSize, pBuffer, xRasterSize, yRasterSize, GDT_Float64, bandCount, NULL, 0, 0, 0);
	
		//计算出起始位置
		int rowDiff = (vecRect[i].z() - maxRect.z()) / yRes;
		int colDiff = (vecRect[i].x() - maxRect.x()) / xRes;

		for (int j = 0; j < yRasterSize; j++)
		{
			for (int k = 0; k < xRasterSize; k++)
			{
				int rowImg = j + rowDiff;
				int colImg = k + colDiff;

				if (width - 1< colImg)
				{
					continue;
				}

				if (height - 1< rowImg)
				{
					continue;
				}

				double Height = pBuffer[j * xRasterSize + k];

				if (Height != 0)
				{
					pCombineBuffer[rowImg * width + colImg] = Height;
				}
				
			}
		}
		delete[] pBuffer;
		GDALClose(po);
		po = NULL;
	}

	poDataset->RasterIO(GF_Write, 0, 0, width, height, pCombineBuffer, width, height, GDT_Float64, bandCount, NULL, 0, 0, 0);
	GDALClose(poDataset);
	poDataset = NULL;
	delete[] pCombineBuffer;
}