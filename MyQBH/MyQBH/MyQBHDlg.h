
// MyQBHDlg.h : ͷ�ļ�
//

#pragma once
#include<Windows.h>
#include "Pipe.h"

// CMyQBHDlg �Ի���
class CMyQBHDlg : public CDialogEx, CPipe
{
// ����
public:
	CMyQBHDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYQBH_DIALOG };
	CButton	m_RecButton;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnRecording();

public:
	CTime mytime;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBnQbh();

public:
	//��õ�ǰ·��
	CString GetCurrentFilePath();
	afx_msg void OnStnClickedStaticText();
	CString m_statictxt;
};
