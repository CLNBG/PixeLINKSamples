

#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <PixeLINKApi.h>

// Events we use to tell the app/doc that something's happened with the programming
#define WM_WORKER_THREAD_UPDATE			(WM_USER+0x101)
#define WM_WORKER_THREAD_FINISHED		(WM_USER+0x102)

class WorkerThread;
typedef PXL_RETURN_CODE (*workerThreadFunction_t)(WorkerThread*);

class WorkerThread
{
public:
	WorkerThread(PXL_RETURN_CODE(*pFn)(WorkerThread* pWorkerThread), void* userData, bool freeUserData);
	virtual ~WorkerThread();

	workerThreadFunction_t GetThreadFunction() const { return m_pFn; }

	// For the worker thread
	void*	GetUserData() const { return m_userData; }
	bool	KeepRunning() const { return m_keepRunning; }
	void	NotifyUpdate(WPARAM wParam, LPARAM lParam);
	void	SetErrorMessage(const CString& msg) { m_errorMessage = msg; }
	void	SetReturnCode(PXL_RETURN_CODE rc)	{ m_returnCode = rc;	}

	// For the controller
	bool	StartRunning();
	void	RequestStop() { m_keepRunning = false; };
	void	WaitUntilFinished();
	CString	GetErrorMessage()		const { return m_errorMessage; }
	PXL_RETURN_CODE GetReturnCode() const { return m_returnCode; }

protected:
	workerThreadFunction_t	m_pFn;
	HANDLE					m_hThread;
	void*					m_userData;
	bool					m_freeUserData;
	volatile bool			m_keepRunning;
	CString					m_errorMessage;
	PXL_RETURN_CODE			m_returnCode;
};

#endif