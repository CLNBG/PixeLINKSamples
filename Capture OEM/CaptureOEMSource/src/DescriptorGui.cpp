// DescriptorGui.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "DescriptorGui.h"
#include <afxpriv.h>			// To get UPDATE_COMMAND_UI support using WM_KICKIDLE


#pragma warning(disable:4355) //'this' : used in base member initializer list.


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDescriptorGui property page

IMPLEMENT_DYNCREATE(CDescriptorGui, CDevAppPage)

CDescriptorGui::CDescriptorGui(CPxLDevAppView* pView) 
	: CDevAppPage(CDescriptorGui::IDD, pView)
	, m_DescriptorDlg(this)
	, m_DescriptorEditScrollBox(m_DescriptorDlg)
	, m_hCameraNode(NULL)
	, m_hInfoNode(NULL)
	, m_hDescRootNode(NULL)
{
}

CDescriptorGui::~CDescriptorGui()
{
}

void CDescriptorGui::DoDataExchange(CDataExchange* pDX)
{
	CDevAppPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DG_CAMERASELECT_COMBO, m_CameraSelectCombo);
	DDX_Control(pDX, IDC_DG_PLAY_BUTTON, m_PlayButton);
	DDX_Control(pDX, IDC_DG_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_DG_STOP_BUTTON, m_StopButton);

	DDX_Control(pDX, IDC_DG_CAMERA_TREE, m_CameraTree);

	DDX_Control(pDX, IDC_DG_CREATEDESCRIPTOR_BUTTON, m_CreateDescriptorButton);
	DDX_Control(pDX, IDC_DG_DELETEDESCRIPTOR_BUTTON, m_DeleteDescriptorButton);
	DDX_Control(pDX, IDC_DG_DELETEALL_BUTTON, m_DeleteAllButton);

	DDX_Control(pDX, IDC_DG_UPDATECAMERA_RADIO, m_UpdateModeCameraRadio);
	DDX_Control(pDX, IDC_DG_UPDATEHOST_RADIO, m_UpdateModeHostRadio);
	DDX_Control(pDX, IDC_DG_UPDATENOW_BUTTON, m_UpdateNowButton);

	DDX_Control(pDX, IDC_DESCRIPTOREDIT_SCROLLBOX, m_DescriptorEditScrollBox);

	DDX_Control(pDX, IDC_DESCRIPTORINFOCUS_LABEL, m_DescriptorInFocusLabel);
}


BEGIN_MESSAGE_MAP(CDescriptorGui, CDevAppPage)

	ON_CBN_DROPDOWN(IDC_DG_CAMERASELECT_COMBO, OnDropdownCameraSelectCombo)
	ON_CBN_SELCHANGE(IDC_DG_CAMERASELECT_COMBO, OnSelchangeCameraSelectCombo)
	ON_BN_CLICKED(IDC_DG_PLAY_BUTTON, OnClickPlayButton)
	ON_BN_CLICKED(IDC_DG_PAUSE_BUTTON, OnClickPauseButton)
	ON_BN_CLICKED(IDC_DG_STOP_BUTTON, OnClickStopButton)

	ON_BN_CLICKED(IDC_DG_UPDATENOW_BUTTON, OnClickUpdateNowButton)

	ON_BN_CLICKED(IDC_DG_CREATEDESCRIPTOR_BUTTON, OnClickCreateDescriptorButton)
	ON_BN_CLICKED(IDC_DG_DELETEDESCRIPTOR_BUTTON, OnClickDeleteDescriptorButton)
	ON_BN_CLICKED(IDC_DG_DELETEALL_BUTTON, OnClickDeleteAllButton)

	ON_BN_CLICKED(IDC_DG_UPDATECAMERA_RADIO, OnClickUpdateCameraRadio)
	ON_BN_CLICKED(IDC_DG_UPDATEHOST_RADIO, OnClickUpdateHostRadio)

	// UPDATE_COMMAND_UI handlers
	ON_UPDATE_COMMAND_UI(IDC_DG_DELETEDESCRIPTOR_BUTTON, OnUpdateDeleteDescriptorButton)
	ON_UPDATE_COMMAND_UI(IDC_DG_DELETEALL_BUTTON, OnUpdateDeleteAllButton)
	ON_UPDATE_COMMAND_UI(IDC_DG_UPDATENOW_BUTTON, OnUpdateUpdateNowButton)

	ON_NOTIFY(TVN_SELCHANGED, IDC_DG_CAMERA_TREE, OnSelchangedCameraTree)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDescriptorGui message handlers

int CDescriptorGui::GetHelpContextId()
{
	return IDH_DESCRIPTORS_TAB;
}


/**
* Function: OnInitDialog
* Purpose:  
*/
BOOL CDescriptorGui::OnInitDialog(void)
{
	CDevAppPage::OnInitDialog();

	// Put the embedded Descriptor-Edit dialog into the scrolling window.
	EmbedDescriptorDialog();
	m_DescriptorDlg.EnableControls(false); // until a descriptor is selected...

	return TRUE;
}


/**
* Function: EmbedDescriptorDialog
* Purpose:  Put the Descriptor Edit dialog inside the scrolling box.
*/
void CDescriptorGui::EmbedDescriptorDialog(void)
{
	m_DescriptorEditScrollBox.ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	m_DescriptorDlg.Create(IDD_DESCRIPTOR_EDIT, &m_DescriptorEditScrollBox);

	m_DescriptorEditScrollBox.ModifyStyleEx(0, WS_VSCROLL);

	CRect rctSB, rctChild;
	m_DescriptorEditScrollBox.GetClientRect(&rctSB);
	m_DescriptorDlg.GetWindowRect(&rctChild);

	SCROLLINFO	si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nPage = rctSB.Height();
	si.nMax = rctChild.Height();
	si.nMin = 0;
	m_DescriptorEditScrollBox.SetScrollInfo(SB_VERT, &si);

	m_DescriptorEditScrollBox.ShowScrollBar(SB_VERT);
	m_DescriptorEditScrollBox.EnableScrollBar(SB_VERT);
}


/**
* Function: OnKillActive
* Purpose:  For the purposes of this application, it was decided that we should
*           force the active descriptor (if there is one) into UPDATE_CAMERA
*           mode when leaving the Descriptors tab.
*           If we did not do this, then it would be possible to get into a
*           situation where all the GetFeature / SetFeature calls made by other
*           parts of the application are reading from and writing to a
*           descriptor in Host mode, and so the feature values displayed in
*           the GUI would not reflect the state of the *camera*. The potential
*           for confusion is just too great, so we don't allow this sitution
*           to occur.
*/
BOOL CDescriptorGui::OnKillActive()
{
	try
	{
		GetActiveCamera()->SetDescriptorUpdateMode(PXL_UPDATE_CAMERA);
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}

	return CDevAppPage::OnKillActive();
}


/**
* Function: SetGUIState
* Purpose:  Enable / Disable controls, according to the state of the camera.
*           Most controls cannot be used in any meaningful way until a camera
*           is initialized.
*/
void CDescriptorGui::SetGUIState(eGUIState state)
{
	if (state == GS_NoCamera)
		m_DescriptorDlg.KillPopup();

	// The CameraSelect and Play/Pause/Stop button states are set in the base
	// class version of this function.
	CDevAppPage::SetGUIState(state);

	CPxLCamera* cam = GetActiveCamera();

	bool enable = (state != GS_NoCamera);

	m_CameraTree.EnableWindow(enable);
	m_CreateDescriptorButton.EnableWindow(enable);
	m_UpdateModeCameraRadio.EnableWindow(enable);
	m_UpdateModeHostRadio.EnableWindow(enable);

	if (!enable)
	{
		m_DescriptorDlg.ClearControls();
		m_DescriptorDlg.EnableControls(false);
	}
	// else: Don't enable until a descriptor is selected or created.

	// Many of the controls on this page are enabled or disabled in 
	// UPDATE_COMMAND_UI handlers. This is because their state depends more
	// on what is selected elsewhere - specifically, in the tree control - than
	// on the state of the active camera or the preview.

}


/**
* Function: ConfigureControls
* Purpose:  Set up the controls on the page to customize them according to the
*           currently active camera.
*/
void CDescriptorGui::ConfigureControls(void)
{
	// This is called just after the camera has just been selected.

	// Populate the Tree control with:
	//   a) Root entry ("PixeLINK Camera s/n:<serial number>") - same as in
	//      the Camera Select combo box.
	//   b) Info node, with children for each item in the CAMERA_INFO struct.
	//   c) Descriptors node, with either "No Descriptors Created" leaf-node,
	//      or "Descriptor 1", "Descriptor 2", ...etc.

	m_CameraTree.DeleteAllItems();

	CString s;
	s.Format(_T("%s: %lu"), GetActiveCamera()->GetCameraInfo()->CameraName, GetActiveCamera()->GetSerialNumber());
	m_hCameraNode = m_CameraTree.InsertItem(s);

	m_hInfoNode = m_CameraTree.InsertItem(_T("Camera Info"), m_hCameraNode);


	CAMERA_INFO const* pInfo = GetActiveCamera()->GetCameraInfo();

	s.Format(_T("Camera Name : %s"), pInfo->CameraName);
	m_CameraTree.InsertItem(s, m_hInfoNode);

	s.Format(_T("Serial Number : %s"), pInfo->SerialNumber);
	m_CameraTree.InsertItem(s, m_hInfoNode);

	s.Format(_T("Model Name : %s"), pInfo->ModelName);
	m_CameraTree.InsertItem(s, m_hInfoNode);

	s.Format(_T("Vendor Name : %s"), pInfo->VendorName);
	m_CameraTree.InsertItem(s, m_hInfoNode);

	s.Format(_T("Description : %s"), pInfo->Description);
	m_CameraTree.InsertItem(s, m_hInfoNode);

	s.Format(_T("Firmware Version : %s"), pInfo->FirmwareVersion);
	m_CameraTree.InsertItem(s, m_hInfoNode);

	s.Format(_T("FPGA Version : %s"), pInfo->FPGAVersion);
	m_CameraTree.InsertItem(s, m_hInfoNode);
	
	// Add the XML version, if the camera has one.  we can determine this by looking
	// for non-NULL for the XMLVersion filed.
	if (pInfo->XMLVersion[0] != 0)
	{
    	s.Format(_T("XML Version : %s"), pInfo->XMLVersion);
	    m_CameraTree.InsertItem(s, m_hInfoNode);
	}

	// Add the BOOT Version, if the camera has one.  we can determine this by looking
	// for non-NULL for the BootVersion filed.
	if (pInfo->BootloadVersion[0] != 0)
	{
    	s.Format(_T("Bootload Version : %s"), pInfo->BootloadVersion);
	    m_CameraTree.InsertItem(s, m_hInfoNode);
	}

	// Add the Lens Descriptor, if the camera has one.  we can determine this by looking
	// for non-NULL for the Lens Descriptor.
	if (pInfo->LensDescription[0] != 0)
	{
    	s.Format(_T("Lens : %s"), pInfo->LensDescription);
	    m_CameraTree.InsertItem(s, m_hInfoNode);
	}

	m_hCameraIdInfoRootNode = m_CameraTree.InsertItem(_T("Camera Id Info"), m_hCameraNode);
	UpdateCameraIdInfoNode();


	m_hDescRootNode = m_CameraTree.InsertItem(_T("Descriptors"), m_hCameraNode);

	UpdateDescriptorNodes();

	// Show the root node and the Descriptors node expanded by default.
	m_CameraTree.Expand(m_hCameraNode, TVE_EXPAND);
	m_CameraTree.Expand(m_hDescRootNode, TVE_EXPAND);

	// So that the tree doesn't collapse when you first tab the focus to it:
	m_CameraTree.SetItemState(m_hCameraNode, TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);

}


/**
* Function: PopulateDefaults
* Purpose:  Put some meaningles default values into the controls, for purely
*           aesthetic reasons.
*/
void CDescriptorGui::PopulateDefaults(void)
{
	m_CameraTree.DeleteAllItems();
	m_CameraTree.InsertItem(_T("No Camera Selected ---"));
	m_UpdateModeCameraRadio.SetCheck(BST_CHECKED);
	m_UpdateModeCameraRadio.SetCheck(BST_UNCHECKED);
	//...
}


/**
* Function: PopulateControls
* Purpose:  Make the values displayed in the controls reflect the current 
*           settings of the active camera.  Called immediately after a 
*           camera is initialized.
*/
void CDescriptorGui::PopulateControls(void)
{
	CDevAppPage::PopulateControls();

	PopulateDescriptorControls();
}


/**
* Function: PopulateDescriptorControls
* Purpose:  Populate all the stuff that is Descriptor-related. At the moment,
*           that's pretty much everything on this page. We break it out into
*           a separate function because it is called both from the general-
*           purpose PopulateControls method (eg: from OnSetFocus), and from
*           the descriptor-changing functions (eg: the OnClickCreateDescriptor
*           button handler).
*/
void CDescriptorGui::PopulateDescriptorControls(void)
{
	UpdateDescriptorNodes();

	bool hostMode = GetActiveCamera()->GetDescriptorUpdateMode() == PXL_UPDATE_HOST;
	m_UpdateModeCameraRadio.SetCheck(hostMode ? BST_UNCHECKED : BST_CHECKED);
	m_UpdateModeHostRadio.SetCheck(hostMode ? BST_CHECKED : BST_UNCHECKED);

	// If there is an active descriptor, display the values of its features.
	if (GetActiveCamera()->GetDescriptorCount() > 0)
	{
		m_DescriptorDlg.EnableControls(true);
		m_DescriptorDlg.PopulateControls();
	}
	else
	{
		m_DescriptorDlg.ClearControls();
		m_DescriptorDlg.EnableControls(false);
	}
}


/**
* Function: FeatureChanged
* Purpose:  This method is called by the base class (CDevAppPage) as a result
*           of a call to the View class's OnUpdate method (which is called by
*           the Document class as a result of a change to its data).
*/
void CDescriptorGui::FeatureChanged(U32 featureId)
{
	// If there is a Descriptor in focus, pass the notification on to the
	// embedded dialog, so that it can update the displayed value of the
	// specified feature.
	if (GetActiveCamera()->GetDescriptorCount() > 0)
		m_DescriptorDlg.FeatureChanged(featureId);
}


/**
* Function: OnClickUpdateCameraRadio
* Purpose:  Put the active Descriptor (if any) in to UPDATE_CAMERA mode.
*/
void CDescriptorGui::OnClickUpdateCameraRadio()
{
	try
	{
		GetActiveCamera()->SetDescriptorUpdateMode(PXL_UPDATE_CAMERA);
		//PopulateDescriptorControls(); // not needed, for now.
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickUpdateHostRadio
* Purpose:  Put the active Descriptor (if any) in to UPDATE_HOST mode.
*/
void CDescriptorGui::OnClickUpdateHostRadio()
{
	try
	{
		GetActiveCamera()->SetDescriptorUpdateMode(PXL_UPDATE_HOST);
		//PopulateDescriptorControls(); // not needed, for now.
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickUpdateNowButton
* Purpose:  Update the camera with any buffered changes that have been made
*           to the active descriptor while it has been in HOST mode.
*/
void CDescriptorGui::OnClickUpdateNowButton()
{
	try
	{
		// Set to CAMERA mode - sends all buffered changes to the camera...
		GetActiveCamera()->SetDescriptorUpdateMode(PXL_UPDATE_CAMERA);
		// Then go back into HOST mode.
		GetActiveCamera()->SetDescriptorUpdateMode(PXL_UPDATE_HOST);
		//PopulateDescriptorControls(); // not needed, for now.
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}

/**
* Function: UpdateCameraIdInfoNode
* Purpose:  Display the CAMERA_ID_INFO information in the tree
*/
void 
CDescriptorGui::UpdateCameraIdInfoNode()
{
	HTREEITEM hRoot = m_hCameraIdInfoRootNode;
	CString s;
	const CAMERA_ID_INFO* pInfo = GetActiveCamera()->GetCameraIdInfo();
	ASSERT(NULL != pInfo);

	s.Format("Struct Size: %d", pInfo->StructSize);
	m_CameraTree.InsertItem(s,hRoot);

	s.Format("Serial Number: %d", pInfo->CameraSerialNum);
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("MAC Address: %s", GetMacAddressAsString(pInfo->CameraMac));
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("IP Address: %s", GetIPAddressAsString(pInfo->CameraIpAddress));
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("IP Mask: %s", GetIPAddressAsString(pInfo->CameraIpMask));
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("IP Gateway: %s", GetIPAddressAsString(pInfo->CameraIpGateway));
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("Nic IP Address: %s", GetIPAddressAsString(pInfo->NicIpAddress));
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("Nic IP Mask: %s", GetIPAddressAsString(pInfo->NicIpMask));
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("Nic Access Mode: %d", pInfo->NicAccessMode);
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("Camera IP Assignment Type: %d", pInfo->CameraIpAssignmentType);
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("IP Engine Load Version: %d.%d.%d ", pInfo->IpEngineLoadVersionMajor, pInfo->IpEngineLoadVersionMinor, pInfo->IpEngineLoadVersionSubminor);
	m_CameraTree.InsertItem(s, hRoot);

	s.Format("CameraProperties: %d", pInfo->CameraProperties);
	m_CameraTree.InsertItem(s, hRoot);

    s.Format("Controlling IP Address: %s", GetIPAddressAsString(pInfo->ControllingIpAddress));
	m_CameraTree.InsertItem(s, hRoot);

    s.Format("Camera Link Speed (mb/s): %d", pInfo->CameraLinkSpeed);
	m_CameraTree.InsertItem(s, hRoot);
}


/**
* Function: UpdateDescriptorNodes
* Purpose:  Make the "Descriptors" part of the tree control reflect the
*           actual number of descriptors that exist, as stored by the active
*           CPxLCamera object.
*/
void CDescriptorGui::UpdateDescriptorNodes()
{
	int nDesc = GetActiveCamera()->GetDescriptorCount();
	int activeDesc = GetActiveCamera()->GetInFocusDescriptorNumber();

	HTREEITEM hDescNode = m_CameraTree.GetChildItem(m_hDescRootNode);
	if (hDescNode == NULL)
	{
	// There are no nodes under the descriptor root node. Add one.
		hDescNode = m_CameraTree.InsertItem(_T("???"), m_hDescRootNode);
	}

	// Set the properties of the first item.
	if (nDesc == 0)
	{
		m_CameraTree.SetItemText(hDescNode, _T("No Descriptors Defined"));
		m_CameraTree.SetItemData(hDescNode, 0);
		// Make sure it's NOT bold.
		m_CameraTree.SetItemState(hDescNode, 0, TVIS_BOLD);
	}
	else
	{
		m_CameraTree.SetItemText(hDescNode, _T("Descriptor # 1"));
		m_CameraTree.SetItemData(hDescNode, 1);
		// Make sure it's bold only if it's in focus.
		m_CameraTree.SetItemState(hDescNode, activeDesc == 1 ? TVIS_BOLD | TVIS_SELECTED : 0, TVIS_BOLD | TVIS_SELECTED);
	}
	
	// Now hDescNode refers to the first Descriptor node.
	CString s;
	for (int i = 2; i <= nDesc; i++)
	{
		hDescNode = m_CameraTree.GetNextSiblingItem(hDescNode);
		s.Format(_T("Descriptor # %d"), i);
		if (hDescNode == NULL)
		{
			hDescNode = m_CameraTree.InsertItem(s, m_hDescRootNode, TVI_LAST);
			m_CameraTree.SetItemData(hDescNode, i);
		}
		else
		{
			// Text and ItemData should already be correct...
			ASSERT(m_CameraTree.GetItemText(hDescNode) == s);
			ASSERT(m_CameraTree.GetItemData(hDescNode) == i);
		}
		// Make sure it's bold only if it's in focus.
		m_CameraTree.SetItemState(hDescNode, i == activeDesc ? TVIS_BOLD | TVIS_SELECTED : 0, TVIS_BOLD | TVIS_SELECTED);
	}

	// Remove any extra nodes.
	HTREEITEM hExtraNode;
	while ((hExtraNode = m_CameraTree.GetNextSiblingItem(hDescNode)) != NULL)
	{
		m_CameraTree.DeleteItem(hExtraNode);
	}

	if (activeDesc > 0)
		s.Format(_T("%d"), activeDesc);
	else
		s = _T("None");
	m_DescriptorInFocusLabel.SetWindowText(s);
}


/**
* Function: OnClickCreateDescriptorButton
* Purpose:  Create a new Descriptor.
*/
void CDescriptorGui::OnClickCreateDescriptorButton()
{
	CWaitCursor wc;

	try
	{
		GetActiveCamera()->CreateDescriptor(m_UpdateModeCameraRadio.GetCheck() == BST_CHECKED ? PXL_UPDATE_CAMERA : PXL_UPDATE_HOST);
		PopulateDescriptorControls();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickDeleteDescriptorButton
* Purpose:  Remove the active Descriptor.
*/
void CDescriptorGui::OnClickDeleteDescriptorButton()
{
	CWaitCursor wc;

	try
	{
		// Remove the active descriptor.
		GetActiveCamera()->RemoveDescriptor();
		PopulateDescriptorControls();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnClickDeleteAllButton
* Purpose:  
*/
void CDescriptorGui::OnClickDeleteAllButton()
{
	CWaitCursor wc;

	try
	{
		GetActiveCamera()->DeleteAllDescriptors();
		PopulateDescriptorControls();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: OnSelchangedCameraTree
* Purpose:  
*/
void CDescriptorGui::OnSelchangedCameraTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// We set the lParam of the TVITEM to the 1-based index of the descriptor
	// for the current camera. All other nodes in the tree have an lParam of 0.
	int descriptorNumber = pNMTreeView->itemNew.lParam;
	if (descriptorNumber > 0)
	{
		// A descriptor node was selected.
		try
		{
			CWaitCursor wc;

			GetActiveCamera()->SetActiveDescriptor(descriptorNumber);
			PopulateDescriptorControls();
		}
		catch (PxLException const& e)
		{
			OnApiError(e);
		}
	}

	*pResult = 0;
}


/**
* Function: OnUpdateUpdateNowButton
* Purpose:  
*/
void CDescriptorGui::OnUpdateUpdateNowButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetActiveCamera()->GetDescriptorUpdateMode() == PXL_UPDATE_HOST);
}


/**
* Function: OnUpdateDeleteDescriptorButton
* Purpose:  
*/
void CDescriptorGui::OnUpdateDeleteDescriptorButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetActiveCamera()->GetDescriptorCount() > 0);
}


/**
* Function: OnUpdateDeleteAllButton
* Purpose:  
*/
void CDescriptorGui::OnUpdateDeleteAllButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetActiveCamera()->GetDescriptorCount() > 0); // Will be 0 if no camera is selected.
}








