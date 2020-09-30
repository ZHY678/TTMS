#pragma once
#include "Utils.h"

// CParamReviseDlg dialog

class CParamReviseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParamReviseDlg)

public:
	CParamReviseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParamReviseDlg();

// Dialog Data
	enum { IDD = IDD_PARAMRIVISE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL OnInitDialog();

	CString m_Rev_up_x;
	CString m_Rev_up_y;
	CString m_Rev_down_x;
	CString m_Rev_down_y;
	afx_msg void OnDestroy();
};
