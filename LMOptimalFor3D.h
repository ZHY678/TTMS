#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
using namespace std;
class LMOptimalFor3D
{
public:

	LMOptimalFor3D(void);
	virtual ~LMOptimalFor3D(void);

public:
	/********************************************
	para  待优化的参数;
	m     误差方程的个数;
	n     
	**********************************************/
	void TNFP_LM(double *para, int m, int n, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints);
	int lmdif_(void (*fcn)(int* m,int* n,double* x,double* fvec, int* iflag, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints), 
		int* m, int* n, double* x, double* fvec, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints, 
		double* ftol, double* xtol, double* gtol, int* maxfev, 
		double* epsfcn, double*diag, int* mode, double* factor, 
		int* nprint, int* info, int* nfev, double* fjac, int*ldfjac,
		int* ipvt,double* qtf, double* wa1, 
		double* wa2, double* wa3, double* wa4);  
	/* Subroutine */
	int qrfac_(int* m, int* n, double* a, int* lda,
		int* pivot, int* ipvt, int* lipvt, double* rdiag,
		double* acnorm,double* wa);
	int lmpar_(int* n, double* r, int* ldr, int* ipvt, 
		double* diag, double*qtb, double* delta, double* par,
		double* x, double* sdiag,double* wa1, double* wa2);
	double enorm_(int* n, double* x);
	int fdjac2_(void(*fcn)(int* a,int* b,double* c,double* d,int* e, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints), int* m, int* n, double* x,
		double* fvec, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectPoints,double* fjac, int* ldfjac, 
		int* iflag, double* epsfcn,double* wa);
	double dpmpar_(int* i);
	int qrsolv_(int* n, double* r, int* ldr, int* ipvt, double* diag, 
		double* qtb, double* x, double* sdiag, double* wa);
	static void Costfunction(int *m_ptr, int *n_ptr, double *param, double *err, int *iflag, double* fdis, vector<vector<cv::Point2f>> pImgL, vector<vector<cv::Point2f>> pImgR, vector<cv::Point3f> ObjectP);
	static double Cal2Dis(cv::Point3f point_1, cv::Point3f point_2);
};

