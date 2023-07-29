

#include "Thread.h"
#include <process.h>
#include <cassert>
#define ASSERT(x) do { assert((x)); } while(0)

//
// Function that is used as Main for the newly created thread
//
unsigned int __stdcall 
Thread::Main(void* pParam)
{
	ASSERT(NULL != pParam);
	Thread* pThread = reinterpret_cast<Thread*>(pParam);

	// Call the user-supplied function
	pThread->m_keepRunning = true;
	pThread->MainImpl();
	pThread->m_keepRunning = false;
	return 0;
}



Thread::Thread() : 
	m_threadAddr(0), 
	m_hThread(NULL), 
	m_keepRunning(true)
{
}
	
Thread::~Thread()
{
	CloseHandle(m_hThread);
}

bool
Thread::StartRunning()
{
	// Better to do this here than in the constructor because we don't want a thread accessing
	// this object while the creating thread is still in the constructor. The object
	// isn't 'valid' until the constructor is complete.
	m_hThread = (HANDLE)_beginthreadex(
		NULL,	// security
		0,		// stack size
		Thread::Main, 
		reinterpret_cast<void*>(this),
		0,	// Run now
		&m_threadAddr);
	if (0 == m_hThread) {
		return false;
	}
	return true;
}

void	
Thread::RequestTermination()
{
	m_keepRunning = false;
}

void	
Thread::WaitForThreadToFinish()
{
	WaitForSingleObject(m_hThread, INFINITE);
}





