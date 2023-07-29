//
// camerautils.cpp
//
// This has some camera utilities.
//
//
#include <PixeLINKApi.h>
#include "CameraUtils.h"
#include <vector>
#include <assert.h>
#define ASSERT(x) do { assert((x)); } while(0)

// Local function protoypes
PXL_RETURN_CODE	SetTriggering(HANDLE hCamera, int mode, int triggerType, int polarity, float delay, float param);

//
// Taken from 'triggerwithcontrolledlight' demo app.
//
PXL_RETURN_CODE 
EnableSoftwareTriggering(const HANDLE hCamera)
{
	return SetTriggering(hCamera, 
		0,								// Mode 0 Triggering
		TRIGGER_TYPE_SOFTWARE, 
		POLARITY_ACTIVE_LOW, 
		0.0,							// no delay
		0);								// unused for Mode 0
}

PXL_RETURN_CODE 
EnableHardwareTriggering(const HANDLE hCamera)
{
	return SetTriggering(hCamera, 
		0,								// Mode 0 Triggering
		TRIGGER_TYPE_HARDWARE, 
		POLARITY_ACTIVE_LOW, 
		0.0,							// no delay
		0);								// unused for Mode 0
}


//
// Set up the camera for triggering, and, enable triggering.
// Taken from the 'triggering' demo app.
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

static float 
GetMinGpioDuration(const HANDLE hCamera)
{
	U32 bufferSize(0);
	PXL_RETURN_CODE rc = PxLGetCameraFeatures(hCamera, FEATURE_GPIO, NULL, &bufferSize);
	ASSERT(API_SUCCESS(rc));
	ASSERT(bufferSize > 0);

	std::vector<U8> buffer(bufferSize,0);
	CAMERA_FEATURES* pFeatureInfo = (CAMERA_FEATURES*)&buffer[0];
	rc = PxLGetCameraFeatures(hCamera, FEATURE_GPIO, pFeatureInfo, &bufferSize);
	ASSERT(API_SUCCESS(rc));
	ASSERT(buffer.size() == bufferSize);

	return pFeatureInfo->pFeatures[0].pParams[FEATURE_GPIO_MODE_STROBE_PARAM_DURATION].fMinValue;
	
}

PXL_RETURN_CODE 
EnableGpoForStrobe(const HANDLE hCamera, const int gpoNumber)
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
	params[FEATURE_GPIO_PARAM_MODE]		= (float)GPIO_MODE_STROBE;
	params[FEATURE_GPIO_PARAM_POLARITY]  = (float)POLARITY_POSITIVE;
	params[FEATURE_GPIO_MODE_STROBE_PARAM_DELAY]	= 0.0f;
	params[FEATURE_GPIO_MODE_STROBE_PARAM_DURATION] = GetMinGpioDuration(hCamera);

	// Enable the GPO
	flags = ENABLE_FEATURE(flags, true);

	rc = PxLSetFeature(hCamera, FEATURE_GPIO, flags, numParams, &params[0]);

	return ApiSuccess;
}

