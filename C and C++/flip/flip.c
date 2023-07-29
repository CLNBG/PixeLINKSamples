//
// flip.c
//
// Flip is an 'unusual' feature because most PixeLINK cameras don't
// support it intrinsically. This means that the API must perform 
// the flipping in software. Because of this, the feature is not
// persistent across reboots. 
// Flip settings will persist as long as you maintain the same
// camera handle, but when you uninitialize a camera, the flip settings are
// lost. (i.e. the API loses the flip setting when it destroys the handle)
//

#include <PixeLINKApi.h>
#include <stdio.h>

//
// Prototypes for local helper functions
//
static PXL_RETURN_CODE	SetFlip(HANDLE hCamera, int horizontal, int vertical);
static PXL_RETURN_CODE	GetFlip(HANDLE hCamera, int* pHorizontal, int* pVertical);
static void	PrintCurrentSettings(HANDLE hCamera);

//
// Used 
//
enum {
	NO_FLIP = 0,
	FLIP = 1
};

int 
main(int argc, char* argv[])
{
	// Initialize a camera, any camera.
	// We're assuming that there's only ONE PixeLINK camera connected to the computer.
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to initialize a camera (Error 0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	// Start by disabling flip and then checking the settings
	printf("Setting to NO_FLIP, NO_FLIP\n");
	rc = SetFlip(hCamera, NO_FLIP, NO_FLIP);
	PrintCurrentSettings(hCamera);

	// Now toggle through the other possibilities
	printf("\nSetting to NO_FLIP, FLIP\n");
	SetFlip(hCamera, NO_FLIP, FLIP);
	PrintCurrentSettings(hCamera);

	printf("\nSetting to FLIP, NO_FLIP\n");
	SetFlip(hCamera, FLIP, NO_FLIP);
	PrintCurrentSettings(hCamera);

	printf("\nSetting to FLIP, FLIP\n");
	SetFlip(hCamera, FLIP, FLIP);
	PrintCurrentSettings(hCamera);

	// Now, with the camera set to FLIP FLIP, uninitialize, re-initialize, then
	// check what the settings are.
	// If the settings are not the same, this demonstrates that the camera being used
	// does not intrinsically support flipping. Rather flipping is being done by the 
	// PixeLINK API.
	printf("\nUninitializing the camera...\n");
	PxLUninitialize(hCamera);
	hCamera = (HANDLE)-1;

	// Initialize again
	// We're assuming there's still only one camera connected to the computer
	printf("\nInitializing the camera again... \n");
	rc = PxLInitialize(0, &hCamera);
	if (API_SUCCESS(rc)) {
		printf("done");
	} else {
		printf("ERROR: Unable to initialize a camera (Error 0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	PrintCurrentSettings(hCamera);

	PxLUninitialize(hCamera);
	return EXIT_SUCCESS;
}

//
// Assuming FLIP takes only these parameters.
// You should confirm this by checking the information in CAMERA_FEATURES or
// by calling PxLGetFeatures with a NULL params pointer, 
// e.g. PxLGetFeature(hCamera, FEATURE_FLIP, &flags, &numParams, NULL)
// 
enum {
	FLIP_PARAM_HORIZONTAL = 0,
	FLIP_PARAM_VERTICAL,
	FLIP_NUM_PARAMS
};

static PXL_RETURN_CODE
SetFlip(HANDLE hCamera, int horizontal, int vertical)
{
	U32 flags = FEATURE_FLAG_MANUAL;
	U32 numParams = FLIP_NUM_PARAMS;
	float params[FLIP_NUM_PARAMS] = { (float)horizontal, (float)vertical };
	return PxLSetFeature(hCamera, FEATURE_FLIP, flags, numParams, &params[0]);
}

static PXL_RETURN_CODE
GetFlip(HANDLE hCamera, int* pHorizontal, int* pVertical)
{
	U32 flags = 0;
	U32 numParams = FLIP_NUM_PARAMS;
	float params[FLIP_NUM_PARAMS];
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_FLIP, &flags, &numParams, &params[0]);
	if (API_SUCCESS(rc)) {
		if (NULL != pHorizontal) {
			*pHorizontal = (int)params[FLIP_PARAM_HORIZONTAL];
		}
		if (NULL != pVertical) {
			*pVertical = (int)params[FLIP_PARAM_VERTICAL];
		}
	}
	return rc;
}

static void
PrintCurrentSettings(HANDLE hCamera)
{
	int hFlip;
	int vFlip;
	PXL_RETURN_CODE rc = GetFlip(hCamera, &hFlip, &vFlip);
	if (API_SUCCESS(rc)) {
		printf("Current settings:\nhorizontal: %sFLIPPED\nvertical  : %sFLIPPED\n",
			(NO_FLIP==hFlip) ? "NOT " : "",
			(NO_FLIP==vFlip) ? "NOT " : "");
	} else {
		printf("ERROR: Unable to read flip settings (Error 0x%8.8X)\n", rc);
	}
}




