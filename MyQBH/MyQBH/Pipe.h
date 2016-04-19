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
	//ֹͣ¼��
	void StopRecordingToFile();
	//��ʼ¼��
	void StartRecordingToFile(CString strPath);
	//д����������
	void WriteSoundDataToFile(CBuffer* buffer);

	static void DataFromSoundIn(CBuffer* buffer, void* Owner);
	
private:
	//�����ļ�
	CSoundFile* m_pFile;
	//¼�ƶ���
	CSoundIn	m_SoundIn;
};