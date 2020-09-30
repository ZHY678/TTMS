
// StereoCamera.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

// CStereoCameraApp: 
// 有关此类的实现，请参阅 StereoCamera.cpp
//

class CStereoCameraApp : public CWinApp
{
public:
	CStereoCameraApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

#ifdef _DEBUG
protected:
	CMemoryState m_msOld, m_msNew, m_msDiff;
#endif  // _DEBUG



	DECLARE_MESSAGE_MAP()
};

extern CStereoCameraApp theApp;