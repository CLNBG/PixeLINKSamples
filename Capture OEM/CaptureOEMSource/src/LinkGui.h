#pragma once


#include "DevAppPage.h"
#include "SubwindowCtrl.h"
#include "CSlider.h"
#include "afxwin.h"


// Default size of the ROI shown in the subwindow control - to display when
// no camera is active.
#define DEFAULT_FOV_WIDTH 1280
#define DEFAULT_FOV_HEIGHT 1024


// CRoiGui dialog

class CLinkGui : public CDevAppPage
{
	DECLARE_DYNCREATE(CLinkGui)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CLinkGui(CPxLDevAppView* pView = NULL);
	~CLinkGui(void);
	int GetHelpContextId(void);
	BOOL OnInitDialog(void);
	virtual BOOL OnSetActive();

	enum { IDD = IDD_LINK_GUI };
	UINT GetDlgId() { return IDD; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	void SetGUIState(eGUIState state);
	
	void PopulateDefaults(void);
	void PopulateControls(void);
	void ConfigureControls(void);
	void SetPacketSizeEntries(void);
	void FeatureChanged(U32 featureId);

   void PopulateFirewallWarning(void);
	void PopulatePacketSize(void);

   void PopulateBandwidthLimitOnOff(void);
   void PopulateFpsBwWarning(void);

	SLIDER_HANDLER_SUITE(BandwidthLimit)

	afx_msg void OnSetFocusMaxPacketSizeEdit();
	afx_msg void OnKillFocusMaxPacketSizeEdit();
	afx_msg void OnClickMaxPacketSizeAutoButton(void);

	afx_msg void OnClickBandwidthLimitOnOffCheck(void);

	SLIDER_CONTROL_SUITE(BandwidthLimit)
	
   CStatic         m_FirewallWarningStatic;
	CFeatureEdit    m_MaxPacketSizeEdit;
   CButton         m_MaxPacketSizeAutoButton;

	CButton         m_BandwidthLimitOnOffCheck;
   CStatic         m_FpsWarningBwStatic;
   CStatic         m_LinkSpeedWarningBwStatic;

};











