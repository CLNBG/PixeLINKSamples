//
// This demo app toggles flat-field correction (FFC) in the camera. 
// i.e  If FFC is enabled, it disables it.
//      if FFC is disabled, it enables it.
//
// NOTE: This does not control defective pixel correction
//
// There's not a lot of documentation in this demo app because
// PxLCameraRead and PxLCameraWrite are not functions you 
// should be experimenting or playing with. 
//
//
//
#include <stdio.h>
#include <PixeLINKApi.h>
#include <assert.h>

#define ASSERT(x) do { assert((x)); } while(0)

// Function prototypes
PXL_RETURN_CODE	PxLCameraHasFfcData(HANDLE hCamera, int* pHasFfcData);
PXL_RETURN_CODE	PxLIsFfcEnabled	(HANDLE hCamera, int* pIsFfcEnabled);
PXL_RETURN_CODE PxLEnableFfc	(HANDLE hCamera, int enable);


int 
main(int argc, char* argv[])
{
	int returnValue = EXIT_FAILURE;

	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to initialize a camera (0x%8.8X)\n", rc);
		return returnValue;
	}

	// Toggle FFC
	int hasFfcData = 0;
	rc = PxLCameraHasFfcData(hCamera, &hasFfcData);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to determine if camera has FFC data (0x%8.8X)\n", rc);
	} else {
		if (hasFfcData) {
			// Read the current setting - Is FFC on or off?
            int isEnabled = 0xDEADBEEF; // initialize with a bogus value;
			rc = PxLIsFfcEnabled(hCamera, &isEnabled);
			if (!API_SUCCESS(rc)) {
				printf("ERROR: Unable to determine if FFC is enabled or not (0x%8.8X)\n", rc);
			} else {
				ASSERT(0xDEADBEEF != isEnabled);
				printf("FFC is %sabled.\n", (isEnabled) ? "en" : "dis");

				// Toggle the setting now
				const int enable = !isEnabled;
				printf("\n%sabling FFC...\n", (enable) ? "En" : "Dis");
				rc = PxLEnableFfc(hCamera, enable);
				printf("PxLEnableFfc(hCamera, %d) returned 0x%8.8X.\n", enable, rc);
				if (!API_SUCCESS(rc)) {
					printf("ERROR: (0x%8.8X)\n", rc);
				} else {
					// And query again just to confirm
					int currentlyIsEnabled = 0xDEADBEEF;
					printf("\n");
					rc = PxLIsFfcEnabled(hCamera, &currentlyIsEnabled);
					if (!API_SUCCESS(rc)) {
						printf("ERROR: Unable to verify that FFC is %sabled (0x%8.8X)\n", (enable) ? "En" : "Dis", rc);
					} else {
						ASSERT(0xDEADBEEF != currentlyIsEnabled);
						printf("FFC is %sabled.\n", (currentlyIsEnabled) ? "en" : "dis");
						if (currentlyIsEnabled == enable) {
							returnValue = EXIT_SUCCESS;
						}
					}
				}
			}
		} else {
			printf("The camera does not have FFC data.\n");
		}
	}

	PxLUninitialize(hCamera);
	return returnValue;
}

//
// Struct describing the buffer used to read and write the ffc settings
// 
struct FfcSettings
{
    U32		command;
    U32		ffcInfo;
    float	ffcCalibratedGain;
};

#define COMMAND_FFC_SETTINGS_READ           0x00008001
#define COMMAND_FFC_SETTINGS_WRITE          0x00008002

#define IS_FFC_ENABLED(ffcInfo)		(((ffcInfo) & 0x0F) == 0) ? 0 : 1
#define GET_FFC_TYPE(ffcInfo)		(((ffcInfo) & 0xF0) >> 4)

#define FFC_TYPE_UNKNOWN			0
#define FFC_TYPE_UNCALIBRATED		1
#define	FFC_TYPE_FACTORY			2
#define	FFC_TYPE_FIELD				3

//
// Helper function to read the FFC information from the camera.
// Note that we will re-try the request in the event that
// the request was interrupted, and as a result the stale results 
// of another query were read back. While this is unlikely, it is
// still possible and therefore must be handled.
//
static PXL_RETURN_CODE 
ReadFfcSettings(HANDLE hCamera, FfcSettings* pFfcSettings)
{
	if (NULL == pFfcSettings) { return ApiNullPointerError; }

	int numTries = 5;
	do {
		// Prepare the command buffer
		memset(pFfcSettings, 0, sizeof(*pFfcSettings));
		pFfcSettings->command = COMMAND_FFC_SETTINGS_READ;

		// Send the command to read the FFC settings
		PXL_RETURN_CODE rc = PxLCameraWrite(hCamera, sizeof(*pFfcSettings), (U8*)pFfcSettings);
		if (!API_SUCCESS(rc)) { return rc; }

		// Read the response
		rc = PxLCameraRead(hCamera, sizeof(*pFfcSettings), (U8*)pFfcSettings);
		if (!API_SUCCESS(rc)) { return rc; }
		
		// Is this the response to OUR request?
		if (COMMAND_FFC_SETTINGS_READ == pFfcSettings->command) {
			return ApiSuccess;
		}

	} while (numTries-- > 0);

	return ApiUnknownError;
}

//
// Does the camera have FFC data at all? 
// If there's no FFC data, it doesn't make sense to
// enable or disable flat-field correction. 
// 
// Most PixeLINK cameras go out with FFC data, but 
// some clients have asked for no FFC data. 
//
PXL_RETURN_CODE 
PxLCameraHasFfcData(HANDLE hCamera, int* pHasFfcData)
{
	if (NULL == pHasFfcData) { return ApiNullPointerError; }

	FfcSettings ffcSettings;
	PXL_RETURN_CODE rc = ReadFfcSettings(hCamera, &ffcSettings);
	if (!API_SUCCESS(rc)) { return rc; }

	int hasFfcData = 0;
	switch(GET_FFC_TYPE(ffcSettings.ffcInfo)) {
		case FFC_TYPE_UNCALIBRATED:
			hasFfcData = 0;
			break;

		case FFC_TYPE_FACTORY:
		case FFC_TYPE_FIELD:
			hasFfcData =  1;
			break;

		case FFC_TYPE_UNKNOWN:
		default:
			ASSERT(0); // Very unusual
			break;
	}

	*pHasFfcData = hasFfcData;
	return rc;
}

//
// Assumes the camera has FFC data
//
PXL_RETURN_CODE 
PxLIsFfcEnabled(HANDLE hCamera, int* pIsFfcEnabled)
{
	if (NULL == pIsFfcEnabled) { return ApiNullPointerError; }

	FfcSettings ffcSettings;
	PXL_RETURN_CODE rc = ReadFfcSettings(hCamera, &ffcSettings);
	if (API_SUCCESS(rc)) {
		*pIsFfcEnabled = IS_FFC_ENABLED(ffcSettings.ffcInfo);
	}
	return rc;
}

//
// Assumes the camera has FFC data
//
PXL_RETURN_CODE
PxLEnableFfc(HANDLE hCamera, int enable)
{
	FfcSettings ffcSettings;
	memset(&ffcSettings, 0, sizeof(ffcSettings));
	ffcSettings.command = COMMAND_FFC_SETTINGS_WRITE;
	ffcSettings.ffcInfo = (enable) ? 1 : 0;

	PXL_RETURN_CODE rc = PxLCameraWrite(hCamera, sizeof(ffcSettings), (U8*)&ffcSettings);
	return rc;
}