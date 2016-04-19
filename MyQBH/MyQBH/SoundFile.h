#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "soundbase.h"

typedef enum{
	FILE_ERROR = 0,
	WRITE = 2
}EREADWRITE;

class CSoundFile : public CSoundBase
{
public:
	bool IsOK();
	void Close();
	EREADWRITE GetMode();
	bool Write(CBuffer* buffer);
	CSoundFile(CString FileName, WAVEFORMATEX* format = NULL);
	virtual ~CSoundFile();
protected:
	//打开声音文件
	bool OpenWaveFile();
	//创建声音文件
	bool CreateWaveFile();
private:
	//WAVE文件句柄
	HMMIO		m_hFile;
	//文件名称
	CString		m_FileName;
	//读写模式
	EREADWRITE	m_Mode;
	//数据块
	MMCKINFO	m_MMCKInfoData;
	//RIFF块
	MMCKINFO	m_MMCKInfoParent;
	//RIFF下的子块
	MMCKINFO	m_MMCKInfoChild;
};