#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "soundbase.h"

class CSoundIn : public CWinThread, public CSoundBase
{
	DECLARE_DYNCREATE(CSoundIn)
		
public:
	CSoundIn();
	virtual ~CSoundIn();
	// �ص����������ڲ��϶���WAVE���ݽ��ж�д����
	void (*DataFromSoundIn)(CBuffer* buffer, void* Owner);
	//����ָ��
	void* m_pOwner;
	//ֹͣ¼��
	virtual void Stop();
	//��ʼ¼��
	virtual bool Start(WAVEFORMATEX* format = NULL);
	static void waveInErrorMsg(MMRESULT result, LPCTSTR addstr);
	//MM_WIM_DATA��Ϣ������
	//��¼�ƻ������Ѿ�װ�����ݵ�ʱ��
	//ϵͳ������ص����ڷ���MM_WIM_DATA��Ϣ
	afx_msg void OnMM_WIM_DATA(UINT parm1, LONG parm2);
	DECLARE_MESSAGE_MAP()
		
protected:
	BOOL InitInstance();
	//¼������
	int AddInputBufferToQueue();
	
protected:
	//�Ƿ�ʼ¼��
	bool m_bRecording;
	//¼�ƾ��
	HWAVEIN m_hRecord;
	//���벥�Ŷ��еĻ���������
	int m_QueuedBuffers;
	//�߳�ID
	DWORD m_ThreadID;
	
};