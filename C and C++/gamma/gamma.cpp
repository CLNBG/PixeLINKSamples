//
// gamma.cpp 
//
// This demo application shows:
// 
// 1) How to determine if a feature is supported
// 1) How to determine if a feature can be enabled and disabled
// 1) How to determine if a feature is currently enabled or disabled
// 2) How to enable and disable a feature
// 3) How gamma and lookup table are interconnected
//
// The local helper functions are generic, and will work with any PixeLINK 4.0 camera.
// However, the interaction between gamma and the lookup table is only possible on colour cameras.
//
//
#include <PixeLINKApi.h>
#include <cassert>
#include <vector>

#define ASSERT(x)	do { assert((x)); } while (0)


static U32
GetCameraFeatureFlags(HANDLE hCamera, U32 featureId)
{
	// Determine the buffer size we'll need
	U32 bufferSize;
	PXL_RETURN_CODE rc = PxLGetCameraFeatures(hCamera, featureId, NULL, &bufferSize);
	ASSERT(API_SUCCESS(rc));

	// And now read the info
	std::vector<U8> buffer(bufferSize);
	rc = PxLGetCameraFeatures(hCamera, featureId, (CAMERA_FEATURES*)&buffer[0], &bufferSize);
	ASSERT(API_SUCCESS(rc));

	// Drill our way down to the flags.
	// This could be done with the one line
	// return ((CAMERA_FEATURES*)&buffer[0])->pFeatures[0].uFlags;
	// but we'll do a few checks here along the way
	CAMERA_FEATURES* pFeatures = (CAMERA_FEATURES*)&buffer[0];
	ASSERT(1 == pFeatures->uNumberOfFeatures);

	CAMERA_FEATURE*	pFeature  = pFeatures->pFeatures;
	ASSERT(NULL != pFeature);
	ASSERT(featureId == pFeature->uFeatureId);
	
	return pFeature->uFlags;

}

//
// With the flags returned by PxLGetCameraFeatures, if FEATURE_FLAG_OFF is set, that means
// we can turn this feature on and off
//
static bool
IsFeatureEnableable(HANDLE hCamera, U32 featureId)
{
	U32 flags = GetCameraFeatureFlags(hCamera, featureId);

	if (flags & FEATURE_FLAG_OFF) {
		return true;
	} else {
		return false;
	}
}

//
// With the flags returned by PxLGetCameraFeatures, if FEATURE_FLAG_PRESENCE is set, that means
// the camera supports this feature.
//
static bool
IsFeatureSupported(HANDLE hCamera, U32 featureId)
{
	U32 flags = GetCameraFeatureFlags(hCamera, featureId);

	if (flags & FEATURE_FLAG_PRESENCE) {
		return true;
	} else {
		return false;
	}
}




//
// We ask if a feature is enabled by looking at the FEATURE_FLAG_OFF bit of the feature's flags.
// 
// N.B. To read the feature's flags, we have to successfully read the parameters.
// With some cameras, it so happens that you can read the flags by calling
// PxLGetFeature(hCamera, featureId, &flags, &numParams, NULL);
// but is not guaranteed across all cameras, nor is it guaranteed that it will 
// continue to work in later versions.
//
static bool
IsFeatureEnabled(HANDLE hCamera, U32 featureId)
{
	// Determine the number of parameters
	U32 flags;
	U32 numParams;
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, featureId, &flags, &numParams, NULL);
	ASSERT(API_SUCCESS(rc));

	// Allocate the parameters, and read
	// This time, the flags will be properly set.
	std::vector<float> params(numParams);
	rc = PxLGetFeature(hCamera, featureId, &flags, &numParams, &params[0]);
	ASSERT(API_SUCCESS(rc));

	if (flags & FEATURE_FLAG_OFF) {
		return false;
	} else {
		return true;
	}
}

//
// Enable or disable a feature.
//
// This routine assumes that the feature CAN be enabled. 
//
static void 
EnableFeature(HANDLE hCamera, U32 featureId, bool enable)
{
	// Determine the number of parameters
	// So that we can read the current parameters and flags
	U32 flags;
	U32 numParams;
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, featureId, &flags, &numParams, NULL);
	ASSERT(API_SUCCESS(rc));

	// Read the current parameters and flags
	std::vector<float> params(numParams);
	rc = PxLGetFeature(hCamera, featureId, &flags, &numParams, &params[0]);
	ASSERT(API_SUCCESS(rc));

	// Set or clear the bit accordingly using a macro from PixeLINKTypes.h
	flags = ENABLE_FEATURE(flags, enable);

	// And write back to the camera
	rc = PxLSetFeature(hCamera, featureId, flags, numParams, &params[0]);
	ASSERT(API_SUCCESS(rc));
	ASSERT(IsFeatureEnabled(hCamera, featureId) == enable);

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


int 
main(int argc, char* argv[])
{
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if(!API_SUCCESS(rc)) {
		return EXIT_FAILURE;
	}

	// Check a few assumptions
	// First of all, this demo app only works with colour cameras that
	// allow gamma to be enabled.
	// Some color PixeLINK cameras don't support this.
	ASSERT(IsFeatureSupported(hCamera, FEATURE_GAMMA));
	ASSERT(IsFeatureSupported(hCamera, FEATURE_LOOKUP_TABLE));

	ASSERT( IsFeatureEnableable(hCamera, FEATURE_GAMMA));
	ASSERT( IsFeatureEnableable(hCamera, FEATURE_LOOKUP_TABLE));
	ASSERT(!IsFeatureEnableable(hCamera, FEATURE_GAIN));


	// Start with a clean state
	EnableFeature(hCamera, FEATURE_GAMMA, false);
	EnableFeature(hCamera, FEATURE_LOOKUP_TABLE, false);
	bool gammaIsEnabled = IsFeatureEnabled(hCamera, FEATURE_GAMMA);
	bool lutIsEnabled	= IsFeatureEnabled(hCamera, FEATURE_LOOKUP_TABLE);

	// When enabling gamma, the camera automatically disables the lookup table
	EnableFeature(hCamera, FEATURE_GAMMA, true);
	gammaIsEnabled	= IsFeatureEnabled(hCamera, FEATURE_GAMMA);
	lutIsEnabled	= IsFeatureEnabled(hCamera, FEATURE_LOOKUP_TABLE);
	ASSERT(gammaIsEnabled);
	ASSERT(!lutIsEnabled);

	// Conversely, when enabling the lookup table, the camera automatically disables the gamma
	EnableFeature(hCamera, FEATURE_LOOKUP_TABLE, true);
	gammaIsEnabled	= IsFeatureEnabled(hCamera, FEATURE_GAMMA);
	lutIsEnabled	= IsFeatureEnabled(hCamera, FEATURE_LOOKUP_TABLE);
	ASSERT(!gammaIsEnabled);
	ASSERT(lutIsEnabled);

	PxLUninitialize(hCamera);
	return EXIT_SUCCESS;
}

