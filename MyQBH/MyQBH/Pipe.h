#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "soundin.h"
#include "soundfile.h"
#include "ptrFIFO.h"


class CPipe
{
public:
	CPipe();
	virtual ~CPipe();
	
	virtual void OnEndOfPlayingFile();
	
public:
	//停止录音
	void StopRecordingToFile();
	//开始录音
	void StartRecordingToFile(CString strPath);
	//写入声音数据
	void WriteSoundDataToFile(CBuffer* buffer);

	static void DataFromSoundIn(CBuffer* buffer, void* Owner);
	
private:
	//声音文件
	CSoundFile* m_pFile;
	//录制对象
	CSoundIn	m_SoundIn;
};