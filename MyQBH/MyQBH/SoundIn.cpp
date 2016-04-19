#include "stdafx.h"
#include "SoundIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSoundIn, CWinThread)

CSoundIn::CSoundIn()
{
	m_QueuedBuffers = 0;
	m_hRecord = NULL;
	m_bRecording = false;
	DataFromSoundIn = NULL;
	m_pOwner = NULL;
	CreateThread();
	m_bAutoDelete = false;
}

CSoundIn::~CSoundIn()
{
	if(m_bRecording)
		Stop();
	::PostQuitMessage(0);
}

BOOL CSoundIn::InitInstance()
{
	m_ThreadID = ::GetCurrentThreadId();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSoundIn, CWinThread)
	//{{AFX_MSG_MAP(CSoundIn)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(MM_WIM_DATA, OnMM_WIM_DATA)
END_MESSAGE_MAP()

bool CSoundIn::Start(WAVEFORMATEX* format)
{
	MMRESULT mmReturn = 0;
	//检验是否正在录制	
	if(m_bRecording || DataFromSoundIn == NULL || m_pOwner == NULL)
	{
		return FALSE;
	}
	else
	{
		if(format != NULL)
			m_Format = *format;
		//打开录制设备
		mmReturn = ::waveInOpen( &m_hRecord, WAVE_MAPPER, &m_Format, m_ThreadID, NULL, CALLBACK_THREAD);
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, _T("in Start()"));
			return FALSE;
		}
		else
		{
			//开始准备录制缓冲区
			for(int i=0; i<3; i++)
			{
				AddInputBufferToQueue();
			}
			
			// 开始录音
			mmReturn = ::waveInStart(m_hRecord);
			if(mmReturn )
			{
				waveInErrorMsg(mmReturn, _T("in Start()"));
				return FALSE;
			}
			m_bRecording = true;
		}
	}
	return TRUE;
}

void CSoundIn::Stop()
{
	MMRESULT mmReturn = MMSYSERR_NOERROR;
	if(!m_bRecording)
	{
		return;
	}
	else
	{
		//停止录制
		mmReturn = ::waveInReset(m_hRecord);
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, _T("in Stop()"));
			return;
		}
		else
		{
			m_bRecording = FALSE;
			Sleep(500);
			mmReturn = ::waveInClose(m_hRecord);
			if(mmReturn) waveInErrorMsg(mmReturn, _T("in Stop()"));
		}
	}
}

void CSoundIn::OnMM_WIM_DATA(UINT parm1, LONG parm2)
{
	MMRESULT mmReturn = 0;
	//获取数据缓冲区	
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;
	//清理数据
	mmReturn = ::waveInUnprepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, _T("in OnWIM_DATA()"));
		return;
	}
	TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);
	
	if(m_bRecording)
	{
		//获取写满的输入缓冲区
		CBuffer buf(pHdr->lpData, pHdr->dwBufferLength);
		//将录制数据写入文件中	
		DataFromSoundIn(&buf, m_pOwner);
		//重新使用该缓冲区
		mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, _T("in OnWIM_DATA()"));
		}
		else 
		{
			//重新加入到录制队列中
			mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
			if (mmReturn) waveInErrorMsg(mmReturn, _T("in OnWIM_DATA()"));
			else return;  
		}
		
	}
	delete pHdr->lpData;
	delete pHdr;
	m_QueuedBuffers--;
}

int CSoundIn::AddInputBufferToQueue()
{
	MMRESULT mmReturn = 0;
	//创建一个WAVEHDR	
	LPWAVEHDR pHdr = new WAVEHDR;
	if(pHdr == NULL) return NULL;
	ZeroMemory(pHdr, sizeof(WAVEHDR));
	//分配缓冲区
	CBuffer buf(m_Format.nBlockAlign*m_BufferSize, false);
	pHdr->lpData = buf.ptr.c;
	pHdr->dwBufferLength = buf.ByteLen;
	//准备录制缓冲头	
	mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, _T("in AddInputBufferToQueue()"));
		return m_QueuedBuffers;
	}
	//加入到录制队列中
	mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, _T("in AddInputBufferToQueue()"));
		return m_QueuedBuffers;
	}
	
	return m_QueuedBuffers++;
}

void CSoundIn::waveInErrorMsg(MMRESULT result, LPCTSTR addstr)
{
	// say error message
	char errorbuffer[100];
	waveInGetErrorText(result,errorbuffer ,100);
}
