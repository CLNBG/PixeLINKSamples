#ifndef MOTIONDETECTINGTHREAD_H
#define MOTIONDETECTINGTHREAD_H

#include "Thread.h"
#include <PixeLINKApi.h>
#include <string>

//
// Implementation of the thread that will sample images, looking for motion, and then save any images
// in which motion is detected.
//

class Camera;
class MotionDetectingThread : public Thread
{
public:
	MotionDetectingThread(Camera& camera, const U8 pixelDeltaThreshold, const float percentChangedThreshold, const U32 imageFormat, const std::string& fileExtension);
	virtual ~MotionDetectingThread();

	bool	IsVerbose() const { return m_verbose; }
	void	SetVerbose(bool verbose) { m_verbose = verbose; }

protected:
	virtual void MainImpl();
	PXL_RETURN_CODE DetectMotion();

	std::string GetFileName();

	const U32			m_imageFormat;
	const std::string	m_fileExtension;
	Camera&				m_camera;
	U8					m_pixelDeltaThreshold;
	float				m_percentChangedThreshold;
	CRITICAL_SECTION	m_critSection; // Protects the verbose flag
	volatile bool		m_verbose;
	U32					m_numberOfImagesCaptured;
};


#endif