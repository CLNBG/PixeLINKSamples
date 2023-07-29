// OneTimeAutoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "OneTimeAutoDialog.h"
#include "Helpers.h"
#include "PxLCamera.h"


// COneTimeAutoDialog dialog

IMPLEMENT_DYNAMIC(COneTimeAutoDialog, CDialog)

COneTimeAutoDialog::COneTimeAutoDialog(CPxLCamera* pParent)
	//: CDialog(COneTimeAutoDialog::IDD, pParent)
	: CDialog(COneTimeAutoDialog::IDD)
	, m_parent(pParent)
	, m_timerId(0)
	, m_pollNumber(0)
{
}

COneTimeAutoDialog::~COneTimeAutoDialog()
{
}

TCHAR* COneTimeAutoDialog::FeatureIdStr(U32 featureID)
{
    switch (featureID)
    {
    case FEATURE_SHUTTER:       return _T("Exposure");
    case FEATURE_GAIN:          return _T("Gain");
    case FEATURE_SATURATION:    return _T("Saturation");
    case FEATURE_BRIGHTNESS:    return _T("Brightness");
    case FEATURE_FRAME_RATE:    return _T("Frame Rate");
    case FEATURE_GAMMA:         return _T("Gamma");
    case FEATURE_WHITE_SHADING: return _T("White Balance");
    case FEATURE_FOCUS:         return _T("Focus");
    default:                     return _T("Unknown");
    }
}

float COneTimeAutoDialog::FeatureValue(U32 featureID, U32* pFlags/* = NULL*/)
{
    // This will also set the pFlags filed, if supplied.
    float featureValue = m_parent->GetFeatureValue(featureID,  pFlags);
    
    return featureValue;
}

void COneTimeAutoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OTA_FEATURE, m_OneTimeAutoFeatureLabel);
	DDX_Control(pDX, IDC_OTA_VALUE, m_OneTimeAutoValueLabel);
	DDX_Control(pDX, IDC_OTA_POLL_NUMBER, m_OneTimeAutoPollNumberLabel);
}


BEGIN_MESSAGE_MAP(COneTimeAutoDialog, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTimeLapseCaptureDialog message handlers

BOOL COneTimeAutoDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    CString str;

    m_OneTimeAutoFeatureLabel.SetWindowText(FeatureIdStr(m_parent->m_oneTimeAutofeatureId));
    SetFloatText (m_OneTimeAutoValueLabel, FeatureValue(m_parent->m_oneTimeAutofeatureId)); 
    SetIntText (m_OneTimeAutoPollNumberLabel, m_pollNumber);

	// Set our timer to periodically check to see if we are done
	m_timerId = SetTimer(ONE_TIME_AUTO_TIMER_ID, 300, NULL);

	return TRUE;
}

void COneTimeAutoDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ONE_TIME_AUTO_TIMER_ID)
	{
		U32  flags;
        CString str;
		
		// Poll the camera to get the feature value, and the flags so that we can see if it is done
		m_pollNumber++;
		float feature = FeatureValue (m_parent->m_oneTimeAutofeatureId, &flags);
		
		// Grab the next image
		if (! (flags & FEATURE_FLAG_ONEPUSH))
		{
			// Whoo-Hoo, the auto operation is done.
			this->PostMessage(WM_CLOSE, 0, 0);
			return;
		}
		
        if (FEATURE_WHITE_SHADING == m_parent->m_oneTimeAutofeatureId)
        {
            // Special handling for white shading, as it requires 'special' formatting to show all 3 color channels
            CString  str;
            U32 flags;
            U32 nParms = 3;
        	std::vector<float> parms(nParms, 0.0f);

	        PXL_ERROR_CHECK(PxLGetFeature(m_parent->m_handle, FEATURE_WHITE_SHADING, &flags, &nParms, &parms[0]));
            str.Format(_T("R:%4.2f G:%4.2f B%4.2f"),parms[0],parms[1], parms[2]);
            m_OneTimeAutoValueLabel.SetWindowText(str); 
        } else {
            SetFloatText (m_OneTimeAutoValueLabel, feature); 
        }
        SetIntText (m_OneTimeAutoPollNumberLabel, m_pollNumber);
	}
}


void COneTimeAutoDialog::OnCancel()
{
	if (m_timerId)
	{
		KillTimer(m_timerId);
	}
	// else: we never started the timer, so don't need to Kill it.

    m_parent->SetOnePush(m_parent->m_oneTimeAutofeatureId, false);
	CDialog::OnCancel();
}




















