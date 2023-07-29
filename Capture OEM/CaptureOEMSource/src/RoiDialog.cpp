// RoiDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "RoiDialog.h"


/******************************************************************************
* CRoiDialog dialog
******************************************************************************/

IMPLEMENT_DYNAMIC(CRoiDialog, CFeatureDialog)


BEGIN_MESSAGE_MAP(CRoiDialog, CFeatureDialog)

	ON_EN_KILLFOCUS(IDC_RD_WINDOWTOP_EDIT, OnKillFocusSubwindowTopEdit)
	ON_EN_KILLFOCUS(IDC_RD_WINDOWLEFT_EDIT, OnKillFocusSubwindowLeftEdit)
	ON_EN_KILLFOCUS(IDC_RD_WINDOWWIDTH_EDIT, OnKillFocusSubwindowWidthEdit)
	ON_EN_KILLFOCUS(IDC_RD_WINDOWHEIGHT_EDIT, OnKillFocusSubwindowHeightEdit)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CRoiDialog, CFeatureDialog)
	ON_EVENT(CRoiDialog, IDC_RD_SUBWINDOW_CTRL, 1, OnSubwindowChanged, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()


/**
* Function: CRoiDialog
* Purpose:  Constructor
*/
CRoiDialog::CRoiDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CRoiDialog::IDD, pParent)
{
}


/**
* Function: ~CRoiDialog
* Purpose:  Destructor
*/
CRoiDialog::~CRoiDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CRoiDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RD_SUBWINDOW_CTRL, m_SubwindowCtrl);
	DDX_Control(pDX, IDC_RD_WINDOWTOP_EDIT, m_TopEdit);
	DDX_Control(pDX, IDC_RD_WINDOWLEFT_EDIT, m_LeftEdit);
	DDX_Control(pDX, IDC_RD_WINDOWHEIGHT_EDIT, m_HeightEdit);
	DDX_Control(pDX, IDC_RD_WINDOWWIDTH_EDIT, m_WidthEdit);
}


/**
* Function: Configure
* Purpose:  
*/
void CRoiDialog::Configure(void)
{
	// The PxLSubwindow control is able to semi-automatically exchange data
	// with up to four edit controls  - we just need to tell it which ones:
	m_SubwindowCtrl.SetEdits(&m_LeftEdit, &m_TopEdit, &m_WidthEdit, &m_HeightEdit);

	try
	{
		long maxWidth = static_cast<long>(
			m_pCam->GetFeatureMaxVal(m_featureId, FEATURE_ROI_PARAM_WIDTH));
		long maxHeight = static_cast<long>(
			m_pCam->GetFeatureMaxVal(m_featureId, FEATURE_ROI_PARAM_HEIGHT));

		// We want the SubwindowControl to work in an intuitive way, even in the
		// presence of Flip and Rotation.
		bool sideways = m_pCam->FeatureSupported(FEATURE_ROTATE)
					&& static_cast<int>(m_pCam->GetFeatureValue(FEATURE_ROTATE)) % 180 != 0;
		if (sideways)
			std::swap(maxWidth, maxHeight);

		TempVal<bool> temp(m_bAdjusting, true); // SetBounds fires a SubwindowChanged event.
		m_SubwindowCtrl.SetBounds(maxWidth, maxHeight);

		long minWidth = static_cast<long>(
			m_pCam->GetFeatureMinVal(m_featureId, FEATURE_ROI_PARAM_WIDTH));
		long minHeight = static_cast<long>(
			m_pCam->GetFeatureMinVal(m_featureId, FEATURE_ROI_PARAM_HEIGHT));

		if (sideways)
			std::swap(minWidth, minHeight);

		//minWidth = max(minWidth, 24);
		//minHeight = max(minHeight, 24);
		m_SubwindowCtrl.put_MinWidth(minWidth);
		m_SubwindowCtrl.put_MinHeight(minHeight);

	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: Populate
* Purpose:  
*/
void CRoiDialog::Populate(void)
{
	ASSERT(m_featureId == FEATURE_ROI || m_featureId == FEATURE_AUTO_ROI);

	CRect r = m_pCam->GetRoi(m_featureId);

	// Set the subwindow control to display the correct subwindow.
	TempVal<bool> temp(m_bAdjusting, true); // SetSubwindow *always* emits an OnChange event.
	m_SubwindowCtrl.SetSubwindow(r.left,
								 r.top,
								 r.right - r.left,		// width
								 r.bottom - r.top);		// height

	// Make sure the edit boxes are displaying the values too.
	m_SubwindowCtrl.UpdateEdits();
}


/**
* Function: OnSubwindowChanged
* Purpose:  
*/
void CRoiDialog::OnSubwindowChanged(long left, long top, long width, long height)
{
	// Make sure the edits are set properly (they should be - OnSubwindowChanging updates them)
	m_SubwindowCtrl.UpdateEdits(left, top, width, height);

	if (m_bAdjusting)
		return;

	// Update the camera.
	try
	{
		m_pCam->SetRoi(left, top, width, height, theApp.m_bAutoResetPreview, m_featureId);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnKillFocusSubwindowTopEdit
* Purpose:  
*/
void CRoiDialog::OnKillFocusSubwindowTopEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_TOP);
}


/**
* Function: OnKillFocusSubwindowLeftEdit
* Purpose:  
*/
void CRoiDialog::OnKillFocusSubwindowLeftEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_LEFT);
}


/**
* Function: OnKillFocusSubwindowWidthEdit
* Purpose:  
*/
void CRoiDialog::OnKillFocusSubwindowWidthEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_WIDTH);
}


/**
* Function: OnKillFocusSubwindowHeightEdit
* Purpose:  
*/
void CRoiDialog::OnKillFocusSubwindowHeightEdit()
{
	m_SubwindowCtrl.UpdateFromEdits(CSubwindowCtrl::WND_HEIGHT);
}













