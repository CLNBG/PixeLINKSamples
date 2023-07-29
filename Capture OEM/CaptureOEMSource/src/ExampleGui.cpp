// ExampleGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "ExampleGui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExampleGui property page

IMPLEMENT_DYNCREATE(CExampleGui, CDevAppPage)

CExampleGui::CExampleGui(CPxLDevAppView* pView)
	: CDevAppPage(CExampleGui::IDD, pView)
{
	//{{AFX_DATA_INIT(CExampleGui)
	//}}AFX_DATA_INIT
}

CExampleGui::~CExampleGui()
{
}

void CExampleGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExampleGui)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_EG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_EG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_EG_STOP_BUTTON, m_StopButton);

}


BEGIN_MESSAGE_MAP(CExampleGui, CDevAppPage)
	//{{AFX_MSG_MAP(CExampleGui)
	//}}AFX_MSG_MAP
	ON_CBN_DROPDOWN(IDC_EG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_EG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_EG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_EG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_EG_STOP_BUTTON, OnClickStopButton)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExampleGui message handlers

int CExampleGui::GetHelpContextId()
{
	return 0; //IDH_EXAMPLE_TAB;
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CExampleGui::SetGUIState(eGUIState state)
{
	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	bool enable = false;

	// Things that cannot be changed while the preview is running:
	switch (state)
	{
	case GS_NoCamera:
	case GS_PreviewPaused:
	case GS_PreviewPlaying:
		enable = false;
		break;
	case GS_CameraSelected:
		enable = true;
		break;
	default:
		TRACE("Invalid case label reached in CExampleGui::SetGUIState\n");
	}

	// Controls that are enabled only while a camera is selected:
	switch (state)
	{
	case GS_NoCamera:
		enable = false;
		break;
	case GS_CameraSelected:
	case GS_PreviewPaused:
	case GS_PreviewPlaying:
		enable = true;
		break;
	default:
		TRACE("Invalid case label reached in CExampleGui::SetGUIState\n");
		ASSERT(FALSE);
	}

}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CExampleGui::ConfigureControls(void)
{
//	ConfigureXXX();
//	ConfigureYYY();
	//...
}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CExampleGui::PopulateDefaults(void)
{
//	m_XXXRadio.SetCheck(BST_CHECKED);
//	m_XXXSlider.SetRange(0, 100);
//	m_XXXSlider.put_Value(0);
	//...
}


/**
* Function: ApplyUserSettings
* Purpose:  
*/
void CExampleGui::ApplyUserSettings(UserSettings& settings)
{
	// Apply settings
}


/**
* Function: StoreUserSettings
* Purpose:  
*/
void CExampleGui::StoreUserSettings(UserSettings& settings)
{
	// Store settings
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CExampleGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

//	PopulateXXX();
//	PopulateYYY();
	// ...
}


/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CExampleGui::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case 0: //FEATURE_XXX:
		//PopulateXXX();
		break;
	case 1: //FEATURE_YYY:
		//PopulateYYY();
		break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}


