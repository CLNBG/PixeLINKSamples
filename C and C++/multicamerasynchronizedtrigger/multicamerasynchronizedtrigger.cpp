//
// multicamerasynchronizedtrigger.cpp 
//
// Assumes that there are N cameras connected to the host, and that one of the cameras (the 'master' camera) is used to 
// A) capture an image, 
// B) trigger all the other cameras ('slaves') so that they all also capture an image at the same time.
//
// NOTE: This demo does not configure FEATURE_TRIGGER_WITH_CONTROLLED_LIGHTING. See the demo 
//       app 'triggerwithcontrolledlight' for example code to control this feature. 
//
////////////////////////////////////////////
// The Master Camera
//
// The master camera will be configured for software trigging, and GPO2 will be put into 'strobe' mode so that when the 
// camera is software triggered (i.e. PxLGetNextFrame is called), GPO2 will trigger all the slave cameras.
//
// The serial number of the master camera must be passed in (as a decimal number) on the command line.
//
////////////////////////////////////////////
// The Slave Cameras
//
// The slave cameras will all be configured for hardware triggering, The  hardware trigger for each slave camera will be 
// connected to GPO2 on the master camera.
// One thread will be created for each slave camera. That thread must be blocked in PxLGetNextFrame when the trigger is received.
//
////////////////////////////////////////////
//
// This sample software is purposely pretty light on the error handling so that it can focus primarily on demonstrating
// how to configure and control the cameras.
//
//

#include <PixeLINKApi.h>
#include <stdio.h>
#include <vector>
#include <process.h>
#include <assert.h>
#include "CameraUtils.h"

#define ASSERT(x) do { assert((x)); } while (0)

// Handles for the cameras we'll be interacting with
static U32		s_masterSerialNumber;
static size_t	s_masterCameraIndex; // which camera in the s_hCameras array is the master camera?
static std::vector<HANDLE> s_hCameras;
// This struct is passed to each slave image acquisition thread
struct SlaveCameraInfo {
	HANDLE m_hThread;
	volatile bool m_threadStarted;
	volatile bool m_keepRunning;
	HANDLE m_hCamera;
};
static std::vector<SlaveCameraInfo> s_slaveCameraInfo;

// Some useful constants


// For simplicity, we're using a number big enough to hold any PixeLINK camera image.
// We recommend for an actual application that you properly determine the required image size. 
// See the function DetermineRawImageSize() in the 'getsnapshot' demo app for an example of how to do this.
static const U32 MAX_FRAME_SIZE = 3000 * 3000 * sizeof(U16); 
static const int MASTER_GPO_FOR_TRIGGERING_SLAVES = 2;	// Assumes you've connected GPO2 on the master to the hardware trigger on all the slave cameras

//////////////////////////////////////////////////////////////////////////////////////////////////
// Local Function Prototypes
//
static bool	InitializeCameras();
static void	UninitializeCameras();
static bool	ConfigureCameras();
static bool	ConfigureMasterCamera(HANDLE hCamera);
static bool	ConfigureSlaveCamera(HANDLE hCamera);
static bool	StartImageAcquisitionThreadsForSlaveCameras();
static bool	StartCapture();
static bool StopCapture();
static unsigned int __stdcall	ImageCapturingThread(void* pArguments);
static bool	CaptureImage(HANDLE hCamera, U32 frameSize, U8* pFrame, FRAME_DESC* pFrameDesc);



//////////////////////////////////////////////////////////////////////////////////////////////////

static const int PARAM_EXE_PATH			= 0;
static const int PARAM_MASTER_CAMERA	= 1;
static const int EXE_SUCCESS	= 0;
static const int EXE_ERROR		= 1;

int 
main(int argc, char* argv[])
{
	// What's the serial number of the control camera?
	if (argc <= PARAM_MASTER_CAMERA) {
		printf("ERROR: You did not specify the serial number of the master camera on the command line\n");
		return EXE_ERROR;
	}

	const U32 masterSerialNumber = (U32)atoi(argv[PARAM_MASTER_CAMERA]);
	if (0 == masterSerialNumber) {
		printf("ERROR: The serial number %d for the master camera is invalid\n", masterSerialNumber);
		return EXE_ERROR;
	}

	s_masterSerialNumber = masterSerialNumber;
	if (!InitializeCameras()) {
		return EXE_ERROR;
	}
	if (!ConfigureCameras()) {
		UninitializeCameras();
		return EXE_ERROR;
	}

	const bool rv = StartCapture();

	UninitializeCameras();

	return (rv) ? EXE_SUCCESS : EXE_ERROR;
}


//////////////////////////////////////////////////////////////////////

static bool
InitializeCameras()
{
	// Ask how many cameras there are
	U32 numCameras;
	PXL_RETURN_CODE rc = PxLGetNumberCameras(NULL, &numCameras);
	if (!API_SUCCESS(rc)) return false;
	if (numCameras < 2)  return false;

	// Now read in the serial numbers
	std::vector<U32> serialNumbers(numCameras,0);
	rc = PxLGetNumberCameras(&serialNumbers[0], &numCameras);
	if (!API_SUCCESS(rc)) return false;
	ASSERT(numCameras == serialNumbers.size());

	// Can we find the serial number of the master camera? 
	int masterCameraIndex = -1;
	for(size_t i=0; i < serialNumbers.size(); i++) {
		if (serialNumbers[i] == s_masterSerialNumber) {
			masterCameraIndex = (int)i;
			break;
		}
	}
	if (-1 == masterCameraIndex) {
		return false;
	}

	// Now initialize/connect to each of the cameras
	std::vector<HANDLE> cameraHandles;
	for(size_t i = 0; i < serialNumbers.size(); i++) {
		HANDLE hCamera;
		rc = PxLInitialize(serialNumbers[i], &hCamera);
		if (!API_SUCCESS(rc)) break;
		cameraHandles.push_back(hCamera);
	}
	
	// If we weren't able to initialize to all the cameras, then uninitialize those we did and return an error
	if (!API_SUCCESS(rc)) {
		for(size_t i=0; i < cameraHandles.size(); i++) {
			PxLUninitialize(cameraHandles[i]);
		}
		return false;
	}
	ASSERT(serialNumbers.size() == cameraHandles.size());

	// Now save the handles in the file-scope variables
	s_hCameras = cameraHandles;
	s_masterCameraIndex = masterCameraIndex;

	return true;
}

static void
UninitializeCameras()
{
	for(size_t i=0; i < s_hCameras.size(); i++) {
		PxLUninitialize(s_hCameras[i]);
	}
}


static bool
ConfigureCameras()
{
	for(size_t i=0; i < s_hCameras.size(); i++) {
		bool rv;
		if (i == s_masterCameraIndex) {
			rv = ConfigureMasterCamera(s_hCameras[i]);
		} else {
			rv = ConfigureSlaveCamera(s_hCameras[i]);
		}
		if (!rv) {
			return false;
		}
	}

	return true;
}

//
// The master camera uses software triggering, and triggering with controlled lighting. 
// Therefore, GPO1 is used to control the lighting, and GPO2 will trigger all the slave cameras.
// *** Remember, set the triggering_with_controlled_lighting, THEN set triggering
//
static bool
ConfigureMasterCamera(const HANDLE hCamera)
{
	PXL_RETURN_CODE rc;

	rc = EnableSoftwareTriggering(hCamera);
	if (!API_SUCCESS(rc)) return false;

	rc = EnableGpoForStrobe(hCamera, MASTER_GPO_FOR_TRIGGERING_SLAVES);
	if (!API_SUCCESS(rc)) return false;

	return true;
}

//
// A slave camera uses hardware triggering, and triggering with controlled lighting. 
// Therefore, GPO1 is used to control the lighting, and GPO2 is unused.
//
static bool 
ConfigureSlaveCamera(const HANDLE hCamera)
{
	EnableHardwareTriggering(hCamera);
	return true;
}


//////////////////////////////////////////////////////////////////////

static bool
SetStreamStateForAllCameras(const int streamState)
{
	for(size_t i=0; i < s_hCameras.size(); i++) {
		const PXL_RETURN_CODE rc = PxLSetStreamState(s_hCameras[i], streamState);
		if (!API_SUCCESS(rc)) {
			return false;
		}
	}
	return true;
}

//
// Start an image acquisition thread for each slave camera
// Pass the thread a pointer a struct with the information it'll need.
//
static bool
StartImageAcquisitionThreadsForSlaveCameras()
{
	for(size_t i=0; i < s_hCameras.size(); i++) {
		if (i != s_masterCameraIndex) {
			SlaveCameraInfo ci;
			ci.m_hCamera = s_hCameras[i];
			ci.m_keepRunning = true;
			ci.m_threadStarted = false;
			s_slaveCameraInfo.push_back(ci);
			SlaveCameraInfo * const pCameraInfo = &s_slaveCameraInfo[s_slaveCameraInfo.size()-1];

			// Launch the thread
			unsigned int threadId;
			pCameraInfo->m_hThread = (HANDLE)_beginthreadex(NULL, 0, ImageCapturingThread, pCameraInfo, 0, &threadId);
			if ((HANDLE)-1 == pCameraInfo->m_hThread) return false;

			// Poll to detect that the thread has started
			while(!pCameraInfo->m_threadStarted) {
				Sleep(10);
			}
		}
	}

	// Wait a little bit just to ensure all the threads are blocked in PxLGetNextFrame
	Sleep(500);
	return true;

}

//
// Everything's initialized and configured. Now we can start capturing images.
// Very simple interface using MessageBox.
//
static bool
StartCapture()
{
	if (!SetStreamStateForAllCameras(START_STREAM)) {
		return false;
	}

	if (!StartImageAcquisitionThreadsForSlaveCameras()) {
		return false;
	}

	const HANDLE hMasterCamera = s_hCameras[s_masterCameraIndex];
	std::vector<U8> frame(MAX_FRAME_SIZE);
	FRAME_DESC frameDesc;

	while(true) {
		// Wait for the user to say they want to capture an image
		int rv = MessageBox(0, "Press OK to capture an image, Cancel to quit.", "Capture an image", MB_ICONINFORMATION | MB_OKCANCEL);
		if (IDCANCEL == rv) {
			break;
		}

		// Call PxLGetNextFrame on the master camera
		bool isSuccess = CaptureImage(hMasterCamera, (U32)frame.size(), &frame[0], &frameDesc);
		// Error handling here

	}

	return StopCapture();
}

//
// Tell all the image acquisition threads that they're supposed to stop running, then stop streaming on all cameras.
// Stopping the streaming on the slave cameras will cause PxLGetNextFrame to return an error (and therefore unblock the call to PxLGetNextFrame)
//
static bool 
StopCapture()
{
	for(size_t i = 0; i < s_slaveCameraInfo.size(); i++) {
		s_slaveCameraInfo[i].m_keepRunning = false;
	}

	if (!SetStreamStateForAllCameras(STOP_STREAM)) {
		return false;
	}

	// Now wait for all the image acquisition threads to finish
	bool isSuccess = true;
	for(size_t i = 0; i < s_slaveCameraInfo.size(); i++) {
		const int rv = WaitForSingleObject(s_slaveCameraInfo[i].m_hThread, 5*1000);
		if (WAIT_OBJECT_0 != rv) {
			isSuccess = false;
		}
	}
	
	return isSuccess;
}


//
// The image acquisition thread for each slave camera.
// 
//
static unsigned int __stdcall	
ImageCapturingThread(void* const pArguments)
{
	ASSERT(NULL != pArguments);
	SlaveCameraInfo * const pCameraInfo((SlaveCameraInfo*)pArguments);
	ASSERT(0 != pCameraInfo->m_hCamera);

	// Declare a buffer big enough to hold data for any PixeLINK camera
	std::vector<U8> frame(MAX_FRAME_SIZE);
	FRAME_DESC frameDesc;

	// Notify the launcher we're running. Unfortunately there's no deterministic way to 
	// know when this thread is truly blocked in PxLGetNextFrame.
	pCameraInfo->m_threadStarted = true;

	while(pCameraInfo->m_keepRunning) {
		CaptureImage(pCameraInfo->m_hCamera, (U32)frame.size(), &frame[0], &frameDesc);
	}

	return 0;
}

//
// Common image capturing function
//
static bool
CaptureImage(const HANDLE hCamera, const U32 frameSize, U8* const pFrame, FRAME_DESC* const pFrameDesc)
{
	ASSERT(0 != hCamera);
	ASSERT(NULL != pFrameDesc);
	ASSERT(NULL != pFrame);
	ASSERT(frameSize > 0);

	pFrameDesc->uSize = sizeof(FRAME_DESC);
	const PXL_RETURN_CODE rc = PxLGetNextFrame(hCamera, frameSize, pFrame, pFrameDesc);
	if (API_SUCCESS(rc)) {
		// Store the image here or do whatever you want with the image
		printf("Image %d captured by 0x%8.8X\n", pFrameDesc->uFrameNumber, (U32)hCamera);
	} else {
		// Whatever error handling is appropriate
		printf("ERROR: Failed to capture an image from camera 0x%8.8X\n", hCamera);
	}

	return API_SUCCESS(rc);
}