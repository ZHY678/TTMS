#include "stdafx.h"
#include "Camera.h"


Camera::Camera()
{
}


Camera::~Camera()
{
}
int Camera::cam_count = 0; //相机数量初始化0
map<int, string> Camera::camDevices;
int Camera::width = 0;
int Camera::height = 0;
bool Camera::CameraInit(int type)
{
	//mindvision
	//if (type == MIND)
	//{
	//	CameraSdkInit(1);//"初始化SDK
	//	//////////////////////////////////////////////////////////////////////////TODO
	//	return true;
	//}
	//UVC
	if (type == UVC)
	{
		string wi_ = GetPropertyByName("camera", "width");
		string he_ = GetPropertyByName("camera", "height");
		width = atoi(wi_.c_str());
		height = atoi(he_.c_str());

		cam_count = CCameraDS::CameraCount();
		if (cam_count>0)
		{
			for (int i = 0; i < cam_count; i++)
			{
				char camer_name[1024];
				int retval = CCameraDS::CameraName(i, camer_name, sizeof(camer_name));
				if (retval > 0)
				{
					camDevices.insert(make_pair(i, string(camer_name)));
				}
			}
			return true;
		}
	}

	return false;
}

bool Camera::CameraOpen(int type, int deviceid, bool openProperty)
{
	//mindvision
	//if (type == MIND)
	//{
	//	CameraPlay(deviceid);
	//}
	//UVC
	if (type == UVC)
	{
		//camDs.CloseCamera(); //先释放相机
		isOpen = camDs.OpenCamera(deviceid, openProperty, width, height);
		if (!isOpen) {
			return false;
		}
		return true;
	}

	return false;
}

void Camera::CameraClose()
{
	isOpen = false;
	camDs.CloseCamera();
}

bool Camera::CameraIsOpen()
{
	return isOpen;
}

Mat Camera::getOneFrame()
{
	frame = Mat(camDs.QueryFrame(),true);
	return frame;
}

Camera m_cameraSupport1;   //相机1
Camera m_cameraSupport2;  //相机2


void Camera::CameraTakePics(Camera& cameraSupport, int num, vector<Mat>& imgs)
{
	Mat frame;
	if (!cameraSupport.CameraIsOpen()) //判断相机是否打开
		return;
	for (int i = 0; i < num; i++) //拍摄num张
	{
		frame = cameraSupport.getOneFrame();
		imgs.push_back(frame);
	}
}

void Camera::GetMeasPic(vector<Mat>& imgs, Mat& outMat)
{
	if (!imgs.empty())  //上部相机图像相减
	{
		int upLen = imgs.size();
		outMat = Mat::zeros(imgs[0].size(), CV_32F);

		for (int i = 0; i < upLen / 2; i++)
		{
			Mat src1 = imgs[i + upLen / 2];
			Mat src2 = imgs[i];
			cvtColor(src1, src1, CV_BGR2GRAY);
			cvtColor(src2, src2, CV_BGR2GRAY);
			Mat upDiff;
			subtract(src1, src2, upDiff);
			accumulate(upDiff, outMat);
		}
		outMat /= (upLen / 2);
		outMat.convertTo(outMat, CV_8U);
	}
}

void Camera::SplitPic(Mat& src, Mat& dstLeft, Mat& dstRight)
{
	if (!src.empty())
	{
		//切分左右视图
		Size2i img_size1(src.cols / 2, src.rows);
		Mat dstLeft = src(Rect(0, 0, img_size1.width, img_size1.height));
		Mat dstRight = src(Rect(img_size1.width, 0, img_size1.width, img_size1.height));
	}
}
