
// MyQBH.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMyQBHApp: 
// �йش����ʵ�֣������ MyQBH.cpp
//

class CMyQBHApp : public CWinApp
{
public:
	CMyQBHApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMyQBHApp theApp;