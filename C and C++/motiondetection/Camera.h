
#ifndef CAMERA_H
#define CAMERA_H


#include <PixeLINKApi.h>
#include <vector>

//
// Simple camera class to help with some of the things we'll need to capture images.
//
class Image;

class Camera
{
public:
	explicit Camera(HANDLE hCamera);

	void	ConfigureFor8Bit()	const;
	void	StartStreaming()	const;
	U32		GetNumberOfPixels()	const;
	U32		GetImageSize()		const;

	bool	IsPreviewing()		const;
	void	EnablePreview(bool enable);

	PXL_RETURN_CODE Capture(Image&) const;
	PXL_RETURN_CODE Capture(const char* fileName, U32 format) const;

protected:
	const HANDLE	m_hCamera;
	bool			m_isPreviewing;
};



#endif