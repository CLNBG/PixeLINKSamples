//
// main.c
//
// Initializes a camera (assumes only one connected),
// sets it in a streaming state, captures N frames of video
// then shuts everything down.
// 
//
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <PixeLINKApi.h>
#include <PixeLINKCodes.h>
#include "getvideo.h"

int 
main(int argc, char* argv[])
{
	HANDLE hCamera;
	PXL_RETURN_CODE retVal;

	char filename[] = "video.avi";

	if (!API_SUCCESS(PxLInitialize(0, &hCamera))) {
		return 1;
	}

	if (API_SUCCESS(PxLSetStreamState(hCamera, START_STREAM))) {
		U32 numFramesCaptured = 0;
		U32 numFramesToCapture = 10;

		retVal = GetVideo(hCamera, filename, numFramesToCapture, &numFramesCaptured);

		PxLSetStreamState(hCamera, STOP_STREAM);

		if (API_SUCCESS(retVal)) {
			printf("%d frames stored to %s\n", numFramesCaptured, filename);
		} else {
			printf("Error: 0x%8.8x\n", retVal);
		}
	}

	PxLUninitialize(hCamera);

	return (API_SUCCESS(retVal)) ? 0 : 1;
}

