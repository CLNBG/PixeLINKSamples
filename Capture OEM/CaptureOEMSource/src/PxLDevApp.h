#pragma once


#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "HelpTopics.h"
#include "UserSettings.h"

#include "PxLExceptions.h"

#include "Common.h"
#include "PxLCamera.h"
#include "PxLCameraDoc.h"


// Private messages.
#define WM_APP_PREVIEW_CLOSED       WM_APP + 0x0001
#define WM_APP_PREVIEW_ACTIVATED    WM_APP + 0x0002
#define WM_APP_FEATURE_CHANGED      WM_APP + 0x0003
#define WM_APP_HISTOGRAM_DATA       WM_APP + 0x0004
#define WM_APP_DEFERRED_DEVICE_CHANGED       WM_APP + 0x0005
#define WM_APP_DEVICE_RESET         WM_APP + 0x0006
#define WM_APP_DEVICE_DISCONNECTED  WM_APP + 0x0007


// As far as I can tell, it doesn't matter what ID you use for timers, but
// it seems like a good idea to at least make sure that all the timers in the
// application have distinct IDs.
#define FEATURE_GUI_TIMER_ID        5001
#define CLIP_CAPTURE_DLG_TIMER_ID   5002
#define HISTOGRAM_DLG_TIMER_ID      5003
#define AUTO_EXPOSURE_TIMER_ID      5004
#define AUTO_GAIN_TIMER_ID          5005
#define TIME_LAPSE_TIMER_ID         5006
#define TIME_LAPSE_TIMER2_ID        5007
#define TEMPERATURE_TIMER_ID        5008
#define IMAGE_CAPTURE_DLG_TIMER_ID  5009
#define ONE_TIME_AUTO_TIMER_ID      5010
#define PTP_TIMER_ID                5011

class CDevAppPage; // Forward declaration

// Support for the application help system. The following are
// defined in HelpMap.cpp.
BOOL DevAppHelp(HELPINFO const * const pHelpInfo, CDevAppPage *pCurrentTab = NULL);
void ShowHelpContents(void);
extern const DWORD g_dwHelpMap[];


/******************************************************************************
* Class CPxLDevApp:
******************************************************************************/

class CPxLDevApp : public CWinApp
{
    DECLARE_MESSAGE_MAP()

public:
    CPxLDevApp();

    virtual BOOL InitInstance();

    UserSettings& GetUserSettings();

    bool    IsStartingUp() const { return m_appIsStartingUp; }

    afx_msg void OnHelpContents();

public:
    // Public member data
    bool            m_bAutoResetPreview;
    bool            m_bAlwaysOnTop;
    int             m_framerateDivisor;                 // for Laptop Friendly Mode
    bool            m_bAutoPreview;                     // if true, auto-start preview on start-up
    bool            m_bAllowMultipleInstances;          // if true, auto-start preview on start-up
    bool            m_bAllowConnectToMultipleCameras;   // if false uninitialize current camera when selecting another camera
    bool            m_bCmdLineHasBadParameters; // true iff an unrecognized parameters is seen
    bool            m_bShowCmdLineParameterHelp;
    bool            m_bRegistrySettingsLoaded;
    bool            m_bEnableJumboFrames;
    bool            m_bMonitorAccessOnly;              

protected:
    virtual int ExitInstance();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
    bool            m_appIsStartingUp;
    UserSettings    m_settings;

};


// Make the global application instance (defined in PxLDevApp.cpp) visible
// throughout the application.
extern CPxLDevApp theApp;













