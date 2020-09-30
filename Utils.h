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
* ���� :	��ȡ��������ʱ���ڵ��ļ���·��
*----------------------------
* ���� : ·��
*/
CString GetAppExecPath();

//����ڴ�й©
//void EnableMemLeakCheck();

//�������ò���,sensorData��ȫ�ֱ���
bool WriteConfigFile();
//��ȡ���ò���sensorData
bool ReadConfigFile();

//��catName, porperName��ȡ��Ӧ�Ĳ���
string GetPropertyByName(char * catName, char * porperName);
//��catName, porperName������Ӧ�Ĳ���
bool SetPropertyByName(char* catName, char* properName, const char* var);

//�رճ�����̣��ػ�
BOOL ReSetWindows(DWORD dwFlags, BOOL bForce);

//�õ������ļ��еĽ�������
void GetRevParam(double& up_x, double& up_y, double& down_x, double& down_y);
//�������ͼƬ������ļ���
CString CreatePicFolder(CString data, CString time);

//�������ͼƬ
void SaveMeasPics(map<string, Mat> pics);
//�ݹ�ɾ��ָ���ļ���
void RecursiveDelete(CString szPath);
//�ݹ�����ļ��д�С
ULONGLONG CalcFloderSize(CString szPath);
//�õ�Ҫɾ�����ļ��У����Ƚ�����time�ļ���
CString GetPreDelFloder(CString szPath);


#endif 


