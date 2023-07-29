// Stuff that is used in various places in the project.

#include "StdAfx.h"
// TEMP PEC +++
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501

#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#define NTDDI_VERSION 0x05010000
#include <Windows.h>
// ----
#include "Common.h"
#include <PixeLINKTypes.h>
#include "PxLCamera.h"
#include <shlwapi.h>
#include <algorithm>


/**
* Function: ErrorMessage
* Purpose:  Display an error message.
*/
void ErrorMessage(LPCTSTR msg)
{
	AfxMessageBox(msg, MB_OK);
}

#ifdef _UNICODE
// Overload - this version accepts the char* that std::string::c_str returns.
void ErrorMessage(LPCSTR msg)
{
	AfxMessageBox(CString(msg), MB_OK);
}
#endif


/**
* Function: TCHARtoStdString
* Purpose:  This app attempts to be "Unicode aware" - we use _T("foo") instead
*           of "foo", and TCHAR instead of char. This means that if the app
*           is compiled with _UNICODE defined, all strings in the application
*           are made up of 2-byte characters (wchar_t). However, sometimes we 
*           still need a char* to pass to an API call, or for some other reason.
*           These two overloaded functions are here to help in that situation.
*           In non-unicode mode, this version is called, which is a bit of a
*           waste of time - it essentially does nothing, but the alternative
*           would be to put ugly #ifdef _UNICODE blocks in the code, so I
*           decided to go for elegance before efficiency in this case, since
*           we do very little string manipulation anyway.
*/
std::string TStringToStdString(char const* cstr, bool appendNullChar /*=true*/)
{
	std::string s(cstr);
	if (appendNullChar)
		//s.push_back('\0'); // vc7
		s += '\0';      // vc6
	return s;
}


/**
* Function: TCHARtoStdString
* Purpose:  See note above.
*           This is the Unicode version of this function.
*/
std::string
TStringToStdString(wchar_t const* wstr, bool appendNullChar /*=true*/)
{
	int len = wcslen(wstr);
	std::string s(len + (appendNullChar ? 1 : 0), '\0');
	for (int i = 0; i < len; i++)
		s[i] = static_cast<char>(wstr[i]); // ???
	return s;
}


/******************************************************************************
* Standard ROI sizes
******************************************************************************/

// Used to populate the "Standard Sizes" combo box.
static int standardSizes[] = 
{
// As per Bugzilla.530 -- the width of these sould be a multiple of 32.
//	width,	height,
    32,     32,
    64,     64,
    128,    128,
    256,    256,
	320,	240,
	640,	480,
	800,	600,
	1024,	768,
	1280,	1024,
	1600,	1200,
	1920,	1080,
	2560,	1440,
	4096,   2160,

	0 // Important - termination marker.
};


/**
* Function: SetWindowSizeEntries
* Purpose:  Put items in the "Standard Sizes" combo box.
*/
void SetWindowSizeEntries(CComboBox & cb, 
						  int minWidth, int minHeight, 
						  int maxWidth, int maxHeight,
						  bool sideways /*=false*/)
{
	int* oldSel = (int*)cb.GetItemDataPtr(cb.GetCurSel());
	int   oldWidth = 0, oldHeight = 0;
	if (oldSel && oldSel != (int*)-1)
	{
	    oldWidth = oldSel[0];
	    oldHeight = oldSel[1];
	}

	ClearComboBox(cb);

	CString s;
	int selIndex = -1;
	bool have_max = false;
	for (int* pDims = &standardSizes[0]; *pDims != 0; pDims += 2)
	{
		if (   pDims[0] <= maxWidth 
			&& pDims[1] <= maxHeight
			&& pDims[0] >= minWidth
			&& pDims[1] >= minHeight )
		{
			s.Format(_T("%d x %d"), pDims[0], pDims[1]);
			int i = cb.AddString(s);

			// Point the ItemData of the entry to the actual values:
			cb.SetItemDataPtr(i, pDims);

			if (pDims[0] == oldWidth && pDims[1] == oldHeight)
				selIndex = i;

			if (pDims[0] == maxWidth && pDims[1] == maxHeight)
				have_max = true;
		}
	}

	static int extra_entry[2] = {0, 0};
	if (!have_max)
	{
		// There is no entry in the list for the full-frame size of
		// this camera. Add it
		extra_entry[0] = maxWidth;
		extra_entry[1] = maxHeight;

		s.Format(_T("%d x %d"), maxWidth, maxHeight);
		int i = cb.AddString(s);

		// Point the ItemData of the entry to the static array.
		cb.SetItemDataPtr(i, &extra_entry[0]);

		if (maxWidth == oldWidth && maxHeight == oldHeight)
			selIndex = i;

	}

	if (selIndex != -1)
		cb.SetCurSel(selIndex);
}


// Used to populate the "Standard Sizes" combo box.
static const std::pair<int,TCHAR const*> decimationModes[] = 
{
	std::make_pair<int,TCHAR const*>( PIXEL_ADDRESSING_MODE_DECIMATE,		(TCHAR const*)("Decimate") ),
	std::make_pair<int,TCHAR const*>( PIXEL_ADDRESSING_MODE_AVERAGE,		(TCHAR const*)("Average") ),
	std::make_pair<int,TCHAR const*>( PIXEL_ADDRESSING_MODE_BIN,			(TCHAR const*)("Bin") ),

	std::make_pair<int,TCHAR const*>( -1, (TCHAR const*)"" ), // Important - termination marker.
};


/**
* Function: SetDecimationModeEntries
* Purpose:  
*/
void SetDecimationModeEntries(CComboBox& cb, std::vector<int> const& supportedValues)
{
	ClearComboBox(cb);

	for (std::vector<int>::const_iterator iter = supportedValues.begin();
			iter != supportedValues.end(); iter++)
	{
		cb.SetItemData(
				cb.AddString(ApiConstAsString(CT_DECIMATION_MODE, *iter)),
				static_cast<DWORD>(*iter));
	}
}


/**
* Function: SetDecimationEntries
* Purpose:  
*/
void SetDecimationEntries(CComboBox& cb, std::vector<int> const& supportedXValues, std::vector<int> const& supportedYValues, bool asymmetricPa)
{
	DWORD numX = supportedXValues.size();
	DWORD numY = supportedYValues.size();
	DWORD sel = 1;
	
	ClearComboBox(cb);

	// Step 1
	//      Add the first item for no pixel addressing
	CString str(_T("None"));
	cb.SetItemData(
			cb.AddString(str),
			sel++);

	// Step 2
	//      Add the other entries (be they asymetric, or not
    for (std::vector<int>::const_iterator xPa = supportedXValues.begin();
		 xPa != supportedXValues.end(); xPa++)
    {
        for (std::vector<int>::const_iterator yPa = supportedYValues.begin();
		     yPa != supportedYValues.end(); yPa++)
		{
		    if (*xPa == 1 && *yPa == 1)
    		    continue; // We've already added the "None" entry.
    		    
    		if (!asymmetricPa && *xPa != *yPa)
    		    continue;  // this camera only supports symmetric pixel addressing

    		str.Format(_T("%d by %d"),*xPa, *yPa);
	        cb.SetItemData(
			        cb.AddString(str),
			        sel++);
        }
    }
}


/**
* Function: SetComboBoxEntries
* Purpose:  Helper function used by the various SetXXXEntries functions in this file.
*/
static void SetComboBoxEntries(CComboBox& cb, int nItems, TCHAR const* pNames[], int const* pData, bool append=false)
{
	if (!append)
		ClearComboBox(cb);

	for ( int i = 0; i < nItems; i++ )
	{
		int idx = cb.AddString(pNames[i]);
		if (idx >= 0)
			cb.SetItemData(idx, static_cast<DWORD>(pData[i]));
	}
}


/******************************************************************************
* Helper functions for working with File Formats & File Extensions
******************************************************************************/

/**
* Function: PathGetExtension
* Purpose:  Extract the extension from a path and return it.
*           Eg: given "C:\temp\foo.bmp", we return "bmp".
*           Eg: given "C:\temp\", we return "".
*/
CString PathGetExtension(TCHAR const* path)
{
	CString s(path);
	int dotpos = s.ReverseFind('.');
	if (dotpos == -1) // No extension specified
		return CString(_T(""));
	return s.Right(s.GetLength() - dotpos - 1);
}

/**
* Function: GetExtension
* Purpose:  Return the file extension at the indicated index in the specified
*           array of strings. Each entry in exts may specify multiple
*           extensions that are associated with the same file type - this
*           function returns the first one.
*           Eg: if exts[index] is "jpg|jpeg", we return "jpg".
*/
CString GetExtension(int index, TCHAR const* exts[])
{
	CString ext = exts[index];
	int pos;
	if (-1 != (pos=ext.Find(_T("|"))))
		//ext.Truncate(pos); // vc7
		ext = ext.Left(pos); // vc6
	return ext;
}

/**
* Function: GetExtensionIndex
* Purpose:  Return the index of the item in formatExts that has the given
*           string as one of its associated extensions.
*/
int GetExtensionIndex(CString& ext, TCHAR const* formatExts[], int nExts)
{
	CString delim = _T("|");
	//CString lcExt = ext.MakeLower(); // vc7
	CString lcExt = ext; // vc6
	lcExt.MakeLower();   // vc6
	for (int i = 0; i < nExts; i++)
	{
		CString exts = delim + formatExts[i] + delim;
		if (exts.Find(delim + lcExt + delim) != -1)
		{
			return i;
		}
	}
	return -1;
}

/**
* Function: SetFormatFromFilename
* Purpose:  Select the item in the combo box that is associated with the file
*           extension that appears in the window-text of pathEdit.
*/
void SetFormatFromFilename(CWnd& pathEdit, CComboBox& formatCombo, TCHAR const* formatExts[], int nExts)
{
	CString ext = PathGetExtension(WindowTextAsString(pathEdit));
	if (ext.GetLength() == 0)
		return;
	int i;
	if ((i = GetExtensionIndex(ext, formatExts, nExts)) != -1)
	{
		formatCombo.SetCurSel(i);
	}
	// else: Didn't find the extension - don't change anything.
}

/**
* Function: SetFileExtensionFromFormat
* Purpose:  Sort of the inverse of the previous function: change the window
*           text of pathEdit so that it has a file extension that is
*           associated with the selected item in the combo box.
*/
void SetFileExtensionFromFormat(CWnd& pathEdit, CComboBox& formatCombo, TCHAR const* formatExts[], int nExts)
{
	CString s = WindowTextAsString(pathEdit);
	CString ext = PathGetExtension(s);
	if (GetExtensionIndex(ext, formatExts, nExts) == formatCombo.GetCurSel())
		return; // File extension is already OK.

	// Get the file extension to change to:
	if (ext.GetLength() == 0) // No extension at all
		s += _T(".");
	CString newExt = GetExtension(formatCombo.GetCurSel(), formatExts);
	s = s.Left(s.GetLength() - ext.GetLength()) + newExt;
	pathEdit.SetWindowText(s);
}

/******************************************************************************
* Clip File Formats
******************************************************************************/

int const clipFormats[] = 
{
	CLIP_FORMAT_AVI,
    CLIP_FORMAT_MP4
};

int const nClipFormats = sizeof(clipFormats)/sizeof(int);

TCHAR const* clipFormatNames[] = 
{
	_T("AVI"),
	_T("MP4"),
};

void SetClipFormatEntries(CComboBox& cb)
{
	SetComboBoxEntries(cb, nClipFormats, clipFormatNames, clipFormats);
}

TCHAR const* clipFormatFilterString = 
	_T("AVI Files (*.avi)|*.avi|")
	_T("MP4 Files (*.mp4)|*.mp4|")
	_T("All Files (*.*)|*.*||");

TCHAR const* clipFormatExtensions[] =
{
	_T("avi"),
	_T("mp4"),
};

int GetClipFormatExtensionIndex(CString& ext)
{
	return GetExtensionIndex(ext, clipFormatExtensions, nClipFormats);
}

CString GetClipFormatExtension(int index)
{
	return GetExtension(index, clipFormatExtensions);
}

void SetClipFormatFromFilename(CWnd& pathEdit, CComboBox& formatCombo)
{
	SetFormatFromFilename(pathEdit, formatCombo, clipFormatExtensions, nClipFormats);
}

void SetFileExtensionFromClipFormat(CWnd& pathEdit, CComboBox& formatCombo)
{
	SetFileExtensionFromFormat(pathEdit, formatCombo, clipFormatExtensions, nClipFormats);
}

int const clipEncodings[] = 
{
	CLIP_ENCODING_PDS,   // No encoding(or compression)
    CLIP_ENCODING_H264,
    //CLIP_ENCODING_H265, //Bugzilla.1943
};

int const nClipEncodings = sizeof(clipEncodings)/sizeof(int);

TCHAR const* clipEncodingNames[] = 
{
	_T("None"),
	_T("H.264"),
	//_T("H.265"), //Bugzilla.1943 
};

void SetClipEncodingEntries(CComboBox& cb)
{
	SetComboBoxEntries(cb, nClipEncodings, clipEncodingNames, clipEncodings);
}

TCHAR const* clipEncodingExtensions[] =
{
	_T("pds"),
	_T("h264"),
	//_T("h265"), Bugzilla.1943
};

int GetClipEncodingExtensionIndex(CString& ext)
{
	return GetExtensionIndex(ext, clipEncodingExtensions, nClipEncodings);
}

CString GetClipEncodingExtension(int index)
{
	return GetExtension(index, clipEncodingExtensions);
}

void SetClipEncodingFromFilename(CWnd& pathEdit, CComboBox& formatCombo)
{
	SetFormatFromFilename(pathEdit, formatCombo, clipEncodingExtensions, nClipEncodings);
}

void SetFileExtensionFromClipEncoding(CWnd& pathEdit, CComboBox& formatCombo)
{
	SetFileExtensionFromFormat(pathEdit, formatCombo, clipEncodingExtensions, nClipFormats);
}


/******************************************************************************
* Image File Formats
******************************************************************************/

int const imageFormats[] = 
{
	IMAGE_FORMAT_BMP,
	IMAGE_FORMAT_JPEG,
    IMAGE_FORMAT_PNG,
	IMAGE_FORMAT_PSD,
	IMAGE_FORMAT_TIFF,
	IMAGE_FORMAT_RAW,
};

int const nImageFormats = sizeof(imageFormats)/sizeof(int);

TCHAR const* imageFormatNames[] = 
{
	_T("Bitmap"),
	_T("JPEG"),
    _T("PNG"),
	_T("PSD"),
	_T("Tiff"),
	_T("Raw Data"),
};

void SetImageFormatEntries(CComboBox& cb)
{
	SetComboBoxEntries(cb, nImageFormats, imageFormatNames, imageFormats);
}

TCHAR const* imageFormatFilterString = 
	_T("Bitmap Files (*.bmp)|*.bmp|")
	_T("Jpeg Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|")
    _T("PNG Files (*.png)|*.png|")
	_T("Photoshop Files (*.psd)|*.psd|")
	_T("TIFF Files (*.tif;*.tiff)|*.tif;*.tiff|")
	_T("Raw Data Files (*.bin)|*.bin|")
	_T("All Files (*.*)|*.*||");

TCHAR const* imageFormatExtensions[] =
{
	_T("bmp"),
	_T("jpg|jpeg"),
    _T("png"),
	_T("psd"),
	_T("tif|tiff"),
	_T("bin"),
};

int GetImageFormatExtensionIndex(CString& ext)
{
	return GetExtensionIndex(ext, imageFormatExtensions, nImageFormats);
}

CString GetImageFormatExtension(int index)
{
	return GetExtension(index, imageFormatExtensions);
}

void SetImageFormatFromFilename(CWnd& pathEdit, CComboBox& formatCombo)
{
	SetFormatFromFilename(pathEdit, formatCombo, imageFormatExtensions, nImageFormats);
}

void SetFileExtensionFromImageFormat(CWnd& pathEdit, CComboBox& formatCombo)
{
	SetFileExtensionFromFormat(pathEdit, formatCombo, imageFormatExtensions, nImageFormats);
}


/******************************************************************************
* Combined Image and Video File Formats
******************************************************************************/
int const combinedFormats[] = 
{
	IMAGE_FORMAT_BMP,
	IMAGE_FORMAT_JPEG,
    IMAGE_FORMAT_PNG,
	IMAGE_FORMAT_PSD,
	IMAGE_FORMAT_TIFF,
	IMAGE_FORMAT_RAW,

	0x80 + CLIP_FORMAT_AVI,
    0x80 + CLIP_FORMAT_MP4

};
int const nCombinedFormats = sizeof(combinedFormats)/sizeof(int);

TCHAR const* combinedFormatNames[] = 
{
	_T("Bitmap"),
	_T("JPEG"),
    _T("PNG"),
	_T("PSD"),
	_T("Tiff"),
	_T("Raw Data"),
	_T("AVI"),
	_T("MP4"),
};

void SetCombinedFormatEntries(CComboBox& cb)
{
	SetComboBoxEntries(cb, nCombinedFormats, combinedFormatNames, combinedFormats);
}

TCHAR const* combinedFormatFilterString = 
	_T("Bitmap Files (*.bmp)|*.bmp|")
	_T("Jpeg Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|")
    _T("PNG File (*.png)|*.png|")
	_T("Photoshop Files (*.psd)|*.psd|")
	_T("TIFF Files (*.tif;*.tiff)|*.tif;*.tiff|")
	_T("Raw Data Files (*.bin)|*.bin|")
	_T("AVI Files (*.avi)|*.avi|")
	_T("MP4 Files (*.mp4)|*.mp4|")
	_T("All Files (*.*)|*.*||");

TCHAR const* combinedFormatExtensions[] =
{
	_T("bmp"),
	_T("jpg|jpeg"),
    _T("png"),
	_T("psd"),
	_T("tif|tiff"),
	_T("bin"),
	_T("avi"),
	_T("mp4"),
};

int GetCombinedFormatExtensionIndex(CString& ext)
{
	return GetExtensionIndex(ext, imageFormatExtensions, nCombinedFormats);
}

CString GetCombinedFormatExtension(int index)
{
	return GetExtension(index, combinedFormatExtensions);
}

void SetCombinedFormatFromFilename(CWnd& pathEdit, CComboBox& formatCombo)
{
	SetFormatFromFilename(pathEdit, formatCombo, combinedFormatExtensions, nCombinedFormats);
}

void SetFileExtensionFromCombinedFormat(CWnd& pathEdit, CComboBox& formatCombo)
{
	SetFileExtensionFromFormat(pathEdit, formatCombo, combinedFormatExtensions, nCombinedFormats);
}



/******************************************************************************
* Pds (PixeLINK Data Stream) file stuff:
******************************************************************************/

TCHAR const* pdsFormatFilterString = 
    _T("H264 Files (*.h264)|*.h264|")
    //_T("H265 Files (*.h265)|*.h265|") //Bugzilla.1943
	_T("PDS Files (*.pds)|*.pds|")
	_T("All Files (*.*)|*.*||");


/******************************************************************************
* Pixel Formats
******************************************************************************/

// Values used to populate the Pixel Formats combo box.
int const pixelFormats[] = 
{
	PIXEL_FORMAT_MONO8,
	PIXEL_FORMAT_MONO16,
	PIXEL_FORMAT_YUV422,
	PIXEL_FORMAT_BAYER8,
	PIXEL_FORMAT_BAYER16,
	PIXEL_FORMAT_RGB24,
	PIXEL_FORMAT_RGB48,
	PIXEL_FORMAT_MONO12_PACKED,
	PIXEL_FORMAT_BAYER12_PACKED,
	PIXEL_FORMAT_MONO12_PACKED_MSFIRST,
	PIXEL_FORMAT_BAYER12_PACKED_MSFIRST,
	PIXEL_FORMAT_MONO10_PACKED_MSFIRST,
	PIXEL_FORMAT_BAYER10_PACKED_MSFIRST,
    PIXEL_FORMAT_STOKES4_12,
    PIXEL_FORMAT_POLAR4_12,
    PIXEL_FORMAT_POLAR_RAW4_12,
    PIXEL_FORMAT_HSV4_12,
    PIXEL_FORMAT_RGBA,
    PIXEL_FORMAT_BGRA,
    PIXEL_FORMAT_ARGB,
    PIXEL_FORMAT_ARGB,
};

int const nPixelFormats = sizeof(pixelFormats)/sizeof(int);

TCHAR const* pixelFormatNames[] =
{
	_T("8-Bit Mono"),
	_T("16-Bit Mono"),
	_T("YUV-422 Color"),
	_T("8-Bit Bayer"),
	_T("16-Bit Bayer"),
	_T("24-Bit RGB"),
	_T("48-Bit RGB"),
	_T("12-Bit Mono Packed"),
	_T("12-Bit Bayer Packed"),
	_T("12-Bit Mono Packed g"),
	_T("12-Bit Bayer Packed g"),
	_T("10-Bit Mono Packed g"),
	_T("10-Bit Bayer Packed g"),
    _T("12-Bit Stokes (4)"),
    _T("12-Bit Polar (4)"),
    _T("12-Bit HSV (4)"),
    _T("32-Bit RGBA"),
    _T("32-Bit BGRA"),
    _T("32-Bit ARGB"),
    _T("32-Bit ARGB"),
};


/**
* Function: GetPixelFormatSize
* Purpose:  Return the number of bytes per pixel for a pixel format.
*/
float GetPixelFormatSize(int pixelFormat)
{
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
	return -1.0f;
}


/******************************************************************************
* GPIO Modes
******************************************************************************/

int const gpioModes[] = 
{
	GPIO_MODE_STROBE,
	GPIO_MODE_NORMAL,
	GPIO_MODE_PULSE,
	GPIO_MODE_BUSY,
	GPIO_MODE_FLASH,
    GPIO_MODE_INPUT,
    GPIO_MODE_ACTION_STROBE,
    GPIO_MODE_ACTION_NORMAL,
    GPIO_MODE_ACTION_PULSE
};

int const nGpioModes = sizeof(gpioModes)/sizeof(int);

TCHAR const* gpioModeNames[] =
{
	_T("Strobe"),
	_T("Normal"),
	_T("Pulse"),
	_T("Busy"),
	_T("Flash"),
	_T("Input"),
	_T("ActionStrobe"),
	_T("ActionNormal"),
	_T("ActionPulse"),
};

TCHAR const* gpioModeDescriptions[] =
{
	// Strobe
	_T("The GPO is set after a trigger occurs. The GPO pulse occurs X (parameter 1) ")
		_T("seconds from the trigger and is Y (parameter 2) seconds in duration."),
	// Normal
	_T("The GPO is set to either low or high, depending on the value of Polarity."),
	// Pulse
	_T("The GPO is pulsed whenever it is turned on. The GPO outputs X (parameter 1) ")
		_T("pulses of Y (parameter 2) seconds in length separated by Z (parameter 3) seconds."),
	// Busy
	_T("The GPO is set whenever the camera is unable to respond to a trigger. ")
		_T("This is only useful when using Trigger Type Hardware or Software, for all other ")
		_T("modes it will always be set."),
	// Flash
	_T("The GPO signal is set once the sensor has been reset and starts integrating, ")
		_T("and will be deactivated at the end of the exposure time as readout of the ")
		_T("array commences."), 
    // Input
    _T("Function as a General Purpose Input.  The value of the input line is returned ")
    _T("as parameter 2.  Note that only GPIO #1 can be configured as a GPI."),
	// ActionStrobe
	_T("The GPO is set after a GPO action occurs. The GPO pulse occurs X (parameter 1) ")
		_T("seconds from the action and is Y (parameter 2) seconds in duration."),
	// ActionNormal
	_T("The GPO is set to either low or high on action, depending on the value of Polarity."),
	// ActionPulse
	_T("The GPO is pulsed on action. The GPO outputs X (parameter 1) ")
		_T("pulses of Y (parameter 2) seconds in length separated by Z (parameter 3) seconds."),
};

TCHAR const* gpioModeParameterNames[nGpioModes][6] =
{
	// Param 1			// Units 1          // Param 2			// Units 2          // Param 3      // Units 3
	{_T("Delay"),		_T("seconds"),		_T("Duration"),     _T("seconds"),	    _T("n/a"),      _T(" ")},    		// Strobe
	{_T("n/a"), 		_T(" "),     		_T("n/a"),          _T(" "),     	    _T("n/a"),      _T(" ")}, 			// Normal
	{_T("Number"),		_T(" "),     		_T("Duration"),     _T("seconds"),	    _T("Interval"), _T("seconds")},		// Pulse
	{_T("n/a"), 		_T(" "),     		_T("n/a"),          _T(" "),     	    _T("n/a"),      _T(" ")}, 			// Busy
	{_T("n/a"), 		_T(" "),     		_T("n/a"),          _T(" "),     	    _T("n/a"),      _T(" ")}, 			// Flash
	{_T("Status"), 		_T(" "),     		_T("n/a"),          _T(" "),     	    _T("n/a"),      _T(" ")}, 			// Input
	{_T("Delay"),		_T("seconds"),		_T("Duration"),     _T("seconds"),	    _T("n/a"),      _T(" ")},    		// ActionStrobe
	{_T("n/a"), 		_T(" "),     		_T("n/a"),          _T(" "),     	    _T("n/a"),      _T(" ")}, 			// ActionNormal
	{_T("Number"),		_T(" "),     		_T("Duration"),     _T("seconds"),	    _T("Interval"), _T("seconds")},		// ActionPulse
};

void SetGpioModeEntries(CComboBox& cb, std::vector<int> const& supportedModes)
{
	//SetComboBoxEntries(cb, nGpioModes, gpioModeNames, gpioModes);
	ClearComboBox(cb);

	for ( int i = 0; i < nGpioModes; i++ )
	{
		if (contains(supportedModes, gpioModes[i]))
		{
			cb.SetItemData(
					cb.AddString(gpioModeNames[i]),
					gpioModes[i]);
		}
	}
}

static int const gpoModeNumParams[] = 
{
	2,	// Strobe
	0,	// Normal
	3,	// Pulse
	0,	// Busy
	0,	// Flash
    1,  // Input
	2,	// ActionStrobe
	0,	// ActionNormal
	3,	// ActionPulse
};

int GpioModeNumParams(U32 mode)
{
	ASSERT(mode < nGpioModes);
	return gpoModeNumParams[mode];
}

TCHAR const** GetGpoParameterNames(int mode)
{
	return gpioModeParameterNames[mode];
}

CString GetGpoDescription(CComboBox const& modeCombo)
{
	int mode = ItemData<int>(modeCombo);
	for (int i = 0; i < nGpioModes; i++)
	{
		if (gpioModes[i] == mode)
			return gpioModeDescriptions[i];
	}
	return _T("");
}



/******************************************************************************
* Action Commands
******************************************************************************/

int const actionCommands[] = 
{
	ACTION_FRAME_TRIGGER,
	ACTION_GPO1,
	ACTION_GPO2,
	ACTION_GPO3,
	ACTION_GPO4,
};

int const nActionCommands = sizeof(actionCommands)/sizeof(int);

TCHAR const* actionCommandNames[] =
{
	_T("Frame"),
	_T("GPO1"),
	_T("GPO2"),
	_T("GPO3"),
	_T("GPO4"),
};

void SetActionCommandEntries(CComboBox& cb, std::vector<int> const& supportedActions)
{
	ClearComboBox(cb);

	for ( int i = 0; i < nActionCommands; i++ )
	{
		if (contains(supportedActions, actionCommands[i]))
		{
			cb.SetItemData(
					cb.AddString(actionCommandNames[i]),
					actionCommands[i]);
		}
	}
}

/******************************************************************************
* Events
******************************************************************************/

int const events[] = 
{
    EVENT_HW_TRIGGER_RISING_EDGE,
    EVENT_HW_TRIGGER_FALLING_EDGE,
    EVENT_GPI_RISING_EDGE,
    EVENT_GPI_FALLING_EDGE,
    EVENT_HW_TRIGGER_MISSED,
    EVENT_SYNCHRONIZED_TO_MASTER,
    EVENT_UNSYNCHRONIZED_FROM_MASTER,
    EVENT_FRAMES_SKIPPED,
};

int const nEvents = sizeof(events)/sizeof(int);

TCHAR const* eventNames[] =
{
	_T("Hardware Trigger Rising Edge"),
	_T("Hardware Trigger Falling Edge"),
	_T("GPI Rising Edge"),
	_T("GPIFalling Edge"),
	_T("Hardware Trigger Missed"),
	_T("PTP Synchronized to Master Clock"),
	_T("PTP Lost Synchronization from Master Clock"),
    _T("Frames Skipped"),
};

TCHAR const* GetEventNames(int eventId)
{
    // Bugzilla.1980.
    for (int i = 0; i < nEvents; i++)
    {
        if (eventId == events[i]) return eventNames[i];
    }
    return _T("Unknown Event");
}


/******************************************************************************
* Trigger Types
******************************************************************************/

int const triggerTypes[] = 
{
	TRIGGER_TYPE_FREE_RUNNING,
	TRIGGER_TYPE_SOFTWARE,
	TRIGGER_TYPE_HARDWARE,
    TRIGGER_TYPE_ACTION
};

int const nTriggerTypes = sizeof(triggerTypes)/sizeof(int);

TCHAR const* triggerTypeNames[] =
{
	_T("Free Running"),
	_T("Software"),
	_T("Hardware"),
    _T("Action")
};

void SetTriggerTypeEntries(CComboBox& cb, std::vector<int> const& supportedTypes)
{
	ClearComboBox(cb);

	for ( int i = 0; i < nTriggerTypes; i++ )
	{
		if (contains(supportedTypes, triggerTypes[i]))
		{
			cb.SetItemData(
					cb.AddString(triggerTypeNames[i]),
					triggerTypes[i]);
		}
	}
}


/******************************************************************************
* Trigger Modes
******************************************************************************/

TriggerModeDescriptor triggerModeDescriptors[] = 
{
	// Mode 0
	{
		TRIGGER_MODE_0,
		_T("Trigger Mode 0"),
		_T("Start integration at external trigger's leading edge or action. ")
			_T("Integration time is defined by FEATURE_SHUTTER. No parameters.  ")
			_T("Check Controlled Lighting if the GPO is being used to control ")
			_T("lighting via an external mechanical shutter or light source. "),
		false, // takes param?
		_T("n/a"), // param name
		_T(""), // param units
		//	Free	SW		HW
		{	true,	true,	true	}
	},

	// Mode 1
	{
		TRIGGER_MODE_1,
		_T("Trigger Mode 1"),
		_T("Start integration at external trigger's leading edge and end at ")
			_T("trigger's trailing edge.  No parameters.  ")
			_T("Check Controlled Lighting if the GPO is being used to control ")
			_T("lighting via an external mechanical shutter or light source.  "),
		false, // takes param?
		_T("n/a"), // param name
		_T(""), // param units
		//	Free	SW		HW
		{	true,	true,	true	}
	},

	// Mode 2
	{
		TRIGGER_MODE_2,
		_T("Trigger Mode 2"),
		_T("Start integration at the first external trigger's leading edge, ")
			_T("and end at the N-th (parameter) external trigger's leading edge.  ")
			_T("Parameter is required and must be two or more (N >= 2).  ")
			_T("Check Controlled Lighting if the GPO is being used to control ")
			_T("lighting via an external mechanical shutter or light source. "),
		true, // takes param?
		_T("Number"), // param name
		_T(""), // param units
		//	Free	SW		HW
		{	true,	true,	true	}
	},

	// Mode 3
	{
		TRIGGER_MODE_3,
		_T("Trigger Mode 3"),
		_T("Camera will issue triggers internally at intervals of N (parameter) ")
			_T("times Tf, where Tf is the cycle time of the fastest frame rate. ")
			_T("Integration time is defined by FEATURE_SHUTTER. Parameter is required ")
			_T("and must be one or more (N >= 1).  ")
			_T("Check Controlled Lighting if the GPO is being used to control ")
			_T("lighting via an external mechanical shutter or light source.  "),
		true, // takes param?
		_T("Number"), // param name
		_T("(>=1)"), // param units
		//	Free	SW		HW
		{	true,	true,	true	}
	},

	// Mode 4
	{
		TRIGGER_MODE_4,
		_T("Trigger Mode 4"),
		_T("The camera will capture N (parameter) frames after a trigger at the ")
			_T("current integration time and frame rate. Paramerter is required and ")
			_T("must be 1 or more (N >= 1).  ")
			_T("Check Controlled Lighting if the GPO is being used to control ")
			_T("lighting via an external mechanical shutter or light source.  "),
		true, // takes param?
		_T("Number"), // param name
		_T("(>=1)"), // param units
		//	Free	SW		HW
		{	true,	true,	true	}
	},

	// Mode 5
	{
		TRIGGER_MODE_5,
		_T("Trigger Mode 5"),
		_T("The camera will capture frames after a trigger for N (parameter) ")
			_T("seconds at the current integration time and frame rate. Parameter is ")
			_T("required and may be any number > 0.0 ")
			_T("Check Controlled Lighting if the GPO is being used to control ")
			_T("lighting via an external mechanical shutter or light source.  "),
		true, // takes param?
		_T("Duration"), // param name
		_T("seconds"), // param units
		//	Free	SW		HW
		{	true,	true,	true	}
	},

	// XXX
	{ 6, _T("Trigger Mode 6"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },
	{ 7, _T("Trigger Mode 7"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },
	{ 8, _T("Trigger Mode 8"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },
	{ 9, _T("Trigger Mode 9"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },
	{ 10, _T("Trigger Mode 10"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },
	{ 11, _T("Trigger Mode 11"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },
	{ 12, _T("Trigger Mode 12"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },
	{ 13, _T("Trigger Mode 13"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },

	// Mode 14
	{
		14,
		_T("Trigger Mode 14"),
		_T("The camera will capture N (parameter) frames after a trigger/action at the ")
			_T("current integration time and frame rate. Paramerter is required and ")
			_T("must be 1 or more (N >= 1).  ")
			_T("Check Controlled Lighting if the GPO is being used to control ")
			_T("lighting via an external mechanical shutter or light source.  "),
		true, // takes param?
		_T("Number"), // param name
		_T("(>=1)"), // param units
		//	Free	SW		HW
		{	true,	true,	true	}
	},

	{ 15, _T("Trigger Mode 15"), _T("???"), false, _T("???"), _T("???"), {true,true,true} },
};

int const nTriggerModes = sizeof(triggerModeDescriptors)/sizeof(triggerModeDescriptors[0]);

bool TriggerModeTakesParameter(U32 mode)
{
	return (mode >= nTriggerModes ? false : triggerModeDescriptors[mode].takesParam);
}

void SetTriggerModeEntries(CComboBox& cb, std::vector<int> const& supportedModes)
{
	ClearComboBox(cb);

	for ( int i = 0; i < nTriggerModes; i++ )
	{
		if (contains(supportedModes, triggerModeDescriptors[i].mode))
		{
			cb.SetItemData(
					cb.AddString(triggerModeDescriptors[i].name),
					triggerModeDescriptors[i].mode);
		}
	}
}

bool TriggerModeTypeComboIsValid(U32 mode, U32 type)
{
	ASSERT(mode < nTriggerModes && type < nTriggerTypes);
	return triggerModeDescriptors[mode].validWithType[type];
}

TriggerModeDescriptor* GetTriggerDescriptor(int mode)
{
	return (mode >= nTriggerModes ? NULL : &triggerModeDescriptors[mode]);
}


/******************************************************************************
* PTP States (Status)
******************************************************************************/

int const ptpStates[] = 
{
    FEATURE_PTP_STATUS_DISABLED, // 0 is unused, treat it the same as Disabled
    FEATURE_PTP_STATUS_INITIALIZING,
    FEATURE_PTP_STATUS_FAULTY,
    FEATURE_PTP_STATUS_DISABLED,
    FEATURE_PTP_STATUS_LISTENING,
    FEATURE_PTP_STATUS_PREMASTER,
    FEATURE_PTP_STATUS_MASTER,
    FEATURE_PTP_STATUS_PASSIVE,
    FEATURE_PTP_STATUS_UNCALIBRATED,
    FEATURE_PTP_STATUS_SLAVE
};

int const nPtStates = sizeof(ptpStates)/sizeof(int);

TCHAR const* ptpStateNames[] =
{
	_T("Disabled"),     // 0 is unused, treat it the same as Disabled
	_T("Initializing"),
	_T("Faulty"),
	_T("Disabled"),
    _T("Listening"),
	_T("PreMaster"),
	_T("Master"),
	_T("Passive"),
    _T("Uncalibrated"),
    _T("Slave")
};

/**
* Function: GetPtpState
* Purpose:  
*/
TCHAR const* GetPtpState(U32 ptpStatus)
{
    if (ptpStatus < nPtStates) return ptpStateNames[ptpStatus];
    else                       return ptpStateNames[FEATURE_PTP_STATUS_DISABLED];
}


/******************************************************************************
* Whatever (generally useful functions that don't belong anywhere else)
******************************************************************************/

/**
* Function: SetPolarityEntries
* Purpose:  Helper function to add the entries to the "Polarity" combo boxes.
*/
void SetPolarityEntries(CComboBox& cb, std::pair<float,float> minmax)
{
	ClearComboBox(cb);

	if (POLARITY_POSITIVE >= minmax.first && POLARITY_POSITIVE <= minmax.second)
	{
		cb.SetItemData(
			cb.AddString(_T("Positive")), // or "Active High" ?
			POLARITY_POSITIVE);
	}
	
	if (POLARITY_NEGATIVE >= minmax.first && POLARITY_NEGATIVE <= minmax.second)
	{
		cb.SetItemData(
			cb.AddString(_T("Negative")), // or "Active Low" ?
			POLARITY_NEGATIVE);
	}
}


/**
* Function: BrowseForFile
* Purpose:  Present the user with a File->SaveAs dialog, and put the resulting
*           path into the window text of the CWnd passed as a parameter.
*           Return true if a path was selected, false if the user clicked "Cancel".
*/
bool BrowseForFile(CWnd& wnd,
				   TCHAR const* filterString /*=NULL*/,
				   TCHAR const* defaultExtension /*=NULL*/)
{
	CString defaultFilename(_T("*.*"));
	if (defaultExtension != NULL)
		defaultFilename.Format(_T("*.%s"), defaultExtension);

	CFileDialog fd(
					FALSE,				// FALSE for "Save As" dialog, TRUE for "Open" dialog
					defaultExtension,	// lpsz Default extension
					defaultFilename,	// lpsz Default filename
					OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
					filterString,		// lpsz Filters
					NULL);				// CWnd* parent

	if (defaultExtension != NULL)
	{
		fd.m_ofn.nFilterIndex = 1 + GetImageFormatExtensionIndex(CString(defaultExtension)); // vc6
	}

	if (fd.DoModal() == IDOK)
	{
		CString s = fd.GetPathName();
		wnd.SetWindowText(s);
		return true;
	}
	return false;
}


/**
* Function: EnsureValidPath
* Purpose:  Make sure that pathEdit contains a valid path to a file when
*           this function returns. If it already contains a valid path, then
*           this function does nothing. If it does not already contain a valid
*           path, the user is presented with a "File Save" dialog, and the
*           path to the selected file is set as the window-text of pathEdit.
*           Returns true if a valid path is obtained, and false if the user
*           clicks "Cancel" on the file dialog.
*/
bool EnsureValidPath(CWnd& pathEdit,
					 TCHAR const* filterString /*=NULL*/,
					 TCHAR const* defaultExtension /*=NULL*/)
{
	CString path = WindowTextAsString(pathEdit);

	if(PathFileExists(path) && !PathIsDirectory(path))
	{
		// Path is to an existing file.
		return true;
	}

	// Find out if path is to a new file name in an existing directory:
	TCHAR temp[MAX_PATH];
	_tcscpy_s(temp, MAX_PATH, path);
	if (!PathIsDirectory(path))
		PathRemoveFileSpec(temp); // eg: trim "C:\temp\foo.bar" down to "C:\temp"
	if (PathIsDirectory(temp) && (path.GetLength() >= _tcslen(temp) + 2))
	{
		// path == (valid directory) + "\" + at least one more character...
		return true;
	}

	// pathEdit does not contain a valid path - show File Save dialog.
	if (BrowseForFile(pathEdit, filterString, defaultExtension))
	{
		return true;
	}
	return false;
}


/**
* Function: EnsureValidImagePath
* Purpose:  Make sure that pathEdit contains a valid path to save an image
*           file to, opening a File Save dialog if neccesary.
*           The combo box specifies the file format, which determines the
*           default file extension to be used in the File Save dialog.
*           Returns false if the user clicks "Cancel".
*/
bool EnsureValidImagePath(CWnd& pathEdit, CComboBox& formatCombo)
{
	return EnsureValidPath(pathEdit,
							imageFormatFilterString,
							GetImageFormatExtension(formatCombo.GetCurSel()));
}


/**
* Function: EnsureValidClipPath
* Purpose:  Make sure that pathEdit contains a valid path to save a clip
*           file to, opening a File Save dialog if neccesary.
*           The combo box specifies the file format, which determines the
*           default file extension to be used in the File Save dialog.
*           Returns false if the user clicks "Cancel".
*/
bool EnsureValidClipPath(CWnd& pathEdit, CComboBox& formatCombo)
{
	return EnsureValidPath(pathEdit,
							clipFormatFilterString,
							GetClipFormatExtension(formatCombo.GetCurSel()));
}


/**
* Function: EnsureValidPdsPath
* Purpose:  Make sure that pathEdit contains a valid path to save a pds
*           file to, opening a File Save dialog if neccesary.
*           Only the "pds" file extension is allowed.
*           Returns false if the user clicks "Cancel".
*/
bool EnsureValidPdsPath(CWnd& pathEdit)
{
	return EnsureValidPath(pathEdit,
							pdsFormatFilterString,
							_T("pds"));
}


/**
* Function: SetFloatText
* Purpose:  Format a float as a string and put it in the specified window.
*/
void SetFloatText(CWnd& wnd, float val, int dp /*=2*/)
{
	CString s;
	s.Format(_T("%.*f"), dp, val);
	//s.Format(_T("%g"), val);
	wnd.SetWindowText(s);
}

/**
* Function: SetIntText
* Purpose:  Format an int as a string and put it in the specified window.
*/
void SetIntText(CWnd& wnd, int val)
{
	CString s;
	s.Format(_T("%d"), val);
	wnd.SetWindowText(s);
}


/**
* Function: ClearComboBox
* Purpose:  Remove all entries from a combo box.
*/
void ClearComboBox(CComboBox& cb)
{
	while (cb.GetCount() > 0)
		cb.DeleteString(0);
}


/**
* Function: ScrollToEnd
* Purpose:  
*/
void ScrollToEnd(CEdit& ed)
{
	int len = ed.GetWindowTextLength();
	BOOL wasDisabled = (0 < ed.EnableWindow(TRUE));
	ed.SetSel(len, len);
	if (wasDisabled)
		ed.EnableWindow(FALSE);
}


/**
* Function: WindowTextAsString
* Purpose:  Syntactic shortcut.
*/
CString WindowTextAsString(CWnd const& wnd)
{
	CString str;
	wnd.GetWindowText(str);
	return str;
}


/**
* Function: WindowTextToInt
* Purpose:  Extracts and integer from the window text of the CWnd parameter,
*           forces the window text to be formatted properly as an integer,
*           if it is not already, and returns the extracted integer.
*           Defaults to 0 if no integer can be parsed from the window text .
*/
int WindowTextToInt(CWnd& wnd)
{
	CString s = WindowTextAsString(wnd);
	int val = 0;
	if (s.GetLength() > 0) {
		_stscanf_s(s, _T("%d"), &val, sizeof(val));
	}

	s.Format(_T("%d"), val);
	wnd.SetWindowText(s);

	return val;
}


/**
* Function: WindowTextToFloat
* Purpose:  Like WindowTextToInt, but for decimal numbers.
*/
float WindowTextToFloat(CWnd& wnd)
{
	CString s = WindowTextAsString(wnd);
	float val = 0.0f;
	if (s.GetLength() > 0) {
		_stscanf_s(s, _T("%f"), &val, sizeof(val));
	}

	s.Format(_T("%g"), val);
	wnd.SetWindowText(s);

	return val;
}


/**
* Function: IncrementFileName
* Purpose:  Read the file name from the provided window, parse out whatever
*           number immediately precedes the file extension, increment the
*           number by one, and change the file name in the window to contain
*           the new number.
*           Eg: Changes "C:\image_3.bmp" to"C:\image_4.bmp".
*/
void IncrementFileName(CWnd& wnd, const CString& format)
{
	CString path = WindowTextAsString(wnd);
	int dotpos = path.ReverseFind('.');
	if (dotpos == -1)
		dotpos = path.GetLength();
	int numpos = dotpos - 1;
	while (CString(_T("0123456789")).Find(path.GetAt(numpos)) != -1)
		--numpos;
	++numpos;
	int fileno = 0;
	if (numpos < dotpos)
		fileno = _ttoi(path.GetBuffer(0) + numpos);
	++fileno;
	CString fno;
	fno.Format(format, fileno);
	path = path.Left(numpos) + fno + path.Right(path.GetLength() - dotpos);
	wnd.SetWindowText(path);
}


/**
* Function: SysOpenFile
* Purpose:  Ask the system to open the specified file. The application used
*           depends on the user's file associations. Essentially the same as
*           just entering the bare file name on the command line.
*/
void SysOpenFile(CString& path)
{
    
    HINSTANCE hInst = ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOW);
	int retCode = reinterpret_cast<int>(hInst);
	if (retCode <= 32) // Return value less than 32 indicates an error...
	{
		AfxMessageBox(_T("Failed to open the specified file. Make sure that there is a registered file association for the file type."));
	}
}


/**
* Function: ApiConstAsString
* Purpose:  Generic function for getting a string representation of a contant
*           from the PxLApi. 
*/
TCHAR const* ApiConstAsString(eApiConstType constType, U32 value, bool asCodeConst /*=false*/)
{
	switch(constType)
	{
	case CT_FEATURE:
		return asCodeConst ? FeatureIdAsString(value) : FeatureAsString(value);

	case CT_FEATURE_FLAG:
		return FeatureFlagAsString(value);

	case CT_IMAGE_FORMAT:
		switch (value)
		{
		case IMAGE_FORMAT_BMP :
			return asCodeConst ? _T("IMAGE_FORMAT_BMP") : _T("Bitmap");
		case IMAGE_FORMAT_TIFF:
			return asCodeConst ? _T("IMAGE_FORMAT_TIFF") : _T("TIFF");
		case IMAGE_FORMAT_PSD :
			return asCodeConst ? _T("IMAGE_FORMAT_PSD") : _T("PSD");
		case IMAGE_FORMAT_JPEG:
			return asCodeConst ? _T("IMAGE_FORMAT_JPEG") : _T("JPEG");
		case IMAGE_FORMAT_PNG:
			return asCodeConst ? _T("IMAGE_FORMAT_PNG") : _T("PNG");
		}
		break;

	case CT_PIXEL_FORMAT:
		switch (value)
		{
		case PIXEL_FORMAT_MONO8:
			return asCodeConst ? _T("PIXEL_FORMAT_MONO8") : _T("MONO8");
		case PIXEL_FORMAT_MONO16:
			return asCodeConst ? _T("PIXEL_FORMAT_MONO16") : _T("MONO16");
		case PIXEL_FORMAT_YUV422:
			return asCodeConst ? _T("PIXEL_FORMAT_YUV422") : _T("YUV422");
		case PIXEL_FORMAT_BAYER8_GRBG:
			return asCodeConst ? _T("PIXEL_FORMAT_BAYER8_GRBG") : _T("BAYER8");
		case PIXEL_FORMAT_BAYER8_RGGB:
			return asCodeConst ? _T("PIXEL_FORMAT_BAYER8_RGGB") : _T("BAYER8_RGGB");
		case PIXEL_FORMAT_BAYER8_GBRG:
			return asCodeConst ? _T("PIXEL_FORMAT_BAYER8_GBRG") : _T("BAYER8_GBRG");
		case PIXEL_FORMAT_BAYER8_BGGR:
			return asCodeConst ? _T("PIXEL_FORMAT_BAYER8_BGGR") : _T("BAYER8_BGGR");
		case PIXEL_FORMAT_BAYER16_GRBG:
			return asCodeConst ? _T("PIXEL_FORMAT_BAYER16_GRBG") : _T("BAYER16");
		case PIXEL_FORMAT_BAYER16_RGGB:
			return asCodeConst ? _T("PIXEL_FORMAT_BAYER16_RGGB") : _T("BAYER16_RGGB");
		case PIXEL_FORMAT_BAYER16_GBRG:
			return asCodeConst ? _T("PIXEL_FORMAT_BAYER16_GBRG") : _T("BAYER16_GBRG");
		case PIXEL_FORMAT_BAYER16_BGGR:
			return asCodeConst ? _T("PIXEL_FORMAT_BAYER16_BGGR") : _T("BAYER16_BGGR");
		case PIXEL_FORMAT_RGB24_NON_DIB:
			return asCodeConst ? _T("PIXEL_FORMAT_RGB24") : _T("RGB24");
		case PIXEL_FORMAT_BGR24_NON_DIB:
			return asCodeConst ? _T("PIXEL_FORMAT_BGR24") : _T("BGR24");
		case PIXEL_FORMAT_RGB48:
			return asCodeConst ? _T("PIXEL_FORMAT_RGB48") : _T("RGB48");
		case PIXEL_FORMAT_MONO12_PACKED:
		    return asCodeConst ? _T("PIXEL_FORMAT_MONO12_PACKED") : _T("MONO12_PACKED");
		case PIXEL_FORMAT_BAYER12_GRBG_PACKED:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER12_GRBG_PACKED") : _T("BAYER12_PACKED");
		case PIXEL_FORMAT_BAYER12_RGGB_PACKED:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER12_RGGB_PACKED") : _T("BAYER12_RGGB_PACKED");
		case PIXEL_FORMAT_BAYER12_GBRG_PACKED:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER12_GBRG_PACKED") : _T("BAYER12_GBRG_PACKED");
		case PIXEL_FORMAT_BAYER12_BGGR_PACKED:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER12_BGGR_PACKED") : _T("BAYER12_BGGR_PACKED");
		case PIXEL_FORMAT_MONO12_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_MONO12_PACKED_MSFIRST") : _T("MONO12_PACKED_MSFIRST");
		case PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER12_GRBG_PACKED_MSFIRST") : _T("BAYER12_PACKED_MSFIRST");
		case PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER12_RGGB_PACKED_MSFIRST") : _T("BAYER12_RGGB_PACKED_MSFIRST");
		case PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER12_GBRG_PACKED_MSFIRST") : _T("BAYER12_GBRG_PACKED_MSFIRST");
		case PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER12_BGGR_PACKED_MSFIRST") : _T("BAYER12_BGGR_PACKED_MSFIRST");
		case PIXEL_FORMAT_MONO10_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_MONO10_PACKED_MSFIRST") : _T("MONO10_PACKED_MSFIRST");
		case PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER10_GRBG_PACKED_MSFIRST") : _T("BAYER10_PACKED_MSFIRST");
		case PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER10_RGGB_PACKED_MSFIRST") : _T("BAYER10_RGGB_PACKED_MSFIRST");
		case PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER10_GBRG_PACKED_MSFIRST") : _T("BAYER10_GBRG_PACKED_MSFIRST");
		case PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST:
		    return asCodeConst ? _T("PIXEL_FORMAT_BAYER10_BGGR_PACKED_MSFIRST") : _T("BAYER10_BGGR_PACKED_MSFIRST");
		case PIXEL_FORMAT_STOKES4_12:
		    return asCodeConst ? _T("PIXEL_FORMAT_STOKES4_12") : _T("STOKES4_12");
		case PIXEL_FORMAT_POLAR4_12:
		    return asCodeConst ? _T("PIXEL_FORMAT_POLAR4_12") : _T("POLAR4_12");
		case PIXEL_FORMAT_POLAR_RAW4_12:
		    return asCodeConst ? _T("PIXEL_FORMAT_POLAR_RAW4_12") : _T("POLAR_RAW4_12");
		case PIXEL_FORMAT_HSV4_12:
		    return asCodeConst ? _T("PIXEL_FORMAT_HSV4_12") : _T("HSV4_12");
		case PIXEL_FORMAT_RGBA:
		    return asCodeConst ? _T("PIXEL_FORMAT_RGBA") : _T("RGBA");
		case PIXEL_FORMAT_BGRA:
		    return asCodeConst ? _T("PIXEL_FORMAT_BGRA") : _T("BGRA");
		case PIXEL_FORMAT_ARGB:
		    return asCodeConst ? _T("PIXEL_FORMAT_ARGB") : _T("ARGB");
		case PIXEL_FORMAT_ABGR:
		    return asCodeConst ? _T("PIXEL_FORMAT_ABGR") : _T("ABGR");
		}
		break;

    case CT_PIXEL_FORMAT_INTERPRETATION:
        switch (value)
        {
        case CT_HSV_AS_COLOR:
            return asCodeConst ? _T("CT_HSV_AS_COLOR") : _T("COLOR");
        case CT_HSV_AS_ANGLE:
            return asCodeConst ? _T("CT_HSV_AS_ANGLE") : _T("ANGLE");
        case CT_HSV_AS_DEGREE:
            return asCodeConst ? _T("CT_HSV_AS_DEGREE") : _T("DEGREE");
        }
        break;

	case CT_PREVIEW_STATE:
		switch (value)
		{
		case START_PREVIEW:
			return asCodeConst ? _T("START_PREVIEW") : _T("Start Preview");
		case PAUSE_PREVIEW:
			return asCodeConst ? _T("PAUSE_PREVIEW") : _T("Pause Preview");
		case STOP_PREVIEW:
			return asCodeConst ? _T("STOP_PREVIEW") : _T("Stop Preview");
		}
		break;

	case CT_STREAM_STATE:
		switch (value)
		{
		case START_STREAM:
			return asCodeConst ? _T("START_STREAM") : _T("Start Stream");
		case PAUSE_STREAM:
			return asCodeConst ? _T("PAUSE_STREAM") : _T("Pause Stream");
		case STOP_STREAM:
			return asCodeConst ? _T("STOP_STREAM") : _T("Stop Stream");
		}
		break;

	case CT_TRIGGER_TYPE:
		switch (value)
		{
		case TRIGGER_TYPE_FREE_RUNNING:
			return asCodeConst ? _T("TRIGGER_TYPE_FREE_RUNNING") : _T("Free Running");
		case TRIGGER_TYPE_SOFTWARE:
			return asCodeConst ? _T("TRIGGER_TYPE_SOFTWARE") : _T("Software");
		case TRIGGER_TYPE_HARDWARE:
			return asCodeConst ? _T("TRIGGER_TYPE_HARDWARE") : _T("Hardware");
		case TRIGGER_TYPE_ACTION:
			return asCodeConst ? _T("TRIGGER_TYPE_ACTION") : _T("Action");
		}
		break;

	case CT_TRIGGER_MODE:
		switch (value)
		{
		case TRIGGER_MODE_0:	return asCodeConst ? _T("TRIGGER_MODE_0") : _T("Mode 0");
		case TRIGGER_MODE_1:	return asCodeConst ? _T("TRIGGER_MODE_1") : _T("Mode 1");
		case TRIGGER_MODE_2:	return asCodeConst ? _T("TRIGGER_MODE_2") : _T("Mode 2");
		case TRIGGER_MODE_3:	return asCodeConst ? _T("TRIGGER_MODE_3") : _T("Mode 3");
		case TRIGGER_MODE_4:	return asCodeConst ? _T("TRIGGER_MODE_4") : _T("Mode 4");
		case TRIGGER_MODE_5:	return asCodeConst ? _T("TRIGGER_MODE_5") : _T("Mode 5");
		}
		break;

	case CT_GPIO_MODE:
		switch (value)
		{
		case GPIO_MODE_STROBE:
			return asCodeConst ? _T("GPIO_MODE_STROBE") : _T("Strobe");
		case GPIO_MODE_NORMAL:
			return asCodeConst ? _T("GPIO_MODE_NORMAL") : _T("Normal");
		case GPIO_MODE_PULSE:
			return asCodeConst ? _T("GPIO_MODE_PULSE") : _T("Pulse");
		case GPIO_MODE_BUSY:
			return asCodeConst ? _T("GPIO_MODE_BUSY") : _T("Busy");
		case GPIO_MODE_INPUT:
			return asCodeConst ? _T("GPIO_MODE_INPUT") : _T("Input");
		case GPIO_MODE_ACTION_STROBE:
			return asCodeConst ? _T("GPIO_MODE_ACTION_STROBE") : _T("ActionStrobe");
		case GPIO_MODE_ACTION_NORMAL:
			return asCodeConst ? _T("GPIO_MODE_ACTION_NORMAL") : _T("ActionNormal");
		case GPIO_MODE_ACTION_PULSE:
			return asCodeConst ? _T("GPIO_MODE_ACTION_PULSE") : _T("ActionPulse");
		}
		break;

	case CT_DESCRIPTOR_UPDATE_MODE:
		switch (value)
		{
		case PXL_UPDATE_CAMERA:
			return asCodeConst ? _T("PXL_UPDATE_CAMERA") : _T("Camera");
		case PXL_UPDATE_HOST:
			return asCodeConst ? _T("PXL_UPDATE_HOST") : _T("Host");
		}
		break;

	case CT_DECIMATION_MODE:
		switch (value)
		{
		case PIXEL_ADDRESSING_MODE_DECIMATE:
			return asCodeConst ? _T("PIXEL_ADDRESSING_MODE_DECIMATE") : _T("Decimate");
		case PIXEL_ADDRESSING_MODE_BIN:
			return asCodeConst ? _T("PIXEL_ADDRESSING_MODE_BIN") : _T("Bin");
		case PIXEL_ADDRESSING_MODE_AVERAGE:
			return asCodeConst ? _T("PIXEL_ADDRESSING_MODE_AVERAGE") : _T("Average");
		case PIXEL_ADDRESSING_MODE_RESAMPLE:
			return asCodeConst ? _T("PIXEL_ADDRESSING_MODE_RESAMPLE") : _T("Resample");
		}
		break;

    case CT_HDR_MODE:
		switch (value)
		{
        case FEATURE_GAIN_HDR_MODE_NONE:
            return asCodeConst ? _T("HDR_MODE_DISABLED") : _T("Disabled");
        case FEATURE_GAIN_HDR_MODE_CAMERA:
            return asCodeConst ? _T("HDR_MODE_CAMERA") : _T("Camera");
        case FEATURE_GAIN_HDR_MODE_INTERLEAVED:
            return asCodeConst ? _T("HDR_MODE_INTERLEAVED") : _T("Interleaved");
        }
        break;

    }

	TRACE("Invalid parameters passed to ApiConstAsString");
	//ASSERT(FALSE);
	return _T("");
}


/**
* Function: FeatureAsString
* Purpose:  
*/
TCHAR const* FeatureAsString(U32 featureId)
{
	switch (featureId)
	{
		case FEATURE_BRIGHTNESS:		return _T("Brightness");
		case FEATURE_SHARPNESS:			return _T("Sharpness");
		case FEATURE_COLOR_TEMP:		return _T("Color Temperature");
		case FEATURE_HUE:				return _T("Hue");
		case FEATURE_SATURATION:		return _T("Saturation");
		case FEATURE_GAMMA:				return _T("Gamma");
		case FEATURE_SHUTTER:			return _T("Exposure Time");
		case FEATURE_GAIN:				return _T("Gain");
		case FEATURE_IRIS:				return _T("Iris");
		case FEATURE_FOCUS:				return _T("Focus");
		case FEATURE_SENSOR_TEMPERATURE:return _T("Sensor Temperature");
		case FEATURE_BODY_TEMPERATURE:	return _T("Body Temperature");
		case FEATURE_TRIGGER:			return _T("Trigger");
		case FEATURE_ZOOM:				return _T("Zoom");
		case FEATURE_PAN:				return _T("Pan");
		case FEATURE_TILT:				return _T("Tilt");
		case FEATURE_OPT_FILTER:		return _T("Optical Filter");
		case FEATURE_GPIO:				return _T("Gpio");
		case FEATURE_FRAME_RATE:		return _T("Frame Rate");
		case FEATURE_ROI:				return _T("ROI");
		case FEATURE_FLIP:				return _T("Flip");
		case FEATURE_PIXEL_ADDRESSING:	return _T("Pixel Addressing");
		case FEATURE_PIXEL_FORMAT:		return _T("Pixel Format");
		case FEATURE_EXTENDED_SHUTTER:	return _T("Extended Shutter");
		case FEATURE_AUTO_ROI:			return _T("Auto ROI");
		case FEATURE_LOOKUP_TABLE:		return _T("Lookup Table");
		case FEATURE_MEMORY_CHANNEL:	return _T("Memory Channel");
		case FEATURE_WHITE_SHADING:		return _T("White Shading");
		case FEATURE_ROTATE:			return _T("Rotate");
		case FEATURE_IMAGER_CLK_DIVISOR:return _T("Imager Clock Divisor");
		case FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT:
										return _T("Trigger With Controlled Light");
		case FEATURE_MAX_PIXEL_SIZE:	return _T("Max Pixel Size");
		case FEATURE_MAX_PACKET_SIZE:	return _T("Max Packet Size");
		case FEATURE_BANDWIDTH_LIMIT:	return _T("Bandwidth Limit");
		case FEATURE_ACTUAL_FRAME_RATE:	return _T("Actual Frame Rate");
		case FEATURE_SHARPNESS_SCORE:   return _T("Sharpness Score");
		case FEATURE_SPECIAL_CAMERA_MODE:   
		                                return _T("Special Camera Mode");
		case FEATURE_GAIN_HDR:          return _T("Gain HDR");
		case FEATURE_POLAR_WEIGHTINGS:  return _T("Polar Weightings");
		case FEATURE_POLAR_HSV_INTERPRETATION:  return _T("Polar HSV Interpretation");
		case FEATURE_PTP:               return _T("PTP");
      case FEATURE_LIGHTING:          return _T("Lighting");
      default:
			return _T("Unknown Feature");
	}
	STATIC_ASSERT(43 == FEATURES_TOTAL, _ADD_NEW_FeatureAsString_CASE);
};

#define CASE(x) case x: return _T(#x)

TCHAR const* 
FeatureIdAsString(U32 featureId)
{
	switch (featureId)
	{
		CASE(FEATURE_PIXELINK_RESERVED_1);
		CASE(FEATURE_BRIGHTNESS);
		CASE(FEATURE_SHARPNESS);
		CASE(FEATURE_COLOR_TEMP);
		CASE(FEATURE_HUE);
		CASE(FEATURE_SATURATION);
		CASE(FEATURE_GAMMA);
		CASE(FEATURE_SHUTTER);
		CASE(FEATURE_GAIN);
		CASE(FEATURE_IRIS);
		CASE(FEATURE_FOCUS);
		CASE(FEATURE_SENSOR_TEMPERATURE);
		CASE(FEATURE_BODY_TEMPERATURE);
		CASE(FEATURE_TRIGGER);
		CASE(FEATURE_ZOOM);
		CASE(FEATURE_PAN);
		CASE(FEATURE_TILT);
		CASE(FEATURE_OPT_FILTER);
		CASE(FEATURE_GPIO);
		CASE(FEATURE_FRAME_RATE);
		CASE(FEATURE_ROI);
		CASE(FEATURE_FLIP);
		CASE(FEATURE_PIXEL_ADDRESSING);
		CASE(FEATURE_PIXEL_FORMAT);
		CASE(FEATURE_EXTENDED_SHUTTER);
		CASE(FEATURE_AUTO_ROI);
		CASE(FEATURE_LOOKUP_TABLE);
		CASE(FEATURE_MEMORY_CHANNEL);
		CASE(FEATURE_WHITE_SHADING);
		CASE(FEATURE_ROTATE);
		CASE(FEATURE_IMAGER_CLK_DIVISOR);
		CASE(FEATURE_TRIGGER_WITH_CONTROLLED_LIGHT);
		CASE(FEATURE_MAX_PIXEL_SIZE);
		CASE(FEATURE_MAX_PACKET_SIZE);
		CASE(FEATURE_BANDWIDTH_LIMIT);
		CASE(FEATURE_ACTUAL_FRAME_RATE);
		CASE(FEATURE_SHARPNESS_SCORE);
		CASE(FEATURE_SPECIAL_CAMERA_MODE);
		CASE(FEATURE_GAIN_HDR);
		CASE(FEATURE_POLAR_WEIGHTINGS);
		CASE(FEATURE_POLAR_HSV_INTERPRETATION);
		CASE(FEATURE_PTP);
      CASE(FEATURE_LIGHTING);
   default:
			ASSERT(0);
			return _T("Unknown feature");
	}
	STATIC_ASSERT(43 == FEATURES_TOTAL, _ADD_NEW_FeatureCodeAsString_CASE);
};

TCHAR const* 
FeatureFlagAsString(U32 featureFlag)
{
	switch (featureFlag)
	{
		CASE(FEATURE_FLAG_PRESENCE);
		CASE(FEATURE_FLAG_MANUAL);
		CASE(FEATURE_FLAG_AUTO);
		CASE(FEATURE_FLAG_ONEPUSH);
		CASE(FEATURE_FLAG_OFF);
		CASE(FEATURE_FLAG_DESC_SUPPORTED);
		CASE(FEATURE_FLAG_READ_ONLY);
		CASE(FEATURE_FLAG_SETTABLE_WHILE_STREAMING);
		CASE(FEATURE_FLAG_PERSISTABLE);
		CASE(FEATURE_FLAG_EMULATION);
		CASE(FEATURE_FLAG_VOLATILE);
        CASE(FEATURE_FLAG_ASSERT_LOWER_LIMIT);
        CASE(FEATURE_FLAG_ASSERT_UPPER_LIMIT);
	}
	return _T("Unknown flag");
}

CString 
GetIPAddressAsString(const PXL_IP_ADDRESS& ipa, bool inHex)
{
	CString s;
	if (inHex) {
		s.Format("%2.2X.%2.2X.%2.2X.%2.2X", 
			ipa.U8Address[0],
			ipa.U8Address[1],
			ipa.U8Address[2],
			ipa.U8Address[3]);
	} else {
		s.Format("%d.%d.%d.%d", 
			ipa.U8Address[0],
			ipa.U8Address[1],
			ipa.U8Address[2],
			ipa.U8Address[3]);
	}
	return s;
}

//
// Convert the CameraIpAssignmentType field of the 
// CAMERA_ID_INFO struct into a human-readable string
//
CString 
GetIpAddressAssignmentAsString(const U8 ipAddressAssignmentType)
{
	switch(ipAddressAssignmentType)
	{
		case PXL_IP_STATIC_VOLATILE:	return "Static (Volatile)";
		case PXL_IP_STATIC_PERSISTENT:	return "Static (Persistent)";
		case PXL_IP_LLA_ASSIGNED:		return "LLA";
		case PXL_IP_DHCP_ASSIGNED:		return "DHCP";
		case PXL_IP_UNKNOWN_ASSIGNMENT:	return "Unknown";
		default:						return "Unknown value";
	}
}

CString 
GetMacAddressAsString(const PXL_MAC_ADDRESS& mac)
{
	CString s;
	s.Format("%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
		mac.MacAddr[0],
		mac.MacAddr[1],
		mac.MacAddr[2],
		mac.MacAddr[3],
		mac.MacAddr[4],
		mac.MacAddr[5]);
	return s;

}

char const*
ReturnCodeAsString(PXL_RETURN_CODE rc)
{
	switch (rc)
	{
	case ApiSuccess:
		return "ApiSuccess";
	case ApiUnknownError:
		return "ApiUnknownError";
	case ApiInvalidHandleError:
		return "ApiInvalidHandleError";
	case ApiInvalidParameterError:
		return "ApiInvalidParameterError";
	case ApiBufferTooSmall:
		return "ApiBufferTooSmall";
	case ApiInvalidFunctionCallError:
		return "ApiInvalidFunctionCallError";
	case ApiNotSupportedError:
		return "ApiNotSupportedError";
	case ApiCameraInUseError:
		return "ApiCameraInUseError";
	case ApiNoCameraError:
		return "ApiNoCameraError";
	case ApiHardwareError:
		return "ApiHardwareError";
	case ApiCameraUnknownError:
		return "ApiCameraUnknownError";
	case ApiOutOfBandwidthError:
		return "ApiOutOfBandwidthError";
	case ApiOutOfMemoryError:
		return "ApiOutOfMemoryError";
	case ApiOSVersionError:
		return "ApiOSVersionError";
	case ApiNoSerialNumberError:
		return "ApiNoSerialNumberError";
	case ApiInvalidSerialNumberError:
		return "ApiInvalidSerialNumberError";
	case ApiDiskFullError:
		return "ApiDiskFullError";
	case ApiIOError:
		return "ApiIOError";
	case ApiStreamStopped:
		return "ApiStreamStopped";
	case ApiNullPointerError:
		return "ApiNullPointerError";
	case ApiCreatePreviewWndError:
		return "ApiCreatePreviewWndError";
	case ApiSuccessParametersChanged:
		return "ApiSuccessParametersChanged";
	case ApiOutOfRangeError:
		return "ApiOutOfRangeError";
	case ApiNoCameraAvailableError:
		return "ApiNoCameraAvailableError";
	case ApiInvalidCameraName:
		return "ApiInvalidCameraName";
	case ApiGetNextFrameBusy:
		return "ApiGetNextFrameBusy";
	case ApiStreamExistingError:
		return "ApiStreamExistingError";
	case ApiEnumDoneError:
		return "ApiEnumDoneError";
	case ApiNotEnoughResourcesError:
		return "ApiNotEnoughResourcesError";
	case ApiBadFrameSizeError:
		return "ApiBadFrameSizeError";
	case ApiNoStreamError:
		return "ApiNoStreamError";
	case ApiVersionError:
		return "ApiVersionError";
	case ApiNoDeviceError:
		return "ApiNoDeviceError";
	case ApiCannotMapFrameError:
		return "ApiCannotMapFrameError";
	case ApiLinkDriverError: // same as ApiOhciDriverError:
		return "ApiLinkDriverError";
	case ApiInvalidIoctlParameter:
		return "ApiInvalidIoctlParameter";
	case ApiInvalidOhciDriverError:
		return "ApiInvalidOhciDriverError";
	case ApiCameraTimeoutError:
		return "ApiCameraTimeoutError";
	case ApiInvalidFrameReceivedError:
		return "ApiInvalidFrameReceivedError";
	case ApiOSServiceError:
		return "ApiOSServiceError";
	case ApiTimeoutError:
		return "ApiTimeoutError";
	case ApiRequiresControlAccess:
		return "ApiRequiresControlAccess";
	case ApiGevInitializationError:
		return "ApiGevInitializationError";
	case ApiIpServicesError:
		return "ApiIpServicesError";
	case ApiIpAddressingError:
		return "ApiIpAddressingError";
	case ApiDriverCommunicationError:
		return "ApiDriverCommunicationError";
	case ApiInvalidXmlError:
		return "ApiInvalidXmlError";
	case ApiCameraRejectedValueError:
		return "ApiCameraRejectedValueError";
	case ApiSuspectedFirewallBlockError:
		return "ApiSuspectedFirewallBlockError";
    case ApiIncorrectLinkSpeed:
        return "ApiIncorrectLinkSpeed";
    case ApiCameraNotReady:
        return "ApiCameraNotReady";
    case ApiInconsistentConfiguration:
        return "ApiInconsistentConfiguration";
    case ApiSuccessReducedSpeedWarning:
        return "ApiSuccessReducedSpeedWarning";
    case ApiNotPermittedWhileStreamingError:
        return "ApiNotPermittedWhileStreamingError";
    case ApiOSAccessDeniedError:
        return "ApiOSAccessDeniedError";
    case ApiInvalidAutoRoiError:
        return "ApiInvalidAutoRoiError";
    case ApiGpiHardwareTriggerConflict:
        return "ApiGpiHardwareTriggerConflict";
    case ApiGpioConfigurationError:
        return "ApiGpioConfigurationError";
    case ApiUnsupportedPixelFormatError:
        return "ApiUnsupportedPixelFormatError";
    case ApiUnsupportedClipEncoding:
        return "ApiUnsupportedClipEncoding";
    case ApiH264EncodingError:
        return "ApiH264EncodingError";
    case ApiH264FrameTooLargeError:
        return "ApiH264FrameTooLargeError";
    case ApiH264InsufficientDataError:
        return "ApiH264InsufficientDataError";
    case ApiNoControllerError:
        return "ApiNoControllerError";
    case ApiControllerAlreadyAssignedError:
        return "ApiControllerAlreadyAssignedError";
    case ApiControllerInaccessibleError:
        return "ApiControllerInaccessibleError";
    case ApiControllerCommunicationError:
        return "ApiControllerCommunicationError";
    case ApiControllerTimeoutError:
        return "ApiControllerTimeoutError";
    case ApiBufferTooSmallForInterleavedError:
        return "ApiBufferTooSmallForInterleavedError";
    case ApiThisEventNotSupported:
        return "ApiThisEventNotSupported";
    case ApiFeatureConflictError:
        return "ApiFeatureConflictError";
    case ApiGpiOnlyError:
        return "ApiGpiOnlyError";
    case ApiGpoOnlyError:
        return "ApiGpoOnlyError";
    case ApiInvokedFromIncorrectThreadError:
        return "ApiInvokedFromIncorrectThreadError";
    case ApiNotSupportedOnLiteVersion:
        return "ApiNotSupportedOnLiteVersion";
    case ApiControllerDuplicateType:
        return "ApiControllerDuplicateType";
    case ApiControllerDuplicateFeature:
        return "ApiControllerDuplicateFeature";

    case ApiSuccessGainIneffectiveWarning:
        return "ApiSuccessGainIneffectiveWarning";
    case ApiSuccessSuspectedFirewallBlockWarning:
        return "ApiSuccessSuspectedFirewallBlockWarning";
    case ApiSuccessApiLiteWarning:
        return "ApiSuccessApiLiteWarning";
	default:
		ASSERT(0);
		return "Unknown Return Code";
	}
}

