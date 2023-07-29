// HistogramControlPanel.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "HistogramDialog2.h"
#include "HistogramControlPanel_Eng.h"
#include <afxpriv.h>


// CHistogramControlPanel_Eng dialog

IMPLEMENT_DYNAMIC(CHistogramControlPanel_Eng, CHistogramControlPanel)


CHistogramControlPanel_Eng::CHistogramControlPanel_Eng(CHistogramDialog2* pParent)
	: CHistogramControlPanel(CHistogramControlPanel_Eng::IDD, pParent)
{
	m_pixelTypeChecks[EVEN_COLUMN_PIXELS] = &m_ShowEvenColsCheck;
	m_pixelTypeChecks[ODD_COLUMN_PIXELS] = &m_ShowOddColsCheck;
	m_pixelTypeChecks[BAYER_GREEN1_PIXELS] = &m_ShowBayerGreen1Check;
	m_pixelTypeChecks[BAYER_RED_PIXELS] = &m_ShowBayerRedCheck;
	m_pixelTypeChecks[BAYER_BLUE_PIXELS] = &m_ShowBayerBlueCheck;
	m_pixelTypeChecks[BAYER_GREEN2_PIXELS] = &m_ShowBayerGreen2Check;
	m_pixelTypeChecks[CHANNEL_0] = &m_ShowChannel0Check;
	m_pixelTypeChecks[CHANNEL_1] = &m_ShowChannel1Check;
	m_pixelTypeChecks[CHANNEL_2] = &m_ShowChannel2Check;

}

CHistogramControlPanel_Eng::~CHistogramControlPanel_Eng()
{
}

void CHistogramControlPanel_Eng::DoDataExchange(CDataExchange* pDX)
{
	CHistogramControlPanel::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_HD_EVENPIXELS_CHECK, m_ShowEvenColsCheck);
	DDX_Control(pDX, IDC_HD_ODDPIXELS_CHECK, m_ShowOddColsCheck);
	DDX_Control(pDX, IDC_HD_BAYERBLUE_CHECK, m_ShowBayerBlueCheck);
	DDX_Control(pDX, IDC_HD_BAYERGREEN1_CHECK, m_ShowBayerGreen1Check);
	DDX_Control(pDX, IDC_HD_BAYERGREEN2_CHECK, m_ShowBayerGreen2Check);
	DDX_Control(pDX, IDC_HD_BAYERRED_CHECK, m_ShowBayerRedCheck);
    DDX_Control(pDX, IDC_STATIC_HD_CHANNEL_ENABLE_CHECK, m_ShowChannelChecks);
	DDX_Control(pDX, IDC_HD_CHANNEL0_CHECK, m_ShowChannel0Check);
	DDX_Control(pDX, IDC_HD_CHANNEL1_CHECK, m_ShowChannel1Check);
	DDX_Control(pDX, IDC_HD_CHANNEL2_CHECK, m_ShowChannel2Check);
   DDX_Control(pDX, IDC_HD_ALPHA_CHECK, m_ShowAlphaCheck);
   DDX_Control(pDX, IDC_HD_CHANNEL0_STATS, m_ShowChannel0Stats);
    DDX_Control(pDX, IDC_HD_CHANNEL1_STATS, m_ShowChannel1Stats);
    DDX_Control(pDX, IDC_HD_CHANNEL2_STATS, m_ShowChannel2Stats);
    DDX_Control(pDX, IDC_HD_BLUE_ALPHA_STATS, m_ShowBlueAlphaStats);

	DDX_Control(pDX, IDC_HD_EVENMEAN_EDIT, m_EvenMeanEdit);
	DDX_Control(pDX, IDC_HD_ODDMEAN_EDIT, m_OddMeanEdit);
	DDX_Control(pDX, IDC_HD_REDMEAN_EDIT, m_RedMeanEdit);
	DDX_Control(pDX, IDC_HD_GREEN1MEAN_EDIT, m_Green1MeanEdit);
	DDX_Control(pDX, IDC_HD_GREEN2MEAN_EDIT, m_Green2MeanEdit);
	DDX_Control(pDX, IDC_HD_BLUEMEAN_EDIT, m_BlueMeanEdit);
	DDX_Control(pDX, IDC_HD_CHANNEL0_MEAN_EDIT, m_Channel0MeanEdit);
	DDX_Control(pDX, IDC_HD_CHANNEL1_MEAN_EDIT, m_Channel1MeanEdit);
	DDX_Control(pDX, IDC_HD_CHANNEL2_MEAN_EDIT, m_Channel2MeanEdit);

	DDX_Control(pDX, IDC_HD_EVENSD_EDIT, m_EvenSDEdit);
	DDX_Control(pDX, IDC_HD_ODDSD_EDIT, m_OddSDEdit);
	DDX_Control(pDX, IDC_HD_REDSD_EDIT, m_RedSDEdit);
	DDX_Control(pDX, IDC_HD_GREEN1SD_EDIT, m_Green1SDEdit);
	DDX_Control(pDX, IDC_HD_GREEN2SD_EDIT, m_Green2SDEdit);
	DDX_Control(pDX, IDC_HD_BLUESD_EDIT, m_BlueSDEdit);
	DDX_Control(pDX, IDC_HD_CHANNEL0_SD_EDIT, m_Channel0SDEdit);
	DDX_Control(pDX, IDC_HD_CHANNEL1_SD_EDIT, m_Channel1SDEdit);
	DDX_Control(pDX, IDC_HD_CHANNEL2_SD_EDIT, m_Channel2SDEdit);

	DDX_Control(pDX, IDC_HD_ACCUM_NONE_RADIO, m_AccumNoneRadio);
	DDX_Control(pDX, IDC_HD_ACCUM_BOTTOMUP_RADIO, m_AccumBottomUpRadio);
	DDX_Control(pDX, IDC_HD_ACCUM_TOPDOWN_RADIO, m_AccumTopDownRadio);

	DDX_Control(pDX, IDC_HD_SHARPNESS_SCORE_EDIT, m_SharpnessScoreEdit);
}


BEGIN_MESSAGE_MAP(CHistogramControlPanel_Eng, CHistogramControlPanel)

	ON_BN_CLICKED(IDC_HD_EVENPIXELS_CHECK, OnBnClickedHdEvenpixelsCheck)
	ON_BN_CLICKED(IDC_HD_ODDPIXELS_CHECK, OnBnClickedHdOddpixelsCheck)

	ON_BN_CLICKED(IDC_HD_BAYERBLUE_CHECK, OnBnClickedHdBayerblueCheck)
	ON_BN_CLICKED(IDC_HD_BAYERGREEN1_CHECK, OnBnClickedHdBayergreen1Check)
	ON_BN_CLICKED(IDC_HD_BAYERGREEN2_CHECK, OnBnClickedHdBayergreen2Check)
	ON_BN_CLICKED(IDC_HD_BAYERRED_CHECK, OnBnClickedHdBayerredCheck)

    ON_BN_CLICKED(IDC_HD_CHANNEL0_CHECK, OnBnClickedHdChannel0Check)
	ON_BN_CLICKED(IDC_HD_CHANNEL1_CHECK, OnBnClickedHdChannel1Check)
	ON_BN_CLICKED(IDC_HD_CHANNEL2_CHECK, OnBnClickedHdChannel2Check)
   //ON_BN_CLICKED(IDC_HD_ALPHA_CHECK, OnBnClickedHdAlphaCheck) // Always enabled 

	ON_BN_CLICKED(IDC_HD_ACCUM_NONE_RADIO, OnClickAccumNoneRadio)
	ON_BN_CLICKED(IDC_HD_ACCUM_BOTTOMUP_RADIO, OnClickAccumBottomUpRadio)
	ON_BN_CLICKED(IDC_HD_ACCUM_TOPDOWN_RADIO, OnClickAccumTopDownRadio)

END_MESSAGE_MAP()


// CHistogramControlPanel_Eng message handlers

BOOL CHistogramControlPanel_Eng::OnInitDialog()
{
	CHistogramControlPanel::OnInitDialog();

	m_meanEdits[EVEN_COLUMN_PIXELS] = this->GetDlgItem(IDC_HD_EVENMEAN_EDIT);
	m_meanEdits[ODD_COLUMN_PIXELS] = this->GetDlgItem(IDC_HD_ODDMEAN_EDIT);
	m_meanEdits[BAYER_RED_PIXELS] = this->GetDlgItem(IDC_HD_REDMEAN_EDIT);
	m_meanEdits[BAYER_GREEN1_PIXELS] = this->GetDlgItem(IDC_HD_GREEN1MEAN_EDIT);
	m_meanEdits[BAYER_GREEN2_PIXELS] = this->GetDlgItem(IDC_HD_GREEN2MEAN_EDIT);
	m_meanEdits[BAYER_BLUE_PIXELS] = this->GetDlgItem(IDC_HD_BLUEMEAN_EDIT);
	m_meanEdits[CHANNEL_0] = this->GetDlgItem(IDC_HD_CHANNEL0_MEAN_EDIT);
	m_meanEdits[CHANNEL_1] = this->GetDlgItem(IDC_HD_CHANNEL1_MEAN_EDIT);
	m_meanEdits[CHANNEL_2] = this->GetDlgItem(IDC_HD_CHANNEL2_MEAN_EDIT);

	m_stddevEdits[EVEN_COLUMN_PIXELS] = this->GetDlgItem(IDC_HD_EVENSD_EDIT);
	m_stddevEdits[ODD_COLUMN_PIXELS] = this->GetDlgItem(IDC_HD_ODDSD_EDIT);
	m_stddevEdits[BAYER_RED_PIXELS] = this->GetDlgItem(IDC_HD_REDSD_EDIT);
	m_stddevEdits[BAYER_GREEN1_PIXELS] = this->GetDlgItem(IDC_HD_GREEN1SD_EDIT);
	m_stddevEdits[BAYER_GREEN2_PIXELS] = this->GetDlgItem(IDC_HD_GREEN2SD_EDIT);
	m_stddevEdits[BAYER_BLUE_PIXELS] = this->GetDlgItem(IDC_HD_BLUESD_EDIT);
	m_stddevEdits[CHANNEL_0] = this->GetDlgItem(IDC_HD_CHANNEL0_SD_EDIT);
	m_stddevEdits[CHANNEL_1] = this->GetDlgItem(IDC_HD_CHANNEL1_SD_EDIT);
	m_stddevEdits[CHANNEL_2] = this->GetDlgItem(IDC_HD_CHANNEL2_SD_EDIT);

    // Initialize the sharpnessScore window
    if (this->m_pParent->m_pCam->FeatureSupported(FEATURE_SHARPNESS_SCORE))
    {
        m_SharpnessScoreEdit.EnableWindow(true);
    } else {
        m_SharpnessScoreEdit.EnableWindow(false);
        m_SharpnessScoreEdit.SetWindowText("N/A");
    }

    // Bugzilla.851 - Default channel plot lines, as enabled
    m_pParent->m_elts[CHANNEL_0].bShow = true;
    m_pParent->m_elts[CHANNEL_1].bShow = true;
    m_pParent->m_elts[CHANNEL_2].bShow = true;

    m_ShowAlphaCheck.SetCheck (TRUE);


    // Update the channel labels to show the correct ones
    UpdateChannelLabels (PIXEL_FORMAT_YUV422);  // Default to YUV, as the most 'likely' channelized format
    
	OnIdleUpdateControls(NULL);

	return TRUE;
}

void CHistogramControlPanel_Eng::OnIdleUpdateControls(CCmdUI* pCmdUI)
{
    m_AccumNoneRadio.SetCheck(this->m_pParent->m_cumulativeMode == 0);
	m_AccumBottomUpRadio.SetCheck(this->m_pParent->m_cumulativeMode == 1);
	m_AccumTopDownRadio.SetCheck(this->m_pParent->m_cumulativeMode == 2);

    // A 0 max means sharpnessScore is not enables/supported.
    if (m_pParent->m_maxSharpnessScore != 0.0)
    {
	    CString sharpnessScore;
	    // Represent the Sharpness score as a 1/100 of 1%
	    sharpnessScore.Format(_T("%g"), (this->m_pParent->m_sharpnessScore / m_pParent->m_maxSharpnessScore) * 10000.0);
	    m_SharpnessScoreEdit.SetWindowText(sharpnessScore);
	}

	CHistogramControlPanel::OnIdleUpdateControls(pCmdUI);
}

void CHistogramControlPanel_Eng::UpdateChannelLabels(ULONG pixelFormat)
{
   // Update the channel labes to match the requested pixel format
   m_ShowAlphaCheck.ShowWindow(SW_HIDE);
   switch (pixelFormat)
    {
    case PIXEL_FORMAT_YUV422:
        m_ShowChannelChecks.SetWindowText(_T("YUV"));
        m_ShowChannel0Check.SetWindowText(_T("Y"));
        m_ShowChannel1Check.SetWindowText(_T("U"));
        m_ShowChannel2Check.SetWindowText(_T("V"));
        m_ShowChannel0Stats.SetWindowText(_T("Y"));
        m_ShowChannel1Stats.SetWindowText(_T("U"));
        m_ShowChannel2Stats.SetWindowText(_T("V"));
        break;
    case PIXEL_FORMAT_HSV4_12:
        m_ShowChannelChecks.SetWindowText(_T("HSV"));
        m_ShowChannel0Check.SetWindowText(_T("H"));
        m_ShowChannel1Check.SetWindowText(_T("S"));
        m_ShowChannel2Check.SetWindowText(_T("V"));
        m_ShowChannel0Stats.SetWindowText(_T("Hue"));
        m_ShowChannel1Stats.SetWindowText(_T("Sat"));
        m_ShowChannel2Stats.SetWindowText(_T("Value"));
        break;
    case PIXEL_FORMAT_STOKES4_12:
        m_ShowChannelChecks.SetWindowText(_T("Stokes"));
        m_ShowChannel0Check.SetWindowText(_T("S0"));
        m_ShowChannel1Check.SetWindowText(_T("S1"));
        m_ShowChannel2Check.SetWindowText(_T("S2"));
        m_ShowChannel0Stats.SetWindowText(_T("S0"));
        m_ShowChannel1Stats.SetWindowText(_T("S1"));
        m_ShowChannel2Stats.SetWindowText(_T("S2"));
        break;
    case PIXEL_FORMAT_RGB24_NON_DIB:
    case PIXEL_FORMAT_BGR24_NON_DIB:
        m_ShowChannelChecks.SetWindowText(_T("RGB"));
        m_ShowChannel0Check.SetWindowText(_T("R"));
        m_ShowChannel1Check.SetWindowText(_T("G"));
        m_ShowChannel2Check.SetWindowText(_T("B"));
        m_ShowChannel0Stats.SetWindowText(_T("Red"));
        m_ShowChannel1Stats.SetWindowText(_T("Green"));
        m_ShowChannel2Stats.SetWindowText(_T("Blue"));
        break;
    case PIXEL_FORMAT_RGBA:
    case PIXEL_FORMAT_BGRA:
    case PIXEL_FORMAT_ARGB:
    case PIXEL_FORMAT_ABGR:
       m_ShowChannelChecks.SetWindowText(_T("Alpha"));
       m_ShowChannel0Check.SetWindowText(_T("R"));
       m_ShowChannel1Check.SetWindowText(_T("G"));
       m_ShowChannel2Check.SetWindowText(_T("B"));
       m_ShowChannel0Stats.SetWindowText(_T("Red"));
       m_ShowChannel1Stats.SetWindowText(_T("Green"));
       m_ShowChannel2Stats.SetWindowText(_T("Blue"));
       m_ShowBlueAlphaStats.SetWindowText(_T("Alpha")); // Steal the Blue column for Alpha
       m_ShowAlphaCheck.ShowWindow(SW_SHOW);
       break;
    default:
       m_ShowChannelChecks.SetWindowText(_T("RGB"));
       m_ShowBlueAlphaStats.SetWindowText(_T("Blue")); // Restore this back the default of 'blue'
    }
}

void CHistogramControlPanel_Eng::OnClickAccumNoneRadio()
{
	m_pParent->m_cumulativeMode = 0;
}

void CHistogramControlPanel_Eng::OnClickAccumBottomUpRadio()
{
	m_pParent->m_cumulativeMode = 1;
}

void CHistogramControlPanel_Eng::OnClickAccumTopDownRadio()
{
	m_pParent->m_cumulativeMode = 2;
}


void CHistogramControlPanel_Eng::OnBnClickedHdEvenpixelsCheck()
{
	m_pParent->m_elts[EVEN_COLUMN_PIXELS].bShow = (m_ShowEvenColsCheck.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdOddpixelsCheck()
{
	m_pParent->m_elts[ODD_COLUMN_PIXELS].bShow = (m_ShowOddColsCheck.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdBayerblueCheck()
{
	m_pParent->m_elts[BAYER_BLUE_PIXELS].bShow = (m_ShowBayerBlueCheck.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdBayergreen1Check()
{
	m_pParent->m_elts[BAYER_GREEN1_PIXELS].bShow = (m_ShowBayerGreen1Check.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdBayergreen2Check()
{
	m_pParent->m_elts[BAYER_GREEN2_PIXELS].bShow = (m_ShowBayerGreen2Check.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdBayerredCheck()
{
	m_pParent->m_elts[BAYER_RED_PIXELS].bShow = (m_ShowBayerRedCheck.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdChannel0Check()
{
	m_pParent->m_elts[CHANNEL_0].bShow = (m_ShowChannel0Check.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdChannel1Check()
{
	m_pParent->m_elts[CHANNEL_1].bShow = (m_ShowChannel1Check.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdChannel2Check()
{
	m_pParent->m_elts[CHANNEL_2].bShow = (m_ShowChannel2Check.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel_Eng::OnBnClickedHdAlphaCheck()
{
   m_pParent->m_elts[ALPHA_PIXELS].bShow = (m_ShowAlphaCheck.GetCheck() == BST_CHECKED);
}





