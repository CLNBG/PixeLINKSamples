

#include "stdafx.h"
#include <process.h>
#include "WorkerThread.h"
#include "hardwaretrigger.h"
#include <iostream>


#define POST_MESSAGE_TO_APP(messageId, wParam, lParam)	do { ::PostMessage(theApp.GetMainWnd()->GetSafeHwnd(), (messageId), (wParam), (lParam)); } while(0)


static unsigned int __stdcall 
WorkerThreadFunction(void* pParam)
{
	ASSERT(NULL != pParam);
	WorkerThread* pWorkerThread = (WorkerThread*)pParam;
	ASSERT(pWorkerThread->KeepRunning());
	
	PXL_RETURN_CODE rc = ApiUnknownError;

	try {
		workerThreadFunction_t pFn = pWorkerThread->GetThreadFunction();
		ASSERT(NULL != pFn);
		rc = pFn(pWorkerThread);
	} catch (const std::runtime_error& e) {
		pWorkerThread->SetErrorMessage(e.what());
	}

	pWorkerThread->SetReturnCode(rc);
//	STATIC_ASSERT(sizeof(LPARAM) >= sizeof(void*), LPARAM_cannot_be_used_to_pass_a_pointer);
	POST_MESSAGE_TO_APP(WM_WORKER_THREAD_FINISHED, 0, (LPARAM)pWorkerThread);
	return 0;
}


WorkerThread::WorkerThread(workerThreadFunction_t pFn, void* userData, bool freeUserData) :
	m_pFn(pFn),
	m_keepRunning(true),
	m_userData(userData),
	m_freeUserData(freeUserData),
	m_hThread(0)
{
	ASSERT(NULL != pFn);

	unsigned int threadAddr;
	m_hThread = (HANDLE)_beginthreadex(
		NULL,	// security
		0,		// stack size
		WorkerThreadFunction, 
		(void*)this,
		CREATE_SUSPENDED,
		&threadAddr);
	if (0 == m_hThread) {
		throw std::runtime_error("Unable to create a thread");
	}

}

//
// If we're going to free the data, then it must be POD because we don't 
// call any destructors.
// 
WorkerThread::~WorkerThread()
{
	CloseHandle(m_hThread);
	if (m_freeUserData) {
		free(m_userData);
	}
}

bool	
WorkerThread::StartRunning()
{
	DWORD retVal = ResumeThread(m_hThread);
	return (-1 != retVal);
}

void
WorkerThread::WaitUntilFinished()
{
	WaitForSingleObject(m_hThread, INFINITE);
}


void	
WorkerThread::NotifyUpdate(WPARAM wParam, LPARAM lParam)
{
	POST_MESSAGE_TO_APP(WM_WORKER_THREAD_UPDATE, wParam, lParam);
}