//
// dropframe.cpp 
//
// Demonstration of a way to call PxLGetNextFrame to tell the API to drop a frame
// without having to transfer the frame to a caller's buffer. 
//
//
#include <PixeLINKApi.h>
#include <stdio.h>
#include <vector>

// Local function prototypes
static void	DemonstrateDroppingAFrame(HANDLE hCamera);
static PXL_RETURN_CODE	DropAFrame(HANDLE hCamera);


int 
main(int argc, char* argv[])
{
	// Initialize any camera we can find
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to initialize a camera (Error code = 0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	DemonstrateDroppingAFrame(hCamera);

	PxLUninitialize(hCamera);
	return EXIT_SUCCESS;
}


static void
DemonstrateDroppingAFrame(HANDLE hCamera)
{
	PXL_RETURN_CODE rc = PxLSetStreamState(hCamera, START_STREAM);

	// Declare a buffer that's large enough for any camera PixeLINK currently supports.
	std::vector<U8> frame(3000*3000*2,0);
	FRAME_DESC frameDesc;
	memset(&frameDesc, 0xA5, sizeof(frameDesc));

	// Get some frames the 'usual' way.
	// Note that each call to PxLGetNextFrame causes the API to copy the entire frame into our local buffer.
	for(int i=0; i < 10; i++) {
		frameDesc.uSize = sizeof(frameDesc);
		rc = PxLGetNextFrame(hCamera, (U32)frame.size(), &frame[0], &frameDesc);
		printf("GetNextFrame: rc = 0x%8.8X, frame count = %d, roi = %f x %f\n", rc, frameDesc.uFrameNumber, frameDesc.Roi.fWidth, frameDesc.Roi.fHeight);
	}

	// Drop some frames. We don't care about the contents of the images, so shouldn't have to get a copy of the data.
	for(int i=0; i < 10; i++) {
		DropAFrame(hCamera);
	}

	rc = PxLSetStreamState(hCamera, STOP_STREAM);
}

/* 
 * Supported starting with the 7.0 API, we have a better way to drop frames.
 * This does not involve having to copy the frame from the API buffer to the 
 * local buffer. Rather, we can instruct the API to drop a frame by passing
 * in a special buffer size value, -1. 
 *
 * Note that it's possible to drop a frame, but still retrieve the information
 * about the dropped frame by doing this:
 *
 * FRAME_DESC frameDesc;
 * frameDesc.uSize = sizeof(frameDesc);
 * PXL_RETURN_CODE rc = PxLGetNextFrame(hCamera, (U32)-1, NULL, &frameDesc);
 *
 *
 */
static PXL_RETURN_CODE 
DropAFrame(HANDLE hCamera)
{
	FRAME_DESC frameDesc;
	frameDesc.uSize = sizeof(frameDesc);
	PXL_RETURN_CODE rc = PxLGetNextFrame(hCamera, (U32)-1, NULL, &frameDesc);
	printf("Dropped: rc = 0x%8.8X, frame count = %d, roi = %f x %f\n", rc, frameDesc.uFrameNumber, frameDesc.Roi.fWidth, frameDesc.Roi.fHeight);
	return rc;

}
