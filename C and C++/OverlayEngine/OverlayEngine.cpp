//
// A simple overlay engine for environments that can't easily make use of callbacks.
//
// Right now we only support one bitmap at a time, and it applies to all cameras and all callback types.
// 
// Feel free to modify, adapt, or improve.
//


#include <windows.h>
#include <PixeLINKApi.h>
#include "OverlayEngine.h"
#include <vector>
#include <algorithm>

#include <cassert>
#define ASSERT(x)	do { assert((x)); } while(0)


//
// Using RAII to enter and leave a critical section
//
class CriticalSectionLocker
{
public:
	CriticalSectionLocker(CRITICAL_SECTION& cs) : m_cs(cs)
	{
			EnterCriticalSection(&cs); 
	}
	~CriticalSectionLocker()
	{ 
		LeaveCriticalSection(&m_cs); 
	}
private:
	CRITICAL_SECTION &	m_cs;
};


//
// Here's where we hold the overlay bitmap and associated information
//
static volatile bool	s_isInitialized = false;	// Has the entire engine been initialized?

static CRITICAL_SECTION	s_critSection; // Used to control access to s_hOverlayBitmap, s_offsetX, and s_offsetY
static HBITMAP			s_hOverlayBitmap = NULL;
static	U32				s_offsetX = 0;
static	U32				s_offsetY = 0;



//
//
//
BOOL APIENTRY 
DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}


//
// Initialize the resources we use.
// 
int __stdcall 
OverlayEngineInitialize()
{
	// Don't call this while the engine is initialized!
	if (s_isInitialized) {
		return OVERLAY_ENGINE_FAILURE;
	}

	InitializeCriticalSection(&s_critSection);
	s_isInitialized = true;
	return OVERLAY_ENGINE_SUCCESS;
}

//
// Release the resources we use.
//
int __stdcall
OverlayEngineUninitialize()
{
	if (!s_isInitialized) {
		return OVERLAY_ENGINE_FAILURE;
	}

	{
		CriticalSectionLocker lock(s_critSection);
		s_isInitialized = false;
		if (NULL != s_hOverlayBitmap) {
			DeleteObject(&s_hOverlayBitmap);
		}
		s_hOverlayBitmap = NULL;
		s_offsetX = 0;
		s_offsetY = 0;

	}

	DeleteCriticalSection(&s_critSection);	
	return OVERLAY_ENGINE_SUCCESS;
}




//
// They're specifying a bitmap to be used.
// If one exists already, punt it.
//
// http://support.microsoft.com/kb/158898
//
int __stdcall 
OverlayEngineSetOverlayBitmap(HANDLE hCamera, char* pFileName)
{
	if (!s_isInitialized) {
		return OVERLAY_ENGINE_FAILURE;
	}

	if ((0 == hCamera) || (NULL == pFileName)) {
		return OVERLAY_ENGINE_FAILURE;
	}

	CriticalSectionLocker lock(s_critSection);

	// Returns a device independent bitmap that we can keep around for a while
	HBITMAP hBmp = (HBITMAP)::LoadImage(NULL, pFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	if (NULL == hBmp) {
		return OVERLAY_ENGINE_FAILURE;
	}

	// If there's an old one, get rid of it now that we have a valid handle on a new one.
	if (NULL != s_hOverlayBitmap) {
		BOOL rv = DeleteObject(s_hOverlayBitmap);
		ASSERT(rv);
		s_hOverlayBitmap = NULL;
	}

	s_hOverlayBitmap = hBmp;
	s_offsetX = 0;
	s_offsetY = 0;

	return OVERLAY_ENGINE_SUCCESS;
}

//
// Offset the bitmap overlay into the frame
//
int	__stdcall 
OverlayEngineSetOverlayOffset(HANDLE hCamera, int offsetX, int offsetY)
{
	if ((0 == hCamera) || (offsetX < 0) || (offsetY < 0)) {
		return OVERLAY_ENGINE_FAILURE;
	}

	CriticalSectionLocker lock(s_critSection);
	s_offsetX = offsetX;
	s_offsetY = offsetY;
	return OVERLAY_ENGINE_SUCCESS;
}

static
float 
GetPixelFormatSize(int pixelFormat)
{
   switch (pixelFormat)
   {
   case PIXEL_FORMAT_MONO8:
   case PIXEL_FORMAT_BAYER8_BGGR:
   case PIXEL_FORMAT_BAYER8_GBRG:
   case PIXEL_FORMAT_BAYER8_GRBG:
   case PIXEL_FORMAT_BAYER8_RGGB:
      return 1.0f;
   case PIXEL_FORMAT_MONO16:
   case PIXEL_FORMAT_BAYER16_BGGR:
   case PIXEL_FORMAT_BAYER16_GBRG:
   case PIXEL_FORMAT_BAYER16_GRBG:
   case PIXEL_FORMAT_BAYER16_RGGB:
   case PIXEL_FORMAT_YUV422:
      return 2.0f;
   case PIXEL_FORMAT_RGB24_DIB:
   case PIXEL_FORMAT_RGB24_NON_DIB:
   case PIXEL_FORMAT_BGR24_NON_DIB:
      return 3.0f;
   case PIXEL_FORMAT_RGBA:
   case PIXEL_FORMAT_BGRA:
   case PIXEL_FORMAT_ARGB:
   case PIXEL_FORMAT_ABGR:
      return 4.0f;
   case PIXEL_FORMAT_RGB48:
      return 6.0f;
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
      return 1.5f;
   case PIXEL_FORMAT_MONO10_PACKED_MSFIRST:
   case PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST:
   case PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST:
   case PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST:
   case PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST:
      return 1.25f;
   case PIXEL_FORMAT_STOKES4_12:
   case PIXEL_FORMAT_POLAR4_12:
   case PIXEL_FORMAT_POLAR_RAW4_12:
   case PIXEL_FORMAT_HSV4_12:
      return 6.0f;

   }
   ASSERT(FALSE);
   return -1.0f;
}

//
// Some code adapted from Capture OEM's PXLAPI_CALLBACK(BitmapOverlay)
// Assumptions here: 
//	The formatted image is going to be either MONO8 or RGB24
//  And as the documentation states, RGB24 is actually row-inverted BGR24, which is what Windows' bitmaps use.
//
// Basically, what we do is:
// Create a DIB with the dimensions of the bitmap to be overlaid
// Copy from the camera frame an area the size of the bitmap into the DIB
// Do a transparent blit of the bitmap into the DIB, using white as 'transparent'.
// Copy the data in the DIB back to the camera frame.
// 
//
static  U32 __stdcall 
Callback(HANDLE hCamera, LPVOID pData, U32 dataFormat, FRAME_DESC const * pFrameDesc, LPVOID pContext)
{
	if (!s_isInitialized) {	return 0; }

	CriticalSectionLocker lock(s_critSection);

	// This shouldn't happen, but we'll handle it just in case.
	ASSERT(NULL != s_hOverlayBitmap);
	if (NULL == s_hOverlayBitmap) { return 0; }
	

	// Get some information about the dimensions of the bitmap
	BITMAP bitmapDimensions;
	::GetObject(s_hOverlayBitmap, sizeof(bitmapDimensions), &bitmapDimensions);

	// Calculation dimensions of the frame, taking pixel addressing into account
	const U32 dec			= static_cast<U32>(pFrameDesc->Decimation.fValue);
	const U32 frameWidth	= static_cast<U32>(pFrameDesc->Roi.fWidth)	/ dec;
	const U32 frameHeight	= static_cast<U32>(pFrameDesc->Roi.fHeight) / dec;
	const float bytesPerPixel = GetPixelFormatSize(dataFormat);

	// Do some bounds checking in case the offset pushes us off the frame
	if ((s_offsetX + bitmapDimensions.bmWidth) > frameWidth) {
		return 0;
	}
	if ((s_offsetY + bitmapDimensions.bmHeight) > frameHeight) {
		return 0;
	}

	// In case the frame is smaller than the bitmap
	const U32 numRowsToModify = std::min<U32>(frameHeight,	bitmapDimensions.bmHeight);
	const U32 numColsToModify = std::min<U32>(frameWidth,	bitmapDimensions.bmWidth);

	// Pointer with a bit more type info
	U8 * const pFrameData = reinterpret_cast<U8* const>(pData);

	// RGB frames are bottom-up, but MONO frames are top-down:
	const bool isFrameInverted = (dataFormat == PIXEL_FORMAT_RGB24);

	// Create a BITMAPINFO to hold information about our bitmap
	// We need this in order to create a device independent bitmap (DIB)
	const U32 NUM_PALETTE_ENTRIES = 256;	// We're using a palette of 256 colors.
	std::vector<UCHAR> buffer(sizeof(BITMAPINFOHEADER) + NUM_PALETTE_ENTRIES * sizeof(RGBQUAD), 0);
	BITMAPINFO* pBitmapInfo					= reinterpret_cast<BITMAPINFO*>(&buffer[0]);
	pBitmapInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pBitmapInfo->bmiHeader.biWidth			= numColsToModify;
	pBitmapInfo->bmiHeader.biHeight			= numRowsToModify;
	pBitmapInfo->bmiHeader.biPlanes			= 1;
	pBitmapInfo->bmiHeader.biBitCount		= static_cast<WORD> (bytesPerPixel * 8.0);
	pBitmapInfo->bmiHeader.biCompression	= BI_RGB;	// Uncompressed

	// If it's a MONO8 image, initialize the bitmap's palette to a 
	// simple monochrome image
	if (PIXEL_FORMAT_MONO8 == dataFormat) {
		RGBQUAD* pPalette = (RGBQUAD*)((U32)pBitmapInfo + sizeof(BITMAPINFOHEADER));
		for(int i=0; i < NUM_PALETTE_ENTRIES; i++, pPalette++) {
			pPalette->rgbBlue		= i;
			pPalette->rgbGreen		= i;
			pPalette->rgbRed		= i;
			pPalette->rgbReserved	= 0;
		}
	}

	// We're going to blend / overlay the bitmap with the specific region of the frame that
	// will be affected. Create a device-independent bitmap into which we can blend.
	void* pBlenderBmpData = NULL;
	HBITMAP hBlenderBmp = ::CreateDIBSection(NULL, pBitmapInfo, DIB_RGB_COLORS, &pBlenderBmpData, NULL, 0);

	U32 pitch = ((U32((float)numColsToModify * bytesPerPixel)) + 3) / 4 * 4;

	// Copy the required portion of the frame data into the blender bitmap
	for (U32 y = 0; y < numRowsToModify; y++)
	{
		U32 dataRow = isFrameInverted ? (frameHeight - (y+s_offsetY) - 1 ) : (y + s_offsetY);
		U8* pSource = pFrameData + U32((float(dataRow * frameWidth + s_offsetX)) * bytesPerPixel); 

		U8* pDest = (U8*)pBlenderBmpData + ((numRowsToModify-y-1) * pitch);

		memcpy(pDest, pSource, U32((float)numColsToModify * bytesPerPixel));
	}

	// Create DCs for the overlay and the blender so that we can blit.
	HDC	overlayDC = ::CreateCompatibleDC(NULL);
	::SelectObject(overlayDC, s_hOverlayBitmap);
	HDC blenderDC = ::CreateCompatibleDC(NULL);
	::SelectObject(blenderDC, hBlenderBmp);

	// Now do a transparent blit of the overlay onto the blender. (white is the color to be transparent)
	::TransparentBlt(blenderDC, 0, 0, numColsToModify, numRowsToModify, overlayDC, 0, 0, numColsToModify, numRowsToModify, RGB(0xFF,0xFF,0xFF));

	// Copy the data back into the frame.
	for (U32 y = 0; y < numRowsToModify; y++)
	{
		U8* pSource = (U8*)pBlenderBmpData + ((numRowsToModify-y-1) * pitch);

		U32 dataRow = isFrameInverted ? (frameHeight - (y+s_offsetY) - 1) : (y+s_offsetY);
		U8* pDest = pFrameData + U32((float(dataRow * frameWidth + s_offsetX) * bytesPerPixel)); 

		memcpy(pDest, pSource, U32((float)numColsToModify * bytesPerPixel));
	}

	::DeleteDC(overlayDC);
	::DeleteDC(blenderDC);
	::DeleteObject(hBlenderBmp);

	return 0;
}

//
// Specify which type, and if, a callback should be used.
//
int __stdcall 
OverlayEngineEnableOverlay(const HANDLE hCamera, const int overlayType, const int enable)
{
	if (!s_isInitialized) {	return OVERLAY_ENGINE_FAILURE;	}

	// If they haven't specified a bitmap, no can do
	CriticalSectionLocker lock(s_critSection);
	if (NULL == s_hOverlayBitmap) {	return OVERLAY_ENGINE_FAILURE;	}

	PXL_RETURN_CODE rc(ApiUnknownError);
	U32 (__stdcall *pFn)(HANDLE,LPVOID,U32,FRAME_DESC const *,LPVOID) = (enable) ? Callback : NULL;

	switch(overlayType)
	{
		// Is this one of the types we support?
		case CALLBACK_PREVIEW:	
		case CALLBACK_FORMAT_IMAGE:
		case CALLBACK_FORMAT_CLIP:
			rc = PxLSetCallback(hCamera, overlayType, NULL, pFn);	
			break;
		default:
			rc = ApiInvalidParameterError;
	};

	return (API_SUCCESS(rc)) ? OVERLAY_ENGINE_SUCCESS : OVERLAY_ENGINE_FAILURE;
}



