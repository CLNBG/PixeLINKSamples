#pragma once


#include <PixeLINKTypes.h>
#include "PxLExceptions.h"
#include "PxLDescriptor.h"
#include <vector>
#include <utility>


// Macro to allow us to get more detailed error messages in a debug build
// without changing any code.
// Define the PXL_ERROR_CHECK macro, which is equivalent to the PxLErrorCheck
// function in Release mode, but adds preprocessor-generated filename and line
// info as the optional second parameter to PxLErrorCheck in Debug mode.
#ifdef _DEBUG
# define TOSTR_2(x)				#x
# define TOSTR(x)				TOSTR_2(x)
# if MSC_VER >= 1300 // vc7.1
#  define DEBUG_MSG				__FILE__  "\nIn: "  __FUNCTION__  "\nAt line: "  TOSTR(__LINE__)
# else
#  define DEBUG_MSG				__FILE__  "\nAt line: "  TOSTR(__LINE__)
# endif
# define PXL_ERROR_CHECK(x)		PxLErrorCheck((x), DEBUG_MSG)
#else
# define PXL_ERROR_CHECK(x)		PxLErrorCheck(x)
#endif
#define THROW_PXL_EXCEPTION(x)	PXL_ERROR_CHECK(x)


// Macro to calculate decimated width or height of the ROI:
#define DEC_SIZE(len,dec) (((len) + (dec) - 1) / (dec))


// Forward declarations
class CPxLCameraDoc;
class CLookupGraph;
class CHistogramDialog2;
class CDevAppPreview;


// Types of changes that can occur in the document. One of these
// values is passed to the View class whenever UpdateAllViews is called,
// and the view uses it to determine what sort of updates need to be done.
enum eChangeType
{
	CT_NoCamera			= 0,
	CT_NewCamera		= 1,
	CT_PreviewStarted	= 2,
	CT_PreviewPaused	= 4,
	CT_PreviewStopped	= 8,
	CT_FeatureChanged	= 16 // Make sure this one is always LAST (see View class's OnUpdate for reason)
};


// Some helper structs to use when.
struct Trigger
{
	int		Mode;
	int		Type;
	int		Polarity;
	float	Delay;
	float	Parameter;
};

// Note: We include flags in the GPO struct becase GPOs can be turned
//       on or off individually (unlike other featurs, which are described
//       by a single, feature-wide set of flags).
struct Gpo
{
	U32		flags;
	int		Mode;
	int		Polarity;
	float	Parameter1;
	float	Parameter2;
	float	Parameter3;
};

struct PixelAddressing
{
	int		Mode;
	int		x;
	int     y;
	bool	supportAsymmetry; // true indicates the camera will accept dirffert x and y values
};


//
// Model 'generation' designators
enum
{   
    PL_A = 0,
    PL_B = 1
};

// Pixelink cameras support a wide variety of differnt GPIO configurations.  To help simply the representationused by a specific
// camera, the GPIO arrangment can be categorized into one of the following 'profiles'
typedef enum _GpioProfiles
{
                      //     Product Families                  Description
    NO_GPIOS,         //         PL-A/B/D                No GPIOs at all
    FOUR_GPOS,        //         PL-A/B                  GPIO#1...GPIO#4, all of them GPOs
    ONE_GPO,          //   older auto-focus PL-D         GPIO#1  is a GPO
    ONE_GPIO,         //   newer auto-focus PL-D         GPIO#1.  IS configurable as a GPO or GPI
    ONE_GPIO_ONE_GPO, //       newer PL-D                GPIO#1 IS configurable as a GPO or GPI.  GPO#2 is a GPO
    TWO_GPOS,         //       older PL-D                GPIO#1 and GPO#2 are GPOs.
    TWO_GPOS_ONE_GPI, //         PL-X                    GPIO#1 and GPO#2 are GPOs.  GPIO#3 is a GPI
} GpioProfiles;

// Lighting information
struct Lighting
{
   int      Brightness;  // 0 - 99
   float    Current;     // In Amps
   float    Voltage;     // In Volts
   float	   Temperature; // In degrees Celcius
};

// Callback types
typedef U32 (__stdcall * PxLApiCallback)(HANDLE, LPVOID, U32, FRAME_DESC const *, LPVOID);
typedef U32 (__stdcall * ClipTerminationCallback)(HANDLE, U32, PXL_RETURN_CODE);
typedef U32 (__stdcall * EventCallback)(HANDLE, U32, double, U32, LPVOID, LPVOID);

typedef std::vector<CPxLDescriptor> DescriptorList_t;


class CPxLCamera
{
	friend class CPxLCameraDoc;
	friend class CPreviewState;
	friend class CStreamState;

public:
	explicit CPxLCamera(U32 serialNumber, CPxLCameraDoc* pDoc = NULL);
	~CPxLCamera(void);

	static bool EnableJumboFrames(bool enable);

	void PostViewChange(int /*eChangeType*/ change, U32 changedFeature = 0);
	void OnApiError(PxLException const& e, bool cameraRelated = true);
	CString ErrorReportAsString(PxLException const& e, bool cameraReleated = true);

	void Initialize(void);
	void Uninitialize(void);
	void Reinitialize(void);

	void LoadCameraSettings(int channel);
	void SaveCameraSettings(int channel);
	void SetCameraName(LPCSTR name);
	HANDLE GetHandle(void) { return m_handle; }

	U32 GetSerialNumber(void);
	CAMERA_INFO const* GetCameraInfo(void);

	CAMERA_ID_INFO	const* GetCameraIdInfo() const;
	void	SetCameraIdInfo(CAMERA_ID_INFO& cameraIdInfo);

	int GetStreamState(void);
	void SetStreamState(int state, bool forceChange = false);

	static UINT __stdcall PreviewMonitorThread(void* pData);
	void StartPreview(void);
	void PausePreview(void);
	void StopPreview(void);
	int GetPreviewState(void);
	void SetPreviewState(int state);
	void ResetPreviewWindow(void);
	HWND GetPreviewHwnd(void);

	void GetNextFrame(std::vector<byte> & buf, PFRAME_DESC pfd, int discardFrames=0, int avgFrames=1);

	bool FeatureSupported(U32 featureId);
	bool FeatureSupportsManual(U32 featureId);
	bool FeatureSupportsAuto(U32 featureId);
	bool FeatureSupportsOnePush(U32 featureId);
	bool FeatureSupportsAutoLimits(U32 featureId);
	bool FeatureSupportsOnOff(U32 featureId);
	bool FeatureSupportsDescriptor(U32 featureId);
	bool FeatureSettableWhileStreaming(U32 featureId);
    bool FeatureSupportedInController(U32 featureId);
    bool FeatureEmulated(U32 featureId);
    bool FeatureUsesAutoRoi(U32 featureId);
    bool FeatureSupportsAssertLowerLimit(U32 featureId);
    bool FeatureSupportsAssertUpperLimit(U32 featureId);
	std::pair<float,float> GetFeatureLimits(U32 featureId, int paramNo = 0);
	float GetFeatureMinVal(U32 featureId, int paramNo = 0);
	float GetFeatureMaxVal(U32 featureId, int paramNo = 0);
	bool IsDynamicFeature(U32 featureId);

	float GetAbsMaxShutter();
	float GetAbsMinShutter();

	std::vector<int> const& GetSupportedDecimationModeValues(void);
	std::vector<int> const& GetSupportedXDecimationValues(void);
	std::vector<int> const& GetSupportedYDecimationValues(void);
	std::vector<int> const& GetSupportedPixelFormats(void);
	std::vector<float> const& GetSupportedGains(void);
	std::vector<int> const& GetSupportedTriggerModes(void);
	std::vector<int> const& GetSupportedTriggerTypes(void);
	std::vector<int> const& GetSupportedGpioModes(void);
	std::vector<float> const& GetSupportedColorTempValues(void);
    bool SupportsAsymmetricPixelAddressing(void);

	std::vector<float>		GetSupportedValues(U32 featureId);
	void					BinarySearchForSupportedValues(U32 featureId, U32 flags, std::vector<float>& supportedValues, float min, float max);


	// Methods for getting and setting features
	// Methods for getting or setting a single parameter:
	float GetFeatureValue(U32 featureId, U32* pFlags = NULL, int paramNo = 0);
	bool IsFeatureFlagSet(U32 featureId, U32 flag);
    bool FeatureEnabled(const U32 featureId);
	void SetFeatureValue(U32 featureId, float val, int paramNo = 0, bool doPreviewReset = false);
	U32 GetFeatureParamCount(U32 featureId);
	std::pair<float,float> GetFeatureAutoLimits(U32 featureId);

	// Methods for getting and setting multi-parameter features:
	CRect GetFeatureRect(U32 featureId);
	void SetFeatureRect(U32 featureId, long left, long top, long width, long height, bool doPreviewReset = false, bool disable = false);
	void SetFeatureRectSize(U32 featureId, long width, long height, bool doPreviewReset = false, bool disabled = false);
	CRect GetRoi(U32 featureId = FEATURE_ROI);
	void SetRoi(long left, long top, long width, long height, bool doPreviewReset = false, U32 featureId = FEATURE_ROI, bool disable = false);
	void SetRoiSize(long width, long height, bool doPreviewReset = false, U32 featureId = FEATURE_ROI, bool disabled = false);

	bool GetHorizontalFlip(void);
	void SetHorizontalFlip(bool flipped);
	bool GetVerticalFlip(void);
	void SetVerticalFlip(bool flipped);

	Trigger GetTrigger(U32* pFlags = NULL);
	void SetTrigger(Trigger trigger, U32 flags = 0);

	Gpo GetGpo(int gpoNumber);
	PXL_RETURN_CODE SetGpo(Gpo gpo, int gpoNumber);
    GpioProfiles GetGpioProfile(void);

	PixelAddressing GetPixelAddressing(U32* pFlags = NULL);
	void SetPixelAddressing(PixelAddressing pa, U32 flags = 0, bool doPreviewReset = false);

	std::vector<float> GetKneePoints(void);
	void SetKneePoints(std::vector<float> knees);

	std::vector<float> GetWhiteShading(void);
	void SetWhiteShading(std::vector<float> rgb);

   Lighting GetLighting(U32* pFlags = NULL);

	std::vector<float> GetLookupTable(U32* pFlags = NULL);
	void SetLookupTable(std::vector<float> & values, U32 extraFlags = 0);
	void InvalidateLookupGraphCache();
	void CacheLookupGraphPoints(std::vector<long> const& pts, std::vector<BYTE> const& types);
	bool PopulateGraph(CLookupGraph& graph);

	// Setting special flags
	void SetAuto(U32 featureId, bool turnOn, std::pair<float,float> limit = std::pair<float,float>(0.0,0.0));
	void SetOnePush(U32 featureId, bool turnOn = true, std::pair<float,float> limit = std::pair<float,float>(0.0,0.0));
    void SetLimit(U32 featureId, bool lower);
	void SetOnOff(U32 featureId, bool turnOn);
	void SetGpoFlags(U32 flags, int gpoNumber);

	// Callbacks
	void SetCallback(int usageFlags, void* context, PxLApiCallback callback);
	PxLApiCallback GetCallback(int usageFlag);
	void* GetCallbackParam(int usageFlag);

	// Event Callbacks
	void SetEventCallback(int eventId, void* context, EventCallback callback);

    // Descriptor stuff:
	void CreateDescriptor(int updateMode);
	void SetActiveDescriptor(int descriptorNumber);
	void RemoveDescriptor(void);
	void DeleteAllDescriptors(void);
	int GetDescriptorCount(void);
	int GetDescriptorUpdateMode(void);
	void SetDescriptorUpdateMode(int mode);
	int GetInFocusDescriptorNumber(void);

	// Saving clips
	void GetClip(U32 uNumFrames, LPCSTR fileName, ClipTerminationCallback termCallback);
    void GetEncodedClip(U32 uNumFrames, U32 uClipDecimationFactor, LPCSTR fileName,  float pbFrameRate, int pbBitRate, U32 encoding, ClipTerminationCallback termCallback);

	CHistogramDialog2* m_pHistogramWindow;
	void CloseHistogram(void);

	// FFC Stuff
	bool SupportsManualFFC(void);
	void EnableFfc(bool enable);
	bool IsFfcOn(void);
	float GetFfcCalibratedGain(void);
	bool ReadMemory(ULONG addr, ULONG nbytes, UCHAR* buffer);
	CString GetFFCType(void);

	int GetModelNumber(void);
	int GetModelGeneration(void);
	int GetHWVersion(void);

	bool IsLinkSpeedReduced(void);

    bool IsFirewallBlocked(void);
    bool IsFixedFramerateModeSupported(void);
    bool IsFixedFramerateModeEnabled(void);

  	// Polar camera stuff
    std::vector<float> GetPolarWeightings(void);
	void SetPolarWeightings(std::vector<float> weightings);

    // PTP stuff
    double GetTimestamp(void);

    bool IsActionsSupported(void);

	HANDLE	m_handle;
    U32     m_oneTimeAutofeatureId; // The feature currently doing a one-time auto adjustment.

private:
	PXL_RETURN_CODE DebugRequestSend(UCHAR* dataPointer, ULONG inputSize, ULONG outputSize);

private:
   void LoadCameraFeatures(void);
   void ReloadFeature(U32 featureId);
	void LoadCameraInfo(void);
	void ClearCameraInfo(void);
	void ClearFeatures(void);

	bool FeatureSupportsFlag(U32 featureId, U32 flag);
	CAMERA_FEATURE* GetFeaturePtr(U32 featureId);

	void LoadSupportedDecimations(void);
	void LoadSupportedPixelFormats(void);
	void LoadSupportedGains(void);
	void LoadSupportedTriggerParams(void);
	void LoadSupportedGpioModes(void);
	void LoadSupportedColorTempValues(void);

	bool RequiresStoppedStream(U32 featureId);
	bool FeatureAffectsPreview(U32 featureId);

	bool FeatureAffectsBandwidth(U32 featureId);
	bool SetBandwidth(void);

	void SetSingleParamFeatureValue(U32 featureId, float val);
	void SetMultiParamFeatureValue(U32 featureId, float val, int paramNo);

	void SetFlags(U32 featureId, U32 flags, std::pair<float,float> limit = std::pair<float,float>(0.0,0.0));
	void ClearFlags(U32 featureId, U32 flags);
    
private:
	//CPxLCameraDoc*					m_pDoc;
	U32								m_serialNumber;
	std::auto_ptr<CAMERA_INFO>		m_pInfo;
	CAMERA_ID_INFO					m_cameraIdInfo;
	PCAMERA_FEATURES				m_pFeatures;
	std::vector<CAMERA_FEATURE*>	m_features;
	int								m_streamState;
	int								m_streamStartRequests;
	int                             m_disallowedPauses;
    EXECUTION_STATE                 m_savedThreadExecutionState;
	int								m_previewState;
	HWND							m_previewHwnd;
	HANDLE							m_previewStopEvent;
	RECT							m_previewPos;

	PxLApiCallback					m_frameCallback;
	PxLApiCallback					m_previewCallback;
	PxLApiCallback					m_imageCallback;
	PxLApiCallback					m_clipCallback;
	void*							m_frameCallbackParam;
	void*							m_previewCallbackParam;
	void*							m_imageCallbackParam;
	void*							m_clipCallbackParam;

	bool                            m_supportsAsymmetricPa;
	std::vector<int>				m_supportedXDecimations;
	std::vector<int>				m_supportedYDecimations;
	std::vector<int>				m_supportedDecimationModes;
	std::vector<int>				m_supportedPixelFormats;
	std::vector<float>				m_supportedGains;
	std::vector<int>				m_supportedTriggerModes;
	std::vector<int>				m_supportedTriggerTypes;
	std::vector<int>				m_supportedGpioModes;
	std::vector<float>				m_supportedColorTempValues;

	DescriptorList_t				m_descriptors;
	int								m_inFocusDescIndex; // -1 == All

	std::vector<long> m_lookupPts;
	std::vector<BYTE> m_lookupTypes;

	//CDevAppPreview*					m_pPreview;

	bool							m_bNoPostViewChange;
	bool							m_bLookupTableNeedsUpdate;

	float							m_absMaxShutter;
	float							m_absMinShutter;
	
	bool                            m_bReducedLinkSpeed;
	bool                            m_bSuspectedFirewallBlocking;

};









