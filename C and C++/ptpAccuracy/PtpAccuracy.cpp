// PtpAccuracy.cpp 
// 
// Demostrates how to capture synchronised frames on 2 cameras using:
//    - 2 PL-X cameras of the same model, and using the same settings 
//    - PTP (Percision Time Protocol V2)
//    - scheduled ACTION_SENSOR_SYNC actions
//    - scheduled ACTION_FRAME_TRIGGER actions
// For simplicity, the program does not check to see if the cameras do
// indeed support each of the above -- it is assumed.
//
//

#include "stdafx.h"
#include <PixeLINKApi.h>
#include <conio.h>
#include <stdio.h>
#include <vector>
#include <cmath>

#define A_OK          0  // non-zero error codes
#define GENERAL_ERROR 1

HANDLE hCameraA = NULL;
HANDLE hCameraB = NULL;

double cameraAFrameTime;
double cameraBFrameTime;

bool   newCameraAFrame = false;
bool   newCameraBFrame = false;

//char* LogFile = "PtpDelta.csv";  // Set this to NULL if you don't want to log the results to a file
char* LogFile = NULL;  // Set this to NULL if you don't want to log the results to a file

static U32 _stdcall
FrameCallbackFunction(const HANDLE hCamera, void* const pFrameData, const U32 dataFormat, FRAME_DESC const * const pFrameDesc, void* const context)
{
    if (hCamera == hCameraA)
    {
        cameraAFrameTime = pFrameDesc->dFrameTime;
        newCameraAFrame = true;
    } else if (hCamera == hCameraB) {
        cameraBFrameTime = pFrameDesc->dFrameTime;
        newCameraBFrame = true;
    }
    return ApiSuccess;
}


int _tmain(int argc, _TCHAR* argv[])
{
	int             rc = A_OK;

    //
    // Step 0
    //      Find 2 cameras.  We will assume that there are at least 2 cameras connected/available -- return
    //      an error if this is not the case
	PXL_RETURN_CODE pxlRc = A_OK;
    U32 numCameras = 0;

    pxlRc = PxLGetNumberCamerasEx (NULL, &numCameras);
    if (!API_SUCCESS(pxlRc) || numCameras < 2)
    {
        printf ("   Error: Did not find 2 cameras\n");
        return GENERAL_ERROR;
    }
    std::vector<CAMERA_ID_INFO> pCameraIds(numCameras);
    pCameraIds[0].StructSize = sizeof(pCameraIds[0]);
    pxlRc = PxLGetNumberCamerasEx (&pCameraIds[0], &numCameras);
    if (!API_SUCCESS(pxlRc) || numCameras < 2)
    {
        printf ("   Error: Did not find 2 cameras\n");
        rc = GENERAL_ERROR;
        goto AllDone;
    }
    pxlRc = PxLInitialize (pCameraIds[0].CameraSerialNum, &hCameraA);
    if (API_SUCCESS(pxlRc))
    {
        pxlRc = PxLInitialize (pCameraIds[1].CameraSerialNum, &hCameraB); 
    }
    if (! API_SUCCESS(pxlRc) || hCameraA == NULL || hCameraB == NULL)
    {
        printf ("   Error: Did not find/grab 2 cameras\n");
        rc = GENERAL_ERROR;
        goto AllDone;
    }

    //
    // Step  1
    //      Make sure that both cameras support PTP
    U32   numParams = 5;
    float pParams[5] = {0,0,0,0,0};
    U32   flags;
    int   cameraAPtpState;
    pxlRc = PxLGetFeature (hCameraA, FEATURE_PTP, &flags, &numParams, pParams);
    if (API_SUCCESS(pxlRc))
    {
        cameraAPtpState = (int)pParams[FEATURE_PTP_PARAM_STATUS];
        pxlRc = PxLGetFeature (hCameraB, FEATURE_PTP, &flags, &numParams, pParams);
    }
    if (!API_SUCCESS(pxlRc))
    {
        printf ("   Error: Both cameras do not support PTP\n");
        rc = GENERAL_ERROR;
        goto AllDone;
    }
    int cameraBPtpState = (int)pParams[FEATURE_PTP_PARAM_STATUS];
    printf ("   Camera %d: %s \n", 
        pCameraIds[0].CameraSerialNum, 
        cameraAPtpState == FEATURE_PTP_STATUS_MASTER ? "Master" : cameraAPtpState == FEATURE_PTP_STATUS_SLAVE ? "Slave" : "Unknown");
    printf ("   Camera %d: %s \n", 
        pCameraIds[1].CameraSerialNum, 
        cameraBPtpState == FEATURE_PTP_STATUS_MASTER ? "Master" : cameraBPtpState == FEATURE_PTP_STATUS_SLAVE ? "Slave" : "Unknown");

    //
    // Step 3
    //      Enable Frame Action trigger on both camera
    float pTriggerParams[5] = {0,TRIGGER_TYPE_ACTION,0,0,0};
    //float pTriggerParams[5] = {0,TRIGGER_TYPE_HARDWARE,0,0,0};
    pxlRc = PxLSetFeature (hCameraA, FEATURE_TRIGGER, FEATURE_FLAG_MANUAL, 5, pTriggerParams);
    if (API_SUCCESS(pxlRc))
    {
        pxlRc = PxLSetFeature (hCameraB, FEATURE_TRIGGER, FEATURE_FLAG_MANUAL, 5, pTriggerParams);
    }
    if (!API_SUCCESS(pxlRc))
    {
        printf ("   Error: Both cameras do not support PTP\n");
        rc = GENERAL_ERROR;
        goto AllDone;
    }

    //
    // Step 4
    //      We need to make sure that both camera's have the necessry bandwidth to get a frame from both cameras when
    //      they are triggered simultanesouly.  To ensure this, restrict the bandwidth to 30% each
    pParams[0] = 3000.0; // 3 gb/s
    pxlRc = PxLSetFeature (hCameraA, FEATURE_BANDWIDTH_LIMIT, FEATURE_FLAG_MANUAL, 1, pParams);
    if (API_SUCCESS(pxlRc))
    {
        pxlRc = PxLSetFeature (hCameraB, FEATURE_BANDWIDTH_LIMIT, FEATURE_FLAG_MANUAL, 5, pParams);
    }
    if (!API_SUCCESS(pxlRc))
    {
        printf ("   Error: Could not set the bandwidth limit\n");
        rc = GENERAL_ERROR;
        goto AllDone;
    }


    //
    // Step 5
    //      Enable callback on both cameras, and start the stream
    pxlRc = PxLSetCallback(hCameraA, CALLBACK_FRAME, NULL, FrameCallbackFunction);
    if (API_SUCCESS(pxlRc))
    {
        pxlRc = PxLSetCallback(hCameraB, CALLBACK_FRAME, NULL, FrameCallbackFunction);
    }
    if (!API_SUCCESS(rc)) {
        printf("   Error: Could not set the frame callback\n");
        rc = GENERAL_ERROR;
        goto AllDone;
    }
    pxlRc = PxLSetStreamState (hCameraA, START_STREAM);
    if (API_SUCCESS(pxlRc))
    {
        pxlRc = PxLSetStreamState (hCameraB, START_STREAM);
    }
    if (!API_SUCCESS(rc)) {
        printf("   Error: Could not start the stream\n");
        rc = GENERAL_ERROR;
        goto AllDone;
    }

    //
    // Step 6
    //      Log the results to a csv file, if the user wants
    FILE* pFile = NULL;
    char  printBuffer[20];
    if (LogFile != NULL)
    {
        fopen_s (&pFile, LogFile, "wb");
        if (NULL == pFile)
        {
            rc = GENERAL_ERROR;
            goto AllDone;
        }
    }

    // 
    // Step 7
    //      Periodically, get both cameras to frame action trigger a frame at the same time, and wait until
    //      we receive a frame from both cameras;
    double currentCameraTime = 0.0;  // In seconds.

    double sample;  // The diffence in the image timestamps between the 2 cameras -- in uSeconds.
    double numSamples = 0.0;
    double sampleSum = 0.0;
    double sampleMean = 0.0;
    double sampleMax = 0.0;

    printf ("   Press any key to exit...\n");
    printf ("       Iterations   Current(uS)       Mean(uS)       Max(uS)  \n");
    printf ("       ----------   -----------       --------       -------  \n");
    while (! _kbhit())
    {
        newCameraAFrame = newCameraBFrame = false;
        // Get the camera's current time.  It does not matter which camera, as they are synchronized
        PxLGetCurrentTimestamp (hCameraA, &currentCameraTime);
        // Send the SensorSync action (to all cameras).  Doing this operations shortly before we do the ACTION_FRAME_TRIGGER assures that the
        // sensor clocks are as closely aligned as possible.  This will ensure the best possible synchronization between the frames.  If we
        // don't perform this operation, then the sensor clocks on teh 2 cameras, may drift apart over time
        PxLSetActions (ACTION_SENSOR_SYNC,currentCameraTime + 0.5); // delay the frame action by 500 ms, just to be sure both camera see it
        // Send the frame action (to all cameras)
        PxLSetActions (ACTION_FRAME_TRIGGER,currentCameraTime + 0.5 + 0.000020); // Delay a little bit (20 uS) more, just to be sure the SensorSync is done first
        while (!(newCameraAFrame && newCameraBFrame) && ! _kbhit()){};  // wait until we get a frame from both camera;
        sample = (cameraAFrameTime - cameraBFrameTime)*1000000.0;  // Be sure to express it in useconds
        // Sanity check
        if (abs(sample) > 500.0)
        {
            // More that 500 ms??   A frame must have been dropped/skipped, report this without recording the data
            printf ("\n   Warning -- saw a delta of %6.2f ms, must have dropped a frame               \n", sample/1000.0);
            continue;
        }
        numSamples += 1.0;
        if (abs(sample) > abs(sampleMax)) sampleMax = sample;
        sampleSum += abs(sample);
        sampleMean = sampleSum / numSamples;
        printf ("\r        %5d       %8.5f        %8.5f       %8.5f", 
               (int)numSamples, sample, sampleMean, sampleMax);
        //printf ("%8.5f\n", sample);
        // Print the result to a file (if requested)
        if (pFile)
        {
            sprintf_s (printBuffer, "%8.5f ,", sample);
            fwrite (printBuffer, sizeof(char), strlen(printBuffer), pFile);
        }
        Sleep (500); // Wait 500 ms before trying again

    }
    printf ("\n");
    printf ("Final: numSamples:%d, mean:%.5f, max:%.5f",(int)numSamples, sampleMean, sampleMax);

    // 
    // Step 8
    //      Stop the stream of both cameas and do some cleanup
AllDone:
    if (hCameraA)
    {
        PxLSetStreamState(hCameraA, STOP_STREAM);
        PxLSetCallback(hCameraA, CALLBACK_FRAME, NULL, NULL);
        PxLUninitialize (hCameraA);
    }
    if (hCameraB)
    {
        PxLSetStreamState(hCameraB, STOP_STREAM);
        PxLSetCallback(hCameraB, CALLBACK_FRAME, NULL, NULL);
        PxLUninitialize (hCameraB);
    }
    if (pFile)
    {
        fclose (pFile);
    }

    return rc;
}

