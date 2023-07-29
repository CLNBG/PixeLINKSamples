//
// callbackclip.cpp 
//
// A demonstration of the use of:
// 
// PxLGetClip     - to capture video to a PixeLINK data stream (PDS) file
// PxLFormatClip  - to convert a PDS file to an AVI file
// PxLSetCallback - to use a callback function to modify PSD frames as they're 
//                  being converted to an AVI by PxLFormatClip
//

#include <PixeLINKAPI.h>
#include <stdio.h>
#include <assert.h>
#define ASSERT(x)	do { assert((x)); } while (0)


// Local function prototypes
static void	DoCallback(HANDLE hCamera);
static U32 _stdcall GetClipTerminationFunction(HANDLE hCamera, U32 numFramesCaptured, PXL_RETURN_CODE rc);
static U32 _stdcall FormatClipCallbackFunction(HANDLE hCamera, LPVOID pFrameData, U32 dataFormat, FRAME_DESC const * pFrameDesc, LPVOID pContext);




int 
main(int argc, char* argv[])
{
	HANDLE hCamera; 
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (API_SUCCESS(rc)) {
		DoCallback(hCamera);
		PxLUninitialize(hCamera);
	}

	return 0;
}



//
// Callback function for PxLGetClip.
// This function is called when PxLGetClip is finished capturing images to the PDS file.
// PxLGetClip is a non-blocking call.
//
//
static PXL_RETURN_CODE volatile s_getClipReturnCode = -1;

static U32 _stdcall 
GetClipTerminationFunction(HANDLE hCamera, U32 numFramesCaptured, PXL_RETURN_CODE rc)
{
	s_getClipReturnCode = rc;
	return rc;
}


static const int NUM_FRAMES_TO_CAPTURE = 10;
static const char* DATA_STREAM_FILE_NAME = "temp.pds";
static const char* AVI_FILE_NAME = "video.avi";

static void
DoCallback(HANDLE hCamera)
{
	PXL_RETURN_CODE rc = PxLSetStreamState(hCamera, START_STREAM);
	ASSERT(API_SUCCESS(rc));

	// Capture N frames from the camera to a file
	s_getClipReturnCode = -1;
	rc = PxLGetClip(hCamera, NUM_FRAMES_TO_CAPTURE, (LPSTR)DATA_STREAM_FILE_NAME, GetClipTerminationFunction);
	ASSERT(API_SUCCESS(rc));

	// Cheap and easy polling to see when the capture of the 
	// frames to the clip is finished.
	while(-1 == s_getClipReturnCode) {
		Sleep(100);
	}
	ASSERT(API_SUCCESS(s_getClipReturnCode));


	rc = PxLSetStreamState(hCamera, STOP_STREAM);
	ASSERT(API_SUCCESS(rc));


	//
	// Set the callback so that as PxLFormatClip is processing individual frames from the 
	// PDS to convert them into an AVI, our callback function is called, giving us a chance
	// to modify the image.
	// 
	rc = PxLSetCallback(0, CALLBACK_FORMAT_CLIP, 0, FormatClipCallbackFunction);
	ASSERT(API_SUCCESS(rc)); // Older versions of PixeLINK API may fail here

	// Certain versions of the driver have a bug where the 
	// AVI file name passed in is converted to upper-case.
	char tempBufSrc[MAX_PATH];
	strcpy(&tempBufSrc[0], DATA_STREAM_FILE_NAME);

	char tempBufDst[MAX_PATH];
	strcpy(&tempBufDst[0], AVI_FILE_NAME);
	rc = PxLFormatClip((LPSTR)tempBufSrc, (LPSTR)tempBufDst, CLIP_FORMAT_AVI);
	ASSERT(API_SUCCESS(rc));

	return;
}

//
// Callback function for PxLFormatClip.
// Process a frame from the PDS file as just before it's put into an AVI file.
// This will be called with the sa
//
static U32 _stdcall 
FormatClipCallbackFunction(HANDLE hCamera, LPVOID pFrameData, U32 dataFormat, FRAME_DESC const * pFrameDesc, LPVOID pContext)
{
	printf("Current Thread ID = %d\n", GetCurrentThreadId());
	printf("FormatClipCallbackFunction called for camera [0x%8.8X], pFrameData=[0x%8.8X], pContext=[0x%8.8X]\n", (U32)hCamera, (U32)pFrameData, (U32)pContext);

	// 
	// Just put a simple white streak in the bottom left
	//
	U8* pData = (U8*)pFrameData;
	for(U32 i=0; i < 100; i++, pData++) {
		*pData = -1;
	}
	return ApiSuccess;
}
