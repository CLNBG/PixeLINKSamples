#ifndef PIXELINK_COM_OVERLAY_ENGINE_H
#define PIXELINK_COM_OVERLAY_ENGINE_H

#ifdef OVERLAYENGINE_EXPORTS
#define OVERLAYENGINE_API __declspec(dllexport) __stdcall
#else
#define OVERLAYENGINE_API __declspec(dllimport)__stdcall
#endif

#include <PixeLINKApi.h>

#define OVERLAY_ENGINE_SUCCESS	(0)
#define OVERLAY_ENGINE_FAILURE	(-1)

#ifdef __cplusplus
extern "C" {
#endif

//
// Initialize the engine for use
//
int	OVERLAYENGINE_API OverlayEngineInitialize();

//
// Tell the engine to release all resources.
//
int	OVERLAYENGINE_API OverlayEngineUninitialize();


//
// Specify the name of the file with the bitmap to be overlaid
// 
// Parameters:
//
// hCamera			Camera to which overlays are to be applied
// pFileName		Name of hte 24-bit bitmap to overlay onto the image
//					White will be considered to be transparent
//
int	OVERLAYENGINE_API OverlayEngineSetOverlayBitmap(HANDLE hCamera, char* pFileName);
int	OVERLAYENGINE_API OverlayEngineSetOverlayOffset(HANDLE hCamera, int offsetX, int offsetY);

//
//
//	
// Parameters:
// 
//		hCamera			Camera to which overlays are to be applied
//		overlayType		One of the following values
//							CALLBACK_PREVIEW	(1)
//							CALLBACK_FORMAT_IMAGE (2)
//							CALLBACK_FORMAT_CLIP  (4)
//
 int OVERLAYENGINE_API OverlayEngineEnableOverlay(HANDLE hCamera, int overlayType, int enable);

#ifdef __cplusplus
};
#endif




#endif