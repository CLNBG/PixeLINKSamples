#pragma once


#include <map>
#include <vector>


// Forward declartions
class CPxLCamera;
class CClipCaptureDialog;


#define PDS_TEMP_NAME  _T("pxl_temp.pds")
#define H264_TEMP_NAME _T("pxl_temp.h264")

// Used to simplify the management of GUI state (i.e. Enabled/Disabled state
// of controls on the page).
enum eGUIState
{
	GS_NoCamera,
	GS_CameraSelected,
	GS_PreviewPlaying,
	GS_PreviewPaused
};



/******************************************************************************
* CPxLCameraDoc class
******************************************************************************/

class CPxLCameraDoc : public CDocument
{
	friend U32 __stdcall CameraDisconnectCallback(HANDLE,U32, double, U32, LPVOID, LPVOID);

    typedef std::map<U32, CPxLCamera*> CameraMap_t;

	DECLARE_DYNCREATE(CPxLCameraDoc)

	DECLARE_MESSAGE_MAP()

protected: // create from serialization only
	CPxLCameraDoc();

public:
	virtual ~CPxLCameraDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void DoViewChange(U32 changeId);

	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	void HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
	// Error handling
	void OnApiError(PxLException const& e, bool cameraRelated = true);

	// Camera Control
	eGUIState GetGUIState(void);
	void UpdateCameraList(CComboBox& cb, bool reloadSerialNumbers=false);
	CPxLCamera* GetActiveCamera(void);
	void SetActiveCamera(U32 serialNumber);
	bool HasActiveCamera(void);
	U32 GetActiveSerialNumber(void);
	bool IsCameraConnected(U32 serialNumber);
	void PingCamera(CPxLCamera* pcam = NULL);
	void UninitializeCamera(void);

	bool CaptureImage(CWnd& pathEdit, CComboBox& formatCombo, int discardFrames=0, int avgFrames=1, bool autoExpose=false);
	bool CaptureClip(CWnd& numFramesEdit, CWnd* pathEdit, CComboBox* formatCombo, CWnd* pdsPathEdit = NULL);
	bool CaptureEncodedClip(CWnd& numFramesEdit, int clipDecimationFactor, CWnd* pathEdit, CComboBox* formatCombo, float pbFrameRate, int pbBitRate, U32 encoding, CWnd* pdsPathEdit = NULL);
	
	void StartCameraPoller(void);  //Starts the camera poller thread.
	void StopCameraPoller(void);   //Stops the camera poller thread.
    static ULONG __stdcall CameraPollerThread(LPVOID context);

private:

	void LoadSerialNumbers(void);
	void RemoveActiveCamera(void);

	bool WriteConfigFile(CPxLCamera* cam, LPCTSTR fileName);
	bool LoadFromConfigFile(CPxLCamera* cam, LPCTSTR fileName);

	// Menu Item message handlers
	afx_msg void OnFileImportConfigFile();
	afx_msg void OnFileExportConfigFile();
	afx_msg void OnFileSetIPAddress();
	afx_msg void OnViewResetPreviewWindow();
	afx_msg void OnViewHistogram();
	afx_msg void OnCameraLoadCameraSettings();
	afx_msg void OnCameraSaveCameraSettings();
	afx_msg void OnCameraSetCameraName();
	afx_msg void OnCameraReinitialize();
	afx_msg void OnAppAbout();

	afx_msg void OnUpdateFileImportConfigFile(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileExportConfigFile(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSetIPAddress(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewResetPreviewWindow(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewHistogram(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCameraLoadCameraSettings(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCameraSaveCameraSettings(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCameraSetCameraName(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCameraReinitialize(CCmdUI *pCmdUI);

	afx_msg void OnLaptopfriendlymodeOff();
	afx_msg void OnLaptopfriendlymode2();
	afx_msg void OnLaptopfriendlymode3();
	afx_msg void OnLaptopfriendlymode4();
	afx_msg void OnUpdateLaptopfriendlymodeOff(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLaptopfriendlymode2(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLaptopfriendlymode3(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLaptopfriendlymode4(CCmdUI *pCmdUI);

    afx_msg void OnSlowImagerMode();
    afx_msg void OnUpdateSlowImagerMode(CCmdUI *pCmdUI);

    afx_msg void OnFixedFramerateMode();
    afx_msg void OnUpdateFixedFramerateMode(CCmdUI *pCmdUI);
private:
	
	std::vector<U32>					m_serialNumbers;
	std::vector<CAMERA_ID_INFO>			m_cameraIdInfo;
	CameraMap_t							m_cameras;
	CPxLCamera*							m_pActiveCamera;
	CClipCaptureDialog*					m_pOpenClipCaptureDlg;
	bool								m_bDriversLoading;
	
	HANDLE                              m_CameraPoller;          // Polls for cameras. Self terminates when it finds at least one
	HANDLE                              m_CameraPollerStopEvent; // Kills the cameraPoller thread.

	void SetLaptopMode(int framerateDivisor);

public:
	afx_msg void OnViewAutoResetPreview();
	afx_msg void OnUpdateViewAutoResetPreview(CCmdUI *pCmdUI);
	afx_msg void OnLaptopfriendlymode8();
	afx_msg void OnUpdateLaptopfriendlymode8(CCmdUI *pCmdUI);
};



