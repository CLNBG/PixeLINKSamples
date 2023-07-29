

#ifndef UTILS_H
#define UTILS_H

#include <PixeLINKAPI.h>

#include <cassert>

#ifndef ASSERT
#define ASSERT(x)	do { assert(x); } while (0)
#endif


//
//  Some camera utilities
//
float	PxLGetExposure(HANDLE hCamera);
void	PxLSetExposure(HANDLE hCamera, float newExposure);


// 
// Some timer utilities
//

U64	PxLGetCurrentTimeMillis();

//
// Extremely simple timer class
//
class Timer
{
public:
	Timer(bool start=true) 
	{
		if (start) {
			m_startTime = PxLGetCurrentTimeMillis();
		}
	}

	void Start()
	{
		m_startTime = PxLGetCurrentTimeMillis();
	}

	U64 ElapsedTime()
	{
		return PxLGetCurrentTimeMillis() - m_startTime;
	}

	void PrintElapsedTime(const char* pMsg) {
		ASSERT(NULL != pMsg);
		printf("%s - Elapsed time = %I64u ms\n", pMsg, ElapsedTime());
	}

private:
	U64 m_startTime;

};


#endif // UTILS_H