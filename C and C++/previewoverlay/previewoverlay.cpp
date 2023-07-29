//
// previewoverlay.cpp : Defines the entry point for the application.
//
// A demonstration of how you can take a pointer to colour data and use Windows GDI functions
// to draw on top of it (overlay).
//
// This code is based on the demo app 'previewtohwnd'.
//
//

// Pull out the stdafx.h information into this file so we can keep the number of files down.
//<stdafx.h>
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
//</stafx.h>


#include "previewoverlay.h"
#define MAX_LOADSTRING 100

// PixeLINK Helper Functions
static BOOL	InitializeCamera(HWND hWnd);
static void UninitializeCamera();
static void ResizePreviewWindow(HWND hWnd);


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PREVIEWOVERLAY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_PREVIEWOVERLAY);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_PREVIEWOVERLAY);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_PREVIEWOVERLAY;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return InitializeCamera(hWnd);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			UninitializeCamera();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	case WM_SIZE:
		ResizePreviewWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
// PixeLINK camera-specific code
//
//
#include <PixeLINKAPI.h>
#include <assert.h>
#include <vector>
#define ASSERT(x)	do { assert((x)); } while(0)

static U32 __stdcall 	PreviewCallbackFunction(HANDLE hCamera, void* pFrameData, U32 pixelFormat, FRAME_DESC const * pFrameDesc, void* context);
static void				DetermineColorImageMeans(U8* pData, U32 numPixels, std::vector<double>& means);
static float			DetermineMonoImageMean(U8* pData, U32 numPixels);


static HANDLE	s_hCamera;
static HBITMAP	s_hbmpLogo;
static const U32 PREVIEW_PERCENTAGE_OF_WINDOW = 95;

//
// Initialize a camera for previewing.
//
static BOOL
InitializeCamera(HWND hWndParent)
{
	// Load the bitmap that we'll be displaying in the preview window.
	s_hbmpLogo = (HBITMAP)LoadImage(NULL, "pixelink.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);

	// Initialize a camera. Any camera.
	PXL_RETURN_CODE rc = PxLInitialize(0, &s_hCamera);
	if (!API_SUCCESS(rc)) {
		return FALSE;
	}

	// Start steaming so that we can start the preview
	rc = PxLSetStreamState(s_hCamera, START_STREAM);
	if (!API_SUCCESS(rc)) {
		PxLUninitialize(s_hCamera);
		s_hCamera = 0;
		DeleteObject(s_hbmpLogo);
		s_hbmpLogo = 0;
		return FALSE;
	}

	// Register our callback, saying that we want to be able to view and modify preview frames.
    rc = PxLSetCallback(s_hCamera, CALLBACK_PREVIEW, 0, PreviewCallbackFunction);

	// Start the preview by resizing the window based on the parent window.
	ResizePreviewWindow(hWndParent);

	return TRUE;

}

static void
UninitializeCamera()
{
	HWND temp;
	PXL_RETURN_CODE rc;

	// Free up our GDI resource(s)
	DeleteObject(s_hbmpLogo);

	// Shut everything down
	rc = PxLSetPreviewState(s_hCamera, STOP_PREVIEW, &temp);
	rc = PxLSetStreamState(s_hCamera, STOP_STREAM);
	rc = PxLUninitialize(s_hCamera);
}


//
// If the window is resized, we'll just 
// 1) stop previewing
// 2) set the new preview settings
// 3) resume previewing
//
static void
ResizePreviewWindow(HWND hWndParent)
{
	int left   = 100;
	int top    = 100;
	int width  = 256;
	int height = 256;
	PXL_RETURN_CODE rc;
	RECT rectParent;
	HWND tmp;

	// Is the camera initialized?
	// If not, we've been called too early in the app initialization process.
	if (0 == s_hCamera) {
		return;
	}

	GetClientRect(hWndParent, &rectParent);

	// Take up 75% of the window, and centre the preview image
	width  = (int)((float)rectParent.right  * PREVIEW_PERCENTAGE_OF_WINDOW / 100.0f);
	height = (int)((float)rectParent.bottom * PREVIEW_PERCENTAGE_OF_WINDOW / 100.0f);

	left = (rectParent.right - width) / 2;
	top  = (rectParent.bottom - height) / 2;

	// We have to stop previewing otherwise we'll just
	// end up changing the height and width, but 
	// not the top and the left.

	rc = PxLSetPreviewState(s_hCamera, STOP_PREVIEW, &tmp);
	ASSERT(API_SUCCESS(rc));

	rc = PxLSetPreviewSettings(
		s_hCamera, 
		"", 
		WS_CHILD | WS_VISIBLE, 
		left, top, width, height,
		hWndParent, 
		0);
	ASSERT(API_SUCCESS(rc));

	rc = PxLSetPreviewState(s_hCamera, START_PREVIEW, &tmp);
	ASSERT(API_SUCCESS(rc));

}

//
// Callback function with the preview data to be manipulated.
//
// Basically what we'll do is:
// 1) Create a device context (hdc)
// 2) Create a HBITMAP using the data in the preview buffer (pFrameData)
// 3) Select the HBITMAP into the device context
// 4) Use Win32 GDI functions to draw into the DC (and therefore over the bitmap)
// 5) Copy the raw bitmap data into the preview buffer
//
// For mono cameras, the pixel format will be MONO8 (regardless of whether the camera 
// is set up for MONO8 or MONO16), so when we create the HBITMAP, we'll set up
// a simple MONO palette of 256 values. 
//

static U32 __stdcall 
PreviewCallbackFunction(const HANDLE hCamera, void* const pFrameData, const U32 pixelFormat, FRAME_DESC const * const pFrameDesc, void * const context)
{
	HDC hdc = CreateCompatibleDC(NULL);

	// Determine the camera image dimensions from the frame descriptor.
	// Don't forget to take decimation (pixel addressing value) into account.
	const int width			= (int)(pFrameDesc->Roi.fWidth	/	pFrameDesc->Decimation.fValue);
	const int height		= (int)(pFrameDesc->Roi.fHeight	/	pFrameDesc->Decimation.fValue);
	const bool isMonoImage	= (PIXEL_FORMAT_MONO8 == pixelFormat);
	const int pixelSize		= (isMonoImage) ? 1 : 3; // 1 byte for mono8; 3 bytes for an RGB pixel

	// If it's a mono image, we have to create a 256-entry palette
	const U32 bufferSize = sizeof(BITMAPINFOHEADER) + ((isMonoImage) ? (256 * sizeof(RGBQUAD)) : 0);

	std::vector<U8> buffer(bufferSize,0);
	BITMAPINFO* pBitmapInfo = reinterpret_cast<BITMAPINFO*>(&buffer[0]);
	pBitmapInfo->bmiHeader.biSize			= sizeof(pBitmapInfo->bmiHeader);
	pBitmapInfo->bmiHeader.biPlanes			= 1;		// Always just one plane.
	pBitmapInfo->bmiHeader.biCompression	= BI_RGB;	// We're using uncompressed data.
	pBitmapInfo->bmiHeader.biSizeImage		= 0;		// Set to 0 for BI_RGB images.
	pBitmapInfo->bmiHeader.biClrImportant	= 0;		// Want to use all colours.
	pBitmapInfo->bmiHeader.biClrUsed		= 0;		// Color table not used.
	pBitmapInfo->bmiHeader.biWidth			= width;
	if (PIXEL_FORMAT_MONO8 == pixelFormat) {
		pBitmapInfo->bmiHeader.biBitCount	= 8;		// 8 bits of data per pixel
		pBitmapInfo->bmiHeader.biHeight		= -height;	// top-down
		// Initialize a simple mono palette
		RGBQUAD* pPalette = (RGBQUAD*)((U32)pBitmapInfo + sizeof(BITMAPINFOHEADER));
		for(int i=0; i < 256; i++, pPalette++) {
			pPalette->rgbBlue		= i;
			pPalette->rgbGreen		= i;
			pPalette->rgbRed		= i;
			pPalette->rgbReserved	= 0;
		}
	} else {
		pBitmapInfo->bmiHeader.biBitCount	= 24;		// 24 bits of data per pixel (RGB24)
		pBitmapInfo->bmiHeader.biHeight		= height;	// inverted
	}

	// Create the DIB section.
	// pData will point to the memory allocated for our image data
	BYTE* pData = NULL;
	HBITMAP hBmp = ::CreateDIBSection(
		hdc,					//
		pBitmapInfo,			//
		DIB_RGB_COLORS,			//
		(void**)&pData,			//	
		NULL,					//	Not using file mapping.
		0);						//

	// Copy the preview data into the DIB section
	const U32 imageSize = width * height * pixelSize;
	memcpy(pData, pFrameData, imageSize);

	// Select our preview bitmap into the DC, and save the current (n
	HBITMAP previousBitmap = (HBITMAP)SelectObject(hdc, hBmp);

	SetROP2(hdc, R2_NOT);

	// Add a bit of text.
	{
		char message[1024];
		const U32 lineYOffset = 14;
		SetBkMode(hdc, TRANSPARENT);

		if (isMonoImage) {
			const float mean = DetermineMonoImageMean((U8*)pFrameData, width * height);

			sprintf(message, "R: %5.2f", mean);
			SetTextColor(hdc, RGB(255-mean, 255-mean, 255-mean));
			TextOut(hdc, 0, lineYOffset * 0, message, (int)strlen(message));

		} else {
			std::vector<double> means(3,0);
			DetermineColorImageMeans((U8*)pFrameData, width * height, means);

			sprintf(message, "R: %5.2f", means[2]);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, 0, lineYOffset * 1, message, (int)strlen(message));

			sprintf(message, "G: %5.2f", means[1]);
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, 0, lineYOffset * 2, message, (int)strlen(message));

			sprintf(message, "B: %5.2f", means[0]);
			SetTextColor(hdc, RGB(0, 0, 255));
			TextOut(hdc, 0, lineYOffset * 3, message, (int)strlen(message));

			SetTextColor(hdc, RGB(255 - (int)means[2],255 - (int)means[1],255 - (int)means[0]));
			sprintf(message, "Frame Number: %d", pFrameDesc->uFrameNumber);
			TextOut(hdc, 0, lineYOffset * 0, message, strlen(message));
		}
	}

	// Draw a few lines
	HPEN hpenDot = CreatePen(PS_DOT, 1, RGB(255,255,255));
	SelectObject(hdc, hpenDot);

	MoveToEx(hdc, width / 2,	0,			NULL);
	LineTo	(hdc, width / 2, height);
	MoveToEx(hdc, 0,	height / 2, NULL);
	LineTo	(hdc, width, height / 2);

	int centreX = width / 2;
	int centreY = height / 2;
	int radius = 50;

	// A circle in the centre
	Arc(hdc, 
		centreX - radius,	centreY - radius,		// upper left
		centreX + radius,	centreY + radius,		// lower right
		centreX - radius,	centreY,				// first radial ending point
		centreX - radius,	centreY);

	// Blit the logo bmp into the DC
	{
		HDC hdcLogo = CreateCompatibleDC(hdc);

		// Get the dimensions of the logo
		HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcLogo, s_hbmpLogo);
		BITMAP bmLogo;
		GetObject(s_hbmpLogo, sizeof(bmLogo), &bmLogo);
		BitBlt(hdc, 
			width - bmLogo.bmWidth, height - bmLogo.bmHeight,	// put into bottom right corner
			bmLogo.bmWidth,	bmLogo.bmHeight,
			hdcLogo, 0, 0, SRCCOPY);

		SelectObject(hdcLogo, hbmpOld);
		DeleteDC(hdcLogo);
	}
	
	// Restore the original bmp to the DC
	SelectObject(hdc, previousBitmap);

	// Copy the bitmap from the DC to the preview callback frame data
	GetDIBits(hdc, hBmp, 0, height, pFrameData, pBitmapInfo, DIB_RGB_COLORS);

	// Clean up our GDI resources
	DeleteObject(hpenDot);
	DeleteObject(hBmp);
	DeleteDC(hdc);
	
	return 0;
}


//
// Determine simple image means for each channel.
//
// NOTE: Very slow in debug so we'll try to optimize this by:
// 1) enabling optimization
// 2) using a bit of loop unrolling with Duff's device.
//
// No PixeLINK camera is 3000x3000, so we'll never see
// a max total of 3000*3000*255 = 0x88CA_EBC0, so we can 
// use a U32 for the totals without having to worry about rolling.
//
#define	UPDATE_TOTALS()	{\
	blueTotal	+= *pPixelByte++;	\
	greenTotal	+= *pPixelByte++;	\
	redTotal	+= *pPixelByte++;	\
}

#pragma optimize( "atp", on )
static void
DetermineColorImageMeans(U8* pData, U32 numPixels, std::vector<double>& means)
{
	register U32 redTotal	= 0;
	register U32 greenTotal	= 0;
	register U32 blueTotal	= 0;
	register U8* pPixelByte = pData;

#if SIMPLE_METHOD
	register U8* pEnd = &pPixelByte[numPixels * 3];
	for(; pPixelByte != pEnd ; ) {
		blueTotal	+= *pPixelByte++;
		greenTotal	+= *pPixelByte++;
		redTotal	+= *pPixelByte++;
	}
#else
	// Duff's Device
	register int n = (numPixels+7) / 8;
	switch(numPixels%8) {
		case 0:	do	{		UPDATE_TOTALS();
		case 7:				UPDATE_TOTALS();
		case 6:				UPDATE_TOTALS();
		case 5:				UPDATE_TOTALS();
		case 4:				UPDATE_TOTALS();
		case 3:				UPDATE_TOTALS();
		case 2:				UPDATE_TOTALS();
		case 1:				UPDATE_TOTALS();
				} while (--n > 0);
	}
#endif

	means[0] = (double)blueTotal	/ (double)numPixels;
	means[1] = (double)greenTotal	/ (double)numPixels;
	means[2] = (double)redTotal		/ (double)numPixels;
}

static float 
DetermineMonoImageMean(U8* pData, U32 numPixels)
{
	U32 total = 0;
	for(U32 i=0; i < numPixels; i++, pData) {
		total += *pData++;
	}
	return (float)total / (float)numPixels;
}


#pragma optimize( "", on ) // reset to defaults
