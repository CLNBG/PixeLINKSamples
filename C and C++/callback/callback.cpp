//
// callback.cpp 
//
// Demonstrates how to use callbacks with CALLBACK_FORMAT_IMAGE and CALLBACK_PREVIEW
// The callback functions here do not modify the image data. 
//
//
#include <PixeLINKAPI.h>
#include <stdio.h>
#include <vector>

// Local function prototypes
static const char* GetPixelFormatAsString(U32 dataFormat);
static void DoCallbackOnFormatImage(HANDLE hCamera);
static void	DoCallbackOnPreview(HANDLE hCamera);


int 
main(int argc, char* argv[])
{

	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!(API_SUCCESS(rc))) {
		printf("ERROR: 0x%8.8X\n", rc);
		return EXIT_FAILURE;
	}

	printf("Main thread id = %d\n\n", GetCurrentThreadId());

	DoCallbackOnFormatImage(hCamera);
	DoCallbackOnPreview(hCamera);

	PxLUninitialize(hCamera);
	return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Callback function called by the API when an image is being converted via a call to PxLFormatImage.
// Note that the hCamera passed into this function will be 0 because it's API-global. i.e. there is only one
// callback function for PxLFormatImage. If you're controlling two cameras at once, any call to PxLFormatImage will
// end up invoking this function. 
//
// Note too that this function is called by the API using the same thread that called PxLFormatImage.
//
// N.B. The callback calling convention is __stdcall
//
//
static U32 __stdcall 
CallbackFormatImage(
	HANDLE		hCamera,
	LPVOID		pData,
	U32			dataFormat,
	FRAME_DESC const *	pFrameDesc,
	LPVOID		userData)
{
	printf("CallbackFormatImage: hCamera=0x%8.8X, pData=0x%8.8X\n", (U32)hCamera, (U32)pData);
	printf("    dataFormat=%d (%s), pFrameDesc=0x%8.8X\n", dataFormat, GetPixelFormatAsString(dataFormat), pFrameDesc);
	printf("    userData=0x%8.8X, threadId=%d\n\n", userData, GetCurrentThreadId());
	return ApiSuccess;
}

static void 
DoCallbackOnFormatImage(HANDLE hCamera)
{
	// Set the callback function
	printf("============================================\n");
	printf("DoCallbackOnFormatImage\n");
	U32 userData = 0xCAFEBABE;
	printf("Registering FORMAT_IMAGE callback with userData 0x%8.8X\n", userData);

	PXL_RETURN_CODE rc = PxLSetCallback(
		0,			// for IMAGE_FORMAT_IMAGE, have to pass in 0
		CALLBACK_FORMAT_IMAGE,
		(LPVOID)userData,
		CallbackFormatImage);
	if (!API_SUCCESS(rc)) {
		printf("ERROR setting callback function: 0x%8.8X\n", rc);
		return;
	}

	std::vector<U8> rawDataBuffer(3000*3000*2); // buffer big enough to hold any image
	FRAME_DESC frameDesc;

	rc = PxLSetStreamState(hCamera, START_STREAM);

	printf("Capturing a  frame...\n");
	while(1) {
		frameDesc.uSize = sizeof(FRAME_DESC);
		rc= PxLGetNextFrame(hCamera, (U32)rawDataBuffer.size(), &rawDataBuffer[0], &frameDesc);
		if (API_SUCCESS(rc)) {
			break;
		}
	}
	printf("frameDesc.PixelFormat=%d (%s)\n\n",(U32)frameDesc.PixelFormat.fValue, GetPixelFormatAsString((U32)frameDesc.PixelFormat.fValue));

	rc = PxLSetStreamState(hCamera, STOP_STREAM);

	printf("Calling PxLFormatImage with pBuffer=0x%8.8X, &frameDesc=0x%8.8X\n\n", &rawDataBuffer, &frameDesc);
	

	// And now format the image 
	// 1) How big is the resulting image going to be?
	// N.B. The callback function will not be invoked during this call
	U32 formattedImageSize = -1;
	rc = PxLFormatImage(&rawDataBuffer[0], &frameDesc, IMAGE_FORMAT_BMP, NULL, &formattedImageSize);

	std::vector<U8> formattedImage(formattedImageSize);

	// 2) Create the formatted image
	// The callback will be invoked during this call
	rc = PxLFormatImage(&rawDataBuffer[0], &frameDesc, IMAGE_FORMAT_BMP, &formattedImage[0], &formattedImageSize);

	// Disable the callback function
	rc = PxLSetCallback(0, CALLBACK_FORMAT_IMAGE, NULL, NULL);

	
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Callback function called by the API just before an image is displayed in the preview window. 
//
// N.B. This is called by the API on a thread created in the API. 
// N.B. The callback calling convention is __stdcall
//

static U32 __stdcall 
CallbackFormatPreview(
	HANDLE		hCamera,
	LPVOID		pData,
	U32			dataFormat,
	FRAME_DESC const *	pFrameDesc,
	LPVOID		userData)
{
	printf("CallbackFormatPreview: hCamera=0x%8.8X, pData=0x%8.8X\n", (U32)hCamera, (U32)pData);
	printf("    dataFormat=%d (%s), pFrameDesc=0x%8.8X\n", dataFormat, GetPixelFormatAsString(dataFormat), pFrameDesc);
	printf("    userData=0x%8.8X, threadId=%d\n\n", userData, GetCurrentThreadId());
	return ApiSuccess;
}


static void 
DoCallbackOnPreview(HANDLE hCamera)
{
	// Set the callback function
	printf("============================================\n");
	printf("DoCallbackOnPreview\n");
	U32 userData = 0xDEADBEEF;
	printf("Registering PREVIEW callback with userData 0x%8.8X\n\n", userData);

	PXL_RETURN_CODE rc = PxLSetCallback(
		hCamera,			// for IMAGE_FORMAT_IMAGE, have to pass in 0
		CALLBACK_PREVIEW,
		(LPVOID)userData,
		CallbackFormatPreview);
	if (!API_SUCCESS(rc)) {
		printf("ERROR setting callback function: 0x%8.8X\n", rc);
		return;
	}

	rc = PxLSetStreamState(hCamera, START_STREAM);

	// We will start getting our callback called after we start previewing
	HWND hWnd;
	rc = PxLSetPreviewState(hCamera, START_PREVIEW, &hWnd);

	Sleep(20*1000); // Sleep 20 seconds

	rc = PxLSetPreviewState(hCamera, STOP_PREVIEW, &hWnd);

	rc = PxLSetStreamState(hCamera, STOP_STREAM);

}


#define CASE(x) case x: return #x

static const char*
GetPixelFormatAsString(U32 dataFormat)
{
	switch(dataFormat) {
		CASE(PIXEL_FORMAT_MONO8);
		CASE(PIXEL_FORMAT_MONO16);
		CASE(PIXEL_FORMAT_YUV422);
		CASE(PIXEL_FORMAT_BAYER8_GRBG);
		CASE(PIXEL_FORMAT_BAYER16_GRBG);
		CASE(PIXEL_FORMAT_RGB24);
		CASE(PIXEL_FORMAT_RGB48);
		CASE(PIXEL_FORMAT_BAYER8_RGGB);
		CASE(PIXEL_FORMAT_BAYER8_GBRG);
		CASE(PIXEL_FORMAT_BAYER8_BGGR);
		CASE(PIXEL_FORMAT_BAYER16_RGGB);
		CASE(PIXEL_FORMAT_BAYER16_GBRG);
		CASE(PIXEL_FORMAT_BAYER16_BGGR);
		default: return "Unknown data format";
	}

}