
// StereoCamera.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

// CStereoCameraApp: 
// �йش����ʵ�֣������ StereoCamera.cpp
//

class CStereoCameraApp : public CWinApp
{
public:
	CStereoCameraApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

#ifdef _DEBUG
protected:
	CMemoryState m_msOld, m_msNew, m_msDiff;
#endif  // _DEBUG



	DECLARE_MESSAGE_MAP()
};

extern CStereoCameraApp theApp;