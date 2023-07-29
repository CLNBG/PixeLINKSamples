#pragma once

#include <vector>
#include <algorithm>


void ErrorMessage(LPCTSTR msg);
#ifdef _UNICODE
void ErrorMessage(LPCSTR msg);
#endif


//
// Simple static assert for checking things at compile-time
//
#ifndef STATIC_ASSERT
/* We add a useless reference to get rid of the 'unreferenced variable' warning */
#define	STATIC_ASSERT(x, msg)	do { char static_assert_failure ## msg[(true == (x)) ? 1 : 0]; static_assert_failure ## msg; } while(0)
#endif

// IMAGE_FORMAT_RAW may be defined in PixeLINKTypes.h in a future version:
#ifndef IMAGE_FORMAT_RAW
#define IMAGE_FORMAT_RAW 0x1000
#endif


enum eApiConstType
{
	CT_FEATURE,
	CT_FEATURE_FLAG,
	CT_IMAGE_FORMAT,
	CT_PIXEL_FORMAT,
	CT_PIXEL_FORMAT_INTERPRETATION,
	CT_PREVIEW_STATE,
	CT_STREAM_STATE,
	CT_TRIGGER_TYPE,
	CT_TRIGGER_MODE,
	CT_GPIO_MODE,
	CT_DESCRIPTOR_UPDATE_MODE,
	CT_DECIMATION_MODE,
    CT_HDR_MODE
};

typedef enum _PIXEL_FORMAT_INTERPRETATION
{
    CT_HSV_AS_COLOR,
    CT_HSV_AS_ANGLE,
    CT_HSV_AS_DEGREE
} PIXEL_FORMAT_INTERPRETATION;


// Round off a floating point number.
template<typename T, typename U>
T round_to(U val)
{
	return static_cast<T>(val + 0.5);
}


// ROI Standard Sizes
void SetWindowSizeEntries(CComboBox & cbo, 
						  int minWidth, int minHeight, 
						  int maxWidth, int maxHeight,
						  bool sideways = false);

// Decimation combo box entries
void SetDecimationModeEntries(CComboBox& cb, std::vector<int> const& supportedValues);
void SetDecimationEntries(CComboBox& cb, std::vector<int> const& supportedXValues, std::vector<int> const& supportedYValues, bool asymmetricPa);


// Image File Formats
//extern int const imageFormats[];
//extern TCHAR const* imageFormatNames[];
void SetImageFormatEntries(CComboBox& cb);
extern TCHAR const* imageFormatFilterString;

void SetImageFormatFromFilename(CWnd& pathEdit, CComboBox& formatCombo);
void SetFileExtensionFromImageFormat(CWnd& pathEdit, CComboBox& formatCombo);
CString GetImageFormatExtension(int index);
//int GetImageFormatExtensionIndex(CString& ext);


// Clip File Formats
//extern int const clipFormats[];
//extern TCHAR const* clipFormatNames[];
void SetClipFormatEntries(CComboBox& cb);
extern TCHAR const* clipFormatFilterString;
extern TCHAR const* pdsFormatFilterString;

void SetClipFormatFromFilename(CWnd& pathEdit, CComboBox& formatCombo);
void SetFileExtensionFromClipFormat(CWnd& pathEdit, CComboBox& formatCombo);
CString GetClipFormatExtension(int index);
//int GetClipFormatExtensionIndex(CString& ext);

void SetClipEncodingEntries(CComboBox& cb);

void SetClipEncodingFromFilename(CWnd& pathEdit, CComboBox& formatCombo);
void SetFileExtensionFromClipEncoding(CWnd& pathEdit, CComboBox& formatCombo);
CString GetClipEncodingExtension(int index);

// Combined Image and Video formats (for Simple Mode)
void SetCombinedFormatEntries(CComboBox& cb);
extern TCHAR const* combinedFormatFilterString;

void SetCombinedFormatFromFilename(CWnd& pathEdit, CComboBox& formatCombo);
void SetFileExtensionFromCombinedFormat(CWnd& pathEdit, CComboBox& formatCombo);
CString GetCombinedFormatExtension(int index);


// Pixel Formats
//extern int const pixelFormats[];
//extern TCHAR const* pixelFormatNames[];
//extern int const nPixelFormats;
float GetPixelFormatSize(int pixelFormat);


// GPIO Modes
//extern int const gpioModes[];
//extern TCHAR const* gpioModeNames[];
void SetGpioModeEntries(CComboBox& cb, std::vector<int> const& supportedModes);
int GpioModeNumParams(U32 mode);
TCHAR const** GetGpoParameterNames(int mode);
CString GetGpoDescription(CComboBox const& modeCombo);

// Action Commands
void SetActionCommandEntries(CComboBox& cb, std::vector<int> const& supportedCommands);

// Events
TCHAR const* GetEventNames(int eventId);

// Trigger Types
//extern int const triggerTypes[];
//extern TCHAR const* triggerTypeNames[];
void SetTriggerTypeEntries(CComboBox& cb, std::vector<int> const& supportedTypes);
bool TriggerModeTakesParameter(U32 mode);


// Trigger Modes
struct TriggerModeDescriptor
{
	int				mode;
	TCHAR const*	name;
	TCHAR const*	description;
	bool			takesParam;
	TCHAR const*	paramName;
	TCHAR const*	paramUnits;
	bool			validWithType[3];
};
void SetTriggerModeEntries(CComboBox& cb, std::vector<int> const& supportedModes);
bool TriggerModeTypeComboIsValid(U32 mode, U32 type);
TriggerModeDescriptor* GetTriggerDescriptor(int mode);

// PTP State (status)
TCHAR const* GetPtpState(U32 ptpStatus);

// Generally useful functions
bool BrowseForFile(CWnd& wnd,
				   TCHAR const* filterString = NULL,
				   TCHAR const* defaultExtension =NULL);
//bool EnsureValidPath(CWnd& pathEdit,
//					 TCHAR const* filterString = NULL,
//					 TCHAR const* defaultExtension = NULL);
bool EnsureValidImagePath(CWnd& pathEdit, CComboBox& formatCombo);
bool EnsureValidClipPath(CWnd& pathEdit, CComboBox& formatCombo);
bool EnsureValidPdsPath(CWnd& pathEdit);

void SetFloatText(CWnd& wnd, float val, int dp = 2);
void SetIntText(CWnd& wnd, int val);

void ClearComboBox(CComboBox& cb);
void ScrollToEnd(CEdit& ed);
void SetPolarityEntries(CComboBox& cb, std::pair<float,float> minmax);

TCHAR const* FeatureAsString(U32 featureId);
TCHAR const* FeatureIdAsString(U32 featureId);
TCHAR const* FeatureFlagAsString(U32 featureFlag);
TCHAR const* ApiConstAsString(eApiConstType constType, U32 value, bool asCodeConst = false);

CString WindowTextAsString(CWnd const& wnd);
int WindowTextToInt(CWnd& wnd);
float WindowTextToFloat(CWnd& wnd);
void IncrementFileName(CWnd& wnd, const CString& format = CString("%d"));
void SysOpenFile(CString& path);

std::string TStringToStdString(char const* cstr, bool appendNullChar =true);
std::string TStringToStdString(wchar_t const* wstr, bool appendNullChar =true);


/**
* Function: SelectByItemData
* Purpose:  Select the item in the combo box whose ItemData property is equal
*           to the specified value. If no item matches, set the selection to
*           -1 (nothing selected).
*           Return true if something is selected.
*/
template<typename T>
bool SelectByItemData(CComboBox& cb, T val)
{
	for (int idx = 0; idx < cb.GetCount(); idx++)
	{
		if (val == static_cast<T>(cb.GetItemData(idx)))
		{
			cb.SetCurSel(idx);
			return true;
		}
	}
	cb.SetCurSel(-1);
	return false;
}

// Like std::copy, but does a static_cast of each value;
// Doesn't work in vc6.
template<typename InIt, typename OutIt>
void cast_copy(InIt start, InIt end, OutIt dest)
{
	for ( ; start != end ; ++start, ++dest )
		*dest = static_cast<typename OutIt::value_type>(*start);
}

// Get the ItemData property of the currently selected item in a combo box.
template<typename T>
T ItemData(CComboBox const& cb)
{
	return static_cast<T>(cb.GetItemData(cb.GetCurSel()));
}

// Version of the previous function for getting pointers.
template<typename T>
T ItemDataPtr(CComboBox const& cb)
{
	return static_cast<T>(cb.GetItemDataPtr(cb.GetCurSel()));
}

/**
* Function: contains
* Purpose:  
*/
template<typename T>
bool contains(T const& container,
			  typename T::value_type val)
{
	return std::find(container.begin(), container.end(), val) != container.end();
}

/**
* Function: remove
* Purpose:  
*/
template<typename T>
void remove(T& container,
		    typename T::value_type val)
{
   container.erase(std::remove(container.begin(), container.end(), val), container.end());
}


/**
* Class:   TempVal
* Purpose: Create one of these on the stack to temporarily change the value
*          of a variable within the scope of a code block.
*/
template<typename T>
class TempVal
{
public:
	TempVal(T& var, T val) : m_var(var), m_oldVal(var) { var = val; }
	~TempVal() { m_var = m_oldVal; }
private:
	T&	m_var;
	T	m_oldVal;
};


/**
* Class:   TempIncr
* Purpose: Create one of these on the stack to temporarily increment the value
*          of a numeric variable within the scope of a code block.
*/
template<typename T>
class TempIncr
{
public:
	TempIncr(T& var) : m_var(var) { ++var; }
	~TempIncr() { --m_var; }
private:
	T& m_var;
};


// Source: http://www.freescale.com/webapp/sps/site/overview.jsp?nodeId=018rH3bTdGmKqW5Nf2F9DHMbVXVDcM
template<typename T1, typename T2>
inline void BGRtoYCbCr(T1 const* BGR, T2* YCbCr)
{
	int Y  = (BGR[2] *  8432  + BGR[1] *  16425 + BGR[0] *  3176 ) / 32678 + 16;
	int Cb = (BGR[2] * -4818  + BGR[1] * -9527  + BGR[0] *  14345) / 32768 + 128;
	int Cr = (BGR[2] *  14345 + BGR[1] * -12045 + BGR[0] * -2300 ) / 32768 + 128;
	YCbCr[0] = min(max(Y,0),255);
	YCbCr[1] = min(max(Y,0),255);
	YCbCr[2] = min(max(Y,0),255);
}

// RGB <==> YUV conversions
template<typename T1, typename T2>
inline void RGBtoYUV(T1 const* RGB, T2* YUV)
{
	int Y = static_cast<int>( ( 0.299000 * RGB[0]) + ( 0.615000 * RGB[1]) + ( 0.114000 * RGB[2]) + 0  );
	int U = static_cast<int>( (-0.168077 * RGB[0]) + (-0.329970 * RGB[1]) + ( 0.498047 * RGB[2]) + 128);
	int V = static_cast<int>( ( 0.498047 * RGB[0]) + (-0.417052 * RGB[1]) + (-0.080994 * RGB[2]) + 128);
	YUV[0] = round_to<T2>(min(max(Y,0),255));
	YUV[1] = round_to<T2>(min(max(U,0),255));
	YUV[2] = round_to<T2>(min(max(V,0),255));
}

template<typename T1, typename T2>
inline void BGRtoYUV(T1 const* BGR, T2* YUV)
{
	T1 RGB[3] = {BGR[2], BGR[1], BGR[0]};
	RGBtoYUV(&RGB[0], YUV);
}

//  | 1	    -1.223674E-6	1.407498    |
//  | 1	    -0.345485	    -0.716937   |
//  | 1	    1.778949	    4.078914E-7 |
template<typename T1, typename T2>
inline void YUVtoRGB(T1 const* YUV, T2* RGB)
{
	int R = static_cast<int>( YUV[0] + (-1.223674e-6 * (YUV[1]-128)) + ( 1.407498    * (YUV[2]-128)) );
	int G = static_cast<int>( YUV[0] + (-0.345485    * (YUV[1]-128)) + (-0.716937    * (YUV[2]-128)) );
	int B = static_cast<int>( YUV[0] + (1.778949     * (YUV[1]-128)) + ( 4.078914e-7 * (YUV[2]-128)) );
	RGB[0] = static_cast<T2>(min(max(R,0),255));
	RGB[1] = static_cast<T2>(min(max(G,0),255));
	RGB[2] = static_cast<T2>(min(max(B,0),255));
}

template<typename T1, typename T2>
inline void YUVtoBGR(T1 const* YUV, T2* BGR)
{
	T2 RGB[3] = {0,0,0};
	YUVtoRGB(YUV, &RGB[0]);
	BGR[0] = RGB[2];
	BGR[1] = RGB[1];
	BGR[2] = RGB[0];
}

CString GetIPAddressAsString(const PXL_IP_ADDRESS& ipa, bool useHex=false);
CString GetMacAddressAsString(const PXL_MAC_ADDRESS& mac);
CString	GetIpAddressAssignmentAsString(const U8);
char const*	ReturnCodeAsString(PXL_RETURN_CODE rc);



