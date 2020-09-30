#include "stdafx.h"
#include "Utils.h"

CString GetAppExecPath()
{
	CString strPath;
	TCHAR path[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	::GetModuleFileName(AfxGetApp()->m_hInstance, path, _MAX_PATH);
	_tsplitpath(path, drive, dir, fname, ext);

	strPath.Empty();
	strPath += CString(drive);
	strPath += CString(dir);

	return strPath;
}

//void EnableMemLeakCheck()
//{
//	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
//	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
//	_CrtSetDbgFlag(tmpFlag);
//}

bool WriteConfigFile()
{
	CString path = GetAppExecPath();
	CString file;
	file.Format("%s\\Config.ini", path);

	WritePrivateProfileString("SensorData", "Lvalue", to_string(sensorData.Lvalue).c_str(), file);
	WritePrivateProfileString("SensorData", "dip", to_string(sensorData.dip).c_str(), file);
	WritePrivateProfileString("SensorData", "trackDis", to_string(sensorData.trackDis).c_str(), file);
	WritePrivateProfileString("SensorData", "battery", to_string(sensorData.battery).c_str(), file);
	WritePrivateProfileString("SensorData", "version", to_string(sensorData.version).c_str(), file);

	return true;
}

bool ReadConfigFile()
{
	CString path = GetAppExecPath();
	CString file;
	file.Format("%s\\Config.ini", path);

	LPTSTR strValue = new char[20];
	GetPrivateProfileStringA("SensorData", "Lvalue", "", strValue, 20, file);
	sensorData.Lvalue = atof(strValue);
	GetPrivateProfileStringA("SensorData", "dip", "", strValue, 20, file);
	sensorData.dip = atof(strValue);
	GetPrivateProfileStringA("SensorData", "trackDis", "", strValue, 20, file);
	sensorData.trackDis = atof(strValue); 
	GetPrivateProfileStringA("SensorData", "battery", "", strValue, 20, file);
	sensorData.battery = atoi(strValue);
	GetPrivateProfileStringA("SensorData", "version", "", strValue, 20, file);
	sensorData.version = atoi(strValue);
	GetPrivateProfileStringA("SensorData", "Dvalue0", "", strValue, 20, file);
	sensorData.Dvalue0 = atoi(strValue);
	GetPrivateProfileStringA("SensorData", "trackDisRef", "", strValue, 20, file);
	sensorData.trackDisRef = atof(strValue);

	delete strValue;
	return true;
}

bool SetPropertyByName(char* catName, char* properName, const char* var)
{
	CString path = GetAppExecPath();
	CString file;
	file.Format("%s\\Config.ini", path);

	WritePrivateProfileStringA(catName, properName, var, file);
	return true;
}

string GetPropertyByName(char * catName, char * porperName)
{
	CString path = GetAppExecPath();
	CString file;
	file.Format("%s\\Config.ini", path);

	LPTSTR strValue = new char[20];
	GetPrivateProfileStringA(catName, porperName, "", strValue, 20, file);
	string retStr(strValue);
	delete strValue;
	return retStr;
}

CString CreatePicFolder(CString data, CString time)
{
	CString place = "D:\\TTMS";
	CString place0 = "Data";
	if (!PathFileExists(place))
	{
		CreateDirectory(place, NULL);
	}
	CString path;
	path.Format("%s\\%s", place, place0);
	if (!PathFileExists(path))
	{
		CreateDirectory(path, NULL);
	}
	CString path1;
	path1.Format("%s\\%s", path, data);
	if (!PathFileExists(path1))
	{
		CreateDirectory(path1, NULL);
	}
	CString path2;
	path2.Format("%s\\%s", path1, time);
	if (!PathFileExists(path2))
	{
		CreateDirectory(path2, NULL);
	}
	 
	return path2;
}

void SaveMeasPics(map<string, Mat> pics)
{
	if (pics.empty())
		return;
	//string picPath = (*pics.begin()).first;
	CString filepath = "D:\\TTMS\\Data\\";
	ULONGLONG fileSize = CalcFloderSize(filepath);
	CString deleteFilePath;
	//ULONGLONG SAVE_SIZE = 15i64 * 1024i64 * 1024i64 * 1024i64;
	ULONGLONG SAVE_SIZE = 16106127360ULL;
	while (fileSize > SAVE_SIZE)
	{
		deleteFilePath = GetPreDelFloder(filepath);
		if (deleteFilePath !="" && deleteFilePath != filepath)
		{
			RecursiveDelete(deleteFilePath);
			//判断上层目录是否为空，为空则删除
			CString upDeleteFilePath = deleteFilePath + "\\..";
			//ULONGLONG size = CalcFloderSize(upDeleteFilePath);
			if (CalcFloderSize(upDeleteFilePath) == 0)
			{
				RemoveDirectory(upDeleteFilePath);
			}
		}
		fileSize = CalcFloderSize(filepath);
	}
	for each (auto a in pics) //保存图片
	{
		string filename = a.first;
		Mat img = a.second;
		imwrite(filename, img);
	}
}

void GetRevParam(double& up_x, double& up_y, double& down_x, double& down_y)
{
	CString path = GetAppExecPath();
	CString file;
	file.Format("%s\\Config.ini", path);
	LPTSTR strValue = new char[20];
	GetPrivateProfileStringA("RiviseParams", "up_x", "", strValue, 20, file);
	up_x = atof(strValue);
	GetPrivateProfileStringA("RiviseParams", "up_y", "", strValue, 20, file);
	up_y = atof(strValue);
	GetPrivateProfileStringA("RiviseParams", "down_x", "", strValue, 20, file);
	down_x = atof(strValue);
	GetPrivateProfileStringA("RiviseParams", "down_y", "", strValue, 20, file);
	down_y = atof(strValue);

	delete strValue;
}

//这是一个通用的提升权限函数，如果需要提升其他权限  
//更改LookupPrivilegeValue的第二个参数SE_SHUTDOWN_NAME，即可  
BOOL EnableShutDownPriv()
{
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tkp = { 0 };
	//打开当前程序的权限令牌  
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}
	//获得某一特定权限的权限标识LUID，保存在tkp中  
	if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//调用AdjustTokenPrivileges来提升我们需要的系统权限  
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	return TRUE;
}

BOOL ReSetWindows(DWORD dwFlags, BOOL bForce)
{
	//检查参数是否正确  
	if (dwFlags != EWX_LOGOFF&&dwFlags != EWX_REBOOT&&dwFlags != EWX_SHUTDOWN&&dwFlags != (EWX_REBOOT | EWX_FORCE))
		return FALSE;
	//获得系统的版本信息，让我们后面确定是否需要提升系统权限  
	OSVERSIONINFO osvi = { 0 };
	//获得参数的大小，以提供给GetVersionEx来判断这是一个新版本的OSVERSIONINFO，还是旧版本的  
	//新版本是OSVERSIONINFOEX。扩充版本  
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&osvi))
	{
		return FALSE;
	}
	//检查操作系统的版本，如果是NT类型的系统，需要提升系统权限  
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		EnableShutDownPriv();  
	}
	//判断是否是强制关机，强制关闭系统进程。  
	dwFlags |= (bForce != FALSE) ? EWX_FORCE : EWX_FORCEIFHUNG;
	//调用API  
	return ExitWindowsEx(dwFlags, 0);
}

CString GetPreDelFloder(CString szPath)
{
	CString data("9999-12-31");
	CString time("24-00-00");

	if (szPath.Right(1) != "\\")
		szPath += "\\";
	CString path = szPath +"*.*";
	CFileFind ff;
	CString datePath = szPath;
	BOOL res = ff.FindFile(path);
	CString dateFilePathTitle_PRE = data;
	while (res)
	{
		res = ff.FindNextFile();
		//CString ffname = ff.GetFileName();
		if (ff.IsDirectory() && !ff.IsDots())
		{
			CString dateFilePathTitle = ff.GetFileTitle();
			if (dateFilePathTitle_PRE > dateFilePathTitle)
			{
				dateFilePathTitle_PRE = dateFilePathTitle;
				datePath = ff.GetFilePath();
			}
		}
	}

	CString datePathIn = datePath + "\\*.*";
	CString deleteFilePath = datePath;
	CString timeFilePathTitle_PRE = time;
	res = ff.FindFile(datePathIn);
	while (res)
	{
		res = ff.FindNextFile();
		if (ff.IsDirectory() && !ff.IsDots())
		{
			CString timeFilePathTitle = ff.GetFileTitle();
			if (timeFilePathTitle_PRE > timeFilePathTitle)
			{
				timeFilePathTitle_PRE = timeFilePathTitle;
				deleteFilePath = ff.GetFilePath();
			}
		}
	}

	return deleteFilePath;
}

ULONGLONG CalcFloderSize(CString szPath)
{
	ULONGLONG size;
	ULONGLONG Size = 0;
	CString path = szPath;
	if (path.Right(1) != "\\")
		path += "\\";
	path += "*.*";

	CFileFind ff;
	CString strPath;
	BOOL res = ff.FindFile(path);
	while (res)
	{
		res = ff.FindNextFile();
		strPath = ff.GetFilePath();
		if (ff.IsDirectory() && !ff.IsDots())
		{
			Size = Size + CalcFloderSize(strPath);
		} //递归调用
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			size = ff.GetLength();
			Size = Size + size;
		}
	}
	return Size;
}

void RecursiveDelete(CString szPath)
{
	CFileFind ff;
	CString path = szPath;
	if (path.Right(1) != "\\")
		path += "\\";
	path += "*.*";
	BOOL res = ff.FindFile(path);
	while (res)
	{
		res = ff.FindNextFile();//是文件时直接删除  
		if (!ff.IsDots() && !ff.IsDirectory())
			DeleteFile(ff.GetFilePath());
		else if (ff.IsDots())
			continue;
		else if (ff.IsDirectory())
		{
			path = ff.GetFilePath();
			//是目录时继续递归，删除该目录下的文件  
			RecursiveDelete(path);
			//目录为空后删除目录  
			RemoveDirectory(path);  //删除空目录，若目录不为空则返回0  
		}
	}
	//最终目录被清空了，于是删除该目录  
	RemoveDirectory(szPath);

}