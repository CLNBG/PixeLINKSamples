//
// descriptors.cpp 
//
// This application demonstrates how to interact with 
// PixeLINK 4.0 PL-A cameras (e.g. PL-A741) to initialize and use descriptors. 
//
// Note that PL-B cameras do not support descriptors.
//
// By changing UPDATE_MODE_TO_USE it's possible to compare the time it takes to 
// configure the descriptor when interacting with the camera directly (PXL_UPDATE_CAMERA)
// vs interacting with the cached versions (PXL_UPDATE_HOST) and then 
// flushing the descriptors through to the camera when ready to stream.
//
//

#include <stdio.h>
#include <PixeLINKApi.h>
#include <vector>
#include "utils.h"

//
// Constants for configuring the test
//
static const U32	UPDATE_MODE_TO_USE = PXL_UPDATE_CAMERA; // PXL_UPDATE_CAMERA or PXL_UPDATE_HOST
static const U32	MAX_NUM_DESCRIPTORS = 10;
static const float	BASE_EXPOSURE =  10.0f  / 1000.0f;
static const float	EXPOSURE_STEP =   5.0f  / 1000.0f;
#define USE_FEATURE_LOOPS
static const int	NUM_TIMES_TO_LOOP = 10;
static const int	PREVIEW_TIME = 5; // seconds

//
// Local prototypes
//
static void	TestDescriptors		(HANDLE hCamera);
static bool	CreateDescriptors	(HANDLE hCamera, std::vector<HANDLE>& hDescriptors, U32 updateMode);
static bool	IncreaseExposures	(HANDLE hCamera, std::vector<HANDLE>& hDescriptors, U32 updateMode, float delta);
static bool	DestroyDescriptors	(HANDLE hCamera, std::vector<HANDLE>& hDescriptors);


int 
main(int argc, char* argv[])
{
	// Initialize any camera
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("Unable to initialize a camera (rc=0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	U64	startTime = PxLGetCurrentTimeMillis();

	TestDescriptors(hCamera);

	U64	totalTime = PxLGetCurrentTimeMillis() - startTime;
	printf("Total time = %lf seconds\n", totalTime / 1000.0);

	PxLUninitialize(hCamera);

	return EXIT_SUCCESS;
}


static void 
TestDescriptors(HANDLE hCamera)
{
	std::vector<HANDLE> hDescriptors;

	U32 updateMode = UPDATE_MODE_TO_USE;
	printf("Using update mode %s\n", (PXL_UPDATE_HOST == updateMode) ? "PXL_UPDATE_HOST" : "PXL_UPDATE_CAMERA");
#ifdef USE_FEATURE_LOOPS
	printf("Feature get/set loop: %d\n", NUM_TIMES_TO_LOOP);
#endif

	Timer timer;
	if (!CreateDescriptors (hCamera, hDescriptors, updateMode)) {
		return;
	}
	timer.PrintElapsedTime("Done creating descriptors");

	IncreaseExposures(hCamera, hDescriptors, updateMode, EXPOSURE_STEP);
	timer.PrintElapsedTime("Done increasing exposures");

	//
	// If we've only been interacting with the host, we have to flush all the 
	// descriptors through to the camera.
	//
	// If we've been interacting with the camera directly, we can skip this step
	//
	if (PXL_UPDATE_CAMERA != updateMode) {
		for(size_t i = 0; i < hDescriptors.size(); i++) {
			PXL_RETURN_CODE rc = PxLUpdateDescriptor(hCamera, hDescriptors[i], PXL_UPDATE_CAMERA);
			ASSERT(API_SUCCESS(rc));
		}
		timer.PrintElapsedTime("Done updating descriptors");
	} else {
		timer.PrintElapsedTime("No need to flush descriptors");
	}
	

	//
	// Do some previewing to ensure the descriptors are behaving as expected
	//
	HWND hPreview;
	PxLSetStreamState(hCamera, START_STREAM);
	PxLSetPreviewState(hCamera, START_PREVIEW, &hPreview);
	printf("Previewing for %d seconds\n", PREVIEW_TIME);
	Sleep(PREVIEW_TIME * 1000);
	PxLSetPreviewState(hCamera, STOP_PREVIEW, &hPreview);
	PxLSetStreamState(hCamera, STOP_STREAM);
	timer.PrintElapsedTime("Done preview");

	// Destroy the descriptors now that we're done with them.
	DestroyDescriptors(hCamera, hDescriptors);
	timer.PrintElapsedTime("Done destroying descriptors");
	
};

//
// Create a number of descriptors, and initialize the exposure to some value
//
// If USE_FEATURE_LOOPS is defined, set get and set the exposure, to simulate
// setting other camera features
//
static bool
CreateDescriptors(HANDLE hCamera, std::vector<HANDLE>& hDescriptors, U32 updateMode)
{
	ASSERT(hDescriptors.size() == 0);

	for(U32 i=0; i < MAX_NUM_DESCRIPTORS; i++) {
		HANDLE hTemp;
		PXL_RETURN_CODE rc = PxLCreateDescriptor(hCamera, &hTemp, updateMode);
		if (!(API_SUCCESS(rc))) {
			printf("Unable to create descriptor #%d (rc=0x%8.8X)\n", i, rc);
			return false;
		}

		PxLSetExposure(hCamera, BASE_EXPOSURE);
#ifdef USE_FEATURE_LOOPS
		for (int j=0; j < NUM_TIMES_TO_LOOP; j++) {
			PxLSetExposure(hCamera, PxLGetExposure(hCamera));
		}
#endif

		hDescriptors.push_back(hTemp);
	}

	return true;
}


//
// Increase the exposure time for each descriptor
//
// If USE_FEATURE_LOOPS is defined, set get and set the exposure, to simulate
// setting other camera features
//

static bool
IncreaseExposures(HANDLE hCamera, std::vector<HANDLE>& hDescriptors, U32 updateMode, float delta)
{

	for(size_t i = 0; i < hDescriptors.size(); i++ ) {
		PXL_RETURN_CODE rc = PxLUpdateDescriptor(hCamera, hDescriptors[i], updateMode);
		if (!API_SUCCESS(rc)) {
			printf("Error selecting descriptor %d (hDescriptor = 0x%8.8X, rc = 0x%8.8X\n", i, (U32)hDescriptors[i], rc);
			return false;
		}

		float exposureTime = PxLGetExposure(hCamera);
		exposureTime += (i * delta);
		PxLSetExposure(hCamera, exposureTime);

#ifdef USE_FEATURE_LOOPS
		for(int j=0; j < NUM_TIMES_TO_LOOP; j++) {
			PxLSetExposure(hCamera, PxLGetExposure(hCamera));
		}
#endif

	}

	return true;
}


//
// Tear down the descriptors we've created
//
static bool
DestroyDescriptors(HANDLE hCamera, std::vector<HANDLE>& hDescriptors)
{
	bool success = true;
	for(size_t i = 0; i < hDescriptors.size(); i++) {
		PXL_RETURN_CODE rc = PxLRemoveDescriptor(hCamera, hDescriptors[i]);
		if (!(API_SUCCESS(rc))) {
			printf("Error removing descriptor %d (hDescriptor = 0x%8.8X, rc = 0x%8.8X\n", i, (U32)hDescriptors[i], rc);
			success = false;
		}
	}

	return success;

}


