//
// Assuming the camera supports hardware triggering, this puts the camera into hardware trigging mode and then
// captures images to file. 
//
// The image are called imageX.E, where 
// X is an incrementing number starting at 0
// E is an extension appropriate for the image file format chosen.
//
// We use a worker thread to capture images so that the main (GUI) thread is 
// free to do GUI stuff.
//
// Some of the code here is taken directly from the GetSnapshot demo app.
//
// HardwareTriggerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hardwaretrigger.h"
#include "HardwareTriggerDlg.h"
#include ".\hardwaretriggerdlg.h"
#include <PixeLINKApi.h>
#include <vector>
#include "WorkerThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// Defining the ability to save as raw
#define	CUSTOM_IMAGE_FORMAT_RAW	1000

// CHardwareTriggerDlg dialog



CHardwareTriggerDlg::CHardwareTriggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHardwareTriggerDlg::IDD, pParent),
	m_hCamera(0),
	m_pWorkerThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CHardwareTriggerDlg::~CHardwareTriggerDlg()
{
	UninitializeCamera();
}


void CHardwareTriggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_txtMessage);
	DDX_Control(pDX, IDC_BUTTON_CAPTURE, m_btnCapture);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_COMBO1, m_cmbImageFormat);
}

BEGIN_MESSAGE_MAP(CHardwareTriggerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_WORKER_THREAD_UPDATE,		OnWorkerThreadUpdate)
	ON_MESSAGE(WM_WORKER_THREAD_FINISHED,	OnWorkerThreadFinished)

	ON_BN_CLICKED(IDC_BUTTON_CAPTURE, OnBnClickedCapture)
END_MESSAGE_MAP()


// CHardwareTriggerDlg message handlers

BOOL CHardwareTriggerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize a camera
	if (!InitializeCamera()) {
		PostQuitMessage(1);
	}

	int index = m_cmbImageFormat.AddString("Bitmap (.bmp)");
	m_cmbImageFormat.SetItemData(index, IMAGE_FORMAT_BMP);
	index = m_cmbImageFormat.AddString("TIFF (.tiff)");
	m_cmbImageFormat.SetItemData(index, IMAGE_FORMAT_TIFF);
	index = m_cmbImageFormat.AddString("PSD (.psd)");
	m_cmbImageFormat.SetItemData(index, IMAGE_FORMAT_PSD);
	index = m_cmbImageFormat.AddString("JPEG (.jpg)");
	m_cmbImageFormat.SetItemData(index, IMAGE_FORMAT_JPEG);
	index = m_cmbImageFormat.AddString("RGB24 (.bin)");
	m_cmbImageFormat.SetItemData(index, IMAGE_FORMAT_RAW_RGB24);
	index = m_cmbImageFormat.AddString("Raw (.bin)");
	m_cmbImageFormat.SetItemData(index, CUSTOM_IMAGE_FORMAT_RAW);

	m_cmbImageFormat.SetCurSel(0);

	SetMessage("");
	m_btnCapture.SetWindowText("Start Capture");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHardwareTriggerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHardwareTriggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL 
CHardwareTriggerDlg::InitializeCamera()
{
	HANDLE hCamera;
	PXL_RETURN_CODE rc = PxLInitialize(0, &hCamera);
	if (!API_SUCCESS(rc)) {
		AfxMessageBox("Unable to initialize a camera", MB_ICONEXCLAMATION);
		return FALSE;
	}

	// All we're going to do for initialization is make sure that the camera
	// supports triggering, and then set up hardware triggering
	if (!IsTriggeringSupported(hCamera)) {
		AfxMessageBox("This camera does not support triggering.", MB_ICONEXCLAMATION);
		PxLUninitialize(hCamera);
		return FALSE;
	}

	if (!EnableHardwareTriggering(hCamera)) {
		AfxMessageBox("Unable to configure hardware triggering.", MB_ICONEXCLAMATION);
		PxLUninitialize(hCamera);
		return FALSE;
	}

	// Record our successfully initialized camera's handle
	m_hCamera = hCamera;
	return TRUE;
}

void
CHardwareTriggerDlg::UninitializeCamera()
{
	if (0 != m_hCamera) {
		PxLUninitialize(m_hCamera);
		m_hCamera = 0;
	}
}


BOOL 
CHardwareTriggerDlg::IsTriggeringSupported(HANDLE hCamera)
{
	ASSERT(0 != hCamera);
	U32 bufferSize(0);

	// Query for the buffer size
	PXL_RETURN_CODE rc = PxLGetCameraFeatures(hCamera, FEATURE_TRIGGER, NULL, &bufferSize);
	if (!API_SUCCESS(rc)) { return FALSE; }

	// Alloc a buffer and read the information
	ASSERT(bufferSize > 0);
	std::vector<U8> buffer(bufferSize,0);
	CAMERA_FEATURES* pFeatureInfo = reinterpret_cast<CAMERA_FEATURES*>(&buffer[0]);
	rc = PxLGetCameraFeatures(hCamera, FEATURE_TRIGGER, pFeatureInfo, &bufferSize);
	if (!API_SUCCESS(rc)) { return FALSE; }

	ASSERT(buffer.size() == bufferSize);
	ASSERT(1 == pFeatureInfo->uNumberOfFeatures);
	ASSERT(FEATURE_TRIGGER == pFeatureInfo->pFeatures[0].uFeatureId);

	// If the PRESENCE flag is set, then triggering is supported
	return (0 == (pFeatureInfo->pFeatures[0].uFlags & FEATURE_FLAG_PRESENCE)) ? FALSE : TRUE;

}

BOOL 
CHardwareTriggerDlg::EnableHardwareTriggering(HANDLE hCamera)
{
	U32 flags;
	U32 numParams;

	// Query the number of parameters
	PXL_RETURN_CODE rc = PxLGetFeature(hCamera, FEATURE_TRIGGER, &flags, &numParams, NULL);
	if (!API_SUCCESS(rc)) { return FALSE; }
	ASSERT(numParams >= FEATURE_TRIGGER_NUM_PARAMS);

	// Read in the current trigger parameters
	std::vector<float> params(numParams,0.0f);
	rc = PxLGetFeature(hCamera, FEATURE_TRIGGER, &flags, &numParams, &params[0]);
	if (!API_SUCCESS(rc)) { return FALSE; }

	// TODO Set up triggering parameters as you require them.
	params[FEATURE_TRIGGER_PARAM_MODE] = 0;
	params[FEATURE_TRIGGER_PARAM_TYPE] = TRIGGER_TYPE_HARDWARE;
	params[FEATURE_TRIGGER_PARAM_POLARITY] = POLARITY_POSITIVE;
	params[FEATURE_TRIGGER_PARAM_DELAY] = 0.0;

	flags &= ENABLE_FEATURE(flags, true); // enable triggering
	rc = PxLSetFeature(hCamera, FEATURE_TRIGGER, flags, numParams, &params[0]);
	return API_SUCCESS(rc);
}

void 
CHardwareTriggerDlg::SetMessage(const CString& msg)
{
	m_txtMessage.SetWindowText(msg);
}

struct CaptureImageInfo
{
	CaptureImageInfo() {}
	HANDLE m_hCamera;
	CString filenameFormat;
	U32		outputFormat;
};


BOOL
SaveImageToFile(const char* pFilename, const char* pImage, U32 imageSize)
{
	size_t numBytesWritten;
	FILE* pFile;

	ASSERT(NULL != pFilename);
	ASSERT(NULL != pImage);
	ASSERT(imageSize > 0);

	// Open our file for binary write
	pFile = fopen(pFilename, "wb");
	if (NULL == pFile) {
		return FALSE;
	}

	numBytesWritten = fwrite((void*)pImage, sizeof(char), imageSize, pFile);

	fclose(pFile);
	
	return ((U32)numBytesWritten == imageSize) ? TRUE : FALSE;
}


BOOL
ConvertAndSaveImage(CaptureImageInfo const * const pCaptureInfo, std::vector<U8>& srcBuffer, FRAME_DESC& srcFrameDesc, int imageNumber)
{
	CString fileName;
	fileName.Format(pCaptureInfo->filenameFormat, imageNumber);

	PXL_RETURN_CODE rc;
	std::vector<U8> dstBuffer;

	if (pCaptureInfo->outputFormat == CUSTOM_IMAGE_FORMAT_RAW) {
		dstBuffer.resize(srcBuffer.size());
		// copy(srcBuffer.begin(), srcBuffer.end(), dstBuffer.begin());
		memcpy(&dstBuffer[0], &srcBuffer[0], srcBuffer.size());
	} else {
		U32 dstBufferSize(0);
		rc = PxLFormatImage(reinterpret_cast<void*>(&srcBuffer[0]), &srcFrameDesc, pCaptureInfo->outputFormat, NULL, &dstBufferSize);
		if (!API_SUCCESS(rc)) { return FALSE; }
		ASSERT(dstBufferSize > 0);

		dstBuffer.resize(dstBufferSize);
		rc = PxLFormatImage(reinterpret_cast<void*>(&srcBuffer[0]), &srcFrameDesc, pCaptureInfo->outputFormat, reinterpret_cast<void*>(&dstBuffer[0]), &dstBufferSize);
		if (!API_SUCCESS(rc)) { return FALSE; }
		ASSERT(dstBufferSize > 0);
	}

	// Now save the file to disk
	return SaveImageToFile(fileName, reinterpret_cast<const char*>(&dstBuffer[0]), dstBuffer.size());

}

//
// Given the pixel format, return the size of a individual pixel (in bytes)
//
// Returns 0 on failure.
//
float
GetPixelSize(U32 pixelFormat)
{
	U32 retVal = 0;

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
   return retVal;
}

//
// Query the camera for region of interest (ROI), decimation, and pixel format
// Using this information, we can calculate the size of a raw image
//
// Returns 0 on failure
//
U32 
DetermineRawImageSize(HANDLE hCamera)
{
	float parms[4];		// reused for each feature query
	U32 roiWidth;
	U32 roiHeight;
	U32 pixelAddressingValue;		// integral factor by which the image is reduced
	U32 pixelFormat;
	U32 numPixels;
	float pixelSize;
	U32 flags = FEATURE_FLAG_MANUAL;
	U32 numParams;

	ASSERT(0 != hCamera);

	// Get region of interest (ROI)
	numParams = 4; // left, top, width, height
	PxLGetFeature(hCamera, FEATURE_ROI, &flags, &numParams, &parms[0]);
	roiWidth	= (U32)parms[FEATURE_ROI_PARAM_WIDTH];
	roiHeight	= (U32)parms[FEATURE_ROI_PARAM_HEIGHT];

	// Query pixel addressing
	numParams = 2; // value, type (e.g. bin, average, ...)
	PxLGetFeature(hCamera, FEATURE_PIXEL_ADDRESSING, &flags, &numParams, &parms[0]);
	pixelAddressingValue = (U32)parms[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE];

	// We can calulate the number of pixels now.
	numPixels = (roiWidth / pixelAddressingValue) * (roiHeight / pixelAddressingValue);

	// Knowing pixel format means we can determine how many bytes per pixel.
	numParams = 1;
	PxLGetFeature(hCamera, FEATURE_PIXEL_FORMAT, &flags, &numParams, &parms[0]);
	pixelFormat = (U32)parms[0];

	// And now the size of the frame
	pixelSize = GetPixelSize(pixelFormat);

	return (U32)((float)numPixels * pixelSize);
}

PXL_RETURN_CODE 
CHardwareTriggerDlg::CaptureImagesToDisk(WorkerThread* pWorkerThread)
{
	CaptureImageInfo const * const pCaptureInfo = reinterpret_cast<CaptureImageInfo*>(pWorkerThread->GetUserData());
	ASSERT(NULL != pCaptureInfo);
	ASSERT(0 != pCaptureInfo->m_hCamera);

	int numImagesCaptured = 0;

	U32 bufferSize = DetermineRawImageSize(pCaptureInfo->m_hCamera);
	if (0 == bufferSize) {
		return ApiUnknownError;
	}
	std::vector<U8> buffer(bufferSize);
	FRAME_DESC frameDesc;
	while(pWorkerThread->KeepRunning()) {
		frameDesc.uSize = sizeof(frameDesc);
		PXL_RETURN_CODE rc = PxLGetNextFrame(pCaptureInfo->m_hCamera, static_cast<U32>(buffer.size()), reinterpret_cast<void*>(&buffer[0]), &frameDesc);
		if (API_SUCCESS(rc)) {
			if (ConvertAndSaveImage(pCaptureInfo, buffer, frameDesc, numImagesCaptured)) {
				numImagesCaptured++;
				pWorkerThread->NotifyUpdate(numImagesCaptured, TRUE);
			} else {
				// Error converting or saving file
				return ApiUnknownError;
			}
		} else {
			// Are we being asked to stop?
			if (!pWorkerThread->KeepRunning()) {
				return ApiSuccess;
			}
			// Error - stop
			return rc;
		}
	}

	return ApiSuccess;
}

//
// Called by the worker (image capture) thread when an image is captured.
// The wParam has the index of the number captured.
//
LRESULT	
CHardwareTriggerDlg::OnWorkerThreadUpdate	(WPARAM wParam, LPARAM lParam)
{
	CString msg;
	msg.Format("Captured image %d", wParam);
	m_numImagesCaptured = wParam;
	SetMessage(msg);
	return 0;
}

//
// Called when the worker thread has exited.
//
// We have a couple scenarios here:
// 1) The worker thread encountered an error and has exited.
// 2) We've asked the worker thread to exit and this is confirmation it's dead.
//
LRESULT	
CHardwareTriggerDlg::OnWorkerThreadFinished(WPARAM wParam, LPARAM lParam)
{
	m_critSection.Lock();

	// If the thread is reporting an error, we have to do the cleanup that
	// Stop Capture would normally do
	if ((m_pWorkerThread != 0) && (!API_SUCCESS(m_pWorkerThread->GetReturnCode()))) {
		CString msg;
		msg.Format("An error occurred capturing images. (Error 0x%8.8X)", m_pWorkerThread->GetReturnCode());
		StopCapturing();
		SetMessage(msg);
	} else {
		CString msg;
		msg.Format("Num images captured: %d", m_numImagesCaptured);
		SetMessage(msg);
	}

	m_btnCapture.SetWindowText("Start Capture");
	m_btnOK.EnableWindow(TRUE);

	m_critSection.Unlock();
	return 0;
}


//
// Assumes that the critical section is locked
// 
void
CHardwareTriggerDlg::StartCapturing()
{
	// And start streaming
	PXL_RETURN_CODE rc = PxLSetStreamState(m_hCamera, START_STREAM);
	if (!API_SUCCESS(rc)) {
		AfxMessageBox("Unable to start streaming.", MB_ICONEXCLAMATION);
		return;
	}
	// Start the thread that captures images
	CaptureImageInfo* pInfo = new CaptureImageInfo();
	pInfo->m_hCamera		= m_hCamera;
	
	// Get info from the combo box
	CString fileFormat;
	m_cmbImageFormat.GetWindowText(fileFormat);
	// Extract the file extension
	int offset = fileFormat.Find("(");
	CString extension = fileFormat.Mid(offset+1, fileFormat.GetLength()- offset - 2);
	pInfo->filenameFormat	= "image%3.3d" + extension;
	pInfo->outputFormat		= m_cmbImageFormat.GetItemData(m_cmbImageFormat.GetCurSel());

	m_pWorkerThread = new WorkerThread(CHardwareTriggerDlg::CaptureImagesToDisk, pInfo, true);
	m_numImagesCaptured = 0;
	m_pWorkerThread->StartRunning();

	// Update the GUI
	m_btnCapture.SetWindowText("Stop Capture");
	m_btnOK.EnableWindow(FALSE);
	SetMessage("Waiting to capture an image");

}


//
// Assumes that the critical section is locked
// 
void
CHardwareTriggerDlg::StopCapturing()
{
	ASSERT(0 != m_hCamera);
	ASSERT(NULL != m_pWorkerThread);

	// Stop streaming so that the thread, if blocked in PxLGetNextFrame, will unblock.
	PXL_RETURN_CODE rc = PxLSetStreamState(m_hCamera, STOP_STREAM);
	if (!API_SUCCESS(rc)) {
		AfxMessageBox("Unable to stop streaming.", MB_ICONEXCLAMATION);
	} else {
		m_pWorkerThread->WaitUntilFinished();
		delete m_pWorkerThread;
		m_pWorkerThread = NULL;
	}
}

void 
CHardwareTriggerDlg::OnBnClickedCapture()
{
	m_critSection.Lock();

	// Are we being asked to capture or asked to stop capturing?
	if (NULL == m_pWorkerThread) {
		StartCapturing();
	} else {
		CWaitCursor wait;
		SetMessage("Stopping capture...");
		m_pWorkerThread->RequestStop();
		StopCapturing();
	}
	m_critSection.Unlock();
}
