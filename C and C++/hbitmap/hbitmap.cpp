//
// hbitmap.cpp : Defines the entry point for the application.
//
// Demonstrates how to capture an image from a PixeLINK camera and
// use it to generate an HBITMAP. 
//
// Note that despite the references to RGB24, this will work with 
// a monochrome camera. Converting a mono image to RAW_RGB24 with PxLFormatImage 
// gives you an RAW_RGB24 image where the R, G, and B values for each 
// pixel are the same. 
//
// Note that this assumes the camera is not triggered and the exposure time is
// rather short. A more complete and robust implementation of a preview window
// would have a thread dedicated to capturing and saving the most recent image
// so that it can be redrawn as required (which is what happens with the 
// preview functionality in the PixeLINK API.)
//
//

// Took this out of stdafx.h so that we don't have to use it or use precompiled headers.
// <stdafx.h>
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
// </stdafx.h>

#include "hbitmap.h"
#include <PixeLINKApi.h>
#include <vector>
#define MAX_LOADSTRING 100

// Just to keep things simple, use a buffer size large enough for a 24 MegaPixel camera, 16-bit format.
#define MAX_IMAGE_SIZE (5000*5000*2)


// Handle to the camera we'll be using.
static HANDLE s_hCamera = 0;
// Timer we use to invalidate the window and therefore update the image
static const int TIMER_ID = 0x71077345;	// SHELL OIL

// Do you want the image scaled to fix the client rect, or not?
//#define DO_NOT_SCALE_IMAGE



// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// Pixelink local helpers
static BOOL			InitializeCamera();
static void			UninitializeCamera();
static void			CaptureAndDrawImage(HWND hWnd, HDC hdc);
static void			DetermineImageMeans(U8* pPixelByte, U32 numPixels, std::vector<double>& means);
static VOID CALLBACK	TimerProc( HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);





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
	LoadString(hInstance, IDC_HBITMAP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_HBITMAP);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_HBITMAP);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_HBITMAP;
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

	if (!InitializeCamera()) {
		MessageBox(NULL, "Unable to initialize a camera", "Bitmap", MB_ICONEXCLAMATION);
		return FALSE;
	}

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Update the image every 50 ms.
	SetTimer(hWnd, TIMER_ID, 50, TimerProc);

	return TRUE;
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
			UninitializeCamera();
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

#ifndef DO_NOT_SCALE_IMAGE 
	// If we're scaling the image to fit the entire client rect, we
	// don't want to erase the background because we're going to 
	// paint over everything. Returning true here means
	// the default behaviour (paint over with white) doesn't happen,
	// and we therefore reduce flickering.
	//
	// If we are NOT scaling the image, we need the default behavour.
	case WM_ERASEBKGND:
		return TRUE;
#endif

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		CaptureAndDrawImage(hWnd, hdc);
		EndPaint(hWnd, &ps);
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

//
// Initialize a camera - any PixeLINK camera that happens to be connected.
//
static BOOL
InitializeCamera()
{
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		return FALSE;
	}

	if (!API_SUCCESS(PxLSetStreamState(hCamera, START_STREAM))) {
		PxLUninitialize(hCamera);
		return FALSE;
	}

	s_hCamera = hCamera;

	return TRUE;
}

static void
UninitializeCamera()
{
	if (s_hCamera != 0) {
		PxLUninitialize(s_hCamera);
		s_hCamera = 0;
	}
}

//
// Capture and Draw Image
//
// 
static void 
CaptureAndDrawImage(HWND hWnd, HDC hdc)
{
	// Create a buffer big enough to hold a frame from any PixeLINK camera.
	// This is done to keep this demo simple.
	// See the function DetermineRawImageSize in the demo app 'getsnapshot' to see 
	// how to determine the actual size you need.
	static std::vector<U8>	s_rawBuffer(MAX_IMAGE_SIZE);

	// Capture an image.
	// See the function GetNextFrame in the demo app 'getnextframe' for a more robust method.
	FRAME_DESC	frameDesc;
	frameDesc.uSize = sizeof(frameDesc);
	PXL_RETURN_CODE rc = PxLGetNextFrame(s_hCamera, (U32)s_rawBuffer.size(), &s_rawBuffer[0], &frameDesc);
	if (!API_SUCCESS(rc)) {
		return;
	}

	// We can now determine the camera image dimensions from the frame descriptor.
	// Don't forget to take decimation (pixel addressing value) into account.
	int width	= (int)(frameDesc.Roi.fWidth	/	frameDesc.Decimation.fValue);
	int height	= (int)(frameDesc.Roi.fHeight	/	frameDesc.Decimation.fValue);

	//
	// Create the HBITMAP 
	//

	// Mock up a BITMAPINFO struct
	// (See MSDN for more info on this struct)
	//
	// If the camera config never changes, you could initialize the BITMAPINFO just 
	// once and then re-use it over and over.
	//
	BITMAPINFO bitmapInfo;
	memset(&bitmapInfo.bmiHeader, 0,	  sizeof(bitmapInfo.bmiHeader));

	bitmapInfo.bmiHeader.biSize			= sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biPlanes		= 1;		// Always just one plane.
	bitmapInfo.bmiHeader.biCompression	= BI_RGB;	// VERY IMPORTANT. We're using uncompressed RGB data.
	bitmapInfo.bmiHeader.biSizeImage	= 0;		// Set to 0 for BI_RGB images.
	bitmapInfo.bmiHeader.biClrImportant = 0;		// Want to use all colours.
	bitmapInfo.bmiHeader.biClrUsed		= 0;		// Color table not used.
	bitmapInfo.bmiHeader.biBitCount		= 24;		// 24 bit of data per pixel (RGB24)
	bitmapInfo.bmiHeader.biWidth		= width;
	bitmapInfo.bmiHeader.biHeight		= height;

	// Create the DIB section.
	// pRGB24Data will point to the memory allocated for our RGB24 data
	BYTE* pRGB24Data = NULL;
	HBITMAP hBmp = ::CreateDIBSection(
		hdc,					//
		&bitmapInfo,			//
		DIB_RGB_COLORS,			//	VERY IMPORTANT. Not using a palette. The bitmap will have RGB info.	 
		(void**)&pRGB24Data,	//	
		NULL,					//	Not using file mapping.
		0);						//

	
	// We can determine the size easily enough because there's no compression with IMAGE_FORMAT_RAW_RGB24.
	U32 rgb24ImageSize = width * height * 3;	// 3 bytes per RGB24 pixel.

	// We can ask PxLFormatImage to convert directly from the raw image into the data in the HBITMAP.
	// As luck would have it, IMAGE_FORMAT_RAW_RGB24 is exactly what HBITMAP needs.
	// i.e. 
	// - each pixel has byte values representing B, G, R 
	// - image rows are inverted 
	// 
	PxLFormatImage(&s_rawBuffer[0], &frameDesc, IMAGE_FORMAT_RAW_RGB24, pRGB24Data, &rgb24ImageSize);

	std::vector<double> means(3, 0.0);
	DetermineImageMeans(pRGB24Data, width * height, means);

	//
	// And now draw the HBITMAP with the image into the window
	//
	HDC hdcMem = CreateCompatibleDC(hdc);	// Or NULL, since we're dealing with the video display

	// Select the HBITMAP with the camera image as the current object for the hdcMem.
	SelectObject(hdcMem, hBmp);

	// Get the BITMAP so we know the dimensions of the HBITMAP with the 
	BITMAP bm;
	GetObject(hBmp, sizeof(bm), &bm);

	// And finally, blit the image (now in the bitmap) into the hdc of the window
#ifdef DO_NOT_SCALE_IMAGE
	BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
#else
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchBlt(
		hdc, clientRect.left, clientRect.top, clientRect.right - clientRect.left , clientRect.bottom - clientRect.top, 
		hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, 
		SRCCOPY);
#endif

	// Some cleanup
	DeleteDC(hdcMem);
	DeleteObject(hBmp);

	//

	//
	// Display the mean(s) while we're at it.
	//

	// Make the background transparent
	int oldBkMode = SetBkMode(hdc, TRANSPARENT);

#ifdef CREATE_A_SPECIFIC_FONT
	// Set up a simple Courier New font
	HFONT font = CreateFont(
		0,			// width
		0,			// height
		0,			// escapement	
		0,			// orientation
		FW_NORMAL,	// weight
		FALSE,		// italic
		FALSE,		// underline
		FALSE,		// strikeout
		ANSI_CHARSET,				// char set identifier
		OUT_DEFAULT_PRECIS,			// output precision
		CLIP_DEFAULT_PRECIS,		// clip precision
		DEFAULT_QUALITY,			// quality
		DEFAULT_PITCH | FF_ROMAN,	// pitch and family
		"Courier New");
#else
	// Use a stock font
	HFONT font = (HFONT)GetStockObject(ANSI_FIXED_FONT);
#endif
	SelectObject(hdc, font);

	char messageBuf[1024];

	// Is this a mono or colour camera?
	U32 pixelFormat = (int)frameDesc.PixelFormat.fValue;
	if ((PIXEL_FORMAT_MONO8 == pixelFormat) || (PIXEL_FORMAT_MONO16 == pixelFormat)) {
		// All the means will be the same because the conversion to RGB just copied the mono value to R, G, and B
		sprintf(messageBuf, "Mean: %6.2lf", means[2]);
		int mean = (int)means[2];
		if (mean >= 128) {
			SetTextColor(hdc, RGB(0x00, 0x00, 0xFF));
		} else {
			SetTextColor(hdc, RGB(0x00, 0xFF, 0x00));
		}
		TextOut(hdc, 0,  0, messageBuf, strlen(messageBuf));
	} else {
		// Print out the three means using the appropriate colour
		sprintf(messageBuf, "R: %6.2lf", means[2]);
		SetTextColor(hdc, RGB(0xFF, 0x00, 0x00));
		TextOut(hdc, 0,  0, messageBuf, strlen(messageBuf));

		sprintf(messageBuf, "G: %6.2lf", means[1]);
		SetTextColor(hdc, RGB(0x00, 0xFF, 0x00));
		TextOut(hdc, 0, 20, messageBuf, strlen(messageBuf));

		sprintf(messageBuf, "B: %6.2lf", means[0]);
		SetTextColor(hdc, RGB(0x00, 0x00, 0xFF));
		TextOut(hdc, 0, 40, messageBuf, strlen(messageBuf));
	}
	
	// Restore the background mix mode
	SetBkMode(hdc, oldBkMode);

	DeleteObject(font);

}

//
// Timer used to poll the camera until the 'autoexpose once' is done.
// When we see that the camera is done 'autoexpose once', the timer is killed.
//

static VOID CALLBACK 
TimerProc( HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (s_hCamera != 0) {
		// Invalidate, but don't erase.
		// Erasing causes flickering.
		BOOL eraseBackground = FALSE;
		InvalidateRect(hWnd, NULL, eraseBackground);
	}
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
DetermineImageMeans(U8* pData, U32 numPixels, std::vector<double>& means)
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
#pragma optimize( "", on ) // reset to defaults
