#include "cvLine.h"

DLL_API void CVLine(std::vector<osg::Vec3d>& data,float& nx,float& ny,float& x,float& y)
{
	
	std::vector<Point2f> points;
	for (size_t i = 0;i < data.size();++i)
	{
		points.push_back(Point2f(data[i].x(), data[i].y()));
	}
	Vec4f line;
	fitLine(Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);

	nx = line[0];
	ny = line[1];
	x = line[2];
	y = line[3];

}

DLL_API void CVPlane(std::vector<osg::Vec3d>& data,double& a,double& b,double& c,double& d)
{
	CvMat*points_mat = cvCreateMat(data.size(), 3, CV_64FC1);//定义用来存储需要拟合点的矩阵   
	for (int i=0;i < data.size(); ++i)  
	{  
		points_mat->data.db[i*3+0] = data[i].x();//矩阵的值进行初始化   X的坐标值  
		points_mat->data.db[i * 3 + 1] = data[i].y();//  Y的坐标值  
		points_mat->data.db[i * 3 + 2] = data[i].z();//  Z的坐标值</span>  
			
	 }  
	 double plane12[4] = { 0 };//定义用来储存平面参数的数组   
	 cvFitPlane(points_mat, plane12);//调用方程   
	 a = plane12[0];
	 b = plane12[1];
	 c = plane12[2];
	 d = -plane12[3];
}

void cvFitPlane(const CvMat* points, double* plane)
{  
	  // Estimate geometric centroid.  
    int nrows = points->rows;  
    int ncols = points->cols;  
	int type = points->type;  
	CvMat* centroid = cvCreateMat(1, ncols, type);  
	cvSet(centroid, cvScalar(0));  
	for (int c = 0; c<ncols; c++)
	{  
        for (int r = 0; r < nrows; r++)  
		{  
		centroid->data.db[c] += points->data.db[ncols*r + c];  
		}  
		centroid->data.db[c] /= nrows;  
	}  
	     // Subtract geometric centroid from each point.  
	CvMat* points2 = cvCreateMat(nrows, ncols, type);  
	for (int r = 0; r<nrows; r++)  
		for (int c = 0; c<ncols; c++)  
		     points2->data.db[ncols*r + c] = points->data.db[ncols*r + c] - centroid->data.db[c];  
	     // Evaluate SVD of covariance matrix.  
	CvMat* A = cvCreateMat(ncols, ncols, type);  
	CvMat* W = cvCreateMat(ncols, ncols, type);  
	CvMat* V = cvCreateMat(ncols, ncols, type);  
	cvGEMM(points2, points, 1, NULL, 0, A, CV_GEMM_A_T);  
	cvSVD(A, W, NULL, V, CV_SVD_V_T);  
	// Assign plane coefficients by singular vector corresponding to smallest singular value.  
	plane[ncols] = 0;  
	for (int c = 0; c<ncols; c++)
	{  
		plane[c] = V->data.db[ncols*(ncols - 1) + c];  
		plane[ncols] += plane[c] * centroid->data.db[c];  
	}  
	    // Release allocated resources.  
	cvReleaseMat(&centroid);  
	cvReleaseMat(&points2);  
	cvReleaseMat(&A);  
	cvReleaseMat(&W);  
	cvReleaseMat(&V);  
}  
