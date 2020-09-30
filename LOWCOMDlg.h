#pragma once

#include "DataComm.h"
#include "Utils.h"

// LOWCOMDlg dialog

class LOWCOMDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LOWCOMDlg)

public:
	LOWCOMDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~LOWCOMDlg();

// Dialog Data
	enum { IDD = IDD_LOWCOM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CString m_trackDis2;
	CString m_trackDis;
	CString m_dip;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
};
