
// MyQBHDlg.h : 头文件
//

#pragma once
#include<Windows.h>
#include "Pipe.h"

// CMyQBHDlg 对话框
class CMyQBHDlg : public CDialogEx, CPipe
{
// 构造
public:
	CMyQBHDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYQBH_DIALOG };
	CButton	m_RecButton;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	//获得当前路径
	CString GetCurrentFilePath();
	afx_msg void OnStnClickedStaticText();
	CString m_statictxt;
};
