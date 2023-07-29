//
// descriptors_and_knees.cpp
//
// Creates two descriptors in a PL-A741 camera, each with a different exposure and set of kneepoints.
//
//

#include <PixeLINKAPI.h>
#include <stdio.h>
#include <vector>

static bool PrepareCamera(HANDLE);
static void CaptureImages(HANDLE);
static PXL_RETURN_CODE	CreateAndInitializeDescriptor(HANDLE hCamera, HANDLE* phDescriptor, float exposure, std::vector<float>& kneePoints);


int 
main(int argc, char* argv[])
{
	int returnValue = EXIT_FAILURE;

	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("Unable to initialize a camera (Error 0x%8.8X)\n", rc);
		return returnValue;
	}

	if (PrepareCamera(hCamera)) {
		CaptureImages(hCamera);
		returnValue = EXIT_SUCCESS;
	}

	PxLUninitialize(hCamera);
	return returnValue;
}

//
// Prepare the camera by creating two descriptors.
// Note that we do not do anything more with the descriptor handles, but you may 
// want to keep them if you intend to do more interactions with the created descriptors.
//
static bool
PrepareCamera(HANDLE hCamera)
{
	HANDLE hDescriptors[2];
	std::vector<float> kneePoints;

	// Set up the first descriptor
	// The first descriptor has an exposure time of 40 ms, and two kneepoints
	kneePoints.push_back(0.015f); // 15ms into the exposure
	kneePoints.push_back(0.033f); // 33ms into the exposure
	PXL_RETURN_CODE rc = CreateAndInitializeDescriptor(hCamera, &hDescriptors[0], 0.040f, kneePoints);
	if (!API_SUCCESS(rc)) return false;

	// Set up the second descriptor
	kneePoints.resize(0);
	kneePoints.push_back(0.002f); //  2ms into the exposure
	kneePoints.push_back(0.044f); // 44ms into the exposure
	kneePoints.push_back(0.082f); // 82ms into the exposure
	rc = CreateAndInitializeDescriptor(hCamera, &hDescriptors[0], 0.09f, kneePoints);
	if (!API_SUCCESS(rc)) return false;

	return true;
}

static PXL_RETURN_CODE 
CreateAndInitializeDescriptor(HANDLE hCamera, HANDLE* phDescriptor, float exposure, std::vector<float>& kneePoints)
{
	// Create a new descriptor
	HANDLE hTemp;
	PXL_RETURN_CODE rc = PxLCreateDescriptor(hCamera, &hTemp, PXL_UPDATE_CAMERA); // Write through to camera immediately
	if (!API_SUCCESS(rc)) {
		return rc;
	}

	std::vector<float> params(8,0.0f); // array of floats big enough to hold all the calls we'll make (plus some extra)
	U32 numParams;
	U32 flags;

	// Set the exposure
	params[0] = exposure;
	rc = PxLSetFeature(hCamera, FEATURE_SHUTTER, FEATURE_FLAG_MANUAL, 1, &params[0]);
	if (!API_SUCCESS(rc)) {
		return rc;
	}

	//
	// Set up the kneepoints
	// Note that regardless of how many kneepoints we're setting, we have to provide 5 params to PxLSetFeature for FEATURE_EXTENDED_SHUTTER
	// Easiest thing to do is just read the feature with NULL so we know how many params it's expecting.
	//
	rc = PxLGetFeature(hCamera, FEATURE_EXTENDED_SHUTTER, &flags, &numParams, NULL);
	
	// Set the number of knees
    params[0] = (float)kneePoints.size();
	// and copy over the kneepoint values
	for(size_t i=0; i < kneePoints.size(); i++) {
		params[i+1] = kneePoints[i];
	}

	rc = PxLSetFeature(hCamera, FEATURE_EXTENDED_SHUTTER, FEATURE_FLAG_MANUAL, numParams, &params[0]);
	if (!API_SUCCESS(rc)) {
		return rc;
	}

	// Everything's successful so pass the handle back
	*phDescriptor = hTemp;

	return rc;

}

static void
CaptureImages(HANDLE hCamera)
{
	PXL_RETURN_CODE rc;

	// We just display the preview window for a few seconds
	HWND hTemp;
	rc = PxLSetStreamState(hCamera, START_STREAM);
	rc = PxLSetPreviewState(hCamera, START_PREVIEW, &hTemp);

	Sleep(10*1000);

	rc = PxLSetPreviewState(hCamera, STOP_PREVIEW, &hTemp);
	rc = PxLSetStreamState(hCamera, STOP_STREAM);
}




