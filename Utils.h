#ifndef __UTILS_HEAD
#define __UTILS_HEAD

#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "DataComm.h"
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <map>

//#define CRTDBG_MAP_ALLOC
//#include <stdlib.h>  
//#include <crtdbg.h>  
using namespace cv;
using namespace std;
/*----------------------------
* 功能 :	获取程序运行时所在的文件夹路径
*----------------------------
* 返回 : 路径
*/
CString GetAppExecPath();

//检测内存泄漏
//void EnableMemLeakCheck();

//保存配置参数,sensorData是全局变量
bool WriteConfigFile();
//读取配置参数sensorData
bool ReadConfigFile();

//以catName, porperName读取相应的参数
string GetPropertyByName(char * catName, char * porperName);
//以catName, porperName设置相应的参数
bool SetPropertyByName(char* catName, char* properName, const char* var);

//关闭程序进程，关机
BOOL ReSetWindows(DWORD dwFlags, BOOL bForce);

//得到配置文件中的矫正参数
void GetRevParam(double& up_x, double& up_y, double& down_x, double& down_y);
//创建相机图片保存的文件夹
CString CreatePicFolder(CString data, CString time);

//保存测量图片
void SaveMeasPics(map<string, Mat> pics);
//递归删除指定文件夹
void RecursiveDelete(CString szPath);
//递归计算文件夹大小
ULONGLONG CalcFloderSize(CString szPath);
//得到要删除的文件夹，最先建立的time文件夹
CString GetPreDelFloder(CString szPath);


#endif 


