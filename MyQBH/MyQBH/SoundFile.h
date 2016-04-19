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
	//�������ļ�
	bool OpenWaveFile();
	//���������ļ�
	bool CreateWaveFile();
private:
	//WAVE�ļ����
	HMMIO		m_hFile;
	//�ļ�����
	CString		m_FileName;
	//��дģʽ
	EREADWRITE	m_Mode;
	//���ݿ�
	MMCKINFO	m_MMCKInfoData;
	//RIFF��
	MMCKINFO	m_MMCKInfoParent;
	//RIFF�µ��ӿ�
	MMCKINFO	m_MMCKInfoChild;
};