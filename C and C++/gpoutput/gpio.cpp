//
// A simple introduction to FEATURE_GPIO.
//
// This shows how to set up a GPIO (which is really just a GPO) for 
// 'normal' mode. In normal mode, setting the GPO polarity determines
// whether the light will be on or not. 
// 
// This code assumes your camera supports GPOs. 
//
//
#include <PixeLINKApi.h>
#include <vector>
#include <stdio.h>
#include <assert.h>

// Some defines that might come in handy
#define ASSERT(x)	do { assert((x)); } while(0)

// Local helper functions
static PXL_RETURN_CODE	InitializeGpoForNormalMode(HANDLE hCamera, U32 gpoNumber);
static PXL_RETURN_CODE 	SetGpoPolarity(HANDLE hCamera, U32 gpoNumber, int polarity);
static void				RunGpioTest(HANDLE hCamera);
static void				SendMorseCodeOverGpio(HANDLE hCamera, U32 gpioIndex, U32 unitLength, const char* pMessage);



int 
main(int argc, char* argv[])
{
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		printf("ERROR: Unable to initialize a camera (0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	RunGpioTest(hCamera);

	PxLUninitialize(hCamera);

	return EXIT_SUCCESS;
}

//
// We initialize the GPIO for 'normal mode' by:
//   1) setting the mode to 'normal'
//   2) enabling the feature (clear the FEATURE_FLAG_OFF flag)
//   3) turning off the GPO (don't have to, but it's a good way to start things)
//
static PXL_RETURN_CODE
InitializeGpoForNormalMode(HANDLE hCamera, U32 gpoIndex)
{
	U32 flags;
	std::vector<float> params(FEATURE_GPIO_NUM_PARAMS, 0.0f);
	U32 numParams = static_cast<U32>(params.size());

	// !VERY IMPORTANT!
	// Before calling PxLGetFeature, we have to tell the API which GPO we're asking about
	params[FEATURE_GPIO_PARAM_GPIO_INDEX] = static_cast<float>(gpoIndex);

	// Now we can read the information for the specified GPO
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_GPIO, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) {
		return rc;
	}
	ASSERT(params[FEATURE_GPIO_PARAM_GPIO_INDEX] == static_cast<float>(gpoIndex));
	params[FEATURE_GPIO_PARAM_MODE]			= GPIO_MODE_NORMAL;
	params[FEATURE_GPIO_PARAM_POLARITY]		= POLARITY_ACTIVE_LOW;	// Initially start off
	// Leave the other parameters alone as they're not used in normal mode

	// Enable the feature by clearing the OFF flag
	flags &= ENABLE_FEATURE(flags, true);

	rc = PxLSetFeature(hCamera, FEATURE_GPIO, flags, numParams, &params[0]);
	return rc;
}

//
// Set just the polarity for a particular GPIO
//
static PXL_RETURN_CODE 
SetGpoPolarity(HANDLE hCamera, U32 gpoIndex, int polarity)
{
	U32 flags(0);
	std::vector<float> params(FEATURE_GPIO_NUM_PARAMS,0.0f);
	U32 numParams = static_cast<U32>(params.size());

	params[FEATURE_GPIO_PARAM_GPIO_INDEX] = static_cast<float>(gpoIndex);
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_GPIO, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) {
		return rc;
	}
	params[FEATURE_GPIO_PARAM_POLARITY] = static_cast<float>(polarity);
	rc = PxLSetFeature(hCamera, FEATURE_GPIO, flags, numParams, &params[0]);
	return rc;

}




//
// Just do a few things with the GPIO to show that we can control it.
//
// NOTE! The GPO index starts at 1, not 0.
//
static void
RunGpioTest(HANDLE hCamera)
{
	// Initialize two GPIOs (assuming 2 gpios are supported)
	PXL_RETURN_CODE rc;
	rc = InitializeGpoForNormalMode(hCamera, 1);	// Note: first GPIO is 1, not 0
	rc = InitializeGpoForNormalMode(hCamera, 2);

	// Check all 4 states of two GPOs
	SetGpoPolarity(hCamera, 1, POLARITY_ACTIVE_LOW);	// 00
	SetGpoPolarity(hCamera, 2, POLARITY_ACTIVE_LOW);
	Sleep(1000);

	SetGpoPolarity(hCamera, 1, POLARITY_ACTIVE_LOW);	// 01
	SetGpoPolarity(hCamera, 2, POLARITY_ACTIVE_HIGH);
	Sleep(1000);

	SetGpoPolarity(hCamera, 1, POLARITY_ACTIVE_HIGH);	// 10
	SetGpoPolarity(hCamera, 2, POLARITY_ACTIVE_LOW);
	Sleep(1000);

	SetGpoPolarity(hCamera, 1, POLARITY_ACTIVE_HIGH);	// 11
	SetGpoPolarity(hCamera, 2, POLARITY_ACTIVE_HIGH);
	Sleep(1000);

	// Turn both off
	SetGpoPolarity(hCamera, 1, POLARITY_ACTIVE_LOW);	// 00
	SetGpoPolarity(hCamera, 2, POLARITY_ACTIVE_LOW);

	SendMorseCodeOverGpio(hCamera, 1, 100, "... --- ...");	// S O S
	Sleep(1000);

	SendMorseCodeOverGpio(hCamera, 2, 50, "- .... .   -.. ..- -.. .   .- -... .. -.. . ... .-.-.- ");
	//                                     T  H   E    D   U   D  E   A   B   I   D  E  S     .


	// Turn both off before exiting.
	SetGpoPolarity(hCamera, 1, POLARITY_ACTIVE_LOW);
	SetGpoPolarity(hCamera, 2, POLARITY_ACTIVE_LOW);

}

//
// Just for fun. 
//
static void
SendMorseCodeOverGpio(HANDLE hCamera, U32 gpioIndex, U32 unitLength, const char* pMessage)
{
	ASSERT(NULL != pMessage);

	for(const char* p = pMessage; *p != '\0'; p++) {
		
		switch(*p) {
			case '.':
				SetGpoPolarity(hCamera, gpioIndex, POLARITY_ACTIVE_HIGH);
				Sleep(unitLength);
				break;
			case '-':
				SetGpoPolarity(hCamera, gpioIndex, POLARITY_ACTIVE_HIGH);
				Sleep(unitLength*3);
				break;
			case ' ':
				Sleep(unitLength*3);

		}
		SetGpoPolarity(hCamera, gpioIndex, POLARITY_ACTIVE_LOW);
		Sleep(unitLength);
	}

	return;
}