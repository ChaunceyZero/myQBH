#include "stdafx.h"
#include "MyQBH.h"
#include "Pipe.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPipe::CPipe()
{
	m_SoundIn.DataFromSoundIn = DataFromSoundIn;
	m_SoundIn.m_pOwner = this;
}

CPipe::~CPipe()
{

}

void CPipe::DataFromSoundIn(CBuffer* buffer, void* Owner)
{
	((CPipe*) Owner)->WriteSoundDataToFile(buffer);
}

void CPipe::WriteSoundDataToFile(CBuffer* buffer)
{
	if(m_pFile)
	{
		if(!m_pFile->Write(buffer))
		{
			m_SoundIn.Stop();
			AfxMessageBox(_T("Unable to write to file"));
		}
	}

}

void CPipe::StartRecordingToFile(CString strPath)
{
	CString savePath = strPath + "sound1.wav";
	m_pFile = new CSoundFile(savePath, m_SoundIn.GetFormat());
	if(m_pFile && m_pFile->IsOK())
		m_SoundIn.Start();
}

void CPipe::StopRecordingToFile()
{
	m_SoundIn.Stop();
	// close output file
	if(m_pFile)
		delete m_pFile;
}

void CPipe::OnEndOfPlayingFile()
{
	// implement this function in the GUI to change things when EOF is reached
}
