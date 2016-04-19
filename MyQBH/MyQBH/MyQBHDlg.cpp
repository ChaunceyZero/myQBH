
// MyQBHDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMyQBHDlg �Ի���



CMyQBHDlg::CMyQBHDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyQBHDlg::IDD, pParent)
	, m_statictxt(_T("1.ʹ��ǰ����ȷ��¼���豸����\n\n2.��ȷ��¼�������İ�����\n\n3.¼��ʱ�����Գ�����ʣ�Ҳ���Ժ߳�������\n\n4.���Record��ť��¼����ʼ������8�룬�����Զ�ֹͣ��\n\n5.���QBH��ť���к߳�����ƥ�䣬������б������ʾ��"))
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

// CMyQBHDlg ��Ϣ�������

BOOL CMyQBHDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyQBHDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyQBHDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyQBHDlg::OnBnClickedBnRecording()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	__super::OnTimer(nIDEvent);

	StopRecordingToFile();
	m_RecButton.SetWindowText(_T("Record"));
	KillTimer(nIDEvent);
}


void CMyQBHDlg::OnBnClickedBnQbh()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString currentPath = GetCurrentFilePath();
	CString mfcc_save = currentPath + "sound_mfcc.txt";
	CString soundPath = currentPath + "sound1.wav";
	::MessageBox(NULL, soundPath, _T("OK"), NULL);
	getmfcc(soundPath, mfcc_save);
}

CString CMyQBHDlg::GetCurrentFilePath(){
	//��ȡ��ǰ�ļ���·��
	TCHAR exeFullPath[MAX_PATH];
	CString strPath;
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = (CString)exeFullPath;
	int position = strPath.ReverseFind('\\');
	strPath = strPath.Left(position + 1);
	//::MessageBox(NULL, strPath, _T("OK"), NULL);
	return strPath;
}