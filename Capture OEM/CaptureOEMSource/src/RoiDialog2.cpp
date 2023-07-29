// RoiDialog2.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "RoiDialog2.h"
#include <afxpriv.h>


// CRoiDialog2 dialog

IMPLEMENT_DYNAMIC(CRoiDialog2, CDialog)

CRoiDialog2::CRoiDialog2(CDevAppPage* pParent)
	: CDialog(CRoiDialog2::IDD, pParent)
	, m_parent(pParent)
	, m_bAdjusting(pParent->m_bAdjusting)
{
}

CRoiDialog2::~CRoiDialog2()
{
}

void CRoiDialog2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SGRD_SUBWINDOW_CTRL, m_SubwindowCtrl);
	DDX_Control(pDX, IDC_SGRD_WINDOWTOP_EDIT, m_TopEdit);
	DDX_Control(pDX, IDC_SGRD_WINDOWLEFT_EDIT, m_LeftEdit);
	DDX_Control(pDX, IDC_SGRD_WINDOWHEIGHT_EDIT, m_HeightEdit);
	DDX_Control(pDX, IDC_SGRD_WINDOWWIDTH_EDIT, m_WidthEdit);
	DDX_Control(pDX, IDC_SGRD_DECIMATION_COMBO, m_DecimationCombo);
	DDX_Control(pDX, IDC_SGRD_DECIMATIONMODE_COMBO, m_DecimationModeCombo);
	DDX_Control(pDX, IDC_SGRD_WINDOWSIZE_COMBO, m_WindowSizeCombo);
	DDX_Control(pDX, IDC_SGRD_CENTER_BUTTON, m_CenterButton);

	DDX_Control(pDX, IDC_SGRD_ROTATE0_RADIO, m_RotateNoneRadio);
	DDX_Control(pDX, IDC_SGRD_ROTATE90_RADIO, m_Rotate90Radio);
	DDX_Control(pDX, IDC_SGRD_ROTATE180_RADIO, m_Rotate180Radio);
	DDX_Control(pDX, IDC_SGRD_ROTATE270_RADIO, m_Rotate270Radio);

	DDX_Control(pDX, IDC_SGRD_IMAGEFLIPHORIZONTALOFF_RADIO, m_ImageFlipHorizontalOffRadio);
	DDX_Control(pDX, IDC_SGRD_IMAGEFLIPHORIZONTALON_RADIO, m_ImageFlipHorizontalOnRadio);
	DDX_Control(pDX, IDC_SGRD_IMAGEFLIPVERTICALOFF_RADIO, m_ImageFlipVerticalOffRadio);
	DDX_Control(pDX, IDC_SGRD_IMAGEFLIPVERTICALON_RADIO, m_ImageFlipVerticalOnRadio);
}


BEGIN_MESSAGE_MAP(CRoiDialog2, CDialog)

	ON_EN_KILLFOCUS(IDC_SGRD_WINDOWTOP_EDIT, OnKillFocusSubwindowTopEdit)
	ON_EN_KILLFOCUS(IDC_SGRD_WINDOWLEFT_EDIT, OnKillFocusSubwindowLeftEdit)
	ON_EN_KILLFOCUS(IDC_SGRD_WINDOWWIDTH_EDIT, OnKillFocusSubwindowWidthEdit)
	ON_EN_KILLFOCUS(IDC_SGRD_WINDOWHEIGHT_EDIT, OnKillFocusSubwindowHeightEdit)

	ON_EN_SETFOCUS(IDC_SGRD_WINDOWLEFT_EDIT, OnSetFocusWindowLeftEdit)
	ON_EN_SETFOCUS(IDC_SGRD_WINDOWTOP_EDIT, OnSetFocusWindowTopEdit)
	ON_EN_SETFOCUS(IDC_SGRD_WINDOWWIDTH_EDIT, OnSetFocusWindowWidthEdit)
	ON_EN_SETFOCUS(IDC_SGRD_WINDOWHEIGHT_EDIT, OnSetFocusWindowHeightEdit)

	ON_BN_CLICKED(IDC_SGRD_CENTER_BUTTON, OnClickCenterButton)

	ON_CBN_SELCHANGE(IDC_SGRD_DECIMATION_COMBO, OnSelchangeDecimationCombo)
	ON_CBN_SELCHANGE(IDC_SGRD_DECIMATIONMODE_COMBO, OnSelchangeDecimationModeCombo)
	ON_CBN_SELCHANGE(IDC_SGRD_WINDOWSIZE_COMBO, OnSelchangeWindowsizeCombo)

	ON_UPDATE_COMMAND_UI(IDC_SGRD_DECIMATIONMODE_COMBO, OnUpdateDecimationModeCombo)

	ON_BN_CLICKED(IDC_SGRD_ROTATE0_RADIO, OnClickRotateNoneRadio)
	ON_BN_CLICKED(IDC_SGRD_ROTATE90_RADIO, OnClickRotate90Radio)
	ON_BN_CLICKED(IDC_SGRD_ROTATE180_RADIO, OnClickRotate180Radio)
	ON_BN_CLICKED(IDC_SGRD_ROTATE270_RADIO, OnClickRotate270Radio)

	ON_BN_CLICKED(IDC_SGRD_IMAGEFLIPHORIZONTALOFF_RADIO, OnClickHorizontalOff)
	ON_BN_CLICKED(IDC_SGRD_IMAGEFLIPHORIZONTALON_RADIO, OnClickHorizontalOn)
	ON_BN_CLICKED(IDC_SGRD_IMAGEFLIPVERTICALOFF_RADIO, OnClickVerticalOff)
	ON_BN_CLICKED(IDC_SGRD_IMAGEFLIPVERTICALON_RADIO, OnClickVerticalOn)

	ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CRoiDialog2, CDialog)
	ON_EVENT(CRoiDialog2, IDC_SGRD_SUBWINDOW_CTRL, 1, OnSubwindowChanged, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

// CRoiDialog2 message handlers


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CRoiDialog2::OnInitDialog()
{
	CDialog::OnInitDialog();

	// The PxLSubwindow control is able to semi-automatically exchange data
	// with up to four edit controls  - we just need to tell it which ones:
	m_SubwindowCtrl.SetEdits(&m_LeftEdit, &m_TopEdit, &m_WidthEdit, &m_HeightEdit);

	this->SetGUIState(GS_CameraSelected);
	this->ConfigureControls();
	this->PopulateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CRoiDialog2::OnKickIdle(void)
{
	this->UpdateDialogControls(this, FALSE);
}


CPxLCamera* CRoiDialog2::GetActiveCamera(void)
{
	return m_parent->GetActiveCamera();
}


void CRoiDialog2::OnApiError(PxLException const& e, bool cameraRelated)
{
	m_parent->OnApiError(e, cameraRelated);
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the feature support of
*           the camera.
*/
void CRoiDialog2::SetGUIState(eGUIState state)
{
	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

	bool enableRoi = enable && cam->FeatureSupportsManual(FEATURE_ROI); // Can be ROI or AUTO_ROI

    m_SubwindowCtrl.put_Enabled(enableRoi);
	m_TopEdit.EnableWindow(enableRoi);
	m_LeftEdit.EnableWindow(enableRoi);
	m_WidthEdit.EnableWindow(enableRoi);
	m_HeightEdit.EnableWindow(enableRoi);
	m_WindowSizeCombo.EnableWindow(enableRoi);
	m_CenterButton.EnableWindow(enableRoi);

	// The decimation control is only used in ROI mode (not in AUTO_ROI mode)
	m_DecimationCombo.EnableWindow(enable && cam->FeatureSupportsManual(FEATURE_PIXEL_ADDRESSING));
	m_DecimationModeCombo.EnableWindow(enable && cam->FeatureSupportsManual(FEATURE_PIXEL_ADDRESSING)
										&& cam->GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING) >= 2);

	bool enableRot = enable && cam->FeatureSupportsManual(FEATURE_ROTATE);
	m_RotateNoneRadio.EnableWindow(enableRot);
	m_Rotate90Radio.EnableWindow(enableRot);
	m_Rotate180Radio.EnableWindow(enableRot);
	m_Rotate270Radio.EnableWindow(enableRot);

	bool enableFlip = enable 
					&& cam->FeatureSupportsManual(FEATURE_FLIP)
					&& static_cast<int>(cam->GetFeatureMaxVal(FEATURE_FLIP, FEATURE_FLIP_PARAM_HORIZONTAL)) >= 1;
	m_ImageFlipHorizontalOffRadio.EnableWindow(enableFlip);
	m_ImageFlipHorizontalOnRadio.EnableWindow(enableFlip);

	enableFlip = enable 
					&& cam->FeatureSupportsManual(FEATURE_FLIP)
					&& static_cast<int>(cam->GetFeatureMaxVal(FEATURE_FLIP, FEATURE_FLIP_PARAM_VERTICAL)) >= 1;
	m_ImageFlipVerticalOffRadio.EnableWindow(enableFlip);
	m_ImageFlipVerticalOnRadio.EnableWindow(enableFlip);

}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera. This involves things like setting the max
*           and min values for sliders and setting the bounds of the subwindow
*           control.
*/
void CRoiDialog2::ConfigureControls(void)
{
	ConfigureRoi();
	ConfigureDecimation();
}


/**
* Function: ConfigureRoi
* Purpose:  
*/
void CRoiDialog2::ConfigureRoi(void)
{
	if (!GetActiveCamera()->FeatureSupported(FEATURE_ROI))
		return;

	try
	{
		long maxWidth = static_cast<long>(
			GetActiveCamera()->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_WIDTH));
		long maxHeight = static_cast<long>(
			GetActiveCamera()->GetFeatureMaxVal(FEATURE_ROI, FEATURE_ROI_PARAM_HEIGHT));

		// We want the SubwindowControl to work in an intuitive way, even in the
		// presence of Flip and Rotation.
		bool sideways = GetActiveCamera()->FeatureSupported(FEATURE_ROTATE)
					&& static_cast<int>(GetActiveCamera()->GetFeatureValue(FEATURE_ROTATE)) % 180 != 0;
		if (sideways)
			std::swap(maxWidth, maxHeight);

		TempVal<bool> temp(m_bAdjusting[FEATURE_ROI], true); // SetBounds fires a SubwindowChanged event.

		m_SubwindowCtrl.SetBounds(maxWidth, maxHeight);

		long minWidth = static_cast<long>(
			GetActiveCamera()->GetFeatureMinVal(FEATURE_ROI, FEATURE_ROI_PARAM_WIDTH));
		long minHeight = static_cast<long>(
			GetActiveCamera()->GetFeatureMinVal(FEATURE_ROI, FEATURE_ROI_PARAM_HEIGHT));

		if (sideways)
			std::swap(minWidth, minHeight);

		//minWidth = max(minWidth, 24);
		//minHeight = max(minHeight, 24);
		m_SubwindowCtrl.put_MinWidth(minWidth);
		m_SubwindowCtrl.put_MinHeight(minHeight);

		// Limit the list of standard subwindow sizes to those that are
		// within the capabilities of the camera.
		SetWindowSizeEntries(
			m_WindowSizeCombo,
			static_cast<int>(minWidth),
			static_cast<int>(minHeight),
			static_cast<int>(maxWidth), 
			static_cast<int>(maxHeight));
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: ConfigureDecimation
* Purpose:  
*/
void CRoiDialog2::ConfigureDecimation(void)
{
	try
	{
		SetDecimationModeEntries(m_DecimationModeCombo, GetActiveCamera()->GetSupportedDecimationModeValues());
		SetDecimationEntries(m_DecimationCombo, 
		                     GetActiveCamera()->GetSupportedXDecimationValues(),
		                     GetActiveCamera()->GetSupportedYDecimationValues(),
		                     GetActiveCamera()->SupportsAsymmetricPixelAddressing());
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CRoiDialog2::PopulateControls(void)
{
	PopulateSubwindow();
	PopulateDecimation();
	m_parent->PopulateWindowSize(m_WindowSizeCombo);
	PopulateRotation();
	PopulateFlip();
}


/**
* Function: PopulateSubwindow
* Purpose:  Set the subwindow control to reflect the current value of the ROI
*           in the active camera.
*/
void CRoiDialog2::PopulateSubwindow(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_ROI))
	{
		try
		{
			ConfigureRoi();

			CRect r = GetActiveCamera()->GetRoi();

			// Set the subwindow control to display the correct subwindow.
			TempVal<bool> temp(m_bAdjusting[FEATURE_ROI], true); // SetSubwindow *always* emits an OnChange event.
			m_SubwindowCtrl.SetSubwindow(
									r.left,
									r.top,
									r.right - r.left,		// width
									r.bottom - r.top);		// height
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateDecimation
* Purpose:  Set the decimation radio buttons to reflect the state of the 
*           active camera.
*/
void CRoiDialog2::PopulateDecimation(void)
{    
	if (GetActiveCamera()->FeatureSupported(FEATURE_PIXEL_ADDRESSING))
	{
		try
		{
			PixelAddressing pa = GetActiveCamera()->GetPixelAddressing();
    	    std::vector<int> const& xPas = GetActiveCamera()->GetSupportedXDecimationValues();
     		int x,y;
			TempVal<bool> temp(m_bAdjusting[FEATURE_PIXEL_ADDRESSING], true);
			
			// Step 1
			//      Set Value selector....
			if (pa.supportAsymmetry) 
			{
       	        std::vector<int> const& yPas = GetActiveCamera()->GetSupportedYDecimationValues();
       	        // covert the x and y pa factor, into offsets into the supported pa vectors
       	        for (y=0; y<yPas.size(); y++)
       	            if (yPas[y] == pa.y) break;
       	        for (x=0; x<xPas.size(); x++)
       	            if (xPas[x] == pa.x) break;
    			SelectByItemData(m_DecimationCombo, x*yPas.size() + y + 1);
			} else {
      	        // covert the x pa factor, into offsets into the supported pa vector
       	        for (x=0; x<xPas.size(); x++)
       	            if (xPas[x] == pa.x) break;
				SelectByItemData(m_DecimationCombo, x+1);
		    }
			
			// Step 2
			//      Set Mode selector...
			// If the camera knows about Decimation Mode, populate the combo box:
			if (GetActiveCamera()->GetFeatureParamCount(FEATURE_PIXEL_ADDRESSING) >= 2)
			{
				SelectByItemData(m_DecimationModeCombo, pa.Mode);
			}
			else
			{
				m_DecimationModeCombo.SetCurSel(0);
			}
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateRotation
* Purpose:  
*/
void CRoiDialog2::PopulateRotation(void)
{
	if (GetActiveCamera()->FeatureSupported(FEATURE_ROTATE))
	{
		try
		{
			U32 flags = 0;
			float rot = GetActiveCamera()->GetFeatureValue(FEATURE_ROTATE, &flags);

			TempVal<bool> _temp(m_bAdjusting[FEATURE_ROTATE], true);

			m_RotateNoneRadio.SetCheck(BST_UNCHECKED);
			m_Rotate90Radio.SetCheck(BST_UNCHECKED);
			m_Rotate180Radio.SetCheck(BST_UNCHECKED);
			m_Rotate270Radio.SetCheck(BST_UNCHECKED);

			if (rot == 0.0f || (flags & FEATURE_FLAG_OFF) != 0)
				m_RotateNoneRadio.SetCheck(BST_CHECKED);
			else if (rot == 90.0f)
				m_Rotate90Radio.SetCheck(BST_CHECKED);
			else if (rot == 180.0f)
				m_Rotate180Radio.SetCheck(BST_CHECKED);
			else if (rot == 270.0f)
				m_Rotate270Radio.SetCheck(BST_CHECKED);
			// else ???
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: PopulateFlip
* Purpose:  
*/
void CRoiDialog2::PopulateFlip(void)
{
	try
	{
		if (GetActiveCamera()->FeatureSupported(FEATURE_FLIP))
		{
			bool hFlipOn = GetActiveCamera()->GetHorizontalFlip();
			m_ImageFlipHorizontalOnRadio.SetCheck(hFlipOn);
			m_ImageFlipHorizontalOffRadio.SetCheck(!hFlipOn);

			bool vFlipOn = GetActiveCamera()->GetVerticalFlip();
			m_ImageFlipVerticalOnRadio.SetCheck(vFlipOn);
			m_ImageFlipVerticalOffRadio.SetCheck(!vFlipOn);
		}
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: FeatureChanged
* Purpose:  
*/
void CRoiDialog2::FeatureChanged(U32 featureId)
{
	switch (featureId)
	{
	case FEATURE_ROI:
		PopulateSubwindow();
		m_parent->PopulateWindowSize(m_WindowSizeCombo);
		break;
	case FEATURE_PIXEL_ADDRESSING:
		PopulateDecimation();
		break;
	case FEATURE_ROTATE:
		PopulateRotation();
		PopulateSubwindow();
		m_parent->PopulateWindowSize(m_WindowSizeCombo);
		break;
	case FEATURE_FLIP:
		PopulateFlip();
		PopulateSubwindow();
		m_parent->PopulateWindowSize(m_WindowSizeCombo);
		break;
	case FEATURE_SPECIAL_CAMERA_MODE:
	    ConfigureRoi();
	    m_parent->PopulateWindowSize(m_WindowSizeCombo);
	    break;
	default:
		// A feature that is not relevant to this page. Ignore the change.
		break;
	}
}

/******************************************************************************
 * CRoiDialog2 message handlers
 *****************************************************************************/

/**
* Function: OnSubwindowChanged
* Purpose:  Event handler for the SubwindowChanged event, which is emitted by
*           the PxLSubwindow control at the completion of a move or resize
*           operation.
*/
void CRoiDialog2::OnSubwindowChanged(long left, long top, long width, long height)
{
	// Make sure the edits are set properly (they should be - OnSubwindowChanging updates them)
	m_SubwindowCtrl.UpdateEdits(left, top, width, height);

	// Update the camera.
	if (!m_bAdjusting[FEATURE_ROI])
	{
		try
		{
			GetActiveCamera()->SetRoi(left, top, width, height,
										theApp.m_bAutoResetPreview);
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}
}


/**
* Function: OnKillFocusSubwindowTopEdit
* Purpose:  
*/
void CRoiDialog2::OnKillFocusSubwindowTopEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_TOP);
}


/**
* Function: OnKillFocusSubwindowLeftEdit
* Purpose:  
*/
void CRoiDialog2::OnKillFocusSubwindowLeftEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_LEFT);
}


/**
* Function: OnKillFocusSubwindowWidthEdit
* Purpose:  
*/
void CRoiDialog2::OnKillFocusSubwindowWidthEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_WIDTH);
}


/**
* Function: OnKillFocusSubwindowHeightEdit
* Purpose:  
*/
void CRoiDialog2::OnKillFocusSubwindowHeightEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_HEIGHT);
}


void CRoiDialog2::OnSetFocusWindowLeftEdit()
{
	m_LeftEdit.SetSel(0, 99);
}

void CRoiDialog2::OnSetFocusWindowTopEdit()
{
	m_TopEdit.SetSel(0, 99);
}

void CRoiDialog2::OnSetFocusWindowWidthEdit()
{
	m_WidthEdit.SetSel(0, 99);
}

void CRoiDialog2::OnSetFocusWindowHeightEdit()
{
	m_HeightEdit.SetSel(0, 99);
}


/**
* Function: OnClickCenterButton
* Purpose:  Center the Region of Interest in the available space, without
*           changing its size.
*/
void CRoiDialog2::OnClickCenterButton(void)
{
	CWaitCursor wc;

	long dummy, width, height, maxwidth, maxheight;

	m_SubwindowCtrl.GetSubwindow(&dummy, &dummy, &width, &height);
	m_SubwindowCtrl.GetBounds(&maxwidth, &maxheight);

	if (width < maxwidth || height < maxheight)
	{
		m_SubwindowCtrl.SetSubwindow((maxwidth - width)/2,
									 (maxheight - height)/2,
									 width, height);
	}
}


/**
* Function: OnSelchangeDecimationModeCombo
* Purpose:  
*/
void CRoiDialog2::OnSelchangeDecimationModeCombo(void)
{
	if (m_DecimationModeCombo.GetCurSel() == -1)
		return;

	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_PIXEL_ADDRESSING,
										   ItemData<float>(m_DecimationModeCombo),
										   FEATURE_PIXEL_ADDRESSING_PARAM_MODE,
										   theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnSelchangeDecimationCombo
* Purpose:  
*/
void CRoiDialog2::OnSelchangeDecimationCombo(void)
{
	if (m_DecimationCombo.GetCurSel() == -1)
		return;

	try
	{
	    PixelAddressing pa = GetActiveCamera()->GetPixelAddressing();
	    int selector = ItemData<int>(m_DecimationCombo);
	    std::vector<int> const& xPas = GetActiveCamera()->GetSupportedXDecimationValues();
	    
	    if (pa.supportAsymmetry) 
	    {
    	    std::vector<int> const& yPas = GetActiveCamera()->GetSupportedYDecimationValues();
    	    int yOffset = ((selector-1) / yPas.size());
            pa.x = xPas[yOffset];
            pa.y = yPas[(selector-1) - (yOffset * yPas.size())];
	    } else {
	        pa.x = pa.y = xPas[selector-1];
	    }
		GetActiveCamera()->SetPixelAddressing(pa, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnSelchangeWindowsizeCombo
* Purpose:  Event handler for the Change event of the "Standard Sizes" combo
*           box. Sets the PxLSubwindow control to the specified size.
*/
void CRoiDialog2::OnSelchangeWindowsizeCombo(void)
{
	m_parent->WindowsizeComboChanged(m_WindowSizeCombo);
}


/**
* Function: OnClickRotateNoneRadio
* Purpose:  
*/
void CRoiDialog2::OnClickRotateNoneRadio()
{
	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, 0.0f, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate90Radio
* Purpose:  
*/
void CRoiDialog2::OnClickRotate90Radio()
{
	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, 90.0f, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate180Radio
* Purpose:  
*/
void CRoiDialog2::OnClickRotate180Radio()
{
	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, 180.0f, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickRotate270Radio
* Purpose:  
*/
void CRoiDialog2::OnClickRotate270Radio()
{
	try
	{
		GetActiveCamera()->SetFeatureValue(FEATURE_ROTATE, 270.0f, 0, theApp.m_bAutoResetPreview);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickHorizontalOff
* Purpose:  
*/
void CRoiDialog2::OnClickHorizontalOff(void)
{
	try
	{
		GetActiveCamera()->SetHorizontalFlip(false);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickHorizontalOn
* Purpose:  
*/
void CRoiDialog2::OnClickHorizontalOn(void)
{
	try
	{
		GetActiveCamera()->SetHorizontalFlip(true);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickVerticalOff
* Purpose:  
*/
void CRoiDialog2::OnClickVerticalOff(void)
{
	try
	{
		GetActiveCamera()->SetVerticalFlip(false);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickVerticalOn
* Purpose:  
*/
void CRoiDialog2::OnClickVerticalOn(void)
{
	try
	{
		GetActiveCamera()->SetVerticalFlip(true);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnUpdateDecimationModeCombo
* Purpose:  
*/
void CRoiDialog2::OnUpdateDecimationModeCombo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_DecimationCombo.GetCurSel() > 0);
}




































