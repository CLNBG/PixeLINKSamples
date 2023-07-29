// ExtendedShutterDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "CSlider.h"
#include "ExtendedShutterDialog.h"


#define MIN_INCREMENT	0.01f	// .01 milliseconds


// CExtendedShutterDialog dialog

IMPLEMENT_DYNAMIC(CExtendedShutterDialog, CFeatureDialog)


/**
* Function: CExtendedShutterDialog
* Purpose:  Constructor
*/
CExtendedShutterDialog::CExtendedShutterDialog(CWnd* pParent /*=NULL*/)
	: CFeatureDialog(CExtendedShutterDialog::IDD, pParent)
{
}


/**
* Function: ~CExtendedShutterDialog
* Purpose:  Destructor
*/
CExtendedShutterDialog::~CExtendedShutterDialog()
{
}


/**
* Function: DoDataExchange
* Purpose:  
*/
void CExtendedShutterDialog::DoDataExchange(CDataExchange* pDX)
{
	CFeatureDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_KD_KNEEPOINT1_CHECK, m_KPCheck1);
	DDX_Control(pDX, IDC_KD_KNEEPOINTMIN1_LABEL, m_KPMinLabel1);
	DDX_Control(pDX, IDC_KD_KNEEPOINT1_SLIDER, m_KPSlider1);
	DDX_Control(pDX, IDC_KD_KNEEPOINTMAX1_LABEL, m_KPMaxLabel1);
	DDX_Control(pDX, IDC_KD_KNEEPOINT1_EDIT, m_KPEdit1);

	DDX_Control(pDX, IDC_KD_KNEEPOINT2_CHECK, m_KPCheck2);
	DDX_Control(pDX, IDC_KD_KNEEPOINTMIN2_LABEL, m_KPMinLabel2);
	DDX_Control(pDX, IDC_KD_KNEEPOINT2_SLIDER, m_KPSlider2);
	DDX_Control(pDX, IDC_KD_KNEEPOINTMAX2_LABEL, m_KPMaxLabel2);
	DDX_Control(pDX, IDC_KD_KNEEPOINT2_EDIT, m_KPEdit2);

	DDX_Control(pDX, IDC_KD_KNEEPOINT3_CHECK, m_KPCheck3);
	DDX_Control(pDX, IDC_KD_KNEEPOINTMIN3_LABEL, m_KPMinLabel3);
	DDX_Control(pDX, IDC_KD_KNEEPOINT3_SLIDER, m_KPSlider3);
	DDX_Control(pDX, IDC_KD_KNEEPOINTMAX3_LABEL, m_KPMaxLabel3);
	DDX_Control(pDX, IDC_KD_KNEEPOINT3_EDIT, m_KPEdit3);

	DDX_Control(pDX, IDC_KD_KNEEPOINT4_CHECK, m_KPCheck4);
	DDX_Control(pDX, IDC_KD_KNEEPOINTMIN4_LABEL, m_KPMinLabel4);
	DDX_Control(pDX, IDC_KD_KNEEPOINT4_SLIDER, m_KPSlider4);
	DDX_Control(pDX, IDC_KD_KNEEPOINTMAX4_LABEL, m_KPMaxLabel4);
	DDX_Control(pDX, IDC_KD_KNEEPOINT4_EDIT, m_KPEdit4);

}


BEGIN_MESSAGE_MAP(CExtendedShutterDialog, CFeatureDialog)

	ON_BN_CLICKED(IDC_KD_KNEEPOINT1_CHECK, OnClickKneePointCheck1)
	ON_EN_KILLFOCUS(IDC_KD_KNEEPOINT1_EDIT, OnKillFocusKneePointEdit1)

	ON_BN_CLICKED(IDC_KD_KNEEPOINT2_CHECK, OnClickKneePointCheck2)
	ON_EN_KILLFOCUS(IDC_KD_KNEEPOINT2_EDIT, OnKillFocusKneePointEdit2)

	ON_BN_CLICKED(IDC_KD_KNEEPOINT3_CHECK, OnClickKneePointCheck3)
	ON_EN_KILLFOCUS(IDC_KD_KNEEPOINT3_EDIT, OnKillFocusKneePointEdit3)

	ON_BN_CLICKED(IDC_KD_KNEEPOINT4_CHECK, OnClickKneePointCheck4)
	ON_EN_KILLFOCUS(IDC_KD_KNEEPOINT4_EDIT, OnKillFocusKneePointEdit4)

	ON_UPDATE_COMMAND_UI(IDC_KD_KNEEPOINT1_EDIT, OnUpdateKneePoint1)

	ON_UPDATE_COMMAND_UI(IDC_KD_KNEEPOINT2_CHECK, OnUpdateKneePointCheck2)
	ON_UPDATE_COMMAND_UI(IDC_KD_KNEEPOINT2_EDIT, OnUpdateKneePoint2)

	ON_UPDATE_COMMAND_UI(IDC_KD_KNEEPOINT3_CHECK, OnUpdateKneePointCheck3)
	ON_UPDATE_COMMAND_UI(IDC_KD_KNEEPOINT3_EDIT, OnUpdateKneePoint3)

	ON_UPDATE_COMMAND_UI(IDC_KD_KNEEPOINT4_CHECK, OnUpdateKneePointCheck4)
	ON_UPDATE_COMMAND_UI(IDC_KD_KNEEPOINT4_EDIT, OnUpdateKneePoint4)

END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CExtendedShutterDialog, CFeatureDialog)

	ON_EVENT(CExtendedShutterDialog, IDC_KD_KNEEPOINT1_SLIDER, 1, OnScrollKneePointSlider1, VTS_NONE)
	ON_EVENT(CExtendedShutterDialog, IDC_KD_KNEEPOINT1_SLIDER, 2, OnChangeKneePointSlider1, VTS_NONE)

	ON_EVENT(CExtendedShutterDialog, IDC_KD_KNEEPOINT2_SLIDER, 1, OnScrollKneePointSlider2, VTS_NONE)
	ON_EVENT(CExtendedShutterDialog, IDC_KD_KNEEPOINT2_SLIDER, 2, OnChangeKneePointSlider2, VTS_NONE)

	ON_EVENT(CExtendedShutterDialog, IDC_KD_KNEEPOINT3_SLIDER, 1, OnScrollKneePointSlider3, VTS_NONE)
	ON_EVENT(CExtendedShutterDialog, IDC_KD_KNEEPOINT3_SLIDER, 2, OnChangeKneePointSlider3, VTS_NONE)

	ON_EVENT(CExtendedShutterDialog, IDC_KD_KNEEPOINT4_SLIDER, 1, OnScrollKneePointSlider4, VTS_NONE)
	ON_EVENT(CExtendedShutterDialog, IDC_KD_KNEEPOINT4_SLIDER, 2, OnChangeKneePointSlider4, VTS_NONE)

END_EVENTSINK_MAP()


// CExtendedShutterDialog message handlers

BOOL CExtendedShutterDialog::OnInitDialog()
{
	m_KPSlider1.SetLinkedWindows(&m_KPEdit1, &m_KPMinLabel1, &m_KPMaxLabel1, NULL);
	m_KPSlider2.SetLinkedWindows(&m_KPEdit2, &m_KPMinLabel2, &m_KPMaxLabel2, NULL);
	m_KPSlider3.SetLinkedWindows(&m_KPEdit3, &m_KPMinLabel3, &m_KPMaxLabel3, NULL);
	m_KPSlider4.SetLinkedWindows(&m_KPEdit4, &m_KPMinLabel4, &m_KPMaxLabel4, NULL);

	CFeatureDialog::OnInitDialog();

	return TRUE;
}


/**
* Function: Configure
* Purpose:  
*/
void CExtendedShutterDialog::Configure(void)
{
	m_MaxKnees = static_cast<int>(m_pCam->GetFeatureMaxVal(FEATURE_EXTENDED_SHUTTER, FEATURE_EXTENDED_SHUTTER_PARAM_NUM_KNEES));
}


/**
* Function: Populate
* Purpose:  
*/
void CExtendedShutterDialog::Populate(void)
{
	try
	{
		std::vector<float> kneePoints = m_pCam->GetKneePoints();

		TempVal<bool> temp(m_bAdjusting, true);

		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,1);
		m_Exposure = m_pCam->GetFeatureValue(FEATURE_SHUTTER);
		bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);
		
    	if (useCameraKpLimits) m_KPSlider1.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT); 
		else                   m_KPSlider1.SetRange(kneePointLimits.first + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
		m_KPCheck1.SetCheck(BST_UNCHECKED); m_KPCheck1.EnableWindow(true);

		if (kneePoints.size() > 0)
		{
		    if (useCameraKpLimits) kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,2);
			m_KPCheck1.SetCheck(BST_CHECKED);
			m_KPSlider1.SetValue(kneePoints[0]);
			if (useCameraKpLimits) m_KPSlider2.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT); 
			else                   m_KPSlider2.SetRange(kneePoints[0] + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
		}

		if (kneePoints.size() > 1)
		{
		    if (useCameraKpLimits) kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,3);
			m_KPCheck2.SetCheck(BST_CHECKED); m_KPCheck2.EnableWindow(true);
			m_KPSlider2.SetValue(kneePoints[1]);
			if (useCameraKpLimits) m_KPSlider3.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT); 
			else                   m_KPSlider3.SetRange(kneePoints[0] + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
		}

		if (kneePoints.size() > 2)
		{
		    if (useCameraKpLimits) kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,4);
			m_KPCheck3.SetCheck(BST_CHECKED); m_KPCheck3.EnableWindow(true);
			m_KPSlider3.SetValue(kneePoints[2]);
			if (useCameraKpLimits) m_KPSlider4.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT); 
			else                   m_KPSlider4.SetRange(kneePoints[0] + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
		}

		if (kneePoints.size() > 3)
		{
    		m_KPCheck4.SetCheck(BST_CHECKED); m_KPCheck4.EnableWindow(true);
			m_KPSlider4.SetValue(kneePoints[3]);
		}

	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/**
* Function: SetKneePoints
* Purpose:  
*/
void CExtendedShutterDialog::SetKneePoints(void)
{
	try
	{
		std::vector<float> knees;
		float val;

		if (m_KPCheck1.GetCheck() == BST_CHECKED)
		{
			val = m_KPSlider1.GetValue();
			knees.push_back(val);

			if (m_KPCheck2.GetCheck() == BST_CHECKED)
			{
				val = m_KPSlider2.GetValue();
				if (val >= m_KPSlider2.GetMin() + MIN_INCREMENT)
				{
					knees.push_back(val);

					if (m_KPCheck3.GetCheck() == BST_CHECKED)
					{
						val = m_KPSlider3.GetValue();
						if (val >= m_KPSlider3.GetMin() + MIN_INCREMENT)
						{
							knees.push_back(val);

							if (m_KPCheck4.GetCheck() == BST_CHECKED)
							{
								val = m_KPSlider4.GetValue();
								if (val >= m_KPSlider4.GetMin() + MIN_INCREMENT)
								{
									knees.push_back(val);
								}
							}
						}
					}
				}
			}
		}

		m_pCam->SetKneePoints(knees);
		Populate();
	}
	catch (PxLException const& e)
	{
		OnApiError(e);
	}
}


/******************************************************************************
* Mesage Handlers
******************************************************************************/

void CExtendedShutterDialog::OnClickKneePointCheck1(void)
{
	if (m_KPCheck1.GetCheck() == BST_CHECKED)
	{
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,1);
		bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

		if (useCameraKpLimits) m_KPSlider1.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider1.SetRange(kneePointLimits.first + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnClickKneePointCheck2(void)
{
	if (m_KPCheck2.GetCheck() == BST_CHECKED)
	{
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,2);
		bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

		if (useCameraKpLimits) m_KPSlider2.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider2.SetRange(kneePointLimits.first + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnClickKneePointCheck3(void)
{
	if (m_KPCheck3.GetCheck() == BST_CHECKED)
	{
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,3);
		bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

		if (useCameraKpLimits) m_KPSlider3.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider3.SetRange(kneePointLimits.first + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnClickKneePointCheck4(void)
{
	if (m_KPCheck4.GetCheck() == BST_CHECKED)
	{
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,4);
		bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

		if (useCameraKpLimits) m_KPSlider4.SetRange(kneePointLimits.first + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider4.SetRange(kneePointLimits.first + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnScrollKneePointSlider1(void)
{
	m_KPSlider1.OnScroll();

	if (m_KPSlider1.GetValue() <= m_KPSlider1.GetMin() + MIN_INCREMENT)
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
		m_KPCheck2.SetCheck(BST_UNCHECKED); m_KPCheck2.EnableWindow(false);
	}
	else 
	{
	    m_KPCheck2.EnableWindow(true);
        // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
	    // properly, so for those cameras -- use the shutter as the limit
	    std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,2);
	    bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

	    if (useCameraKpLimits) m_KPSlider2.SetRange(m_KPSlider1.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
	    else                   m_KPSlider2.SetRange(m_KPSlider1.GetValue() + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnChangeKneePointSlider1(void)
{
	m_KPSlider1.OnChange();

	if (m_bAdjusting)
		return;

	if (m_KPSlider1.GetValue() <= m_KPSlider1.GetMin() + MIN_INCREMENT)
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
		m_KPCheck2.SetCheck(BST_UNCHECKED); m_KPCheck2.EnableWindow(false);
	}
	else
	{
		m_KPCheck2.EnableWindow(true);
		// Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
		// properly, so for those cameras -- use the shutter as the limit
		std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,2);
		bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

		if (useCameraKpLimits) m_KPSlider2.SetRange(m_KPSlider1.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
		else                   m_KPSlider2.SetRange(m_KPSlider1.GetValue() + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnScrollKneePointSlider2(void)
{
	m_KPSlider2.OnScroll();

	if (m_KPSlider2.GetValue() <= m_KPSlider2.GetMin()  + MIN_INCREMENT)
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
	}
	else
	{
	    m_KPCheck3.EnableWindow(true);
        // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
	    // properly, so for those cameras -- use the shutter as the limit
	    std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,3);
	    bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

	    if (useCameraKpLimits) m_KPSlider3.SetRange(m_KPSlider2.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
	    else                   m_KPSlider3.SetRange(m_KPSlider2.GetValue() + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnChangeKneePointSlider2(void)
{
	m_KPSlider2.OnChange();

	if (m_bAdjusting)
		return;

	if (m_KPSlider2.GetValue() <= m_KPSlider2.GetMin() + MIN_INCREMENT)
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
		m_KPCheck3.SetCheck(BST_UNCHECKED); m_KPCheck3.EnableWindow(false);
	}
	else 
	{
	    m_KPCheck3.EnableWindow(true);
        // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
	    // properly, so for those cameras -- use the shutter as the limit
	    std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,3);
	    bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

	    if (useCameraKpLimits) m_KPSlider3.SetRange(m_KPSlider2.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
	    else                   m_KPSlider3.SetRange(m_KPSlider2.GetValue() + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnScrollKneePointSlider3(void)
{
	m_KPSlider3.OnScroll();

	if (m_KPSlider3.GetValue() <= m_KPSlider3.GetMin() + MIN_INCREMENT)
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
	}
	else 
	{
	    m_KPCheck4.EnableWindow(true);
        // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
	    // properly, so for those cameras -- use the shutter as the limit
	    std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,4);
	    bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

	    if (useCameraKpLimits) m_KPSlider4.SetRange(m_KPSlider3.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
	    else                   m_KPSlider4.SetRange(m_KPSlider3.GetValue() + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnChangeKneePointSlider3(void)
{
	m_KPSlider3.OnChange();

	if (m_bAdjusting)
		return;

	if (m_KPSlider3.GetValue() <= m_KPSlider3.GetMin() + MIN_INCREMENT)
	{
		m_KPCheck4.SetCheck(BST_UNCHECKED); m_KPCheck4.EnableWindow(false);
	}
	else 
	{
	    m_KPCheck4.EnableWindow(true);
        // Bugzilla.178 - Limit the kneepoints to the feature limits.  However, not all cameras will set them
	    // properly, so for those cameras -- use the shutter as the limit
	    std::pair<float,float> kneePointLimits = m_pCam->GetFeatureLimits(FEATURE_EXTENDED_SHUTTER,4);
	    bool useCameraKpLimits = (kneePointLimits.second < m_Exposure);

	    if (useCameraKpLimits) m_KPSlider4.SetRange(m_KPSlider3.GetValue() + MIN_INCREMENT, kneePointLimits.second - MIN_INCREMENT);
	    else                   m_KPSlider4.SetRange(m_KPSlider3.GetValue() + MIN_INCREMENT, m_Exposure - MIN_INCREMENT);
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnScrollKneePointSlider4(void)
{
	m_KPSlider4.OnScroll();

	SetKneePoints();
}


void CExtendedShutterDialog::OnChangeKneePointSlider4(void)
{
	m_KPSlider4.OnChange();

	if (m_bAdjusting)
		return;

	SetKneePoints();
}


void CExtendedShutterDialog::OnKillFocusKneePointEdit1(void)
{
	{
		TempVal<bool> temp(m_bAdjusting, true);
		m_KPSlider1.UpdateFromWnd();
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnKillFocusKneePointEdit2(void)
{
	{
		TempVal<bool> temp(m_bAdjusting, true);
		m_KPSlider2.UpdateFromWnd();
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnKillFocusKneePointEdit3(void)
{
	{
		TempVal<bool> temp(m_bAdjusting, true);
		m_KPSlider3.UpdateFromWnd();
	}
	SetKneePoints();
}


void CExtendedShutterDialog::OnKillFocusKneePointEdit4(void)
{
	{
		TempVal<bool> temp(m_bAdjusting, true);
		m_KPSlider4.UpdateFromWnd();
	}
	SetKneePoints();
}


/******************************************************************************
 * UPDATE_COMMAND_UI handlers.
 *****************************************************************************/

void CExtendedShutterDialog::OnUpdateKneePoint1(CCmdUI *pCmdUI)
{
	BOOL enable = m_MaxKnees >= 1
					&& m_KPCheck1.GetCheck() == BST_CHECKED;
	pCmdUI->Enable(enable);
	if (enable != m_KPSlider1.get_Enabled())
		m_KPSlider1.put_Enabled(enable);
}


void CExtendedShutterDialog::OnUpdateKneePointCheck2(CCmdUI *pCmdUI)
{
	BOOL enable = m_MaxKnees >= 2
					&& m_KPCheck1.GetCheck() == BST_CHECKED
					&& m_KPSlider1.GetValue() >= m_KPSlider1.GetMin() + MIN_INCREMENT;
	pCmdUI->Enable(enable);
	if (!enable)
		pCmdUI->SetCheck(BST_UNCHECKED);
}


void CExtendedShutterDialog::OnUpdateKneePoint2(CCmdUI *pCmdUI)
{
	BOOL enable = m_KPCheck2.GetCheck() == BST_CHECKED;
	pCmdUI->Enable(enable);
	if (enable != m_KPSlider2.get_Enabled())
		m_KPSlider2.put_Enabled(enable);
}


void CExtendedShutterDialog::OnUpdateKneePointCheck3(CCmdUI *pCmdUI)
{
	BOOL enable = m_MaxKnees >= 3
					&& m_KPCheck2.GetCheck() == BST_CHECKED
					&& m_KPSlider2.GetValue() >= m_KPSlider2.GetMin() + MIN_INCREMENT;
	pCmdUI->Enable(enable);
	if (!enable)
		pCmdUI->SetCheck(BST_UNCHECKED);
}


void CExtendedShutterDialog::OnUpdateKneePoint3(CCmdUI *pCmdUI)
{
	BOOL enable = m_KPCheck3.GetCheck() == BST_CHECKED;
	pCmdUI->Enable(enable);
	if (enable != m_KPSlider3.get_Enabled())
		m_KPSlider3.put_Enabled(enable);
}


void CExtendedShutterDialog::OnUpdateKneePointCheck4(CCmdUI *pCmdUI)
{
	BOOL enable = m_MaxKnees >= 4
					&& m_KPCheck3.GetCheck() == BST_CHECKED
					&& m_KPSlider3.GetValue() >= m_KPSlider3.GetMin() + MIN_INCREMENT;
	pCmdUI->Enable(enable);
	if (!enable)
		pCmdUI->SetCheck(BST_UNCHECKED);
}


void CExtendedShutterDialog::OnUpdateKneePoint4(CCmdUI *pCmdUI)
{
	BOOL enable = m_KPCheck4.GetCheck() == BST_CHECKED;
	pCmdUI->Enable(enable);
	if (enable != m_KPSlider4.get_Enabled())
		m_KPSlider4.put_Enabled(enable);
}


















