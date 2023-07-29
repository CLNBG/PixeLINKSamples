
#ifndef IMAGE_H
#define IMAGE_H


#include <PixeLINKApi.h>
#include <vector>
#include <string>

//
// Simple class used to hold a raw image from the camera and its associated frame descriptor.
//
class Image
{
public:
	Image(U32 imageSize);
	PXL_RETURN_CODE Save(const std::string& fileName, U32 imageFormat) const;

	std::vector<U8> m_data;
	FRAME_DESC		m_frameDesc;
};

#endif