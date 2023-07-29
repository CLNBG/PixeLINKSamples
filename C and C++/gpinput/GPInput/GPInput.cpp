// GPInput.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <PixeLINKApi.h>
#include <conio.h>
#include <stdio.h>
#include <vector>

#define A_OK          0  // non-zero error codes
#define GENERAL_ERROR 1

// Prototypes to aloow top-down structure
void usage (_TCHAR **argv);
int  getParameters (int argc, _TCHAR* argv[], U32* pollPeriod, bool* invert);

int _tmain(int argc, _TCHAR* argv[])
{
    U32  uPollPeriod = 50;  // Units are in milliseconds.  Default to poll 20 times/second
	bool bInvert = false;   // Indicates if the GP Input polarity shold be reversed
	
	//
	// Step 1
	//      Validate the user parameters, getting poll period and invert value
    if (A_OK != getParameters(argc, argv, &uPollPeriod, &bInvert))
    {
        usage(argv);
        return GENERAL_ERROR;
    }

	//
	// Step 2
	//		Grab our camera
	HANDLE			hCamera;
	PXL_RETURN_CODE rc = A_OK;
	U32				uNumberOfCameras = 0;

    rc = PxLGetNumberCameras (NULL, &uNumberOfCameras);
	if (!API_SUCCESS(rc) || uNumberOfCameras != 1)
	{
        printf ("Error:  There should be exactly one PixeLINK camera connected.\n");
        return GENERAL_ERROR;
	}
    rc = PxLInitialize (0, &hCamera);
    if (!API_SUCCESS(rc))
    {
        printf ("Error:  Could not initialize the camera.\n");
        return GENERAL_ERROR;
    }

    //
    // Step 3
    //      See how many GPIOs this camera has
    U32 bufferSize = 0;
    rc = PxLGetCameraFeatures (hCamera, FEATURE_GPIO, NULL, &bufferSize);
    std::vector<U8> buffer(bufferSize, 0);  // zero-initialized buffer
    CAMERA_FEATURES* pCameraFeatures = NULL;
    if (API_SUCCESS(rc) && bufferSize > 0)
    {
       pCameraFeatures = (CAMERA_FEATURES*)&buffer[0];
       rc = PxLGetCameraFeatures(hCamera, FEATURE_GPIO, pCameraFeatures, &bufferSize);
    }
    if (!API_SUCCESS(rc))
    {
       printf("Error:  Could not get GPIO information.\n");
       return GENERAL_ERROR;
    }
    // Which GPIO is the GPI?  Well, on cameras that have 3 GPIOs, it is the 3rd GPIO.  On all others, it is the 1st one. 
    float gpiNum = pCameraFeatures->pFeatures->pParams->fMaxValue == 3.0 ? 3.0 : 1.0;

    // 
    // Step 4 (Optional)
    //      If requested, invert the polarity of the input signal.  This is actualy done
    //      using the hardware trigger polarity
    U32  uFlags;
    F32  fParams[6]; // Make this large enough for trigger or GPO feature
    U32  uNumParams = 5;  // Trigger uses 5 parameters
    if (bInvert)
    {
        fParams[FEATURE_TRIGGER_PARAM_MODE] = 0.0;
        fParams[FEATURE_TRIGGER_PARAM_TYPE] = (F32)TRIGGER_TYPE_HARDWARE;
        fParams[FEATURE_TRIGGER_PARAM_POLARITY] = 1.0; // <-- setting this to 1 will cause the signal inversion
        fParams[FEATURE_TRIGGER_PARAM_DELAY] = 0.0;
        rc = PxLSetFeature (hCamera, FEATURE_TRIGGER, FEATURE_FLAG_MANUAL, uNumParams, fParams);
        if (!API_SUCCESS(rc)) 
        {
            printf ("Error:  Could not invert the input signal\n");
            goto cleanupAndExit;    
        }     
    }

    //
    // Step 5
    //      Setup the GPIO as an input signal
    uNumParams = 6;  // GPO uses 6 parameters
    fParams[FEATURE_GPIO_PARAM_GPIO_INDEX] = gpiNum;  // See Step 3 to see how we find the GPI
    fParams[FEATURE_GPIO_PARAM_MODE] = (F32)GPIO_MODE_INPUT; // Use input mode
    fParams[FEATURE_GPIO_PARAM_POLARITY] = 0.0; // This must be 0 or 1
    // Be sure to enable the GPO
    rc = PxLSetFeature (hCamera, FEATURE_GPIO, FEATURE_FLAG_MANUAL, uNumParams, fParams);
    if (!API_SUCCESS(rc)) 
    {
        printf ("Error:  Could not enable the General Purpose Input\n");
        goto cleanupAndExit;    
    }

    //
    // Step 5
    //      Continously loop, reporting on the GP Input status
    bool bLastGpiValue = false;

   	LARGE_INTEGER liFreq;
	LARGE_INTEGER liStartTime, liCurrentTime;	
	QueryPerformanceFrequency(&liFreq);
    QueryPerformanceCounter(&liStartTime);

    printf ("Press any key to exit\n");
    while (true)
    {
        if (_kbhit()) break;

        //
        // Step 6.1
        //      Read and report the GP input value
        rc = PxLGetFeature (hCamera, FEATURE_GPIO, &uFlags, &uNumParams, fParams);
        if (!API_SUCCESS(rc))
        {
            printf ("\nError:  Could not read the GPI.\n"); 
            break;
        }
        QueryPerformanceCounter(&liCurrentTime);
        // It is the FEATURE_GPIO_MODE_INPUT_PARAM_STATUS parameter that tells us the input signal value.
        printf ("  %8.2f GPI:%d\r",
            ((float)(liCurrentTime.QuadPart - liStartTime.QuadPart)/(float)liFreq.QuadPart), 
            (bool)fParams[FEATURE_GPIO_MODE_INPUT_PARAM_STATUS]);
        if ((bool)fParams[FEATURE_GPIO_MODE_INPUT_PARAM_STATUS] != bLastGpiValue)
        {
            bLastGpiValue = !bLastGpiValue;
            printf ("\n");
        }

        //
        // Step 6.2
        //      Wait the desired amount
        Sleep (uPollPeriod);
    }

cleanupAndExit:
    PxLUninitialize (hCamera);

	return rc;
}

void usage (_TCHAR **argv)
{
        _tprintf(_T("\nTests/Demonstrates a the GPI functionalty on those cameras that support a general\n"));
        _tprintf(_T("purpose input signal.  On some cameras (PL-D and older), the first GPIO can operate as\n"));
        _tprintf(_T("a hardware trigger OR a GPI.  Newer cameras (PL-X and newer) have the 3rd GPIO line\n"));
        _tprintf(_T("dedicated as a GPI.\n\n"));
        _tprintf(_T("    Usage: %s [-i] [-t poll_period] \n"), argv[0]);
        _tprintf(_T("       where: \n"));
        _tprintf(_T("          -i               Indicates that the input signal input should be inverted (by the camera) \n"));
        _tprintf(_T("          -t poll_period   Wait poll_period milliseconds between each read of the input signal \n"));
        _tprintf(_T("    Example: \n"));
        _tprintf(_T("        %s -t 100 \n"), argv[0]);
        _tprintf(_T("              Ths will poll the camera aproximatly 10 times a second, reporting on the General \n"));
        _tprintf(_T("              Purpose Input signal).\n"));
}

int getParameters (int argc, _TCHAR* argv[], U32* pollPeriod, bool* invert)
{
    
    // Default our local copies to the user supplied values
    U32  uPollPeriod = *pollPeriod;
    bool bInvert = *invert;
   
    // 
    // Step 1
    //      Simple parameter parameter check
    if (argc < 1 || // No parameters specified -- OK
        argc > 4)
    {
        printf ("\nERROR -- Incorrect number of parameters");
        return GENERAL_ERROR;
    }
    
    //
    // Step 2
    //      Parse the command line looking for parameters.
    for (int i = 1; i < argc; i++)
    {
        if (!_tcscmp(argv[i],_T("-i")) ||
            !_tcscmp(argv[i],_T("-I")))
        {
            bInvert = true;
        } else if (!_tcscmp(argv[i],_T("-t")) ||
                   !_tcscmp(argv[i],_T("-T"))) {
            if (i+1 >= argc) return GENERAL_ERROR;
            uPollPeriod = _ttoi(argv[i+1]);
            i++;
        } else {
            return GENERAL_ERROR;
        }
    }
    
    //
    // Step 3
    //      Let the app know the user parameters.
    *pollPeriod = uPollPeriod;
    *invert = bInvert;
    return A_OK;
}



