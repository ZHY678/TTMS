#pragma once


// Creminder 对话框

class Creminder : public CDialogEx
{
	DECLARE_DYNAMIC(Creminder)

public:
	Creminder(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Creminder();

// 对话框数据
	enum { IDD = IDD_REMINDER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CFont font;
};
