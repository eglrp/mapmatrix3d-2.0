#include "StdAfx.h"
#include "toShpAndDxf.h"
#include <conio.h>
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_string.h" 
#include <ogrsf_frmts/ogrsf_frmts.h>
#include <fstream>
namespace toShpAndDxf
{

	DLL_TO_SHAPEFILE_API void convertToDxf(map<string, vector<vector<osg::Vec3d> > > &vec, string filename, string str)
		//DLL_API void convertToDxf(vector<vector<osg::Vec3d> > &vec,string filename,string str)
	{
		if (vec.empty())
		{
			//AfxMessageBox(_T("未发现特征线！"));
			return;
		}
		/*if (str!="hatching"&&str!="outline")
		{
		return;
		}*/

		const char* outputFile = filename.c_str();

		vector<osg::Vec3d> vec1;
		//设置GDAL_DATA目录  
		CPLSetConfigOption("GDAL_DATA", "..\\data");
		//定义DXF的驱动  
		const char *pszDriverName = "DXF";
		//注册OGR驱动  
		OGRRegisterAll();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
		OGRSFDriver *poDriver;
		poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
		if (poDriver == NULL)
		{
			return;
		}
		OGRDataSource *poDS; //创建文件  

		poDS = poDriver->CreateDataSource(outputFile, NULL);



		if (poDS == NULL)
		{
			return;
		}
		OGRLayer *poLayer; //创建一个图层，估计DXF可以创建很多个图层  

		poLayer = poDS->CreateLayer("line", NULL, wkbLineString, NULL);

		if (poLayer == NULL)
		{
			return;
		}

		//创建一个要素，由于DXF不支持属性表，就不创建属性表了  
		OGRFeature *poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
		//poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );  

		//如果是剖面线
		//if(str == "hatching")//创建一条线段 
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
		/*poFeature->SetField("Layer", "剖面线");
		{
		for(vector<vector<osg::Vec3d> >::iterator it = vec.begin();it!=vec.end();++it)
		{
		OGRLineString line  ;
		vec1 = *it;
		for (std::vector<osg::Vec3d>::iterator ite = vec1.begin(); ite!=vec1.end(); ++ite)
		{
		line.addPoint((*ite).x(), (*ite).y(),(*ite).z());
		}
		poFeature->SetGeometry(&line);
		if ( poLayer->CreateFeature( poFeature) != OGRERR_NONE)
		{
		return;
		}
		}
		}*/

		for (map<string, vector<vector<osg::Vec3d> > >::iterator ite = vec.begin(); ite != vec.end(); ++ite)
		{


			for (int i = 0; i < (*ite).second.size(); ++i)
			{
				poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
				OGRLineString line;
				for (int j = 0; j< ((*ite).second)[i].size(); ++j)
				{
					line.addPoint(((*ite).second)[i][j].x(), ((*ite).second)[i][j].y(), ((*ite).second)[i][j].z());
				}
				poFeature->SetField("Layer", (*ite).first.c_str());
				poFeature->SetGeometry(&line);
				if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
				{
					continue;
				}
			}
		}


		/*for (size_t i = 0;i<vec.size();++i)
		{

		poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
		OGRLineString line;
		for (std::vector<osg::Vec3d>::iterator ite = vec[i].begin(); ite != vec[i].end(); ++ite)
		{
		line.addPoint((*ite).x(), (*ite).y(), (*ite).z());
		}
		string s1 = "剖面线";
		s1 += to_string(i);
		poFeature->SetField("Layer", s1.c_str());
		poFeature->SetGeometry(&line);
		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
		{
		return;
		}
		}*/

		//如果是轮廓线
		//else if (str == "outline")//创建一个多边形
		//{
		//	for(vector<vector<osg::Vec3d> >::iterator it = vec.begin();it!=vec.end();++it)
		//	{
		//		OGRLineString line  ;  
		//		vec1 = *it;
		//		for (std::vector<osg::Vec3d>::iterator ite = vec1.begin(); ite!=vec1.end(); ++ite)  
		//		{   	
		//			line.addPoint((*ite).x(), (*ite).y(),(*ite).z()); 
		//		}
		//		std::vector<osg::Vec3d>::iterator ite = vec1.begin();
		//		line.addPoint((*ite).x(), (*ite).y(),(*ite).z()); 
		//		poFeature->SetGeometry(&line);  
		//		if ( poLayer->CreateFeature( poFeature) != OGRERR_NONE)  
		//		{  
		//			return;  
		//		}  
		//	}
		//}

		OGRFeature::DestroyFeature(poFeature);
		OGRDataSource::DestroyDataSource(poDS);

		//修改dxf文件  
		modifyDxfFile(filename);
	}

	void modifyDxfFile(std::string & filename)
	{
		std::fstream file(filename);
		string line;

		ofstream outfile("temp.txt", ios::out | ios::trunc);

		while (!file.eof())
		{
			getline(file, line);
			if (line != "ANSI_1252")//如果要修改内容就在这修改line的内容,再存到文件中就行了
				outfile << line << endl;
			else
				outfile << "ANSI_936" << endl;
		}
		file.close();
		outfile.close();

		ofstream outfile1(filename, ios::out | ios::trunc);
		fstream file1("temp.txt");
		while (!file1.eof())
		{
			getline(file1, line);
			outfile1 << line << endl;
		}
		outfile1.close();
		file1.close();

		remove("temp.txt");
	}

	DLL_TO_SHAPEFILE_API void convertToShp(std::vector<vector<osg::Vec3d> > &vec, string filename, string str)
	{
		if (vec.empty())
		{
			//AfxMessageBox(_T("未发现特征线！"));
			return;
		}
		if (str != "hatching" && str != "outline")
		{
			return;
		}

		const char* outputFile = filename.c_str();
		vector<osg::Vec3d> vec1;
		const char *pszDriverName = "ESRI Shapefile";
		OGRSFDriver *poDriver;

		OGRRegisterAll();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "YES");

		poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
		if (poDriver == NULL)
		{
			return;
		}

		OGRDataSource *poDS;

		poDS = poDriver->CreateDataSource(outputFile, NULL);

		if (poDS == NULL)
		{
			return;
		}

		OGRLayer *poLayer;

		poLayer = poDS->CreateLayer("line", NULL, wkbLineString, NULL);

		if (poLayer == NULL)
		{
			return;
		}
		//下面创建属性表  
		OGRFieldDefn poFieldID("ID", OFTInteger);//创建ID字段  
		poFieldID.SetWidth(10);
		poLayer->CreateField(&poFieldID);

		OGRFieldDefn poFieldX("X", OFTString);//创建Name字段  
		poFieldX.SetWidth(30);
		poLayer->CreateField(&poFieldX);

		OGRFieldDefn poFieldY("Y", OFTString);//创建Name字段  
		poFieldY.SetWidth(30);
		poLayer->CreateField(&poFieldY);

		OGRFieldDefn poFieldZ("Z", OFTString);//创建Name字段  
		poFieldZ.SetWidth(30);
		poLayer->CreateField(&poFieldZ);
		OGRFeature *poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

		if (str == "hatching")
		{
			for (vector<vector<osg::Vec3d> >::iterator it = vec.begin(); it != vec.end(); ++it)
			{
				OGRLineString line;
				vec1 = *it;
				int i = 0;
				for (std::vector<osg::Vec3d>::iterator ite = vec1.begin(); ite != vec1.end(); ++ite)
				{
					poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
					poFeature->SetField(0, i++);
					poFeature->SetField(1, (*ite).x());
					poFeature->SetField(2, (*ite).y());
					poFeature->SetField(3, (*ite).z());
					poLayer->CreateFeature(poFeature);
					line.addPoint((*ite).x(), (*ite).y(), (*ite).z());
				}
				poFeature->SetGeometry(&line);
				if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
				{
					return;
				}
			}
		}
		else if (str == "outline")
		{
			for (vector<vector<osg::Vec3d> >::iterator it = vec.begin(); it != vec.end(); ++it)
			{
				OGRLineString line;
				vec1 = *it;
				int i = 0;
				for (std::vector<osg::Vec3d>::iterator ite = vec1.begin(); ite != vec1.end(); ++ite)
				{
					poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
					poFeature->SetField(0, i++);
					poFeature->SetField(1, (*ite).x());
					poFeature->SetField(2, (*ite).y());
					poFeature->SetField(3, (*ite).z());
					poLayer->CreateFeature(poFeature);
					line.addPoint((*ite).x(), (*ite).y(), (*ite).z());
				}
				std::vector<osg::Vec3d>::iterator ite = vec1.begin();
				poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
				poFeature->SetField(0, i++);
				poFeature->SetField(1, (*ite).x());
				poFeature->SetField(2, (*ite).y());
				poFeature->SetField(3, (*ite).z());
				poLayer->CreateFeature(poFeature);
				line.addPoint((*ite).x(), (*ite).y(), (*ite).z());
				poFeature->SetGeometry(&line);
				if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
				{
					return;
				}
			}
		}
		OGRFeature::DestroyFeature(poFeature);
		OGRDataSource::DestroyDataSource(poDS);
	}
}