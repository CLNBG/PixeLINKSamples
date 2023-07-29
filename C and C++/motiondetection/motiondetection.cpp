//
//
// motiondetection.cpp : Defines the entry point for the console application.
//
// A demo application to show how to do very simple motion detection.
// When motion is detected, an image is saved.
//

#include <cstdio>
#include <PixeLINKApi.h>
#include "Camera.h"
#include "MotionDetectingThread.h"
#include <cassert>
#define ASSERT(x)	do { assert((x)); } while(0)


//
// Local function prototypes
//
static void	ProcessCommands(Camera&, MotionDetectingThread&);



//
// Optional parameters: 
// pixel delta threshold: When a pixel changes by this value (or more) from one frame to the next, it is considered to be changed.
// percent changed threshold: When this percentage of pixels in the image has changed, movement is considered to be detected.
//
enum {
	PARAM_EXE_NAME = 0,
	PARAM_PIXEL_DELTA,
	PARAM_PERCENT_CHANGED,
};

int 
main(int argc, char* argv[])
{
	::printf("Motion Detection V1.5\n");

	// Provide our default values
	U8 pixelDeltaThreshold = 20;
	float percentChangedThreshold = 10.0f;

	// Did the user specify any parameters?
	if (argc > PARAM_PIXEL_DELTA) {
		const int temp = ::atoi(argv[PARAM_PIXEL_DELTA]);
		// Assumes we're using 8-bit pixels
		if ((temp <= 0) || (temp > 255)) {
			::printf("ERROR: the pixel delta threshold is out of range;\n");
			return EXIT_FAILURE;
		}
		pixelDeltaThreshold = temp;
	}

	if (argc > PARAM_PERCENT_CHANGED) {
		const float temp = static_cast<float>(::atof(argv[PARAM_PERCENT_CHANGED]));
		if ((temp <= 0.0f) || (temp >= 100.0f))  {
			::printf("ERROR: the percent changed threshold is out of range;\n");
			return EXIT_FAILURE;
		}
		percentChangedThreshold = temp;
	}

	::printf("Using a pixel delta threshold of %d\n", pixelDeltaThreshold);
	::printf("Using an percentage threshold of %3.1f%%\n", percentChangedThreshold);


	// Let's see if we can initialize a camera.
	HANDLE hCamera(0);
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		::printf("Unable to initialize a camera (0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	// Prepare the camera
	Camera camera(hCamera);
	camera.ConfigureFor8Bit();
	camera.StartStreaming();

	MotionDetectingThread worker(camera, pixelDeltaThreshold, percentChangedThreshold, IMAGE_FORMAT_JPEG, std::string("jpg"));
	if (worker.StartRunning()) {
		ProcessCommands(camera, worker);
		worker.RequestTermination();
		worker.WaitForThreadToFinish();
	}

	PxLUninitialize(hCamera);
	return EXIT_SUCCESS;
}

//
// This function will not return until a key is pressed.
// The user does not have to hit enter, unlike getc().
//
U32
WaitForAnyKey()
{
	DWORD savedMode;
	CHAR c = 0;
	DWORD numRead = -1;

	HANDLE hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
	if (INVALID_HANDLE_VALUE == hConsoleIn) {
		ASSERT(0);
		return 0;
	}

	/* Save the current mode so we can restore it later */
	GetConsoleMode(hConsoleIn, &savedMode);

	/* Set up the console to not require Enter to be pressed */
	SetConsoleMode(hConsoleIn, ENABLE_PROCESSED_INPUT);

	/* Clear the buffer and wait for a character */
	FlushConsoleInputBuffer(hConsoleIn);
	
	ReadConsole(hConsoleIn, &c, 1 * sizeof(c), &numRead, NULL);

	/* Restore the console input mode */
	SetConsoleMode(hConsoleIn, savedMode);

	return c;
}

//
// Process commands from the console from the user.
//
static void
ProcessCommands(Camera& camera, MotionDetectingThread& worker)
{
	char const * const CURRENT_IMAGE_NAME = "current.jpg";
	const U32 CURRENT_IMAGE_FORMAT = IMAGE_FORMAT_JPEG;
	PXL_RETURN_CODE rc;

	::printf("Waiting for a command...\n");
	bool keepProcessing = true;
	while(worker.IsRunning() && keepProcessing) 
	{
		const U32 key = WaitForAnyKey();
		switch(key)
		{
			case 'q':
			case 'Q':
			case 'x':
			case 'X':
				keepProcessing = false;
				break;

			// Toggle verbosity
			case 'v': 
				worker.SetVerbose(!worker.IsVerbose());
				break;

			case 's':
				::printf("Taking a snapshot ...\n");
				rc = camera.Capture(CURRENT_IMAGE_NAME, CURRENT_IMAGE_FORMAT);
				if (!API_SUCCESS(rc)) {
					::printf("Unable to capture an image (0x%8.8X)\n", rc);
				} else {
					::printf("Image saved to %s\n", CURRENT_IMAGE_NAME);
				}
				break;

			// Toggle the preview
			case 'p':
				camera.EnablePreview(!camera.IsPreviewing());
				break;
		}
	}
	::printf("Exiting...\n");
	return;
}


