#include "StdAfx.h"
#include "resource.h"
#include "help\popups.h"
#include "PxLDevApp.h"
#include "DevAppPage.h"

#pragma warning(disable:4786)

#include <string>
#include <map>


#define CHM_PATH _T(".\\help\\PixeLINK_Capture_OEM_Help.chm")


CString 
GetTopicAddress(const int contextId)
{
	static std::map<int, CString> contextMap;
	if (contextMap.empty())
	{
		contextMap[0]						= _T("index.htm");
		contextMap[IDH_SIMPLE_TAB]			= _T("SimpleTab.htm");
		contextMap[IDH_DESCRIPTORS_TAB]		= _T("DescriptorsTab.htm");
		contextMap[IDH_LOOKUPTABLES_TAB]	= _T("LookupTablesTab.htm");
		contextMap[IDH_SPECIALIZATIONS_TAB]	= _T("KneePointsTab.htm");
	}
	return CString(CHM_PATH _T("::/html/")) + contextMap[contextId];
}


// Returns true if the control id specifies one of the controls in the app that
// do not have a HelpId property, but for which we still want to show popup
// help.  This is the case for ActiveX controls.
bool 
HasPopupHelp(const int ctrlId)
{
	switch (ctrlId)
	{
	// Menu items:
	case ID_FILE_IMPORTCONFIGFILE:
	case ID_FILE_EXPORTCONFIGFILE:
	case ID_APP_EXIT:
	case ID_VIEW_ADVANCEDMODE:
	case ID_VIEW_ALWAYSONTOP:
	case ID_VIEW_RESETPREVIEWWINDOW:
	case ID_VIEW_RESTOREGUIDEFAULTS:
	case ID_VIEW_HISTOGRAM:
	case ID_APICALLLOG_CLEAR:
	case ID_APICALLLOG_FILTER:
	case ID_CAMERA_LOADCAMERASETTINGS:
	case ID_CAMERA_SAVECAMERASETTINGS:
	case ID_CAMERA_SETCAMERANAME:
	case ID_CAMERA_REINITIALIZE:
	case ID_HELP_CONTENTS:
	case ID_APP_ABOUT:
	// Controls:
	case IDC_SG_EXPOSURE_SLIDER:
	case IDC_SG_GAIN_SLIDER:
	case IDC_RG_SUBWINDOW_CTRL:
	case IDC_FG_FRAMERATE_SLIDER:
	case IDC_FG_GAMMA_SLIDER:
	case IDC_FG_HUE_SLIDER:
	case IDC_FG_SATURATION_SLIDER:
	case IDC_FG_SHARPNESS_SLIDER:
	case IDC_FG_COLORTEMP_SLIDER:
	case IDC_LT_LOOKUP_GRAPH:
	case IDC_KG_EXPOSURE_SLIDER:
	case IDC_KG_KNEEPOINT1_SLIDER:
	case IDC_KG_KNEEPOINT2_SLIDER:
	case IDC_KG_KNEEPOINT3_SLIDER:
	case IDC_KG_KNEEPOINT4_SLIDER:
		return true;
	default:
		return false;
	}
}


/**
* Function: GetHelpId
* Purpose:  
*/
UINT 
GetHelpId(const UINT ctrlId)
{
	for (DWORD const * pId = &g_dwHelpMap[0]; *pId != 0; pId += 2)
	{
		if (*pId == ctrlId) {
			return static_cast<UINT>(*(pId + 1));
		}
	}
	return IDH_TODO;
}


// Global Application Help function.
BOOL 
DevAppHelp(HELPINFO const * const pHelpInfo, CDevAppPage *pCurrentTab /*=NULL*/)
{
	static HH_POPUP popup = 
	{
		sizeof(HH_POPUP),				// cbStruct
		NULL,							// hinst
		IDH_TODO,						// idString
		NULL,							// pszText
		{0,0},							// pt
		RGB(0,0,0),						// clrForeground
		RGB(0xFF,0xFF,0xEE),			// clrBackground
		{-1,-1,-1,-1},					// rcMargins
		_T("MS Sans Serif,9,,")			// pszFont
	};

	//ConfigureHelpWindow();

	short state = GetKeyState(VK_F1);
	short shift = GetKeyState(VK_SHIFT);

	if (state < 0 && shift >= 0)   // F1 key is down, Shift key is not. Show help for the application.
	{
		// 2004-Dec.14 : I'm in the process of getting rid of the HTML Help - for now,
		//               just make it so that hitting F1 does nothing.
		/*
		int contextId;
		if (pCurrentTab != NULL 
			&& (contextId=pCurrentTab->GetHelpContextId()) > 0)
		{
			// Open in context of current tab
			HtmlHelp(AfxGetMainWnd()->GetSafeHwnd(),
					 GetTopicAddress(contextId),
					 HH_DISPLAY_TOPIC,
					 NULL);
		}
		else
		{
			// Open to contents
			ShowHelpContents();
		}
		*/
		ShowHelpContents();
	}
	else // F1 key is not down, or Shift-F1 was pressed. Get help for the specific control.
	{
		int ctrlId(0);
		CRect rctCtrl;

		if (pHelpInfo->iContextType == 1) // Dialog control
		{
			// Roundabout way to get the control id.  pHelpInfo->iCtrlId seems
			// to be wrong in some cases (eg: for ActiveX controls).
			HWND hWnd = static_cast<HWND>(pHelpInfo->hItemHandle);
			CWnd *pWnd;
			if (::IsWindow(hWnd))
				pWnd = CWnd::FromHandle(hWnd);
			else
				return FALSE;
			pWnd->GetWindowRect(&rctCtrl);

			ctrlId = pWnd->GetDlgCtrlID();
		}
		else if (pHelpInfo->iContextType == 2) // Menu Item
		{
			HMENU hMenu = static_cast<HMENU>(pHelpInfo->hItemHandle);
			if (::IsMenu(hMenu))
			{
				::GetMenuItemRect(AfxGetMainWnd()->m_hWnd, hMenu, 0, &rctCtrl);
			}
			else
				return FALSE;

			ctrlId = pHelpInfo->iCtrlId;
		}
		
		if (state < 0 && shift < 0)
		{
			// Called with Shift-F1 - position popup over control.
			popup.pt = rctCtrl.TopLeft();
			popup.pt.x += rctCtrl.Width() / 2;
		}
		else
		{
			// Called by mouse click - position popup at click location.
			popup.pt = pHelpInfo->MousePos;
			popup.pt.y -= 16; // Move it up a bit so the mouse-cursor doesn't obscure the first line of text.
		}


		// Show the popup if the HelpID property of the control is set, or if 
		// the control is one that does not have a HelpId property, but that we
		// want to show help for anyway.
		if (pHelpInfo->dwContextId > 0 || HasPopupHelp(ctrlId))
		{
			popup.idString = GetHelpId(ctrlId);

			HtmlHelp(AfxGetMainWnd()->m_hWnd,
					 CHM_PATH _T("::/popups.txt"), 
					 HH_DISPLAY_TEXT_POPUP, 
					 (DWORD)&popup);
		}
	}

	return TRUE;
}


/**
* Function: ShowHelpContents
* Purpose:  Open the HTML Help window to the Table of Contents.
*/
void 
ShowHelpContents(void)
{
	HtmlHelp(AfxGetMainWnd()->GetSafeHwnd(), CHM_PATH, HH_DISPLAY_TOC, NULL);
}


const DWORD 
g_dwHelpMap[] = {

/*
* Menu Items
*/
ID_FILE_IMPORTCONFIGFILE,				IDH_IMPORTCONFIGFILE,
ID_FILE_EXPORTCONFIGFILE,				IDH_EXPORTCONFIGFILE,
ID_APP_EXIT,							IDH_APP_EXIT,
ID_VIEW_ADVANCEDMODE,					IDH_VIEW_ADVANCEDMODE,
ID_VIEW_ALWAYSONTOP,					IDH_VIEW_ALWAYSONTOP,
ID_VIEW_RESETPREVIEWWINDOW,				IDH_VIEW_RESETPREVIEW,
ID_VIEW_RESTOREGUIDEFAULTS,				IDH_VIEW_RESTOREDEFAULTS,
ID_VIEW_HISTOGRAM,						IDH_VIEW_HISTOGRAM,
ID_APICALLLOG_CLEAR,					IDH_APILOG_CLEAR,
ID_APICALLLOG_FILTER,					IDH_APILOG_FILTER,
ID_CAMERA_LOADCAMERASETTINGS,			IDH_CAMERA_LOADSETTINGS,
ID_CAMERA_SAVECAMERASETTINGS,			IDH_CAMERA_SAVESETTINGS,
ID_CAMERA_SETCAMERANAME,				IDH_CAMERA_SETNAME,
ID_CAMERA_REINITIALIZE,					IDH_CAMERA_REINITIALIZE,
ID_HELP_CONTENTS,						IDH_HELP_CONTENTS,
ID_APP_ABOUT,							IDH_HELP_ABOUT,


/*
* Top-level dialog controls
*/
IDC_APICALL_LOG,						IDH_APICALL_LOG,


/*
* Controls common to all tabs.
*/
// Simple mode
IDC_SG_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_SG_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_SG_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_SG_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_SG_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_SG_STOP_BUTTON,						IDH_PREVIEW_STOP,

// Image Enhancement (Lookup Table)
IDC_LT_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_LT_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_LT_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_LT_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_LT_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_LT_STOP_BUTTON,						IDH_PREVIEW_STOP,

// Region of Interest
IDC_RG_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_RG_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_RG_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_RG_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_RG_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_RG_STOP_BUTTON,						IDH_PREVIEW_STOP,

// Image Capture
IDC_IG_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_IG_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_IG_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_IG_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_IG_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_IG_STOP_BUTTON,						IDH_PREVIEW_STOP,

// Video Capture
IDC_CG_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_CG_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_CG_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_CG_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_CG_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_CG_STOP_BUTTON,						IDH_PREVIEW_STOP,

// DCAM Features
IDC_FG_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_FG_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_FG_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_FG_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_FG_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_FG_STOP_BUTTON,						IDH_PREVIEW_STOP,

// Descriptors
IDC_DG_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_DG_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_DG_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_DG_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_DG_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_DG_STOP_BUTTON,						IDH_PREVIEW_STOP,

// Triggers & GPOs
IDC_GG_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_GG_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_GG_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_GG_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_GG_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_GG_STOP_BUTTON,						IDH_PREVIEW_STOP,

// Callbacks
IDC_CB_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_CB_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_CB_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_CB_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_CB_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_CB_STOP_BUTTON,						IDH_PREVIEW_STOP,

// Extended Shutter (Knee Points)
IDC_KG_CAMERASELECT_FRAME,				IDH_CAMERA_SELECT,
IDC_KG_CAMERASELECT_COMBO,				IDH_CAMERA_SELECT,
IDC_KG_VIDEOPREVIEW_FRAME,				IDH_PREVIEW,
IDC_KG_PAUSE_BUTTON,					IDH_PREVIEW_PAUSE,
IDC_KG_PLAY_BUTTON,						IDH_PREVIEW_PLAY,
IDC_KG_STOP_BUTTON,						IDH_PREVIEW_STOP,

 
/*
* Simple Mode controls.
*/
IDC_SG_EXPOSURE_FRAME,					IDH_EXPOSURE,
IDC_SG_EXPOSURE_SLIDER,					IDH_EXPOSURE,
IDC_SG_EXPOSURE_EDIT,					IDH_EXPOSURE_EDIT,
IDC_SG_EXPOSUREAUTO_BUTTON,				IDH_EXPOSURE_AUTO,

IDC_SG_GAIN_FRAME,						IDH_GAIN,
IDC_SG_GAIN_SLIDER,						IDH_GAIN,
IDC_SG_GAIN_EDIT,						IDH_GAIN_EDIT,
IDC_SG_GAINAUTO_BUTTON,					IDH_GAIN_AUTO,

IDC_SG_WINDOWSIZE_COMBO,				IDH_STANDARD_SIZES,
IDC_SG_WINDOWSIZE_FRAME,				IDH_STANDARD_SIZES,

IDC_SG_IMAGEFILENAME_LABEL,				IDH_IMAGE_FILE,
IDC_SG_IMAGEFILENAME_EDIT,				IDH_IMAGE_FILE,
IDC_SG_IMAGEFILEBROWSE_BUTTON,			IDH_IMAGE_FILE,
IDC_SG_IMAGEFILEFORMAT_COMBO,			IDH_IMAGE_FORMAT,
IDC_SG_IMAGEFILEFORMAT_LABEL,			IDH_IMAGE_FORMAT,
IDC_SG_IMAGECAPTURE_BUTTON,				IDH_IMAGE_CAPTURE,

IDC_SG_NUMFRAMES_LABEL,					IDH_CLIP_NUMFRAMES,
IDC_SG_NUMFRAMES_EDIT,					IDH_CLIP_NUMFRAMES,

/*
* Lookup Tables Tab controls.
*/
IDC_LT_ENABLELOOKUP_CHECK,				IDH_LOOKUP_ENABLE,
IDC_LT_AUTOUPDATE_CHECK,				IDH_LOOKUP_AUTOUPDATE,
IDC_LT_UPDATECAMERA_BUTTON,				IDH_LOOKUP_UPDATENOW,
IDC_LT_LOOKUPTABLE_LIST,				IDH_LOOKUP_LIST,
IDC_LT_LOOKUP_GRAPH,					IDH_LOOKUP_GRAPH,


/*
* ROI Tab controls.
*/
IDC_RG_APPLYTOROI_RADIO,				IDH_ROI_APPLYTO_STREAM,
IDC_RG_APPLYTOAUTOROI_RADIO,			IDH_ROI_APPLYTO_AUTO,

IDC_RG_SUBWINDOW_CTRL,					IDH_WINDOWLAYOUT,
IDC_RG_WINDOWHEIGHT_EDIT,				IDH_WINDOWLAYOUT_HEIGHT,
IDC_RG_WINDOWHEIGHT_LABEL,				IDH_WINDOWLAYOUT_HEIGHT,
IDC_RG_WINDOWLEFT_EDIT,					IDH_WINDOWLAYOUT_LEFT,
IDC_RG_WINDOWLEFT_LABEL,				IDH_WINDOWLAYOUT_LEFT,
IDC_RG_WINDOWTOP_EDIT,					IDH_WINDOWLAYOUT_TOP,
IDC_RG_WINDOWTOP_LABEL,					IDH_WINDOWLAYOUT_TOP,
IDC_RG_WINDOWWIDTH_EDIT,				IDH_WINDOWLAYOUT_WIDTH,
IDC_RG_WINDOWWIDTH_LABEL,				IDH_WINDOWLAYOUT_WIDTH,
IDC_RG_CENTER_BUTTON,					IDH_ROI_CENTER,

IDC_RG_WINDOWSIZE_FRAME,				IDH_STANDARD_SIZES,
IDC_RG_WINDOWSIZE_COMBO,				IDH_STANDARD_SIZES,

IDC_RG_DECIMATION_FRAME,				IDH_DECIMATION,
IDC_RG_DECIMATION_COMBO,				IDH_DECIMATION,


/*
* Images Tab controls.
*/
IDC_IG_FILENAME_LABEL,					IDH_IMAGE_FILE,
IDC_IG_FILENAME_EDIT,					IDH_IMAGE_FILE,
IDC_IG_FILEBROWSE_BUTTON,				IDH_IMAGE_FILE,

IDC_IG_FILEFORMAT_LABEL,				IDH_IMAGE_FORMAT,
IDC_IG_FILEFORMAT_COMBO,				IDH_IMAGE_FORMAT,

IDC_IG_INCREMENTFILENAME_CHECK,			IDH_INCREMENTFILENAME,
IDC_IG_PREVIEWAFTERCAPTURE_CHECK,		IDH_IMAGE_PREVIEWAFTERCAPTURE,
IDC_IG_CAPTUREFULLFRAME_CHECK,			IDH_IMAGE_CAPTUREFULLFRAME,

IDC_IG_TIMELAPSECAPTURE_CHECK,			IDH_TIMELAPSE_CAPTURE,
IDC_IG_CAPTUREEVERY_LABEL,				IDH_TIMELAPSE_PARAMS,
IDC_IG_CAPTUREEVERY_EDIT,				IDH_TIMELAPSE_PARAMS,
IDC_IG_CAPTUREEVERY_COMBO,				IDH_TIMELAPSE_PARAMS,
IDC_IG_CAPTUREFOR_LABEL,				IDH_TIMELAPSE_PARAMS,
IDC_IG_CAPTUREFOR_EDIT,					IDH_TIMELAPSE_PARAMS,
IDC_IG_CAPTUREFOR_COMBO,				IDH_TIMELAPSE_PARAMS,
IDC_IG_AUTOEXPOSE_CHECK,				IDH_TIMELAPSE_AUTOEXPOSE,

IDC_IG_CAPTURE_BUTTON,					IDH_IMAGE_CAPTURE,


/*
* Clips Tab controls.
*/
IDC_CG_NUMFRAMES_EDIT,					IDH_CLIP_NUMFRAMES,
IDC_CG_NUMFRAMES_LABEL,					IDH_CLIP_NUMFRAMES,

IDC_CG_CAPTUREFULLFRAME_CHECK,			IDH_CLIP_CAPTUREFULLFRAME,

IDC_CG_SAVERAWDATA_CHECK,				IDH_CLIP_SAVERAW,
IDC_CG_RAWDATAFILE_EDIT,				IDH_CLIP_RAWFILE,
IDC_CG_RAWDATABROWSE_BUTTON,			IDH_CLIP_RAWFILE,
IDC_CG_INCREMENTRAWDATAFILENAME_CHECK,	IDH_CLIP_RAWINCREMENT,

IDC_CG_SAVEFORMATTED_CHECK,				IDH_CLIP_SAVEFORMATTED,
IDC_CG_FORMATTEDFILE_EDIT,				IDH_CLIP_FILE,
IDC_CG_FORMATTEDBROWSE_BUTTON,			IDH_CLIP_FILE,
IDC_CG_CLIPFORMAT_LABEL,				IDH_CLIP_FORMAT,
IDC_CG_CLIPFORMAT_COMBO,				IDH_CLIP_FORMAT,
IDC_CG_INCREMENTFORMATTEDFILENAME_CHECK,IDH_CLIP_INCREMENT,
IDC_CG_VIEWAFTERCAPTURE_CHECK,			IDH_CLIP_VIEWAFTERCAPTURE,

IDC_CG_BEGINCAPTURE_BUTTON,				IDH_CLIP_CAPTURE,


/*
* Features Tab controls.
*/
IDC_FG_PIXELFORMAT_FRAME,				IDH_PIXELFORMAT,
IDC_FG_PIXELFORMAT_COMBO,				IDH_PIXELFORMAT,

IDC_FG_TEMPERATURE_FRAME,				IDH_TEMPERATURE,
IDC_FG_TEMPERATURE_EDIT,				IDH_TEMPERATURE,

IDC_FG_IMAGEFLIP_FRAME,					IDH_IMAGEFLIP,
IDC_FG_IMAGEFLIPHORIZONTAL_FRAME,		IDH_IMAGEFLIP,
IDC_FG_IMAGEFLIPHORIZONTALON_RADIO,		IDH_IMAGEFLIP,
IDC_FG_IMAGEFLIPHORIZONTALOFF_RADIO,	IDH_IMAGEFLIP,
IDC_FG_IMAGEFLIPVERTICAL_FRAME,			IDH_IMAGEFLIP,
IDC_FG_IMAGEFLIPVERTICALON_RADIO,		IDH_IMAGEFLIP,
IDC_FG_IMAGEFLIPVERTICALOFF_RADIO,		IDH_IMAGEFLIP,

IDC_FG_COLORTEMP_FRAME,				IDH_FEATURE_SLIDER,
IDC_FG_COLORTEMP_SLIDER,				IDH_FEATURE_SLIDER,
IDC_FG_COLORTEMP_EDIT,				IDH_FEATURE_EDIT,
IDC_FG_COLORTEMPAUTO_BUTTON,			IDH_FEATURE_AUTO,

IDC_FG_FRAMERATE_FRAME,					IDH_FEATURE_SLIDER,
IDC_FG_FRAMERATE_SLIDER,				IDH_FEATURE_SLIDER,
IDC_FG_FRAMERATE_EDIT,					IDH_FEATURE_EDIT,
IDC_FG_FRAMERATEAUTO_BUTTON,			IDH_FEATURE_AUTO,

IDC_FG_HUE_FRAME,						IDH_FEATURE_SLIDER,
IDC_FG_HUE_SLIDER,						IDH_FEATURE_SLIDER,
IDC_FG_HUE_EDIT,						IDH_FEATURE_EDIT,
IDC_FG_HUEAUTO_BUTTON,					IDH_FEATURE_AUTO,

IDC_FG_SATURATION_FRAME,				IDH_FEATURE_SLIDER,
IDC_FG_SATURATION_SLIDER,				IDH_FEATURE_SLIDER,
IDC_FG_SATURATION_EDIT,					IDH_FEATURE_EDIT,
IDC_FG_SATURATIONAUTO_BUTTON,			IDH_FEATURE_AUTO,

IDC_FG_SHARPNESS_FRAME,					IDH_FEATURE_SLIDER,
IDC_FG_SHARPNESS_SLIDER,				IDH_FEATURE_SLIDER,
IDC_FG_SHARPNESS_EDIT,					IDH_FEATURE_EDIT,
IDC_FG_SHARPNESSAUTO_BUTTON,			IDH_FEATURE_AUTO,

IDC_FG_GAMMA_FRAME,						IDH_FEATURE_SLIDER,
IDC_FG_GAMMA_SLIDER,					IDH_FEATURE_SLIDER,
IDC_FG_GAMMA_EDIT,						IDH_FEATURE_EDIT,
IDC_FG_GAMMAAUTO_BUTTON,				IDH_FEATURE_AUTO,


/*
* Descriptors Tab controls.
*/
IDC_DG_CAMERA_TREE,						IDH_CAMERA_TREE,
IDC_DG_CREATEDESCRIPTOR_BUTTON,			IDH_DESCRIPTOR_CREATE,
IDC_DG_DELETEDESCRIPTOR_BUTTON,			IDH_DESCRIPTOR_DELETE,
IDC_DG_DELETEALL_BUTTON,				IDH_DESCRIPTOR_DELETEALL,
IDC_DG_UPDATEMODE_FRAME,				IDH_DESCRIPTOR_UPDATEMODE,
IDC_DG_UPDATECAMERA_RADIO,				IDH_DESCRIPTOR_UPDATECAMERA,
IDC_DG_UPDATEHOST_RADIO,				IDH_DESCRIPTOR_UPDATEHOST,
IDC_DG_UPDATENOW_BUTTON,				IDH_DESCRIPTOR_UPDATENOW,

IDC_DG_BRIGHTNESS_BUTTON,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_BRIGHTNESS_EDIT,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_BRIGHTNESS_STATIC,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_DECIMATION_BUTTON,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_DECIMATION_EDIT,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_DECIMATION_STATIC,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_EXPOSURE_BUTTON,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_EXPOSURE_EDIT,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_EXPOSURE_STATIC,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_EXTENDEDSHUTTER_BUTTON,			IDH_DESCRIPTOR_FEATURE,
IDC_DG_EXTENDEDSHUTTER_EDIT,			IDH_DESCRIPTOR_FEATURE,
IDC_DG_EXTENDEDSHUTTER_STATIC,			IDH_DESCRIPTOR_FEATURE,
IDC_DG_FLIP_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_FLIP_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_FLIP_STATIC,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_FOCUS_BUTTON,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_FOCUS_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_FOCUS_STATIC,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_FRAMERATE_BUTTON,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_FRAMERATE_EDIT,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_FRAMERATE_STATIC,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_GAIN_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_GAIN_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_GAIN_STATIC,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_GAMMA_BUTTON,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_GAMMA_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_GAMMA_STATIC,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_GPIO_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_GPIO_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_GPIO_STATIC,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_HUE_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_HUE_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_HUE_STATIC,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_IRIS_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_IRIS_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_IRIS_STATIC,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_OPTICALFILTER_BUTTON,			IDH_DESCRIPTOR_FEATURE,
IDC_DG_OPTICALFILTER_EDIT,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_OPTICALFILTER_STATIC,			IDH_DESCRIPTOR_FEATURE,
IDC_DG_PAN_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_PAN_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_PAN_STATIC,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_PIXELFORMAT_BUTTON,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_PIXELFORMAT_EDIT,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_PIXELFORMAT_STATIC,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_ROI_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_ROI_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_ROI_STATIC,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_SATURATION_BUTTON,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_SATURATION_EDIT,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_SATURATION_STATIC,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_SHARPNESS_BUTTON,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_SHARPNESS_EDIT,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_SHARPNESS_STATIC,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_SHUTTER_BUTTON,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_SHUTTER_EDIT,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_SHUTTER_STATIC,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_TILT_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_TILT_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_TILT_STATIC,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_TRIGGER_BUTTON,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_TRIGGER_EDIT,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_TRIGGER_STATIC,					IDH_DESCRIPTOR_FEATURE,
IDC_DG_COLORTEMP_BUTTON,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_COLORTEMP_EDIT,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_COLORTEMP_STATIC,				IDH_DESCRIPTOR_FEATURE,
IDC_DG_ZOOM_BUTTON,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_ZOOM_EDIT,						IDH_DESCRIPTOR_FEATURE,
IDC_DG_ZOOM_STATIC,						IDH_DESCRIPTOR_FEATURE,


/*
* Triggers & GPOs Tab controls.
*/
IDC_GG_TRIGGERONOFF_CHECK,				IDH_TRIGGER_ONOFF,
IDC_GG_TRIGGERMODE_COMBO,				IDH_TRIGGER_MODE,
IDC_GG_TRIGGERTYPE_COMBO,				IDH_TRIGGER_TYPE,
IDC_GG_TRIGGERPOLARITY_COMBO,			IDH_TRIGGER_POLARITY,
IDC_GG_TRIGGERDELAY_EDIT,				IDH_TRIGGER_DELAY,
IDC_GG_TRIGGERPARAMETER_EDIT,			IDH_TRIGGER_PARAMETER,
IDC_GG_UPDATETRIGGER_BUTTON,			IDH_TRIGGER_UPDATE,
IDC_GG_TRIGGERDESCRIPTION_EDIT,			IDH_TRIGGER_DESCRIPTION,

IDC_GG_GPONNUMBER_COMBO,				IDH_GPO_NUMBER,
IDC_GG_GPOONOFF_CHECK,					IDH_GPO_ONOFF,
IDC_GG_GPOMODE_COMBO,					IDH_GPO_MODE,
IDC_GG_GPOPOLARITY_COMBO,				IDH_GPO_POLARITY,
IDC_GG_GPOPARAMETER1_EDIT,				IDH_GPO_PARAMETER,
IDC_GG_GPOPARAMETER2_EDIT,				IDH_GPO_PARAMETER,
IDC_GG_GPOPARAMETER3_EDIT,				IDH_GPO_PARAMETER,
IDC_GG_UPDATEGPO_BUTTON,				IDH_GPO_UPDATE,
IDC_GG_GPODESCRIPTION_EDIT,				IDH_GPO_DESCRIPTION,


/*
* Callbacks Tab controls.
*/
IDC_CB_PREVIEWFUNCTION_LABEL,			IDH_CALLBACK,
IDC_CB_PREVIEWFUNCTION_COMBO,			IDH_CALLBACK,
IDC_CB_PREVIEWFILE_LABEL,				IDH_CALLBACK_FILE,
IDC_CB_PREVIEWFILE_EDIT,				IDH_CALLBACK_FILE,
IDC_CB_PREVIEWBROWSE_BUTTON,			IDH_CALLBACK_FILE,

IDC_CB_IMAGEFUNCTION_LABEL,				IDH_CALLBACK,
IDC_CB_IMAGEFUNCTION_COMBO,				IDH_CALLBACK,
IDC_CB_IMAGEFILE_LABEL,					IDH_CALLBACK_FILE,
IDC_CB_IMAGEFILE_EDIT,					IDH_CALLBACK_FILE,
IDC_CB_IMAGEBROWSE_BUTTON,				IDH_CALLBACK_FILE,

IDC_CB_CLIPFUNCTION_LABEL,				IDH_CALLBACK,
IDC_CB_CLIPFUNCTION_COMBO,				IDH_CALLBACK,
IDC_CB_CLIPFILE_LABEL,					IDH_CALLBACK_FILE,
IDC_CB_CLIPFILE_EDIT,					IDH_CALLBACK_FILE,
IDC_CB_CLIPBROWSE_BUTTON,				IDH_CALLBACK_FILE,


/*
* Knee Points Tab controls.
*/
IDC_KG_EXPOSURE_SLIDER,					IDH_EXPOSURE,
IDC_KG_EXPOSURE_EDIT,					IDH_EXPOSURE_EDIT,
IDC_KG_EXPOSUREAUTO_BUTTON,				IDH_EXPOSURE_AUTO,

IDC_KG_KNEEPOINT1_CHECK,				IDH_KNEE_CHECK,
IDC_KG_KNEEPOINT1_SLIDER,				IDH_KNEE,
IDC_KG_KNEEPOINT1_EDIT,					IDH_KNEE,

IDC_KG_KNEEPOINT2_CHECK,				IDH_KNEE_CHECK,
IDC_KG_KNEEPOINT2_SLIDER,				IDH_KNEE,
IDC_KG_KNEEPOINT2_EDIT,					IDH_KNEE,

IDC_KG_KNEEPOINT3_CHECK,				IDH_KNEE_CHECK,
IDC_KG_KNEEPOINT3_SLIDER,				IDH_KNEE,
IDC_KG_KNEEPOINT3_EDIT,					IDH_KNEE,

IDC_KG_KNEEPOINT4_CHECK,				IDH_KNEE_CHECK,
IDC_KG_KNEEPOINT4_SLIDER,				IDH_KNEE,
IDC_KG_KNEEPOINT4_EDIT,					IDH_KNEE,


/*
* Dialogs
*/
// ROI dialog
IDC_RD_WINDOWHEIGHT_EDIT,				IDH_WINDOWLAYOUT_HEIGHT,
IDC_RD_WINDOWLEFT_EDIT,					IDH_WINDOWLAYOUT_LEFT,
IDC_RD_WINDOWTOP_EDIT,					IDH_WINDOWLAYOUT_TOP,
IDC_RD_WINDOWWIDTH_EDIT,				IDH_WINDOWLAYOUT_WIDTH,

// Trigger dialog
IDC_TD_TRIGGERDELAY_EDIT,				IDH_TRIGGER_DELAY,
IDC_TD_TRIGGERMODE_COMBO,				IDH_TRIGGER_MODE,
IDC_TD_TRIGGERPARAMETER_EDIT,			IDH_TRIGGER_PARAMETER,
IDC_TD_TRIGGERPOLARITY_COMBO,			IDH_TRIGGER_POLARITY,
IDC_TD_TRIGGERTYPE_COMBO,				IDH_TRIGGER_TYPE,

// GPO dialog
IDC_GD_GPOMODE_COMBO,					IDH_GPO_MODE,
IDC_GD_GPONNUMBER_COMBO,				IDH_GPO_NUMBER,
IDC_GD_GPOPARAMETER1_EDIT,				IDH_GPO_PARAMETER,
IDC_GD_GPOPARAMETER2_EDIT,				IDH_GPO_PARAMETER,
IDC_GD_GPOPARAMETER3_EDIT,				IDH_GPO_PARAMETER,
IDC_GD_GPOPOLARITY_COMBO,				IDH_GPO_POLARITY,


/**
 * Termination Marker. This final 0,0 MUST be here
 */
	0,0
};
























