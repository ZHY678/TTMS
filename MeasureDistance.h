
#pragma once
#define _USE_MATH_DEFINES

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <cmath>
#include <iostream>
#include <fstream>

#include "CameraCalib.h"
#include "Utils.h"

using namespace std;
using namespace cv;

//测量边界参数
#define CENTER_FLAG_GRADIENT 1
#define CENTER_FLAG_STEGER 2
#define CENTER_FLAG_E 3


struct Match
{
	Point2f left;
	Point2f right;
	double distance;
	Match(){}
	Match(Point2f left, Point2f right, double d) :left(left), right(right), distance(d){}
	Match(const Match& m) :left(m.left), right(m.right), distance(m.distance){}

};


class MeasureDistance
{
public:

	MeasureDistance();
	~MeasureDistance();
	int R_L, R_L2;
	double MEAS_MAX_Z ;
	double MEAS_VALI_MIN_Z;
	double MEAS_VALI_MAX_Z;
	double MEAS_VALI_MIN_Y;
	double MEAS_VALI_MAX_Y;
	bool Test_Calibrate;
	/*----------------------------
	* 功能 : 提取红色激光线
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : src			[in]	原图像
	* 参数 : dst			[out]	输出图像
	* 参数 : red_points	[out]	图像中的所有红色点
	*/
	float getThresh(vector<float>& vData);

	/*----------------------------
	* 功能 : 提取中心线
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : src			[in]	原图像
	* 参数 : dst			[out]	输出图像
	* 参数 : vSubCenterPoint	[out]	中心线
	*/
	bool DetectLinerCenterHor(cv::Mat &src, cv::Mat &dst, vector<cv::Point2f>& vSubCenterPoint);
	
	/*----------------------------
	* 功能 : steger方法提取中心线
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : pSrcImg			[in]	原图像
	* 参数 : centerPoint	[out]	中心线
	*/
	void stegerCenter(IplImage* pSrcImg, IplImage* pBinImg, vector<Point2f> &centerPoint, vector<int> &lineWidth, int kind);
	bool GetDisparityRIO(cv::Mat& image1,Point3f &a, int Up_Down,int left_right);
	bool fittingCurve(vector<Point2d> &vec, int times, float *p);
	bool fitting_intrest(vector<Point2f> &pointsIn, vector<Point2f>&pointsOut);//P1-P5 fitting
	//void MeasureDistance::yuchuli(Mat&downMat);
	void  gaussianfilter(vector<float>& signal, vector<float>& result, float sigma);
	/*----------------------------
	* 功能 : 使用极线约束匹配左右两幅图像激光线特征
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : leftImg			[in]	左图像
	* 参数 : rightImg		[in]	右图像
	* 参数 : lefts		    [in]	左图激光点
	* 参数 : rights		    [in]	右图激光点
	* 参数 : matches		    [out]	匹配结果
	* 参数 ：matchMat		[out]	匹配特征点mat  
	* 参数 : threshold	    [in]	阈值
	*/
	bool match(Mat& leftImg, Mat& rightImg, Mat& F, vector<Point2f>& lefts, vector<Point2f>& rights, vector<Match>& matches, Mat& matchMat, double threshold = 5.0);
	//对Y轴方向上进行插值，优化匹配结果
	void interWithY(vector<Point2f>& lefts, vector<Point2f>& rights);

	/*----------------------------
	* 功能 : 重建出三维坐标
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : M1			[in]	左相机内参
	* 参数 : M2			[in]	右相机内参
	* 参数 : R		    [in]	选择矩阵
	* 参数 : t		    [in]	平移向量
	* 参数 : matches		    [in]	特征点匹配
	* 参数 : real_points	    [out]	三维空间坐标
	*/
	bool reprojectTo3D(Mat& M1, Mat& M2, Mat& R, Mat& t, vector<Match> matches, vector<Point3d>& real_points);
	//最小二乘法求解三维空间坐标
	bool reprojectTo3D_LSM(Mat& M1, Mat& M2, Mat& R, Mat& t, vector<Match> matches, vector<Point3d>& real_points);
	bool reprojectTo3D_ARC(Mat& M1, Mat& R, Mat& t, vector<Point2f> input, vector<Point3d>& real_points);

	/*----------------------------
	* 功能 : 计算距离
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : img1			[in]	第一幅图像
	* 参数 : img2			[in]	第二幅图像
	* 参数 : calibResult		    [in]	相机标定参数
	* 参数 : real_points         [out]  重建出的三维坐标点
	* 参数 : outLeftCenterMat    [out]  左图中心线
	* 参数 : outRightCenterMat   [out]  右图中心线
	* 参数 : matchCenterMat      [out]  特征点匹配图
	*/
	bool caculateDistance(Mat& left_img, Mat& right_img, const CameraCalib::CalibrationParames& calibResult, vector<Point3d>& real_points, Mat& outLeftCenterMat, Mat& outRightCenterMat, Mat& matchCenterMat, int Up_Down, int kind, int CENTER_FLAG = CENTER_FLAG_STEGER);
	bool caculateDistance2(vector<Mat>& left_img, vector<Mat>& right_img, const CameraCalib::CalibrationParames& calibResult, vector<Point3d>& real_points, Mat& outLeftCenterMat, Mat& outRightCenterMat, Mat& matchCenterMat, int Up_Down, int kind, int CENTER_FLAG = CENTER_FLAG_STEGER);
	void morphology_corrode(vector<float>&input, vector<float>&output, int scale,int k);
	void morphology_dilate(vector<float>&input, vector<float>&output, int scale,int k);
	void morphology_oc(vector<float>&input, vector<float>&output,int k);

	bool caculateDistance_ARC(Mat& img, const CameraCalib::CalibrationParames& calibResult, vector<Point3d>& real_points, Mat& outCenterMat, int CENTER_FLAG);

	/*----------------------------
	* 功能 : 预处理三维点
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*/
	bool deal3DPoints(const vector<Point3d>& real_points, vector<Point2d>& points2D, const Mat& rotateMat, double camHeigh);
	bool do2DPointsOpt(const vector<Point2d>& inPoints2D, vector<Point2d>& outPoints2D);

	/*----------------------------
	* 功能 : //处理2D数据点，增加旋转和平移的修改
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*/
	bool deal2DPoints(vector<Point2d>& points2D, Mat* const rotateMat, Mat* const translation);
	//theta为一角度，xT为x轴方向平移位移，yT为y轴方向平移位移
	bool deal2DPoints(vector<Point2d>& points2D, double theta, double xT, double yT);

	bool fillyzList(vector<Point2d>& points2D, vector<double>& ylist, vector<double>& zlist);

	/*----------------------------
	* 功能 : 轨检几何参数测量
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*/
	bool MeasureDistance::measureA1A2(vector<Point2d> points, double &a1_maxx, double &a1_maxy, double &a2_minx, double &a2_miny, double &a2, vector<Point2d> &pointsA1, vector<Point2d>& pointsA2, vector<Point2d>&pointsA12);
	bool MeasureDistance::measureA3A4(vector<Point2d> points, double &a3, double &a4, double &a3_min, double &a4_min, vector<Point2d> &pointsA3, vector<Point2d>& pointsA4, vector<Point2d>& pointsA5);
	bool MeasureDistance::measureA3A4(vector<Point2d> points, double &a3, double &a4, double &a3_min, double &a4_min, vector<Point2d> &pointsA3, vector<Point2d>& pointsA4, vector<Point2d>& pointsA5, double &a2_minx1, double &a2_miny);
	bool measureA1A2_NEW(vector<Point2d> points, double &a1_maxx, double &a1_maxy, double &a2_minx, double &a2_miny, double &a2, vector<Point2d> &pointsA1, vector<Point2d>& pointsA2, vector<Point2d>&pointsA12);
	bool MeasureDistance::fitting(vector<Point2d> &pointsIn, vector<Point2d>&pointsOut, int m);
	//bool MeasureDistance::fitting(vector<Point2d> &pointsIn, vector<Point2d>&pointsOut);

	bool MeasureDistance::fitting(vector<Point2d> &pointsIn, vector<Point2d>&pointsOut);
	bool MeasureDistance::fitting_med(vector<Point2f> &pointsIn, vector<Point2f>&pointsOut);
	void MeasureDistance::medianfilter(vector<float>& signal, vector<float>& result,int N);
	//void MeasureDistance::median_filter(vector<int>vec, vector<int>temp, int scale);
	//void MeasureDistance::median_filter(vector<uchar>VecInput, vector<uchar>&VecOutput);
	bool MeasureDistance::DetectLinerCenterM(cv::Mat &src, cv::Mat &dst, vector<cv::Point2f>& vSubCenterPoint, Point3f&a, vector<int> &lineWidth);
	
	/*----------------------------
	* 功能 : 主界面测量
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*/
	bool cacul(const vector<Mat>& measImgs, const vector<Mat>& measImgs2, const CameraCalib::CalibrationParames& upCalibParams, const CameraCalib::CalibrationParames& downCalibParams,
		vector<Point3d>& upRealPoints, vector<Point3d>& downRealPoints);
	bool cacul(const vector<Mat>& measImgs, const CameraCalib::CalibrationParames& upCalibParams, const CameraCalib::CalibrationParames& downCalibParams,
		vector<Point3d>& upRealPoints, vector<Point3d>& downRealPoints);

	bool MeasureDistance::insert(vector<Point2f>&Inpoint);

private:

};
