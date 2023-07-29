

#include "MotionDetectingThread.h"
#include <ctime>
#include "Image.h"
#include "Camera.h"


MotionDetectingThread::MotionDetectingThread(Camera& camera, const U8 pixelDeltaThreshold, const float percentChangedThreshold, const U32 imageFormat, const std::string& fileExtension) :
	m_camera(camera), 
	m_pixelDeltaThreshold(pixelDeltaThreshold), m_percentChangedThreshold(percentChangedThreshold), 
	m_imageFormat(imageFormat), m_fileExtension(fileExtension),
	m_numberOfImagesCaptured(0), m_verbose(false)
{
	InitializeCriticalSection(&m_critSection);
}


MotionDetectingThread::~MotionDetectingThread()
{
	DeleteCriticalSection(&m_critSection);
}

std::string
MotionDetectingThread::GetFileName()
{
	time_t rawTime;
	time(&rawTime);
	struct tm const * const pTimeInfo = localtime(&rawTime);
	char buffer[128];
	sprintf(buffer, "Image%4.4d_%4.4d%2.2d%2.2d_%2.2d%2.2d%2.2d.%s", 
		m_numberOfImagesCaptured,
		(pTimeInfo->tm_year + 1900), (pTimeInfo->tm_mon+1), pTimeInfo->tm_mday,
		pTimeInfo->tm_hour, pTimeInfo->tm_min, pTimeInfo->tm_sec,
		m_fileExtension.c_str());
	return std::string(buffer);
}

void 
MotionDetectingThread::MainImpl()
{
	PXL_RETURN_CODE rc = DetectMotion();
	if (API_SUCCESS(rc)) {
		::printf("Done detecting motion.\n");
	} else {
		::printf("An error was encountered while detecting motion (Error code 0x%8.8X)\n", rc);
	}
}

//
// The meat and potatoes of this whole demo app.
// Get images from camera. Look for motion. Save images in which motion is detected.
//
PXL_RETURN_CODE
MotionDetectingThread::DetectMotion()
{
	const U32 imageSize = m_camera.GetImageSize();
	Image image1(imageSize);
	Image image2(imageSize);
	Image& lastImage	= image1;
	Image& currentImage = image2;
	
	// Gather a reference 'last' image
	PXL_RETURN_CODE rc = m_camera.Capture(lastImage);
	if (!API_SUCCESS(rc)) { return rc; }

	// Now start gathering images, and comparing them against the last image.
	const U32 numberOfPixels = m_camera.GetNumberOfPixels();
	const float fNumberOfPixels = static_cast<float>(numberOfPixels);

	::printf("Waiting to detect motion...\n");

	while(KeepRunning()) {
		rc = m_camera.Capture(currentImage);
		if (!API_SUCCESS(rc)) { return rc; }

		// We're just locking the verbose flag here
		EnterCriticalSection(&m_critSection);
		// Compare the two images
		// Do this by comparing the individual pixels for change.
		// Count how many pixels have changed, and therefore what percentage of the image has changed.
		U32 numPixelsChanged = 0;
		U32 totalDelta = 0;
		int minDelta = 256;
		int maxDelta = 0;
		for(U32 i = 0; i < numberOfPixels; i++) {
			int delta = ::abs(lastImage.m_data[i]- currentImage.m_data[i]);
			// Some profiling info
			if (m_verbose) {
				totalDelta += delta;
				if (delta < minDelta) { minDelta = delta; }
				if (delta > maxDelta) { maxDelta = delta; }
			}
			if (delta >= m_pixelDeltaThreshold) {
				numPixelsChanged++;
			}
		}

		float percentMoving = (100.0f * static_cast<float>(numPixelsChanged) / fNumberOfPixels);
		float averageDelta = static_cast<float>(totalDelta) / fNumberOfPixels;
		if (m_verbose) {
			::printf("Pixel Deltas: Min:%3.3d, Max:%3.3d, Avg:%3.1f, %% Changed: %3.5f%%\n", 
				minDelta, maxDelta, averageDelta, percentMoving);
		}

		LeaveCriticalSection(&m_critSection);

		// If we 'detect' motion, save the current image.
		if (percentMoving >= m_percentChangedThreshold) {
			::printf("Motion detected. ");
			m_numberOfImagesCaptured++;
			std::string fileName(GetFileName());
			::printf("Saving image %s... ", fileName.c_str());
			rc = currentImage.Save(fileName, m_imageFormat);
			if (API_SUCCESS(rc)) {
				::printf("done.\n");
			} else {
				::printf("Error 0x%8.8X", rc);
				return rc;
			}

		}
		

		// Make the current image the reference image
		Image& temp = lastImage;
		lastImage = currentImage;
		currentImage = temp;
	}

	return ApiSuccess;
}
