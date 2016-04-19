
// MyQBHDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyQBH.h"
#include "MyQBHDlg.h"
#include "afxdialogex.h"
#include "Pipe.h"
#include "getmfcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyQBHDlg 对话框



CMyQBHDlg::CMyQBHDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyQBHDlg::IDD, pParent)
	, m_statictxt(_T("1.使用前，请确保录音设备正常\n\n2.请确保录音环境的安静。\n\n3.录音时，可以唱出歌词，也可以哼出歌曲。\n\n4.点击Record按钮，录音开始，持续8秒，程序自动停止。\n\n5.点击QBH按钮进行哼唱旋律匹配，结果在列表框中显示。"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyQBHDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFisterDlg)
	DDX_Control(pDX, IDC_BN_RECORDING, m_RecButton);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STATIC_TEXT, m_statictxt);
}

BEGIN_MESSAGE_MAP(CMyQBHDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BN_RECORDING,OnBnClickedBnRecording)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BN_QBH, &CMyQBHDlg::OnBnClickedBnQbh)
END_MESSAGE_MAP()

// CMyQBHDlg 消息处理程序

BOOL CMyQBHDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyQBHDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyQBHDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyQBHDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyQBHDlg::OnBnClickedBnRecording()
{
	// TODO:  在此添加控件通知处理程序代码
	CString string;
	m_RecButton.GetWindowText(string);
	if (string == "Record")
	{
		SetTimer(0, 8000, NULL);
		StartRecordingToFile(GetCurrentFilePath());
		m_RecButton.SetWindowText(_T("Recording"));
	}
}

void CMyQBHDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	__super::OnTimer(nIDEvent);

	StopRecordingToFile();
	m_RecButton.SetWindowText(_T("Record"));
	KillTimer(nIDEvent);
}


void CMyQBHDlg::OnBnClickedBnQbh()
{
	// TODO:  在此添加控件通知处理程序代码
	CString currentPath = GetCurrentFilePath();
	CString mfcc_save = currentPath + "sound_mfcc.txt";
	CString soundPath = currentPath + "sound1.wav";
	::MessageBox(NULL, soundPath, _T("OK"), NULL);
	getmfcc(soundPath, mfcc_save);
}

CString CMyQBHDlg::GetCurrentFilePath(){
	//获取当前文件的路径
	TCHAR exeFullPath[MAX_PATH];
	CString strPath;
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = (CString)exeFullPath;
	int position = strPath.ReverseFind('\\');
	strPath = strPath.Left(position + 1);
	//::MessageBox(NULL, strPath, _T("OK"), NULL);
	return strPath;
}