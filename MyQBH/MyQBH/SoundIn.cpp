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
	//�����Ƿ�����¼��	
	if(m_bRecording || DataFromSoundIn == NULL || m_pOwner == NULL)
	{
		return FALSE;
	}
	else
	{
		if(format != NULL)
			m_Format = *format;
		//��¼���豸
		mmReturn = ::waveInOpen( &m_hRecord, WAVE_MAPPER, &m_Format, m_ThreadID, NULL, CALLBACK_THREAD);
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, _T("in Start()"));
			return FALSE;
		}
		else
		{
			//��ʼ׼��¼�ƻ�����
			for(int i=0; i<3; i++)
			{
				AddInputBufferToQueue();
			}
			
			// ��ʼ¼��
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
		//ֹͣ¼��
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
	//��ȡ���ݻ�����	
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;
	//��������
	mmReturn = ::waveInUnprepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, _T("in OnWIM_DATA()"));
		return;
	}
	TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);
	
	if(m_bRecording)
	{
		//��ȡд�������뻺����
		CBuffer buf(pHdr->lpData, pHdr->dwBufferLength);
		//��¼������д���ļ���	
		DataFromSoundIn(&buf, m_pOwner);
		//����ʹ�øû�����
		mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, _T("in OnWIM_DATA()"));
		}
		else 
		{
			//���¼��뵽¼�ƶ�����
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
	//����һ��WAVEHDR	
	LPWAVEHDR pHdr = new WAVEHDR;
	if(pHdr == NULL) return NULL;
	ZeroMemory(pHdr, sizeof(WAVEHDR));
	//���仺����
	CBuffer buf(m_Format.nBlockAlign*m_BufferSize, false);
	pHdr->lpData = buf.ptr.c;
	pHdr->dwBufferLength = buf.ByteLen;
	//׼��¼�ƻ���ͷ	
	mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, _T("in AddInputBufferToQueue()"));
		return m_QueuedBuffers;
	}
	//���뵽¼�ƶ�����
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
