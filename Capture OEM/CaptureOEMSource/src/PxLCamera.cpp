#include "stdafx.h"
#include "PxLDevApp.h"
#include "PxLCamera.h"
#include "LookupGraph.h"
#include "HistogramDialog2.h"
#include "OneTimeAutoDialog.h"
#include "Helpers.h"
#include <process.h>
#include <set>
#include <vector>

// Used to keep track of which descriptor is in focus:
#define FOCUS_NONE		-1
#define FOCUS_ALL		-2

// Some feature parameters are defined by the DCAM spec to be in seconds, but
// we think than milliseconds are a much more logical unit to use.
#define MS_PER_SEC	1000


// Macro to repeat an API call until it succeeds, up to a specified
// maximum number of tries.
#define PXL_ERROR_CHECK_RETRY(MAXTRIES,APICALL)					\
	{ int _retry_count = (MAXTRIES);							\
	  while (--_retry_count > 0 && FAILED(APICALL))/*no-op*/;	\
	  if (_retry_count == 0) PXL_ERROR_CHECK(APICALL); }		\


// Shortcut to temporarily stop the video stream inside a scope:
#define STOP_STREAM_IF_REQUIRED(FEATURE)													\
	std::auto_ptr<CStreamState> _temp_ss(NULL);												\
	if (RequiresStoppedStream(FEATURE))														\
		_temp_ss = std::auto_ptr<CStreamState>(new CStreamState(this, STOP_STREAM, true));	\


// We use this type for allocating raw memory to be passed to the API.
typedef unsigned char byte;


// We prefer to use different units than those defined by the API for some
// of the features (eg: Milliseconds instead of seconds)
int 
GetUnitsMuliplier(const U32 featureId, const int paramNo)
{
	switch (featureId)
	{
	case FEATURE_SHUTTER:
		return MS_PER_SEC;
	case FEATURE_EXTENDED_SHUTTER:
		if (paramNo > 0) {
			return MS_PER_SEC;
		}
	default:
		return 1;
	}
}


/******************************************************************************
* Public Member Functions
******************************************************************************/

/**
* Function: CPxLCamera
* Purpose:  Constructor
*/
CPxLCamera::CPxLCamera(U32 serialNumber, CPxLCameraDoc* pDoc /*=NULL*/)
	: m_serialNumber(serialNumber)
	, m_handle(NULL)
	, m_pInfo(NULL)
	, m_pFeatures(NULL)
	, m_streamState(STOP_STREAM)
	, m_streamStartRequests(0)
	, m_disallowedPauses(0)
	, m_previewState(STOP_PREVIEW)
	, m_previewHwnd(NULL)
	, m_inFocusDescIndex(FOCUS_NONE)
	, m_frameCallback(NULL)
	, m_previewCallback(NULL)
	, m_imageCallback(NULL)
	, m_clipCallback(NULL)
	, m_frameCallbackParam(NULL)
	, m_previewCallbackParam(NULL)
	, m_imageCallbackParam(NULL)
	, m_clipCallbackParam(NULL)
	, m_pHistogramWindow(NULL)
	//, m_pPreview(new CDevAppPreview)
	, m_bNoPostViewChange(false)
	, m_bLookupTableNeedsUpdate(true)
	, m_absMaxShutter(-1.0f)
	, m_absMinShutter(-1.0f)
	, m_supportsAsymmetricPa(FALSE)
    , m_bSuspectedFirewallBlocking (FALSE)
{
    m_savedThreadExecutionState = SetThreadExecutionState(0); // this simply reads the current state
	m_previewStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_previewPos.left = m_previewPos.top = m_previewPos.right = m_previewPos.bottom = CW_USEDEFAULT;
	m_supportedXDecimations.clear();
	m_supportedYDecimations.clear();
}


/**
* Function: ~CPxLCamera
* Purpose:  Destructor
*/
CPxLCamera::~CPxLCamera(void)
{
	if (m_handle != NULL)
		Uninitialize();
	
	CloseHandle(m_previewStopEvent);
	//delete m_pPreview;
	
	// bugzilla.232, we're destroying this camera, ensure the historgram doesn't try to use it.
	if (m_pHistogramWindow != NULL)
	    m_pHistogramWindow->ForgetCamera();
	
}


/**
* Function: PostViewChange
* Purpose:  Notify the View that data has changed, and it may want to update
*           its appearance accordingly.
*           The possible values for the "change" parameter are:
*               CT_NoCamera
*               CT_NewCamera
*               CT_PreviewStarted
*               CT_PreviewPaused
*               CT_PreviewStopped
*               CT_FeatureChanged
*           The "changedFeature" parameter must be 0 except when the "change"
*           parameter is CT_FeatureChanged.
*/
void 
CPxLCamera::PostViewChange(const int /*eChangeType*/ change, const U32 changedFeature /*=0*/)
{
	HWND hwndMain = theApp.GetMainWnd()->GetSafeHwnd();
	if (hwndMain != NULL)
	{
		::PostMessage(hwndMain, 
					  WM_APP_FEATURE_CHANGED, 
					  reinterpret_cast<WPARAM>(this), 
					  static_cast<LPARAM>(change + changedFeature));
	}
}


/**
* Function: OnApiError
* Purpose:  
*/
void 
CPxLCamera::OnApiError(PxLException const& e, bool cameraRelated)
{
	static int recursionDepth = 0;
	TempIncr<int> ti(recursionDepth);
	if (recursionDepth > 5) {
		return;
	}

    // Bugzilla.2099 -- Don't report ApiRequiresControlAccess if the user has specified m_bMonitorAccessOnly
    if (e.GetReturnCode() == ApiRequiresControlAccess && theApp.m_bMonitorAccessOnly) return;

	CString msg = ErrorReportAsString(e);
	ErrorMessage(msg.GetLength() == 0 ? e.what() : msg);

    if (e.GetReturnCode() == ApiRequiresControlAccess &&
        ! theApp.m_bMonitorAccessOnly)
    {
        // We tried to do a write operation, but the camera indicaed we don't have write acceess, 
        // even though we used to have it.  In all likelihood, the camera reset, so re-initialize our
        // app to the new (power-up) settngs
        CWnd* wnd = theApp.GetMainWnd();
		if (wnd != NULL) {
		    PostMessage(wnd->m_hWnd, WM_APP_DEVICE_RESET, NULL, NULL);
		}
    }

}


/**
* Function: ErrorReportAsString
* Purpose:  
*/
CString 
CPxLCamera::ErrorReportAsString(PxLException const& e, bool cameraReleated)
{
	ERROR_REPORT er;
    PXL_RETURN_CODE rc = PxLGetErrorReport(cameraReleated ? m_handle : NULL, &er);
	CString s;
	if (!FAILED(rc) && strlen(er.strFunctionName) > 0 && strlen(er.strReturnCode) > 0)
	{
#ifdef _DEBUG
		// Put extra info in error messages in DEBUG mode
		s.Format(_T("%.32s returned %.32s\n%.265s\n\n%s"), 
					&er.strFunctionName[0], 
					&er.strReturnCode[0], 
					&er.strReport[0],
					e.what());
#else
		s.Format(_T("%.32s returned %.32s\n%.265s"), 
					&er.strFunctionName[0], 
					&er.strReturnCode[0], 
					&er.strReport[0]);
#endif
	}
	else
	{
#ifdef _DEBUG
		s.Format(_T("%s\n\n%s"), ReturnCodeAsString(e.GetReturnCode()), e.what());
#else
		s.Format(_T("%s"), ReturnCodeAsString(e.GetReturnCode()));
#endif
	}
	return s;
}


/**
* Function: Initialize
* Purpose:  
*/
void 
CPxLCamera::Initialize(void)
{
    ULONG rc;

	rc = PxLInitializeEx(m_serialNumber, 
                         &m_handle,
                         theApp.m_bMonitorAccessOnly ? CAMERA_INITIALIZE_EX_FLAG_MONITOR_ACCESS_ONLY : 0);
	PXL_ERROR_CHECK(rc); 

    // If we are using a reduced speed, remember this so we can warn the user
    m_bReducedLinkSpeed = ApiSuccessReducedSpeedWarning == rc;
    
	// Load the camera features. The data returned will be held for as long
	// as this camera remains initialized.
	LoadCameraFeatures();
	LoadCameraInfo();

	if (m_serialNumber == 0)
	{
		// Get the serial number from the CAMERA_INFO (it is stored there as text).
		char temp[32] = {0};
		std::copy(&m_pInfo->SerialNumber[0], &m_pInfo->SerialNumber[9], temp);
		m_serialNumber = atoi(temp);
	}	
}


/**
* Function: Uninitialize
* Purpose:  
*/
void 
CPxLCamera::Uninitialize(void)
{
	ClearCameraInfo();
	ClearFeatures();

	if (m_previewState != STOP_PREVIEW)
	{
		try
		{
			StopPreview();
		}
		catch (PxLException const&)
		{
			// ignore.
		}
	}

	CloseHistogram();

	// No PxLErrorCheck around this call - we don't wan't to throw exceptions
	// from Uninitialize, because at this point, we are done with this camera,
	// and don't really care that much if it doesn't uninitialize properly.
	// (And also because Uninitialize is called from ~CPxLCamera).
	PxLUninitialize(m_handle);

	// Clear everything - we leave only m_serialNumber.
	m_handle = NULL;
	m_streamState = STOP_STREAM;
	m_streamStartRequests = 0; // Bugzilla.521 - reset the stream start count.
	m_previewState = STOP_PREVIEW;
	m_previewHwnd = NULL;
	m_descriptors.clear();
	m_inFocusDescIndex = FOCUS_NONE;
}


/**
* Function: CloseHistogram
* Purpose:  
*/
void 
CPxLCamera::CloseHistogram(void)
{
	if (m_pHistogramWindow != NULL)
	{
		::SendMessage(m_pHistogramWindow->m_hWnd, WM_CLOSE, 0, 0);
	}
}


/**
* Function: Reinitialize
* Purpose:  
*/
void 
CPxLCamera::Reinitialize(void)
{
	if (m_serialNumber == 0) {
		return;
	}

	Uninitialize();
	Initialize();
}


/**
* Function: SetCameraName
* Purpose:  
*/
void 
CPxLCamera::SetCameraName(LPCSTR name)
{
	PXL_ERROR_CHECK(PxLSetCameraName(m_handle, name));
	LoadCameraInfo();
}


/**
* Function: LoadCameraSettings
* Purpose:  
*/
void 
CPxLCamera::LoadCameraSettings(const int channel)
{
	// Stop the stream if it is running.
	CStreamState tempState(this, STOP_STREAM, true);
	// PxLLoadSettings will block until the camera is responsive again.
	PXL_ERROR_CHECK(PxLLoadSettings(m_handle, channel));

	// Bug 155
	// Restore the pixel format to lowest supported pixel format for most cameras.  For Color cameras, 
    // used the 'generic' bayer8 specifier.
	if (FACTORY_DEFAULTS_MEMORY_CHANNEL == channel) {
		unsigned int minPixelFormat = static_cast<int>(GetFeatureMinVal(FEATURE_PIXEL_FORMAT));
        if (PIXEL_FORMAT_MONO8 == minPixelFormat) {
			SetFeatureValue(FEATURE_PIXEL_FORMAT, static_cast<float>(minPixelFormat));
		} else if (PIXEL_FORMAT_YUV422 == minPixelFormat) {
			SetFeatureValue(FEATURE_PIXEL_FORMAT, static_cast<float>(PIXEL_FORMAT_BAYER8));
		}
	}

}


/**
* Function: SaveCameraSettings
* Purpose:  
*/
void 
CPxLCamera::SaveCameraSettings(const int channel)
{
	// Unlike LoadCameraSettings, we do not have to stop the stream.
	// PxLSaveSettings will block until the camera is responsive again.
	PXL_ERROR_CHECK(PxLSaveSettings(m_handle, channel));
}


/**
* Function: GetSerialNumber
* Purpose:  
*/
U32 
CPxLCamera::GetSerialNumber(void)
{
	return m_serialNumber;
}


/**
* Function: GetCameraInfo
* Purpose:  Return a pointer to the CAMERA_INFO structure that describes
*           this camera.
*/
CAMERA_INFO const * 
CPxLCamera::GetCameraInfo(void)
{
	return m_pInfo.get();
}


/**
* Function: GetFeaturePtr
* Purpose:  
*/
CAMERA_FEATURE* 
CPxLCamera::GetFeaturePtr(const U32 featureId)
{
	static CAMERA_FEATURE dummy = {0, 0, 0, NULL};
	// If we're running on an older version that doesn't know about 
	// this feature, just say "Unsupported" via a dummy feature.
	if ((featureId >= m_features.size()) || (m_pFeatures == NULL) || (m_features[featureId] == NULL)) {
		return &dummy;
	}
	return m_features[featureId];
}


/**
* Function: LoadCameraInfo
* Purpose:  Query the camera for its Name, Model, Vendor, etc., and store all
*           that stuff in the CAMERA_INFO structure pointed to by m_pInfo.
*/
void 
CPxLCamera::LoadCameraInfo(void)
{
	if (m_handle == NULL)
	{
		TRACE("CPxLCamera::LoadCameraInfo called while m_handle == NULL\n");
		return;
	}

	// Allocate memory.
	//m_pInfo.reset(new CAMERA_INFO); // vc7
	m_pInfo = std::auto_ptr<CAMERA_INFO>(new CAMERA_INFO); // vc6

	PXL_ERROR_CHECK(PxLGetCameraInfoEx(m_handle, m_pInfo.get(), sizeof(CAMERA_INFO)));

}


/**
* Function: LoadCameraFeatures
* Purpose:  Query the camera to find out which features it supports, and store
*           all the information that it returns in the CAMERA_FEATURES struct
*           pointed to by m_pFeatures.
*/
void 
CPxLCamera::LoadCameraFeatures(void)
{
	ClearFeatures();

	// Determine how much memory to allocate for the CAMERA_FEATURES struct.
	// API Note: By passing NULL in the second parameter, we are telling the
	//   API that we don't want it to populate our CAMERA_FEATURES structure
	//   yet - instead we just want it to tell us how much memory it will
	//   need when it does populate it.
	U32 size = 0;
	PXL_ERROR_CHECK(PxLGetCameraFeatures(m_handle, FEATURE_ALL, NULL, &size));
	ASSERT(size > 0); // API sanity check

	// Allocate the amount of memory that we were told is required.
	m_pFeatures = reinterpret_cast<CAMERA_FEATURES*>(new byte[size]);

	PXL_ERROR_CHECK(PxLGetCameraFeatures(m_handle, FEATURE_ALL, m_pFeatures, &size));

	// The CAMERA_FEATURES structure is loaded only when a camera is 
	// initialized, but is read from frequently to determine the camera's
	// capabilities. To make reading this data easier, we populate a vector
	// that maps feature IDs to their corresponding CAMERA_FEATURE structure.
	m_features.resize(m_pFeatures->uNumberOfFeatures, NULL);

	
	CAMERA_FEATURE* pCurr = &m_pFeatures->pFeatures[0];
	for (int i=0; i < m_pFeatures->uNumberOfFeatures; i++, pCurr++)
	{
		ASSERT(pCurr->uFeatureId < FEATURES_TOTAL);
		m_features[pCurr->uFeatureId] = pCurr;
	}
}


/**
* Function: IsDynamicFeature
* Purpose:  Return true if the CAMERA_FEATURE structure that describes the
*           specified feature needs to be updated each time it is requested.
*           For most features, we can read the Max and Min values once, when
*           the camera is initialized, and assume that those limits never
*           change. Other features, however, have dynamic limits.
*           FrameRate is the best example - the maximum frame rate depends
*           on the current value of other features, including ROI, Decimation,
*           and Pixel Format.
*/
bool 
CPxLCamera::IsDynamicFeature(const U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_FRAME_RATE: // Depends on ROI, decimation, etc.
	case FEATURE_SHUTTER: // As of ~May 2004, can depend on frame rate (for DCAM 1.31 compliance)
	case FEATURE_EXTENDED_SHUTTER: // Bugzilla.178 -- reread new limits when a kneepoint is set
	case FEATURE_SHARPNESS_SCORE:
	case FEATURE_AUTO_ROI:
    case FEATURE_GAIN:  // the addition of FEATURE_GAIN_HDR makes this one dyamic
	// others?
		return true;
	case FEATURE_ROI:
	    // this feature is dynamic for those cameras that support Special Camera Modes.
	    if (FeatureSupported (FEATURE_SPECIAL_CAMERA_MODE)) return true;
	    else return false;
    case FEATURE_ZOOM:
    case FEATURE_FOCUS:
    case FEATURE_LIGHTING:
        // These features are sometimes implemented on a seperate controller, and are therfor dynamic
        return true;
	default:
		return false;
	}
}


/**
* Function: ReloadFeature
* Purpose:  
*/
void 
CPxLCamera::ReloadFeature(const U32 featureId)
{
	// Determine how much memory to allocate for the CAMERA_FEATURES struct.
	U32 size = 0;
	PXL_ERROR_CHECK(PxLGetCameraFeatures(m_handle, featureId, NULL, &size)	);
	ASSERT(size > 0); // API sanity check

	// Allocate the amount of memory that we were told is required.
	std::vector<U8> buffer(size,0);
	CAMERA_FEATURES* const pFeatures = reinterpret_cast<CAMERA_FEATURES*>(&buffer[0]);

	// Load the feature information.
	PXL_ERROR_CHECK(PxLGetCameraFeatures(m_handle, featureId, pFeatures, &size));

	// We asked for info on a single feature - make sure we got only one:
	ASSERT(pFeatures->uNumberOfFeatures == 1);
	// Make sure we got the right one:
	ASSERT(pFeatures->pFeatures->uFeatureId == featureId);

	// Copy the new information into our global CAMERA_FEATURES structure.
	// Get pointer to the first (and only) CAMERA_FEATURE in our newly loaded data.
	CAMERA_FEATURE* pNewFeature = pFeatures->pFeatures;
	// Get a pointer to the corresponding CAMERA_FEATURE in m_pFeatures:
	CAMERA_FEATURE* pOldFeature = GetFeaturePtr(featureId);

	// The number of parameters should not have changed:
	ASSERT(pNewFeature->uNumberOfParameters == pOldFeature->uNumberOfParameters);

	// Copy all the data:
	pOldFeature->uFlags = pNewFeature->uFlags;

	for (int i = 0; i < pNewFeature->uNumberOfParameters; i++)
	{
		pOldFeature->pParams[i].fMinValue = pNewFeature->pParams[i].fMinValue;
		pOldFeature->pParams[i].fMaxValue = pNewFeature->pParams[i].fMaxValue;
	}

}


/**
* Function: GetAbsMaxShutter
* Purpose:  We cache the absolute maximum value of FEATURE_SHUTTER because
*           calling PxLGetCameraFeatures(FEATURE_SHUTTER) returns the *current*
*           maximum, which is limited by the current framerate if framerate
*           is on. However, in Simple mode we want the Shutter slider to allow
*           the full range, and simply turn OFF framerate behind the scenes if
*           shutter is set to a higher value than is currently allowed. The 
*           problem occurs in Simple mode's ConfigureControls method, which
*           used to determine the absolute maximum shutter value every time
*           you went into simple mode by shutting off framerate (which required
*           a stream-stop), querying the maximum, and then turning framerate
*           back on (another stream-stop).  This was too slow, so I added code
*           (which, by the way, occupies fewer lines than this comment does) to
*           cache the max shutter value.
*           2004-11-20 - ditto for the min val.
*/
float 
CPxLCamera::GetAbsMaxShutter(void)
{
	if (m_absMaxShutter == -1)
	{
		// determine the value.
		bool toggle = false;
		U32 fr_flags = 0;
		float fr = 0;
		if (this->FeatureSupportsOnOff(FEATURE_FRAME_RATE))
		{
			fr = this->GetFeatureValue(FEATURE_FRAME_RATE, &fr_flags);
			toggle = (fr_flags & FEATURE_FLAG_OFF) == 0;
		}
		
		if (toggle)
		{
			// We need to toggle FrameRate OFF so we can read the *real* shutter limits.
			STOP_STREAM_IF_REQUIRED(FEATURE_FRAME_RATE)

			// Turn frame rate off
			this->SetOnOff(FEATURE_FRAME_RATE, false);

			// With framerate OFF, this should give us the absolute maximum shutter:
			std::pair<float,float> minmax = this->GetFeatureLimits(FEATURE_SHUTTER);
			m_absMinShutter = minmax.first;
			m_absMaxShutter = minmax.second;

			// Turn it back on, and set the value back to where it was.
			PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_FRAME_RATE, fr_flags, 1, &fr));
		}
		else
		{
			// No need to turn off framerate - just read the max value.
			std::pair<float,float> minmax = this->GetFeatureLimits(FEATURE_SHUTTER);
			m_absMinShutter = minmax.first;
			m_absMaxShutter = minmax.second;
		}
	}
	return m_absMaxShutter;
}

// See comment for GetAbsMaxShutter, above.
float 
CPxLCamera::GetAbsMinShutter(void)
{
	if (m_absMinShutter == -1)
	{
		this->GetAbsMaxShutter(); // load cached values for abs min and max
	}
	return m_absMinShutter;
}


/**
* Function: ClearCameraInfo
* Purpose:  Delete the CAMERA_INFO structure that contains the info about
*           the active camera. Called when the camera is uninitialized.
*/
void 
CPxLCamera::ClearCameraInfo(void)
{
	m_pInfo.reset(NULL);
}


/**
* Function: ClearFeatures
* Purpose:  Delete the CAMERA_FEATURES structure that contains info about
*           the active camera. Called when the camera is uninitialized.
*/
void 
CPxLCamera::ClearFeatures(void)
{
	if (m_pFeatures != NULL)
	{
		m_features.clear();
		delete[] reinterpret_cast<byte*>(m_pFeatures);
		m_pFeatures = NULL;
	}
}


/**
* Function: GetStreamState
* Purpose:  
*/
int 
CPxLCamera::GetStreamState(void)
{
	return m_streamState;
}


/**
* Function: SetStreamState
* Purpose:  Starting and stopping of the video stream is handled by
*           incrementing or decrementing a count of the number of requests
*           for a started stream. This way the various parts of the application
*           that require the video stream (preview window, histogram, image and
*           clip capture...) can remain decoupled from each other.
*           However, there is a second parameter that gives the option to force
*           the stream to stop. This is needed, for example, in order to cancel
*           a clip capture in progress, or temporarily suspend streaming when changing
*           a feature that can only be changed while streaming is stopped.
*
*           Use Cases of SetStreamState
*           
*           // Use Case 1) Normal use
*           Call			Action Taken
*           -------------------------------
*           start			// start
*           stop			// stop
*           
*           // Use Case 2) Normal Use with Temporary Suspension
*           Call			Action Taken
*           -------------------------------
*           start			// start
*           stop - force	// stop
*           start			// start          
*           stop			// stop
*           
*           // Use Case 3) Recursive Use
*           Call			Action Taken
*           -------------------------------
*           start			// start
*           start			// ignore
*           stop			// ignore
*           stop			// stop
*           
*           // Use Case 4) Recursive Use with Temporary Suspension
*           Call			Action Taken
*           -------------------------------
*           start			// start
*           start			// ignored
*           stop - force	// stop
*           start			// start **
*           stop			// ignore
*           stop			// stop
*
*           The difficultly is when it comes to the line marked with **:
*           When coming in, m_streamState is STREAM_STOP, and m_streamStartRequests is 1.
*           m_streamStartRequests will be set incrememented to be 2, so might appear that we're in the recursion state.
*           To detect this situation, we have to check that the current state is not STREAM_STOP.
*
*           // Use Case 5) Start stream fails (See Issue.Software.342)
*           Call			Action Taken
*           -------------------------------
*           start			// start attempted, but PxLSetStreamState fails
*
*           In this use case, we have to be exception safe and not commit any changes to the recorded state until
*           after the successful call to PxLSetStreamState.
*           
*           Things to keep in mind:
*           When actually starting the stream, have to record the thread execution state
*           When actually stopping the stream, have to restore the thread execution state
*           See Issue.USB.38
*           
*/

void 
CPxLCamera::SetStreamState(const int state, const bool forceChange)
{
	PXL_RETURN_CODE rc;
	
	// Doing the assert this way makes it easier to read;
	if (forceChange) {
		ASSERT(START_STREAM != state);
	}

	// Use a temporary copy of m_streamStateRequests because we don't want to commit the 
	// change to it until we were successful in our attempts to change the state (if required).
	int startRequests = m_streamStartRequests;
	int disallowedPauses = m_disallowedPauses;
	if (START_STREAM == state ) {
		++startRequests;
	} else {
		//ASSERT(startRequests > 0);
		if (startRequests > 0) {
   	        --startRequests;
		    // Bugzilla.386 -- Careful we may have just messed up our startRequest count.  Specifically,
		    // if a 'client' transitions from start->pause->stop, then we don't want do the --startRequest
		    // for the start-->pause AND again on pause-->stop, as this will stop the stream even if some other client is 
		    // interested in the stream.
		    // In these circumstances, then we should not do the dec on pause->stop, as we already did the dec on start->pause.
		    
		    // So, how do we identify pause->stop.  Well there is this:
		    //     STOP_STREAM == state && PAUSE_STREAM == m_streamState  
		    // but that is not always sufficient, as actual m_streamState may be START_STREAM event though a client had
		    // requested PAUSE_STREAM, becuase some other client wanted START_STREAM.  To deal with this circumstance, we count
		    // the number of times we try to pause, but could not
		    if ((STOP_STREAM == state && PAUSE_STREAM == m_streamState) ||
		        (STOP_STREAM == state && disallowedPauses > 0)) {
		        //Yea, I know -- why bother adding whet we just took out -- it just makes the loigic above easier (to me)
		        ++startRequests;
		    }
		}
	}

	// Are we actually supposed to change the stream state of the camera, or is
	// nothing required because we're just dealing with recursion?
	if (  forceChange || 
		((START_STREAM != state) && (0 == startRequests)) ||
		((START_STREAM == state) && (startRequests >= 1) && (START_STREAM != m_streamState)) )
	{
		rc = PxLSetStreamState(m_handle, state);
		PXL_ERROR_CHECK(rc);
		m_streamState = state;
		
		// We've successfully changed the stream state.
		// Set the thread execution state so that the computer won't sleep while we're streaming (See Issue.USB.38)
        if (START_STREAM == state) {
            m_savedThreadExecutionState = SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
        } else {
            SetThreadExecutionState (ES_CONTINUOUS | m_savedThreadExecutionState);  // restore the old state
        }
	}
	
	if (PAUSE_STREAM == state && PAUSE_STREAM != m_streamState) {
	    ++disallowedPauses;
	// all other state transitions work correctly on startRequests, so dec our kludge factor. 
	} else if (disallowedPauses > 0) {
	    --disallowedPauses;
	}

	// Record the change to the recursion depth
	m_streamStartRequests = startRequests;
	m_disallowedPauses = disallowedPauses;
}


/**
* Function: GetPreviewState
* Purpose:  
*/
int 
CPxLCamera::GetPreviewState(void)
{
	return m_previewState;
}


/**
* Function: SetPreviewState
* Purpose:  
*/
void 
CPxLCamera::SetPreviewState(const int state)
{
	PXL_ERROR_CHECK(PxLSetPreviewState(m_handle, state, &m_previewHwnd));
	m_previewState = state;
}


/**
* Function: PreviewMonitorThread
* Purpose:  This thread function just waits for the preview window to be
*           either closed or activated.
*/
UINT __stdcall 
CPxLCamera::PreviewMonitorThread(void* pData)
{
	CPxLCamera* pCam = static_cast<CPxLCamera*>(pData);
	while (true)
	{
		DWORD dw = WaitForSingleObject(pCam->m_previewStopEvent, 250);
		if (WAIT_TIMEOUT == dw)
		{
			// Preview window has not yet been closed by using the stop button.
			// Poll the preview to make sure it's still there.
			if (!::IsWindow(pCam->m_previewHwnd))
			{
				// The preview window is gone. The user probably closed it by
				//clicking the little "x" or hitting Alt-F4 or something.
				// Notify the application.
				CWnd* wnd = theApp.GetMainWnd();
				if (wnd != NULL)
				{
					PostMessage(wnd->m_hWnd, WM_APP_PREVIEW_CLOSED, reinterpret_cast<WPARAM>(pCam), 0);
				}
				// Preview will be closed, so end this tread.
				break;
			}
			else if (::GetForegroundWindow() == pCam->m_previewHwnd)
			{
				// The preview window that we are monitoring is now the
				// active window. Notify the application.
				CWnd* wnd = theApp.GetMainWnd();
				if (wnd != NULL)
				{
					PostMessage(wnd->m_hWnd, WM_APP_PREVIEW_ACTIVATED, reinterpret_cast<WPARAM>(pCam), 0);
				}
			}
		}
		else if (WAIT_OBJECT_0 == dw)
		{
			// We have been signalled that the preview was closed properly.
			break;
		}
	}
	return 0; // never executed
}


/**
* Function: StartPreview
* Purpose:  Start the video preview window.
*/
void 
CPxLCamera::StartPreview()
{
	bool resetWindow = false;

	// Api Note: It is OK to attempt to start the video stream, even if it is
	//   already started - the API returns a Success code.
	SetStreamState(START_STREAM);

	bool wasStopped = false;
	bool resetNeeded = false;
	if (m_previewState == STOP_PREVIEW)
	{
		wasStopped = true;

		// The preview window is being opened - set its attributes.

		const int MAX_WINDOW_TITLE = 64;
		char windowTitle[MAX_WINDOW_TITLE];
		sprintf_s(windowTitle, MAX_WINDOW_TITLE, "PixeLINK Video Preview - s/n %lu", m_serialNumber);

		U32 width, height;
		if (!theApp.m_bAutoResetPreview
			&& m_previewPos.left != CW_USEDEFAULT
			&& m_previewPos.top != CW_USEDEFAULT)
		{
			// Set the window size to whatever it was last time it was closed.
			width = m_previewPos.right - m_previewPos.left;
			height = m_previewPos.bottom - m_previewPos.top;
		}
		else
		{
			// Let the size of the preview window fit the current ROI.
			width = height = CW_USEDEFAULT;
			// Because our ResetPreviewWindow is enhanced above and beyond the
			// capabilities of the API to prevent too-big windows, we call it
			// at start-up of the preview, in case the default preview window
			// that the API creates is distorted due to a large ROI.
			resetNeeded = true;
		}

		PXL_ERROR_CHECK(
			PxLSetPreviewSettings(
					m_handle, 
					windowTitle,
					WS_VISIBLE | WS_OVERLAPPEDWINDOW,
					m_previewPos.left,
					m_previewPos.top,
					width,
					height,
					NULL,
					0
				)
		);
	}

	SetPreviewState(START_PREVIEW);

	// Start the preview monitor thread, which will notify us if the preview
	// window is closed by any means other than the Stop button.
	ResetEvent(m_previewStopEvent);
	UINT thrdaddr = 0;
	HANDLE thrd = (HANDLE)_beginthreadex(NULL, 0, PreviewMonitorThread, this, 0, &thrdaddr);
	if (thrd != NULL)
	{
		::CloseHandle(thrd); // We do not wait on this thread, so close its handle now.
	}

	if ((theApp.m_bAutoResetPreview && wasStopped) || resetNeeded)
		ResetPreviewWindow();

	PostViewChange(CT_PreviewStarted);

} // StartPreview


/**
* Function: PausePreview
* Purpose:  Pause the video preview.
*/
void 
CPxLCamera::PausePreview()
{
	if (m_previewState != START_PREVIEW)
	{
		TRACE("CPxLCamera::PausePreview called, but preview is not started.\n");
		return;
	}

	SetPreviewState(PAUSE_PREVIEW);

	// We also pause the stream.  We could leave it running - it doesn't really
	// matter. However, if we were to STOP the stream, then start the stream
	// of another camera, we may no longer have the bandwith to resume running
	// this preview, which would be confusing to the user since the preview
	// window is already open.
	SetStreamState(PAUSE_STREAM);
	
	PostViewChange(CT_PreviewPaused);

} // PausePreview


/**
* Function: StopPreview
* Purpose:  Stop the video preview, closing the preview window.
*/
void 
CPxLCamera::StopPreview()
{
	if (STOP_PREVIEW == m_previewState) {
		TRACE("CPxLCamera::StopPreview called, but preview is not started.\n");
		return;
	}

	// Keep a record of where on the screen the preview window is, so that the
	// next time it is started, we can put it in the same place.
	if (::IsWindow(m_previewHwnd)) {
		::GetWindowRect(m_previewHwnd, &m_previewPos);
	}

	// Notify the window monitor thread that we are shutting down the preview
	// through proper channels.
	SetEvent(m_previewStopEvent);

	// Stop the preview...
	SetPreviewState(STOP_PREVIEW);

	// ...and the video stream.
	SetStreamState(STOP_STREAM);

	PostViewChange(CT_PreviewStopped);
}


/**
* Function: ResetPreviewWindow
* Purpose:  
*/
void 
CPxLCamera::ResetPreviewWindow(void)
{
	if (m_previewState == STOP_PREVIEW)
		return;

	// Find the size of the non-client area of the window.
	CRect wnd, client;
	::GetWindowRect(m_previewHwnd, &wnd);
	::GetClientRect(m_previewHwnd, &client);
	CSize borders(wnd.Width() - client.Width(), wnd.Height() - client.Height());

	PXL_ERROR_CHECK(PxLResetPreviewWindow(m_handle));

	bool adjustWindow = false;

	// PxLResetPreviewWindow just sets the size of the window - it does not
	// deal with a MAXIMIZED window. We need to un-maximize it ourselves.
    WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(wndpl);
	::GetWindowPlacement(m_previewHwnd, &wndpl);
    if (wndpl.showCmd == SW_SHOWMAXIMIZED)
    {
        wndpl.showCmd = SW_SHOWNORMAL;
		adjustWindow = true;
    }
    
	// We don't want the preview to become distorted because the ROI is bigger
	// than the screen can accomodate.  Because we just called PxLResetPreview,
	// wnpl.rcNormalPosition contains the full height and width of the window,
	// as set by the API. This size may be larger than Windows will actually
	// display the window.
	CRect rctMax; // Max size that we want to allow the preview to be.
	if (!::SystemParametersInfo(SPI_GETWORKAREA, 0, &rctMax, 0))
		return;

	CRect rctWnd(wndpl.rcNormalPosition);
	if (rctWnd.Width() > rctMax.Width()
		|| rctWnd.Height() > rctMax.Height())
	{
		// Too big. Shrink the preview.
		adjustWindow = true;

		CRect roi = this->GetFeatureRect(FEATURE_ROI);
		PixelAddressing pa = this->GetPixelAddressing();
        
        CSize roiSize(DEC_SIZE(roi.Width(), pa.x), DEC_SIZE(roi.Height(), pa.y));
        // Accomodate interleaving
        bool interleaved = GetFeatureValue(FEATURE_GAIN_HDR) == FEATURE_GAIN_HDR_MODE_INTERLEAVED;
        if (interleaved) roiSize.cx *= 2;	
		if (this->FeatureSupported(FEATURE_ROTATE))
		{
			float rot = this->GetFeatureValue(FEATURE_ROTATE);
			if (rot == 90 || rot == 270)
			{
				std::swap(roiSize.cx, roiSize.cy);
			}
		}

		// Find a client-area size that is:
		//   a) an integral fraction of the ROI, and
		//   b) fits on the screen.
		/*
		int div = 2;
		while (roiSize.cx / div + borders.cx > rctMax.Width()
				|| roiSize.cy / div + borders.cy > rctMax.Height())
		{
			// Still to big
			++div;
		}
		
		// Set the new width and height of the window.
		wndpl.rcNormalPosition.right = wndpl.rcNormalPosition.left + borders.cx + roiSize.cx/div;
		wndpl.rcNormalPosition.bottom = wndpl.rcNormalPosition.top + borders.cy + roiSize.cy/div;
		*/

		// New version: make it just barely fit on the desktop.
		double scale = min(
				static_cast<double>(rctMax.Width() - borders.cx) / roiSize.cx,
				static_cast<double>(rctMax.Height() - borders.cy) / roiSize.cy
			);
		wndpl.rcNormalPosition.right = (LONG)(wndpl.rcNormalPosition.left + borders.cx + (roiSize.cx * scale));
		wndpl.rcNormalPosition.bottom = (LONG)(wndpl.rcNormalPosition.top + borders.cy + (roiSize.cy * scale));
	}

	if (adjustWindow)
	{
		::SetWindowPlacement(m_previewHwnd, &wndpl);
	}
}


/**
* Function: GetPreviewHwnd
* Purpose:  
*/
HWND 
CPxLCamera::GetPreviewHwnd(void)
{
	return m_previewHwnd;
}


inline bool 
Is16BitFormat(const ULONG fmt)
{
	// These are the pixel formats that come in byte-swapped.
	return fmt == PIXEL_FORMAT_BAYER16_BGGR
		|| fmt == PIXEL_FORMAT_BAYER16_GBRG
		|| fmt == PIXEL_FORMAT_BAYER16_GRBG
		|| fmt == PIXEL_FORMAT_BAYER16_RGGB
		|| fmt == PIXEL_FORMAT_MONO16
		;
}

inline bool 
Is12BitPackedFormat(const ULONG fmt)
{
	// These are the pixel formats that come in byte-swapped.
	return fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_GRBG_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED
		|| fmt == PIXEL_FORMAT_MONO12_PACKED
		;
}

inline bool 
Is12BitgPackedFormat(const ULONG fmt)
{
	// These are the pixel formats that come in byte-swapped.
	return fmt == PIXEL_FORMAT_MONO12_PACKED
		|| fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_MONO12_PACKED_MSFIRST
		;
}

inline bool 
Is10BitgPackedFormat(const ULONG fmt)
{
	// These are the pixel formats that come in byte-swapped.
	return fmt == PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST
		|| fmt == PIXEL_FORMAT_MONO10_PACKED_MSFIRST
		;
}

inline U16 
ByteSwapU16(const U16 val)
{
	return (val << 8) | (val >> 8);
}

// Notes on 12-bit packing....
//   We have two different flavors of 12-bit packing:
//       1. XXXX_PACKED  AKA GigE Vision 12-bit packed.  It is packed as follows:
//             11 12 22 33 34 44 55 56 66
//       2. XXXX_PACKED_MSFIRST, AKA Pixel Fromat Naming Convention (PFNC) YYYY12g.  It is packed as follows:
//             11 22 12 33 44 34 55 66 56
inline void 
Pack12Bit(const U16 val1, const U16 val2, U8* const data)
{
    data [0] = static_cast<U8>(val1 >> 4);
    data [1] = static_cast<U8>((val1 & 0xF) | (val2 & 0xF) << 4 );
    data [2] = static_cast<U8>(val2 >> 4);
}

inline U16 
Unpack12Bit(const U16 val, const bool even)
{
	// even is refernce to an even (first) or odd (second) byte in the packed sequence
	if (even) {
	        //    MS 8 bits              LS 4 bits
	        //  ----------------      -------------------
	    return ((val & 0xFF) << 4   | (val & 0x0F00) >> 8);
	} else {
	    return ((val & 0xFF00) >> 4 | (val & 0xF0)>> 4);
	}
}

inline void 
Pack12Bitg(const U16 val1, const U16 val2, U8* const data)
{
    data [0] = static_cast<U8>(val1 >> 4);
    data [1] = static_cast<U8>(val2 >> 4);
    data [2] = static_cast<U8>((val1 & 0xF) | (val2 & 0xF) << 4 );
}

inline void 
Pack10Bitg(const U16 val1, const U16 val2, const U16 val3, const U16 val4, U8* const data)
{
    data [0] = static_cast<U8>(val1 >> 2);
    data [1] = static_cast<U8>(val2 >> 2);
    data [2] = static_cast<U8>(val3 >> 2);
    data [3] = static_cast<U8>(val4 >> 2);
    data [4] = static_cast<U8>((val1 & 0x03) | (val2 & 0x03) << 2 |  (val3 & 0x03) << 4 | (val4 & 0x03) << 6);
}

inline U16 
Unpack12Bitg(const U8* const data, const bool even)
{
	// even is refernce to an even (first) or odd (second) byte in the packed sequence
	if (even) {
	        //    MS 8 bits              LS 4 bits
	        //  ----------------      -------------------
	    return (((U16)data[0]) << 4   | ((U16)data[2]) & 0x000F);
	} else {
	    return (((U16)data[1]) << 4   | ((U16)data[2]) >> 4);
	}
}


inline U16 
Unpack10Bitg(const U8* const data, const int pixel)
{
	// pixel is a value of 0..3, identifying the which of the 4 pixels in the the packing
    // sequence, to use
    switch (pixel)
    {
            //        MS 8 bits              LS 2 bits
            //  --------------------    --------------------
    case 0:
        return (((U16)data[0]) << 2) | ((U16)data[4] & 0x03);
    case 1:
        return (((U16)data[1]) << 2) | (((U16)data[4] & 0x0C) >> 2);
    case 2:
        return (((U16)data[2]) << 2) | (((U16)data[4] & 0x30) >> 4);
    case 3:
    default:
        return (((U16)data[3]) << 2) | (((U16)data[4] & 0xC0) >> 6);
    }
}
/**
* Function: GetNextFrame
* Purpose:  Fill the vector with image data, and pfd with the frame descriptor.
*/
void 
CPxLCamera::GetNextFrame(std::vector<byte>& buf, FRAME_DESC* const pfd, const int discardFrames /*=0*/, const int avgFrames /*=1*/)
{
	CRect roi = GetFeatureRect(FEATURE_ROI);
	PixelAddressing pa = GetPixelAddressing();

	// Make sure that the stream is started, and then restored to its current 
	// state when the function exits (even in the event of an exception).
	CStreamState tempSS(this, START_STREAM);

	// Calculate how big our buffer needs to be.
	int fmt = static_cast<int>(GetFeatureValue(FEATURE_PIXEL_FORMAT));
	int bufSize = (int) ((float) DEC_SIZE(roi.Width(), pa.x) 
				          * (float) DEC_SIZE(roi.Height(), pa.y) 
				          * GetPixelFormatSize(fmt));
    // compensate for interleaving.  This will return 0.0 if not supported (and is therefore not interleaved).
    BOOL interleaved = GetFeatureValue(FEATURE_GAIN_HDR) == FEATURE_GAIN_HDR_MODE_INTERLEAVED;
    if (interleaved) bufSize *= 2;

	buf.resize(bufSize);

	// If there is an OVERLAY_FRAME callback set, then calls to PxLGetNextFrame
	// are not allowed. We need to temporarily remove any frame callback that
	// is in place.
	PxLApiCallback frameCallback = m_frameCallback;
	void* frameCallbackParam = m_frameCallbackParam;
	if (frameCallback != NULL)
		this->SetCallback(OVERLAY_FRAME, NULL, NULL);

	// Discard frames if required. Normally, the first frame out of the camera
	// after the stream is started is overexposed. If the stream was started just
	// before calling this function, the discardFrames parameter should be non-zero.
	int i;
	for (i = 0; i < discardFrames; i++)
	{
		PxLGetNextFrame(m_handle, bufSize, &buf[0], pfd); // ignore errors
	}

	if (avgFrames > 1)
	{
		// Average over multiple frames. Allocate a buffer to hold the accumulated data.
		std::vector<double> tempBuffer(buf.size(), 0.0);
		for (i = 0; i < avgFrames; i++)
		{
			PXL_ERROR_CHECK(PxLGetNextFrame(m_handle, bufSize, &buf[0], pfd));
			if (Is16BitFormat(fmt))
			{
				U16* pbuf = reinterpret_cast<U16*>(&buf[0]);
				int npixels = buf.size() / 2;
				for (int j = 0; j < npixels; j++)
					tempBuffer[j] += ByteSwapU16(pbuf[j]);
			} else if (Is12BitPackedFormat(fmt)) {
			    int nbytes = buf.size();
				int npixels = static_cast<int> (static_cast<float>(nbytes) / 1.5f);
				U16 packedPixel;
				int byteCtr, pixelCtr;
				for (byteCtr = 0, pixelCtr = 0; pixelCtr < npixels; pixelCtr+=2, byteCtr+=3)
				{
				   packedPixel = *(reinterpret_cast<U16*>(&buf[byteCtr])); 
				   tempBuffer[pixelCtr] += Unpack12Bit (packedPixel, true);
				   packedPixel = *(reinterpret_cast<U16*>(&buf[byteCtr+1]));
				   tempBuffer[pixelCtr+1] += Unpack12Bit (packedPixel, false);
				}
			} else if (Is12BitgPackedFormat(fmt)) {
			    int nbytes = buf.size();
				int npixels = static_cast<int> (static_cast<float>(nbytes) / 1.5f);
				int byteCtr, pixelCtr;
				for (byteCtr = 0, pixelCtr = 0; pixelCtr < npixels; pixelCtr+=2, byteCtr+=3)
				{
				   tempBuffer[pixelCtr] += Unpack12Bitg (&buf[byteCtr], true);
				   tempBuffer[pixelCtr+1] += Unpack12Bitg (&buf[byteCtr], false);
				}
			} else if (Is10BitgPackedFormat(fmt)) {
			    int nbytes = buf.size();
				int npixels = static_cast<int> (static_cast<float>(nbytes) / 1.25f);
				int byteCtr, pixelCtr;
				for (byteCtr = 0, pixelCtr = 0; pixelCtr < npixels; pixelCtr+=4, byteCtr+=5)
				{
				   tempBuffer[pixelCtr] += Unpack10Bitg (&buf[byteCtr], 0);
				   tempBuffer[pixelCtr+1] += Unpack10Bitg (&buf[byteCtr], 1);
				   tempBuffer[pixelCtr+2] += Unpack10Bitg (&buf[byteCtr], 2);
				   tempBuffer[pixelCtr+3] += Unpack10Bitg (&buf[byteCtr], 3);
				}
			} else {
			    // 8 bit
				for (int j = 0; j < buf.size(); j++)
					tempBuffer[j] += buf[j];
			}
		}
		// Divide, round off, byteswap again if neccesary, and copy back into the buffer.
		if (Is16BitFormat(fmt))
		{
			U16* pbuf = reinterpret_cast<U16*>(&buf[0]);
			int npixels = buf.size() / 2;
			for (i = 0; i < npixels; i++)
				pbuf[i] = ByteSwapU16( round_to<U16>(tempBuffer[i] / avgFrames) );
		} else if (Is12BitPackedFormat(fmt)) {
		    int nbytes = buf.size();
			int npixels = static_cast<int> (static_cast<float>(nbytes) / 1.5f);
			U16 unpackedPixel1, unpackedPixel2;
			U8* pBuf = &buf[0];
			for (i = 0; i < npixels; i+=2)
			{
				unpackedPixel1 = round_to<U16>(tempBuffer[i] / avgFrames);
				unpackedPixel2 = round_to<U16>(tempBuffer[i+1] / avgFrames);
				Pack12Bit (unpackedPixel1, unpackedPixel2, pBuf);
				pBuf+=3;
			}
		} else if (Is12BitgPackedFormat(fmt)) {
		    int nbytes = buf.size();
			int npixels = static_cast<int> (static_cast<float>(nbytes) / 1.5f);
			U16 unpackedPixel1, unpackedPixel2;
			U8* pBuf = &buf[0];
			for (i = 0; i < npixels; i+=2)
			{
				unpackedPixel1 = round_to<U16>(tempBuffer[i] / avgFrames);
				unpackedPixel2 = round_to<U16>(tempBuffer[i+1] / avgFrames);
				Pack12Bitg (unpackedPixel1, unpackedPixel2, pBuf);
				pBuf+=3;
			}
		} else if (Is10BitgPackedFormat(fmt)) {
		    int nbytes = buf.size();
			int npixels = static_cast<int> (static_cast<float>(nbytes) / 1.25f);
			U16 unpackedPixel1, unpackedPixel2, unpackedPixel3, unpackedPixel4;
			U8* pBuf = &buf[0];
			for (i = 0; i < npixels; i+=4)
			{
				unpackedPixel1 = round_to<U16>(tempBuffer[i] / avgFrames);
				unpackedPixel2 = round_to<U16>(tempBuffer[i+1] / avgFrames);
				unpackedPixel3 = round_to<U16>(tempBuffer[i+2] / avgFrames);
				unpackedPixel4 = round_to<U16>(tempBuffer[i+3] / avgFrames);
				Pack10Bitg (unpackedPixel1, unpackedPixel2, unpackedPixel3, unpackedPixel4, pBuf);
				pBuf += 5;
			}
		} else {
		    // 8 bit
			for (i = 0; i < buf.size(); i++)
				buf[i] = round_to<U8>(tempBuffer[i] / avgFrames);
		}
	}
	else
	{
		// Not averaging. Just capture directly into the buffer.
		PXL_ERROR_CHECK(PxLGetNextFrame(m_handle, bufSize, &buf[0], pfd));
	}

	// Restore the callback:
	if (frameCallback != NULL)
		this->SetCallback(OVERLAY_FRAME, frameCallbackParam, frameCallback);
}


/**
* Function: GetClip
* Purpose:  
*/
void 
CPxLCamera::GetClip(const U32 uNumFrames, LPCSTR fileName, ClipTerminationCallback termCallback)
{
    PXL_ERROR_CHECK(PxLGetClip(m_handle, uNumFrames, fileName, termCallback));
}

/**
* Function: GetEncodedClip
* Purpose:  
*/
void 
CPxLCamera::GetEncodedClip(const U32 uNumFrames, const U32 uClipDecimationFactor, LPCSTR fileName,  
                           float pbFrameRate, int pbBitRate, U32 uEncoding, ClipTerminationCallback termCallback)
{
    CLIP_ENCODING_INFO clipInfo;

    clipInfo.uStreamEncoding = uEncoding;
    clipInfo.uDecimationFactor = uClipDecimationFactor;
    clipInfo.playbackFrameRate = pbFrameRate;
    clipInfo.playbackBitRate = pbBitRate;

    PXL_ERROR_CHECK(PxLGetEncodedClip(m_handle, 
                                   uNumFrames,
                                   fileName, 
                                   &clipInfo, 
                                   termCallback));
}

/**
* Function: SetCallback
* Purpose:  
*/
void 
CPxLCamera::SetCallback(const int usageFlags, void* context, PxLApiCallback callback)
{
	// If we register a callback for FORMAT_IMAGE or FORMAT_CLIP, we have to 
	// specify a camera handle of 0 because these are API-global (i.e. not camera-specific).
	// See the documentation for PxLSetCallback for more info.
	HANDLE hCamera = m_handle;
	if ((OVERLAY_FORMAT_IMAGE == usageFlags) || (OVERLAY_FORMAT_CLIP == usageFlags)) {
		hCamera = 0;
	}

	// Temporary fix until the API is fixed
	if (OVERLAY_FORMAT_CLIP == usageFlags) {
		PXL_RETURN_CODE rc = PxLSetCallback(hCamera, usageFlags, context, callback);
		if ((ApiInvalidHandleError == rc) && (0 == hCamera)) {
			// Bug in the api - ignore
		} else {
			PxLErrorCheck(rc, "PxLSetCallback failed");
		}
	} else {
		PXL_ERROR_CHECK(PxLSetCallback(hCamera, usageFlags, context, callback));
	}


	// Keep track of which callbacks are in place. There is no way to query the
	// API for this information, so we need to keep track of it ourselves. See
	// CPxLCamera::GetNextFrame for one reason why we need to retain this info.
	if ((usageFlags & OVERLAY_FRAME) != 0)
	{
		m_frameCallback = callback;
		m_frameCallbackParam = context;
	}
	if ((usageFlags & OVERLAY_PREVIEW) != 0)
	{
		m_previewCallback = callback;
		m_previewCallbackParam = context;
	}
	if ((usageFlags & OVERLAY_FORMAT_IMAGE) != 0)
	{
		m_imageCallback = callback;
		m_imageCallbackParam = context;
	}
	if ((usageFlags & OVERLAY_FORMAT_CLIP) != 0)
	{
		m_clipCallback = callback;
		m_clipCallbackParam = context;
	}
}


/**
* Function: GetCallback
* Purpose:  
*/
PxLApiCallback 
CPxLCamera::GetCallback(const int usageFlag)
{
	switch (usageFlag)
	{
	case OVERLAY_PREVIEW:
		return m_previewCallback;
	case OVERLAY_FORMAT_IMAGE:
		return m_imageCallback;
	case OVERLAY_FORMAT_CLIP:
		return m_clipCallback;
	default:
		return NULL;
	}
}


/**
* Function: GetCallbackParam
* Purpose:  
*/
void*
CPxLCamera::GetCallbackParam(const int usageFlag)
{
	switch (usageFlag)
	{
	case OVERLAY_PREVIEW:
		return m_previewCallbackParam;
	case OVERLAY_FORMAT_IMAGE:
		return m_imageCallbackParam;
	case OVERLAY_FORMAT_CLIP:
		return m_clipCallbackParam;
	default:
		return NULL;
	}
}


/**
* Function: SetEventCallback
* Purpose:  
*/
void 
CPxLCamera::SetEventCallback(const int eventId, void* context, EventCallback callback)
{
    try
    {
        PxLSetEventCallback(m_handle, eventId, context, callback);
    }
    catch (PxLException const& e)
	{
		int rc = e.GetReturnCode();
		if (rc != ApiNotSupportedError
			&& rc != ApiThisEventNotSupported
			)
		{
			// Unexpected error
            OnApiError(e);
		}
    }
}


/******************************************************************************
* Querying Camera for its capabilities
******************************************************************************/

/**
* Function: FeatureSupportsFlag
* Purpose:  Return true if the feature supports the given FEATURE_FLAG.
*/
bool 
CPxLCamera::FeatureSupportsFlag(const U32 featureId, const U32 flag)
{
	CAMERA_FEATURE *pFeature = GetFeaturePtr(featureId);
	if (pFeature == NULL) {
		return false;
	}

	return (pFeature->uFlags & flag) != 0;
}


/**
* Function: FeatureSupported
* Purpose:  Return true if the feature exists on the current camera.
*/
bool 
CPxLCamera::FeatureSupported(const U32 featureId)
{
    return FeatureSupportsFlag(featureId, FEATURE_FLAG_PRESENCE);
}


/**
* Function: FeatureSupportsManual
* Purpose:  Return true if the feature supports Manual setting.
*/
bool 
CPxLCamera::FeatureSupportsManual(const U32 featureId)
{
	return FeatureSupported(featureId) && FeatureSupportsFlag(featureId, FEATURE_FLAG_MANUAL);
}


/**
* Function: FeatureSupportsAuto
* Purpose:  Return true if the feature supports Auto mode.
*           In Auto mode, the camera continuously updates the value of the
*           feature (until it is set back into Manual mode).
*/
bool 
CPxLCamera::FeatureSupportsAuto(const U32 featureId)
{
	return FeatureSupported(featureId) && FeatureSupportsFlag(featureId, FEATURE_FLAG_AUTO);
}

/**
* Function: FeatureSupportsOnePush
* Purpose:  Return true if the feature supports One Push setting.
*           One Push setting is when the camera auto-sets the value of the
*           feature, then immediately goes back into manual mode.
*/
bool 
CPxLCamera::FeatureSupportsOnePush(const U32 featureId)
{
	return FeatureSupported(featureId) && FeatureSupportsFlag(featureId, FEATURE_FLAG_ONEPUSH);
}


/**
* Function: FeatureSupportsAutoLimits
* Purpose:  Return true if the feature supports limits for OnePush or Auto mode.
*           That is, both onePush and Auto modes are limited to convergence to values
*           within a specified range.

* IMPORTANT NOTE:
*      This only works for those features that ordianrily, take 1 parameter to get/set, but
*      allow the user to supply more than 1 parameter when they are getting/seeting auto limits.
*/
bool 
CPxLCamera::FeatureSupportsAutoLimits(const U32 featureId)
{
	if (!FeatureSupported(featureId)) return false;
	CAMERA_FEATURE *pFeature = GetFeaturePtr(featureId);
	if (pFeature == NULL) return false;

    switch (featureId) 
    {
    case FEATURE_SHUTTER:
    case FEATURE_FOCUS:
        return (pFeature->uNumberOfParameters > 1);
    default:
        return false;
    }
}

/**
* Function: FeatureSupportsOnOff
* Purpose:  
*/
bool 
CPxLCamera::FeatureSupportsOnOff(const U32 featureId)
{
	return FeatureSupported(featureId) && FeatureSupportsFlag(featureId, FEATURE_FLAG_OFF);
}

/**
* Function: FeatureSupportsDescriptor
* Purpose:  Return true if the feature can be set to a different value in
*           different descriptors, and false if the feature only supports
*           setting a single value that applies to all descriptors.
*/
bool 
CPxLCamera::FeatureSupportsDescriptor(const U32 featureId)
{
	return FeatureSupported(featureId) && FeatureSupportsFlag(featureId, FEATURE_FLAG_DESC_SUPPORTED);
}

/**
* Function: FeatureSupportsAssertLowerLimit
* Purpose:  Return true if the feature can (re)assert the lower limit.  This is generally supported
*           by features implemented mechanically, with electric motors.
*/
bool 
CPxLCamera::FeatureSupportsAssertLowerLimit(const U32 featureId)
{
	return FeatureSupported(featureId) && FeatureSupportsFlag(featureId, FEATURE_FLAG_ASSERT_LOWER_LIMIT);
}

/**
* Function: FeatureSupportsAssertUpperLimit
* Purpose:  Return true if the feature can (re)assert the upper limit.  This is generally supported
*           by features implemented mechanically, with electric motors.
*/
bool 
CPxLCamera::FeatureSupportsAssertUpperLimit(const U32 featureId)
{
	return FeatureSupported(featureId) && FeatureSupportsFlag(featureId, FEATURE_FLAG_ASSERT_UPPER_LIMIT);
}

/**
* Function: FeatureSettableWhileStreaming
* Purpose:  Return true if the feature exists on the current camera and may be 
*           set while the camera is streaming
*/
bool 
CPxLCamera::FeatureSettableWhileStreaming(const U32 featureId)
{
    return FeatureSupported(featureId) && FeatureSupportsFlag(featureId, FEATURE_FLAG_SETTABLE_WHILE_STREAMING);
}

/**
* Function: FeatureSupportedInController
* Purpose:  Return true if the feature is implemented by a controller, as opposed to in the camera.
*/
bool 
CPxLCamera::FeatureSupportedInController(const U32 featureId)
{
    return FeatureSupportsFlag(featureId, FEATURE_FLAG_CONTROLLER);
}

/**
* Function: FeatureSupportedInController
* Purpose:  Return true if the feature is implemented by a controller, as opposed to in the camera.
*/
bool 
CPxLCamera::FeatureEmulated(const U32 featureId)
{
    return FeatureSupportsFlag(featureId, FEATURE_FLAG_EMULATION);
}

/**
* Function: FeatureUsesAutoRoi
* Purpose:  Return true if the feature uses the AutoRoi for oneTime and Continuous auto operations.
*/
bool 
CPxLCamera::FeatureUsesAutoRoi(const U32 featureId)
{
    return FeatureSupportsFlag(featureId, FEATURE_FLAG_USES_AUTO_ROI);
}

/**
* Function: GetFeatureMinVal
* Purpose:  Return the minimum supported value of the specified parameter
*           of the specified feature, for the active camera.
*/
std::pair<float,float> 
CPxLCamera::GetFeatureLimits(const U32 featureId, int paramNo /*=0*/)
{
	// If the feature is one whose Min/Max range is not fixed, we re-query the
	// camera for the values before returning them.
	if (IsDynamicFeature(featureId))
	{
		ReloadFeature(featureId);
	}

	CAMERA_FEATURE *pFeature = GetFeaturePtr(featureId);

    if ((pFeature != NULL) &&
        (pFeature->pParams != NULL) &&
        (paramNo < pFeature->uNumberOfParameters)) {
	    return std::make_pair(
			pFeature->pParams[paramNo].fMinValue * GetUnitsMuliplier(featureId, paramNo),
			pFeature->pParams[paramNo].fMaxValue * GetUnitsMuliplier(featureId, paramNo));
    } else {
        // Opps -- Can't get the limits; perhaps the camera was unplugged?? -- Return a default pair
        return std::pair<float,float>(0.0,0.0);
    }
}


/**
* Function: GetFeatureMinVal
* Purpose:  Return the minimum supported value of the specified parameter
*           of the specified feature, for the active camera.
*/
float
CPxLCamera::GetFeatureMinVal(const U32 featureId, const int paramNo /*=0*/)
{
	// Note: There are some places where GetFeatureMinVal and GetFeatureMaxVal
	// will be called one after the other, and we end up calling ReloadFeature
	// twice in a row. This is a bit of a waste of time, but I don't find that
	// it's enough to matter, so I have left it as is.
	return GetFeatureLimits(featureId, paramNo).first;
}


/**
* Function: GetFeatureMaxVal
* Purpose:  Return the maximum supported value of the specified parameter
*           of the specified feature, for the active camera.
*/
float 
CPxLCamera::GetFeatureMaxVal(const U32 featureId, const int paramNo /*=0*/)
{
	// See note above, in GetFeatureMinVal
	return GetFeatureLimits(featureId, paramNo).second;
}


/**
* Function: GetFeatureParamCount
* Purpose:  Query the API to find the number of parameters that a feature has.
*           The return value is cached for efficiency.
*/
U32 
CPxLCamera::GetFeatureParamCount(const U32 featureId)
{
	CAMERA_FEATURE *pFeature = GetFeaturePtr(featureId);
	ASSERT(pFeature != NULL);

	// Lookup table is special. It really has a lot of parameters (eg: 1024 for
	// the PL-A741), but it reports this fact differently than other features.
	if (featureId == FEATURE_LOOKUP_TABLE) {
		return static_cast<U32>(pFeature->pParams[0].fMaxValue);
	}

	return pFeature->uNumberOfParameters;
}


/**
* Function: GetSupportedPixelFormats
* Purpose:  Return a reference to the vector that contains all the values of
*           FEATURE_PIXEL_FORMAT that are supported by the current camera.
*/
std::vector<int> const& 
CPxLCamera::GetSupportedPixelFormats(void)
{
	if (m_supportedPixelFormats.size() == 0)
	{
		// There *must* be at least one supported pixel format... (otherwise,
		// how would you get any data out of the camera?)
		try
		{
			LoadSupportedPixelFormats();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedPixelFormats;
}


/**
* Function: LoadSupportedPixelFormats
* Purpose:  Fill a vector with all the values of FEATURE_PIXEL_FORMAT that are
*           supported by the current camera.
*/
void 
CPxLCamera::LoadSupportedPixelFormats(void)
{
	// NOTE: Pixel Formats do not constitute a logically consecutive sequence
	// of values. In other words, just because the camera says that the Min
	// and Max Pixel Formats that it supports are X and Y respectively, it
	// does not mean that *all* the values between X and Y will be supported
	// too. We have to explicitly test whether the camera supports the
	// values between X and Y by trying to actually set the PIXEL_FORMAT
	// feature, and checking for an error return code.

	if (!FeatureSupported(FEATURE_PIXEL_FORMAT)) {
		return;
	}

	std::vector<int>& formats = m_supportedPixelFormats;
	formats.clear();

	// Keep track of what the pixel format is to start with.
	float currentFormat = GetFeatureValue(FEATURE_PIXEL_FORMAT);

	// The video stream may need to be stopped to change this feature.
	STOP_STREAM_IF_REQUIRED(FEATURE_PIXEL_FORMAT)

	// Temporarily prevent posting of messages to the GUI to update its display.
	TempVal<bool> _temp_val(m_bNoPostViewChange, true);

	// Try to set the pixel format to each value in the legal range, and add
	// to the vector only those values that we can set without error.
	for (int fmt = static_cast<int>(GetFeatureMinVal(FEATURE_PIXEL_FORMAT));
			fmt <= static_cast<int>(GetFeatureMaxVal(FEATURE_PIXEL_FORMAT));
			fmt++)
	{
		// Skip over the specific bayer formats - eg: PIXEL_FOMAT_BAYER8_RGGB, etc.
		// These specific formats only exist for reporting purposes - the user is
		// only supposed to be able to set the camera into BAYER format, and the 
		// camera/API determines which specific variant of the BAYER pattern is
		// actually output.
		// In other words, we only want to put BAYER8 in the combo box, and not
		// any of BAYER8_GRGB, BAYER8_RGGB, etc.
		if (   fmt == PIXEL_FORMAT_BAYER8_BGGR
			|| fmt == PIXEL_FORMAT_BAYER8_GBRG
			|| fmt == PIXEL_FORMAT_BAYER8_RGGB
			|| fmt == PIXEL_FORMAT_BAYER16_BGGR
			|| fmt == PIXEL_FORMAT_BAYER16_GBRG
			|| fmt == PIXEL_FORMAT_BAYER16_RGGB
			|| fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED
			|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED
			|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED
			|| fmt == PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST
			|| fmt == PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST
			|| fmt == PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST
            || fmt == PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST
			|| fmt == PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST
			|| fmt == PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST)
		{
			continue;
		}

		try
		{
			SetFeatureValue(FEATURE_PIXEL_FORMAT, static_cast<float>(fmt), 0, true);
			formats.push_back(fmt);
		}
		catch (PxLException const& e)
		{
			int rc = e.GetReturnCode();
			if (rc == ApiInvalidParameterError
				|| rc == ApiNotSupportedError
				|| rc == ApiHardwareError // XXX
				)
			{
				// Assume the error is because fmt is not a supported pixel format.
				// Continue looping, and do not add fmt to the vector.
				continue;
			}
			//OnApiError(e);
			// Assume that some other (possibly catastrophic) error has occured.
			// Propagate it to the caller.
            //   2019-09-12
            //     Naw, this is most likely just another 'newer' error code, just mark
            //     this one as unsupported too
			//throw;
            continue;
		}
	}

	// Set the pixel format back to what it was.
	SetFeatureValue(FEATURE_PIXEL_FORMAT, currentFormat);
}


/**
* Function: GetSupportedDecimationValues
* Purpose:  Return a reference to the vector that contains all the values of
*           the mode parameter of FEATURE_PIXEL_ADDRESSING that are supported by
*           the current camera.
*/
std::vector<int> const& 
CPxLCamera::GetSupportedDecimationModeValues(void)
{
	if (m_supportedDecimationModes.size() == 0)
	{
		try
		{
			LoadSupportedDecimations();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedDecimationModes;
}


/**
* Function: GetSupportedXDecimationValues
* Purpose:  Return a reference to the vector that contains all the values of
*           the X (horizontal) value parameter of FEATURE_PIXEL_ADDRESSING that are supported by
*           the current camera.
*/
std::vector<int> const& 
CPxLCamera::GetSupportedXDecimationValues(void)
{
	if (m_supportedXDecimations.size() == 0) // Decimation of 1, at least, is *always* supported.
	{
		try
		{
			LoadSupportedDecimations();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedXDecimations;
}

/**
* Function: GetSupportedYDecimationValues
* Purpose:  Return a reference to the vector that contains all the values of
*           the Y (vertical) value parameter of FEATURE_PIXEL_ADDRESSING that are supported by
*           the current camera.
*/
std::vector<int> const& 
CPxLCamera::GetSupportedYDecimationValues(void)
{
	if (m_supportedYDecimations.size() == 0) // Decimation of 1, at least, is *always* supported.
	{
		try
		{
			LoadSupportedDecimations();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedYDecimations;
}

/**
* Function: SupportsAsymmetricPixelAddressing
* Purpose:  Return true if this camera supports asymetric pixel addressing
*/
bool 
CPxLCamera::SupportsAsymmetricPixelAddressing(void)
{
	if (m_supportedXDecimations.size() == 0) // Decimation of 1, at least, is *always* supported.
	{
		try
		{
			LoadSupportedDecimations();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportsAsymmetricPa;
}


/**
* Function: LoadSupportedDecimations
* Purpose:  Fill a vector with all the values of FEATURE_PIXEL_ADDRESSING that are
*           supported by the current camera.
*/
void 
CPxLCamera::LoadSupportedDecimations(void)
{
	// NOTE: See the comment at the start of LoadSupportedPixelFormats(), above.

	if (!FeatureSupportsManual(FEATURE_PIXEL_ADDRESSING)) {
		return;
	}

	std::vector<int>& xDecs = m_supportedXDecimations;
	std::vector<int>& yDecs = m_supportedYDecimations;
	std::vector<int>& modes = m_supportedDecimationModes;
	xDecs.clear();
	yDecs.clear();
	modes.clear();
	xDecs.push_back(1); // 1 (no decimation) is *always* supported.
	yDecs.push_back(1); // 1 (no decimation) is *always* supported.

	// Keep track of what the decimation is to start with.
	PixelAddressing currentPa = GetPixelAddressing();
	m_supportsAsymmetricPa = currentPa.supportAsymmetry;

	// The video stream must be stopped to change this feature.
	STOP_STREAM_IF_REQUIRED(FEATURE_PIXEL_ADDRESSING)

	// Temporarily prevent posting of messages to the GUI to update its display.
	TempVal<bool> _temp_val(m_bNoPostViewChange, true);

	if (currentPa.supportAsymmetry)
	{
	    // This camera supports asymmetric pixel addressing.  Using a y pa factor
	    // of 1, figure out what x pa factors are supported
	    PixelAddressing pa;
	    pa.Mode = currentPa.Mode;
	    pa.y = 1;
	    for (pa.x = max(static_cast<int>(GetFeatureMinVal(FEATURE_PIXEL_ADDRESSING, FEATURE_PIXEL_ADDRESSING_PARAM_X_VALUE)), 2);
			 pa.x <= GetFeatureMaxVal(FEATURE_PIXEL_ADDRESSING, FEATURE_PIXEL_ADDRESSING_PARAM_X_VALUE); pa.x++)
	    {
		    try
		    {
			    SetPixelAddressing(pa);
    			xDecs.push_back(pa.x);
    		}
		    catch (PxLException const& e)
		    {
			    int rc = e.GetReturnCode();
			    if (rc == ApiInvalidParameterError
				    || rc == ApiNotSupportedError
				    || rc == ApiHardwareError) // XXX - the 682 is still returning HardwareError instead of InvalidParameter
			    {
				    // Not a supported decimation value. Keep trying other values.
				    continue;
			    }
			    // Propogate errors other than Invalid Parameter - they may
			    // indicate a more serious error condition.
			    throw;
		    }
	    }
	    // Now, using a x pa factor of 1, figure out what y pa factors are supported
	    pa.x = 1;
	    for (pa.y = max(static_cast<int>(GetFeatureMinVal(FEATURE_PIXEL_ADDRESSING, FEATURE_PIXEL_ADDRESSING_PARAM_Y_VALUE)), 2);
			 pa.y <= GetFeatureMaxVal(FEATURE_PIXEL_ADDRESSING, FEATURE_PIXEL_ADDRESSING_PARAM_Y_VALUE); pa.y++)
	    {
		    try
		    {
			    SetPixelAddressing(pa);
    			yDecs.push_back(pa.y);
    		}
		    catch (PxLException const& e)
		    {
			    int rc = e.GetReturnCode();
			    if (rc == ApiInvalidParameterError
				    || rc == ApiNotSupportedError
				    || rc == ApiHardwareError) // XXX - the 682 is still returning HardwareError instead of InvalidParameter
			    {
				    // Not a supported decimation value. Keep trying other values.
				    continue;
			    }
			    // Propogate errors other than Invalid Parameter - they may
			    // indicate a more serious error condition.
			    throw;
		    }
	    }
	} else {
	    // This is the easy case where we only support symmetric pixel addressing.
	    // Try setting the pixel addressing for ach value in the legal range, and add
	    // to the vector only those values that we can set without error.
	    for (int dec = max(static_cast<int>(GetFeatureMinVal(FEATURE_PIXEL_ADDRESSING)), 2);
			    dec <= GetFeatureMaxVal(FEATURE_PIXEL_ADDRESSING); dec++)
	    {
		    try
		    {
			    SetFeatureValue(FEATURE_PIXEL_ADDRESSING, static_cast<float>(dec), FEATURE_PIXEL_ADDRESSING_PARAM_VALUE, false);
    			xDecs.push_back(dec);
				yDecs.push_back(dec);
	        }
		    catch (PxLException const& e)
		    {
			    int rc = e.GetReturnCode();
			    if (rc == ApiInvalidParameterError
				    || rc == ApiNotSupportedError
				    || rc == ApiHardwareError) // XXX - the 682 is still returning HardwareError instead of InvalidParameter
			    {
				    // Not a supported decimation value. Keep trying other values.
				    continue;
			    }
			    // Propogate errors other than Invalid Parameter - they may
			    // indicate a more serious error condition.
			    throw;
		    }
	    }
	}

	// Some older cameras (eg: 741) consider the decimation feature to have only one parameter.
	// For these cameras we just add the original Decimation mode to the list of
	// supported modes. (We have confusing terminology here - we use "Decimation" to
	// refer to all the frame-reduction techniques, one of which is also called
	// "Decimation" - the others are "Binning" and "Averaging").
	if (this->GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING) < 2)
	{
		modes.push_back(PIXEL_ADDRESSING_MODE_DECIMATE);
	}
	else
	{
		// Test for the supported modes
		for (int mode = (int)GetFeatureMinVal(FEATURE_PIXEL_ADDRESSING, FEATURE_PIXEL_ADDRESSING_PARAM_MODE);
				mode <= (int)GetFeatureMaxVal(FEATURE_PIXEL_ADDRESSING, FEATURE_PIXEL_ADDRESSING_PARAM_MODE);
				++mode)
		{
			try
			{
				SetFeatureValue(FEATURE_PIXEL_ADDRESSING, static_cast<float>(mode), FEATURE_PIXEL_ADDRESSING_PARAM_MODE, false);
				modes.push_back(mode);
			}
			catch (PxLException const& e)
			{
				int rc = e.GetReturnCode();
				if (rc == ApiInvalidParameterError
					|| rc == ApiNotSupportedError)
				{
					// Not a supported decimation value. Keep trying other values.
					continue;
				}
				// Propogate errors other than Invalid Parameter - they may
				// indicate a more serious error condition.
				throw;
			}
		}

	}

	// Set the decimation back to what it was.
	SetPixelAddressing (currentPa);
}


/**
* Function: GetSupportedTriggerModes
* Purpose:  
*/
std::vector<int> const& 
CPxLCamera::GetSupportedTriggerModes(void)
{
	if (FeatureSupported(FEATURE_TRIGGER) && m_supportedTriggerModes.size() == 0)
	{
		try
		{
			// There must be at least one supported mode - load them.
			LoadSupportedTriggerParams();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedTriggerModes;
}


/**
* Function: GetSupportedTriggerTypes
* Purpose:  
*/
std::vector<int> const& 
CPxLCamera::GetSupportedTriggerTypes(void)
{
	if (FeatureSupported(FEATURE_TRIGGER) && m_supportedTriggerTypes.size() == 0)
	{
		try
		{
			// There must be at least one supported type - load them.
			LoadSupportedTriggerParams();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedTriggerTypes;
}


/**
* Function: LoadSupportedTriggerParams
* Purpose:  
*/
void 
CPxLCamera::LoadSupportedTriggerParams(void)
{
	// NOTE: See the comment at the start of LoadSupportedPixelFormats(), above.

	// XXX - this routine is getting a bit fragile - I'm tempted to just hard-code
	//       the supported trigger modes and types for each camera model, like 
	//       I did for the gain values. It's possible that this routine could
	//       fail to correctly identify the supported modes and types because
	//       the polarity, or the delay, or the parameter were disallowed - we
	//       do not know, when we get an InvalidParameterError, which parameter
	//       was invalid. Anyway, I'm leaving it as is for now.

	if (!FeatureSupportsManual(FEATURE_TRIGGER)) {
		return;
	}

	m_supportedTriggerModes.clear();
	m_supportedTriggerTypes.clear();

	// Keep track of what state the trigger is in to start with.
	U32 currentFlags = 0;
	Trigger currentTrigger = GetTrigger(&currentFlags);

	// Just in case the video stream needs to be stopped to change this feature:
	STOP_STREAM_IF_REQUIRED(FEATURE_TRIGGER)

	// Try to set the trigger mode to each value in the legal range, and add
	// to the vector only those values that we can set without error.
	// We set the rest of the trigger parameters to values that should work
	// in combination with *any* of the modes.
	Trigger trigger;
	trigger.Polarity = POLARITY_POSITIVE;
	trigger.Delay = 0.0f;
	trigger.Parameter = 2.0f;

	std::set<int> okModes;
	std::set<int> okTypes;

	// Temporarily prevent posting of messages to the GUI to update its display.
	TempVal<bool> _temp_val(m_bNoPostViewChange, true);

    // first figure out what types are supported -- All camera support at lease mode 0
    trigger.Mode = TRIGGER_MODE_0;
    for (int type = static_cast<int>(GetFeatureMinVal(FEATURE_TRIGGER, FEATURE_TRIGGER_PARAM_TYPE));
			type <= GetFeatureMaxVal(FEATURE_TRIGGER, FEATURE_TRIGGER_PARAM_TYPE);
			++type)
	{
		trigger.Type = type;
		try
		{
			SetTrigger(trigger);
			okTypes.insert(type);
		}
		catch (PxLException const& e)
		{
			int rc = e.GetReturnCode();
			if (rc == ApiInvalidParameterError
				|| rc == ApiNotSupportedError
				|| rc == ApiHardwareError // XXX - the 682 is still returning HardwareError instead of InvalidParameter
                || rc == ApiGpiHardwareTriggerConflict) // Can't support this type of trigger while GPI is enabled.
			{
				// Not a supported mode and/or type.
				continue;
			}
			// Handle other errors normally.
			OnApiError(e);
			return;
		}
    }


    // Now, figure what what modes are supported.
    if (!okTypes.empty())
    {
        okModes.insert (TRIGGER_MODE_0);
        // All of the other modes require support for either hardware or action
        if (okTypes.find(TRIGGER_TYPE_HARDWARE) != okTypes.end())
        {
            trigger.Type = TRIGGER_TYPE_HARDWARE;
        } else if (okTypes.find(TRIGGER_TYPE_ACTION) != okTypes.end()) {
            trigger.Type = TRIGGER_TYPE_ACTION;
        } else {
            trigger.Type = TRIGGER_TYPE_SOFTWARE;
        }
		for (int mode = TRIGGER_MODE_1;
			 mode <= GetFeatureMaxVal(FEATURE_TRIGGER, FEATURE_TRIGGER_PARAM_MODE);
			 ++mode)
		{
			trigger.Mode = mode;
			try
			{
				SetTrigger(trigger);
				okModes.insert(mode);
			}
			catch (PxLException const& e)
			{
				int rc = e.GetReturnCode();
				if (rc == ApiInvalidParameterError
					|| rc == ApiNotSupportedError
					|| rc == ApiHardwareError // XXX - the 682 is still returning HardwareError instead of InvalidParameter
                    || rc == ApiGpiHardwareTriggerConflict) // Can't support this type of trigger while GPI is enabled.
				{
					// Not a supported mode and/or type.
					continue;
				}
				// Handle other errors normally.
				OnApiError(e);
				return;
			}
        }
    }
    

	m_supportedTriggerTypes.resize(okTypes.size());
	m_supportedTriggerModes.resize(okModes.size());
	std::copy(okTypes.begin(), okTypes.end(), m_supportedTriggerTypes.begin());
	std::copy(okModes.begin(), okModes.end(), m_supportedTriggerModes.begin());

	// Set the trigger state back to what it was.
	SetTrigger(currentTrigger, currentFlags);
}


/**
* Function: GetSupportedGpioModes
* Purpose:  Gets all of the GPIO modes supported.  Note that not all of these modes are necessarily supported
*           on all GPIOs
*/
std::vector<int> const& 
CPxLCamera::GetSupportedGpioModes(void)
{
	if (FeatureSupported(FEATURE_GPIO) && m_supportedGpioModes.size() == 0)
	{
		try
		{
			// There must be at least one supported mode - load them.
			LoadSupportedGpioModes();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedGpioModes;
}


/**
* Function: LoadSupportedGpioModes
* Purpose:  
*/
void 
CPxLCamera::LoadSupportedGpioModes(void)
{
	// NOTE: See the comment at the start of LoadSupportedPixelFormats(), above.

	if (!FeatureSupportsManual(FEATURE_GPIO)) {
		return;
	}

	m_supportedGpioModes.clear();

	// We will use the first GPIO for most of the testing, so keep track of the state of that GPO.
	int  gpoNumber = (int)GetFeatureMinVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX);
	Gpo currentGpo = GetGpo(gpoNumber);

    GpioProfiles gpioProfile = GetGpioProfile();

	// Just in case the video stream needs to be stopped to change this feature:
	STOP_STREAM_IF_REQUIRED(FEATURE_TRIGGER)

	// Try to set the GPIO mode to each value in the legal range, and add
	// to the vector only those values that we can set without error.
	// We set the most of the parameters to values that should work
	// in combination with *any* of the modes.
	Gpo gpo;
	gpo.flags = FEATURE_FLAG_MANUAL;
	gpo.Polarity = POLARITY_POSITIVE;
	// gpo.Parameter1 = ?? See below
	gpo.Parameter2 = 0.01f;
	gpo.Parameter3 = 0.01f;

	// Temporarily prevent posting of messages to the GUI to update its display.
	TempVal<bool> _temp_val(m_bNoPostViewChange, true);

	for (int mode = static_cast<int>(GetFeatureMinVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_MODE));
			mode <= GetFeatureMaxVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_MODE);
			++mode)
	{
        // 
        if (mode == GPIO_MODE_INPUT && gpioProfile == TWO_GPOS_ONE_GPI)
        {
			// NO need to test this one, as we know it is supported
            m_supportedGpioModes.push_back(mode);
            continue;
        }
        gpo.Mode = mode;
		// Issue.Bugzilla.192.  PULSE requires a whole numbers, all other modes specify a time
		gpo.Parameter1 = (GPIO_MODE_PULSE == mode) ? 1.0f : 0.01f;
		try
		{
			SetGpo(gpo, gpoNumber);
			m_supportedGpioModes.push_back(mode);
		}
		catch (PxLException const& e)
		{
			int rc = e.GetReturnCode();
			if (rc == ApiInvalidParameterError
				|| rc == ApiNotSupportedError
				//|| rc == ApiOutOfRangeError
				|| rc == ApiHardwareError) // XXX - API is still returning HardwareError instead of InvalidParameter
			{
				// Not a supported mode and/or type.
				continue;
            } else if (rc == ApiGpiHardwareTriggerConflict) {// Bugzilla.914 - mode is still support if camera says it cant now because
                                                             // the hardware trigger is currently enabled.
    			m_supportedGpioModes.push_back(mode);
                continue;
            }
			// Handle other errors normally.
			OnApiError(e);
			return;
		}
	}

	// Set the gpo state back to what it was.
	SetGpo(currentGpo, gpoNumber);
}


/**
* Function: GetSupportedGains
* Purpose:  
*/
std::vector<float> const& 
CPxLCamera::GetSupportedGains(void)
{
	if (FeatureSupported(FEATURE_GAIN))
	{
		try
		{
			LoadSupportedGains();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedGains;
}


/**
* Function: LoadSupportedGains
* Purpose:  
*/
void 
CPxLCamera::LoadSupportedGains(void)
{
	m_supportedGains = GetSupportedValues(FEATURE_GAIN);
}

/**
* Function: GetSupportedValues
*
* Purpose:  
*
* This only works with single parameter features
*/
std::vector<float>
CPxLCamera::GetSupportedValues(const U32 featureId)
{
	U32 initialFlags;
	U32 numParams = 1;
	float initialValue;
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, featureId, &initialFlags, &numParams, &initialValue));

	std::vector<float> supportedValues;

	try {
		// We know it supports the max and min values
		float min = GetFeatureMinVal(featureId);
		float max = GetFeatureMaxVal(featureId);
		supportedValues.push_back(min);
		supportedValues.push_back(max);
		// Now search for the rest
		BinarySearchForSupportedValues(featureId, initialFlags, supportedValues, min, max);
		std::sort(supportedValues.begin(), supportedValues.end());

	} catch (const PxLException& e) {
		// Not much we can do other than restore to the initial value
		PXL_ERROR_CHECK(PxLSetFeature(m_handle, featureId, initialFlags, numParams, &initialValue));
		throw e;
	}

	// Restore initial settings
	PXL_ERROR_CHECK(PxLSetFeature(m_handle, featureId, initialFlags, numParams, &initialValue));
	return supportedValues;
}

#define MAX_NUMBER_SUPPORTED_VALUES	20 // Higher numbers give more resolution -- but take longer to converge

void
CPxLCamera::BinarySearchForSupportedValues(const U32 featureId, const U32 flags, std::vector<float>& supportedValues, const float min, const float max)
{
	float mid = (min + max) / 2.0f;
	ASSERT(supportedValues.size() >= 2);  // The first two entired must be the min and the max
	float resolution = (supportedValues[1] - supportedValues[0]) / (static_cast<float>(MAX_NUMBER_SUPPORTED_VALUES));
	
	// 2011-07-17 PEC: Limit the 'resolution' of the search.  That is, once the current range is close enough, move on with the search
	if (max-min <= resolution) {
		return;
	}

	// Try the middle gain, and see what the camera adjusted the gain to.
	PXL_ERROR_CHECK(PxLSetFeature(m_handle, featureId, flags, 1, &mid));
	float actualValue = GetFeatureValue(featureId);

	if (std::find(supportedValues.begin(), supportedValues.end(), actualValue) == supportedValues.end()) {
		supportedValues.push_back(actualValue);
		BinarySearchForSupportedValues(featureId, flags, supportedValues, min, actualValue);
		BinarySearchForSupportedValues(featureId, flags, supportedValues, actualValue, max);
	}
}


/**
* Function: GetSupportedColorTempValues
* Purpose:  
*/
std::vector<float> const& 
CPxLCamera::GetSupportedColorTempValues(void)
{
	if (FeatureSupported(FEATURE_COLOR_TEMP) && m_supportedColorTempValues.size() == 0)
	{
		try
		{
			LoadSupportedColorTempValues();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	return m_supportedColorTempValues;
}


/**
* Function: LoadSupportedColorTempValues
*
* If the camera's white balance (aka color temp) is 'snappy' (i.e. when you set 
* a value, it's rounded by the camera down to a supported value) then we can 
* determine the supported values using some trial and error. 
* See GetSupportedValues(). 
*
* To test for snappyness, we take the minimum color temp, add 100, then set the color temp.
* If the camera rounds the value down to the minimum value, it's a snappy feature.
* Why 100? Because there are no colour temps in PixeLINK cameras this close together.
*
* If the feature's not snappy, we'll just empty the supported values vector.
*
*/
void 
CPxLCamera::LoadSupportedColorTempValues(void)
{
	// Is the color temp 'snappy'? 
	U32 flags;
	U32 numParams = 1;
	float initialValue;
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_COLOR_TEMP, &flags, &numParams, &initialValue));

    const float minValue = GetFeatureMinVal(FEATURE_COLOR_TEMP);
	ASSERT(initialValue >= minValue);
	const float testValue = minValue + 100.0f; 
	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_COLOR_TEMP, flags, numParams, &testValue));

	bool isSnappy = false;
	const float currentValue = GetFeatureValue(FEATURE_COLOR_TEMP);
	if (currentValue == minValue) {
		isSnappy = true;
	}
	// Restore settings
	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_COLOR_TEMP, flags, numParams, &initialValue));

	if (isSnappy) {
		m_supportedColorTempValues = GetSupportedValues(FEATURE_COLOR_TEMP);
	} else {
		m_supportedColorTempValues.resize(0);
	}
}


/**
* Function: GetModelNumber
* Purpose:  Return the model number of the camera, based on the Description string.
*/
int 
CPxLCamera::GetModelNumber(void)
{
	static const int models[] = {
		622,
		623,
		682,
		686,
		741,
		742,
		774,
		776,
		781,
		782,
	};
	static const int n_models = sizeof(models) / sizeof(models[0]);

	CAMERA_INFO const* pInfo = this->GetCameraInfo();
	CString desc(&pInfo->Description[0]);
	CString model_string;
	for (int i = 0; i < n_models; i++)
	{
		// Check if it's a PL-A ...
		model_string.Format("PL-A%d", models[i]);
		if (desc.Find(model_string) != -1) {
			return models[i];
		}
		// Now, check for PL-Bs
		model_string.Format("PL-B%d", models[i]);
		if (desc.Find(model_string) != -1) {
			return models[i];
		}
		
	}
	return -1;
}

/**
* Function: GetModelGeneration
* Purpose:  Return the gneration type of the camera, based on the Description string
*/
int 
CPxLCamera::GetModelGeneration(void)
{
	CAMERA_INFO const* pInfo = this->GetCameraInfo();
	CString desc(&pInfo->Description[0]);
	
	if (desc.Find("PL-A") != -1) {
			return PL_A;
	}
	if (desc.Find("PL-B") != -1) {
			return PL_B;
	}
	return -1;
}

/**
* Function: GetHWVersion
* Purpose:  Return the Hardware Version of the camera, which is the last
*           part of the FPGA version.
*/
int 
CPxLCamera::GetHWVersion(void)
{
	int dummy, hwver = 0;
	CAMERA_INFO const * const pCameraInfo = this->GetCameraInfo();
	ASSERT(NULL != pCameraInfo);

	if (4 != sscanf_s(&pCameraInfo->FPGAVersion[0], "%x.%x.%x.%x", &dummy, &dummy, &dummy, &hwver, sizeof(pCameraInfo->FPGAVersion)))
	{
		ASSERT(FALSE);
	}
	return hwver;
}

/**
* Function: IsLinkSpeedReduced(void);
* Purpose:  Return true if this cameers is connected in such a way, that the link speed is less than it's maximum value.
*           For instance, this cunction will return true if a USB3 camera is connected via a USB2 port.
*/
bool 
CPxLCamera::IsLinkSpeedReduced(void)
{
	return m_bReducedLinkSpeed;
}

/**
* Function: IsFirewallBlocked(void);
* Purpose:  Return true if this cameers is suspected to be on the other side of a firewall, which may block certain traffic coming from 
*           the camera/
*/
bool 
CPxLCamera::IsFirewallBlocked(void)
{
	return m_bSuspectedFirewallBlocking;
}

/**
* Function: IsFixedFramerateModeSupported(void);
* Purpose:  Return true if this cameras supports fixedFramerateMode.
*/
bool 
CPxLCamera::IsFixedFramerateModeSupported(void)
{
    std::pair<float,float>  ffLimits;

    if (this->FeatureSupported(FEATURE_SPECIAL_CAMERA_MODE)) {
        ffLimits = this->GetFeatureLimits(FEATURE_SPECIAL_CAMERA_MODE);
        // Note.  this logic assumes that if the camera supports any mode greater
        // than FEATURE_SPECIAL_CAMERA_MODE_FIXED_FRAME_RATE, then that camera
        // also supports FEATURE_SPECIAL_CAMERA_MODE_FIXED_FRAME_RATE.  If that is 
        // not the case, then this code needs to be updated.
        if ((ffLimits.first <= FEATURE_SPECIAL_CAMERA_MODE_NONE) && 
            (ffLimits.second >= FEATURE_SPECIAL_CAMERA_MODE_FIXED_FRAME_RATE)) {
            return TRUE;
        }
    }

	return FALSE;
}

/**
* Function: IsFixedFramerateModeSupported(void);
* Purpose:  Return true if this cameras supports fixedFramerateMode.
*/
bool 
CPxLCamera::IsFixedFramerateModeEnabled(void)
{

    if (this->IsFixedFramerateModeSupported()) {
        return (this->GetFeatureValue(FEATURE_SPECIAL_CAMERA_MODE) == FEATURE_SPECIAL_CAMERA_MODE_FIXED_FRAME_RATE);
    }

	return FALSE;
}

/**
* Function: RequiresStoppedStream
* Purpose:  Return true if the video stream must be stopped in order to change
*           the value of the specified feature.
*/
bool 
CPxLCamera::RequiresStoppedStream(const U32 featureId)
{
    return (!FeatureSettableWhileStreaming(featureId));
}


/**
* Function: FeatureAffectsPreview
* Purpose:  Determines whether a change in the value of the specified feature
*           could affect the size of the preview window.
*           When the application is in Auto-Reset Preview Window mode, a change
*           to any of these features will trigger a call to ResetPreviewWindow.
*/
bool 
CPxLCamera::FeatureAffectsPreview(const U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_PIXEL_ADDRESSING:
	case FEATURE_ROI:
	case FEATURE_ROTATE:
		return true;
	default:
		return false;
	}
}


/**
* Function: FeatureAffectsBandwidth
* Purpose:  Determines whether a change in the value of the specified feature
*           affects the bandwidth requirement on the firewire bus.
*           Used in "Laptop Friendly" mode to determine whether a change to a
*           feature requires that the framerate be automagically adjusted.
*/
bool 
CPxLCamera::FeatureAffectsBandwidth(const U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_LOOKUP_TABLE: // ???
	case FEATURE_PIXEL_FORMAT:
	case FEATURE_PIXEL_ADDRESSING:
	case FEATURE_ROI:
	case FEATURE_SHUTTER:
	// Note: Shutter doesn't really affect bandwidth requirement, but changing it
	// may cause us to need to turn framerate on when in Laptop Mode.
    case FEATURE_IMAGER_CLK_DIVISOR:
		return true;
	default:
		return false;
	}
}


/**
* Function: SetBandwidth
* Purpose:  When in Laptop Friendly mode, resets the framerate to something
*           less than the default, so as not to overwhelm the PCI bus and/or
*           the processor.
*           Returns true if the framerate is modified.
*/
bool 
CPxLCamera::SetBandwidth(void)
{
	if (theApp.m_framerateDivisor > 1)// && this->GetStreamState() == START_STREAM)
	{
		// Get the current state of FrameRate.
		U32 fr_flags = 0;
		float fr = GetFeatureValue(FEATURE_FRAME_RATE, &fr_flags);
		bool fr_off = (fr_flags & FEATURE_FLAG_OFF) != 0;

		std::pair<float,float> fr_limits = GetFeatureLimits(FEATURE_FRAME_RATE);
		float frameRate_min = fr_limits.first;
		float frameRate_max = fr_limits.second;

		float exp = GetFeatureValue(FEATURE_SHUTTER);

		const int fr_divisor = theApp.m_framerateDivisor;

		if ((MS_PER_SEC / exp) < frameRate_max / fr_divisor)
		{
			// The framerate will already be slower than we would set it using
			// the FRAME_RATE feature, due to the long exposure time. Just make
			// sure that FRAME_RATE is OFF, and the actual framerate will be
			// dictated by the exposure time.
			if (!fr_off)
			{
				SetFlags(FEATURE_FRAME_RATE, FEATURE_FLAG_OFF);
				return true;
			}
		}
		else
		{
			// We can achieve the best performance by turning on framerate
			// and setting it to some fraction of the maximum.
			// Get the current state of FrameRate.
			float optimumFrameRate = max(frameRate_min, frameRate_max / fr_divisor);
			if (fr_off || fr / optimumFrameRate < 0.9 || fr / optimumFrameRate > 1.1)
			{
				SetFeatureValue(FEATURE_FRAME_RATE, optimumFrameRate);
				return true;
			}
			// else: current frame rate is close enough to the optimum that it is
			//       not worth changing it - changing framerate is a fairly
			//       expensive thing to do, because the stream needs to be stopped.
		}
	}
	else
	{
		// Not in Laptop mode.
		// XXX - Do Nothing ???

		/*
		// Get the current state of FrameRate.
		U32 fr_flags = 0;
		float fr = GetFeatureValue(FEATURE_FRAME_RATE, &fr_flags);
		bool fr_off = (fr_flags & FEATURE_FLAG_OFF) != 0;

		// Set to default framerate.
		if (!fr_off)
		{
			SetFlags(FEATURE_FRAME_RATE, FEATURE_FLAG_OFF);

			// Trick the camera into resetting itself to its default framerate
			// by temporarily changing the ROI.
			CStreamState _temp(this, STOP_STREAM, true);
			CRect r = this->GetFeatureRect(FEATURE_ROI);
			this->SetFeatureRect(FEATURE_ROI, r.left, r.top, r.Width()/2, r.Height()/2);
			this->SetFeatureRect(FEATURE_ROI, r.left, r.top, r.Width(), r.Height());
			return true;
		}
		*/
	}
	return false;
}


/******************************************************************************
* Getting and Setting features
******************************************************************************/

/**
* Function: GetFeatureValue
* Purpose:  Get the value of one of the parameters of a feature.
*/
float 
CPxLCamera::GetFeatureValue(const U32 featureId, U32* const pFlags /*=NULL*/, const int paramNo /*=0*/)
{
	if (!FeatureSupported(featureId))
	{
		if (featureId == FEATURE_PIXEL_ADDRESSING) { 
		    return 1.0f;  // no decimation == a decimation value of 1
		}
		    
		CString msg;
		msg.Format("Invalid call to CPxLCamera::GetFeatureValue for feature %d on hCamera 0x%8.8X\n", featureId, m_handle);
		TRACE(msg);
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(featureId) before calling this.
		return 0.0f;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(featureId);

	ASSERT(nParms > 0); // API sanity check
	//ASSERT(paramNo >= 0 && paramNo < nParms);
	if (paramNo < 0 || paramNo >= nParms)
	{
		THROW_PXL_EXCEPTION(ApiInvalidParameterError);
	}

	// Allocate memory. All parameters are returned as floats.
	std::vector<float> parms(nParms, 0.0f);

    PXL_RETURN_CODE rc = PxLGetFeature(m_handle, featureId, &flags, &nParms, &parms[0]);
	PXL_ERROR_CHECK(rc);

	// Return the flags, if they were requested.
	if (pFlags != NULL) {
		*pFlags = flags;
	}

    // As per bugzilla.1779, make note of the firewall warning
    m_bSuspectedFirewallBlocking = (featureId == FEATURE_MAX_PACKET_SIZE && rc == ApiSuccessSuspectedFirewallBlockWarning);

	// Return the value of the parameter.
	return parms[paramNo] * GetUnitsMuliplier(featureId, paramNo);

}


/**
* Function: IsFeatureFlagSet
* Purpose:  Return true if the given flag is currently set to 1 for the given
*           feature. Be careful - if FEATURE_FLAG_OFF is ON, then the feature
*           is OFF - it's easy to get confused and get this backwards (I did).
*/
bool 
CPxLCamera::IsFeatureFlagSet(const U32 featureId, const U32 flag)
{
	U32 flags = 0;
	try 
	{
	    GetFeatureValue(featureId, &flags);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
	
	return (flags & flag) != 0;
}

/**
* Function: FeatureEnabled
* Purpose:  Return true if the feature exists on the current camera, can be turned off, but 
*           is not (IE, it is on)
*/
bool 
CPxLCamera::FeatureEnabled(const U32 featureId)
{
    return (FeatureSupportsOnOff (featureId) &&
            ! IsFeatureFlagSet(featureId, FEATURE_FLAG_OFF));
}

/**
* Function: SetFeatureValue
* Purpose:  
*/
void 
CPxLCamera::SetFeatureValue(const U32 featureId, const float val, const int paramNo /*=0*/, const bool doPreviewReset /*=false*/)
{
	if (!FeatureSupportsManual(featureId)) {
		return;
	}

	U32 nParms = GetFeatureParamCount(featureId);
	ASSERT(nParms > 0); // API sanity check
	if (paramNo < 0 || paramNo >= nParms)
	{
		THROW_PXL_EXCEPTION(ApiInvalidParameterError);
	}

	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(featureId);

	// Post a notification to the application, in the form of a Windows
	// message, that the feature is about to be changed. By the time the GUI
	// processes the message, one of three things will have happened:
	// 1) SetFeature will have returned ApiSuccessParametersChanged, and the
	//    GUI needs to be updated to reflect the actual value of the feature.
	// 2) SetFeature will have returned an error, and again the GUI needs to
	//    be updated to reflect the fact that the feature (probably) still has
	//    its old value.
	// 3) SetFeature returned ApiSuccess. In this case, it's probably not
	//    neccesary to update the GUI, but we do it anyway just to play it
	//    safe - we want to be *sure* that the feature values displayed are
	//    accurate.
	// In any case, the point is that we post this message *before* the call
	// to SetFeature so that it gets done regardless of any exceptions that
	// may be thrown.
	PostViewChange(CT_FeatureChanged, featureId);

	if (nParms == 1) {
		SetSingleParamFeatureValue(featureId, val);
	} else {
		SetMultiParamFeatureValue(featureId, val, paramNo);
	}

	if (FeatureAffectsPreview(featureId) && doPreviewReset) {
		ResetPreviewWindow();
	}

	if (FeatureAffectsBandwidth(featureId)) {
		SetBandwidth();
	}
}


/**
* Function: SetSingleParamFeatureValue
* Purpose:  
*/
void 
CPxLCamera::SetSingleParamFeatureValue(const U32 featureId, const float val)
{
	U32 flags = FEATURE_FLAG_MANUAL;
	float parm = val / GetUnitsMuliplier(featureId, 0);

	// Added a wait cursor to accomodate Focus/Zoom controller
    CWaitCursor wc;

    PXL_ERROR_CHECK(PxLSetFeature(m_handle, featureId, flags, 1 /*nParms*/, &parm));
}


/**
* Function: SetMultiParamFeatureValue
* Purpose:  
*/
void 
CPxLCamera::SetMultiParamFeatureValue(const U32 featureId, const float val, const int paramNo)
{
	U32 flags;
	U32 nParms = GetFeatureParamCount(featureId);

	// Allocate memory. All parameters are returned as floats.
	std::vector<float> parms(nParms, 0.0f);

	// Get the value of the rest of the parameters (the ones we are not setting).
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, featureId, &flags, &nParms, &parms[0]));

	parms[paramNo] = val / GetUnitsMuliplier(featureId, paramNo);

	flags = FEATURE_FLAG_MANUAL;
	PXL_ERROR_CHECK(PxLSetFeature(m_handle, featureId, flags, nParms, &parms[0]));
}

/**
* Function: GetFeatureAutoLimits
* Purpose:  Return the range within which the an auto operation (OnePush or Auto) will
*           converge.
*/
std::pair<float,float> 
CPxLCamera::GetFeatureAutoLimits(const U32 featureId)
{
	// If the feature does not allow the user to specify limits, then the limits are
    // the entire range of the feature.
    if (! FeatureSupportsAutoLimits(featureId)) return GetFeatureLimits (featureId);

    U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(featureId);

	// Allocate memory. All parameters are returned as floats.
	std::vector<float> parms(nParms, 0.0f);

	// Get the value of the rest of the parameters (the ones we are not setting).
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, featureId, &flags, &nParms, &parms[0]));

    // SPECIAL NOTE
    //      FEATURE_FOCUS is unusual, in that if it supports auto limits, the limits are in
    //      parameters 0 and 1, not 1 and 2 like most features (such as FEATURE_FOCUS)
    if (featureId == FEATURE_FOCUS)
	    return std::make_pair(parms[0], parms[1]);
    else
	    return std::make_pair(parms[1], parms[2]);
}

/**
* Function: GetFeatureRect
* Purpose:  Return the value of the specified feature (ROI or AUTO_ROI)
*           as a CRect object.
*/
CRect 
CPxLCamera::GetFeatureRect(const U32 featureId)
{
	CRect r(0,0,0,0);

	if (!FeatureSupported(featureId))
	{
		TRACE("Invalid call to CPxLCamera::GetFeatureRect\n");
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(featurId) before calling this.
		return r;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(featureId);

	ASSERT(nParms >= 4);

	// Allocate memory. All parameters are returned as floats.
	std::vector<float> parms(nParms, 0.0f);

	// Get the values of the parameters of the feature.
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, featureId, &flags, &nParms, &parms[0]));

	ASSERT (FEATURE_ROI_PARAM_LEFT == FEATURE_SHARPNESS_SCORE_PARAM_LEFT); // All ROI features use the same representation -- but just ot be sure....
	r.left = static_cast<long>(parms[FEATURE_ROI_PARAM_LEFT]);
	r.top = static_cast<long>(parms[FEATURE_ROI_PARAM_TOP]);
	r.right = r.left + static_cast<long>(parms[FEATURE_ROI_PARAM_WIDTH]);
	r.bottom = r.top + static_cast<long>(parms[FEATURE_ROI_PARAM_HEIGHT]);

	return r;
}


/******************************************************************************
* Stuff for transforming ROI to correct for Flip and Rotate.
******************************************************************************/
struct mat_2x2
{
	int _11, _12,
		_21, _22;
};

mat_2x2 operator*(mat_2x2 const& a, mat_2x2 const& b)
{
	mat_2x2 result;
	result._11 = a._11 * b._11 + a._12 * b._21;
	result._12 = a._11 * b._12 + a._12 * b._22;
	result._21 = a._21 * b._11 + a._22 * b._21;
	result._22 = a._21 * b._12 + a._22 * b._22;
	return result;
}

CPoint operator*(mat_2x2 const& a, CPoint const& b)
{
	CPoint result;
	result.x = a._11 * b.x + a._12 * b.y;
	result.y = a._21 * b.x + a._22 * b.y;
	return result;
}

CRect& operator*=(CRect& r, mat_2x2 trans)
{
	CPoint tl(r.left, r.top);
	CPoint br(r.right, r.bottom);
	CPoint ntl = trans * tl; // new top-left
	CPoint nbr = trans * br; // new bottom-right
	if (ntl.x > nbr.x)
		std::swap(ntl.x, nbr.x);
	if (ntl.y > nbr.y)
		std::swap(ntl.y, nbr.y);
	r.left = ntl.x;
	r.top = ntl.y;
	r.right = nbr.x;
	r.bottom = nbr.y;
	return r;
}

static const mat_2x2 identity = {
	 1,  0,
     0,  1,
};
static const mat_2x2 hflip = {
	-1,  0,
     0,  1,
};
static const mat_2x2 vflip = {
	 1,  0,
     0, -1,
};
static const mat_2x2 rot90 = {
	 0, -1,
     1,  0,
};

void translate(CRect& r, CSize delta)
{
	r.left += delta.cx;
	r.right += delta.cx;
	r.top += delta.cy;
	r.bottom += delta.cy;
}

/******************************************************************************
* END: Stuff for transforming ROI to correct for Flip and Rotate.
******************************************************************************/

/**
* Function: GetRoi
* Purpose:  Get the ROI corrected for Flip and Rotation.
* Parameters:
*/
CRect 
CPxLCamera::GetRoi(U32 featureId /* == FEATURE_ROI */) 
{
	CRect r = GetFeatureRect(featureId);

	bool flipH = 1 == this->GetFeatureValue(FEATURE_FLIP, NULL, FEATURE_FLIP_PARAM_HORIZONTAL);
	bool flipV = 1 == this->GetFeatureValue(FEATURE_FLIP, NULL, FEATURE_FLIP_PARAM_VERTICAL);
	float rot = this->GetFeatureValue(FEATURE_ROTATE);

	long maxwidth = featureId ==  FEATURE_ROI ?
                     (long)this->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_WIDTH) :
	                 (long)this->GetFeatureValue(FEATURE_ROI, NULL, FEATURE_ROI_PARAM_WIDTH);
	long maxheight = featureId ==  FEATURE_ROI ? 
                     (long)this->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_HEIGHT) :
	                 (long)this->GetFeatureValue(FEATURE_ROI, NULL, FEATURE_ROI_PARAM_HEIGHT);

	mat_2x2 trans = identity;

	// Apply flips first...
	if (flipH) {
		trans = trans * hflip;
	}
	if (flipV) {
		trans = trans * vflip;
	}

	// And then rotations.
	bool sideways = false;
	while (rot > 0)
	{
		trans = trans * rot90;
		rot -= 90;
		sideways = !sideways;
	}

	translate(r, CSize(-maxwidth/2, -maxheight/2));

	r *= trans;

	if (sideways)
		std::swap(maxwidth,maxheight);
	translate(r, CSize(maxwidth/2, maxheight/2));

	return r;
}


/**
* Function: SetRoi
* Purpose:  Set the ROI, correcting for Flip and Rotation.
*/
void 
CPxLCamera::SetRoi(long left, long top, long width, long height, bool doPreviewReset /* =false */, U32 featureId /* =FEATURE_ROI */, bool disable /* =false */)
{
	CRect r(left, top, left+width, top+height);
	bool flipH = 1 == this->GetFeatureValue(FEATURE_FLIP, NULL, FEATURE_FLIP_PARAM_HORIZONTAL);
	bool flipV = 1 == this->GetFeatureValue(FEATURE_FLIP, NULL, FEATURE_FLIP_PARAM_VERTICAL);
	float rot = this->GetFeatureValue(FEATURE_ROTATE);

	long maxwidth = featureId == FEATURE_ROI ?
                     (long)this->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_WIDTH) :
	                 (long)this->GetFeatureValue(FEATURE_ROI, NULL, FEATURE_ROI_PARAM_WIDTH);
	long maxheight = featureId == FEATURE_ROI ?
                     (long)this->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_HEIGHT) :
	                 (long)this->GetFeatureValue(FEATURE_ROI, NULL, FEATURE_ROI_PARAM_HEIGHT);

	mat_2x2 trans = identity;

	// Here (as opposed to GetRoi, above), we need to do rotation first...
	bool sideways = false;
	while (rot < 360)
	{
		trans = trans * rot90;
		rot += 90;
		sideways = !sideways;
	}

	// And then flips.
	if (flipH) {
		trans = trans * hflip;
	}
	if (flipV) {
		trans = trans * vflip;
	}

	if (sideways) {
		std::swap(maxwidth,maxheight);
	}
	translate(r, CSize(-maxwidth/2, -maxheight/2));

	r *= trans;

	if (sideways) {
		std::swap(maxwidth,maxheight);
	}
	translate(r, CSize(maxwidth/2, maxheight/2));

	SetFeatureRect(featureId, r.left, r.top, r.right-r.left, r.bottom-r.top, doPreviewReset, disable);
}


/**
* Function: SetRoiSize
* Purpose:  Set the ROI size, correcting for Flip and Rotation.
*/
void 
CPxLCamera::SetRoiSize(long width, long height, 
                       const bool doPreviewReset /* =false */, 
                       const U32 featureId /* =FEATURE_ROI */,
                       const bool disabled /* =false */)
{
	int rot = static_cast<int>(this->GetFeatureValue(FEATURE_ROTATE));
	if ((rot % 180) != 0)
	{
		std::swap(width, height);
	}
	SetFeatureRectSize(featureId, width, height, doPreviewReset, disabled);
}


/**
* Function: SetFeatureRect
* Purpose:  Set either FEATURE_ROI or FEATURE_AUTO_ROI.
*/
void 
CPxLCamera::SetFeatureRect(const U32 featureId, 
                           const long left, const long top, const long width, const long height, 
                           const bool doPreviewReset /*=false*/, bool disable /* = false */)
{
	if (!FeatureSupportsManual(featureId))
	{
		TRACE("Invalid call to CPxLCamera::SetFeatureRect");
		return;
	}

    U32 flags = disable ? FEATURE_FLAG_OFF : FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(featureId);

	ASSERT(nParms >= 4); // We set the first 4, so they had better all be valid...

	std::vector<float> parms(nParms, 0);

	ASSERT (FEATURE_ROI_PARAM_LEFT == FEATURE_SHARPNESS_SCORE_PARAM_LEFT); // All ROI features use the same representation -- but just ot be sure....
	parms[FEATURE_ROI_PARAM_LEFT] = static_cast<float>(left);
	parms[FEATURE_ROI_PARAM_TOP] = static_cast<float>(top);
	parms[FEATURE_ROI_PARAM_WIDTH] = static_cast<float>(width);
	parms[FEATURE_ROI_PARAM_HEIGHT] = static_cast<float>(height);

	
	std::auto_ptr<CStreamState> tempStream(NULL);
	if (RequiresStoppedStream(featureId))
	{
		tempStream = std::auto_ptr<CStreamState>(new CStreamState(this, STOP_STREAM, true));
	}

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, featureId);

	PXL_ERROR_CHECK(PxLSetFeature(m_handle, featureId, flags, nParms, &parms[0]));

	if (FeatureAffectsPreview(featureId) && doPreviewReset) {
		ResetPreviewWindow();
	}

	if (FeatureAffectsBandwidth(featureId)) {
		SetBandwidth();
	}
}


/**
* Function: SetFeatureRectSize
* Purpose:  Set FEATURE_ROI, specifying only the size - the top and left will
*           be calculated in order to center the ROI.
*/
void 
CPxLCamera::SetFeatureRectSize(const U32 featureId, const long width, const long height, 
                               const bool doPreviewReset /*=false*/, const bool disabled /* =false */)
{
	CRect r = this->GetFeatureRect(featureId);

	if (r.Width() == width && r.Height() == height)
		return; // Nothing to change.

	long maxWidth; 
	long maxHeight;

    // Bugzilla.562 - Center the SS ROI within the current ROI
    if (FEATURE_SHARPNESS_SCORE == featureId)
    {
	    maxWidth = static_cast<long>(this->GetFeatureValue(FEATURE_ROI, NULL, FEATURE_ROI_PARAM_WIDTH));
	    maxHeight = static_cast<long>(this->GetFeatureValue(FEATURE_ROI, NULL, FEATURE_ROI_PARAM_HEIGHT));
	} else {
	    maxWidth = static_cast<long>(this->GetFeatureMaxVal(featureId, FEATURE_ROI_PARAM_WIDTH));
	    maxHeight = static_cast<long>(this->GetFeatureMaxVal(featureId, FEATURE_ROI_PARAM_HEIGHT));
	}
	
	// Leave the center of the region where it is, or as close as possible.
	r.left = min(maxWidth - width, max(0, r.left + (r.Width() - width)/2));
	r.top = min(maxHeight - height, max(0, r.top + (r.Height() - height)/2));

	// Alternative 1: Leave the top-left corner where it is, if possible.
	//r.left = min(r.left, maxWidth - width);
	//r.top = min(r.top, maxHeight - height);

	// Alternative 2: Center the resulting region.
	//r.left = (maxWidth - width) / 2;
	//r.top = (maxHeight - height) / 2;

	this->SetFeatureRect(featureId, r.left, r.top, width, height, doPreviewReset, disabled);

}


/**
* Function: GetHorizontalFlip
* Purpose:  Return true if the Horizontal Flip feature is currently on.
*/
bool 
CPxLCamera::GetHorizontalFlip(void)
{
	return static_cast<int>(GetFeatureValue(FEATURE_FLIP, NULL, FEATURE_FLIP_PARAM_HORIZONTAL)) > 0;
}


/**
* Function: SetHorizontalFlip
* Purpose:  Set the horizontal component of FEATURE_FLIP.
*/
void 
CPxLCamera::SetHorizontalFlip(const bool flipped)
{
	this->SetFeatureValue(FEATURE_FLIP, flipped ? 1.0f : 0.0f, FEATURE_FLIP_PARAM_HORIZONTAL);
}


/**
* Function: GetVerticalFlip
* Purpose:  Return true if the Vertical Flip feature is currently on.
*/
bool 
CPxLCamera::GetVerticalFlip(void)
{
	return static_cast<int>(GetFeatureValue(FEATURE_FLIP, NULL, FEATURE_FLIP_PARAM_VERTICAL)) > 0;
}


/**
* Function: SetVerticalFlip
* Purpose:  Set the vertical component of FEATURE_FLIP.
*/
void 
CPxLCamera::SetVerticalFlip(const bool flipped)
{
	this->SetFeatureValue(FEATURE_FLIP, flipped ? 1.0f : 0.0f, FEATURE_FLIP_PARAM_VERTICAL);
}


/**
* Function: GetTrigger
* Purpose:  Return a Trigger object that defines the value of the active 
*           camera's FEATURE_TRIGGER.
*/
Trigger 
CPxLCamera::GetTrigger(U32* const pFlags /*=NULL*/)
{
	Trigger t = {-1, -1, -1, 0.0f, 0.0f}; // Initialize with invalid values

	if (!FeatureSupported(FEATURE_TRIGGER))
	{
		TRACE("Invalid call to CPxLCamera::GetTrigger\n");
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(FEATURE_TRIGGER) before calling this.
		return t;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(FEATURE_TRIGGER);

	ASSERT(nParms >= 5);

	// Allocate memory. All parameters are returned as floats.
	std::vector<float> parms(nParms, 0.0f);

	// Get the values of the parameters of the feature.
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_TRIGGER, &flags, &nParms, &parms[0]));

	t.Mode = static_cast<int>(parms[FEATURE_TRIGGER_PARAM_MODE]);
	t.Type = static_cast<int>(parms[FEATURE_TRIGGER_PARAM_TYPE]);
	t.Polarity = static_cast<int>(parms[FEATURE_TRIGGER_PARAM_POLARITY]);
	t.Delay = parms[FEATURE_TRIGGER_PARAM_DELAY];
	t.Parameter = parms[FEATURE_TRIGGER_PARAM_PARAMETER];

	// Return the flags, if they were requested:
	if (pFlags != NULL)
		*pFlags = flags;

	return t;
}


/**
* Function: SetTrigger
* Purpose:  Set the value of the current camera's FEATURE_TRIGGER.
*/
void 
CPxLCamera::SetTrigger(const Trigger trigger, U32 flags /*=0*/)
{
	if (!FeatureSupportsManual(FEATURE_TRIGGER))
	{
		TRACE("Invalid call to CPxLCamera::SetTrigger");
		return;
	}

	flags |= FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(FEATURE_TRIGGER);

	ASSERT(nParms >= 5); // We set the first 5, so they had better all be valid...

	std::vector<float> parms(nParms, 0.0f);

	parms[FEATURE_TRIGGER_PARAM_MODE] = static_cast<float>(trigger.Mode);
	parms[FEATURE_TRIGGER_PARAM_TYPE] = static_cast<float>(trigger.Type);
	parms[FEATURE_TRIGGER_PARAM_POLARITY] = static_cast<float>(trigger.Polarity);
	parms[FEATURE_TRIGGER_PARAM_DELAY] = trigger.Delay;
	parms[FEATURE_TRIGGER_PARAM_PARAMETER] = trigger.Parameter;

	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(FEATURE_TRIGGER)

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, FEATURE_TRIGGER);

	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_TRIGGER, flags, nParms, &parms[0]));
}


/**
* Function: GetGpo
* Purpose:  Return a Gpo object that defines the value of the active 
*           camera's FEATURE_GPIO.
*/
Gpo 
CPxLCamera::GetGpo(const int gpoNumber)
{
	ASSERT(gpoNumber < PXL_MAX_STROBES);

	Gpo g = {0, -1, -1, 0.0f, 0.0f, 0.0f}; // Initialize with invalid values

	if (!FeatureSupported(FEATURE_GPIO))
	{
		TRACE("Invalid call to CPxLCamera::GetGpo\n");
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(FEATURE_GPIO) before calling this.
		return g;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(FEATURE_GPIO);

	ASSERT(nParms >= 6);

	// Allocate memory. All parameters are returned as floats.
	std::vector<float> parms(nParms, 0.0f);

	// Tell the API which GPO we want.
	parms[FEATURE_GPIO_PARAM_GPIO_INDEX] = static_cast<float>(gpoNumber);

	// Get the values of the parameters of the feature.
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_GPIO, &flags, &nParms, &parms[0]));


	g.flags = flags;
	g.Mode = static_cast<int>(parms[FEATURE_GPIO_PARAM_MODE]);
	g.Polarity = static_cast<int>(parms[FEATURE_GPIO_PARAM_POLARITY]);
	g.Parameter1 = parms[FEATURE_GPIO_PARAM_PARAM_1];
	g.Parameter2 = parms[FEATURE_GPIO_PARAM_PARAM_2];
	g.Parameter3 = parms[FEATURE_GPIO_PARAM_PARAM_3];

	return g;
}


/**
* Function: SetGpo
* Purpose:  Set the value of the current camera's FEATURE_GPIO.
*/
PXL_RETURN_CODE 
CPxLCamera::SetGpo(const Gpo gpo, const int gpoNumber)
{
	if (!FeatureSupportsManual(FEATURE_GPIO))
	{
		TRACE("Invalid call to CPxLCamera::SetGpo");
		return ApiSuccess;
	}

	U32 flags = gpo.flags;
	U32 nParms = GetFeatureParamCount(FEATURE_GPIO);

	ASSERT(nParms >= 6); // We set the first 6, so they had better all be valid...

	std::vector<float> parms(nParms, 0.0f);

	parms[FEATURE_GPIO_PARAM_GPIO_INDEX] = static_cast<float>(gpoNumber);
	parms[FEATURE_GPIO_PARAM_MODE] = static_cast<float>(gpo.Mode);
	parms[FEATURE_GPIO_PARAM_POLARITY] = static_cast<float>(gpo.Polarity);
	parms[FEATURE_GPIO_PARAM_PARAM_1] = gpo.Parameter1;
	parms[FEATURE_GPIO_PARAM_PARAM_2] = gpo.Parameter2;
	parms[FEATURE_GPIO_PARAM_PARAM_3] = gpo.Parameter3;

	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(FEATURE_GPIO)

	// See comment in SetFeatureValue, above.
	if (!m_bNoPostViewChange) {
		PostViewChange(CT_FeatureChanged, FEATURE_GPIO);
	}

    PXL_RETURN_CODE rc = PxLSetFeature(m_handle, FEATURE_GPIO, flags, nParms, &parms[0]);
	PXL_ERROR_CHECK(rc);
    return rc;
}

/**
* Function: GetGpioProfile
* Purpose:  Identifies the GPIO profile of this particular camera
*/
GpioProfiles 
CPxLCamera::GetGpioProfile()
{
	if (!FeatureSupportsManual(FEATURE_GPIO)) return NO_GPIOS;
	
    int maxGpio = static_cast<int> (GetFeatureMaxVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX));
    int maxMode = static_cast<int> (GetFeatureMaxVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_MODE));

    switch (maxGpio)
    {
    case 4:
        return FOUR_GPOS;
    case 3:
        return TWO_GPOS_ONE_GPI;
    case 2:
        if (maxMode >= GPIO_MODE_INPUT) return ONE_GPIO_ONE_GPO;
        else                            return TWO_GPOS;
    case 1:
        if (maxMode >= GPIO_MODE_INPUT) return ONE_GPIO;
        else                            return ONE_GPO;
    case 0:
    default:
        return NO_GPIOS;
    }
}

/**
* Function: GetPixelAddressing
* Purpose:  Return a Pixel Addressing object that defines the value of the active 
*           camera's FEATURE_PIXEL_ADDRESSING.
*/
PixelAddressing
CPxLCamera::GetPixelAddressing(U32* const pFlags /*=NULL*/)
{
	PixelAddressing pa = {0, 1, 1, false}; // Bugzilla.412 Initialize with innocuous values

	if (!FeatureSupported(FEATURE_PIXEL_ADDRESSING))
	{
		TRACE("Invalid call to CPxLCamera::GetPixelAddressing\n");
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(FEATURE_PIXEL_ADDRESSING) before calling this.
		return pa;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING);

	ASSERT(nParms >= 1);

	// Allocate memory. All parameters are returned as floats.
	std::vector<float> parms(nParms, 0.0f);

	// Get the values of the parameters of the feature.
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_PIXEL_ADDRESSING, &flags, &nParms, &parms[0]));

	if (nParms == 1) 
	{
	    // This is a really old camera/API, and it only supports symmetric decimation
	    pa.Mode = 0;
	    pa.x = static_cast<int>(parms[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE]);
	    pa.y = static_cast<int>(parms[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE]);
	    pa.supportAsymmetry = false;
	} else if (nParms < 4) {
	    // Newer camera that does not support asymmetric pixel addressing.
	    pa.Mode = static_cast<int>(parms[FEATURE_PIXEL_ADDRESSING_PARAM_MODE]);
	    pa.x = static_cast<int>(parms[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE]);
	    pa.y = static_cast<int>(parms[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE]);
	    pa.supportAsymmetry = false;    
	} else {
	    // New camera that does support asymmetric pixel addressing.
	    pa.Mode = static_cast<int>(parms[FEATURE_PIXEL_ADDRESSING_PARAM_MODE]);
	    pa.x = static_cast<int>(parms[FEATURE_PIXEL_ADDRESSING_PARAM_X_VALUE]);
	    pa.y = static_cast<int>(parms[FEATURE_PIXEL_ADDRESSING_PARAM_Y_VALUE]);
	    pa.supportAsymmetry = true;    
	}
	// Return the flags, if they were requested:
	if (pFlags != NULL)
		*pFlags = flags;

	return pa;
}


/**
* Function: SetPixelAddressing
* Purpose:  Set the value of the current camera's FEATURE_PIXEL_ADDRESSING.
*/
void 
CPxLCamera::SetPixelAddressing(const PixelAddressing pixelAddressing, U32 flags /*=0*/, const bool doPreviewReset /*=false*/)
{
	if (!FeatureSupportsManual(FEATURE_PIXEL_ADDRESSING))
	{
		TRACE("Invalid call to CPxLCamera::SetPixelAddressing");
		return;
	}

	flags |= FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING);

	ASSERT(nParms >= 1); // We set the first 5, so they had better all be valid...

	std::vector<float> parms(nParms, 0.0f);

	if (nParms == 1)
	{
	    // This is a really old camera/API, and it only supports symmetric decimation
	    parms[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE] = static_cast<float>(pixelAddressing.x);
	} else if (nParms >=2 && nParms < 4) {
	    // Newer camera that does not support asymmetric pixel addressing.
	    parms[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE] = static_cast<float>(pixelAddressing.x);
	    parms[FEATURE_PIXEL_ADDRESSING_PARAM_MODE]  = static_cast<float>(pixelAddressing.Mode);
	} else {
	    // New camera that does support asymmetric pixel addressing.
	    parms[FEATURE_PIXEL_ADDRESSING_PARAM_VALUE] = static_cast<float>(pixelAddressing.x);
	    parms[FEATURE_PIXEL_ADDRESSING_PARAM_MODE]  = static_cast<float>(pixelAddressing.Mode);
	    parms[FEATURE_PIXEL_ADDRESSING_PARAM_X_VALUE] = static_cast<float>(pixelAddressing.x);
	    parms[FEATURE_PIXEL_ADDRESSING_PARAM_Y_VALUE]  = static_cast<float>(pixelAddressing.y);
	}

	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(FEATURE_PIXEL_ADDRESSING)

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, FEATURE_PIXEL_ADDRESSING);

    PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_PIXEL_ADDRESSING, flags, nParms, &parms[0]));

	if (doPreviewReset) {
		ResetPreviewWindow();
	}
}


/**
* Function: GetKneePoints
* Purpose:  Return a vector<float> that defines the value of the active 
*           camera's FEATURE_EXTENDED_SHUTTER.
*/
std::vector<float> 
CPxLCamera::GetKneePoints(void)
{
	std::vector<float> kneePoints;

	if (!FeatureSupported(FEATURE_EXTENDED_SHUTTER))
	{
		TRACE("Invalid call to CPxLCamera::GetKneePoints\n");
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(FEATURE_EXTENDED_SHUTTER) before calling this.
		return kneePoints;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(FEATURE_EXTENDED_SHUTTER);

	// Allocate memory. All parameters are returned as floats.
	std::vector<float> parms(nParms, 0.0f);

	// Get the values of the parameters of the feature.
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_EXTENDED_SHUTTER, &flags, &nParms, &parms[0]));

	// Put the results in the vector to be returned, and convert to milliseconds.
	for (int i = 0; i < parms[FEATURE_EXTENDED_SHUTTER_PARAM_NUM_KNEES]; i++)
		kneePoints.push_back(parms[i + FEATURE_EXTENDED_SHUTTER_PARAM_KNEE_1] * MS_PER_SEC);

	return kneePoints;
}


/**
* Function: SetKneePoints
* Purpose:  
*/
void 
CPxLCamera::SetKneePoints(const std::vector<float> knees)
{
	if (!FeatureSupportsManual(FEATURE_EXTENDED_SHUTTER))
	{
		TRACE("Invalid call to CPxLCamera::SetKneePoints");
		return;
	}
	U32 nParms = GetFeatureParamCount(FEATURE_EXTENDED_SHUTTER);

	std::vector<float> parms(nParms, 0.0f);

	// Convert millisecons to seconds before passing the values to the API.
	parms[FEATURE_EXTENDED_SHUTTER_PARAM_NUM_KNEES] = static_cast<float>(knees.size());
	for (int i = 0; i < knees.size(); i++)
	{
		parms[i + FEATURE_EXTENDED_SHUTTER_PARAM_KNEE_1] = knees[i] / MS_PER_SEC;
	}

	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(FEATURE_EXTENDED_SHUTTER)

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, FEATURE_EXTENDED_SHUTTER);

	U32 flags = FEATURE_FLAG_MANUAL;
    // Klude for Bugzilla.178
    {
        // check to see if we're trying to 'disable' one or more of the kneepoints.
        std::vector<float> currentKnees = GetKneePoints();
        int lastKneePointToKeep = knees.size();

		std::pair<float,float> kneePointLimits = GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,1);
		float exposure = GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < exposure);
        
        if (useCameraKpLimits && currentKnees.size() > knees.size())
        {
            // The user is trying to disable a kneepoint.  In order to do this, first we need to readjust
            // the knee point of all of the kneepoints we are keeping, making sure it is in a valid range.  
            // Use a temporary value of midway within the range..
            std::vector<float> savedValues = parms;
            for (int kp = 1; kp <=  lastKneePointToKeep; kp++)
            {
                std::pair<float,float> kneePointLimits = GetFeatureLimits(FEATURE_EXTENDED_SHUTTER, kp);
                float safeValue = (kneePointLimits.first + kneePointLimits.second) / 2.0f;  // Midway between the two
                parms[kp] = safeValue / MS_PER_SEC;
            }       	
          	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_EXTENDED_SHUTTER, flags, nParms, &parms[0]));
        	parms = savedValues;
        }
    }
    
	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_EXTENDED_SHUTTER, flags, nParms, &parms[0]));
}


/**
* Function: GetWhiteShading
* Purpose:  Return a vector<float> that defines the value of the RGB gains.
*/
std::vector<float> 
CPxLCamera::GetWhiteShading(void)
{
	std::vector<float> gains;

	if (!FeatureSupported(FEATURE_WHITE_SHADING))
	{
		TRACE("Invalid call to CPxLCamera::GetWhiteShading\n");
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(FEATURE_WHITE_SHADING) before calling this.
		return gains;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(FEATURE_WHITE_SHADING);

	// Allocate memory.
	gains.resize(nParms);

	// Get the values of the parameters of the feature.
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_WHITE_SHADING, &flags, &nParms, &gains[0]));

	return gains;
}


/**
* Function: SetWhiteShading
* Purpose:  
*/
void 
CPxLCamera::SetWhiteShading(const std::vector<float> rgb)
{
	if (!FeatureSupportsManual(FEATURE_WHITE_SHADING))
	{
		TRACE("Invalid call to CPxLCamera::SetWhiteShading");
		return;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = rgb.size(); //GetFeatureParamCount(FEATURE_WHITE_SHADING);

	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(FEATURE_WHITE_SHADING)

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, FEATURE_WHITE_SHADING);

	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_WHITE_SHADING, flags, nParms, &rgb[0]));
}


/**
* Function: GetLighting
* Purpose:  Return a Lighting object that defines the value of the active
*           camera's FEATURE_LIGHTING.
*/
Lighting
CPxLCamera::GetLighting(U32* const pFlags /*=NULL*/)
{
   Lighting l = { -1, 0.0f, 0.0f, 0.0f }; // Initialize with invalid values

   if (!FeatureSupported(FEATURE_LIGHTING))
   {
      TRACE("Invalid call to CPxLCamera::GetLighting\n");
      // It is the responsibility of the caller to ensure that he is getting
      // a valid return value from this function - usually by testing the
      // return value of FeatureSupported(FEATURE_LIGHTING) before calling this.
      return l;
   }

   U32 flags = FEATURE_FLAG_MANUAL;
   U32 nParms = GetFeatureParamCount(FEATURE_LIGHTING);

   ASSERT(nParms >= 4);

   // Allocate memory. All parameters are returned as floats.
   std::vector<float> parms(nParms, 0.0f);

   // Get the values of the parameters of the feature.
   PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_LIGHTING, &flags, &nParms, &parms[0]));

   l.Brightness = static_cast<int>(parms[FEATURE_LIGHTING_PARAM_BRIGHTNESS]);
   l.Current = parms[FEATURE_LIGHTING_PARAM_CURRENT];
   l.Voltage = parms[FEATURE_LIGHTING_PARAM_VOLTAGE];
   l.Temperature = parms[FEATURE_LIGHTING_PARAM_TEMPERATURE];

   // Return the flags, if they were requested:
   if (pFlags != NULL)
      *pFlags = flags;

   return l;
}


/**
* Function: GetLookupTable
* Purpose:  
*/
std::vector<float> 
CPxLCamera::GetLookupTable(U32* const pFlags /*=NULL*/)
{
	std::vector<float> table;

	if (!FeatureSupported(FEATURE_LOOKUP_TABLE))
	{
		TRACE("Invalid call to CPxLCamera::GetLookupTable\n");
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(FEATURE_LOOKUP_TABLE) before calling this.
		return table;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = static_cast<U32>(GetFeatureMaxVal(FEATURE_LOOKUP_TABLE));

	// Allocate memory. All parameters are returned as floats.
	table.resize(nParms);

	std::auto_ptr<CStreamState> _temp(NULL);
	if (this->GetModelNumber() == 741 && this->GetStreamState() == START_STREAM)
	{
		// In older model 741's, we need to pause the stream to read the LT.
		_temp = std::auto_ptr<CStreamState>(new CStreamState(this, PAUSE_STREAM, 1));
	}

	// Get the values of the parameters of the feature.
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_LOOKUP_TABLE, &flags, &nParms, &table[0]));

	if (pFlags != NULL) {
		*pFlags = flags;
	}

	return table;
}


/**
* Function: SetLookupTable
* Purpose:  Set the value of FEATURE_LOOKUP_TABLE.
*/
void 
CPxLCamera::SetLookupTable(std::vector<float> & values, const U32 extraFlags /*=0*/)
{
	if (!FeatureSupportsManual(FEATURE_LOOKUP_TABLE)) {
		return;
	}

	// API Note: FEATURE_LOOKUP_TABLE is special - its parameters don't mean 
	//   quite the same thing as the parameters of other features do.
	//   Specifically, the camera reports the number of parameters as 1,
	//   and the max value of that parameter as the number of entries that
	//   the lookup table should have.
	//   For example: If the camera reports the max value of the single 
	//     lookup table parameter as 256, then we must pass to PxLSetFeature
	//     a pointer to an array of 256 floats, each with a value between 
	//     0 and 255, inclusive.
	U32 flags = FEATURE_FLAG_MANUAL;
	flags |= extraFlags;
	U32 nParms = static_cast<U32>(GetFeatureMaxVal(FEATURE_LOOKUP_TABLE));

	ASSERT(values.size() >= nParms);

	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(FEATURE_LOOKUP_TABLE)

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, FEATURE_LOOKUP_TABLE);

	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_LOOKUP_TABLE, flags, nParms, &values[0]));

}

void
CPxLCamera::InvalidateLookupGraphCache()
{
	m_lookupPts.resize(0);
	m_lookupTypes.resize(0);
}

/**
* Function: CacheLookupGraphPoints
* Purpose:  
*/
void 
CPxLCamera::CacheLookupGraphPoints(std::vector<long> const& pts, std::vector<BYTE> const& types)
{
	ASSERT(pts.size() == types.size() * 2);
	m_lookupPts.resize(pts.size());
	std::copy(pts.begin(), pts.end(), m_lookupPts.begin());
	m_lookupTypes.resize(types.size());
	std::copy(types.begin(), types.end(), m_lookupTypes.begin());
}


/**
* Function: PopulateGraph
* Purpose:  
*/
bool 
CPxLCamera::PopulateGraph(CLookupGraph& graph)
{
	if (this->FeatureSupported(FEATURE_GAMMA) && !this->IsFeatureFlagSet(FEATURE_GAMMA, FEATURE_FLAG_OFF))
	{
		// Gamma is on. Because Gamma is implemented in the camera (PL-A682/782)
		// by using a lookup table, then when the GAMMA feature is on, we
		// assume that the lookup table may have been changed, and we cannot
		// use our set of cached points.
		// By returning false here, we are telling the calling function that
		// we were not able to populate the graph from cached points, so it
		// should read the lookup table from the camera.
		return false;
	}

	if (m_lookupPts.size() < 4 || m_lookupTypes.size() < 2)
	{
		// No lookup points have been cached for this camera yet
		//graph.Reset();
		return false;
	}

	graph.SetPoints(&m_lookupPts[0], &m_lookupTypes[0], m_lookupTypes.size());
	return true;
}


/**
* Function: SetAuto
* Purpose:  
*/
void 
CPxLCamera::SetAuto(const U32 featureId, const bool turnOn, const std::pair<float,float> limit)
{
	if (!FeatureSupportsAuto(featureId))
	{
		TRACE("CPxLCamera::SetAuto called for unsupported feature\n");
		//return;
	}

	U32 flags = turnOn ? FEATURE_FLAG_AUTO : FEATURE_FLAG_MANUAL;
	SetFlags(featureId, flags, limit);

	// It can take some time for the firmware to turn AUTO mode on or off.
	// Wait until the AUTO bit comes back in the state that we have just
	// set it to.
	while (IsFeatureFlagSet(featureId, FEATURE_FLAG_AUTO) != turnOn)
	{
		Sleep(50);
	}
}


/**
* Function: SetOnePush
* Purpose:  Request that the camera automatically set the value of a feature.
*           The specified feature must support one-push setting, or this method
*           will not work.
*/
void 
CPxLCamera::SetOnePush(const U32 featureId, bool turnOn /* = true */, std::pair<float,float> limit /* = (0.0, 0.0) */)
{
	if (!FeatureSupportsOnePush(featureId))
	{
		TRACE("CPxLCamera::SetOnePush called for unsupported feature\n");
		//return;
	}

	std::auto_ptr<CStreamState> temp(NULL);
	if (featureId == FEATURE_SHUTTER ||
        featureId == FEATURE_GAIN) {
		temp = std::auto_ptr<CStreamState>(new CStreamState(this, START_STREAM));
	}

	U32 flags = FEATURE_FLAG_ONEPUSH;
	if (turnOn) 
	{
	    SetFlags(featureId, flags, limit);
	} else {
	    ClearFlags(featureId, flags);
	    // If we are simply cancelling the OnePush, we are done.
	    return;
	}
	
	// Some features do the auto adjustment quickly, others take some time.  See if it completes quicky.
	// if not, then launch a new dialog that will show the adjustments being made, and allow the user to
	// cancel the auto adjustment (leaving the feature at where the auto adjustment left off.
	{
        CWaitCursor wc;
	    for (int i=0; i< 10 && IsFeatureFlagSet(featureId, FEATURE_FLAG_ONEPUSH); i++)
	    {
	        Sleep(50);
	    }
	}
	if (IsFeatureFlagSet(featureId, FEATURE_FLAG_ONEPUSH))
	{
	    //Still not done.  Bring out the heavy guns.
	    m_oneTimeAutofeatureId = featureId;
	    COneTimeAutoDialog dlg(this);
		dlg.DoModal();

	}

	// XXX - On the 782, it seems that doing a ONE_PUSH on either:
	// a) WhiteShading, or
	// b) WhiteBalance
	// Causes the framerate to reset to default (full speed)
	if (featureId == FEATURE_WHITE_SHADING || featureId == FEATURE_COLOR_TEMP)
	this->SetBandwidth();
}

/**
* Function: SetLimit
* Purpose:  
*/
void 
CPxLCamera::SetLimit(const U32 featureId, const bool lower)
{
	if ((lower  && !FeatureSupportsAssertLowerLimit(featureId)) ||
        (!lower && !FeatureSupportsAssertUpperLimit(featureId)))
	{
		TRACE("CPxLCamera::SetLimit called for unsupported feature\n");
		return;
	}

	U32 flags = lower ? FEATURE_FLAG_ASSERT_LOWER_LIMIT : FEATURE_FLAG_ASSERT_UPPER_LIMIT;
	SetFlags(featureId, flags);
}

/**
* Function: SetOnOff
* Purpose:  
*/
void 
CPxLCamera::SetOnOff(const U32 featureId, const bool turnOn)
{
	if (!FeatureSupportsOnOff(featureId))
	{
		TRACE("CPxLCamera::SetOnOff called for unsupported feature\n");
		//return;
	}

	U32 flags = turnOn ? FEATURE_FLAG_MANUAL : FEATURE_FLAG_OFF;
	SetFlags(featureId, flags);
}


/**
* Function: SetFlags
* Purpose:  
*/
void 
CPxLCamera::SetFlags(const U32 featureId, const U32 flags, std::pair<float,float> limit /* = {0.0, 0.0) */)
{
	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(featureId)

	U32 nParms = GetFeatureParamCount(featureId);
	U32 dummy = 0;

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, featureId);

	std::vector<float> parms(nParms, 0.0f);
	
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, featureId, &dummy, &nParms, &parms[0]));

    if (((FEATURE_FLAG_ONEPUSH | FEATURE_FLAG_AUTO) & flags) && FeatureSupportsAutoLimits(featureId))
    {
        // This feature supports auto limits.  but, did the user supply the limits?
        if (limit.first == limit.second)
        {
    	    nParms = 1; // bugzilla.797 - We don't support reduced range
        } else {
            // IMPORTANT NOTE
            //      FEATURE_FOCUS is kind of special, in that the limits are parameter 1 and 2, not the typical
            //      parameter 2 & 3
            if (nParms == 2)
            {
                parms[0] = limit.first;
                parms[1] = limit.second;
            } else if (nParms > 2) {
                parms[1] = limit.first;
                parms[2] = limit.second;
            }
        }
    }
	
    PXL_ERROR_CHECK(PxLSetFeature(m_handle, featureId, flags, nParms, &parms[0]));
}

/**
* Function: ClearFlags
* Purpose:  
*/
void 
CPxLCamera::ClearFlags(const U32 featureId, const U32 flags)
{
	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(featureId)

	U32 nParms = GetFeatureParamCount(featureId);
	U32 dummy = 0;

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, featureId);

	std::vector<float> parms(nParms, 0.0f);
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, featureId, &dummy, &nParms, &parms[0]));
	dummy = dummy & ~flags;
	PXL_ERROR_CHECK(PxLSetFeature(m_handle, featureId, dummy, nParms, &parms[0]));
}


/**
* Function: SetGpoFlags
* Purpose:  Special case of SetFlags, for FEATURE_GPIO, because GPOs are
*           getted and setted (?) one at a time.
*/
void 
CPxLCamera::SetGpoFlags(const U32 flags, const int gpoNumber)
{
	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(FEATURE_GPIO)

	U32 nParms = GetFeatureParamCount(FEATURE_GPIO);
	U32 dummy = 0;

	std::vector<float> parms(nParms, 0.0f);
	parms[FEATURE_GPIO_PARAM_GPIO_INDEX] = static_cast<float>(gpoNumber);

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, FEATURE_GPIO);

	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_GPIO, &dummy, &nParms, &parms[0]));
	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_GPIO, flags, nParms, &parms[0]));
}


/******************************************************************************
* Descriptors
******************************************************************************/

/**
* Function: GetDescriptorCount
* Purpose:  Return the number of descriptors currently defined for the
*           active camera.
*/
int 
CPxLCamera::GetDescriptorCount(void)
{
	return m_descriptors.size();
}


/**
* Function: SetDescriptorUpdateMode
* Purpose:  Set the active descriptor into either Camera or Host mode.
*/
void 
CPxLCamera::SetDescriptorUpdateMode(const int mode)
{
	ASSERT(mode == PXL_UPDATE_CAMERA || mode == PXL_UPDATE_HOST);
	ASSERT(m_inFocusDescIndex == FOCUS_NONE || m_descriptors.size() > 0);

	if (m_descriptors.size() == 0) {
		return;
	}

	HANDLE dh = (m_inFocusDescIndex == FOCUS_ALL)
				? NULL
				: m_descriptors[m_inFocusDescIndex].m_descriptorHandle;

	// The Descriptor functions require that the stream be stopped.
	CStreamState tempState(this, STOP_STREAM, true);

	PXL_ERROR_CHECK(PxLUpdateDescriptor(m_handle, dh, mode));

	if (m_inFocusDescIndex == FOCUS_ALL)
	{
		for (DescriptorList_t::iterator it = m_descriptors.begin();
				it != m_descriptors.end(); it++)
		{
			it->m_updateMode = mode;
		}
	}
	else
	{
		m_descriptors[m_inFocusDescIndex].m_updateMode = mode;
	}
}


/**
* Function: GetDescriptorUpdateMode
* Purpose:  Return the update mode (Camera or Host) of the "in focus"
*           descriptor for the active camera.
*/
int 
CPxLCamera::GetDescriptorUpdateMode(void)
{
	if (m_inFocusDescIndex == FOCUS_NONE) {
		return -1;
	}

	ASSERT(m_descriptors.size() > 0);
	if (m_inFocusDescIndex == FOCUS_ALL) {
		return m_descriptors[0].m_updateMode;
	}

	return m_descriptors[m_inFocusDescIndex].m_updateMode;
}


/**
* Function: DeleteAllDescriptors
* Purpose:  Remove all descriptors from the active camera.
*/
void 
CPxLCamera::DeleteAllDescriptors(void)
{
	// The Descriptor functions require that the stream be stopped.
	CStreamState tempState(this, STOP_STREAM, true);

	PXL_ERROR_CHECK(PxLRemoveDescriptor(m_handle, NULL));

	m_descriptors.clear();
	m_inFocusDescIndex = FOCUS_NONE;
}


/**
* Function: CreateDescriptor
* Purpose:  Create a new descriptor for the active camera.
*/
void 
CPxLCamera::CreateDescriptor(const int updateMode)
{
	HANDLE hNewDescriptor;

	// The Descriptor functions require that the stream be stopped.
	CStreamState tempState(this, STOP_STREAM, true);

	PXL_ERROR_CHECK(PxLCreateDescriptor(m_handle, &hNewDescriptor, updateMode));

	// Add a new Descriptor object to the end of the list.
	m_descriptors.push_back(CPxLDescriptor(hNewDescriptor, updateMode));

	// A newly added descriptor is always in focus.
	m_inFocusDescIndex = m_descriptors.size() - 1;
}


/**
* Function: SetActiveDescriptor
* Purpose:  Put the specified descriptor in focus.
*/
void 
CPxLCamera::SetActiveDescriptor(const int descriptorNumber)
{
	if (descriptorNumber > 0 && descriptorNumber <= m_descriptors.size())
	{
		CPxLDescriptor& desc = m_descriptors.at(descriptorNumber - 1);

		// The Descriptor functions require that the stream be stopped.
		CStreamState tempState(this, STOP_STREAM, true);

		// We make the specified descriptor active (aka "In Focus") by calling 
		// PxLUpdateDescriptor, using the update mode that the descriptor is
		// already in. This way we are not changing the update mode - we are
		// just putting the descriptor into focus.
		try
		{
			PXL_ERROR_CHECK(PxLUpdateDescriptor(m_handle, desc.m_descriptorHandle, desc.m_updateMode));
			m_inFocusDescIndex = descriptorNumber - 1;
		}
		catch (PxLException const& e)
		{
			// XXX - try to deal with this better?
			if (e.GetReturnCode() == ApiInvalidParameterError)
			{
				m_descriptors.erase(m_descriptors.begin() + descriptorNumber - 1);

				// Now we don't know which descriptor is in focus. Guess.
				if (m_descriptors.size() > 0)
					m_inFocusDescIndex = 0;
				else
					m_inFocusDescIndex = FOCUS_NONE;
			}
			else
			{
				// Something else is wrong - propagate the exception.
				throw;
			}
		}
	}
}


/**
* Function: RemoveDescriptor
* Purpose:  Remove the currently active Descriptor.
*/
void 
CPxLCamera::RemoveDescriptor(void)
{
	if (m_inFocusDescIndex == FOCUS_NONE) {
		return;
	}

	ASSERT(m_descriptors.size() > 0);

	if (m_inFocusDescIndex == FOCUS_ALL)
	{
		DeleteAllDescriptors();
		return;
	}

	// The Descriptor functions require that the stream be stopped.
	CStreamState tempState(this, STOP_STREAM, true);

	CPxLDescriptor& desc = m_descriptors.at(m_inFocusDescIndex);

	PXL_ERROR_CHECK(PxLRemoveDescriptor(m_handle, desc.m_descriptorHandle));

	m_descriptors.erase(m_descriptors.begin() + m_inFocusDescIndex);

	// When the API removes a descriptor, the first descriptor gets the focus.
	if (m_descriptors.size() > 0) {
		m_inFocusDescIndex = 0;
	} else {
		m_inFocusDescIndex = FOCUS_NONE;
	}
}


/**
* Function: GetInFocusDescriptorNumber
* Purpose:  
*/
int 
CPxLCamera::GetInFocusDescriptorNumber(void)
{
	return 1 + m_inFocusDescIndex;
}






/******************************************************************************
* FFC Stuff - This is temporary code - eventually, we should add FFC control
* to the API (or at least make a more programmer-friendly way of doing it).
******************************************************************************/

#define QSWAP(x)	((x >> 24) | ((x >> 8) & 0xFF00) | ((x & 0xFF00) << 8) | (x << 24))

#define DEBUG_BLOCK_READ					2
#define DEBUG_BLOCK_WRITE					3
#define PIXELINK_DEBUG_OFFSET_HIGH			0xFFFF
#define PIXELINK_DEBUG_OFFSET_LOW			0xF0EE0000
#define COMMAND_FFC_SETTINGS_READ           0x00008001
#define COMMAND_FFC_SETTINGS_WRITE          0x00008002
struct FFC_SETTINGS
{
    ULONG Command;
    ULONG ffcInfo;
    FLOAT FfcCalibratedGain;
};

#define IS_FFC_ON(ffcInfo)	(((ffcInfo) & 0xF) == 0) ? false : true
#define GET_FFC_TYPE(ffcInfo)	(((ffcInfo) & 0xF0) >> 4)
#define FFC_TYPE_UNKNOWN			0
#define FFC_TYPE_UNCALIBRATED		1
#define	FFC_TYPE_FACTORY			2
#define	FFC_TYPE_FIELD				3

#define PXLDBG_REQUEST_READMEMORY 0xEE008002
#define PXLDBG_MEMORY_BYTE_ACCESS 0x0000C001
#define PXLDBG_MEMORY_WORD_ACCESS 0x0000C002
#define PXLDBG_MEMORY_LONG_ACCESS 0x0000C004
struct MEMORY_ACCESS
{
	ULONG Command;
	ULONG Address;
	ULONG Range;
	ULONG AccessType;
};


PXL_RETURN_CODE
CPxLCamera::DebugRequestSend(UCHAR* const dataPointer, const ULONG inputSize, const ULONG outputSize)
{
	ASSERT(0 != m_handle);
	PXL_RETURN_CODE rc = PxLCameraWrite(m_handle, (inputSize + 3) & ~0x3, dataPointer);
	if (!API_SUCCESS(rc)) { return rc; }

	rc = PxLCameraRead(m_handle, outputSize, dataPointer);
	return rc;
}

bool 
CPxLCamera::ReadMemory(const ULONG addr, const ULONG nbytes, UCHAR* const buffer)
{
	if (nbytes % 4) { return false; }

	ULONG total_size = sizeof(MEMORY_ACCESS) + nbytes;
	std::vector<UCHAR> buf(total_size);
	MEMORY_ACCESS* memAcc = reinterpret_cast<MEMORY_ACCESS*>(&buf[0]);
	memAcc->Command = PXLDBG_REQUEST_READMEMORY;
	memAcc->Address = addr;
	memAcc->Range = nbytes;
	memAcc->AccessType = PXLDBG_MEMORY_BYTE_ACCESS;
	PXL_RETURN_CODE rc = DebugRequestSend(&buf[0], total_size, total_size);
	if (!API_SUCCESS(rc)) { 
		return false;
	}
	::memcpy(buffer, &buf[sizeof(MEMORY_ACCESS)], nbytes);
	return true;
}

static const CString FFC_NO_DATA("No FFC Data");
static const CString FFC_FACTORY("Factory");
static const CString FFC_FIELD("Field");
static const CString FFC_UNKNOWN("Unknown");

CString 
CPxLCamera::GetFFCType(void)
{
	// Does this camera support the newer FFC query which indicates the type in the ffcInfo field?
	FFC_SETTINGS buffer = {0, 0, 0.0f};
	buffer.Command = COMMAND_FFC_SETTINGS_READ;

	PXL_RETURN_CODE rc = DebugRequestSend(reinterpret_cast<UCHAR*>(&buffer), sizeof(buffer), sizeof(buffer));
	if (!API_SUCCESS(rc)) {
	    // Issue.Bugzilla.43 -- Don't treat this as an error
	    return FFC_NO_DATA;
		//THROW_PXL_EXCEPTION(rc);
	}

	switch(GET_FFC_TYPE(buffer.ffcInfo)) {
		case FFC_TYPE_UNKNOWN:		
			if (buffer.FfcCalibratedGain == -1) {
				return FFC_NO_DATA;
			}
			// Old query. Try the old way
			break;
		case FFC_TYPE_UNCALIBRATED:	return FFC_NO_DATA;
		case FFC_TYPE_FACTORY:		return FFC_FACTORY;
		case FFC_TYPE_FIELD:		return FFC_FIELD;
		default:					THROW_PXL_EXCEPTION(ApiUnknownError); // Even newer protocol?
	}

	// Try the old way
	ULONG addr = 0;
	switch (this->GetModelNumber())
	{
	case 741:
	case 742:
		addr = 0x083c0418;
		break;
	case 682:
	case 686:
	case 781:
	case 782:
		addr = 0x08fc0418;
		break;
	case 622:
	case 623:
	case 774:
	case 776:
		addr = 0x08fc0418;
		break;
	}
	ULONG ffctype = 0;
	if (addr != 0 && this->ReadMemory(addr, 4, (UCHAR*)&ffctype))
	{
		USHORT type = (USHORT)ffctype;
		switch(type) {
			case 0x1111:	return FFC_FACTORY;
			case 0x2222:	return FFC_FIELD;
			case 0xFFFF:	return FFC_NO_DATA;
		}
	}
	return FFC_UNKNOWN;
}

bool 
CPxLCamera::SupportsManualFFC(void)
{
	// Attempt to read the FFC state, and assume that an error code means that
	// the camera does not support manual FFC control (eg: old firmware version).
	FFC_SETTINGS buffer = {0, 0, 0.0f};
    buffer.Command = COMMAND_FFC_SETTINGS_READ;

	PXL_RETURN_CODE rc = DebugRequestSend(reinterpret_cast<UCHAR*>(&buffer), sizeof(buffer), sizeof(buffer));
	if (!API_SUCCESS(rc)) {
		return false;
	}

	switch(GET_FFC_TYPE(buffer.ffcInfo)) {
		case FFC_TYPE_UNKNOWN:		
			// Must be old ffc protocol
			if (buffer.FfcCalibratedGain == -1) {
				return false;
			} else {
				return true;
			}
			ASSERT(0);

		case FFC_TYPE_UNCALIBRATED:
			return false;

		case FFC_TYPE_FACTORY:
		case FFC_TYPE_FIELD:
			return true;
	}

	THROW_PXL_EXCEPTION(ApiUnknownError); // Even newer protocol?
	return false;
}


void 
CPxLCamera::EnableFfc(const bool enable)
{
	FFC_SETTINGS buffer = {0, 0, 0.0f};
	buffer.Command = COMMAND_FFC_SETTINGS_WRITE;
	buffer.ffcInfo = enable ? 1 : 0;

	// Send the command
	PXL_RETURN_CODE rc = DebugRequestSend(reinterpret_cast<UCHAR*>(&buffer), sizeof(buffer), sizeof(buffer));
	if (!API_SUCCESS(rc)) 
	{
		THROW_PXL_EXCEPTION(rc);
	}
}


bool 
CPxLCamera::IsFfcOn(void)
{
	FFC_SETTINGS buffer = {0, 0, 0.0f};
    buffer.Command = COMMAND_FFC_SETTINGS_READ;

    // Send the command
    PXL_RETURN_CODE rc = DebugRequestSend(reinterpret_cast<UCHAR*>(&buffer), sizeof(buffer), sizeof(buffer));
	if (!API_SUCCESS(rc))
    {
		THROW_PXL_EXCEPTION(ApiUnknownError);
    }

	bool enabled = IS_FFC_ON(buffer.ffcInfo);
    return enabled;
}


float 
CPxLCamera::GetFfcCalibratedGain(void)
{
	FFC_SETTINGS buffer = {0, 0, 0.0f};
    buffer.Command = COMMAND_FFC_SETTINGS_READ;

    // Send the command
    PXL_RETURN_CODE rc = DebugRequestSend(reinterpret_cast<UCHAR*>(&buffer), sizeof(buffer), sizeof(buffer));
	if (!API_SUCCESS(rc))
    {
		THROW_PXL_EXCEPTION(ApiUnknownError);
    }

    return buffer.FfcCalibratedGain;
}


const CAMERA_ID_INFO* 
CPxLCamera::GetCameraIdInfo() const
{
	return &m_cameraIdInfo;
}

void
CPxLCamera::SetCameraIdInfo(CAMERA_ID_INFO& cameraIdInfo)
{
	m_cameraIdInfo = cameraIdInfo;
}


//
// Some of the commands supported by PxLPrivateCmd()
//
enum {
	PRIVATE_COMMAND_ENABLE_JUMBO_FRAMES    = 37,
	PRIVATE_COMMAND_DISABLE_JUMBO_FRAMES   = 38,
	PRIVATE_COMMAND_GIGE_HEARTBEAT_TIMEOUT = 40,
};


//
// All GigE cameras initialized after jumbo frames are enabled will have jumbo frames.
// All GigE cameras initialized after jumbo frames are disabled will not have jumbo frames.
//
bool
CPxLCamera::EnableJumboFrames(const bool enable)
{
	HANDLE hCamera = NULL;
	U32 opcode = (enable) ? PRIVATE_COMMAND_ENABLE_JUMBO_FRAMES : PRIVATE_COMMAND_DISABLE_JUMBO_FRAMES;
	PXL_RETURN_CODE rc = PxLPrivateCmd(hCamera, sizeof(opcode), &opcode);
	return API_SUCCESS(rc);
}

/**
* Function: GetPolarWeightings
* Purpose:  Return a vector<float> that defines the value of each of the polar channels.
*/
std::vector<float> 
CPxLCamera::GetPolarWeightings(void)
{
	std::vector<float> weightings;

	if (!FeatureSupported(FEATURE_POLAR_WEIGHTINGS))
	{
		TRACE("Invalid call to CPxLCamera::GetPolarWeightings\n");
		// It is the responsibility of the caller to ensure that he is getting
		// a valid return value from this function - usually by testing the
		// return value of FeatureSupported(FEATURE_POLAR_WEIGHTINGS) before calling this.
		return weightings;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = GetFeatureParamCount(FEATURE_POLAR_WEIGHTINGS);

	// Allocate memory.
	weightings.resize(nParms);

	// Get the values of the parameters of the feature.
	PXL_ERROR_CHECK(PxLGetFeature(m_handle, FEATURE_POLAR_WEIGHTINGS, &flags, &nParms, &weightings[0]));

	return weightings;
}


/**
* Function: SetPolarWeightings
* Purpose:  
*/
void 
CPxLCamera::SetPolarWeightings(const std::vector<float> weightings)
{
	if (!FeatureSupportsManual(FEATURE_POLAR_WEIGHTINGS))
	{
		TRACE("Invalid call to CPxLCamera::SetPolarWeightings");
		return;
	}

	U32 flags = FEATURE_FLAG_MANUAL;
	U32 nParms = weightings.size(); //GetFeatureParamCount(FEATURE_POLAR_WEIGHTINGS);

	// The video stream must be stopped to change some features.
	STOP_STREAM_IF_REQUIRED(FEATURE_POLAR_WEIGHTINGS)

	// See comment in SetFeatureValue, above.
	PostViewChange(CT_FeatureChanged, FEATURE_POLAR_WEIGHTINGS);

	PXL_ERROR_CHECK(PxLSetFeature(m_handle, FEATURE_POLAR_WEIGHTINGS, flags, nParms, &weightings[0]));
}

/**
* Function: SetPolarWeightings
* Purpose:  
*/
double
CPxLCamera::GetTimestamp()
{
    double currentTimeInSeconds;
	PXL_ERROR_CHECK(PxLGetCurrentTimestamp(m_handle, &currentTimeInSeconds));
    return currentTimeInSeconds;
}

/**
* Function: IsActionsSupported
* Purpose:  
*/
bool
CPxLCamera::IsActionsSupported()
{
    U32 numberSupportedActions = 0;
    PXL_RETURN_CODE rc = PxLGetActions (m_handle, NULL, &numberSupportedActions);
    return ((API_SUCCESS (rc) || rc == ApiBufferTooSmall)
            && numberSupportedActions > 0) ;
}

