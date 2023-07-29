#include "Image.h"
#include <cassert>
#include <cstdio>
#define ASSERT(x) do { assert((x)); } while(0)

//
// The helper functions here are based on some helpers in the GetSnapshot demo app.
//

//
// Given the raw image, put the encoded image into the provided vector.
//
static PXL_RETURN_CODE
EncodeRawImage(const Image& rawImage,
			   const U32 encodedImageFormat, 
			   std::vector<U8>& encodedImage)
{
	U32 encodedImageSize = 0;
	PXL_RETURN_CODE rc;

	// How big is the encoded image going to be?
	// Pass in NULL for the encoded image pointer, and the result is
	// returned in encodedImageSize
	rc = PxLFormatImage(&rawImage.m_data[0], &rawImage.m_frameDesc, encodedImageFormat, NULL, &encodedImageSize);
	if (!API_SUCCESS(rc)) { return rc; }


	ASSERT(encodedImageSize > 0);
	encodedImage.resize(encodedImageSize);

	// Now that we have a buffer for the encoded image, ask for it to be converted.
	rc = PxLFormatImage(&rawImage.m_data[0], &rawImage.m_frameDesc, encodedImageFormat, &encodedImage[0], &encodedImageSize);
	
	return rc;
}

//
// Save a buffer to a file
// This overwrites any existing file
//
static bool
SaveImageToFile(const std::string& fileName, const std::vector<U8>& image)
{
	size_t numBytesWritten;
	FILE* pFile;

	ASSERT(image.size() > 0);

	// Open our file for binary write
	pFile = fopen(fileName.c_str(), "wb");
	if (NULL == pFile) {
		return 0;
	}

	numBytesWritten = fwrite(&image[0], sizeof(image[0]), image.size(), pFile);

	fclose(pFile);
	
	return (numBytesWritten == image.size());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Image::Image(const U32 imageSize) : m_data(imageSize)
{
	// Nothing to do here
}

PXL_RETURN_CODE 
Image::Save(const std::string& fileName, const U32 imageFormat) const
{
	std::vector<U8> encodedImage;
	PXL_RETURN_CODE rc = EncodeRawImage(*this, imageFormat, encodedImage);
	if (!API_SUCCESS(rc)) { return rc; }

	bool success = SaveImageToFile(fileName, encodedImage);
	return success ? ApiSuccess : ApiUnknownError;
}

	


