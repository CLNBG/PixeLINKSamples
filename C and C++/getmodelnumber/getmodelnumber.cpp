//
// getmodelnumber.cpp
//
// A demonstration of how to determine the model number of the PixeLINK camera.
// There is currently no 'official' way using the API to determine the model number, but this is 
// the currently-recommended way.
//
//

#include <PixeLINKApi.h>
#include <stdio.h>
#include <assert.h>

// Local helper function prototypes
static U32 GetModelNumber(HANDLE hCamera);
static U32 SearchStringForModelNumber(const char* pString);


int 
main(int argc, char* argv[])
{
	// Connect to a camera
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("Unable to initialize a camera (0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	const U32 modelNumber = GetModelNumber(hCamera);
	if (0 != modelNumber) {
		printf("Determined the model number to be %d\n", modelNumber);
	} else { 
		printf("Unable to determine the model number\n");
	}

	PxLUninitialize(hCamera);

	return EXIT_SUCCESS;
}


//
// There are two places where we might find the model number:
// 1) CAMERA_INFO.CameraName. The factory default is /PixeLINK PL-[A-Z]\d\d\d[A-Z]/ (using perl regex notation)
//    But, this can be changed by the user using PxLSetCameraName
// 2) CAMERA_INFO.Description. The factory default begins with something similar to the CameraName factory default.
//
static U32
GetModelNumber(const HANDLE hCamera)
{
	CAMERA_INFO ci;
	memset(&ci, 0, sizeof(ci));
	PXL_RETURN_CODE rc = PxLGetCameraInfo(hCamera, &ci);
	if (!API_SUCCESS(rc)) {
		return 0;
	}

	// Is the model number in the CameraName?
	U32 modelNumber = SearchStringForModelNumber(&ci.CameraName[0]);
	if (0 != modelNumber) return modelNumber;

	// If we can't get the model number from the CameraName, perhaps the user has changed the camera name.
	// Let's see if we can get something out of the Description field.
	modelNumber = SearchStringForModelNumber(&ci.Description[0]);
	if (0 != modelNumber) return modelNumber;

	return 0;
}


//
// Looking for something in the form /.*PL-[A-Z]\d\d\d[A-Z].*/ (using perl regex notation).
// For example:  "PixeLINK PL-B778U"
//
// Returns 0 on error.
//
static U32
SearchStringForModelNumber(char const * const pString)
{
	assert(NULL != pString);

	char const * const PREFIX = "PL-"; 
	char const * p = strstr(pString, "PL-"); // .*PL
	if (NULL == p) {
		return 0;
	}
	p += strlen(PREFIX);

	char digits[5];
	if (!isalpha(*p)) return 0; // [A-Z]
	p++;
	if (!isdigit(*p)) return 0;	// \d
	digits[0] = *p++;
	if (!isdigit(*p)) return 0;	// \d
	digits[1] = *p++;
	if (!isdigit(*p)) return 0;	// \d
	digits[2] = *p++;
    if (isalpha(*p)) {
        // It's a 3 digit camera model
    	digits[3] = '\0'; // Terminate digits properly now that we know it's a valid match
    } else {
    	if (! isdigit(*p)) return 0;	// \d
        // It's a 4 digit camera model
        digits[3] = *p++;
    	digits[4] = '\0'; // Terminate digits properly now that we know it's a valid match
    }

	const U32 modelNumber = static_cast<U32>(atoi(&digits[0]));
	return modelNumber;
}