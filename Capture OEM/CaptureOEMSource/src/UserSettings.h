#pragma once


#include <map>


// Names of the registry keys under which each setting is stored.
// Prefixes are:
//		USS_ for "User Setting String"
//		USI_ for "User Setting Int"
//		USF_ for "User Setting Float"
//      USB_ for "User Setting Boolean"
extern TCHAR const
	// General
	*USI_FRAMEWINDOW_LEFT,
	*USI_FRAMEWINDOW_TOP,
	*USI_FRAMEWINDOW_HEIGHT,
	*USB_ADVANCED_MODE,
	*USI_ACTIVE_TAB,
	*USI_LAST_USED_SERIAL,
	*USI_PREVIEW_LEFT,
	*USI_PREVIEW_TOP,
	*USI_PREVIEW_WIDTH,
	*USI_PREVIEW_HEIGHT,
	*USI_FRAMERATE_DIVISOR,


	// Image Capture
	*USS_IMAGE_FILE_PATH,
	*USI_IMAGE_FILE_FORMAT_INDEX,
	*USB_IMAGE_INCREMENT_AFTER_CAPTURE,
	*USB_IMAGE_PREVIEW_AFTER_CAPTURE,
	*USB_IMAGE_CAPTURE_FULL_FRAME,
	*USB_IMAGE_CAPTURE_FULL_RES,
	*USB_IMAGE_CAPTURE_AVERAGE_FRAMES,
	*USI_IMAGE_CAPTURE_AVERAGE_FRAMES,
	*USB_IMAGE_TIME_LAPSE,
	*USI_IMAGE_CAPTURE_EVERY_VALUE,
	*USI_IMAGE_CAPTURE_EVERY_UNIT,
	*USI_IMAGE_CAPTURE_FOR_VALUE,
	*USI_IMAGE_CAPTURE_FOR_UNIT,
	*USB_IMAGE_AUTOEXPOSE_EACH,

	// Clip Capture
	*USI_CLIP_NUM_FRAMES,
    *USF_CLIP_CAPTURE_TIME,
    *USI_CLIP_ENCODING_INDEX,
	*USI_CLIP_DECIMATION_FACTOR,
    *USB_CLIP_MATCH_CAPTURE,
    *USF_CLIP_PB_FRAME_RATE,
    *USB_CLIP_AUTO_BIT_RATE,
    *USI_CLIP_PB_BIT_RATE,
    *USF_CLIP_PLAYBACK_TIME,
	*USB_CLIP_VIEW_AFTER_CAPTURE,
	*USB_RAW_SAVE_FILE,
	*USS_RAW_FILE_PATH,
	*USB_RAW_INCREMENT_AFTER_CAPTURE,
	*USB_CLIP_SAVE_FILE,
	*USS_CLIP_FILE_PATH,
	*USI_CLIP_FILE_FORMAT_INDEX,
	*USB_CLIP_INCREMENT_AFTER_CAPTURE,
	
	*USB_AUTO_RESET_PREVIEW,
	*USB_ALWAYS_ON_TOP,

	// Files used by callback functions.
	*USS_CALLBACK_PREVIEW_FILE,
	*USS_CALLBACK_IMAGE_FILE,
	*USS_CALLBACK_CLIP_FILE;


class UserSettings
{
public:
	UserSettings(void); // Can only be created by CPxLDevApp
	~UserSettings(void);

	CString GetStringSetting(TCHAR const* settingName);
	int   GetIntSetting(TCHAR const* settingName);
	float GetFloatSetting(TCHAR const* settingName);
	bool  GetBoolSetting(TCHAR const* settingName);

	void SetStringSetting(TCHAR const* settingName, CString value);
	void SetIntSetting(TCHAR const* settingName, int value);
	void SetFloatSetting(TCHAR const* settingName, float value);
	void SetBoolSetting(TCHAR const* settingName, bool value);

	bool LoadFromRegistry();
	bool SaveToRegistry();
	bool LoadDefaults(bool reloadOnly);

private:
	// Map the name of each setting to its value.
	std::map<TCHAR const*, CString>	m_stringSettings;
	std::map<TCHAR const*, int>		m_intSettings;
	std::map<TCHAR const*, float>	m_floatSettings;
	std::map<TCHAR const*, bool>	m_boolSettings;

};
