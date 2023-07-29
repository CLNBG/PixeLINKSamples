// PxLCameraDoc.cpp : implementation of the CPxLCameraDoc class
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "PxLCameraDoc.h"
#include "PxLDevAppView.h"
#include "Helpers.h"
#include "AboutDlg.h"
#include "ClipCaptureDialog.h"
#include "ImageCaptureDialog.h"
#include "HistogramDialog2.h"
#include "SetCameraNameDialog.h"
#include "LoadSettingsDialog.h"
#include "SaveSettingsDialog.h"
#include "SetIpAddressDialog.h"
#include <shlwapi.h>
#include <stdexcept>
#include <algorithm>
#include <set>
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Place at the start of function body to require that m_pActiveCamera != NO_CAMERA
#define REQUIRES_CAMERA													\
	if (m_pActiveCamera == NO_CAMERA) PxLErrorCheck(ApiNoCameraError);	\


/******************************************************************************
* Class CPxLCameraDoc
******************************************************************************/

IMPLEMENT_DYNCREATE(CPxLCameraDoc, CDocument)


BEGIN_MESSAGE_MAP(CPxLCameraDoc, CDocument)

	ON_COMMAND(ID_FILE_IMPORTCONFIGFILE, OnFileImportConfigFile)
	ON_COMMAND(ID_FILE_EXPORTCONFIGFILE, OnFileExportConfigFile)
	ON_COMMAND(ID_FILE_SETIPADDRESS,     OnFileSetIPAddress)
	ON_COMMAND(ID_VIEW_RESETPREVIEWWINDOW, OnViewResetPreviewWindow)
	ON_COMMAND(ID_VIEW_HISTOGRAM, OnViewHistogram)
	ON_COMMAND(ID_CAMERA_LOADCAMERASETTINGS, OnCameraLoadCameraSettings)
	ON_COMMAND(ID_CAMERA_SAVECAMERASETTINGS, OnCameraSaveCameraSettings)
	ON_COMMAND(ID_CAMERA_SETCAMERANAME, OnCameraSetCameraName)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)

	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORTCONFIGFILE, OnUpdateFileImportConfigFile)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTCONFIGFILE, OnUpdateFileExportConfigFile)
	ON_UPDATE_COMMAND_UI(ID_FILE_SETIPADDRESS, OnUpdateFileSetIPAddress)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESETPREVIEWWINDOW, OnUpdateViewResetPreviewWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTOGRAM, OnUpdateViewHistogram)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_LOADCAMERASETTINGS, OnUpdateCameraLoadCameraSettings)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_SAVECAMERASETTINGS, OnUpdateCameraSaveCameraSettings)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_SETCAMERANAME, OnUpdateCameraSetCameraName)

	ON_COMMAND(ID_VIEW_AUTORESETPREVIEW, OnViewAutoResetPreview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AUTORESETPREVIEW, OnUpdateViewAutoResetPreview)

	ON_COMMAND(ID_LAPTOPFRIENDLYMODE_OFF, OnLaptopfriendlymodeOff)
	ON_COMMAND(ID_LAPTOPFRIENDLYMODE_2, OnLaptopfriendlymode2)
	ON_COMMAND(ID_LAPTOPFRIENDLYMODE_3, OnLaptopfriendlymode3)
	ON_COMMAND(ID_LAPTOPFRIENDLYMODE_4, OnLaptopfriendlymode4)
	ON_UPDATE_COMMAND_UI(ID_LAPTOPFRIENDLYMODE_OFF, OnUpdateLaptopfriendlymodeOff)
	ON_UPDATE_COMMAND_UI(ID_LAPTOPFRIENDLYMODE_2, OnUpdateLaptopfriendlymode2)
	ON_UPDATE_COMMAND_UI(ID_LAPTOPFRIENDLYMODE_3, OnUpdateLaptopfriendlymode3)
	ON_UPDATE_COMMAND_UI(ID_LAPTOPFRIENDLYMODE_4, OnUpdateLaptopfriendlymode4)

	ON_COMMAND(ID_LAPTOPFRIENDLYMODE_8, OnLaptopfriendlymode8)
	ON_UPDATE_COMMAND_UI(ID_LAPTOPFRIENDLYMODE_8, OnUpdateLaptopfriendlymode8)

    ON_COMMAND(ID_CAMERA_SLOWIMAGERMODE, OnSlowImagerMode)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_SLOWIMAGERMODE, OnUpdateSlowImagerMode)

    ON_COMMAND(ID_CAMERA_FIXEDFRAMERATEMODE, OnFixedFramerateMode)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_FIXEDFRAMERATEMODE, OnUpdateFixedFramerateMode)
END_MESSAGE_MAP()


// This is used as a place holder - when there is no camera initialized, the
// m_pActiveCamera member of the Document points to this instance, instead of
// to NULL. This way, if we call GetActiveCamera()->SomeApiFunction(), we get
// either a PxLException of some sort, or a reasonable return value (eg:
// FeatureSupported -> false), instead of the nasty access violation that we
// would get if we just set m_pActiveCamera to NULL.
// Note: We have to be careful to never call Initialize on this object!
CPxLCamera _unintialized_camera(0);
CPxLCamera * const NO_CAMERA = &_unintialized_camera;


/**
* Function: CPxLCameraDoc
* Purpose:  Constructor
*/
CPxLCameraDoc::CPxLCameraDoc()
	: CDocument()
	, m_pActiveCamera(NO_CAMERA)
	, m_pOpenClipCaptureDlg(NULL)
	, m_bDriversLoading(false)
	, m_CameraPoller(NULL)
{
        m_CameraPollerStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL); //Issue.software.259 - Don't use manual clear events   
}


/**
* Function: CPxLCameraDoc
* Purpose:  Destructor
*/
CPxLCameraDoc::~CPxLCameraDoc()
{
	CloseHandle (m_CameraPollerStopEvent);
	
	// Uninitialize all cameras
	for (CameraMap_t::iterator it = m_cameras.begin(); it != m_cameras.end(); it++)
	{
		try
		{
			delete it->second;
		}
		catch (PxLException const&)
		{
			// At this point, we don't care much about errors. Ignore it.
		}
	}
}


/**
* Function: OnCloseDocument
* Purpose:  
*/
void CPxLCameraDoc::OnCloseDocument()
{
	CDocument::OnCloseDocument();
}


/**
* Function: OnApiError
* Purpose:  General purpose error handler for PxLAPI errors.
*/
void CPxLCameraDoc::OnApiError(PxLException const& e, bool cameraRelated)
{
	static int recursionDepth = 0;
	TempIncr<int> ti(recursionDepth);
	if (recursionDepth > 5)
		return;

	// In the case of certain errors, we attempt to do something slightly more
	// useful than just displaying a message.  For the most part, however,
	// there is not much more we can do - there are just way too many possible
	// sources of error.
	switch (e.GetReturnCode())
	{
	case ApiInvalidHandleError:
	case ApiHardwareError:
    case ApiIOError:
    case ApiCameraTimeoutError:
    case ApiRequiresControlAccess:
		// Errors that may have been caused by the active camera being unplugged.
		// Try to recover gracefully.
		if (HasActiveCamera())
		{
			// We *should* have a valid active camera. Ping it to see
			// whether all is well (The error may have been caused by
			// something else besides the camera being disconnected).
			try
			{
				PingCamera();
                if (e.GetReturnCode() == ApiRequiresControlAccess &&
                    ! theApp.m_bMonitorAccessOnly)
                {
                    /// Hmmm... the original error indicated that we could not write to the
                    // camera, yet it is still there.  Most likely, this camera has reset
                    // Perform a reinitialize
                    throw;
                }
			}
			catch (PxLException const&)
			{
				// PingCamera threw an exception. We *assume* that this is
				// because the camera is no longer connected, or was disconnected
				// and then reconnected, invalidating our camera handle.
				// We need to check to see if the camera is still connected.
				if (IsCameraConnected(m_pActiveCamera->GetSerialNumber()))
				{
					// The camera is connected. Assume it was  unplugged and then
					// re-connected, invalidating its handle. Try reinitializing.
					try
					{
						U32 sn = m_pActiveCamera->GetSerialNumber();
						RemoveActiveCamera();
						SetActiveCamera(sn); // Might throw
                        // We may have been streaming the camera before the disconnect/reconnect.  Put the camera
                        // into a known streaming state.
                        m_pActiveCamera->SetStreamState(STOP_STREAM);
						CString msg;
						msg.Format(_T("Camera %lu had to be reinitialized - it may have been temporarily disconnected.\n")
									_T("Camera settings may have reverted to their default values."), sn);
						ErrorMessage(msg);
					}
					catch (PxLException const& e)
					{
						// Failed to re-initialize the camera. At this point we give up.
						ErrorMessage(ReturnCodeAsString(e.GetReturnCode()));
						SetActiveCamera(0); // Try to make some other camera active.
					}
				}
				else
				{
					// The camera is no longer connected.
					// NEW - 2004-Sept.23 - CP - When there are no other cameras
					// plugged in, don't prompt the user to reconnect. In this case,
					// the reconnect dialog is not really useful, and is kind of annoying.
					// However, when there is some other camera plugged in, we still show
					// the reconnect dialog, because the alternative would be to just
					// silently switch to some other camera, and the user might not
					// notice the change.
					if (this->IsCameraConnected(0))
					{
						// Some other camera is connected.
						int msgBoxRetCode;
						CString msg;
						msg.Format(_T("Camera %lu is no longer connected.\n")
									_T("Reconnect the camera and then click OK."),
									m_pActiveCamera->GetSerialNumber());
						while (IDOK == (msgBoxRetCode=AfxMessageBox(msg, MB_OKCANCEL)))
						{
							// User clicked OK, meaning that they have (hopefully) re-connected
							// the camera. Try to initialize it.
							try
							{
								m_pActiveCamera->Reinitialize();
								DoViewChange(CT_NewCamera);
								break;
							}
							catch (PxLException const&)
							{
								// Keep trying until either the camera is successfully
								// initialized, or the user clicks "Cancel".
								continue;
							}
						}
						if (msgBoxRetCode == IDCANCEL)
						{
							// User clicked Cancel. Try to initialize some other camera.
							RemoveActiveCamera();
							SetActiveCamera(0);
						}
					}
					else
					{
						RemoveActiveCamera();
						SetActiveCamera(0);
					}
				}

				break;

			}
			// If we get here, PingCamera did not throw an exception, ...
		}
		// ...and if we get here, there is no Active Camera.
		// Either way, we just revert to default error handling.
		m_pActiveCamera->OnApiError(e, cameraRelated);
		break;

	// Handle more special cases here...

	default:
		// Let the Camera object try to handle it
		m_pActiveCamera->OnApiError(e, cameraRelated);
	}
}


/**
* Function: HandleMessage
* Purpose:  Handle application-specific windows messages.
*           These messages are actually forwarded here from the CMainFrame
*           class, since this class is not a window (has no HWND), and can't
*           receive windows messages directly.
*/
void CPxLCameraDoc::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{	
    if (message == WM_DEVICECHANGE)
	{
		// This message is broadcast by the operating system whenever a device is added or removed from the system. 
		// The PixeLINK API also takes action on this event.
		// Therefore, rather than take action on this message, we'll post a seperate message to ourselves, in order to 
		// to give an the API an opportunity to act first.
		TRACE("WM_DEVICECHANGE(0x%8.8X,0x%8.8X)\n", wParam, lParam);
		CWnd* wnd = theApp.GetMainWnd();
		if (wnd != NULL) {
			PostMessage(wnd->m_hWnd, WM_APP_DEFERRED_DEVICE_CHANGED, wParam, lParam);
		}
        return;
    }    

	// The only other messages we should receive are private messages (eg: WM_APP + 0x0001)
	ASSERT(message > WM_APP);

	// Most of our WM_APP messages have a CPxLCamera* as the wParam.
	CPxLCamera* pCam = reinterpret_cast<CPxLCamera*>(wParam);

    switch (message)
    {
		case WM_APP_DEFERRED_DEVICE_CHANGED:
        {	
            static bool handlingDevChange = false;
			if (handlingDevChange) {
			    return;
			}

		    TempVal<bool> temp(handlingDevChange, true);

		    try
		    {
			    if (HasActiveCamera())
			    {
				    // There is an active camera. Make sure it's still there. The
				    // WM_DEVICECHANGE message may be because it was unplugged.
				    // Note: PingCamera() throws an exception only if the camera has
				    // been disconnected (or at least, that is the intention - it is
				    // almost certainly not foolproof).
				    PingCamera();
			    }
			    else
			    {
				    // We are in the "No Cameras Connected" state.
				    // Check to see whether any cameras have been connected yet.
					// Bugzilla 197
					TRACE("Attempting to detect new camera...\n");
					U32 numCameras(0);
					bool isCameraDetected = false;
					int numTries = 0;
					do {
						Sleep(250); 
						PXL_RETURN_CODE rc = PxLGetNumberCameras(NULL, &numCameras);
						if (API_SUCCESS(rc)) {
							if (numCameras > 0) {
								isCameraDetected = true;
								break;
							}
						}
						numTries++;
					} while(numTries < 4); 
					CString msg;
					msg.Format("After %d tries, num cameras detected: %d\n", numTries, numCameras);
					TRACE(msg);
					if (isCameraDetected) {
						SetActiveCamera(0);
					}
			    }
			    // TODO - also check up on other initialized cameras - ones that are not
			    //        the active camera, but have a preview or histogram window open.
		    }
		    catch (PxLException const& e)
		    {
			    OnApiError(e);
		    }
		    return;
        }
        break;  // just for completeness

    case WM_APP_FEATURE_CHANGED:
		// We ignore this message if it didn't come from the active camera.
		if (pCam == m_pActiveCamera) {
			DoViewChange(static_cast<U32>(lParam));
		}
        break;

    case WM_APP_PREVIEW_ACTIVATED:
    case WM_APP_PREVIEW_CLOSED:
	    {
		    bool ptrOK = false;

		    // Because these messages are posted from other threads, there is a
		    // chance that the pointer is no longer valid. To be safe, we match
		    // it to one in the collection of initialized cameras.
		    for (CameraMap_t::iterator it = m_cameras.begin(); it != m_cameras.end(); it++)
		    {
			    if (pCam == it->second)
			    {
				    ptrOK = true;
				    break;
			    }
		    }

			if (!ptrOK) {
			    return;
			}

		    if  (message == WM_APP_PREVIEW_CLOSED)
    		{
    			// Notification that a preview window was closed. These messages are
	    		// sent from the PreviewMonitor thread (see PxLCamera.cpp).
		    	try
			    {
				    pCam->StopPreview();
			    }
			    catch (PxLException const&)
			    {
				    // No Error message - the window is already gone, so there's no need.
			    }
            } else {
                ASSERT(message == WM_APP_PREVIEW_ACTIVATED);

    			// Notification that a preview window was made the active window.
	    		// Make the associated camera the active camera.
		    	// This message is also sent from the PreviewMonitor thread.
			    SetActiveCamera(pCam->GetSerialNumber());
		    }
        }
        break;

    case WM_APP_DEVICE_RESET:
        // Let's make it very clear to the user that the device reset, by indicating no camera until the next enumeration
        // cycle finds one.
		m_pActiveCamera->Reinitialize();
		DoViewChange(CT_NewCamera);
        break;

    case WM_APP_DEVICE_DISCONNECTED:
        RemoveActiveCamera();
	    SetActiveCamera(0); 
        break;

    default:
		// It's a message that we shouldn't be getting.
		ASSERT(FALSE);
    }
}


/**
* Function: DoViewChange
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
void CPxLCameraDoc::DoViewChange(U32 changeId)
{
	UpdateAllViews(NULL, static_cast<LPARAM>(changeId), NULL);
    if (m_pActiveCamera != NO_CAMERA && m_pActiveCamera->m_pHistogramWindow)
    {
        // Let the histrogram know about feature changes
        m_pActiveCamera->m_pHistogramWindow->FeatureChanged (changeId - CT_FeatureChanged);
    }
}


/**
* Function: GetGUIState
* Purpose:  Return a value that specifies the state of the active camera - or
*           whether there is an active camera. The different pages that make
*           up the GUI call this when they are activated in order to determine
*           whether they need to enable / disable / configure their controls.
*/
eGUIState CPxLCameraDoc::GetGUIState(void)
{
	if (m_pActiveCamera == NO_CAMERA)
		return  GS_NoCamera;

	switch (m_pActiveCamera->GetPreviewState())
	{
	case START_PREVIEW:
		return GS_PreviewPlaying;
	case PAUSE_PREVIEW:
		return GS_PreviewPaused;
	case STOP_PREVIEW:
		return GS_CameraSelected;
	default:
		ASSERT(FALSE);
	}
	return GS_NoCamera;
}


/**
* Function: UpdateCameraList
* Purpose:  Fill a combo box with a list of the available cameras.
*           This is a helper function that has been put here in the Document
*           class for convenience, because it is used by many of the pages.
*/
void CPxLCameraDoc::UpdateCameraList(CComboBox& cb, bool reloadSerialNumbers)
{
	// Note the serial number of the item that is currently selected, so
	// that we can re-select it at the end of this function.
	U32 selectedSN = 0;
	if (cb.GetCurSel() != CB_ERR)
		selectedSN = ItemData<U32>(cb);

	// Make sure that the vector m_serialNumbers contains an up-to-date list
	// of connected cameras.
	if (reloadSerialNumbers) {
		LoadSerialNumbers();
	}

	// Remove combo-box entries for cameras that are no longer connected.
	int i;
	for (i = 0; i < cb.GetCount(); )
	{
		U32 sn = static_cast<U32>(cb.GetItemData(i));
		if (contains(m_serialNumbers, sn))
			i++;
		else
			cb.DeleteString(i);
	}

	std::vector<U32> temp(m_serialNumbers); // Copy of serial number list.
	// Remove from temp any cameras that are already listed in the combo box - 
	// we don't want to add them again.
	for (i = 0; i < cb.GetCount(); i++)
	{
		U32 sn = static_cast<U32>(cb.GetItemData(i));
		if (contains(temp, sn))
			temp.erase(std::find(temp.begin(), temp.end(), sn));
	}
	// Add combo box items for any entries that are still left in temp.
	for (i = 0; i < temp.size(); i++)
	{
		int j;
		CString s;
		s.Format(_T("PixeLINK Camera s/n : %lu"), temp[i]);
		// Find the right place in the combo box to insert it, to maintain
		// numerical order. This makes it easier to find the camera you are
		// looking for if you have a lot of them connected.
		for (j = 0; j < cb.GetCount(); j++)
		{
			U32 sn = static_cast<U32>(cb.GetItemData(j));
			if (sn > temp[i])
				break;
		}
		cb.InsertString(j, s);
		cb.SetItemData(j, static_cast<DWORD_PTR>(temp[i]));
	}
		// Note: if you 

	// If there are no cameras connected, add an item that says so.
	if (m_serialNumbers.size() == 0)
	{
		cb.AddString(_T("No Cameras are Connected"));
		cb.SetItemData(0, 0);
		// No camers.  Start a poller if there sin't one already
		if (NULL == m_CameraPoller)
		{
		    StartCameraPoller();
		}
	}

	// Re-Select the item that was previously selected. We need to do this
	// explicitly, because there may have been item added to the list before
	// it, so it may no longer have the same index amongst the items.
	SelectByItemData(cb, selectedSN);

}


/**
* Function: GetActiveCamera
* Purpose:  Return a pointer to the active CPxLCamera object.
*           Thows an exception if there is no active camera.
*/
CPxLCamera* CPxLCameraDoc::GetActiveCamera(void)
{
	return m_pActiveCamera;
}


/**
* Function: RemoveActiveCamera
* Purpose:  
*/
void CPxLCameraDoc::RemoveActiveCamera(void)
{
	U32 sn = m_pActiveCamera->GetSerialNumber();
	CPxLCameraDoc::CameraMap_t::iterator iter = m_cameras.find(sn);
	if (iter != m_cameras.end())
	{
		delete iter->second; //iter->second->Uninitialize(); // CPxLCamera::Uninitialize does not throw exceptions.
		m_cameras.erase(iter);
	}
	m_pActiveCamera = NO_CAMERA;
}


/**
* Function: SetActiveCamera
* Purpose:  Sets the active camera to be the one with the specified serial
*           number, or any camera if the serialNumber parameter is 0.
*/
void CPxLCameraDoc::SetActiveCamera(U32 serialNumber)
{
    U32 uCameraToActivate;
	const bool bDeactivateCurrentCamera = !theApp.m_bAllowConnectToMultipleCameras;
	U32 uCameraToDeactivate = m_pActiveCamera->GetSerialNumber();

    if (m_pActiveCamera != NO_CAMERA
		&& ( serialNumber == m_pActiveCamera->GetSerialNumber()
			|| serialNumber == 0 ))
	{
		// Either:
		// a) The specified camera is already the active one, or
		// b) serialNumber == 0 (any camera), and we already have one active.
		// Either way, there is nothing to be done here.
		return;
	}

	// Get an up-to-date list of serial numbers of connected cameras.
	LoadSerialNumbers();

    // If the user has not already done so, pick a camera to activate.
	if (serialNumber == 0) 
	{
		if (m_cameras.empty()) 
		{
            uCameraToActivate = 0;  // No cameras - use serial number 0
		}
		else
		{
            uCameraToActivate = m_cameras.begin()->first;  
		}
	}
	else
	{
        uCameraToActivate = serialNumber;
	}

	
	if (bDeactivateCurrentCamera) {
		// Make sure to uninitialize/disconnect from the old camera
		if ((uCameraToDeactivate != 0) && (uCameraToDeactivate != uCameraToActivate))
		{
            m_pActiveCamera->SetEventCallback (EVENT_CAMERA_DISCONNECTED, NULL, NULL); // Cancel the callback
	        m_pActiveCamera->Uninitialize();
		}
	}

	// Has a camera object for a camera with this serial number already been created?
	if (m_cameras.count(uCameraToActivate) == 0)
	{
		// The requested camera is not initialized, or the the user said grab any camera, and there were
        // none initialized to grab - we need to initialize one.
		std::auto_ptr<CPxLCamera> pCam(new CPxLCamera(uCameraToActivate, this));
		try
		{
			pCam->Initialize(); // Might throw.
			// Add the camera object to the std::map of cameras, using the serial number as the lookup key
			m_cameras[pCam->GetSerialNumber()] = m_pActiveCamera = pCam.release();

			// Give the camera object its camera info.
			// (We looked up the camera info when we loaded info for all cameras)
			for(size_t i=0; i < m_cameraIdInfo.size(); i++) {
				if (m_cameraIdInfo[i].CameraSerialNum == m_pActiveCamera->GetSerialNumber()) {
					m_pActiveCamera->SetCameraIdInfo(m_cameraIdInfo[i]);
					break;
				}
			}
		}
		catch (PxLException const&)
		{
			DoViewChange(CT_NoCamera);
			if (serialNumber != 0)
				throw;
		}
	}
	else if (uCameraToActivate != 0)
	{
		// Find the already-created camera object for this serial number
		try
		{
		    m_pActiveCamera = m_cameras[uCameraToActivate];
			ASSERT(NULL != m_pActiveCamera);
			// If we don't allow connecting to multiple cameras, 
			// the camera isn't initialized, so must do so here.
			if (!theApp.m_bAllowConnectToMultipleCameras) {
				m_pActiveCamera->Initialize();
			}
		}
		catch (PxLException const&)
		{
			DoViewChange(CT_NoCamera);
			if (serialNumber != 0) {
				throw;
			}
		}
	}
	else
	{
		// uCameraToActivate == 0
		// Pick a camera that's already initialized; choose the first one (like above)
		m_pActiveCamera = m_cameras.begin()->second;
	}

	// If we only allow connecting to one camera at a time, we have to
	// disconnect from the camera that we were previously connected to.
	if (bDeactivateCurrentCamera) {
		if ((uCameraToDeactivate != 0) && (uCameraToDeactivate != uCameraToActivate))
		{
            m_cameras[uCameraToDeactivate]->SetEventCallback (EVENT_CAMERA_DISCONNECTED, NULL, NULL); // Cancel the callback
	        m_cameras[uCameraToDeactivate]->Uninitialize();
		}
	}

	if (m_pActiveCamera == NO_CAMERA)
	{
		DoViewChange(CT_NoCamera);
	}
	else
	{

        // Enable the callback for the camera disconnect event
        m_pActiveCamera->SetEventCallback (EVENT_CAMERA_DISCONNECTED, this, CameraDisconnectCallback);

        // "Laptop Mode" is actually a property of the Camera object, but we
		// expose it to the user as if it were an application-global property,
		// so we need to make sure that the active camera is in the right state.
        if (m_pActiveCamera->FeatureSupported(FEATURE_IMAGER_CLK_DIVISOR)) {            
            // This camera supports this feature -- which is a better means to limit the frame rate, so
            //      turn off the laptop friendly mode (by setting it to one)
            SetLaptopMode (1);
        } else {
            SetLaptopMode (theApp.m_framerateDivisor);
        }

		switch (m_pActiveCamera->GetPreviewState())
		{
		case START_PREVIEW:
			DoViewChange(CT_NewCamera | CT_PreviewStarted);
			break;
		case PAUSE_PREVIEW:
			DoViewChange(CT_NewCamera | CT_PreviewPaused);
			break;
		default:
			DoViewChange(CT_NewCamera);
		}
	}

} // SetActiveCamera


/**
* Function: HasActiveCamera
* Purpose:  
*/
bool CPxLCameraDoc::HasActiveCamera(void)
{
	return m_pActiveCamera != NO_CAMERA;
}

/**
* Function: GetActiveSerialNumber
* Purpose:  
*/
U32 CPxLCameraDoc::GetActiveSerialNumber(void)
{
	if (m_pActiveCamera == NO_CAMERA)
		return 0;
	return m_pActiveCamera->GetSerialNumber();
}


/**
* Function: IsCameraConnected
* Purpose:  Determine whether the camera with the given serial number is
*           connected (but not neccessarily initialized). If the serialNumber
*           parameter is 0, return true if *any* camera is connected.
*/
bool CPxLCameraDoc::IsCameraConnected(U32 serialNumber)
{
	if (serialNumber == 0 && m_serialNumbers.size() > 0)
		return true;

	//if (!contains(m_serialNumbers, serialNumber))
		LoadSerialNumbers();

	if (serialNumber == 0 && m_serialNumbers.size() > 0)
		return true;

	return contains(m_serialNumbers, serialNumber);
}


/**
* Function: PingCamera
* Purpose:  Throw an exception if the active camera has been disconnected.
*/
void CPxLCameraDoc::PingCamera(CPxLCamera* pcam /*=NULL*/)
{
	if (pcam == NO_CAMERA) {
		return;
	}
	if (pcam == NULL) {
		pcam = m_pActiveCamera;
	}

	// This is a bit of a trick - we try to get FEATURE_SHUTTER, and if the
	// return code indicates an error that is due to the camera not being
	// there, then we pass this on to the normal, application error handling
	// mechanism, which already deals with this error in a special way.
	// Other error codes are ignored (eg: the camera might not even support
	// the Shutter feature, but we don't care about that - we just want to
	// know whether the camera is still connected or not).
	try
	{
		pcam->GetFeatureValue(FEATURE_SHUTTER);
	}
	catch (PxLException const& e)
	{
		switch (e.GetReturnCode())
		{
		case ApiInvalidHandleError:
		case ApiHardwareError:
        case ApiIOError:
        case ApiCameraTimeoutError:
			// Throw the exception up to the caller of PingCamera() (who had better
			// be calling it from inside a try block...)
			throw;
		default:
			// ignore
			break;
		}
	}
}


/**
* Function: UninitializeCamera
* Purpose:  Uniniialize the currently active camera, and configure the GUI
*           appropriately (eg: disable controls, etc.)
*/
void CPxLCameraDoc::UninitializeCamera(void)
{
	if (m_pActiveCamera == NO_CAMERA) // Already uninitialized.
		return;

    m_pActiveCamera->SetEventCallback (EVENT_CAMERA_DISCONNECTED, NULL, NULL); // Cancel the callback
	m_pActiveCamera->Uninitialize();
	m_cameras.erase(m_pActiveCamera->GetSerialNumber());
	delete m_pActiveCamera;

	// Notify the View that it should update to reflect the fact that
	// there is no active camera.
	DoViewChange(CT_NoCamera);

}


/**
* Function: CaptureImage
* Purpose:  Capture a still image and save it to a file.
*           The pathEdit parameter specifies the window (probably an Edit
*           control) that contains the full path of the file to save to.
*           The formatCombo is a combo box - the ItemData of the selected
*           entry specifies the format to save in.
*           Returns true if the image file is successfully saved.
*/
bool CPxLCameraDoc::CaptureImage(CWnd& pathEdit,
								 CComboBox& formatCombo,
								 int discardFrames /*=0*/,
								 int avgFrames /*=1*/,
								 bool autoExpose /*=false*/)
{
	REQUIRES_CAMERA

	// Data structures to hold the image data and the frame descriptor:
	std::vector<byte> imageData;
	std::auto_ptr<FRAME_DESC> pFrameDesc(new FRAME_DESC);
	pFrameDesc->uSize = sizeof(FRAME_DESC);
    int initialStreamState = m_pActiveCamera->GetStreamState();

	// AutoExpose if required.
	if (autoExpose)
	{
		m_pActiveCamera->SetOnePush(FEATURE_SHUTTER);
	}

	m_pActiveCamera->GetNextFrame(imageData, pFrameDesc.get(), discardFrames, avgFrames);

	// Determine the path to save the formatted image to.  If the specified 
	// window (pathEdit) does not already contain a valid path, the
	// EnsureValidImagePath function will prompt the user to browse to a
	// location to save to.
	if (!EnsureValidImagePath(pathEdit, formatCombo))
		return false; // User clicked "Cancel" - don't save the file.

	CString path = WindowTextAsString(pathEdit);

	// Make sure the Image Format combo is updated - the user may have selected
	// a different file format from the File Save dialog.
	SetImageFormatFromFilename(pathEdit, formatCombo);

	// Determine which Image File Format to use.
	U32 fmt = -1;
	if (formatCombo.GetCurSel() >= 0)
		fmt = ItemData<U32>(formatCombo);

	// If the image needs to be formatted, do so now.
	U32 destSize = 0;
	std::vector<byte> dest;
	void* pOutputData = NULL;
	if (fmt != IMAGE_FORMAT_RAW)
	{
		// Find out how much space we need to allocate to hold the formatted image.
        // Bugzilla.2207
        //     PNG captures can take a long time.  As an performance enhancemnts, don't call PxLFormatImage
        //     twice, as doing so will run the compression algorithm twice.  Rather we will simply allocate
        //     a buffer that is sure to be large enough.  We know that color PNGs will be larger than mono PNGs, 
        //     and color PNGs are always translated to RGB24 or RGBA (32 bits).   So, we will assume worst case 
        //     RGBA with no compression, and allocate a buffer large enough for this.
        if (fmt == IMAGE_FORMAT_PNG)
        {
            float frameSize = (pFrameDesc->Roi.fWidth / pFrameDesc->PixelAddressingValue.fHorizontal)  * 
                              (pFrameDesc->Roi.fHeight / pFrameDesc->PixelAddressingValue.fVertical);
            if (pFrameDesc->HDRInfo.uMode == FEATURE_GAIN_HDR_MODE_INTERLEAVED) frameSize *= 2;
            destSize = (U32) frameSize * 4; // 4 bytes per RGBA pixel
        } else {
		    PXL_ERROR_CHECK(
			    PxLFormatImage(&imageData[0], 
						    pFrameDesc.get(), 
						    fmt, 
						    NULL, 
						    &destSize)
	        );
		    ASSERT(destSize > 0); // API sanity check.
        }

        //
        // If the stream is stopped, the user has cancelled the operation.  Throw an error so that no
        // further processing occurs (Bugzilla.1286).
        if (m_pActiveCamera->GetStreamState() != initialStreamState) THROW_PXL_EXCEPTION(ApiStreamStopped);

		// Allocate memory and format the image.
		dest.resize(destSize);
		PXL_ERROR_CHECK(
			PxLFormatImage(&imageData[0], 
						pFrameDesc.get(),
						fmt, 
						&dest[0], 
						&destSize)
		);

		// dest now contains the formatted data.
		pOutputData = &dest[0];
	}
	else
	{
		// Just save the raw data as is, with no conversion.
		destSize = (U32)imageData.size();
		pOutputData = &imageData[0];
	}

    //
    // If the stream is stopped, the user has cancelled the operation.  Throw an error so that no
    // further processing occurs. (Bugzilla.1286)
    if (m_pActiveCamera->GetStreamState() != initialStreamState) THROW_PXL_EXCEPTION(ApiStreamStopped);

	// Save the formatted image.
	CFile f;
	CFileException fx;
	TCHAR msg[200];
	if (f.Open(path, CFile::modeWrite | CFile::modeCreate, &fx))
	{
		f.Write(pOutputData, destSize);
		return true;
	}
	else
	{
		if (fx.GetErrorMessage(msg, 200))
			AfxMessageBox(msg, MB_OK);
		else
			AfxMessageBox(_T("Unknown Error. Failed to save image file."), MB_OK);
		return false;
	}
}

// Event used to notify the application that a clip capture has been completed.
static HANDLE		s_hClipCaptureDoneEvent = NULL;
static U32			s_lastClipCaptureNumFrames = 0;
static U32			s_lastClipCaptureRetCode = 0;
static ERROR_REPORT s_lastClipCaptureErrorReport;

/**
* Function: ClipTermCallback
* Purpose:  This function gets called by the API when a clip capture finishes.
*/
U32 __stdcall ClipTermCallback(HANDLE hCamera, U32 uNumFramesCaptured, PXL_RETURN_CODE uRetCode)
{
	s_lastClipCaptureNumFrames = uNumFramesCaptured;
	s_lastClipCaptureRetCode = uRetCode;
	SetEvent(s_hClipCaptureDoneEvent);
	return ApiSuccess;
}


float GetFrameRate(std::string const& pdsPath)
{
	// return (NFrames-1) / (lastFrameTime-FirstFrameTime) from a PDS file.
	//return 5.0f; // for testing
	return 50.0f; // the status-quo, for now (until I have time to finish writing this function)
}

/**
* Function: CaptureClip
* Purpose:  
*/
bool CPxLCameraDoc::CaptureClip(CWnd& numFramesEdit,
								CWnd* pathEdit,
								CComboBox* formatCombo,
								CWnd* rawPathEdit /*=NULL*/)
{
	REQUIRES_CAMERA

	if ((pathEdit == NULL || formatCombo == NULL) && rawPathEdit == NULL)
	{
		ErrorMessage(_T("Error - neither Raw Data file nor Formatted Clip file specified"));
		return false;
	}

	// Find out how many frames to capture.
	int numFrames = WindowTextToInt(numFramesEdit);
	if (numFrames < 1)
	{
		ErrorMessage(_T("You must specify the number of frames to capture"));
		numFramesEdit.SetFocus();
		return false;
	}

	// Determine the path to save the raw video data (.pds/h264) file to.
	// If rawPathEdit is not NULL, we get the path from it. If it is NULL, we
	// save the file to the system TEMP directory.
	std::string rawPath;
	bool savingRaw = false;
	if (rawPathEdit == NULL)
	{
		// Save raw video stream to a Temp directory.
		TCHAR tempRawPath[MAX_PATH];
		::GetTempPath(MAX_PATH, tempRawPath);
		::PathAppend(tempRawPath, PDS_TEMP_NAME);
		rawPath = TStringToStdString(tempRawPath);
	}
	else
	{
		EnsureValidPdsPath(*rawPathEdit);
		CString path = WindowTextAsString(*rawPathEdit);
		rawPath = TStringToStdString(path);
		savingRaw = true;
	}

	// Create the event used to signal termination, if this is the first call
	// to CaptureClip.
	if (NULL == s_hClipCaptureDoneEvent)
		s_hClipCaptureDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Reset all the global stuff, just to be safe.
	ResetEvent(s_hClipCaptureDoneEvent);
	s_lastClipCaptureNumFrames = 0;
	s_lastClipCaptureRetCode = 0;

	CClipCaptureDialog dlg(this, s_hClipCaptureDoneEvent);
	dlg.SetNumFrames(numFrames);

	DWORD dlgReturnCode;
	{
		// Scope for CStreamState object
		CStreamState tempStream(m_pActiveCamera, START_STREAM);

		
        if (GetActiveCamera()->FeatureSupported(FEATURE_ACTUAL_FRAME_RATE))
        {
             dlg.SetFrameRate(m_pActiveCamera->GetFeatureValue(FEATURE_ACTUAL_FRAME_RATE));
        } else {
             dlg.SetFrameRate(m_pActiveCamera->GetFeatureValue(FEATURE_FRAME_RATE));
        }

		::memset(&s_lastClipCaptureErrorReport, 0, sizeof(s_lastClipCaptureErrorReport));
		m_pActiveCamera->GetClip(numFrames, &rawPath[0], ClipTermCallback);

		dlgReturnCode = dlg.DoModal();
	}
	if (dlgReturnCode == IDCANCEL && m_pActiveCamera->GetStreamState() != STOP_STREAM)
	{
		// User clicked "Cancel" to terminate the clip capture.
		// Briefly stop the stream, because this is how we force the clip
		// capture to terminate early.
		CStreamState ss(m_pActiveCamera, STOP_STREAM, true); // *force* stream to stop.
	}

	CWaitCursor wc;

	if (WAIT_TIMEOUT == ::WaitForSingleObject(s_hClipCaptureDoneEvent, 5000))
	{
		ErrorMessage(_T("Termination function not called."));
	}

	if (s_lastClipCaptureRetCode == ApiStreamStopped)
	{
		// Normally, this return code means that the application requested that
		// the stream be stopped before the requested number of frames was 
		// captured. This happens if the user clicks "Cancel" in the progress
		// dialog. However, it's possible that the stream could stop for some
		// other reason, so the dialog tracks whether the user explicitly
		// cancelled the clip capture, so we know whether to display an error
		// message.
		if (!dlg.CancelledByUser())
			ErrorMessage(_T("The stream was stopped before the clip capture could be completed."));
		return false;
	}
	else if (s_lastClipCaptureRetCode == ApiDiskFullError)
	{
		ErrorMessage(_T("Could not write the data to disk."));
		return false;
	}
	else
	{
		// At this point, the return code *should* always be ApiSuccess.
		PXL_ERROR_CHECK(s_lastClipCaptureRetCode);
	}

	// The raw clip has now been saved. Determine whether to save the
	// formatted clip, and if so, then the path to save it to.
	if (pathEdit != NULL && formatCombo != NULL 
		&& EnsureValidClipPath(*pathEdit, *formatCombo))
	{
		//U32 fmt = static_cast<U32>(formatCombo->GetItemData(formatCombo->GetCurSel()));
		U32 fmt = ItemData<U32>(*formatCombo);
		if (fmt >= 0x80)
			fmt -= 0x80; // 0x80 is added so we can tell it apart from an Image format when in a combined format-selection combo box.
		std::string clipPath = TStringToStdString(WindowTextAsString(*pathEdit));

		PXL_ERROR_CHECK(PxLFormatClipEx(&rawPath[0], &clipPath[0], CLIP_ENCODING_PDS, fmt));
	}

	return true;

} // CaptureClip

/**
* Function: CaptureClip
* Purpose:  
*/
bool CPxLCameraDoc::CaptureEncodedClip(CWnd& numFramesEdit,
                                    int   clipDecimationFactor,
							   	    CWnd* pathEdit,
								    CComboBox* formatCombo,
                                    float pbFrameRate,
                                    int   pbBitRate,
                                    U32   encoding,
								    CWnd* rawPathEdit /*=NULL*/)
{
	REQUIRES_CAMERA

	if ((pathEdit == NULL || formatCombo == NULL) && rawPathEdit == NULL)
	{
		ErrorMessage(_T("Error - neither Raw Data file nor Formatted Clip file specified"));
		return false;
	}

	// Find out how many frames to capture.
	int numFrames = WindowTextToInt(numFramesEdit);
	if (numFrames < 1)
	{
		ErrorMessage(_T("You must specify the number of frames to capture"));
		numFramesEdit.SetFocus();
		return false;
	}

	// Determine the path to save the raw video data (.pds/h264) file to.
	// If rawPathEdit is not NULL, we get the path from it. If it is NULL, we
	// save the file to the system TEMP directory.
	std::string rawPath;
	bool savingRaw = false;
	if (rawPathEdit == NULL)
	{
		// Save raw video stream to a Temp directory.
		TCHAR tempRawPath[MAX_PATH];
		::GetTempPath(MAX_PATH, tempRawPath);
		::PathAppend(tempRawPath, H264_TEMP_NAME);
		rawPath = TStringToStdString(tempRawPath);
	}
	else
	{
		EnsureValidPdsPath(*rawPathEdit);
		CString path = WindowTextAsString(*rawPathEdit);
		rawPath = TStringToStdString(path);
		savingRaw = true;
	}

	// Create the event used to signal termination, if this is the first call
	// to CaptureClip.
	if (NULL == s_hClipCaptureDoneEvent)
		s_hClipCaptureDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Reset all the global stuff, just to be safe.
	ResetEvent(s_hClipCaptureDoneEvent);
	s_lastClipCaptureNumFrames = 0;
	s_lastClipCaptureRetCode = 0;

	CClipCaptureDialog dlg(this, s_hClipCaptureDoneEvent);
	dlg.SetNumFrames(numFrames);

	DWORD dlgReturnCode;
	{
		// Scope for CStreamState object
		CStreamState tempStream(m_pActiveCamera, START_STREAM);

        float actualFrameRate;
        if (GetActiveCamera()->FeatureSupported(FEATURE_ACTUAL_FRAME_RATE))
        {
             actualFrameRate = m_pActiveCamera->GetFeatureValue(FEATURE_ACTUAL_FRAME_RATE);
        } else {
             actualFrameRate = m_pActiveCamera->GetFeatureValue(FEATURE_FRAME_RATE);
        }
        // we need to 'scale' the frame rate to accomodate clip decimation
        dlg.SetFrameRate(actualFrameRate/clipDecimationFactor);

		::memset(&s_lastClipCaptureErrorReport, 0, sizeof(s_lastClipCaptureErrorReport));
		m_pActiveCamera->GetEncodedClip(numFrames, clipDecimationFactor, &rawPath[0], pbFrameRate, pbBitRate, encoding, ClipTermCallback);

		dlgReturnCode = dlg.DoModal();
	}
	if (dlgReturnCode == IDCANCEL && m_pActiveCamera->GetStreamState() != STOP_STREAM)
	{
		// User clicked "Cancel" to terminate the clip capture.
		// Briefly stop the stream, because this is how we force the clip
		// capture to terminate early.
		CStreamState ss(m_pActiveCamera, STOP_STREAM, true); // *force* stream to stop.
	}

	CWaitCursor wc;

	if (WAIT_TIMEOUT == ::WaitForSingleObject(s_hClipCaptureDoneEvent, 5000))
	{
		ErrorMessage(_T("Termination function not called."));
	}

	if (s_lastClipCaptureRetCode == ApiStreamStopped)
	{
		// Normally, this return code means that the application requested that
		// the stream be stopped before the requested number of frames was 
		// captured. This happens if the user clicks "Cancel" in the progress
		// dialog. However, it's possible that the stream could stop for some
		// other reason, so the dialog tracks whether the user explicitly
		// cancelled the clip capture, so we know whether to display an error
		// message.
		if (!dlg.CancelledByUser())
			ErrorMessage(_T("The stream was stopped before the clip capture could be completed."));
		return false;
	}
	else if (s_lastClipCaptureRetCode == ApiDiskFullError)
	{
		ErrorMessage(_T("Could not write the data to disk."));
		return false;
	}
	else if (s_lastClipCaptureRetCode == ApiH264InsufficientDataError)
	{
		ErrorMessage(_T("There were not enough frames in the capture, to perfrom encoding."));
		return false;
	}
	else
	{
		// At this point, the return code *should* always be ApiSuccess.
		PXL_ERROR_CHECK(s_lastClipCaptureRetCode);
	}

	// The raw clip has now been saved. Determine whether to save the
	// formatted clip, and if so, then the path to save it to.
	if (pathEdit != NULL && formatCombo != NULL 
		&& EnsureValidClipPath(*pathEdit, *formatCombo))
	{
		//U32 fmt = static_cast<U32>(formatCombo->GetItemData(formatCombo->GetCurSel()));
		U32 fmt = ItemData<U32>(*formatCombo);
		if (fmt >= 0x80)
			fmt -= 0x80; // 0x80 is added so we can tell it apart from an Image format when in a combined format-selection combo box.
		std::string clipPath = TStringToStdString(WindowTextAsString(*pathEdit));

		PXL_ERROR_CHECK(PxLFormatClipEx(&rawPath[0], &clipPath[0], CLIP_ENCODING_H264, fmt));
	}

    // Finally, determine if we should warn the user that some image loss occurred while capturing the clip.
    if (s_lastClipCaptureRetCode == ApiSuccessWithFrameLoss)
    {
        float percentLoss = (((float)(s_lastClipCaptureNumFrames - numFrames)) / (float)numFrames) * 100.0f;
   		CString msg;
        if (percentLoss < 0.01)
		     msg.Format("Clip created, but required %d additional frames.\nConsider increasing clip decimation", 
                         s_lastClipCaptureNumFrames - numFrames);
        else
		     msg.Format("Clip created, but required %6.2f%% additional frames.\nConsider increasing clip decimation", 
                         percentLoss);
		AfxMessageBox(msg, MB_ICONINFORMATION);
    }


	return true;

} // CaptureH264Clip


static bool
CompareCameraIdInfo(const CAMERA_ID_INFO& rhs, const CAMERA_ID_INFO& lhs)
{
	return rhs.CameraSerialNum < lhs.CameraSerialNum;
}


/**
* Function: LoadSerialNumbers
* Purpose:  Fill the vector m_serialNumbers with the serial numbers of all
*           PixeLINK cameras that are currently connected to the computer.
*/
void CPxLCameraDoc::LoadSerialNumbers(void)
{
	// If the application is starting up and we've read the list once, don't read it again
	CPxLDevApp* pApp = (CPxLDevApp*)AfxGetApp();
	if (pApp->IsStartingUp()) {
		static bool loaded = false;
		if (loaded) {
			return;
		}
		loaded = true;
	}

	m_serialNumbers.clear();
	m_cameraIdInfo.clear();

	try
	{
		// Ask how many cameras there are
		U32 uMaxNumberOfCameras;
		PXL_ERROR_CHECK(PxLGetNumberCamerasEx(NULL, &uMaxNumberOfCameras));
		if (uMaxNumberOfCameras > 0) {
			// But now we have to figure out how many of them we can connect to.
			// i.e. how many have serial numbers?
			std::vector<CAMERA_ID_INFO> tempCameraIdInfo(uMaxNumberOfCameras);
			memset(&tempCameraIdInfo[0], 0, sizeof(tempCameraIdInfo[0]) * tempCameraIdInfo.size());
			tempCameraIdInfo[0].StructSize = sizeof(CAMERA_ID_INFO);
			PXL_ERROR_CHECK(PxLGetNumberCamerasEx(&tempCameraIdInfo[0], &uMaxNumberOfCameras));

			for(int i=0; i < uMaxNumberOfCameras; i++) {
				U32 serialNumber = tempCameraIdInfo[i].CameraSerialNum;
				if (serialNumber != 0) {
					m_serialNumbers.push_back(serialNumber);
					m_cameraIdInfo.push_back(tempCameraIdInfo[i]);
				}
			}
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
		m_serialNumbers.clear();
	}

	// We store the cameras sorted by serial number.
	std::sort(m_serialNumbers.begin(), m_serialNumbers.end());

	// Remove any entries from the m_cameras collection that refer to cameras
	// that are no longer connected.
	std::vector<U32> missing;
	for (CameraMap_t::iterator mapiter = m_cameras.begin(); mapiter != m_cameras.end(); mapiter++)
	{
		U32 sn = mapiter->second->GetSerialNumber();
		if (!contains(m_serialNumbers, sn))
			missing.push_back(sn);
	}
	for (std::vector<U32>::iterator veciter = missing.begin(); veciter != missing.end(); veciter++)
	{
		if (m_cameras[*veciter] != m_pActiveCamera)
		{
            m_cameras[*veciter]->SetEventCallback (EVENT_CAMERA_DISCONNECTED, NULL, NULL); // Cancel the callback
			m_cameras[*veciter]->Uninitialize(); // Uninitialize never throws.
			delete m_cameras[*veciter];
			m_cameras.erase(*veciter);
		}
	}

} // LoadSerialNumbers


/******************************************************************************
* Message Handlers
******************************************************************************/

/**
* Function: OnFileImportConfigFile
* Purpose:  
*/
void CPxLCameraDoc::OnFileImportConfigFile()
{
	if (!HasActiveCamera())
		return;

	CWaitCursor wc;

	// Force the stream to stop while we load the new settings.
	{
		CStreamState temp(m_pActiveCamera, STOP_STREAM, true);

		LPCTSTR filterString = _T("PixeLINK Camera Configuration (*.pcc)|*.pcc|All Files (*.*)|*.*||");
		CFileDialog fd(
						TRUE,				// FALSE for "Save As" dialog, TRUE for "Open" dialog
						_T("pcc"),			// lpsz Default extension
						_T("*.pcc"),		// lpsz Default filename
						0,
						filterString,		// lpsz Filters
						NULL);				// CWnd* parent

		if (fd.DoModal() != IDOK)
			return;

		CWaitCursor wc2; // Not sure why, but we seem to need this...
		try
		{
			LoadFromConfigFile(m_pActiveCamera, fd.GetPathName());
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
	// Stream state gets reset here.

	// Invalidate the lookupgraphcache
	// so that the graph gets redrawn properly
	m_pActiveCamera->InvalidateLookupGraphCache();

	// Tell the View to update everything:
	switch (m_pActiveCamera->GetPreviewState())
	{
	case STOP_PREVIEW:
		DoViewChange(CT_NewCamera);
		break;
	case START_PREVIEW:
		DoViewChange(CT_NewCamera | CT_PreviewStarted);
		break;
	case PAUSE_PREVIEW:
		DoViewChange(CT_NewCamera | CT_PreviewPaused);
		break;
	}
}


/**
* Function: OnUpdateFileImportConfigFile
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateFileImportConfigFile(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveCamera != NO_CAMERA);
}


/**
* Function: OnFileExportConfigFile
* Purpose:  
*/
void CPxLCameraDoc::OnFileExportConfigFile()
{
	if (!HasActiveCamera())
		return;

	CWaitCursor wc;

	LPCTSTR filterString = _T("PixeLINK Camera Configuration (*.pcc)|*.pcc|All Files (*.*)|*.*||");
	CFileDialog fd(
					FALSE,				// FALSE for "Save As" dialog, TRUE for "Open" dialog
					_T("pcc"),			// lpsz Default extension
					_T("*.pcc"),		// lpsz Default filename
					OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
					filterString,		// lpsz Filters
					NULL);				// CWnd* parent

	if (fd.DoModal() != IDOK)
		return;

	CWaitCursor wc2; // Not sure why, but we seem to need this...
	try
	{
		WriteConfigFile(m_pActiveCamera, fd.GetPathName());
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnUpdateFileExportConfigFile
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateFileExportConfigFile(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveCamera != NO_CAMERA);
}


/**
* Function: OnViewResetPreviewWindow
* Purpose:  
*/
void CPxLCameraDoc::OnViewResetPreviewWindow()
{
	m_pActiveCamera->ResetPreviewWindow();
}


/**
* Function: OnFileSetIPAddress
* Purpose:  Launch the PxLIpAddresses tool if required.
*/
void
CPxLCameraDoc::OnFileSetIPAddress()
{
	if (IDYES == AfxMessageBox(_T("Capture OEM cannot run while IP addresses are being configured and must therefore exit. Do you wish to continue?"), MB_YESNO | MB_ICONQUESTION))
	{
		std::vector<CHAR> pixelinkRootDir(MAX_PATH, 0);
		if (0 != GetEnvironmentVariable(_T("PIXELINK_SDK_ROOT"), &pixelinkRootDir[0], (DWORD)pixelinkRootDir.size())) 
		{
			CString rootDir(&pixelinkRootDir[0]);
			// The command doesn't need quotes around it...
			CString cmd(rootDir + _T("\\Tools\\PxLIpAddresses.exe"));
			// but the first param does, in case there are spaces in the path
			CString param = _T("\"") + cmd + _T("\"");
			const char* argv[] = { param, NULL};
			intptr_t rc = _spawnv(_P_NOWAIT, cmd, argv);
			if (-1 != rc) 
			{
				PostQuitMessage(0);
				return;
			}
			else 
			{
				OutputDebugString("ERROR: Unable to spawn the IP address tool\n");
			}
		} 
		else 
		{
			OutputDebugString("ERROR: PIXELINK_SDK_ROOT environment variable is undefined\n");
		}

		AfxMessageBox(_T("Unable to launch the IP address tool"), MB_ICONEXCLAMATION);
	}
}


/**
* Function: OnUpdateFileSetIPAddress
* Purpose:  
*/
void
CPxLCameraDoc::OnUpdateFileSetIPAddress(CCmdUI *pCmdUI)
{
}



/**
* Function: OnViewAutoResetPreview
* Purpose:  
*/
void CPxLCameraDoc::OnViewAutoResetPreview()
{
	theApp.m_bAutoResetPreview = !theApp.m_bAutoResetPreview;
	if (theApp.m_bAutoResetPreview)
		m_pActiveCamera->ResetPreviewWindow();
}


/**
* Function: OnCameraLoadCameraSettings
* Purpose:  
*/
void CPxLCameraDoc::OnCameraLoadCameraSettings()
{
	if (m_pActiveCamera == NO_CAMERA) {
		return; // Menu item *should* be disabled...
	}

	CLoadSettingsDialog dlg(m_pActiveCamera);

	if (dlg.DoModal() == IDOK)
	{
		try
		{
			CWaitCursor wc;

			int oldPreviewState = m_pActiveCamera->GetPreviewState();

			m_pActiveCamera->LoadCameraSettings(dlg.m_channel);

			int currPreviewState = m_pActiveCamera->GetPreviewState();

			// Tell the View to update everything:
			switch (m_pActiveCamera->GetPreviewState())
			{
			case STOP_PREVIEW:
				DoViewChange(CT_NewCamera);
				break;
			case START_PREVIEW:
				m_pActiveCamera->ResetPreviewWindow();
				DoViewChange(CT_NewCamera | CT_PreviewStarted);
				break;
			case PAUSE_PREVIEW:
				m_pActiveCamera->ResetPreviewWindow();
				DoViewChange(CT_NewCamera | CT_PreviewPaused);
				break;
			}

		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnCameraSaveCameraSettings
* Purpose:  
*/
void CPxLCameraDoc::OnCameraSaveCameraSettings()
{
	if (m_pActiveCamera == NO_CAMERA)
		return; // Menu item *should* be disabled...

	CSaveSettingsDialog dlg(m_pActiveCamera);

	if (dlg.DoModal() == IDOK)
	{
		try
		{
			CWaitCursor wc;
			m_pActiveCamera->SaveCameraSettings(dlg.m_channel);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnCameraSetCameraName
* Purpose:  
*/
void CPxLCameraDoc::OnCameraSetCameraName()
{
	CSetCameraNameDialog dlg(this);

	if (dlg.DoModal() == IDOK)
	{
		CWaitCursor wait;
		char name[33] = {0};
		for (int i = 0; i < min(32, dlg.m_newName.GetLength()); i++)
		{
			name[i] = static_cast<char>(dlg.m_newName.GetAt(i));
		}
		

		try
		{
			m_pActiveCamera->SetCameraName(&name[0]);
        	// Bugzilla.426/458 -- stimulate a camera change event so that the camera infomation  cached in controls, are rebuilt
        	int currentState = m_pActiveCamera->GetStreamState();
	        DoViewChange(CT_NewCamera | (currentState == START_STREAM ? CT_PreviewStarted : 
	                                                                    (currentState == PAUSE_STREAM ? CT_PreviewPaused : 0)));
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnCameraReinitialize
* Purpose:  
*/
void CPxLCameraDoc::OnCameraReinitialize()
{
	REQUIRES_CAMERA

	try
	{
		DoViewChange(CT_NoCamera);
		m_pActiveCamera->Reinitialize();
		DoViewChange(CT_NewCamera);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: SetLaptopMode
* Purpose:  
*/
void CPxLCameraDoc::SetLaptopMode(int framerateDivisor)
{

	theApp.m_framerateDivisor = framerateDivisor;
	if (this->HasActiveCamera())
	{        
        if (GetActiveCamera()->SetBandwidth())
    	{
			DoViewChange(CT_FeatureChanged + FEATURE_FRAME_RATE);
		}
        else
        {
            // Update the view regardless - the limits may have changed even if the 
            // actual frame rate has not.
            DoViewChange(CT_FeatureChanged + FEATURE_FRAME_RATE);
        }
	}
}


/**
* Function: OnViewHistogram
* Purpose:  
*/
void CPxLCameraDoc::OnViewHistogram()
{
	REQUIRES_CAMERA

	if (m_pActiveCamera->m_pHistogramWindow == NULL)
	{
		// Open a Histogram dialog
		CHistogramDialog2* dlg = new CHistogramDialog2(m_pActiveCamera);
		dlg->Create(CHistogramDialog2::IDD);

		// Move the histogram window so that it's not on top of the main window
		CRect rcMain, rcDlg;
		theApp.GetMainWnd()->GetWindowRect(&rcMain);
		dlg->GetWindowRect(&rcDlg);
		int desktopWidth = ::GetSystemMetrics(SM_CXMAXIMIZED);
		if (rcMain.right + rcDlg.Width() > desktopWidth)
		{
			// Not enough room to put it to the right of the main window.
			// Put it on the left instead.
			dlg->SetWindowPos(NULL, rcMain.left - rcDlg.Width(), rcMain.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		else
		{
			// Put it adjacent to the top-right corner of the main window.
			dlg->SetWindowPos(NULL, rcMain.right, rcMain.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		dlg->ShowWindow(SW_SHOW);

		// Give the main app window the input focus - if we don't do this,
		// then the Histogram dialog has the focus, and it is neccesary to use
		// the mouse to set focus back to the main window before you can use
		// the keyboard for any kind of input to the application.
		theApp.GetMainWnd()->SetActiveWindow();
	}
	else
	{
		// Close the camera's Histogram dialog
		m_pActiveCamera->CloseHistogram();
	}

	// The CHistogramDialog object is deleted in CHistogramDialog::OnCancel
}

/**
* Function: OnSlowImagerMode
* Purpose:  
*/
void CPxLCameraDoc::OnSlowImagerMode()
{
	REQUIRES_CAMERA
	
    try
	{
        // NOTE -- don't bother to check to see of the feature is supported, or with the limits.  These are
        //          checked when the menu is displayed -- and this item is only selectable if the camera supports it.
        if (m_pActiveCamera->GetFeatureValue(FEATURE_SPECIAL_CAMERA_MODE) == FEATURE_SPECIAL_CAMERA_MODE_NONE) {
            m_pActiveCamera->SetFeatureValue(FEATURE_SPECIAL_CAMERA_MODE, FEATURE_SPECIAL_CAMERA_MODE_FIXED_FRAME_RATE);
        } else {
            m_pActiveCamera->SetFeatureValue(FEATURE_SPECIAL_CAMERA_MODE, FEATURE_SPECIAL_CAMERA_MODE_NONE);
        }
    }
    catch (PxLException const& e)
	{
		OnApiError(e);
	}

}

/**
* Function: OnFixedFramerateMode
* Purpose:  
*/
void CPxLCameraDoc::OnFixedFramerateMode()
{
	REQUIRES_CAMERA
	
    try
	{
        // NOTE -- don't bother to check to see of the feature is supported, or with the limits.  These are
        //          checked when the menu is displayed -- and this item is only selectable if the camera supports it.
        if (m_pActiveCamera->GetFeatureValue(FEATURE_SPECIAL_CAMERA_MODE) == FEATURE_SPECIAL_CAMERA_MODE_NONE) {
            m_pActiveCamera->SetFeatureValue(FEATURE_SPECIAL_CAMERA_MODE, FEATURE_SPECIAL_CAMERA_MODE_FIXED_FRAME_RATE);
        } else {
            m_pActiveCamera->SetFeatureValue(FEATURE_SPECIAL_CAMERA_MODE, FEATURE_SPECIAL_CAMERA_MODE_NONE);
        }
        // Bugzilla.2293
        //  Turning FixedFramerateMode off or on may change the set of features supportted,so re-read the
        //  features.
        m_pActiveCamera->LoadCameraFeatures();
        // The limits may have changed -- so update the features which may have changed.
        DoViewChange(CT_FeatureChanged + FEATURE_SPECIAL_CAMERA_MODE);
        DoViewChange(CT_FeatureChanged + FEATURE_FRAME_RATE);
        DoViewChange(CT_FeatureChanged + FEATURE_SHUTTER);
    }
    catch (PxLException const& e)
	{
		OnApiError(e);
	}

}

/**
* Function: OnUpdateViewResetPreviewWindow
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateViewResetPreviewWindow(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveCamera->GetPreviewState() != STOP_PREVIEW);
}


/**
* Function: OnUpdateViewAutoResetPreview
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateViewAutoResetPreview(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_bAutoResetPreview);
}


/**
* Function: OnUpdateViewHistogram
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateViewHistogram(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveCamera != NO_CAMERA);
	pCmdUI->SetCheck(m_pActiveCamera->m_pHistogramWindow != NULL);
}


/**
* Function: OnUpdateCameraLoadCameraSettings
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateCameraLoadCameraSettings(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveCamera != NO_CAMERA);
}


/**
* Function: OnUpdateCameraSaveCameraSettings
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateCameraSaveCameraSettings(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveCamera != NO_CAMERA);
}


/**
* Function: OnUpdateCameraSetCameraName
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateCameraSetCameraName(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveCamera != NO_CAMERA);
}


/**
* Function: OnUpdateCameraReinitialize
* Purpose:  
*/
void CPxLCameraDoc::OnUpdateCameraReinitialize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveCamera != NO_CAMERA);
}


/**
* Function: OnAppAbout
* Purpose:  
*/
void CPxLCameraDoc::OnAppAbout()
{
	CAboutDlg aboutDlg(this);
	aboutDlg.DoModal();
}


void CPxLCameraDoc::OnLaptopfriendlymodeOff()
{
	this->SetLaptopMode(1);
}

void CPxLCameraDoc::OnLaptopfriendlymode2()
{
	this->SetLaptopMode(2);
}

void CPxLCameraDoc::OnLaptopfriendlymode3()
{
	this->SetLaptopMode(3);
}

void CPxLCameraDoc::OnLaptopfriendlymode4()
{
	this->SetLaptopMode(4);
}

void CPxLCameraDoc::OnLaptopfriendlymode8()
{
	this->SetLaptopMode(8);
}

void CPxLCameraDoc::OnUpdateLaptopfriendlymodeOff(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_framerateDivisor == 1);
}

void CPxLCameraDoc::OnUpdateLaptopfriendlymode2(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_framerateDivisor == 2);
}

void CPxLCameraDoc::OnUpdateLaptopfriendlymode3(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_framerateDivisor == 3);
}

void CPxLCameraDoc::OnUpdateLaptopfriendlymode4(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_framerateDivisor == 4);
}

void CPxLCameraDoc::OnUpdateLaptopfriendlymode8(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_framerateDivisor == 8);
}

void CPxLCameraDoc::OnUpdateSlowImagerMode(CCmdUI *pCmdUI)
{
    std::pair<float,float>  ffLimits;
    bool                    bSlowModeSupported = FALSE;
    bool                    bSlowModeEnabled = FALSE;

    if (m_pActiveCamera != NO_CAMERA) {
		try
		{
            if (m_pActiveCamera->FeatureSupported(FEATURE_IMAGER_CLK_DIVISOR)) {
                ffLimits = m_pActiveCamera->GetFeatureLimits(FEATURE_IMAGER_CLK_DIVISOR);
                if ((ffLimits.first <= 2) && (ffLimits.second >= 2)) {
                    bSlowModeSupported = TRUE;
                    bSlowModeEnabled = (m_pActiveCamera->GetFeatureValue(FEATURE_IMAGER_CLK_DIVISOR) != 1);
                }
            }
        }
    	catch (PxLException const& e)
	    {
		    OnApiError(e);
	    }
    }

    pCmdUI->Enable(bSlowModeSupported);
    pCmdUI->SetCheck(bSlowModeEnabled);
}

void CPxLCameraDoc::OnUpdateFixedFramerateMode(CCmdUI *pCmdUI)
{
    bool                    bFixedFramrateModeSupported = FALSE;
    bool                    bFixedFramerateModeEnabled = FALSE;

    if (m_pActiveCamera != NO_CAMERA) {
		try
		{
            bFixedFramrateModeSupported = m_pActiveCamera->IsFixedFramerateModeSupported();
            bFixedFramerateModeEnabled = m_pActiveCamera->IsFixedFramerateModeEnabled();
        }
    	catch (PxLException const& e)
	    {
		    OnApiError(e);
	    }
    }

    pCmdUI->Enable(bFixedFramrateModeSupported);
    pCmdUI->SetCheck(bFixedFramerateModeEnabled);
}

/******************************************************************************
* Stuff for saving and loading config files
******************************************************************************/

// Struct used only to read and write features to file.
struct FeatureData
{
	U32		featureId;
	U32		flags;
	U32		nParams;
	float	param1;
};

// This msut be the first 4 bytes of a PxL Camera Config file.
// Note - the number doesn't mean anything - I just banged on the keyboard.
#define PXL_CONFIG_FILE_MAGIC_NUMBER 0x41513879

/**
* Function: WriteConfigFile
* Purpose:  

	File Format of config files:
	int Magic Number
	int #descriptors
	[feature data starts here - either for desc #1, or just the normal settings if #descriptors == 0]
	[int desc #1 update mode, if #descriptors > 0]
	int #features (=N == number of supported features)
		feature #1 : consists of a FeatureData struct, plus [nParams-1] extra floats immediately following the struct.
		feature #2 : ditto
		etc
		feature #N
	[descriptor#2 starts here]
	int desc #2 update mode
	int #features (=M == number of desc-supported features
		feature #1 : as above
		etc
		feature #M
	[descriptor #3 starts here]
		etc
	[tab 1 starts here] 
		// Note that it's optional for a tab to save its data; most don't
		// 'tab 1' doesn't mean the first tab in Capture OEM, but rather the first tab to actually save data
		int tab IDD
		# bytes of data
		[tab 1-specific data starts here]
		tab-specific data
	[tab 2 starts here]
		etc
	etc
	[tab N starts here]
	END OF FILE
		
*/
bool CPxLCameraDoc::WriteConfigFile(CPxLCamera* cam, LPCTSTR fileName)
{

	// Determine how much memory we need to allocate for the data.
	// Find out how many features are supported and how many parameters
	// they have.
	int nSupported = 0;
	int nExtraParams = 0;
	int nDescSupported = 0;
	int nExtraDescParams = 0;
	int featureId;
	for (featureId = 0; featureId < FEATURES_TOTAL; ++featureId)
	{
		if (!cam->FeatureSupported(featureId))
			continue;

		int num = 1;
		if (featureId == FEATURE_GPIO)
		{
			std::pair<float,float> minmax = cam->GetFeatureLimits(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX);
			num = (int)((minmax.second - minmax.first + 1));
		}
		nSupported += num;

		U32 pcount = cam->GetFeatureParamCount(featureId);
		nExtraParams += num * (pcount - 1); // First param is inside the FeatureDesc struct - we just need to count *extra* params.

		if (cam->FeatureSupportsDescriptor(featureId))
		{
			nDescSupported += num;
			nExtraDescParams += num * (pcount - 1);
		}
	}

	int bufsize = 3 * sizeof(int) // Magic number, #descriptors, and feature count
					+ nSupported * sizeof(FeatureData) 
					+ nExtraParams * sizeof(float);
	int nDesc = cam->GetDescriptorCount();
	bufsize += sizeof(int) * nDesc; // 4-byte update mode per descriptor
	if (nDesc >= 2)
	{
		bufsize += (nDesc - 1) // # of additional descriptors
					* ( sizeof(int) // feature count
						+ nDescSupported * sizeof(FeatureData)
						+ nExtraDescParams * sizeof(float) );
	}

	std::vector<byte> buffer(bufsize); // buffer should now be exactly the right size.
	// allow room for growth without having to realloc
	buffer.reserve(10*1024);			// NOTE: this also gets rid of a bug in the debug std::vector dtor

	// Set the magic number and the descriptor count
	int* ip = reinterpret_cast<int*>(&buffer[0]);
	*ip++ = PXL_CONFIG_FILE_MAGIC_NUMBER;
	*ip++ = nDesc;

	// Put the first descriptor in focus, if there is more than one.
	if (nDesc >= 2)
	{
		PXL_ERROR_CHECK(
			PxLUpdateDescriptor(cam->GetHandle(), 
								cam->m_descriptors[0].m_descriptorHandle,
								cam->m_descriptors[0].m_updateMode) // Don't change its mode...just put it in focus
		);
		// Store the update mode of the first descriptor, if there is one.
		*ip++ = cam->m_descriptors[0].m_updateMode;
	}

	// The number of features stored for the first descriptor.
	*ip++ = nSupported;

	// Get the values and flags for all the supported features.
	byte* pbuf = reinterpret_cast<byte*>(ip);
	// 2004-06-29 - Now loops from highest to lowest featureID, to work aroung
	//              obscure bug in 682/782 (essentially, we need to set Lookup
	//              Table before Gamma when we Import config files).
	for (featureId = FEATURES_TOTAL-1; featureId >= 0; --featureId)
	{
		if (!cam->FeatureSupported(featureId))
			continue;

		U32 pcount = cam->GetFeatureParamCount(featureId);

		if (featureId == FEATURE_GPIO)
		{
			// GPIO is special - there may be more than one GPO supported, but
			// we can only read them one at a time. We need to read each of them
			// in a loop.
			for (int gpoNum = (int)cam->GetFeatureMinVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX);
					gpoNum <= (int)cam->GetFeatureMaxVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX);
					++gpoNum)
			{
				FeatureData* pfd = reinterpret_cast<FeatureData*>(pbuf);
				pfd->featureId = featureId;
				pfd->nParams = pcount;

				pfd->param1 = static_cast<float>(gpoNum);
				PXL_RETURN_CODE rc = 
					PxLGetFeature(  cam->GetHandle(), 
									featureId, 
									&pfd->flags, 
									&pfd->nParams, 
									&pfd->param1  );

				pbuf += sizeof(FeatureData) + (pcount-1) * sizeof(float);
			}
		}
		else
		{
			// All other features (other than GPIO) can be read in one go.
			FeatureData* pfd = reinterpret_cast<FeatureData*>(pbuf);
			pfd->featureId = featureId;
			pfd->nParams = pcount;

			PXL_RETURN_CODE rc = 
				PxLGetFeature(  cam->GetHandle(), 
								featureId, 
								&pfd->flags, 
								&pfd->nParams, 
								&pfd->param1  );

			pbuf += sizeof(FeatureData) + (pcount-1) * sizeof(float);
		}
	}

	// Now add in the values for any extra descriptors
	for (int descNo = 1; descNo < nDesc; ++descNo)
	{
		ip = reinterpret_cast<int*>(pbuf);

		// Put the next descriptor in focus.
		PXL_ERROR_CHECK(
			PxLUpdateDescriptor(cam->GetHandle(), 
								cam->m_descriptors[descNo].m_descriptorHandle,
								cam->m_descriptors[descNo].m_updateMode)
		);
		// Store its update mode.
		*ip++ = cam->m_descriptors[descNo].m_updateMode;

		// Write the number of features.
		*ip++ = nDescSupported;
		pbuf = reinterpret_cast<byte*>(ip);

		for (int featureId = FEATURES_TOTAL-1; featureId >= 0; --featureId)
		{
			if (!cam->FeatureSupportsDescriptor(featureId))
				continue;

			U32 pcount = cam->GetFeatureParamCount(featureId);

			if (featureId == FEATURE_GPIO)
			{
				// GPIO is special - there may be more than one GPO supported, but
				// we can only read them one at a time. We need to read each of them
				// in a loop.
				for (int gpoNum = (int)cam->GetFeatureMinVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX);
						gpoNum <= (int)cam->GetFeatureMaxVal(FEATURE_GPIO, FEATURE_GPIO_PARAM_GPIO_INDEX);
						++gpoNum)
				{
					FeatureData* pfd = reinterpret_cast<FeatureData*>(pbuf);
					pfd->featureId = featureId;
					pfd->nParams = pcount;

					pfd->param1 = static_cast<float>(gpoNum);
					PXL_RETURN_CODE rc = 
						PxLGetFeature(  cam->GetHandle(), 
										featureId, 
										&pfd->flags, 
										&pfd->nParams, 
										&pfd->param1  );

					pbuf += sizeof(FeatureData) + (pcount-1) * sizeof(float);
				}
			}
			else
			{
				// All other features (other than GPIO) can be read in one go.
				FeatureData* pfd = reinterpret_cast<FeatureData*>(pbuf);
				pfd->featureId = featureId;
				pfd->nParams = pcount;

				PXL_RETURN_CODE rc = 
					PxLGetFeature(  cam->GetHandle(), 
									featureId, 
									&pfd->flags, 
									&pfd->nParams, 
									&pfd->param1  );

				pbuf += sizeof(FeatureData) + (pcount-1) * sizeof(float);
			}
		}
	}

	// At this point, we should have filled the vector with exactly the number
	// of bytes that we pre-calculated, so the pointer should point one byte 
	// past the end of the vector.
	{
		byte const * const pEnd = &buffer[buffer.size()-1] + 1;
		ASSERT(pbuf == pEnd);
	}

	// Get any data that specific pages want to store in the config file.
	// At the moment (2003-10-21), only the Callbacks are stored.
	// Each page (tab) in the application is given the chance to append
	// page-specfic configuration data to the end of the vector. Most don't.
	POSITION pos = GetFirstViewPosition();
	CPxLDevAppView* view = static_cast<CPxLDevAppView*>(GetNextView(pos));
	if (view != NULL)
	{
		view->GetConfigFileData(buffer);
	}

	// Save the data to disk
	CFile configFile;
	CFileException fileExc;
	if (!configFile.Open(fileName, CFile::modeWrite | CFile::modeCreate, &fileExc))
	{
		// Error opening file in write mode
		fileExc.ReportError();
		return false;
	}

	configFile.Write(&buffer[0], (UINT)buffer.size());
	configFile.Close();

	return true;
}


/**
* Function: LoadFromConfigFile
* Purpose:  
*/
bool CPxLCameraDoc::LoadFromConfigFile(CPxLCamera* cam, LPCTSTR fileName)
{
	/*
	File Format of config files:
	See WriteCongigFile, above.
	*/

	// Load the data from disk
	CFile configFile;
	CFileException fileExc;
	if (!configFile.Open(fileName, CFile::modeRead, &fileExc))
	{
		// Error opening file in write mode
		fileExc.ReportError();
		return false;
	}

	UINT len = (UINT)configFile.GetLength();
	std::vector<byte> buffer(len, 0);
	if (len != configFile.Read(&buffer[0], len))
	{
		ErrorMessage(_T("Error reading config file"));
		return false;
	}
	configFile.Close();

	// The data is now in buffer
	byte* pbuf = &buffer[0];
	int* ip = reinterpret_cast<int*>(pbuf);
	if (*ip++ != PXL_CONFIG_FILE_MAGIC_NUMBER)
	{
		ErrorMessage(_T("The file is not a valid PixeLINK Camera Configuration file."));
		return false;
	}
	int nDesc = *ip++; // # of descriptors
	int updateMode = 0;
	if (nDesc > 0)
	{
		updateMode = *ip++;
	}
	int nFeatures = *ip++; // # of features
	pbuf = reinterpret_cast<byte*>(ip);

	// Create or remove first descriptor, if required.
	if (nDesc > 0)
	{
		// Create or set focus to first descriptor
		if (cam->GetDescriptorCount() == 0)
		{
			cam->CreateDescriptor(updateMode);
		}
		else
		{
			cam->SetActiveDescriptor(1); // We number our descriptors from 1, not 0.
			cam->SetDescriptorUpdateMode(updateMode);
		}
	}
	else
	{
		// No descriptors defined
		cam->DeleteAllDescriptors();
	}

	// Set the feature state and flags for each feature that was read from the file.
	std::set<U32> failedFeatures;
	for (int i = 0; i < nFeatures; i++)
	{
		FeatureData* pfd = reinterpret_cast<FeatureData*>(pbuf);

		U32 myNParams = cam->GetFeatureParamCount(pfd->featureId);
		if (cam->FeatureSupportsManual(pfd->featureId) && pfd->nParams >= myNParams)
		{
			// The stored feature has enough params to satisfy our call to
			// PxLSetFeature (and possibly more - it should work either way).
			PXL_RETURN_CODE rc = 
				PxLSetFeature( cam->GetHandle(),
								pfd->featureId,
								pfd->flags,
								myNParams,
								&pfd->param1);
			if (FAILED(rc)) {
				failedFeatures.insert(pfd->featureId);
			}
		}
		else
		{
			// Either:
			// a) The current camera does not allow us to set this feature, or
			// b) The number of parameters for this feature, as stored in the
			// config file, is less than the number of parameters required
			// by the active camera.
			// Don't try to set the feature.
			failedFeatures.insert(pfd->featureId);
		}

		pbuf += sizeof(FeatureData) + (pfd->nParams - 1) * sizeof(float);
	}

	// Now, if there are 2 or more descriptors defined in the config file, set
	// the feature values for the remaining descriptors as well.
	int descNo;
	for (descNo = 2; descNo <= nDesc; ++descNo)
	{
		ip = reinterpret_cast<int*>(pbuf);

		// Get the update mode for this descriptor.
		updateMode = *ip++;

		// Create or set focus to the next descriptor;
		if (cam->GetDescriptorCount() >= descNo)
		{
			cam->SetActiveDescriptor(descNo);
			cam->SetDescriptorUpdateMode(updateMode);
		}
		else
		{
			cam->CreateDescriptor(updateMode);
		}

		nFeatures = *ip++;
		pbuf = reinterpret_cast<byte*>(ip);

		for (int i = 0; i < nFeatures; i++)
		{
			FeatureData* pfd = reinterpret_cast<FeatureData*>(pbuf);

			U32 myNParams = cam->GetFeatureParamCount(pfd->featureId);
			if (cam->FeatureSupportsManual(pfd->featureId) 
				&& cam->FeatureSupportsDescriptor(pfd->featureId) 
				&& pfd->nParams >= myNParams)
			{
				// The stored feature has enough params to satisfy our call to
				// PxLSetFeature (and possibly more - it should work either way).
				PXL_RETURN_CODE rc = 
					PxLSetFeature(cam->GetHandle(),
									pfd->featureId,
									pfd->flags,
									myNParams,
									&pfd->param1);
				if (FAILED(rc))
					failedFeatures.insert(pfd->featureId);
			}
			else
			{
				// Either:
				// a) The current camera does not allow us to set this feature, or
				// b) The current camera does not support this feature on a per-
				//    descriptor basis, or
				// c) The number of parameters for this feature, as stored in the
				// config file, is less than the number of parameters required
				// by the active camera.
				// Don't try to set the feature.
				failedFeatures.insert(pfd->featureId);
			}

			pbuf += sizeof(FeatureData) + (pfd->nParams - 1) * sizeof(float);
		}
	}

	// Remove any remaining extra descriptors.
	while (cam->GetDescriptorCount() >= descNo)
	{
		cam->SetActiveDescriptor(descNo);
		cam->RemoveDescriptor();
	}

	// Set focus to the first descriptor, if there are any.
	if (cam->GetDescriptorCount() > 0) {
		cam->SetActiveDescriptor(1);
	}

	// If there is any data left, pass it to the View's SetFromConfigFile
	// method, which gives each page (tab) a chance to restore its state based
	// on config file data that it stored earlier.
	int bytesProcessed = (int)(pbuf - &buffer[0]);
	if (bytesProcessed < buffer.size())
	{
		POSITION pos = GetFirstViewPosition();
		CPxLDevAppView* view = static_cast<CPxLDevAppView*>(GetNextView(pos));
		if (view != NULL)
		{
			view->SetFromConfigFileData(buffer, bytesProcessed);
		}
	}

	// If there is anything in the "failedFeatures" set, produce an error
	// message indicating to the user that the config file could not be
	// applied perfectly - some features may not be set correctly.
	// TODO - ...

	return true;
}

	
/******************************************************************************
* App-Wizard generated stuff that we don't really use.
******************************************************************************/

/**
* Function: OnNewDocument
* Purpose:  
*/
BOOL CPxLCameraDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CPxLCameraDoc serialization

void CPxLCameraDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// Add storing code here
	}
	else
	{
		// Add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPxLCameraDoc Threads
// Added to deal with issues Bugzilla.323 and Bugzilla.197

void CPxLCameraDoc::StartCameraPoller(void)
{
    if (NULL != m_CameraPoller) return;
    
    ResetEvent(m_CameraPollerStopEvent);
    m_CameraPoller = CreateThread(NULL, 0, CameraPollerThread, this, 0, NULL);
    if (NULL != m_CameraPoller)
    {
        CloseHandle(m_CameraPoller);  // OK to close this right away.
    }

}

void CPxLCameraDoc::StopCameraPoller(void)
{
    if (NULL == m_CameraPoller) return;
    
    SetEvent(m_CameraPollerStopEvent);
}

// 
// This poller is intended to run anytime C-OEM does not have an active camera.  It will self terminate
// once it finds a camera (and kicks the 'main' application to grab it).
ULONG __stdcall CPxLCameraDoc::CameraPollerThread(LPVOID context)
{
    CPxLCameraDoc* myClass = (CPxLCameraDoc*)context;
    
    while(WaitForSingleObject(myClass->m_CameraPollerStopEvent, 3000) != WAIT_OBJECT_0)
    {
        // time to look to see if there are any cameras now....
		U32 numCameras(0);
		PXL_RETURN_CODE rc = PxLGetNumberCameras(NULL, &numCameras);
		if (API_SUCCESS(rc)) {
		    if (numCameras > 0) {
		        // There is a camera there -- Send another WM_APP_DEFERRED_DEVICE_CHANGED message so the app will try again.
        		CWnd* wnd = theApp.GetMainWnd();
		        if (wnd != NULL) {
			       PostMessage(wnd->m_hWnd, WM_APP_DEFERRED_DEVICE_CHANGED, NULL, NULL);
			    }
		        break;
		    }
        }
    }
    
    myClass->m_CameraPoller = NULL;
    
    return 0;
}

/**
* Function: EventCallback
* Purpose:  This function recirds all events in the EVENTS_EDIT
*/
static U32 __stdcall CameraDisconnectCallback(
							HANDLE hCamera,
							U32    eventId,
                            double eventTimestamp,
                            U32    numDataBytes,
                            LPVOID pData,
                            LPVOID pContext)
{
    CPxLCameraDoc* myClass = (CPxLCameraDoc*)pContext;

	// The API has ruled that the camera is gone.  This only happens for nework (GEV) cameras -- treat this the same as a disconnect 
    // of a USB or FW camera.
	CWnd* wnd = theApp.GetMainWnd();
    if (wnd != NULL) 
    {
        PostMessage(wnd->m_hWnd, WM_APP_DEVICE_DISCONNECTED, NULL, NULL);
	}

    return ApiSuccess;
}

/////////////////////////////////////////////////////////////////////////////
// CPxLCameraDoc diagnostics

#ifdef _DEBUG
void CPxLCameraDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPxLCameraDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


















