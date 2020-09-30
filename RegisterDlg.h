#pragma once
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern BOOL Flag;
#define HOTKEY_PASTE		11111

// CRegisterDlg 对话框

class CRegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegisterDlg();
	void PasteReg();
	HWND hwnd;
	BOOL m_Once;

// 对话框数据
	enum { IDD = IDD_REGISTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_Name;
	CString m_Num1;
	CString m_Num2;
	CString m_Num3;
	CString m_Num4;
	CEdit m_Edit2;
	CEdit m_Edit3;
	CEdit m_Edit4;
	CEdit m_Edit5;
	CButton m_Advance;
	CButton m_Backoff;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit5();
	virtual BOOL OnInitDialog();
	afx_msg void OnHotKey(WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
	afx_msg void OnDrawClipboard();
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnEnChangeEdit1();
};
