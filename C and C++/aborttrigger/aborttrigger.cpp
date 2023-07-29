//
// A demonstration of a few noteworthy aspects concerning the capture of images, and hardware triggering. 
// 
// Assuming the camera is configured for hardware triggering:
//
// 1) A thread calling PxLGetNextFrame will not return from PxLGetNextFrame until a triggered image is returned.
//
// 2) If a thread is NOT blocked in PxLGetNextFrame when an image is triggered, that image will be missed; triggered images are
//    not buffered. PxLGetNextFrame will only return the NEXT frame available.
// 
// 3) Q) How can you get a thread blocked in PxLGetNextFrame to return when you want to shut the app down, or just stop capturing images? 
//    A) Just stop streaming. PxLGetNextFrame will return the error ApiStreamStopped.
//
// 4) Using a callback (CALLBACK_FRAME) will also potentially miss frames if a trigger happens while
//    the callback function is still executing user code. 
//
// 5) It is not possible to capture images using PxLGetNextFrame *and* CALLBACK_FRAME callbacks at the same time.
//
//  This demonstration application can be configured to:
//
//	A) Capture images using a child thread dedicated to capturing images
//	or 
//	B) Capture images using an CALLBACK_FRAME callback function
//	or
//	C) Try both of the above at the same time, which doesn't work. This is for demonstration purposes only.
//
//	Hardware triggering can be enabled if the define ENABLE_HARDWARE_TRIGGING is defined.
//
//	When an image is captured, the function PretendToDoALotOfWork is called to simulate an intensive, time-consuming
//	operation on the image, or as a result of having received an image. The intention is to demonstrate how -- when 
//	hardware triggering is enabled -- there is the potential to miss triggered images. You can see this by triggering the 
//	camera while PretendToDoALotOfWork is being called.
//
//


#include <PixeLINKApi.h>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <process.h>

//
// Use these defines to configure this application.
//
//#define CAPTURE_USING_CALLBACK
#define CAPTURE_USING_THREAD
#define ENABLE_HARDWARE_TRIGGERING
//#define ABORT_TRIGGERING_BY_DISABLING_TRIGGERING

// Just to keep things simple, use a buffer size large enough for a 24 MegaPixel camera, 16-bit format.
#define MAX_IMAGE_SIZE (5000*5000*2)



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Local function prototypes
static PXL_RETURN_CODE	RunTest(HANDLE hCamera);
static PXL_RETURN_CODE	InitializeCamera(HANDLE hCamera);
static PXL_RETURN_CODE	EnableHardwareTriggering(HANDLE hCamera, U32 mode, U32 polarity, float delay, float param);
static PXL_RETURN_CODE	DisableTriggering(HANDLE hCamera);
static void				PretendToDoALotOfWork();
static U32 __stdcall	ImageCaptureCallback(HANDLE hCamera, void* pFrameBuffer, U32 dataFormat, FRAME_DESC* pFrameDesc, void* pContext);
unsigned int __stdcall	ImageCapturingThread(void* pArguments);

int 
main(int argc, char* argv[])
{
	// Initialize any camera. 
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to initialize a camera (0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	RunTest(hCamera);

	PxLUninitialize(hCamera);
	return EXIT_SUCCESS;
}


//
// Configure and enable hardware trigging on the camera.
// Assumes your camera supports hardware triggering. 
//
static PXL_RETURN_CODE 
EnableHardwareTriggering(HANDLE hCamera, U32 mode, U32 polarity, float delay, float param)
{
	U32 numParams = FEATURE_TRIGGER_NUM_PARAMS;
	float params[FEATURE_TRIGGER_NUM_PARAMS];
	U32 flags;

	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_TRIGGER, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) {
		return rc;
	}

	params[FEATURE_TRIGGER_PARAM_TYPE]		= (float)TRIGGER_TYPE_HARDWARE;
	params[FEATURE_TRIGGER_PARAM_MODE]		= (float)mode;
	params[FEATURE_TRIGGER_PARAM_POLARITY]	= (float)polarity;
	params[FEATURE_TRIGGER_PARAM_DELAY]		= delay;
	params[FEATURE_TRIGGER_PARAM_PARAMETER]	= param;

	flags = ENABLE_FEATURE(flags, true);

	rc = PxLSetFeature(hCamera, FEATURE_TRIGGER, flags, numParams, &params[0]);
	return rc;

}

static PXL_RETURN_CODE 
DisableTriggering(HANDLE hCamera)
{
	U32 numParams = FEATURE_TRIGGER_NUM_PARAMS;
	float params[FEATURE_TRIGGER_NUM_PARAMS];
	U32 flags;

	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_TRIGGER, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) {
		return rc;
	}

	flags = ENABLE_FEATURE(flags, false);

	rc = PxLSetFeature(hCamera, FEATURE_TRIGGER, flags, numParams, &params[0]);
	return rc;
}


//
// Callback function that is called when an image is captured.
// 
static U32 __stdcall 
ImageCaptureCallback(HANDLE hCamera, void* pFrameBuffer, U32 dataFormat, FRAME_DESC* pFrameDesc, void* pContext)
{
	printf("ImageCaptureCallback called\n");
	printf("\tthread id    = %d\n",			GetCurrentThreadId());
	printf("\tpFrameBuffer = 0x%8.8X\n",	(U32)pFrameBuffer);
	printf("\tdataFormat   = %d\n",			dataFormat);
	printf("\tpFrameDesc   = 0x%8.8X\n",	(U32)pFrameDesc);
	printf("\t\tpFrameDesc->uFrameNumber %d\n", pFrameDesc->uFrameNumber);
	printf("\tpContext     = 0x%8.8X\n",	(U32)pContext);
	printf("\n");

	PretendToDoALotOfWork();

	return ApiSuccess;
}

static PXL_RETURN_CODE
InitializeCamera(HANDLE hCamera)
{
	PXL_RETURN_CODE rc;

	printf("Initializing camera:\n");

	// Load factory settings so we know what state the camera is in.
	printf("\tLoading factory settings...\n");
	rc = PxLLoadSettings(hCamera, FACTORY_DEFAULTS_MEMORY_CHANNEL);
	if (!API_SUCCESS(rc)) return rc;

#ifdef ENABLE_HARDWARE_TRIGGERING
	printf("\tEnabling hardware triggering...\n");
	rc = EnableHardwareTriggering(hCamera, TRIGGER_MODE_0, POLARITY_POSITIVE, 0.0f, 0.0f);
	if (!API_SUCCESS(rc)) return rc;
#endif

#ifdef CAPTURE_USING_CALLBACK
	// Set callback such that our function is called each and every time a frame is received.
	printf("\tSetting callback function...\n");
	rc = PxLSetCallback(hCamera, CALLBACK_FRAME, (void*)0xAB1DE, ImageCaptureCallback); // The Dude abides.
	if (!API_SUCCESS(rc)) return rc;
#endif

	printf("\tStarting streaming...\n");
	rc = PxLSetStreamState(hCamera, START_STREAM);
	if (!API_SUCCESS(rc)) return rc;

	printf("Camera initialized.\n");
	return rc;
}


// Used by the main thread to tell the child thread when to stop running
static volatile bool s_keepRunning = true;

unsigned int __stdcall 
ImageCapturingThread(void* pArguments)
{
	HANDLE hCamera = (HANDLE) pArguments;
	std::vector<U8> frame(MAX_IMAGE_SIZE,0);
	FRAME_DESC frameDesc;
	U32 lastFrameCaptured = (U32)-1;

	printf("ImageCapturingThread (thread id %d) is running\n", GetCurrentThreadId());

	while(s_keepRunning) {

		// Grab a frame
		frameDesc.uSize = sizeof(frameDesc);
		PXL_RETURN_CODE rc = PxLGetNextFrame(hCamera, (U32)frame.size(), &frame[0], &frameDesc);
		if (!API_SUCCESS(rc)) {
			ERROR_REPORT errorReport;
			PxLGetErrorReport(hCamera, &errorReport);
			printf("\nPxLGetNextFrame returned rc = 0x%8.8X (%s, %s)\n", rc, errorReport.strReturnCode, errorReport.strReport);
		} else {

			printf("\nPxLGetNextFrame returned rc = 0x%8.8X, frame number = %d\n", rc, frameDesc.uFrameNumber);
		
			// Check the frame number to see if any frames were missed.
			if (lastFrameCaptured == (U32)-1) {
				lastFrameCaptured = frameDesc.uFrameNumber;
			} else {
				U32 numMissedFrames = frameDesc.uFrameNumber - lastFrameCaptured - 1;
				lastFrameCaptured = frameDesc.uFrameNumber;
				if (numMissedFrames > 0) {
					printf("%d frame%s missed\n", numMissedFrames, (1 == numMissedFrames) ? "" : "s");
				}
			}
		}

		if (!s_keepRunning) {
			break;
		}

		PretendToDoALotOfWork();

	}
	printf("Child thread exiting.\n");

	return 0;
}


static PXL_RETURN_CODE 
RunTest(HANDLE hCamera)
{
	PXL_RETURN_CODE rc;

	printf("Main thread's thread id = %d\n\n", GetCurrentThreadId());

	rc = InitializeCamera(hCamera);
	if (!API_SUCCESS(rc)) return rc;

#ifdef CAPTURE_USING_THREAD
	unsigned int threadId;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ImageCapturingThread, (void*)hCamera, 0, &threadId);
#endif

	printf("\nRUNNING - HIT ENTER TO QUIT\n\n");
	getchar();

#ifdef CAPTURE_USING_THREAD
	// Tell the thread to stop running and stop the stream so that 
	// it 'fails' out of PxLGetNextFrame
	printf("Telling child thread to stop running...\n");
	s_keepRunning = false;
#endif

#ifdef ABORT_TRIGGERING_BY_DISABLING_TRIGGERING
	// Not the best way to do it, but it will work.
	// PxLGetNextFrame will return either an error, or (erroneously) ApiSuccess, but not ApiStreamStopped.
	printf("Disabling triggering...");
	DisableTriggering(hCamera);
	printf("done.\n");
	Sleep(2000);
#endif

	// Stop streaming 
	printf("Stopping streaming...");
	rc = PxLSetStreamState(hCamera, STOP_STREAM);
	printf("done.\n");

#ifdef CAPTURE_USING_THREAD
	// Wait for the child thread to exit
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	printf("Child thread is done.");
#endif

	return ApiSuccess;

}

static void				
PretendToDoALotOfWork()
{
	// Here's where we pretend to be busy processing an image, and potentially missing frames as a result.
	printf("Working...\n");
	Sleep(5*1000);
	printf("Done working.\n");
}