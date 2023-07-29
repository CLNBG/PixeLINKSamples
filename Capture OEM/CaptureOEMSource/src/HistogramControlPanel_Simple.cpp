// HistogramControlPanel.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "HistogramDialog2.h"
#include "HistogramControlPanel_Simple.h"
#include <afxpriv.h>


// CHistogramControlPanel_Simple dialog

IMPLEMENT_DYNAMIC(CHistogramControlPanel_Simple, CHistogramControlPanel)


CHistogramControlPanel_Simple::CHistogramControlPanel_Simple(CHistogramDialog2* pParent)
	: CHistogramControlPanel(CHistogramControlPanel_Simple::IDD, pParent)
{
	//m_pixelTypeChecks[RGB_RED_PIXELS] = &m_ShowRGBRedCheck;
	//m_pixelTypeChecks[RGB_BLUE_PIXELS] = &m_ShowRGBBlueCheck;
	//m_pixelTypeChecks[RGB_GREEN_PIXELS] = &m_ShowRGBGreenCheck;
}

CHistogramControlPanel_Simple::~CHistogramControlPanel_Simple()
{
}

void CHistogramControlPanel_Simple::DoDataExchange(CDataExchange* pDX)
{
	CHistogramControlPanel::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_HD_REDMEAN_EDIT, m_RGBRedMeanEdit);
	//DDX_Control(pDX, IDC_HD_GREENMEAN_EDIT, m_RGBGreenMeanEdit);
	//DDX_Control(pDX, IDC_HD_BLUEMEAN_EDIT, m_RGBBlueMeanEdit);

	//DDX_Control(pDX, IDC_HD_REDSD_EDIT, m_RGBRedSDEdit);
	//DDX_Control(pDX, IDC_HD_GREENSD_EDIT, m_RGBGreenSDEdit);
	//DDX_Control(pDX, IDC_HD_BLUESD_EDIT, m_RGBBlueSDEdit);

	DDX_Control(pDX, IDC_HD_SHARPNESS_SCORE_EDIT1, m_SharpnessScoreEdit);
}


BEGIN_MESSAGE_MAP(CHistogramControlPanel_Simple, CHistogramControlPanel)
END_MESSAGE_MAP()


// CHistogramControlPanel_Simple message handlers

BOOL CHistogramControlPanel_Simple::OnInitDialog()
{
	CHistogramControlPanel::OnInitDialog();

    // Initialize the sharpnessScore window
    if (this->m_pParent->m_pCam->FeatureSupported(FEATURE_SHARPNESS_SCORE))
    {
        m_SharpnessScoreEdit.EnableWindow(true);
    } else {
        m_SharpnessScoreEdit.EnableWindow(false);
        m_SharpnessScoreEdit.SetWindowText("N/A");
        m_pParent->m_maxSharpnessScore = 0.0f;
    }

	OnIdleUpdateControls(NULL);

	return TRUE;
}

void CHistogramControlPanel_Simple::OnIdleUpdateControls(CCmdUI* pCmdUI)
{
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


void CHistogramControlPanel_Simple::UpdateChannelLabels(ULONG pixelFormat)
{
}

//void CHistogramControlPanel_Simple::OnBnClickedHdRGBblueCheck()
//{
//	m_pParent->m_elts[RGB_BLUE_PIXELS].bShow = (m_ShowRGBBlueCheck.GetCheck() == BST_CHECKED);
//}
//
//void CHistogramControlPanel_Simple::OnBnClickedHdRGBgreenCheck()
//{
//	m_pParent->m_elts[RGB_GREEN_PIXELS].bShow = (m_ShowRGBGreenCheck.GetCheck() == BST_CHECKED);
//}
//
//void CHistogramControlPanel_Simple::OnBnClickedHdRGBredCheck()
//{
//	m_pParent->m_elts[RGB_RED_PIXELS].bShow = (m_ShowRGBRedCheck.GetCheck() == BST_CHECKED);
//}









