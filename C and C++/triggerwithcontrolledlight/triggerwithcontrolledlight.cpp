//
// triggerwithcontrolledlight.cpp
// 
// A demonstration of how to configure the camera to use triggering with controlled lighting.
// This demo assumes you're using GPO1 to control the lighting, and that your camera supports FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT
//

#include <PixeLINKApi.h>
#include <cstdio>
#include <cassert>
#include <vector>


////////////////////////////////////////////////////////////////////////
//
// Some helpful constants and defines
//
static const int GPO_CONTROLLING_LIGHT = 1;

// Used as a simple sanity check in this example file
#define ASSERT(x)	do { assert((x)); } while (0)

// File-scope helper functions
//static PXL_RETURN_CODE RightWayToSetTriggerWithControlledLighting(HANDLE hCamera);
//static PXL_RETURN_CODE WrongWayToSetTriggerWithControlledLighting(HANDLE hCamera);
//static PXL_RETURN_CODE SetTriggerWithControlledLighting(HANDLE hCamera, int enable);
static PXL_RETURN_CODE	EnableControlledLighting(HANDLE hCamera, int enable);
static int				IsFeatureSupported(HANDLE hCamera, int featureId);
static PXL_RETURN_CODE SetSoftwareTriggering(HANDLE hCamera);
static PXL_RETURN_CODE SetTriggering(HANDLE hCamera, int mode, int triggerType, int polarity, float delay, float param);
static PXL_RETURN_CODE SetGpoForFlash(HANDLE hCamera, int gpoNumber);



int 
main(const int argc, char const * const argv[])
{
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to initialize a camera (Return code = 0x%8.8X)\n", rc);
		return 1;
	}

	// Check a couple assumptions here
	if (!IsFeatureSupported(hCamera, FEATURE_TRIGGER)) {
		printf("ERROR: This camera doesn't support triggering.\n");
		PxLUninitialize(hCamera);
		return 1;
	}
	if (!IsFeatureSupported(hCamera, FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT)) {
		printf("ERROR: This camera doesn't support triggering with controlled light.\n");
		PxLUninitialize(hCamera);
		return 1;
	}


	rc = SetSoftwareTriggering(hCamera);
	ASSERT(API_SUCCESS(rc));

	EnableControlledLighting(hCamera, 1);
	ASSERT(API_SUCCESS(rc));

	SetGpoForFlash(hCamera, GPO_CONTROLLING_LIGHT);
	ASSERT(API_SUCCESS(rc));

	// Start streaming and perform image capture here.

	PxLUninitialize(hCamera);
	return 0;
}

//
// Set up the camera for software trigging, and enable triggering
//
static PXL_RETURN_CODE 
SetSoftwareTriggering(const HANDLE hCamera)
{
	printf("\nConfiguring the camera for software triggering\n");
	return SetTriggering(hCamera, 
		0,								// Mode 0 Triggering
		TRIGGER_TYPE_SOFTWARE, 
		POLARITY_ACTIVE_LOW, 
		0.0,							// no delay
		0);								// unused for Mode 0
}

//
// Set up the camera for triggering, and, enable triggering.
// Code taken from the 'triggering' demo app.
//
static PXL_RETURN_CODE
SetTriggering(const HANDLE hCamera, const int mode, const int triggerType, const int polarity, const float delay, const float param)
{
	U32 flags;
	U32 numParams = FEATURE_TRIGGER_NUM_PARAMS;
	float params[FEATURE_TRIGGER_NUM_PARAMS];

	// Read current settings
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_TRIGGER, &flags, &numParams, &params[0]);
	if (API_SUCCESS(rc)) {
		ASSERT(FEATURE_TRIGGER_NUM_PARAMS == numParams);

		// Very important step: Enable triggering by clearing the FEATURE_FLAG_OFF bit
		flags = ENABLE_FEATURE(flags, true);

		// Assign the new values...
		params[FEATURE_TRIGGER_PARAM_MODE]		= (float)mode;
		params[FEATURE_TRIGGER_PARAM_TYPE]		= (float)triggerType;
		params[FEATURE_TRIGGER_PARAM_POLARITY]	= (float)polarity;
		params[FEATURE_TRIGGER_PARAM_DELAY]		= delay;
		params[FEATURE_TRIGGER_PARAM_PARAMETER]	= param;

		// ... and write them to the camera
		rc = PxLSetFeature(hCamera, FEATURE_TRIGGER, flags, numParams, &params[0]);
	}

	return rc;
}

//
// Configure a GPO for flash mode, and enable that GPO.
//
static PXL_RETURN_CODE 
SetGpoForFlash(const HANDLE hCamera, const int gpoNumber)
{
	U32 flags(0);
	U32 numParams(0);

	// Determine number of parameters
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_GPIO, &flags, &numParams, NULL);
	if (!API_SUCCESS(rc)) return rc;
	ASSERT(numParams > 0);

	std::vector<float> params(numParams);
	// Specify which GPO we want to enable
	params[FEATURE_GPIO_PARAM_GPIO_INDEX] = (float)gpoNumber;
	rc = PxLGetFeature(hCamera, FEATURE_GPIO, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) return rc;
	ASSERT(params.size() == numParams);

	ASSERT(gpoNumber == params[FEATURE_GPIO_PARAM_GPIO_INDEX]);

	// Now set up the GPO for strobing
	params[FEATURE_GPIO_PARAM_MODE]		= (float)GPIO_MODE_FLASH;
	params[FEATURE_GPIO_PARAM_POLARITY]  = (float)POLARITY_POSITIVE;

	// Enable the GPO
	flags = ENABLE_FEATURE(flags, true);

	rc = PxLSetFeature(hCamera, FEATURE_GPIO, flags, numParams, &params[0]);

	return rc;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// There's a right way, and a wrong way to set up controlled lighting. 
// The right way first sets the controlled light feature (FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT) 
// and THEN sets the triggering. 
//
// The wrong way does it in the opposite order. (i.e. FEATURE_TRIGGER, then FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT).
//
// The reason for this is that the triggering reads FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT setting
// only when triggering is enabled, or re-set.
//
// To remove this dependency upon order, this function:
// 1) reads the current triggering settings
// 2) enables or disables controlled lighting
// 3) sets the trigger based on the values read in step 1.
// 
// Now, all you have to do is make sure that trigging is enabled either before or after calling this function.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
static PXL_RETURN_CODE
EnableControlledLighting(const HANDLE hCamera, const int enable)
{
	// Read the current triggering information
	PXL_RETURN_CODE rc;
	U32 triggerFlags;
	U32 numTriggerParams;
	rc = PxLGetFeature(hCamera, FEATURE_TRIGGER, &triggerFlags, &numTriggerParams, NULL);
	if (!API_SUCCESS(rc)) return rc;
	std::vector<float> triggerParams(numTriggerParams, 0.0f);
	rc = PxLGetFeature(hCamera, FEATURE_TRIGGER, &triggerFlags, &numTriggerParams, &triggerParams[0]);
	if (!API_SUCCESS(rc)) return rc;

	// Now read the current controlled lighting settings
	U32 flags;
	U32 numParams;
	rc = PxLGetFeature(hCamera, FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT, &flags, &numParams, NULL);
	if (!API_SUCCESS(rc)) return rc;
	std::vector<float> params(numParams, 0.0f);
	rc = PxLGetFeature(hCamera, FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) return rc;

	// Enable or disable controlled lighting as required
	params[0] = (float)enable;

	// Write the new controlled lighting settings
	rc = PxLSetFeature(hCamera, FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT, flags, numParams, &params[0]);
	if (!API_SUCCESS(rc)) return rc;

	// Now write the current triggering settings so that controlled lighting setting is read 
	rc = PxLSetFeature(hCamera, FEATURE_TRIGGER, triggerFlags, numTriggerParams, &triggerParams[0]);
	if (!API_SUCCESS(rc)) return rc;

	return rc;
}

//
// Is a particular feature supported by the camera?
//
static int
IsFeatureSupported(const HANDLE hCamera, const int featureId)
{
	// See how big a buffer we need to hold the information we're looking for
	U32 bufferSize(0);
	PXL_RETURN_CODE rc = PxLGetCameraFeatures(hCamera, featureId, NULL, &bufferSize);
	ASSERT(API_SUCCESS(rc));
	ASSERT(bufferSize > 0);

	// Allocate the buffer and read the information
	std::vector<U8> buffer(bufferSize,0);
	CAMERA_FEATURES* pCameraFeatures = (CAMERA_FEATURES*)&buffer[0];
	rc = PxLGetCameraFeatures(hCamera, featureId, pCameraFeatures, &bufferSize);
	ASSERT(API_SUCCESS(rc));
	ASSERT(bufferSize > 0);

	// Is the feature supported?
	return ((pCameraFeatures->pFeatures[0].uFlags & FEATURE_FLAG_PRESENCE) == 0) ? 0 : 1;

}