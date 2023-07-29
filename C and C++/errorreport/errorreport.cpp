//
// A simple demonstration of how to get more information about the
// most recent error reported by the PixeLINK API. 
//
//

#include <PixeLINKApi.h>
#include <stdio.h>

//
// Get information about the most recent PixeLINK API error.
//
static void 
PrintLastPixeLINKApiError(HANDLE hCamera)
{
	// Get information about the error
	ERROR_REPORT errorReport;
	PxLGetErrorReport(hCamera, &errorReport);

	// Get information about the camera
	// (Don't have to, but just for reporting purposes)
	CAMERA_INFO	cameraInfo;
	PxLGetCameraInfo(hCamera, &cameraInfo);

	printf("--- Error Report ---\n", cameraInfo.SerialNumber, hCamera);
	printf("Camera      : %s\n", cameraInfo.SerialNumber);
	printf("Camera name : %s\n", cameraInfo.CameraName);
	printf("Handle      : 0x%8.8X:\n", hCamera);
	printf("Function    : %s\n", errorReport.strFunctionName);
	printf("Return code : 0x%8.8X (%s)\n", errorReport.uReturnCode, errorReport.strReturnCode);
	printf("Reason      : %s\n", errorReport.strReport);
	printf("\n");

}

int 
main(int argc, char* argv[])
{
	HANDLE hCamera;
	PXL_RETURN_CODE rc;

	// Initialize a camera
	rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to initialize a camera. (0x%8.8X)\n");
		return EXIT_FAILURE;
	}


	// Show that the error report for the camera is set to 0 with empty strings
	printf("Showing the default error report\n");
	PrintLastPixeLINKApiError(hCamera);


	// Now do something that's quite valid: read the current gain value.
	U32 flags;
	U32 numParams = 1;
	float gain;
	rc = PxLGetFeature(hCamera, FEATURE_GAIN, &flags, &numParams, &gain);
	printf("Reading FEATURE_GAIN returned 0x%8.8X\n", rc);
	PrintLastPixeLINKApiError(hCamera);


	// Set the gain to an invalid value
	float bogusGain = 1E7;
	rc = PxLSetFeature(hCamera, FEATURE_GAIN, flags, numParams, &bogusGain);
	printf("Setting FEATURE_GAIN to %f returned 0x%8.8X\n", bogusGain, rc);
	PrintLastPixeLINKApiError(hCamera);


	// Set the gain to a value we know is supported.
	// Note that this call to PxLSetFeature returns success, but 
	// PxLGetErrorReport reports the last *error* seen by the API for
	// the specified camera, not the last return code returned.
	rc = PxLSetFeature(hCamera, FEATURE_GAIN, flags, numParams, &gain);
	printf("Setting FEATURE_GAIN to %f returned 0x%8.8X\n", gain, rc);
	PrintLastPixeLINKApiError(hCamera);

	PxLUninitialize(hCamera);

	return EXIT_SUCCESS;
}

