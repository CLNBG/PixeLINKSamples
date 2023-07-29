

#pragma once

#include <PixeLINKAPi.h>

class AviCaptureState
{
public:
	AviCaptureState(
		HANDLE hCamera, 
		U32	framesPerClip,
		U32 numberOfClips,
		const CString& clipFileNameTemplate,
		U32 (_stdcall pCallbackFunction)(HANDLE, U32, PXL_RETURN_CODE)) :
			m_keepRunning(true), 
			m_numberOfClips(numberOfClips),
			m_framesPerClip(framesPerClip),
			m_hCamera(hCamera), 
			m_pCallbackFunction(pCallbackFunction), 
			m_clipFileNameTemplate(clipFileNameTemplate),
			m_currentCaptureIndex(0)
	{
	}

	HANDLE	GetCameraHandle()	const { return m_hCamera;		}
	U32		GetNumberOfClips()	const { return m_numberOfClips; }

	bool KeepRunning() const { return m_keepRunning; }
	void	StopRunning() { m_keepRunning = false; }

	bool StartCapturing()
	{
		return CaptureAClip();
	}

	bool	CaptureNextClip()
	{
		UpdateCaptureIndex();
		return CaptureAClip();
	}

	bool	CaptureAClip()
	{
		PXL_RETURN_CODE rc = PxLGetClip(m_hCamera, m_framesPerClip, GetCurrentClipFileName(), m_pCallbackFunction);
		return API_SUCCESS(rc);
	}


	void UpdateCaptureIndex()
	{
		m_currentCaptureIndex++;
		if (m_currentCaptureIndex == m_numberOfClips) {
			m_currentCaptureIndex = 0;
		}
	}

	U32 GetCurrentCaptureIndex() const { return m_currentCaptureIndex; }
	
	char* GetCurrentClipFileName() const
	{
		static char buffer[MAX_PATH];
		CString fileName;
		fileName.Format(m_clipFileNameTemplate, m_currentCaptureIndex);
		strcpy(buffer, fileName);
		return &buffer[0];
	}

private:
	volatile bool m_keepRunning;
	U32 (_stdcall * m_pCallbackFunction)(HANDLE, U32, PXL_RETURN_CODE);
	HANDLE m_hCamera;
	U32	m_framesPerClip;
	U32	m_numberOfClips;
	U32 m_currentCaptureIndex;
	CString m_clipFileNameTemplate;

};

