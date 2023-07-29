

#include "Camera.h"
#include "Image.h"
#include <cassert>
#define ASSERT(x) do { assert((x)); } while(0)


Camera::Camera(const HANDLE hCamera) 
	: m_hCamera(hCamera), m_isPreviewing(false)
{
	ASSERT(0 != m_hCamera);
}

U32 
Camera::GetImageSize() const
{
	// Assuming we're using the camera in 8-bit mode
	return GetNumberOfPixels() * sizeof(U8);
}

void	
Camera::ConfigureFor8Bit() const
{
	U32 flags(0);
	U32 numParams(1);
	float param(0.0f);
	PXL_RETURN_CODE rc = PxLGetFeature(m_hCamera, FEATURE_PIXEL_FORMAT, &flags, &numParams, &param);
	ASSERT(API_SUCCESS(rc));
	ASSERT(1 == numParams);
	U32 pixelFormat = static_cast<U32>(param);
	switch(pixelFormat) 
	{
		case PIXEL_FORMAT_MONO8:
		case PIXEL_FORMAT_BAYER8_GRBG:
		case PIXEL_FORMAT_BAYER8_RGGB:
		case PIXEL_FORMAT_BAYER8_GBRG:
		case PIXEL_FORMAT_BAYER8_BGGR:
			return;
	}

	// Currently not 8 bit, so set to 8 bit.
	if (PIXEL_FORMAT_MONO16 == pixelFormat) {
		pixelFormat = PIXEL_FORMAT_MONO8;
	} else {
		pixelFormat = PIXEL_FORMAT_BAYER8;
	}
	param = static_cast<float>(pixelFormat);
	rc = PxLSetFeature(m_hCamera, FEATURE_PIXEL_FORMAT, flags, numParams, &param);
	ASSERT(API_SUCCESS(rc));
}

void 
Camera::StartStreaming() const
{
	PXL_RETURN_CODE rc = PxLSetStreamState(m_hCamera, START_STREAM);
	ASSERT(API_SUCCESS(rc));
}

U32 
Camera::GetNumberOfPixels() const
{
	// Read the ROI
	U32 flags(0);
	U32 numParams(4);
	std::vector<float> params(4,0.0f);
	PXL_RETURN_CODE rc = PxLGetFeature(m_hCamera, FEATURE_ROI, &flags, &numParams, &params[0]);
	ASSERT(API_SUCCESS(rc));
	U32 numPixels = static_cast<U32>(params[FEATURE_ROI_PARAM_WIDTH]) * static_cast<U32>(params[FEATURE_ROI_PARAM_HEIGHT]);

	// Account for pixel addressing
	numParams = 2;
	rc = PxLGetFeature(m_hCamera, FEATURE_PIXEL_ADDRESSING, &flags, &numParams, &params[0]);
	ASSERT(API_SUCCESS(rc));
	numPixels /= static_cast<U32>(params[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE]);

	return numPixels;

}

//
// Make a few attempts to capture an image.
//
PXL_RETURN_CODE 
Camera::Capture(Image& image) const
{
	PXL_RETURN_CODE rc = ApiUnknownError;
	for(U32 i = 0; i < 5; i++) {
		image.m_frameDesc.uSize = sizeof(image.m_frameDesc);
		rc = PxLGetNextFrame(m_hCamera, static_cast<U32>(image.m_data.size()), &image.m_data[0], &image.m_frameDesc);
		if (API_SUCCESS(rc)) {
			return rc;
		}
	}
	return rc;
}

PXL_RETURN_CODE 
Camera::Capture(char const * const pFileName, U32 format) const
{
	Image image(GetImageSize());
	PXL_RETURN_CODE rc = Capture(image);
	if (!API_SUCCESS(rc)) { return rc; }
	rc = image.Save(pFileName, format);
	if (!API_SUCCESS(rc)) { return rc; }

	return rc;
}
bool	
Camera::IsPreviewing()		const
{
	return m_isPreviewing;
}

void	
Camera::EnablePreview(bool enable)
{
	U32 newState = (enable) ? START_PREVIEW : STOP_PREVIEW;
	HWND hWnd(0);
	PXL_RETURN_CODE rc = PxLSetPreviewState(m_hCamera, newState, &hWnd);
	if (API_SUCCESS(rc)) {
		m_isPreviewing = enable;
	}
}
