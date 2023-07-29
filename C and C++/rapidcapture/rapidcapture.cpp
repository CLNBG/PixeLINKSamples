//
// rapidcapture.cpp
//
// Capture a number of images as rapidly as possible from a camera and then save them to file.
// The BMP files are named imageX_Y.bmp, where X is the number of the image captured, and Y is
// the frame number of the image. This makes it possible to detect missed images.
//
// To speed things up, we save the images first to memory, then, after capture of
// the images is complete, we save the captured images to file.
//
#include <PixeLINKApi.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cassert>


#define ASSERT(x) do { assert((x)); } while(0)


// Local function prototypes
static PXL_RETURN_CODE	RapidCapture(HANDLE, U32);


enum {
	PARAM_EXE_NAME = 0,
	PARAM_NUM_FRAMES_TO_CAPTURE
};

int 
main(int argc, char* argv[])
{
	// Did they specify how many frames to capture?
	int numFramesToCapture = 10; // Default
	if (argc > PARAM_NUM_FRAMES_TO_CAPTURE) {
		numFramesToCapture = ::atoi(argv[PARAM_NUM_FRAMES_TO_CAPTURE]);
	}
	if (numFramesToCapture <= 0) {
		printf("ERROR: The number of frames to capture (%d) is invalid\n", numFramesToCapture);
		return EXIT_FAILURE;
	}

	// Now see if we can initialize a camera
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		::printf("ERROR: Unable to initialize a camera (0x%8.8X)\n", rc);
		return EXIT_FAILURE;
	}

	rc = RapidCapture(hCamera, numFramesToCapture);

	return (API_SUCCESS(rc)) ? EXIT_SUCCESS : EXIT_FAILURE;
}

//
// Determine the width and height of the region of interest that
// the camera is currently using.
//
static PXL_RETURN_CODE 
GetRoiWidthAndHeight(const HANDLE hCamera, U32& width, U32& height)
{
	// Determine how many parameters for the ROI feature
	U32 flags(0);
	U32 numParams(0);
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_ROI, &flags, &numParams, NULL);
	if (!API_SUCCESS(rc)) { return rc; };
	ASSERT(numParams >= 4);

	// Allocate memory for the parameters, and then read them
	std::vector<float> params(numParams, 0.0f);
	rc = PxLGetFeature(hCamera, FEATURE_ROI, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) { return rc; };
	
	// Extract the two dimensions of the ROI that we need
	width	= static_cast<U32>(params[FEATURE_ROI_PARAM_WIDTH]);
	height	= static_cast<U32>(params[FEATURE_ROI_PARAM_HEIGHT]);
	return ApiSuccess;
}


//
// Read the pixel format, and then use that to determine the size, in bytes, of
// the pixels being returned by the camera.
//
// This is adapted from code taken from the sample app 'getsnapshot'.
//
static PXL_RETURN_CODE
GetPixelSize(const HANDLE hCamera, float& bytesPerPixel)
{
	// Get the pixel format
	U32 flags(0);
	U32 numParams(1);
	float fPixelFormat;
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_PIXEL_FORMAT, &flags, &numParams, &fPixelFormat);
	if (!API_SUCCESS(rc)) { return rc; }

	const U32 pixelFormat = static_cast<U32>(fPixelFormat);

	rc = ApiSuccess;
	switch(pixelFormat) {
	
		case PIXEL_FORMAT_MONO8:
		case PIXEL_FORMAT_BAYER8_GRBG:
		case PIXEL_FORMAT_BAYER8_RGGB:
		case PIXEL_FORMAT_BAYER8_GBRG:
		case PIXEL_FORMAT_BAYER8_BGGR:
			bytesPerPixel = 1.0f;
			break;

		case PIXEL_FORMAT_YUV422:
		case PIXEL_FORMAT_MONO16:
		case PIXEL_FORMAT_BAYER16_GRBG:
		case PIXEL_FORMAT_BAYER16_RGGB:
		case PIXEL_FORMAT_BAYER16_GBRG:
		case PIXEL_FORMAT_BAYER16_BGGR:
			bytesPerPixel = 2.0f;
			break;

      case PIXEL_FORMAT_RGB24_DIB:
      case PIXEL_FORMAT_RGB24_NON_DIB:
      case PIXEL_FORMAT_BGR24_NON_DIB:
         bytesPerPixel = 3.0f;
			break;

      case PIXEL_FORMAT_RGBA:
      case PIXEL_FORMAT_BGRA:
      case PIXEL_FORMAT_ARGB:
      case PIXEL_FORMAT_ABGR:
         bytesPerPixel = 4.0f;
         break;

      case PIXEL_FORMAT_RGB48:
			bytesPerPixel = 6.0f;
			break;

      case PIXEL_FORMAT_MONO12_PACKED:
      case PIXEL_FORMAT_BAYER12_BGGR_PACKED:
      case PIXEL_FORMAT_BAYER12_GBRG_PACKED:
      case PIXEL_FORMAT_BAYER12_GRBG_PACKED:
      case PIXEL_FORMAT_BAYER12_RGGB_PACKED:
      case PIXEL_FORMAT_MONO12_PACKED_MSFIRST:
      case PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST:
      case PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST:
      case PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST:
      case PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST:
         bytesPerPixel = 1.5f;
         break;

      case PIXEL_FORMAT_MONO10_PACKED_MSFIRST:
      case PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST:
      case PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST:
      case PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST:
      case PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST:
         bytesPerPixel = 1.25f;
         break;

      case PIXEL_FORMAT_STOKES4_12:
      case PIXEL_FORMAT_POLAR4_12:
      case PIXEL_FORMAT_POLAR_RAW4_12:
      case PIXEL_FORMAT_HSV4_12:
         bytesPerPixel = 6.0f;
         break;


      default:
			ASSERT(0);
			rc = ApiNotSupportedError;
			break;
	}

	return rc;
}

//
// Read the pixel addressing value.
//
static PXL_RETURN_CODE
GetPixelAddressingValue(const HANDLE hCamera, U32& value)
{
	// Determine the number of parameters
	U32 flags(0);
	U32 numParams(0);
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_PIXEL_ADDRESSING, &flags, &numParams, NULL);
	if (!API_SUCCESS(rc)) { return rc; }
	ASSERT(numParams >= 2);

	// Allocate memory for the parameters, and read them from memory
	std::vector<float> params(numParams, 0.0f);
	rc = PxLGetFeature(hCamera, FEATURE_PIXEL_ADDRESSING, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) { return rc; }

	value = static_cast<U32>(params[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE]);
	return ApiSuccess;


}

//
// Determine the size of a frame that will be returned by PxLGetNextFrame.
//
static PXL_RETURN_CODE 
GetFrameBufferSize(const HANDLE hCamera, U32& frameBufferSize)
{
	U32 width(0);
	U32 height(0);
	PXL_RETURN_CODE rc = GetRoiWidthAndHeight(hCamera, width, height);
	if (!API_SUCCESS(rc)) {
		::printf("ERROR: Unable to determine the ROI (0x%8.8X)\n", rc);
		return rc;
	}

	U32 pixelAddressingValue(0);
	rc = GetPixelAddressingValue(hCamera, pixelAddressingValue);
	if (!API_SUCCESS(rc)) { 
		::printf("ERROR: Unable to determine the pixel addressing value (0x%8.8X)\n", rc);
		return rc;
	}
	
	ASSERT(pixelAddressingValue > 0);

	ASSERT(0 == (height % pixelAddressingValue));
	ASSERT(0 == (width % pixelAddressingValue));
	const U32 numPixels = (width / pixelAddressingValue) * (height / pixelAddressingValue);

	float bytesPerPixel(0.0f);
	rc = GetPixelSize(hCamera, bytesPerPixel);
	if (!API_SUCCESS(rc)) { 
		::printf("ERROR: Unable to determine the pixel size (0x%8.8X)\n", rc);
		return rc;
	}

	ASSERT(bytesPerPixel >= 1.0f);

	frameBufferSize = (U32)((float)numPixels * bytesPerPixel);

	return ApiSuccess;
}

//
// Save all the captured frames to files.
//
static PXL_RETURN_CODE
SaveFrames(const std::vector< std::vector<U8> >& frames, const std::vector<FRAME_DESC>& frameDescs)
{
	// If you don't want to use BMPs, change these values accordingly.
	const U32 outputFormat = IMAGE_FORMAT_BMP;
	const std::string outputExtension("bmp");

	ASSERT(frames.size() == frameDescs.size());
	for(size_t i=0; i < frames.size(); i++) {

		// Determine the size of the formatted image so we can alloc a buffer for it
		U32 formattedImageSize(0);
		PXL_RETURN_CODE rc = PxLFormatImage(&frames[i][0], &frameDescs[i], outputFormat, NULL, &formattedImageSize);
		if (!API_SUCCESS(rc)) {
			::printf("ERROR: Failed to determine the formatted image size.\n");
			return rc;
		}
		std::vector<U8> formattedImage;
		try {
			formattedImage.resize(formattedImageSize);
		} catch (std::bad_alloc&) {
			::printf("ERROR: Unable to allocate space for formatted image.\n");
			return ApiNotEnoughResourcesError;
		}
		rc = PxLFormatImage(&frames[i][0], &frameDescs[i], outputFormat, &formattedImage[0], &formattedImageSize);
		if (!API_SUCCESS(rc)) {
			::printf("ERROR: Failed to format the image.\n");
			return rc;
		}

		{
			// Write the data to a binary file using the naming convention
			// mentioned at the top of the file.
			char fileName[MAX_PATH];
			::sprintf(&fileName[0], "image%3.3d_%5.5d.%s", i, frameDescs[i].uFrameNumber, outputExtension.c_str());
			FILE* pFile = fopen(&fileName[0], "wb");
			if (NULL == pFile) {
				::printf("ERROR: Failed to open a file\n");
				return ApiUnknownError;
			}
			//numBytesWritten = fwrite((void*)pImage, sizeof(char), imageSize, pFile);
			size_t numBytesWritten = ::fwrite(&formattedImage[0], sizeof(formattedImage[0]), formattedImage.size(), pFile);
			fclose(pFile);
			if (numBytesWritten != formattedImage.size()) {
				::printf("ERROR: Failed to write all data to %s\n", fileName);
				return ApiUnknownError;
			}
		}
		
	}
	return ApiSuccess;
}

//
// This is the main code which allocates memory, captures images, then saves those 
// captured images to files.
//
static PXL_RETURN_CODE 
RapidCapture(const HANDLE hCamera, const U32 numFramesToCapture)
{
	ASSERT(0 != hCamera);

	PXL_RETURN_CODE rc;

	U32 frameBufferSize(0);
	rc = GetFrameBufferSize(hCamera, frameBufferSize);
	if (!API_SUCCESS(rc)) { return rc; }

	::printf("Each frame is %d (0x%8.8X) bytes\n", frameBufferSize, frameBufferSize);

	// Allocate all the memory we're going to need 
	// If we run out of memory, a bad_alloc exception will be thrown.
	::printf("Allocating memory...\n");
	std::vector< std::vector<U8> > frames;
	std::vector<FRAME_DESC> frameDescs;
	try {
		frameDescs.resize(numFramesToCapture);
		frames.resize(numFramesToCapture);
		// Allocate the memory for each frame
		for(size_t i=0; i < frames.size(); i++) {
			frames[i].resize(frameBufferSize);
		}
	} catch (const std::bad_alloc&) {
		::printf("ERROR: Not enough memory\n");
		return ApiNotEnoughResourcesError;
	}
	

	::printf("Starting streaming...\n");
	rc = PxLSetStreamState(hCamera, START_STREAM);
	if (!API_SUCCESS(rc)) {
		::printf("ERROR: Failed to start the stream (0x%8.8X)\n", rc);
		return rc;
	}

	//
	// Capture images from the camera to memory
	//
	::printf("Starting capture of %d images...\n", numFramesToCapture);
	for(U32 i=0; i < numFramesToCapture; i++) {
		// Try up to 5 times to capture an image
		for(int j=0; j < 5; j++) {
			frameDescs[i].uSize = sizeof(frameDescs[i]);
			rc = PxLGetNextFrame(hCamera, static_cast<U32>(frames[i].size()), &frames[i][0], &frameDescs[i]);
			if (API_SUCCESS(rc)) {
				break;
			}
		}
		if (!API_SUCCESS(rc)) {
			::printf("ERROR: Failed to acquire a frame (0x%8.8X)\n", rc);
			return rc;
		}
	}

	rc = PxLSetStreamState(hCamera, STOP_STREAM);

	//
	// Convert the images to bmps and save to file 
	//
	::printf("Saving captured frames...\n");
	rc = SaveFrames(frames, frameDescs);

	return rc;
}

