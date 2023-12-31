// fastMotionVideo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <PixeLINKApi.h>
#include <assert.h>
#include <conio.h>
#include <stdio.h>
#include <vector>
#include "quietStderr.h"
#include "getsnapshot.h"

#define A_OK          0  // non-zero error codes
#define GENERAL_ERROR 1

#define DEFAULT_PLAYBACK_FRAME_RATE (25)     // in frames/second.  25 == smooth(ish) video
#define DEFAULT_RECORD_DURATION     (5 * 60) // in seconds

#define DEFAULT_CLIP_DECIMATION   5           // Every X'th frame will be included in the clip

#define DEFAULT_IMAGE_CAPTURE_PERIOD  10      // in seconds

// Used as a simple sanity check in this example file
#define ASSERT(x)	do { assert((x)); } while (0)

// Prototypes to aloow top-down structure
void  usage (_TCHAR **argv);
int   getParameters (int argc, _TCHAR* argv[], U32* recordTime, U32* decimation, U32* imagePeriod, U32* frameRte, char** fileNames);
float effectiveFrameRate (HANDLE hCamera);
static U32 _stdcall CaptureDoneCallback(HANDLE hCamera, U32 numFramesCapture, PXL_RETURN_CODE returnCode);


// 'Globals' shred between our main line, and the clip callback
static bool captureFinished = false;
static U32  numImagesStreamed = 0;
static PXL_RETURN_CODE captureRc = ApiSuccess;

int _tmain(int argc, _TCHAR* argv[])
{
	CQuietStderr quiet;
    U32  recordTime;
    U32  decimation;
    U32  imagePeriod;
    U32  frameRate;
    char* rootName;
    std::vector<char> aviFile(256,0);
    std::vector<char> bmpFile(256,0);

    //
	// Step 1
	//      Validate the user parameters, getting user specified (or default) values
    if (A_OK != getParameters(argc, argv, &recordTime, &decimation, &imagePeriod, &frameRate, &rootName))
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
    U32   numImages = (U32)(((float)recordTime) * cameraFps);
    numImages = numImages/decimation + 1; // Include the decimation factor
    if (!API_SUCCESS (PxLSetStreamState (hCamera, START_STREAM)))
    {
        printf (" Error:  Could not start the stream.\n");
        PxLUninitialize (hCamera);
        return GENERAL_ERROR;
    }

    //
    // Step 4
    //      Figure out time base, and when we should be capturing our first image
    int capImageNum = 0;
    LARGE_INTEGER  freq;
    LARGE_INTEGER  currentTime;
    LARGE_INTEGER  nextImageTime;

    QueryPerformanceFrequency (&freq);
    QueryPerformanceCounter (&currentTime);
    nextImageTime = currentTime;


    //
    // Step 5
    //      Start captureing the required images into the clip
	std::vector<char> h264File(256,0);
    strcpy_s (&h264File[0],256,rootName);
    strcat_s (&h264File[0],256,".h264");

    printf (" Recording %d seconds of h264 compressed video (based on %d images) + %d still images.  Press any key to abort...",
            recordTime, numImages, (recordTime/imagePeriod));
    captureFinished = false;

    CLIP_ENCODING_INFO clipEncoding = {CLIP_ENCODING_H264, decimation, (float)frameRate, CLIP_PLAYBACK_BITRATE_DEFAULT};
    rc = PxLGetEncodedClip (hCamera, numImages, &h264File[0], &clipEncoding, CaptureDoneCallback);
    if (API_SUCCESS(rc))
    {
        //
        // Step 6
        //      While we are waiting for the clip to finsh (or for the user to quit), collect still images at the requested frequency

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
                QueryPerformanceCounter(&currentTime);
                if (currentTime.QuadPart >= nextImageTime.QuadPart)
                {
                    // It's time to capture an image
                    sprintf_s (&bmpFile[0], 256, "%s%d.bmp", rootName, capImageNum);  
                    getSnapshot (hCamera, IMAGE_FORMAT_BMP, &bmpFile[0]);
                    nextImageTime.QuadPart = currentTime.QuadPart + (imagePeriod * freq.QuadPart);
                    capImageNum++;
                } else {
                    Sleep (200); // There was nothing to do -- don't waste too much time
                }
            }
        }
    }
    PxLSetStreamState (hCamera, STOP_STREAM);  //already stopped if user aborted, but that's OK

    //
    // Step 6
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
            strcpy_s (&aviFile[0],256,rootName);
            strcat_s (&aviFile[0],256,".avi");
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
        _tprintf(_T("\n This application will do two things:\n"));
        _tprintf(_T("    - It will capture a 'fast motion' video clip\n"));
        _tprintf(_T("    - It will capture a series of images\n"));
        _tprintf(_T(" More specificaly, over the capture period it will create a video clip using every N'th\n"));
        _tprintf(_T(" frame from the stream (thus creating the fast motion effect).  Additionaly, it will \n"));
        _tprintf(_T(" also capture an image every X seconds over the same period.\n\n"));
        _tprintf(_T("    Usage: %s [-t capture_duration] [-d decimation] [-i image_period] [-f playback_framerate] capture_names \n"), argv[0]);
        _tprintf(_T("       where: \n"));
        _tprintf(_T("          -t capture_duration   How much time to spend captureing video (in seconds). \n"));
        _tprintf(_T("                                If not specified, %d seconds of video will be captured.\n"), DEFAULT_RECORD_DURATION);
        _tprintf(_T("          -d decimation         Only include every N'th image from the camera camera\n"));
        _tprintf(_T("                                stream, in the video.  The larger this number, the faster\n"));
        _tprintf(_T("                                the video appears.  Its default value is %d\n"), DEFAULT_CLIP_DECIMATION);
        _tprintf(_T("          -i image_period       A still Image will be captured with the specified period\n"));
        _tprintf(_T("                                The default is %d\n"), DEFAULT_IMAGE_CAPTURE_PERIOD);
        _tprintf(_T("          -f playback_framerate Framerate (f/s) that will be used for playback.  This value\n"));
        _tprintf(_T("                                determines the duration of the clip. If this value matches the\n"));
        _tprintf(_T("                                camera's framerate, then the playback duration will match the\n"));
        _tprintf(_T("                                capture_duration.  Its default value is %d \n"), DEFAULT_PLAYBACK_FRAME_RATE);
        _tprintf(_T("          capture_names         Names used for the captured files.  The video will have a '.avi'\n"));
        _tprintf(_T("                                extension, while the images will be of a '.bmp' type\n"));
        _tprintf(_T("    Example: \n"));
        _tprintf(_T("        %s -t 30 cap \n"), argv[0]);
        _tprintf(_T("              Over a 30 second period, a video will be captured using every 5th image, so that\n"));
        _tprintf(_T("              resulting image will be %d seconds long.  Additionally, it will create %d still \n"), 30/DEFAULT_CLIP_DECIMATION,  30/DEFAULT_IMAGE_CAPTURE_PERIOD);
        _tprintf(_T("              images (one captured every %d seconds) \n"), DEFAULT_IMAGE_CAPTURE_PERIOD);
}

int getParameters (int argc, _TCHAR* argv[], U32* recordTime, U32* decimation, U32* imagePeriod, U32* frameRate, char** fileNames)
{
    
    // Default our local copies to the user supplied values
    U32  uRecordTime = DEFAULT_RECORD_DURATION;
    U32  uDecimation = DEFAULT_CLIP_DECIMATION;
    U32  uImagePeriod = DEFAULT_IMAGE_CAPTURE_PERIOD;
    U32  uFrameRate = DEFAULT_PLAYBACK_FRAME_RATE;
    char*  sFileNames;  
   
    // 
    // Step 1
    //      Simple parameter parameter check
    if (argc < 2 ||  // Must have at least the fileManes
        argc > 10)    // Only 4 options allowed
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
        } else if (!_tcscmp(argv[i],_T("-d")) ||
                   !_tcscmp(argv[i],_T("-D"))) {
            if (i+1 >= argc) return GENERAL_ERROR;
            parm = _ttoi(argv[i+1]);
            if (parm < 1) return  GENERAL_ERROR;
            uDecimation = (U32) parm;
            i++;
        } else if (!_tcscmp(argv[i],_T("-i")) ||
                   !_tcscmp(argv[i],_T("-I"))) {
            if (i+1 >= argc) return GENERAL_ERROR;
            parm = _ttoi(argv[i+1]);
            if (parm < 1) return  GENERAL_ERROR;
            uImagePeriod = parm;
            i++;
        } else if (!_tcscmp(argv[i],_T("-f")) ||
                   !_tcscmp(argv[i],_T("-F"))) {
            if (i+1 >= argc) return GENERAL_ERROR;
            parm = _ttoi(argv[i+1]);
            if (parm < 1) return  GENERAL_ERROR;
            uFrameRate = parm;
            i++;
        } else {
            return GENERAL_ERROR;
        }
    }

    //
    // Step 3
    //      The last parameter must be the file name
    if (argv[argc-1][0] == '-') return GENERAL_ERROR;
    sFileNames = argv[argc-1];

    //
    // Step 4.
    //      Additional sanity checks
    if (uRecordTime < uImagePeriod) return GENERAL_ERROR;

    //
    // Step 5
    //      Let the app know the user parameters.  
    *recordTime = uRecordTime;
    *decimation = uDecimation;
    *imagePeriod = uImagePeriod;
    *frameRate = uFrameRate;
    *fileNames = sFileNames;

    return A_OK;
}


