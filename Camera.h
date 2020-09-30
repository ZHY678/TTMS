#ifndef __CAM_HEADER
#define __CAM_HEADER

#pragma once
#include "CameraDS.h"
//#include "CameraApi.h"
#include <map>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Utils.h"

using namespace cv;
using namespace std;

class Camera
{

public:
	//int upCameraCode;
	//int downCameraCode;

	Mat frame;  //当前帧
	static int cam_count; //摄像头数目
	static map<int,string> camDevices;
	CCameraDS camDs;  //DirectShow Support
	static int width;  //相机图像分辨率
	static int height;
	bool isOpen = false;  //是否打开相机

	Camera();
	~Camera();

	enum CAM_TYPE{ MIND = 1, UVC = 2 };  //摄像机类型 1 MINDVISION ；2 UVC免驱摄像头

	//根据摄像机类型初始化摄像头
	static bool CameraInit(int type);
	//打开或运行摄像头
	bool CameraOpen(int type, int deviceid = 0, bool openProperty = false);
	//关闭摄像头
	void CameraClose();
	//void CameraPlay();
	//void CameraPause();
	//得到一帧图像
	Mat getOneFrame();
	bool CameraIsOpen();
	static void CameraTakePics(Camera& cameraSupport, int num, vector<Mat>& imgs);
	static void GetMeasPic(vector<Mat>& imgs, Mat& outMat);
	static void SplitPic(Mat& src, Mat& dstLeft, Mat& dstRight);
};

//全局
extern Camera m_cameraSupport1;   //相机1
extern Camera m_cameraSupport2;  //相机2


#endif