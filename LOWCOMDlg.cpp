// LOWCOMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "LOWCOMDlg.h"
#include "afxdialogex.h"


// LOWCOMDlg dialog

IMPLEMENT_DYNAMIC(LOWCOMDlg, CDialogEx)

LOWCOMDlg::LOWCOMDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(LOWCOMDlg::IDD, pParent)
	, m_trackDis2(_T(""))
	, m_trackDis(_T(""))
	, m_dip(_T(""))
{

}

LOWCOMDlg::~LOWCOMDlg()
{
}

void LOWCOMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SD_trackDis2, m_trackDis2);
	DDX_Text(pDX, IDC_SD_trackDis, m_trackDis);
	DDX_Text(pDX, IDC_SD_dip, m_dip);
}


BEGIN_MESSAGE_MAP(LOWCOMDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &LOWCOMDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &LOWCOMDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// LOWCOMDlg message handlers

BOOL LOWCOMDlg::OnInitDialog()
{
	ReadConfigFile();
	char strTmp[20];
	sprintf_s(strTmp, "%.2f", sensorData.trackDis);
	GetDlgItem(IDC_SD_trackDis)->SetWindowTextA(strTmp);
	sprintf_s(strTmp, "%.2f", sensorData.dip);
	GetDlgItem(IDC_SD_dip)->SetWindowTextA(strTmp);

	SetTimer(1, 50, NULL);

	return TRUE;
}

void LOWCOMDlg::OnBnClickedOk()
{
	sensorData.Dvalue0 = sensorData.Dvalue;
	SetPropertyByName("sensorData", "Dvalue0", to_string(sensorData.Dvalue0).c_str());
	CString strVa;
	GetDlgItemText(IDC_SD_trackDis2, strVa);
	sensorData.trackDisRef = atof(strVa.GetBuffer(0)) - sensorData.trackDis;
	SetPropertyByName("sensorData", "trackDisRef", to_string(sensorData.trackDisRef).c_str());
	//AfxMessageBox(_T("±£´æ³É¹¦£¡"));
	//CDialogEx::OnOK();
}

void LOWCOMDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		ReadConfigFile();
		char strTmp[20];
		float showTrackDis = sensorData.trackDisRef + sensorData.trackDis;
		sprintf_s(strTmp, "%.2f", showTrackDis);
		GetDlgItem(IDC_SD_trackDis)->SetWindowTextA(strTmp);
		sprintf_s(strTmp, "%.2f", sensorData.dip);
		GetDlgItem(IDC_SD_dip)->SetWindowTextA(strTmp);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void LOWCOMDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
