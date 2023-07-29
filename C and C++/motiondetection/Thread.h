#ifndef THREAD_H
#define THREAD_H

//
// A simple class to manage the overhead of creating and managing a worker thread.
//
// To use it, just derive from this class and override MainImpl()
// 

#include <windows.h>
//
// Abstract base class
//
class Thread
{
public:
	Thread();
	virtual ~Thread();

	virtual bool	StartRunning();
	virtual bool	IsRunning() { return m_keepRunning; }
	virtual void	RequestTermination();
	virtual void	WaitForThreadToFinish();

protected:
	virtual bool	KeepRunning() const { return m_keepRunning; }
	virtual void	MainImpl() = 0;		// Main() function for the new thread
	unsigned int	m_threadAddr;
	HANDLE			m_hThread;
	volatile bool	m_keepRunning;
	static unsigned int __stdcall Main(void* pParam);

};



#endif THREAD_H


