

#pragma once

#include <vector>
#include "Utils.h"

class AviConvertState
{
public:
	AviConvertState() :
		m_keepRunning(true),
		m_currentFileNameIndex(0)
	{
		if (!GetListOfFilesInDir("", "*.pds", m_pdsFiles)) {
		}
	}

	U32	GetNumberOfFiles() const
	{
		return (U32)m_pdsFiles.size();
	}

	CString GetCurrentFileName() const
	{
		return m_pdsFiles[m_currentFileNameIndex];
	}

	U32		GetCurrentFileIndex() const { return m_currentFileNameIndex; }
	void	SetCurrentIndex(U32 newIndex)
	{
		ASSERT(newIndex < m_pdsFiles.size());
		m_currentFileNameIndex = newIndex;
	}

	void	StopConverting() 
	{
		m_keepRunning = false;
	}

	bool	KeepRunning() const { return m_keepRunning; }

	CWinThread*	GetThread() const { return m_pThread; }
	void		SetThread(CWinThread* thread) 
	{
		ASSERT(NULL != thread);
		m_pThread = thread; 
	}


protected: 
	volatile bool			m_keepRunning;
	std::vector<CString>	m_pdsFiles;
	U32						m_currentFileNameIndex;
	CWinThread*				m_pThread;
};