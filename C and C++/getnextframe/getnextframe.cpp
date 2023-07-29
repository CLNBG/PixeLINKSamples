//
// getnextframe.cpp 
//
// A demonstration of a robust wrapper around PxLGetNextFrame.
//
//

#include <PixeLINKApi.h>
#include <cstdio>
#include <cassert>
#include <vector>

// Just to keep things simple, use a buffer size large enough for a 24 MegaPixel camera, 16-bit format.
#define MAX_IMAGE_SIZE (5000*5000*2)


#define ASSERT(x)	do { assert((x)); } while(0)

//
// Local Functions
//
static PXL_RETURN_CODE GetNextFrame(HANDLE hCamera, U32 frameBufferSize, void* pFrameBuffer, FRAME_DESC* pFrameDesc, U32 maximumNumberOfTries);



int 
main(const int argc, char const * const argv[])
{
	// Initialize any camera
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("Error: Unable to initialize a camera\n");
		return EXIT_FAILURE;
	}

	// Just going to declare a very large buffer here
	// One that's large enough for any PixeLINK 4.0 camera
	std::vector<U8> frameBuffer(MAX_IMAGE_SIZE);
	FRAME_DESC frameDesc;

	if (API_SUCCESS(PxLSetStreamState(hCamera, START_STREAM))) {
		for(int i=0; i < 30; i++) {
			frameDesc.uSize = sizeof(frameDesc);
			rc = GetNextFrame(hCamera, (U32)frameBuffer.size(), &frameBuffer[0], &frameDesc, 5);
			printf("GetNextFrame returned 0x%8.8X\n", rc);
			if (API_SUCCESS(rc)) {
				printf("\tframe number %d\n", frameDesc.uFrameNumber);
			}
			printf("\n");
		}
	}


	rc = PxLSetStreamState(hCamera, STOP_STREAM);
	ASSERT(API_SUCCESS(rc));
	rc = PxLUninitialize(hCamera);
	ASSERT(API_SUCCESS(rc));

	return EXIT_SUCCESS;
}


//
// A robust wrapper around PxLGetNextFrame.
// This will handle the occasional error that can be returned by the API
// because of timeouts. 
//
// Note that this should only be called when grabbing images from 
// a camera NOT currently configured for triggering. 
//
static PXL_RETURN_CODE
GetNextFrame(const HANDLE hCamera, const U32 frameBufferSize, void* const pFrameBuffer, FRAME_DESC* const pFrameDesc, const U32 maximumNumberOfTries)
{
	ASSERT(NULL != pFrameDesc);

	// Record the frame desc size in case we need it later
	const U32 frameDescSize = pFrameDesc->uSize;

	PXL_RETURN_CODE rc = ApiUnknownError;

	for(U32 i=0; i < maximumNumberOfTries; i++) {
		rc = PxLGetNextFrame(hCamera, frameBufferSize, pFrameBuffer, pFrameDesc);
		if (API_SUCCESS(rc)) {
			return rc;
		} else {
			// If the streaming is turned off, no sense in continuing.
			if (ApiStreamStopped == rc) {
				return rc;
			} else {
				// Is the camera still connected? Try reading the exposure.
				// If the user cannot disconnect the camera, you can skip this.
				U32 flags;
				float exposure;
				U32 numParams = 1;
				PXL_RETURN_CODE rcExposure = PxLGetFeature(hCamera, FEATURE_SHUTTER, &flags, &numParams, &exposure);
				if (!API_SUCCESS(rcExposure)) {
					return rcExposure;
				}
			}
		}
		// Camera's still there, so maybe we just hit a bubble in the frame pipeline.
		// Reset the frame descriptor uSize field (in case the API is newer than what we were compiled with) and try PxLGetNextFrame again.
		pFrameDesc->uSize = frameDescSize;
	}

	// Ran out of tries, so return whatever the last error was.
	return rc;
}
