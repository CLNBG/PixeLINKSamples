

#include <stdio.h>
#include "utils.h"


//	Returns number of milliseconds since Unix started (Jan 1, 1970)
U64
PxLGetCurrentTimeMillis()
{
	// The FILETIME structure is a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC)
	FILETIME filetime;
	LONGLONG* pTime;
	GetSystemTimeAsFileTime(&filetime);

	// Convert from Windows Start of Epoch to Unix Start of Epoch
	// (and 100ns intervals to millis)
	pTime = (LONGLONG*)&filetime;
	*pTime = (*pTime - 116444736000000000) /10000;

	return *pTime;
}


float
PxLGetExposure(HANDLE hCamera)
{
	float exposureTime;
	U32 flags;
	U32 numParams = 1;
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_SHUTTER, &flags, &numParams, &exposureTime);
	ASSERT(API_SUCCESS(rc));
	ASSERT(1 == numParams);
	return exposureTime;

}

void
PxLSetExposure(HANDLE hCamera, float newExposure)
{
	float exposureTime;
	U32 flags;
	U32 numParams = 1;
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_SHUTTER, &flags, &numParams, &exposureTime);
	ASSERT(API_SUCCESS(rc));
	ASSERT(1 == numParams);

	exposureTime = newExposure;
	rc = PxLSetFeature(hCamera, FEATURE_SHUTTER, flags, numParams, &exposureTime);
	ASSERT(API_SUCCESS(rc));
}


