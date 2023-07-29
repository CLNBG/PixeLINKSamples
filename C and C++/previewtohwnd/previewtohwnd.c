// previewtohwnd.cpp : Defines the entry point for the application.
//

#define WIN32_LEAN_AND_MEAN
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

#define ASSERT(x)	do { assert((x)); } while(0)

#include "previewtohwnd.h"
#define MAX_LOADSTRING 100

//
// Camera Control functions
//
static void  StartPreview(HWND hWndParent);
static void  StopPreview();
static void	 ResizePreviewWindow(HWND hWndParent);




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
	LoadString(hInstance, IDC_PREVIEWTOHWND, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_PREVIEWTOHWND);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_PREVIEWTOHWND);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_PREVIEWTOHWND;
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
			DestroyWindow(hWnd);
			break;
		case ID_FILE_STARTPREVIEW:
			StartPreview(hWnd);
			break;
		case ID_FILE_STOPPREVIEW:
			StopPreview();
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



/////////////////////////////////////////////////////////////////////////////////
//
//  PixeLINK camera-specific code
//
/////////////////////////////////////////////////////////////////////////////////

#include <PixeLINKApi.h>

static HANDLE s_hCamera;

static const U32 PREVIEW_PERCENTAGE_OF_WINDOW = 75;

static void 
StartPreview(HWND hWndParent)
{
	PXL_RETURN_CODE rc;

	// Already previewing?
	if (0 != s_hCamera) {
		return;
	}

	rc = PxLInitialize(0, &s_hCamera);
	if (!API_SUCCESS(rc)) {
		assert(0);
	}

	// Load factory settings so the camera's in a known state
	rc = PxLLoadSettings(s_hCamera, 0);
	ASSERT(API_SUCCESS(rc));

	rc = PxLSetStreamState(s_hCamera, START_STREAM);
	ASSERT(API_SUCCESS(rc));

	ResizePreviewWindow(hWndParent);

}

static void 
StopPreview()
{
	HWND tmp = 0;
	PXL_RETURN_CODE rc;

	// If not previewing, nothing to do
	if (0 == s_hCamera) {
		return;
	}

	rc = PxLSetPreviewState(s_hCamera, STOP_PREVIEW, &tmp);
	ASSERT(API_SUCCESS(rc));

	rc = PxLSetStreamState(s_hCamera, STOP_STREAM);
	ASSERT(API_SUCCESS(rc));

	rc = PxLUninitialize(s_hCamera);
	ASSERT(API_SUCCESS(rc));

	s_hCamera = 0;
}



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
		left,top,width,height,
		hWndParent, 
		0);
	ASSERT(API_SUCCESS(rc));

	rc = PxLSetPreviewState(s_hCamera, START_PREVIEW, &tmp);
	ASSERT(API_SUCCESS(rc));

}