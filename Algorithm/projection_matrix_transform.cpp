#include "stdafx.h"
#include "projection_matrix_transform.hpp"

namespace transMat
{
	//由计算机视觉的R矩阵分解出姿态角，注意先要做转置
	void RtoAngle(double* R, double& phi, double& omega, double& kappa)
	{
		double Rc[9];
		TransposeMatrix(R, 3, 3, Rc);
		//由Rc分解出相机外方位元素的三个角
		omega = asin(-Rc[5]);
		if (abs(Rc[4]) < 1e-15)
		{
			kappa = MY_PI / 2.0;
		}
		else
		{
			kappa = atan(Rc[3] / Rc[4]);
		}

		if (Rc[4] * cos(omega) < 0.0)
		{
			kappa += MY_PI;
		}

		if (abs(Rc[8]) < 1e-15)
		{
			phi = MY_PI / 2.0;
		} 
		else
		{
			phi = atan(-Rc[2] / Rc[8]);
		}

		if (cos(omega) * Rc[8] < 0.0)
		{
			phi += MY_PI;
		}

		if (phi > MY_PI)
		{
			phi -= 2.0 * MY_PI;
		} 
		else if (phi < -MY_PI)
		{
			phi += 2.0 * MY_PI;
		}
		else
		{
			phi = phi;
		}

		if (kappa > MY_PI)
		{
			kappa -= 2.0 * MY_PI;
		} 
		else if (kappa < -MY_PI)
		{
			kappa += 2.0 * MY_PI;
		}
		else
		{
			kappa = kappa;
		}
	}

	//姿态角转计算机视觉的旋转矩阵
	void AngleToR(double f, double w, double k, double* R)
	{
		double a1 = cos(f) * cos(k) - sin(f) * sin(w) * sin(k);
		double a2= -cos(f) * sin(k) - sin(f) * sin(w) * cos(k);
		double a3 = -sin(f) * cos(w);
		double b1 = cos(w) * sin(k);
		double b2 = cos(w) * cos(k);
		double b3 = -sin(w);
		double c1 = sin(f) * cos(k) + cos(f) * sin(w) * sin(k);
		double c2 = -sin(f) * sin(k) + cos(f) * sin(w) * cos(k);
		double c3 = cos(f) * cos(w);

		R[0] = a1; R[1] = b1; R[2] = c1;
		R[3] = a2; R[4] = b2; R[5] = c2;
		R[6] = a3; R[7] = b3; R[8] = c3;
	}

	void CalculateP(double* K, double* R, double* C, double* P)
	{
		double RC[3];
		MatrixMuti1D(R, 3, 3, C, 3, 1, RC);

		double Rt[12];
		Rt[0] = R[0]; Rt[1] = R[1]; Rt[2] = R[2]; Rt[3] = -RC[0];
		Rt[4] = R[3]; Rt[5] = R[4]; Rt[6] = R[5]; Rt[7] = -RC[1];
		Rt[8] = R[6]; Rt[9] = R[7]; Rt[10] = R[8]; Rt[11] = -RC[2];

		MatrixMuti1D(K, 3, 3, Rt, 3, 4, P);
		//用吴常常的结果就不用再转成负的形式了
		////P矩阵转成负的，pmvs支持的形式
		//for (int i = 0; i < 12; ++i)
		//{
		//	P[i] *= -1.0;
		//}
	}

	//多视几何109页
	//P=[M | -MC] = K[R | -RC]
	//M = KR，所以M^-1 = (R^-1) * (K^-1) ＝ (R^t) * (K^-1)
	//故先求M的逆，进行QR分解，得出的Q和R分别求转置和逆就得到R和K了

	//可以证明上三角矩阵的逆还是上三角矩阵：
	//方法1.若T是上三角矩阵，求解线性方程组TS=I，从右下角开始向前求解，可以按分块形式来写
	//S(n,n)=1/T(n,n)
	//S(n,1:n-1)=0
	//S(1:n-1,n)=-T(1:n-1,1:n-1)^{-1}T(1:n-1,n)S(n,n)   ——这块不重要
	//S(1:n-1,1:n-1)=T(1:n-1,1:n-1)^{-1}     ——这个地方用归纳法
	//归纳一下即可。
	//
	//方法2.利用ST=TS=I，忽略等于I的条件，直接可以证明和T可交换的矩阵必定是上三角阵。利用线性性只需要考察i>j时T和E_{i,j}(表示i行j列为1，其余位置为0的矩阵)不可交换。
	void CalculateKRC(double* P, double* K, double* R, double* C)
	{
		//从P取M矩阵
		double M[9];
		for (int i = 0; i < 9; i++)
		{
			M[i] = P[i + (i / 3)];
		}
		//M求逆
		double iM[9];
		InverseMatrix(M, 3, iM);
		//对iM进行QR分解
		double Rt[9];
		double iK[9];
		myQR(iM, 3, Rt, iK);
		//恢复K和R
		TransposeMatrix(Rt, 3, 3, R);
		InverseMatrix(iK, 3, K);

		//提取C
		double P4[3];
		P4[0] = P[3];
		P4[1] = P[7];
		P4[2] = P[11];
		MatrixMuti1D(iM, 3, 3, P4, 3, 1, C);
		C[0] *= -1.0;
		C[1] *= -1.0;
		C[2] *= -1.0;
	}

	void CalculateK(double f, double x0, double y0, double* K)
	{
		memset(K, 0.0, 9 * sizeof(double));
		K[0] = -f;
		K[4] = f;
		K[8] = 1.0;	
		K[2] = x0;
		K[5] = y0;
	}

	void SixParmToP(double f, double x0, double y0, CoordDouble3D& xyz, CoordDouble3D& fwk, double* P)
	{
		double R[9];
		AngleToR(fwk.x, fwk.y, fwk.z, R);
		double C[3];
		C[0] = xyz.x;
		C[1] = xyz.y;
		C[2] = xyz.z;
		double K[9];
		CalculateK(f, x0, y0, K);
		CalculateP(K, R, C, P);
	}

	void Point3dToPoint2d(double* P, CoordDouble3D xyz, CoordDouble2D& xy)
	{
		double p4[4];
		p4[0] = xyz.x;
		p4[1] = xyz.y;
		p4[2] = xyz.z;
		p4[3] = 1.0;
		double p3[3];
		MatrixMuti1D(P, 3, 4, p4, 4, 1, p3);
		xy.x = p3[0] / p3[2];
		xy.y = p3[1] / p3[2];
	}


	//由f和R、C计算ccd中心的坐标
	extern void CalculateCCDCenter(double f, double* R, double* C, CoordDouble3D& xyz_ccd_center)
	{
		double iR[9];
		InverseMatrix(R, 3, iR);
		double iRX[3];
		double X[3] = {0.0, 0.0, -f};
		MatrixMuti1D(iR, 3, 3, X, 3, 1, iRX);
		xyz_ccd_center.x = iRX[0] + C[0];
		xyz_ccd_center.y = iRX[1] + C[1];
		xyz_ccd_center.z = iRX[2] + C[2];
	}
}
