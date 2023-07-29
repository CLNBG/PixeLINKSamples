//
// This demonstrates how to control a camera's autofocus features.
//
// Option 1) Enable autofocus (FEATURE_FLAG_AUTO)
// When autofocus is enabled, the camera controls the autofocus based on its own internal algorithm.
// The autofocus will be continually adjusted over time by the camera until autofocus is disabled.
//
// Option 2) AutoFocus Once. (FEATURE_FLAG_ONEPUSH)
// When initiated, the camera will adjust the autofocus based on its own internal algorithm. Once
// a satisfactory autofocus has been determined, the camera will release control of the autofocus whereupon
// the autofocus is now again settable via the SDK. 
//
// With AutoFocus once, this application demonstrates how you can either:
// A) Initiate AutoFocus once, then start polling the camera, looping until the operation is complete.
// B) Initiate AutoFocus once, and then poll the camera at a regular interval with a timer. 
//    If using this option in this application, the camera menu items are disabled while the 
//    camera is still determining a suitable autofocus.
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0502

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <afxwin.h>

#include "autofocus.h"
#define MAX_LOADSTRING 100

#define IDC_STATIC_VAL -1
//#define ASSERT(x)	do { assert((x)); } while(0)

//
// Some camera control functions
//
static BOOL InitializeCamera();
static BOOL StartPreview(HWND hWndParent);
static void StopPreview();
static void	ResizePreviewWindow(HWND hWndParent);
static void UninitializeCamera();
static void AutoFocusOnce(HWND hWnd);
static void	UpdateCameraMenu(HWND hWnd);
static void StartCounter();
static double GetCounter();



// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
double PCFreq;
__int64 CounterStart;


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
	LoadString(hInstance, IDC_AUTOFOCUS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_AUTOFOCUS);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_AUTOFOCUS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_AUTOFOCUS;
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

   if (!InitializeCamera()) {
	   return FALSE;
   }

   if (!StartPreview(hWnd)) {
	   UninitializeCamera();
	   return FALSE;
   }

   UpdateCameraMenu(hWnd);

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
		case ID_CAMERA_AUTOFOCUSONCE:
			AutoFocusOnce(hWnd);
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
		StopPreview();
		UninitializeCamera();
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

// The camera we're interacting with in this application.
static HANDLE s_hCamera;

// How much of the main window does the preview take up?
static const U32 PREVIEW_PERCENTAGE_OF_WINDOW = 75;


static BOOL
InitializeCamera()
{
	// Initialize a camera, any camera.
	PXL_RETURN_CODE rc = PxLInitialize(0, &s_hCamera);
	ASSERT(API_SUCCESS(rc));
	return API_SUCCESS(rc);
}

static void
UninitializeCamera()
{
	PXL_RETURN_CODE rc = PxLUninitialize(s_hCamera);
	ASSERT(API_SUCCESS(rc));
	s_hCamera = 0;
}


static BOOL 
StartPreview(HWND hWndParent)
{
	PXL_RETURN_CODE rc = PxLSetStreamState(s_hCamera, START_STREAM);
	if (!API_SUCCESS(rc)) {
		return FALSE;
	}

	ResizePreviewWindow(hWndParent);

	return TRUE;
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

	// Is the camera initialized yet?
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

static BOOL
IsAutoFocusOnceInProgress(HANDLE hCamera)
{
	PXL_RETURN_CODE rc;
	U32 numParams = 1;
	U32 flags;
	float params[1];
	
	// Read the current flags and parameter setting
	rc = PxLGetFeature(hCamera, FEATURE_FOCUS, &flags, &numParams, &params[0]);
	ASSERT(API_SUCCESS(rc));

	return ((flags & FEATURE_FLAG_ONEPUSH) == 0) ? FALSE : TRUE;
}

static void
UpdateCameraMenu(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	
	// Is an auto autofocus in progress?
	if (IsAutoFocusOnceInProgress(s_hCamera)) {
		EnableMenuItem	(hMenu, ID_CAMERA_AUTOFOCUSONCE,	MF_GRAYED);
	} else {
		EnableMenuItem	(hMenu, ID_CAMERA_AUTOFOCUSONCE,	MF_ENABLED);
	}
}


static BOOL
StartAutoFocusOnce(HANDLE hCamera)
{
	PXL_RETURN_CODE rc;
	CString msg;
	U32 numParams = 1;
	U32 flags;
	float params;
	double elapsed_time_seconds = 0;
	
	// Read the current flags and parameter setting
	rc = PxLGetFeature(hCamera, FEATURE_FOCUS, &flags, &numParams, &params);
	if (!API_SUCCESS(rc)) {
		return rc;
	}
	ASSERT(numParams == 1);

	// Start Timing
	StartCounter();

	// If we're turning it ON and it's already ON, then exit
	flags = FEATURE_FLAG_ONEPUSH;
	rc = PxLSetFeature(hCamera, FEATURE_FOCUS, flags, numParams, &params);
	if (!API_SUCCESS(rc)) {
		return FALSE;
	}

	// At this point you could loop until the auto autofocus is done.
#define WAIT_UNTIL_AUTOFOCUS_IS_DONE
#ifdef WAIT_UNTIL_AUTOFOCUS_IS_DONE

	while(1) {
		Sleep(50);
		if (!IsAutoFocusOnceInProgress(s_hCamera)) {
			break;
		}
	}
#endif

	elapsed_time_seconds = GetCounter()/1000.0;
	msg.Format("AF Complete - %lf Seconds", elapsed_time_seconds);
	AfxMessageBox(msg);

	return TRUE;

}

//
// Timer used to poll the camera until the 'AutoFocus once' is done.
// When we see that the camera is done 'AutoFocus once', the timer is killed.
//
static const int TIMER_ID = 0x71077345;	// SHELL OIL

static VOID CALLBACK 
TimerProc( HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	UpdateCameraMenu(hWnd);
	if (!IsAutoFocusOnceInProgress(s_hCamera)) {
		KillTimer(hWnd, TIMER_ID);
	}
}


static void
AutoFocusOnce(HWND hWnd)
{
	StartAutoFocusOnce(s_hCamera);
	UpdateCameraMenu(hWnd);
	// Enable a timer to poll the camera every 100 ms
	SetTimer(hWnd, TIMER_ID, 50, TimerProc);
}


// Starts high frequency ms counter
void StartCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
	
    PCFreq = li.QuadPart/1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

// Gets elapsed time in ms
double GetCounter()
{
    LARGE_INTEGER li;
	double elapsed_time = 0;
    QueryPerformanceCounter(&li);
	elapsed_time = (li.QuadPart-CounterStart)/PCFreq;
    return elapsed_time;
}