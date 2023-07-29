//
// Clarifying a few descriptor issues on PixeLINK cameras that support descriptors.
// Specifically, what happens to feature values when creating and removing descriptors.
//
#include <PixeLINKApi.h>
#include <cassert>
#include <cstdio>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A few helper functions
//
#define ASSERT(x) do { assert((x)); } while(0)

static float
GetExposure(const HANDLE hCamera)
{
	U32 flags(0);
	U32 numParams(1);
	float exposure(0.0f);
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_SHUTTER, &flags, &numParams, &exposure);
	ASSERT(API_SUCCESS(rc));

	return exposure;
}

static float
SetExposure(const HANDLE hCamera, const float newExposure)
{
	U32 flags(0);
	U32 numParams(1);
	float exposure(0.0f);
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_SHUTTER, &flags, &numParams, &exposure);
	ASSERT(API_SUCCESS(rc));

	exposure = newExposure;
	rc = PxLSetFeature(hCamera, FEATURE_SHUTTER, flags, numParams, &exposure);
	return exposure;
}

static void
DoBriefPreview(const HANDLE hCamera, char const * const pMsg)
{
	PXL_RETURN_CODE rc = PxLSetStreamState(hCamera, START_STREAM);
	ASSERT(API_SUCCESS(rc));
	rc = PxLSetPreviewSettings(hCamera, pMsg);
	HWND hPreviewWnd;
	rc = PxLSetPreviewState(hCamera, START_PREVIEW, &hPreviewWnd);
	ASSERT(API_SUCCESS(rc));
	Sleep(5 * 1000);
	rc = PxLSetPreviewState(hCamera, STOP_PREVIEW, &hPreviewWnd);
	ASSERT(API_SUCCESS(rc));
    rc = PxLSetStreamState(hCamera, STOP_STREAM);
	ASSERT(API_SUCCESS(rc));

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Experimenting with descriptors 
//

int 
main(int argc, char* argv[])
{
	// Initialize (connect to) a camera.
	// We're assuming here that you're using a PixeLINK camera that supports descriptors.
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	ASSERT(API_SUCCESS(rc));

	// Set the camera into a known state.
	rc = PxLLoadSettings(hCamera, PXL_SETTINGS_FACTORY);
	ASSERT(API_SUCCESS(rc));
	Sleep(1000);

	// Read the current (factory default) exposure
	const float initialExposure = GetExposure(hCamera);
	printf("Initial exposure: %f\n", initialExposure);

	// Do a brief preview to see what we're getting from the 
	// camera at factory settings.
	char buf[128];
	sprintf(buf, "Factory Settings (Exposure = %f)", initialExposure);
	DoBriefPreview(hCamera, buf);
	

	// Create a descriptor (it will become the current descriptor) and query the exposure.
	// It will be the same as the initial exposure because the descriptor is initialized using the current settings.
	HANDLE hDescriptor1;
	rc = PxLCreateDescriptor(hCamera, &hDescriptor1, PXL_UPDATE_CAMERA);
	ASSERT(API_SUCCESS(rc));
	float desc1Exposure = GetExposure(hCamera);
	ASSERT(desc1Exposure == initialExposure);
	// And set the exposure for this descriptor to double the factory setting
	desc1Exposure *= 2;
	printf("Descriptor 1 Exposure: %f\n", desc1Exposure);
	SetExposure(hCamera, desc1Exposure);
	ASSERT(GetExposure(hCamera) == desc1Exposure);

	// Do a brief preview to see what we're getting from the camera now.
	// The image won't flicker because we're only using one descriptor right now.
	sprintf(buf, "Using One Descriptor (Exposure = %f)", desc1Exposure);
	DoBriefPreview(hCamera, buf);


	// Create a second descriptor and double the exposure yet again.
	HANDLE hDescriptor2;
	rc = PxLCreateDescriptor(hCamera, &hDescriptor2, PXL_UPDATE_CAMERA);
	ASSERT(API_SUCCESS(rc));
	float desc2Exposure = GetExposure(hCamera);
	ASSERT(desc2Exposure == desc1Exposure);
	desc2Exposure *= 2;
	printf("Descriptor 2 Exposure: %f\n", desc2Exposure);
	SetExposure(hCamera, desc2Exposure);
	ASSERT(GetExposure(hCamera) == desc2Exposure);

	// Do a brief preview to see what we're getting from the camera now.
	// The image will flicker because we're using two descriptors now.
	sprintf(buf, "Using Two Descriptor (Exposures: %f, %f)", desc1Exposure, desc2Exposure);
	DoBriefPreview(hCamera, buf);


	// And destroy the two descriptors and query the camera for the exposure.
	// Note that the order of removal determines the values used when all descriptors are removed.
#define REMOVE_1_THEN_2
#ifdef  REMOVE_1_THEN_2
	printf("Removing descriptor 1 then descriptor 2...\n");
	rc = PxLRemoveDescriptor(hCamera, hDescriptor1);
	ASSERT(API_SUCCESS(rc));
	rc = PxLRemoveDescriptor(hCamera, hDescriptor2);
	ASSERT(API_SUCCESS(rc));
#else
	printf("Removing descriptor 2 then descriptor 1...\n");
	rc = PxLRemoveDescriptor(hCamera, hDescriptor2);
	ASSERT(API_SUCCESS(rc));
	rc = PxLRemoveDescriptor(hCamera, hDescriptor1);
	ASSERT(API_SUCCESS(rc));
#endif
	printf("Exposure now: %f\n", GetExposure(hCamera));


	PxLUninitialize(hCamera);
	ASSERT(API_SUCCESS(rc));

	return EXIT_SUCCESS;
}

