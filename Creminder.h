#pragma once


// Creminder �Ի���

class Creminder : public CDialogEx
{
	DECLARE_DYNAMIC(Creminder)

public:
	Creminder(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Creminder();

// �Ի�������
	enum { IDD = IDD_REMINDER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CFont font;
};
