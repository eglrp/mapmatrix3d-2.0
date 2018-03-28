
#include <string>
#include <iostream>
#include <strstream>
#include <osg/Group>
#include <map>

using namespace std;

#ifdef DLL_SHAPEFILE_APILIBDLL  
#define DLL_TO_SHAPEFILE_API _declspec(dllexport)  
#else  
#define DLL_TO_SHAPEFILE_API  _declspec(dllimport)  
#endif 

namespace toShpAndDxf
{
	DLL_TO_SHAPEFILE_API void convertToShp(vector<vector<osg::Vec3d> > &vec,string filename,string str);
	DLL_TO_SHAPEFILE_API void convertToDxf(map<string, vector<vector<osg::Vec3d> > > &vec, string filename, string str);

	void modifyDxfFile(std::string & filename);
}
