// slowMotionVideo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <PixeLINKApi.h>
#include <assert.h>
#include <conio.h>
#include <stdio.h>
#include <vector>
#include "quietStderr.h"

#define A_OK          0  // non-zero error codes
#define GENERAL_ERROR 1

#define DEFAULT_PLAYBACK_FRAME_RATE (25) // in frames/second (25 == smooth(ish) video
#define DEFAULT_RECORD_DURATION     (20) // in seconds
#define DEFAULT_DECIMATION_FACTOR   (1)  // 1 == capture every frame

// This application is intended to work with very high frame rates.  That is
// the camera is outputting image data at a very high rate.  For most systems
// we will need to do more compression to accomodate this high capture data
// rate.  This takes more procssing power to do the compression, but on most 
// systems, it's the disk access that dictates our ability to sink image
// data at high data rates.  So, we reduce the bitrate (and quality) to do
// more compression, so that we are less likely to loose capture data.
#define CLIP_PLAYBACK_BITRATE     (CLIP_PLAYBACK_BITRATE_DEFAULT/3)

// Used as a simple sanity check in this example file
#define ASSERT(x)	do { assert((x)); } while (0)

// Prototypes to aloow top-down structure
void  usage (_TCHAR **argv);
int   getParameters (int argc, _TCHAR* argv[], U32* playTime, U32* bitRate, U32* frameRate, U32* decimation, char* aviFileName);
float effectiveFrameRate (HANDLE hCamera);
static U32 _stdcall CaptureDoneCallback(HANDLE hCamera, U32 numFramesCapture, PXL_RETURN_CODE returnCode);


// 'Globals' ahred between our main line, and the clip callback
static bool captureFinished = false;
static U32  numImagesStreamed = 0;
static PXL_RETURN_CODE captureRc = ApiSuccess;

int _tmain(int argc, _TCHAR* argv[])
{
	CQuietStderr quiet;
    std::vector<char> aviFile(256,0);
    U32  recordTime;
    U32  bitRate;
    U32  frameRate;
    U32  decimation;
    //
	// Step 1
	//      Validate the user parameters, getting poll period and invert value
    if (A_OK != getParameters(argc, argv, &recordTime, &bitRate, &frameRate, &decimation, &aviFile[0]))
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
        printf (" Error:  There should be exactly one PixeLINK camera connected.\n");
        return GENERAL_ERROR;
	}
    rc = PxLInitialize (0, &hCamera);
    if (!API_SUCCESS(rc))
    {
        printf (" Error:  Could not initialize the camera.\n");
        return GENERAL_ERROR;
    }

    // 
    // Step 3
    //      Determine the effective frame rate for the camera, and the number of images we will need to
    //      capture the video of the requested length then start the stream
    float cameraFps = effectiveFrameRate(hCamera);
    if (cameraFps < (float)frameRate)
    {
        // Although this will 'work', such a configuration will create a fast motion video, 
        // and a rather poor quality one at that.  For these instances, you should be using 
        // the demo FastMotionVideo
        printf (" Error:  The camera's frame rate is currently %.2f; it should be > %.2f.\n", cameraFps, ((float)frameRate));
        PxLUninitialize (hCamera);
        return GENERAL_ERROR;
    }
    U32   numImages = (U32)(((float)recordTime) * cameraFps);
    if (!API_SUCCESS (PxLSetStreamState (hCamera, START_STREAM)))
    {
        printf (" Error:  Could not start the stream.\n");
        PxLUninitialize (hCamera);
        return GENERAL_ERROR;
    }

    //
    // Step 4
    //      Capture the required images into the clip
    std::vector<char> h264File(256,0);
    strcpy_s (&h264File[0],256,&aviFile[0]);
    strcat_s (&h264File[0],256,".h264");

    printf (" Recording %d seconds of h264 compressed video (based on %d images).  Press any key to abort...",
            recordTime, numImages);
    captureFinished = false;
    CLIP_ENCODING_INFO clipEncoding = {CLIP_ENCODING_H264, decimation, (float)frameRate, bitRate};
    rc = PxLGetEncodedClip (hCamera, numImages, &h264File[0], &clipEncoding, CaptureDoneCallback);
    if (API_SUCCESS(rc))
    {
        while (!captureFinished)
        {
            if (_kbhit())
            {
                // User wants to abort.  Tell the API to abort the capture by stopping the stream.  This should call our callback with
                // an error.
                PxLSetStreamState (hCamera, STOP_STREAM);
            } else {
                // No need to steal a bunck of cpu cycles on a loop doing nothing -- sleep for but until it's time to check for keyboard
                // input again.
                Sleep (500);
            }
        }
    }
    PxLSetStreamState (hCamera, STOP_STREAM);  //already stopped if user aborted, but that's OK

    //
    // Step 5
    //      Clip capture is done.  If it completed OK, create the clip video file (.avi)
    if (API_SUCCESS (rc))
    {
        if (API_SUCCESS (captureRc))
        {
            if (captureRc == ApiSuccessWithFrameLoss)
            {
                printf ("Warning\n %d images had to be streamed to capture %d of them.\n",numImagesStreamed,numImages);  
            } else {
                printf ("Success\n %d images captured.\n",numImages);
            }

            //
            // Step 6
            //      convert the clip capture file, into a .avi video file
            rc = PxLFormatClipEx (&h264File[0], &aviFile[0], CLIP_ENCODING_H264, CLIP_FORMAT_AVI);
        } else {
            rc = captureRc;
        }
    }

    if (!API_SUCCESS(rc))
    {
        printf ("Error\n PxLGetH264Clip/PxLFormatClipEx returned 0x%x\n", rc);
    }
    
    PxLUninitialize (hCamera);
    return rc;

}

//
// Function that's called when PxLGetClip is finished capturing frames, or can't continue
// capturing frames.
//
static U32 _stdcall 
CaptureDoneCallback(HANDLE hCamera, U32 numFramesCapture, PXL_RETURN_CODE returnCode)
{
    // Just record the capture information into our shared (global) varaibles so the main line
    // can report/take action on the result.
    numImagesStreamed = numFramesCapture;
    captureRc = returnCode;
    captureFinished = true;
	return ApiSuccess;
}

/* 
 * Returns the frame rate being used by the camera.  Ideally, this is simply FEAUTURE_ACTUAL_FRAME_RATE, but
 * some older cameras do not support that.  If that is the case, use FEATURE_FRAME_RATE, which is 
 * always supported.
 */
float effectiveFrameRate (HANDLE hCamera)
{
    float frameRate = DEFAULT_PLAYBACK_FRAME_RATE;
    PXL_RETURN_CODE rc;
    
	//
    // Step 1
    //      Determine if the camera supports FEATURE_ACTUAL_FRAME_RATE

    // How big a buffer will we need to hold the information about the trigger feature?
	U32 bufferSize = -1;
    U32 frameRateFeature = FEATURE_FRAME_RATE;
	if (API_SUCCESS (PxLGetCameraFeatures(hCamera, FEATURE_ACTUAL_FRAME_RATE, NULL, &bufferSize)))
    {
	    ASSERT(bufferSize > 0);

	    // Declare a buffer and read the feature information
	    std::vector<U8> buffer(bufferSize, 0);  // zero-initialized buffer
	    CAMERA_FEATURES* pCameraFeatures = (CAMERA_FEATURES*)&buffer[0];
	    if (API_SUCCESS (PxLGetCameraFeatures(hCamera, FEATURE_ACTUAL_FRAME_RATE, pCameraFeatures, &bufferSize)))
        {
            //
            //  Step 2
            //      Get the 'best available' frame rate of the camera
            U32 frameRateFeature;
            if (pCameraFeatures[0].pFeatures->uFlags & FEATURE_FLAG_PRESENCE)
            {
                frameRateFeature = FEATURE_ACTUAL_FRAME_RATE;
            }
        }
    }
    
    U32 flags;
    U32 numParams = 1;
    rc = PxLGetFeature (hCamera, frameRateFeature, &flags, &numParams, &frameRate);
	ASSERT(API_SUCCESS(rc));

    return frameRate;
}

void usage (_TCHAR **argv)
{
        _tprintf(_T("\n This application will capture video for the specified number of seconds.  If the camera's frame\n"));
        _tprintf(_T(" rate is playback_framrate, then the generated (a .avi file) will play back at 'normal speed'.\n"));
        _tprintf(_T(" However, this application allows you to create 'slow motion' videos by setting the camera up so\n"));
        _tprintf(_T(" that it uses frame rates that are higher than playback_framrate.  In so doing, you can create\n"));
        _tprintf(_T(" very 'dramatic' slow motion videos, especially when using a very high camera frame rates, \n"));
        _tprintf(_T(" several hundred fps or more.  Note however, that very high frame rates are only possible with\n"));
        _tprintf(_T(" very short exposure times, which (generally) require a lot of light. In addition to adjusting\n"));
        _tprintf(_T(" your lighting accordingly, consider using the following strategies to faclitate very high frame\n"));
        _tprintf(_T(" rates:\n"));
        _tprintf(_T("    - Adding Pixel Addressing.  Binning in particular will accomodate faster frame rates\n"));
        _tprintf(_T("      and help 'brighten' dark images\n"));
        _tprintf(_T("    - Adding Gain to brighten the images\n"));
        _tprintf(_T("    - Reducing the ROI to accomodate faster frame rates\n\n"));
        _tprintf(_T("    Usage: %s [-t capture_duration] [-b playback_bitrate] [-f playback_framerate] video_name \n"), argv[0]);
        _tprintf(_T("       where: \n"));
        _tprintf(_T("          -t capture_duration   How much time to spend captureing video (in seconds) \n"));
        _tprintf(_T("          -b playback_bitrate   Bitrate (b/s) that will be used for playback.  This value\n"));
        _tprintf(_T("                                provides guidance on how much compresion to use.  More\n"));
        _tprintf(_T("                                compression means lower quality.  Generally, if the video\n"));
        _tprintf(_T("                                capture cannot keep pace with the cameras stream (as indicated\n"));
        _tprintf(_T("                                by this application issueing a warning), then you will want to\n"));
        _tprintf(_T("                                to lower this value.  Its default value is %d \n"), CLIP_PLAYBACK_BITRATE);
        _tprintf(_T("          -f playback_framerate Framerate (f/s) that will be used for playback.  This value\n"));
        _tprintf(_T("                                determines the duration of the clip. If this value matches the\n"));
        _tprintf(_T("                                camera's framerate, then the playback duration will match the\n"));
        _tprintf(_T("                                capture_duration.  Its default value is %d \n"), DEFAULT_PLAYBACK_FRAME_RATE);
        _tprintf(_T("          -d decimation_factor  Capture every decimation_factor'th frame.  DEfaults to 1\n"));
        _tprintf(_T("                                (capture every frame) \n"));
        _tprintf(_T("          video_name            Name of file to generate (it will be postfixed with .avi) \n"));
        _tprintf(_T("    Example: \n"));
        _tprintf(_T("        %s -t 30 clip \n"), argv[0]);
        _tprintf(_T("              This will capture a video, called clip.avi, that records for about 30 seconds. \n"));
}

int getParameters (int argc, _TCHAR* argv[], U32* recordTime, U32* bitRate, U32* frameRate, U32* decimation, char* fileName)
{
    
    // Default our local copies to the user supplied values
    U32  uRecordTime = DEFAULT_RECORD_DURATION;
    U32  uBitRate = CLIP_PLAYBACK_BITRATE;
    U32  uFrameRate = DEFAULT_PLAYBACK_FRAME_RATE;
    U32  uDecimation = DEFAULT_DECIMATION_FACTOR;
    char*  nameRoot;  
   
    // 
    // Step 1
    //      Simple parameter parameter check
    if (argc < 2 ||  // No play time specified; use the default
        argc > 10)    // User specifies all options
    {
        printf ("\n ERROR -- Incorrect number of parameters\n");
        return GENERAL_ERROR;
    }

    //
    // Step 2
    //      Parse the command line looking for the optional parameters.
    int parm;
    for (int i=1; i<argc-1; i++)
    {
        if (!_tcscmp(argv[i],_T("-t")) ||
            !_tcscmp(argv[i],_T("-T")))
        {
            if (i+1 >= argc) return GENERAL_ERROR;
            parm = _ttoi(argv[i+1]);
            if (parm < 1) return  GENERAL_ERROR;
            uRecordTime = (U32) parm;
            i++;
        } else if (!_tcscmp(argv[i],_T("-b")) ||
                   !_tcscmp(argv[i],_T("-B"))) {
            if (i+1 >= argc) return GENERAL_ERROR;
            parm = _ttoi(argv[i+1]);
            if (parm < 1000) return  GENERAL_ERROR;
            uBitRate = (U32) parm;
            i++;
        } else if (!_tcscmp(argv[i],_T("-f")) ||
                   !_tcscmp(argv[i],_T("-F"))) {
            if (i+1 >= argc) return GENERAL_ERROR;
            parm = _ttoi(argv[i+1]);
            if (parm < 1) return  GENERAL_ERROR;
            uFrameRate = (U32) parm;
            i++;
        } else if (!_tcscmp(argv[i],_T("-d")) ||
            !_tcscmp(argv[i],_T("-D"))) {
            if (i+1 >= argc) return GENERAL_ERROR;
            parm = _ttoi(argv[i+1]);
            if (parm < 1) return  GENERAL_ERROR;
            uDecimation = (U32) parm;
            i++;
        } else {
            return GENERAL_ERROR;
        }
    }

    //
    // Step 3
    //      The last parameter must be the file name
    if (argv[argc-1][0] == '-') return GENERAL_ERROR;
    nameRoot = argv[argc-1];

    //
    // Step 4
    //      Let the app know the user parameters.  
    *recordTime = uRecordTime;
    *bitRate = uBitRate;
    *frameRate = uFrameRate;
    *decimation = uDecimation;
    strcpy_s (fileName,256,nameRoot);
    strcat_s (fileName,256,".avi");
    return A_OK;
}


