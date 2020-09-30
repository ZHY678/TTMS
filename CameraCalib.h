

#pragma once

#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <fstream>

//#include "LMOptimalFor3D.h"

using namespace std;
using namespace cv;

class CameraCalib
{
public:
	CameraCalib();
	~CameraCalib();


	/***
	*	棋盘角点数据 结构体
	*/
	struct CornerDatas
	{
		int			nImages;			// 棋盘图像数
		int			nPointsPerImage;	// 每幅棋盘的角点数
		cv::Size 	imageSize;			// 图像分辨率
		cv::Size	    boardSize;			// 棋盘尺寸
		vector<vector<cv::Point3f> >	objectPoints;	// 棋盘角点世界坐标序列
		vector<vector<cv::Point2f> >	leftImgPoints;	// 左视图的棋盘角点像素坐标序列
		vector<vector<cv::Point2f> >	rightImgPoints;	// 右视图的棋盘角点像素坐标序列
	};


	/***
	*	标定输出参数
	*/
	struct CalibrationParames
	{
		cv::Mat  	mLeftCameraInParas          = cv::Mat::zeros(3, 3, CV_32FC1);       // 左摄像机内部标定参数（3*3）;
		cv::Mat  	mRightCameraInParas         = cv::Mat::zeros(3, 3, CV_32FC1);       // 右摄像机内部标定参数（3*3）;
		cv::Mat  	mLeftCameraDistorParas      = cv::Mat::zeros(1, 5, CV_32FC1);       // 左相机的畸变参数(1*5);
		cv::Mat  	mRightCameraDistorParas     = cv::Mat::zeros(1, 5, CV_32FC1);       // 右相机的畸变参数(1*5);

		double      dLeftCameraFocalLen         = 0;          // 左相机的焦距，由标定参数计算得到;
		double      dRightCameraFocalLen        = 0;          // 右相机的焦距，由标定参数计算得到;

		cv::Mat  	mL2RRotation                = cv::Mat::zeros(3, 1, CV_32FC1);	            // 旋转矩阵(3*1);
		cv::Mat 	mL2RTranslation             = cv::Mat::zeros(3, 1, CV_32FC1);	            // 平移向量(3*1);
		cv::Mat  	mL2RRotation33              = cv::Mat::zeros(3, 3, CV_32FC1);		        // 旋转矩阵(3*3);

		cv::Mat  	mSsential                   = cv::Mat::zeros(3, 3, CV_32FC1);		        // 本质矩阵;
		cv::Mat 	mFoundational               = cv::Mat::zeros(3, 3, CV_32FC1);	            // 基础矩阵;(3*3)    R = F*L    (2) = F*(1);
		double      dErr                        = 0;               //反投影残差;
		BOOL	    bIsBinocularCameraCalibration = 1;	           // 双目标定所用的标志位;
		double      dCaliratebError             = 0;               //标定误差;
		CString     strCalibrateTime            = _T("");          //标定时间;

		CString      sCaliResultFilePath        = _T("");          //单目相机标定结果存储路径;
		int       dBoardWidth;          //标定板参数，每行多少角点
		int       dBoardHeight;         //标定板参数，每列多少角点
		double       dSquareSize;		   //标定板棋格实际尺寸
		double       dPixelSize;           // 相机的像元尺寸，可以从相机的说明书上查询;

		cv::Mat      amendRotMat = cv::Mat::eye(3,3,CV_32FC1);          //摄像机修正旋转矩阵
		cv::Mat     trianRotation;             //三角测量法标定的旋转矩阵
		cv::Mat     trianTanslation;           //三角测量法标定的平移向量 

		BOOL WriteYMLFile(int num=1)
		{
			if (!PathFileExists(sCaliResultFilePath))  //如果该路径不存在则新建该路径;
			{
				CreateDirectory(sCaliResultFilePath, NULL);
			}

			if (bIsBinocularCameraCalibration)
			{
				CString strBinoCali;
				strBinoCali.Format(_T("%s\\BinocularCaliResult_%d.yml"), sCaliResultFilePath, num);

				string sCaliResultPath((LPCSTR)CStringA(strBinoCali));
				cv::FileStorage fs(sCaliResultPath, cv::FileStorage::WRITE);

				if (!fs.isOpened())
				{
					AfxMessageBox(_T("标定参数输出文件时发生错误！"));
					return FALSE;
				}

				time_t rawtime;
				time(&rawtime);
				fs << "calibrationDate" << asctime(localtime(&rawtime));
				fs << "dBoardHeight" << dBoardHeight;
				fs << "dBoardWidth" << dBoardWidth;
				fs << "dSquareSize" << dSquareSize;
				fs << "dPixelSize" << dPixelSize;
				fs << "amendRotMat" << amendRotMat;
				fs << "mLeftCameraInParas" << mLeftCameraInParas << "mLeftCameraDistorParas" << mLeftCameraDistorParas;
				fs << "mRightCameraInParas" << mRightCameraInParas << "mRightCameraDistorParas" << mRightCameraDistorParas;
				fs << "dLeftCameraFocalLen" << dLeftCameraFocalLen << "dRightCameraFocalLen" << dRightCameraFocalLen;
				fs << "mL2RRotation33" << mL2RRotation33 << "mL2RTranslation" << mL2RTranslation;
				fs << "mSsential" << mSsential << "mFoundational" << mFoundational;
				fs << "dCaliratebError" << dCaliratebError;

				//保存表定时间（由于opencv的FileStorage类中没有关于CString的重载，因此将CSring转换成string）;
				string sCaliTime((LPCSTR)CStringA(strCalibrateTime));

				fs << "strCalibrateTime" << sCaliTime;

				fs.release();
			}
			if (!bIsBinocularCameraCalibration)
			{
				CString strBinoCali;
				strBinoCali.Format(_T("%s\\SingleCaliResult.yml"), sCaliResultFilePath);

				string sCaliResultPath((LPCSTR)CStringA(strBinoCali));
				cv::FileStorage fs(sCaliResultPath, cv::FileStorage::WRITE);

				if (!fs.isOpened())
				{
					AfxMessageBox(_T("标定参数输出错误！"));
					return FALSE;
				}

				time_t rawtime;
				time(&rawtime);
				fs << "calibrationDate" << asctime(localtime(&rawtime));
				fs << "dBoardHeight" << dBoardHeight;
				fs << "dBoardWidth" << dBoardWidth;
				fs << "dSquareSize" << dSquareSize;
				fs << "dPixelSize" << dPixelSize;
				fs << "mCameraInParas" << mLeftCameraInParas;
				fs << "mCameraDistorParas" << mLeftCameraDistorParas;
				fs << "mTrianRotation" << trianRotation;
				fs << "mTrianTanslation" << trianTanslation;
				fs << "dCameraFocalLen" << dLeftCameraFocalLen;
				fs << "dCaliratebError" << dCaliratebError;
				//保存标定时间（由于opencv的FileStorage类中没有关于CString的重载，因此将CSring转换成string）;
				string sCaliTime((LPCSTR)CStringA(strCalibrateTime));
				fs << "strCalibrateTime" << sCaliTime;

				fs.release();
			}
			return TRUE;
		}

		BOOL ReadYMLFile(int num=1)
		{
			if (!PathFileExists(sCaliResultFilePath))  //如果该路径不存在则报错;
			{
				//AfxMessageBox(_T("标定文件不存在！"));
				return FALSE;
			}

			if (bIsBinocularCameraCalibration)
			{
				CString strBinoCali;
				strBinoCali.Format(_T("%s\\BinocularCaliResult_%d.yml"), sCaliResultFilePath, num);

				string sCaliResultPath((LPCSTR)CStringA(strBinoCali));
				cv::FileStorage fs(sCaliResultPath, cv::FileStorage::READ);
				if (!fs.isOpened())
				{
					AfxMessageBox(_T("标定参数读取失败！"));
					return FALSE;
				}

				fs["dBoardWidth"] >> dBoardWidth;
				fs["dBoardHeight"] >> dBoardHeight;
				fs["dSquareSize"] >> dSquareSize;
				fs["dPixelSize"] >> dPixelSize;
				fs["amendRotMat"] >> amendRotMat;
				fs["mLeftCameraInParas"] >> mLeftCameraInParas;
				fs["mLeftCameraDistorParas"] >> mLeftCameraDistorParas;
				fs["mRightCameraInParas"] >> mRightCameraInParas;
				fs["mRightCameraDistorParas"] >> mRightCameraDistorParas;
				fs["dLeftCameraFocalLen"] >> dLeftCameraFocalLen;
				fs["dRightCameraFocalLen"] >> dRightCameraFocalLen;

				fs["mL2RRotation33"] >> mL2RRotation33;
				fs["mL2RTranslation"] >> mL2RTranslation;
				fs["mSsential"] >> mSsential;
				fs["mFoundational"] >> mFoundational;
				fs["dCaliratebError"] >> dCaliratebError;

				//读取标定时间（由于opencv的FileStorage类中没有关于CString的重载，因此将CSring转换成string）;
				string sCaliTime;
				fs["strCalibrateTime"] >> sCaliTime;
				strCalibrateTime = sCaliTime.c_str();
			}
			if (!bIsBinocularCameraCalibration)
			{
				CString strBinoCali;
				strBinoCali.Format(_T("%s\\SingleCaliResult.yml"), sCaliResultFilePath);

				string sCaliResultPath((LPCSTR)CStringA(strBinoCali));
				cv::FileStorage fs(sCaliResultPath, cv::FileStorage::READ);
				if (!fs.isOpened())
				{
					AfxMessageBox(_T("标定参数读取失败！"));
					return FALSE;
				}

				fs["dBoardWidth"] >> dBoardWidth;
				fs["dBoardHeight"] >> dBoardHeight;
				fs["dSquareSize"] >> dSquareSize;
				fs["dPixelSize"] >> dPixelSize;
				fs["mCameraInParas"] >> mLeftCameraInParas;
				fs["mCameraDistorParas"] >> mLeftCameraDistorParas;
				fs["mTrianRotation"] >> trianRotation;
				fs["mTrianTanslation"] >> trianTanslation;
				fs["dCaliratebError"] >> dCaliratebError;
				fs["dCameraFocalLen"] >>dLeftCameraFocalLen;

			}


			return TRUE;
		}
	};

	

	/*----------------------------
	* 功能 : 初始化棋盘角点数据信息
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : nImages		[in]	棋盘图像数
	* 参数 : imageSize		[in]	图像分辨率
	* 参数 : boardSize		[in]	棋盘角点数
	* 参数 : squareWidth	[in]	棋盘方块宽度
	* 参数 : cornerDatas	[out]	初始化后的棋盘角点数据
	*/
	int initCornerData(int nImages, cv::Size imageSize, cv::Size boardSize, float squareWidth, CornerDatas& cornerDatas);

	/*----------------------------
	* 功能 : 检测棋盘角点
	*----------------------------
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : img1			[in]	左视图
	* 参数 : img2			[in]	右视图
	* 参数 : cornerDatas	[i/o]	待导出的棋盘角点数据
	* 参数 : imageCount		[in]	当前棋盘角点检测的成功次数
	*/
	int detectCorners(cv::Mat& img1, cv::Mat& img2, CornerDatas& cornerDatas, int imageCount);

	/*----------------------------
	* 功能 : 保存已标定好的摄像机内部参数
	*----------------------------
	* 函数 : StereoCalib::saveCameraParams
	* 访问 : public
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : cameraParams	[in]	已标定的摄像机参数
	* 参数 : filename		[in]	参数文件路径/文件名
	*/
//	int saveCameraParams(const CameraParams& cameraParams, const char* filename = "cameraParams.yml");

	/*----------------------------
	* 功能 : 执行单目摄像机标定
	*----------------------------
	* 函数 : StereoCalib::calibrateSingleCamera
	* 访问 : public
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : cornerDatas			[in]	棋盘角点数据
	* 参数 : cameraParams			[out]	双目标定数据
	*/
//	int calibrateSingleCamera(CornerDatas& cornerDatas, CameraParams& cameraParams);

	/*----------------------------
	* 功能 : 执行双目摄像机标定
	*		 若每个摄像机尚未标定，则首先进行单目标定，再进行双目标定
	*----------------------------
	* 函数 : StereoCalib::calibrateStereoCamera
	* 访问 : public
	* 返回 : 0 - 操作失败，1 - 操作成功
	*
	* 参数 : cornerDatas			[in]	棋盘角点数据
	* 参数 : stereoParams			[i/o]	双目标定数据
	* 参数 : cameraUncalibrated		[in]	每个摄像机是否已经单独标定
	*/
	int calibrateStereoCamera(CornerDatas& cornerDatas, CalibrationParames& m_CalibrationResult);


protected:
private:
	/*----------------------------
	* 功能 : 在图像右下角显示指定文字信息
	*----------------------------
	* 函数 : StereoCalib::showText
	* 访问 : private
	* 返回 : void
	*
	* 参数 : img	[in]	图像
	* 参数 : text	[in]	文字信息
	*/
	//void showText(cv::Mat& img, string text);

};