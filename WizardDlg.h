#pragma once
#include "afxwin.h"
#include "RegisterDlg.h"


// CWizardDlg 对话框

class CWizardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWizardDlg)

public:
	CWizardDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWizardDlg();

// 对话框数据
	enum { IDD = IDD_WIZARD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton m_OK;
	CButton m_Cancel;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
