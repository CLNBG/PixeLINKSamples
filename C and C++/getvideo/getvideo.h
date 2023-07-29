
#ifndef GETVIDEO_H
#define GETVIDEO_H

#include <PixeLINKApi.h>

PXL_RETURN_CODE GetVideo   (HANDLE hCamera, const char* pFilename, U32 numFramesToCapture, U32* pNumFramesCaptured);
PXL_RETURN_CODE GetRawVideo(HANDLE hCamera, const char* pFilename, U32 numFramesToCapture, U32* pNumFramesCaptured);

#endif