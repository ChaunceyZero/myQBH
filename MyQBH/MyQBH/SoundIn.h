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
	// 回调函数，用于不断对于WAVE数据进行读写操作
	void (*DataFromSoundIn)(CBuffer* buffer, void* Owner);
	//窗口指针
	void* m_pOwner;
	//停止录制
	virtual void Stop();
	//开始录制
	virtual bool Start(WAVEFORMATEX* format = NULL);
	static void waveInErrorMsg(MMRESULT result, LPCTSTR addstr);
	//MM_WIM_DATA消息处理函数
	//当录制缓冲区已经装满数据的时候，
	//系统将会向回调窗口发送MM_WIM_DATA消息
	afx_msg void OnMM_WIM_DATA(UINT parm1, LONG parm2);
	DECLARE_MESSAGE_MAP()
		
protected:
	BOOL InitInstance();
	//录制数据
	int AddInputBufferToQueue();
	
protected:
	//是否开始录音
	bool m_bRecording;
	//录制句柄
	HWAVEIN m_hRecord;
	//加入播放队列的缓冲区个数
	int m_QueuedBuffers;
	//线程ID
	DWORD m_ThreadID;
	
};