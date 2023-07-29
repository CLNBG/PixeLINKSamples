//
// framerate.c
//
// Demonstrates how to enable and set the frame rate
//
// The function EnableAndSetFrameRate() is the one function you'll probably need. The other
// functions are used to demonstrate some potentially unexpected behaviour with frame rate, i.e.
// when frame rate is disabled, setting the frame rate has no effect.
//

#include <stdio.h>
#include <PixeLINKAPI.h>
#include <assert.h>

#define ASSERT(x)	do { assert(x); } while(0)

//
// prototypes of local functions
//
float			GetFrameRate(HANDLE hCamera);
PXL_RETURN_CODE	SetFrameRate(HANDLE hCamera, float newFrameRate);
PXL_RETURN_CODE	EnableFrameRate(HANDLE hCamera, int enable);
PXL_RETURN_CODE	EnableAndSetFrameRate(HANDLE hCamera, float newFrameRate);



int 
main(int argc, char* argv[])
{
	// Initialize any camera
	HANDLE hCamera;
	PXL_RETURN_CODE rc;
	float initialFrameRate;
	float frameRate;

	rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to initialize a camera\n");
		return EXIT_FAILURE;
	}

	// Disable frame rate
	rc = EnableFrameRate(hCamera, 0);
	ASSERT(API_SUCCESS(rc));

	// What's the current frame rate?
	initialFrameRate = GetFrameRate(hCamera);

	// With frame rate disabled, try setting the frame rate
	// It will be successful, but..
	rc = SetFrameRate(hCamera, initialFrameRate / 2.0f);
	ASSERT(API_SUCCESS(rc));

	frameRate = GetFrameRate(hCamera);
	ASSERT(API_SUCCESS(rc));
	
	// .. when frame rate is disabled, setting the frame rate has no effect.
	ASSERT(frameRate != (initialFrameRate / 2.0f));

    
	// Enable the frame rate
	rc = EnableFrameRate(hCamera, 1);
	ASSERT(API_SUCCESS(rc));

	// Now when we set the frame rate it'll properly be set
	rc = SetFrameRate(hCamera, 3.0f); // Assuming 3.0 is suitable for your camera
	ASSERT(API_SUCCESS(rc));
	frameRate = GetFrameRate(hCamera);
	ASSERT(3.0f == frameRate);


	// Or, we can do the enable and set in one step
	EnableAndSetFrameRate(hCamera, 14.5f);	// Assuming 14.5 is suitable for your camera
	ASSERT(14.5f == GetFrameRate(hCamera));


	PxLUninitialize(hCamera);
	return EXIT_SUCCESS;
}


float
GetFrameRate(HANDLE hCamera)
{
	U32 flags;
	float frameRate;
	U32 numParams = 1;
	PXL_RETURN_CODE rc;

	rc = PxLGetFeature(hCamera, FEATURE_FRAME_RATE, &flags, &numParams, &frameRate);
	ASSERT(API_SUCCESS(rc));

	return frameRate;
}


PXL_RETURN_CODE
SetFrameRate(HANDLE hCamera, float newFrameRate)
{
	U32 flags;
	float frameRate;
	U32 numParams = 1;
	PXL_RETURN_CODE rc;

	rc = PxLGetFeature(hCamera, FEATURE_FRAME_RATE, &flags, &numParams, &frameRate);
	if (!API_SUCCESS(rc)) {
		return rc;
	}

	frameRate = newFrameRate;

	rc = PxLSetFeature(hCamera, FEATURE_FRAME_RATE, flags, numParams, &frameRate);

	return rc;
}

PXL_RETURN_CODE
EnableFrameRate(HANDLE hCamera, int enable)
{
	U32 flags;
	float frameRate;
	U32 numParams = 1;
	PXL_RETURN_CODE rc;

	rc = PxLGetFeature(hCamera, FEATURE_FRAME_RATE, &flags, &numParams, &frameRate);
	if (!API_SUCCESS(rc)) {
		return rc;
	}

	flags = ENABLE_FEATURE(flags, enable);

	rc = PxLSetFeature(hCamera, FEATURE_FRAME_RATE, flags, numParams, &frameRate);

	return rc;
}


//
// When setting the frame rate, we have to be sure we also 
// enable frame rate (by clearing the FEATURE_FLAG_OFF flag).
//
PXL_RETURN_CODE
EnableAndSetFrameRate(HANDLE hCamera, float newFrameRate)
{
	U32 flags;
	float frameRate;
	U32 numParams = 1;
	PXL_RETURN_CODE rc;

	rc = PxLGetFeature(hCamera, FEATURE_FRAME_RATE, &flags, &numParams, &frameRate);
	if (!API_SUCCESS(rc)) {
		return rc;
	}

	frameRate = newFrameRate;
	flags &= (~FEATURE_FLAG_OFF);

	rc = PxLSetFeature(hCamera, FEATURE_FRAME_RATE, flags, numParams, &frameRate);

	return rc;
}
