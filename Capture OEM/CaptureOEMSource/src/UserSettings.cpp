#include "stdafx.h"
#include <shlobj.h>
#include "UserSettings.h"


static TCHAR const* DEMOAPP_SETTINGS_REG_KEY = _T("Software\\PixeLINK\\PxLDevApp\\Settings");

// Names of the registry keys under which each setting is stored.
// Prefixes are:
//      USS_ for "User Setting - String"
//      USI_ for "User Setting - Int"
//      USF_ for "User Setting - Float"
//      USB_ for "User Setting - Boolean"
TCHAR const
	// General
	*USI_FRAMEWINDOW_LEFT					= _T("Window Placement Left"),
	*USI_FRAMEWINDOW_TOP					= _T("Window Placement Top"),
	*USB_ADVANCED_MODE						= _T("Advanced Mode"),
	*USI_ACTIVE_TAB							= _T("Active Tab"),
	*USI_FRAMERATE_DIVISOR					= _T("Framerate Divisor"),
	*USI_LAST_USED_SERIAL					= _T("Last Used Serial Number"),
	*USI_PREVIEW_LEFT						= _T("Preview Left"),
	*USI_PREVIEW_TOP						= _T("Preview Top"),
	*USI_PREVIEW_WIDTH						= _T("Preview Width"),
	*USI_PREVIEW_HEIGHT						= _T("Preview Height"),

	*USB_AUTO_RESET_PREVIEW					= _T("Auto Reset Preview"),
	*USB_ALWAYS_ON_TOP  					= _T("Always On Top"),

	// Image Capture
	*USS_IMAGE_FILE_PATH					= _T("Image File Path"),
	*USI_IMAGE_FILE_FORMAT_INDEX			= _T("Image Format Index"),
	*USB_IMAGE_INCREMENT_AFTER_CAPTURE		= _T("Image Increment Filename"),
	*USB_IMAGE_PREVIEW_AFTER_CAPTURE		= _T("Image Preview After Capture"),
	*USB_IMAGE_CAPTURE_FULL_FRAME			= _T("Image Capture Full Frame"),
	*USB_IMAGE_CAPTURE_FULL_RES				= _T("Image Capture Full Resolution"),
	*USB_IMAGE_CAPTURE_AVERAGE_FRAMES		= _T("Image Capture Average Frames"),
	*USI_IMAGE_CAPTURE_AVERAGE_FRAMES		= _T("Image Capture Average Frames Count"),
	*USB_IMAGE_TIME_LAPSE					= _T("Image Capture Time Lapse"),
	*USI_IMAGE_CAPTURE_EVERY_VALUE			= _T("Image TL-Capture Every Value"),
	*USI_IMAGE_CAPTURE_EVERY_UNIT			= _T("Image TL-Capture Every Units"),
	*USI_IMAGE_CAPTURE_FOR_VALUE			= _T("Image TL-Capture For Value"),
	*USI_IMAGE_CAPTURE_FOR_UNIT				= _T("Image TL-Capture For Units"),
	*USB_IMAGE_AUTOEXPOSE_EACH				= _T("Image TL-AutoExpose Each"),

	// Clip Capture
	*USI_CLIP_NUM_FRAMES					= _T("Clip Number Frames"),
	*USF_CLIP_CAPTURE_TIME					= _T("Clip Capture Time"),
    *USI_CLIP_ENCODING_INDEX                = _T("Clip Encoding Index"),
	*USI_CLIP_DECIMATION_FACTOR				= _T("Clip Decimation Factor"),
    *USB_CLIP_MATCH_CAPTURE                 = _T("Clip Match Capture Frame Rate"),
    *USF_CLIP_PB_FRAME_RATE                 = _T("Clip Playback Frame Rate"),
    *USB_CLIP_AUTO_BIT_RATE                 = _T("Clip Auto Bit Rate"),
    *USI_CLIP_PB_BIT_RATE                   = _T("Clip Playback Bit Rate"),
	*USF_CLIP_PLAYBACK_TIME					= _T("Clip Playback Time"),
	*USB_CLIP_VIEW_AFTER_CAPTURE			= _T("Clip View After Capure"),
	*USB_RAW_SAVE_FILE						= _T("Raw Save File"),
	*USS_RAW_FILE_PATH						= _T("Raw File Path"),
	*USB_RAW_INCREMENT_AFTER_CAPTURE		= _T("Raw Increment After Capture"),
	*USB_CLIP_SAVE_FILE						= _T("Clip Save File"),
	*USS_CLIP_FILE_PATH						= _T("Clip File Path"),
	*USI_CLIP_FILE_FORMAT_INDEX				= _T("Clip Format Index"),
	*USB_CLIP_INCREMENT_AFTER_CAPTURE		= _T("Clip Increment After Capture"),

	// Files used by callback functions.
	*USS_CALLBACK_PREVIEW_FILE				= _T("Callback Preview File"),
	*USS_CALLBACK_IMAGE_FILE				= _T("Callback Image File"),
	*USS_CALLBACK_CLIP_FILE					= _T("Callback Clip File");


// Categorize the user settings by the data type they are stored as in the registry.
static TCHAR const* StringSettings[] = 
{
	USS_IMAGE_FILE_PATH,
	USS_RAW_FILE_PATH,
	USS_CLIP_FILE_PATH,
	USS_CALLBACK_PREVIEW_FILE,
	USS_CALLBACK_IMAGE_FILE,
	USS_CALLBACK_CLIP_FILE,
};
static int const nStringSettings = sizeof(StringSettings)/sizeof(TCHAR*);

static TCHAR const* IntSettings[] = 
{
	USI_FRAMEWINDOW_LEFT,
	USI_FRAMEWINDOW_TOP,
	USI_ACTIVE_TAB,
	USI_FRAMERATE_DIVISOR,
	USI_LAST_USED_SERIAL,
	USI_PREVIEW_LEFT,
	USI_PREVIEW_TOP,
	USI_PREVIEW_WIDTH,
	USI_PREVIEW_HEIGHT,

	USI_IMAGE_CAPTURE_AVERAGE_FRAMES,
	USI_IMAGE_FILE_FORMAT_INDEX,
	USI_IMAGE_CAPTURE_EVERY_VALUE,
	USI_IMAGE_CAPTURE_EVERY_UNIT,
	USI_IMAGE_CAPTURE_FOR_VALUE,
	USI_IMAGE_CAPTURE_FOR_UNIT,

	USI_CLIP_NUM_FRAMES,
	USI_CLIP_DECIMATION_FACTOR,
    USI_CLIP_ENCODING_INDEX,
    USI_CLIP_PB_BIT_RATE,
	USI_CLIP_FILE_FORMAT_INDEX,
};
static int const nIntSettings = sizeof(IntSettings)/sizeof(TCHAR*);

static TCHAR const* FloatSettings[] = 
{
	USF_CLIP_CAPTURE_TIME,
	USF_CLIP_PB_FRAME_RATE,
	USF_CLIP_PLAYBACK_TIME,
};
static int const nFloatSettings = sizeof(FloatSettings)/sizeof(TCHAR*);

static TCHAR const* BoolSettings[] = 
{
	USB_ADVANCED_MODE,
	USB_AUTO_RESET_PREVIEW,
	USB_ALWAYS_ON_TOP,

	USB_IMAGE_INCREMENT_AFTER_CAPTURE,
	USB_IMAGE_PREVIEW_AFTER_CAPTURE,
	USB_IMAGE_CAPTURE_FULL_FRAME,
	USB_IMAGE_CAPTURE_FULL_RES,
	USB_IMAGE_CAPTURE_AVERAGE_FRAMES,
	USB_IMAGE_TIME_LAPSE,
	USB_IMAGE_AUTOEXPOSE_EACH,

    USB_CLIP_MATCH_CAPTURE,
    USB_CLIP_AUTO_BIT_RATE,
	USB_CLIP_VIEW_AFTER_CAPTURE,
	USB_RAW_SAVE_FILE,
	USB_RAW_INCREMENT_AFTER_CAPTURE,
	USB_CLIP_SAVE_FILE,
	USB_CLIP_INCREMENT_AFTER_CAPTURE
};
static int const nBoolSettings = sizeof(BoolSettings)/sizeof(TCHAR*);


/**
* Function: LoadDefaults
* Purpose:  
*/
bool UserSettings::LoadDefaults(bool reloadOnly)
{
	// The following settings are here only as *initial* defaults - that is,
	// these are defaults that are only used when the application is run for
	// the first time. They are *not* reset to their default values when the
	// user selects "Restore GUI Defaults" from the View menu.
	if (!reloadOnly)
	{
		// Set default directories to user's "My Pictures" directory, if there is one.
		TCHAR path[MAX_PATH];
		CString pathString;
		if (SHGetSpecialFolderPath(NULL, path, CSIDL_PERSONAL, false))
		{
			pathString = path;
		}
		else
		{
			pathString = _T("C:");
		}

		// File Paths
		m_stringSettings[USS_IMAGE_FILE_PATH]		= pathString + CString(_T("\\image.bmp"));
		m_stringSettings[USS_CLIP_FILE_PATH]		= pathString + CString(_T("\\clip.avi"));
		m_stringSettings[USS_RAW_FILE_PATH]			= pathString + CString(_T("\\clip.h264"));

		// Files used by callbacks
		m_stringSettings[USS_CALLBACK_PREVIEW_FILE]	= pathString + CString(_T("\\image.bmp"));
		m_stringSettings[USS_CALLBACK_IMAGE_FILE]	= pathString + CString(_T("\\image.bmp"));
		m_stringSettings[USS_CALLBACK_CLIP_FILE]	= pathString + CString(_T("\\image.bmp"));

		// General settings
		m_intSettings[USI_FRAMEWINDOW_LEFT]		= -1; // -1 means "not set yet"
		m_intSettings[USI_FRAMEWINDOW_TOP]		= -1;
		m_boolSettings[USB_ADVANCED_MODE]		= true;
		m_intSettings[USI_ACTIVE_TAB]			= 0;
		m_intSettings[USI_FRAMERATE_DIVISOR]	= 1; // 2007-Jan-4 - default to off.
		m_intSettings[USI_LAST_USED_SERIAL]		= 0;
		m_intSettings[USI_PREVIEW_LEFT]			= -1; // not specified
		m_intSettings[USI_PREVIEW_TOP]			= -1; // not specified
		m_intSettings[USI_PREVIEW_WIDTH]		= -1; // not specified
		m_intSettings[USI_PREVIEW_HEIGHT]		= -1; // not specified

		// Image Capture
		m_intSettings[USI_IMAGE_FILE_FORMAT_INDEX]			= 0;

		// Clip Capture
		m_intSettings[USI_CLIP_FILE_FORMAT_INDEX]			= 0;
	}

	// The rest of the settings are reset when the user selects
	// "Restore GUI Defaults".

	m_boolSettings[USB_AUTO_RESET_PREVIEW]				= true;
	m_boolSettings[USB_ALWAYS_ON_TOP]   				= false;

	// Image Capture
	m_boolSettings[USB_IMAGE_INCREMENT_AFTER_CAPTURE]	= false;
	m_boolSettings[USB_IMAGE_PREVIEW_AFTER_CAPTURE]		= false;
	m_boolSettings[USB_IMAGE_CAPTURE_FULL_FRAME]		= false;
	m_boolSettings[USB_IMAGE_CAPTURE_FULL_RES]			= false;
	m_boolSettings[USB_IMAGE_CAPTURE_AVERAGE_FRAMES]	= false;
	m_intSettings[USI_IMAGE_CAPTURE_AVERAGE_FRAMES]		= 1;
	m_boolSettings[USB_IMAGE_TIME_LAPSE]				= false;
	m_intSettings[USI_IMAGE_CAPTURE_EVERY_VALUE]		= 10;	// Default to capturing every 10 seconds for 1 minute
	m_intSettings[USI_IMAGE_CAPTURE_EVERY_UNIT]			= 1;	// seconds
	m_intSettings[USI_IMAGE_CAPTURE_FOR_VALUE]			= 1;
	m_intSettings[USI_IMAGE_CAPTURE_FOR_UNIT]			= 2;	// minutes
	m_boolSettings[USB_IMAGE_AUTOEXPOSE_EACH]			= false;

	// Clip Capture
	m_intSettings[USI_CLIP_NUM_FRAMES]					= 100;
	m_floatSettings[USF_CLIP_CAPTURE_TIME]			    = 10.0;
	m_intSettings[USI_CLIP_ENCODING_INDEX]	      		= CLIP_ENCODING_H264;  // Bugzilla.1943, default to H.264 for now)
	m_intSettings[USI_CLIP_DECIMATION_FACTOR]			= 1;
    m_boolSettings[USB_CLIP_MATCH_CAPTURE]              = true;
    m_boolSettings[USB_CLIP_AUTO_BIT_RATE]              = true;
    m_intSettings[USI_CLIP_PB_BIT_RATE]                 = 1000000;
	m_boolSettings[USB_CLIP_VIEW_AFTER_CAPTURE]			= false;
	m_boolSettings[USB_RAW_SAVE_FILE]					= false;
	m_boolSettings[USB_RAW_INCREMENT_AFTER_CAPTURE]		= false;
	m_boolSettings[USB_CLIP_SAVE_FILE]					= true;
	m_boolSettings[USB_CLIP_INCREMENT_AFTER_CAPTURE]	= false;

	return true;

} // LoadDefaults


/**
* Function: UserSettings
* Purpose:  Constructor
*/
UserSettings::UserSettings(void)
{
}


/**
* Function: UserSettings
* Purpose:  Destructor
*/
UserSettings::~UserSettings(void)
{
}


/**
* Function: GetStringSetting
* Purpose:  
*/
CString UserSettings::GetStringSetting(TCHAR const* settingName)
{
	return m_stringSettings[settingName];
}


/**
* Function: GetIntSetting
* Purpose:  
*/
int UserSettings::GetIntSetting(TCHAR const* settingName)
{
	return m_intSettings[settingName];
}


/**
* Function: GetFloatSetting
* Purpose:  
*/
float UserSettings::GetFloatSetting(TCHAR const* settingName)
{
	return m_floatSettings[settingName];
}


/**
* Function: GetBoolSetting
* Purpose:  
*/
bool UserSettings::GetBoolSetting(TCHAR const* settingName)
{
	return m_boolSettings[settingName];
}


/**
* Function: SetStringSetting
* Purpose:  
*/
void UserSettings::SetStringSetting(TCHAR const* settingName, CString value)
{
	m_stringSettings[settingName] = value;
}


/**
* Function: SetIntSetting
* Purpose:  
*/
void UserSettings::SetIntSetting(TCHAR const* settingName, int value)
{
	m_intSettings[settingName] = value;
}

/**
* Function: SetFloatSetting
* Purpose:  
*/
void UserSettings::SetFloatSetting(TCHAR const* settingName, float value)
{
	m_floatSettings[settingName] = value;
}


/**
* Function: SetBoolSetting
* Purpose:  
*/
void UserSettings::SetBoolSetting(TCHAR const* settingName, bool value)
{
	m_boolSettings[settingName] = value;
}


/**
* Function: LoadFromRegistry
* Purpose:  
*/
bool UserSettings::LoadFromRegistry()
{
	// First, we load the defaults. That way, if any keys are missing from
	// the registry, that setting will have its default value, instead of 
	// no value. If all keys are present, then everyhting we load in
	// LoadDefaults will be overwritten.
	this->LoadDefaults(false);

	HKEY hKey;
	DWORD createOrOpen;
	LONG retVal = ::RegCreateKeyEx(HKEY_CURRENT_USER,
									DEMOAPP_SETTINGS_REG_KEY,
									0,
									NULL,
									REG_OPTION_NON_VOLATILE,
									KEY_QUERY_VALUE,
									NULL,
									&hKey,
									&createOrOpen);

	if (retVal != ERROR_SUCCESS) // Don't ask me why RegCreateKeyEx returns ERROR_SUCCESS...
	{
		// Failed to open (or create) the key.
		// Settings will be the defaults.
		return false;
	}

	DWORD regType, cbData;
	BYTE data[MAX_PATH];
	TCHAR const* settingName;
	int idx;

	// Load up the string settings:
	for (idx = 0; idx < nStringSettings; idx++)
	{
		settingName = StringSettings[idx];
		cbData = MAX_PATH;
		retVal = ::RegQueryValueEx(hKey, settingName, 0, &regType, &data[0], &cbData);
		if (retVal == ERROR_SUCCESS)
		{
			m_stringSettings[settingName] = CString(data);
		}
		else
		{
			// Did not find the key? Couldn't read the value?
		}
	}

	// Load the integer settings:
	for (idx = 0; idx < nIntSettings; idx++)
	{
		settingName = IntSettings[idx];
		cbData = MAX_PATH;
		retVal = ::RegQueryValueEx(hKey, settingName, 0, &regType, &data[0], &cbData);
		if (retVal == ERROR_SUCCESS)
		{
			m_intSettings[settingName] = *(reinterpret_cast<int*>(data));
		}
		else
		{
			// Did not find the key? Couldn't read the value?
		}
	}

	// Load the float settings:
	for (idx = 0; idx < nFloatSettings; idx++)
	{
		settingName = FloatSettings[idx];
		cbData = MAX_PATH;
		retVal = ::RegQueryValueEx(hKey, settingName, 0, &regType, &data[0], &cbData);
		if (retVal == ERROR_SUCCESS)
		{
			m_floatSettings[settingName] = *(reinterpret_cast<float*>(data));
		}
		else
		{
			// Did not find the key? Couldn't read the value?
		}
	}

	// Load the boolean settings:
	for (idx = 0; idx < nBoolSettings; idx++)
	{
		settingName = BoolSettings[idx];
		cbData = MAX_PATH;
		retVal = ::RegQueryValueEx(hKey, settingName, 0, &regType, &data[0], &cbData);
		if (retVal == ERROR_SUCCESS)
		{
			m_boolSettings[settingName] = (0 != *(reinterpret_cast<int*>(data)));
		}
		else
		{
			// Did not find the key? Couldn't read the value?
		}
	}

	::RegCloseKey(hKey);

	return true;

} // LoadFromRegistry


/**
* Function: SaveToRegistry
* Purpose:  
*/
bool UserSettings::SaveToRegistry()
{
	HKEY hKey;
	DWORD createOrOpen;
	LONG retVal = ::RegCreateKeyEx(HKEY_CURRENT_USER,
									DEMOAPP_SETTINGS_REG_KEY,
									0,
									NULL,
									REG_OPTION_NON_VOLATILE,
									KEY_WRITE,
									NULL,
									&hKey,
									&createOrOpen);

	if (retVal != ERROR_SUCCESS) // Don't ask me why RegCreateKeyEx returns ERROR_SUCCESS...
	{
		// Failed to open (or create) the key.
		return false;
	}

	DWORD cbData;
	TCHAR const* settingName;
	int idx;

	// Save the string settings:
	for (idx = 0; idx < nStringSettings; idx++)
	{
		settingName = StringSettings[idx];
		// Get length of the string in bytes, then add one for the terminating NULL char.
		cbData = (m_stringSettings[settingName].GetLength() + 1) * sizeof(TCHAR);
		retVal = ::RegSetValueEx(hKey, 
								 settingName,
								 0,
								 REG_SZ,
								 reinterpret_cast<BYTE*>(m_stringSettings[settingName].GetBuffer(0)),
								 cbData);

		if (retVal != ERROR_SUCCESS)
		{
			// Couldn't write the value?
		}
	}

	// Save the integer settings:
	cbData = sizeof(int);
	for (idx = 0; idx < nIntSettings; idx++)
	{
		settingName = IntSettings[idx];
		int val = m_intSettings[settingName];
		retVal = ::RegSetValueEx(hKey,
								 settingName,
								 0,
								 REG_DWORD,
								 reinterpret_cast<BYTE*>(&val),
								 cbData);

		if (retVal != ERROR_SUCCESS)
		{
			// Couldn't write the value?
		}
	}

	// Save the float settings:
	cbData = sizeof(float);
	for (idx = 0; idx < nFloatSettings; idx++)
	{
		settingName = FloatSettings[idx];
		float val = m_floatSettings[settingName];
		retVal = ::RegSetValueEx(hKey,
								 settingName,
								 0,
								 REG_DWORD,
								 reinterpret_cast<BYTE*>(&val),
								 cbData);

		if (retVal != ERROR_SUCCESS)
		{
			// Couldn't write the value?
		}
	}

	// Save the boolean settings:
	cbData = sizeof(int);
	for (idx = 0; idx < nBoolSettings; idx++)
	{
		settingName = BoolSettings[idx];
		int val = m_boolSettings[settingName] ? 1 : 0;
		retVal = ::RegSetValueEx(hKey,
								 settingName,
								 0,
								 REG_DWORD,
								 reinterpret_cast<BYTE*>(&val),
								 cbData);

		if (retVal != ERROR_SUCCESS)
		{
			// Couldn't write the value?
		}
	}

	::RegCloseKey(hKey);

	return true;

} // SaveToRegistry













