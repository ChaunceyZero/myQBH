#include "stdafx.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBuffer::CBuffer(DWORD size, bool AutoDelete)
{
	m_bAutoDelete = AutoDelete;
	ptr.b = new BYTE[size];
	if(ptr.b) ByteLen = size;
}

CBuffer::CBuffer(void* buffer, DWORD length)
{
	m_bAutoDelete = false;
	if(buffer)
	{
		ptr.v = buffer;
		ByteLen = length;
	}
	else
	{
		ptr.b = NULL;
		ByteLen = 0;
	}
}

CBuffer::~CBuffer()
{
	if(m_bAutoDelete && ptr.b != NULL)  delete ptr.b;
}

void CBuffer::Erase()
{
	if(ptr.b) ZeroMemory(ptr.b,ByteLen);
}
