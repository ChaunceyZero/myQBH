#include "stdafx.h"
#include "SoundFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSoundFile::CSoundFile(CString FileName, WAVEFORMATEX* format)
{
	m_hFile = NULL;
	m_FileName = FileName;
	//初始化MMCKINFO结构
	ZeroMemory(&m_MMCKInfoParent,sizeof(MMCKINFO));
	ZeroMemory(&m_MMCKInfoChild,sizeof(MMCKINFO));
	ZeroMemory(&m_MMCKInfoData,sizeof(MMCKINFO));
	
	//录制声音文件
	{
		m_Mode = WRITE;
		m_Format = *format;
		CreateWaveFile();
	}
	if(m_Mode == ERROR) Close();
}

CSoundFile::~CSoundFile()
{
	Close();
}

void CSoundFile::Close()
{
	if(m_hFile)
	{
		if(m_Mode == WRITE)
		{
			//对于录制声音，需要连续跳出两层
		::mmioAscend(m_hFile, &m_MMCKInfoChild, 0);
		::mmioAscend(m_hFile, &m_MMCKInfoParent, 0);
		}
		::mmioClose(m_hFile, 0);
		m_hFile = NULL;
	}	

}

bool CSoundFile::Write(CBuffer *buffer)
{
	if(m_Mode == WRITE)
	{
		//将缓冲区内的数据写入到文件中去
		int length = mmioWrite(m_hFile, buffer->ptr.c, buffer->ByteLen);
		if(length == buffer->ByteLen)
			return true;
	}
	return false;
}

bool CSoundFile::CreateWaveFile()
{
	// 检验文件句柄是否已经打开
	if(m_hFile) 
		return FALSE;	
	// 创建文件，获取wave文件句柄
	m_hFile = ::mmioOpen(m_FileName.GetBuffer(0),NULL, MMIO_CREATE|MMIO_WRITE|MMIO_EXCLUSIVE | MMIO_ALLOCBUF);
	if(m_hFile == NULL) 
	{
		m_Mode = FILE_ERROR;
		return FALSE;
	}
	//创建“RIFF”块
	ZeroMemory(&m_MMCKInfoParent, sizeof(MMCKINFO));
	m_MMCKInfoParent.fccType = mmioFOURCC('W','A','V','E');
	MMRESULT mmResult = ::mmioCreateChunk( m_hFile,&m_MMCKInfoParent, MMIO_CREATERIFF);
	//创建格式块
	ZeroMemory(&m_MMCKInfoChild, sizeof(MMCKINFO));
	m_MMCKInfoChild.ckid = mmioFOURCC('f','m','t',' ');
	m_MMCKInfoChild.cksize = sizeof(WAVEFORMATEX) + m_Format.cbSize;
	mmResult = ::mmioCreateChunk(m_hFile, &m_MMCKInfoChild, 0);
	mmResult = ::mmioWrite(m_hFile, (char*)&m_Format, sizeof(WAVEFORMATEX) + m_Format.cbSize); 
	mmResult = ::mmioAscend(m_hFile, &m_MMCKInfoChild, 0);
	//创建数据块
	m_MMCKInfoChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmResult = ::mmioCreateChunk(m_hFile, &m_MMCKInfoChild, 0);
	
	return TRUE;
}

bool CSoundFile::OpenWaveFile()
{
	//检验是否已经打开
	if(m_hFile) 
		return FALSE; 
	//打开WAVE文件
	m_hFile = mmioOpen(m_FileName.GetBuffer(0),NULL,MMIO_READ);
	if(m_hFile == NULL) 
	{
		m_Mode = FILE_ERROR;
		return FALSE;
	}
	//需要验证是否为“RIFF”块
	m_MMCKInfoParent.fccType = mmioFOURCC('W','A','V','E');
	MMRESULT mmResult = ::mmioDescend(m_hFile, &m_MMCKInfoParent,NULL,MMIO_FINDRIFF);
	if(mmResult)
	{
		AfxMessageBox(_T("Error descending into file"));
		mmioClose(m_hFile,0);
		m_hFile = NULL;
		m_Mode = FILE_ERROR;
		return FALSE;
	}
	//进入格式块
	m_MMCKInfoChild.ckid = mmioFOURCC('f','m','t',' ');
	mmResult = mmioDescend(m_hFile,&m_MMCKInfoChild,&m_MMCKInfoParent,MMIO_FINDCHUNK);
	if(mmResult)
	{
		AfxMessageBox(_T("Error descending in wave file"));
		mmioClose(m_hFile,0);
		m_Mode = FILE_ERROR;
		m_hFile = NULL;
		return FALSE;
	}
	//读取WAVE格式信息
	DWORD bytesRead = mmioRead(m_hFile,(LPSTR)&m_Format,m_MMCKInfoChild.cksize);
	if(bytesRead < 0)
	{
		AfxMessageBox(_T("Error reading PCM wave format record"));
		mmioClose(m_hFile,0);
		m_Mode = FILE_ERROR;
		return FALSE;
	}
	//跳出格式块	
	mmResult = mmioAscend(m_hFile,&m_MMCKInfoChild,0);
	if(mmResult)
	{
		AfxMessageBox(_T("Error ascending in File"));
		mmioClose(m_hFile,0);
		m_hFile = NULL;
		m_Mode = FILE_ERROR;
		return FALSE;
	}
	//进入数据块
	m_MMCKInfoChild.ckid = mmioFOURCC('d','a','t','a');
	mmResult = mmioDescend(m_hFile,&m_MMCKInfoChild,
		&m_MMCKInfoParent,MMIO_FINDCHUNK);
	if(mmResult)
	{
		AfxMessageBox(_T("error reading data chunk"));
		mmioClose(m_hFile,0);
		m_hFile = NULL;
		m_Mode = FILE_ERROR;
		return FALSE;
	}
	return TRUE;
}

EREADWRITE CSoundFile::GetMode()
{
	return m_Mode;
}

bool CSoundFile::IsOK()
{
	if(m_Mode == FILE_ERROR)
		return false;
	else
		return true;
}
