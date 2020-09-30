// ParamReviseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StereoCamera.h"
#include "ParamReviseDlg.h"
#include "afxdialogex.h"


// CParamReviseDlg dialog

IMPLEMENT_DYNAMIC(CParamReviseDlg, CDialogEx)

CParamReviseDlg::CParamReviseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CParamReviseDlg::IDD, pParent)
	, m_Rev_up_x(_T(""))
	, m_Rev_up_y(_T(""))
	, m_Rev_down_x(_T(""))
	, m_Rev_down_y(_T(""))
{

}

CParamReviseDlg::~CParamReviseDlg()
{
}

void CParamReviseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RiParam_UP_X, m_Rev_up_x);
	DDX_Text(pDX, IDC_RiParam_UP_Y, m_Rev_up_y);
	DDX_Text(pDX, IDC_RiParam_DOWN_X, m_Rev_down_x);
	DDX_Text(pDX, IDC_RiParam_DOWN_Y, m_Rev_down_y);
}


BEGIN_MESSAGE_MAP(CParamReviseDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CParamReviseDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CParamReviseDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CParamReviseDlg message handlers
BOOL CParamReviseDlg::OnInitDialog()
{

	m_Rev_up_x = GetPropertyByName("RiviseParams", "up_x").c_str();
	m_Rev_up_y = GetPropertyByName("RiviseParams", "up_y").c_str();
	m_Rev_down_x = GetPropertyByName("RiviseParams", "down_x").c_str();
	m_Rev_down_y = GetPropertyByName("RiviseParams", "down_y").c_str();
	UpdateData(FALSE);

	return TRUE;
}


void CParamReviseDlg::OnBnClickedOk()
{
	//RiviseParams
	UpdateData(TRUE);
	SetPropertyByName("RiviseParams", "up_x", m_Rev_up_x);
	SetPropertyByName("RiviseParams", "up_y", m_Rev_up_y);
	SetPropertyByName("RiviseParams", "down_x", m_Rev_down_x);
	SetPropertyByName("RiviseParams", "down_y", m_Rev_down_y); 

	AfxMessageBox(_T("±£´æ³É¹¦£¡"));
	CDialogEx::OnOK();
}


void CParamReviseDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
	OnDestroy();
}



void CParamReviseDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}
