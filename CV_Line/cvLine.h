#pragma once
#include <cv.h>  
#include <osg/Vec3d>  
#include <stdio.h>  
 
#include <math.h>  

using namespace cv;


#ifndef DLL_API
#define DLL_API __declspec(dllexport)
#endif

void cvFitPlane(const CvMat* points, double* plane);

DLL_API void CVLine(std::vector<osg::Vec3d>& data,float& nx,float& ny,float& x,float& y);

DLL_API void CVPlane(std::vector<osg::Vec3d>& data,double& a,double& b,double& c,double& d);