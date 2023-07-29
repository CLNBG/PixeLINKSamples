//
// preview.c
//
// Demonstrates how to create a simple preview window using the PixeLINK API.
// Note that the window will be a completely new window. 
// To see how to create a preview window in an existing window, see the
// demo app "previewtohwnd".
//
// See InitializeCamera for preview window style options.
//
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>

#include "preview.h"
#define MAX_LOADSTRING 100

// PixeLINK Helper Functions
static BOOL	InitializeCamera();
static void UninitializeCamera();



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
	LoadString(hInstance, IDC_PREVIEW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_PREVIEW);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_PREVIEW);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_PREVIEW;
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

   return InitializeCamera();
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
#define SET_TITLE

static HANDLE s_hCamera;

static BOOL
InitializeCamera()
{
	HWND hPreviewWnd1;


	PXL_RETURN_CODE rc = PxLInitialize(0, &s_hCamera);
	if (!API_SUCCESS(rc)) {
		return FALSE;
	}

#ifdef USE_PREVIEW_DEFAULTS
	// Option 1
	// Just call PxLSetPreviewState and use the defaults
#endif

	// Option 2
	// Set the title and let the driver do the rest.
#ifdef SET_TITLE
	rc = PxLSetPreviewSettings(
		s_hCamera, 
		"Preview Demo",						// Title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,	// Window Style
		CW_USEDEFAULT,						// Left
		CW_USEDEFAULT,						// Top 
		CW_USEDEFAULT,						// Width
		CW_USEDEFAULT,						// Height
		NULL,								// Parent HWND
		0);									// Child Resource Id
#endif

#ifdef SET_TITLE_AND_WINDOW_SIZE_AND_POSITION
	// Option 3
	// Set the title, window size, and position on the screen.
	rc = PxLSetPreviewSettings(
		s_hCamera, 
		"Preview Demo",						// Title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,	// Window Style
		0,									// Left
		0,									// Top 
		250,								// Width
		400,								// Height
		NULL,								// Parent HWND
		0);									// Child Resource Id
#endif

	// The following settings demonstrate some of the other options
	// possible by playing with the window style. 

#ifdef USE_THICK_FRAME
	rc = PxLSetPreviewSettings(
		s_hCamera, 
		"USE_THICK_FRAME",						// Title
		WS_THICKFRAME | WS_VISIBLE,			// Window Style
		0,									// Left
		0,									// Top 
		400,								// Width
		400,								// Height
		NULL,								// Parent HWND
		0);									// Child Resource Id
#endif


#ifdef NO_MAXIMIZE_BOX
	rc = PxLSetPreviewSettings(
		s_hCamera, 
		"NO_MAXIMIZE_BOX",						// Title
		(WS_OVERLAPPEDWINDOW & (~ WS_MAXIMIZEBOX)) | WS_VISIBLE,			// Window Style
		0,									// Left
		0,									// Top 
		400,								// Width
		400,								// Height
		NULL,								// Parent HWND
		0);									// Child Resource Id
#endif

#ifdef UNRESIZEABLE_WITH_MINIMIZE
	rc = PxLSetPreviewSettings(
		s_hCamera, 
		"UNRESIZEABLE_WITH_MINIMIZE",						// Title
		WS_DLGFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,				// Window Style
		0,									// Left
		0,									// Top 
		400,								// Width
		400,								// Height
		NULL,								// Parent HWND
		0);									// Child Resource Id
#endif

	// Start steaming so that we can start the preview
	rc = PxLSetStreamState(s_hCamera, START_STREAM);
	rc = PxLSetPreviewState(s_hCamera, START_PREVIEW, &hPreviewWnd1);

	return TRUE;

}

static void
UninitializeCamera()
{
	HWND temp;
	PXL_RETURN_CODE rc;

	// Shut everything down
	rc = PxLSetPreviewState(s_hCamera, STOP_PREVIEW, &temp);
	rc = PxLSetStreamState(s_hCamera, STOP_STREAM);
	rc = PxLUninitialize(s_hCamera);
}

