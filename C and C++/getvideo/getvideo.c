//
// GetVideo.c
// 
// Use the PixeLINK API to capture images to create an avi.
// There are two steps to this:
// 1) capture images to a PixeLINK data stream (PDS) file
// 2) convert the pds file to an avi file
// 
//
#include <windows.h>
#include <PixeLINKApi.h>
#include <PixeLINKCodes.h>
#include <PixeLINKTypes.h>
#include <assert.h>

#include "getvideo.h"

//
// Captures a specified number of raw frames to a temporary file, then converts that file
// to an avi.
//
// Note that this is not thread safe. (i.e. non-reentrant)
// This function assumes the camera is already in a streaming state.
//
PXL_RETURN_CODE 
GetVideo(HANDLE hCamera, const char* pFilename, U32 numFramesToCapture, U32* pNumFramesCaptured)
{
	char tempFilename[] = "temp.pds";
	PXL_RETURN_CODE retVal = GetRawVideo(hCamera,tempFilename, numFramesToCapture, pNumFramesCaptured);
	if (API_SUCCESS(retVal)) {
		// NOTE: This may take some time.
		retVal = PxLFormatClip(tempFilename, (char*)pFilename, CLIP_FORMAT_AVI);
	}
	return retVal;
}


// Prototype of the callback function
// This function is called when capture of images to the PDS file is finished.
static U32 _stdcall TerminationFunction(HANDLE hCamera, U32 numFramesCaptures, PXL_RETURN_CODE returnCode);

//
// File-scope variables used by TerminationFunction to relay information to the main thread.
//
static volatile int s_isClipCaptureDone;
static U32 s_numFramesCaptured;
static PXL_RETURN_CODE s_returnCode;

//
// Captures the number of raw frames to a file.
//
// Note that this is not thread-safe. (i.e. non-reentrant)
// This function assumes the camera is already in a streaming state
//
PXL_RETURN_CODE
GetRawVideo(HANDLE hCamera, const char* pFilename, U32 numFramesToCapture, U32* pNumFramesCaptured)
{
	PXL_RETURN_CODE retVal;

	assert(0 != hCamera);
	assert(NULL != pFilename);
	assert(numFramesToCapture > 0);

	s_isClipCaptureDone = 0;
	retVal = PxLGetClip(hCamera, numFramesToCapture, (char*)pFilename, TerminationFunction);
	if (!API_SUCCESS(retVal)) {
		return retVal;
	}

	// Simple polling loop, waiting for the termination
	// function to tell us that PxLGetClip is finished (for one reason or another).
	while(0 == s_isClipCaptureDone) {
		Sleep(100);
	}
	
	// Did PxLGetClip successfully complete?
	if (API_SUCCESS(s_returnCode)) {
		if (NULL != pNumFramesCaptured) {
			*pNumFramesCaptured = s_numFramesCaptured;
		}

	}
	return s_returnCode;
}

//
// Callback function
// This is called when the driver has finished capturing video from the camera.
//
static U32 _stdcall 
TerminationFunction(HANDLE hCamera, U32 numFramesCaptured, PXL_RETURN_CODE returnCode)
{
	s_returnCode = returnCode;
	s_numFramesCaptured = numFramesCaptured;
	s_isClipCaptureDone = 1;
	return ApiSuccess;
}