// HistogramControlPanel.cpp : implementation file
//

#include "stdafx.h"
#include "PxLDevApp.h"
#include "HistogramDialog2.h"
#include "HistogramControlPanel.h"
#include <afxpriv.h>


// CHistogramControlPanel dialog

IMPLEMENT_DYNAMIC(CHistogramControlPanel, CDialog)


CHistogramControlPanel::CHistogramControlPanel(UINT nIDTemplate, CHistogramDialog2* pParent)
	: CDialog(nIDTemplate, pParent)
	, m_pParent(pParent)
	, m_pixelTypeChecks(PIXEL_TYPES_TOTAL, NULL)
{
	std::fill(&m_meanEdits[0], &m_meanEdits[PIXEL_TYPES_TOTAL], (CWnd*)NULL);
	std::fill(&m_stddevEdits[0], &m_stddevEdits[PIXEL_TYPES_TOTAL], (CWnd*)NULL);

	m_pixelTypeChecks[ALL_PIXELS] = &m_ShowAllPixelsCheck;
	//m_pixelTypeChecks.push_back(&m_ShowEvenColsCheck);
	//m_pixelTypeChecks.push_back(&m_ShowOddColsCheck);
	//m_pixelTypeChecks.push_back(&m_ShowBayerGreen1Check);
	//m_pixelTypeChecks.push_back(&m_ShowBayerRedCheck);
	//m_pixelTypeChecks.push_back(&m_ShowBayerBlueCheck);
	//m_pixelTypeChecks.push_back(&m_ShowBayerGreen2Check);
	//m_pixelTypeChecks[RGB_RED_PIXELS] = &m_ShowRGBRedCheck;
	//m_pixelTypeChecks[RGB_BLUE_PIXELS] = &m_ShowRGBBlueCheck;
	//m_pixelTypeChecks[RGB_GREEN_PIXELS] = &m_ShowRGBGreenCheck;
}

CHistogramControlPanel::~CHistogramControlPanel()
{
}

void CHistogramControlPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_HD_PAUSE_BUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_HD_TRACKMOUSE_CHECK, m_TrackMouseCheck);
	DDX_Control(pDX, IDC_HD_USEDEFAULTS_CHECK, m_PlotDefaultsCheck);
	DDX_Control(pDX, IDC_HD_ALLPIXELS_CHECK, m_ShowAllPixelsCheck);
	//DDX_Control(pDX, IDC_HD_EVENPIXELS_CHECK, m_ShowEvenColsCheck);
	//DDX_Control(pDX, IDC_HD_ODDPIXELS_CHECK, m_ShowOddColsCheck);
	//DDX_Control(pDX, IDC_HD_BAYERBLUE_CHECK, m_ShowBayerBlueCheck);
	//DDX_Control(pDX, IDC_HD_BAYERGREEN1_CHECK, m_ShowBayerGreen1Check);
	//DDX_Control(pDX, IDC_HD_BAYERGREEN2_CHECK, m_ShowBayerGreen2Check);
	//DDX_Control(pDX, IDC_HD_BAYERRED_CHECK, m_ShowBayerRedCheck);

	DDX_Control(pDX, IDC_HD_AUTOSCALE_CHECK, m_AutoScaleCheck);

	DDX_Control(pDX, IDC_HD_ALLMEAN_EDIT, m_AllMeanEdit);
	//DDX_Control(pDX, IDC_HD_EVENMEAN_EDIT, m_EvenMeanEdit);
	//DDX_Control(pDX, IDC_HD_ODDMEAN_EDIT, m_OddMeanEdit);
	//DDX_Control(pDX, IDC_HD_REDMEAN_EDIT, m_RedMeanEdit);
	//DDX_Control(pDX, IDC_HD_GREEN1MEAN_EDIT, m_Green1MeanEdit);
	//DDX_Control(pDX, IDC_HD_GREEN2MEAN_EDIT, m_Green2MeanEdit);
	//DDX_Control(pDX, IDC_HD_BLUEMEAN_EDIT, m_BlueMeanEdit);

	DDX_Control(pDX, IDC_HD_ALLSD_EDIT, m_AllSDEdit);
	//DDX_Control(pDX, IDC_HD_EVENSD_EDIT, m_EvenSDEdit);
	//DDX_Control(pDX, IDC_HD_ODDSD_EDIT, m_OddSDEdit);
	//DDX_Control(pDX, IDC_HD_REDSD_EDIT, m_RedSDEdit);
	//DDX_Control(pDX, IDC_HD_GREEN1SD_EDIT, m_Green1SDEdit);
	//DDX_Control(pDX, IDC_HD_GREEN2SD_EDIT, m_Green2SDEdit);
	//DDX_Control(pDX, IDC_HD_BLUESD_EDIT, m_BlueSDEdit);
	DDX_Control(pDX, IDC_HD_VSCALEPLUS_BUTTON, m_VScalePlusButton);
	DDX_Control(pDX, IDC_HD_VSCALEMINUS_BUTTON, m_VScaleMinusButton);
	DDX_Control(pDX, IDC_HD_VSCALEUP_BUTTON, m_VScaleUpButton);
	DDX_Control(pDX, IDC_HD_VSCALEDOWN_BUTTON, m_VScaleDownButton);
	DDX_Control(pDX, IDC_HD_HSCALEPLUS_BUTTON, m_HScalePlusButton);
	DDX_Control(pDX, IDC_HD_HSCALEMINUS_BUTTON, m_HScaleMinusButton);
	DDX_Control(pDX, IDC_HD_HSCALELEFT_BUTTON, m_HScaleLeftButton);
	DDX_Control(pDX, IDC_HD_HSCALERIGHT_BUTTON, m_HScaleRightButton);

	DDX_Control(pDX, IDC_HD_CAMERA_FRAMERATE_EDIT, m_CameraFrameRateEdit);
	DDX_Control(pDX, IDC_HD_HOST_FRAMERATE_EDIT, m_HostFrameRateEdit);
}


BEGIN_MESSAGE_MAP(CHistogramControlPanel, CDialog)
	ON_BN_CLICKED(IDC_HD_TRACKMOUSE_CHECK, OnClickTrackMouseCheck)
	ON_BN_CLICKED(IDC_HD_PAUSE_BUTTON, OnBnClickedHdPauseButton)
	ON_UPDATE_COMMAND_UI(IDC_HD_USEDEFAULTS_CHECK, OnIdleUpdateControls)
	ON_BN_CLICKED(IDC_HD_USEDEFAULTS_CHECK, OnBnClickedHdUsedefaultsCheck)
	ON_BN_CLICKED(IDC_HD_ALLPIXELS_CHECK, OnBnClickedHdAllpixelsCheck)
	//ON_BN_CLICKED(IDC_HD_EVENPIXELS_CHECK, OnBnClickedHdEvenpixelsCheck)
	//ON_BN_CLICKED(IDC_HD_ODDPIXELS_CHECK, OnBnClickedHdOddpixelsCheck)
	//ON_BN_CLICKED(IDC_HD_BAYERBLUE_CHECK, OnBnClickedHdBayerblueCheck)
	//ON_BN_CLICKED(IDC_HD_BAYERGREEN1_CHECK, OnBnClickedHdBayergreen1Check)
	//ON_BN_CLICKED(IDC_HD_BAYERGREEN2_CHECK, OnBnClickedHdBayergreen2Check)
	//ON_BN_CLICKED(IDC_HD_BAYERRED_CHECK, OnBnClickedHdBayerredCheck)

	ON_BN_CLICKED(IDC_HD_AUTOSCALE_CHECK, OnBnClickedHdAutoscaleCheck)
	ON_BN_CLICKED(IDC_HD_VSCALEPLUS_BUTTON, OnBnClickedHdVscaleplusButton)
	ON_BN_CLICKED(IDC_HD_VSCALEMINUS_BUTTON, OnBnClickedHdVscaleminusButton)
	ON_BN_CLICKED(IDC_HD_VSCALEUP_BUTTON, OnBnClickedHdVscaleupButton)
	ON_BN_CLICKED(IDC_HD_VSCALEDOWN_BUTTON, OnBnClickedHdVscaledownButton)
	ON_BN_CLICKED(IDC_HD_HSCALEPLUS_BUTTON, OnBnClickedHdHscaleplusButton)
	ON_BN_CLICKED(IDC_HD_HSCALEMINUS_BUTTON, OnBnClickedHdHscaleminusButton)
	ON_BN_CLICKED(IDC_HD_HSCALELEFT_BUTTON, OnBnClickedHdHscaleleftButton)
	ON_BN_CLICKED(IDC_HD_HSCALERIGHT_BUTTON, OnBnClickedHdHscalerightButton)
END_MESSAGE_MAP()


// CHistogramControlPanel message handlers

void CHistogramControlPanel::OnClickTrackMouseCheck()
{
	m_pParent->m_Graph.put_TrackMouse(m_TrackMouseCheck.GetCheck() == BST_CHECKED);
}

BOOL CHistogramControlPanel::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_meanEdits[ALL_PIXELS] = this->GetDlgItem(IDC_HD_ALLMEAN_EDIT);
	//m_meanEdits[EVEN_COLUMN_PIXELS] = this->GetDlgItem(IDC_HD_EVENMEAN_EDIT);
	//m_meanEdits[ODD_COLUMN_PIXELS] = this->GetDlgItem(IDC_HD_ODDMEAN_EDIT);
	//m_meanEdits[BAYER_RED_PIXELS] = this->GetDlgItem(IDC_HD_REDMEAN_EDIT);
	//m_meanEdits[BAYER_GREEN1_PIXELS] = this->GetDlgItem(IDC_HD_GREEN1MEAN_EDIT);
	//m_meanEdits[BAYER_GREEN2_PIXELS] = this->GetDlgItem(IDC_HD_GREEN2MEAN_EDIT);
	//m_meanEdits[BAYER_BLUE_PIXELS] = this->GetDlgItem(IDC_HD_BLUEMEAN_EDIT);

	m_stddevEdits[ALL_PIXELS] = this->GetDlgItem(IDC_HD_ALLSD_EDIT);
	//m_stddevEdits[EVEN_COLUMN_PIXELS] = this->GetDlgItem(IDC_HD_EVENSD_EDIT);
	//m_stddevEdits[ODD_COLUMN_PIXELS] = this->GetDlgItem(IDC_HD_ODDSD_EDIT);
	//m_stddevEdits[BAYER_RED_PIXELS] = this->GetDlgItem(IDC_HD_REDSD_EDIT);
	//m_stddevEdits[BAYER_GREEN1_PIXELS] = this->GetDlgItem(IDC_HD_GREEN1SD_EDIT);
	//m_stddevEdits[BAYER_GREEN2_PIXELS] = this->GetDlgItem(IDC_HD_GREEN2SD_EDIT);
	//m_stddevEdits[BAYER_BLUE_PIXELS] = this->GetDlgItem(IDC_HD_BLUESD_EDIT);

	return TRUE;
}

void CHistogramControlPanel::OnIdleUpdateControls(CCmdUI* pCmdUI)
{
	// I'm just doing all the idle-updates in this one handler for now.

	// In YUV, HSV, and Stokes mode, we do not allow any other plot lines to be shown;
	m_PlotDefaultsCheck.EnableWindow(m_pParent->m_PixelChannels ==  PIXEL_CHANNELS_NONE);

	if (m_pParent->m_bSimpleMode &&
		(m_pParent->m_uDataFormat == PIXEL_FORMAT_MONO16
		 || m_pParent->m_uDataFormat == PIXEL_FORMAT_MONO8
		 || m_pParent->m_uDataFormat == PIXEL_FORMAT_MONO12_PACKED
		 || m_pParent->m_uDataFormat == PIXEL_FORMAT_MONO12_PACKED_MSFIRST
         || m_pParent->m_uDataFormat == PIXEL_FORMAT_MONO10_PACKED_MSFIRST
         || m_pParent->m_uDataFormat == PIXEL_FORMAT_POLAR4_12
         || m_pParent->m_uDataFormat == PIXEL_FORMAT_POLAR_RAW4_12))
	{
		// Do not allow the user to select R,G,B plots in MONO mode (except in Eng. mode).
		m_PlotDefaultsCheck.EnableWindow(FALSE);
	}

	m_TrackMouseCheck.SetCheck(m_pParent->m_Graph.get_TrackMouse());
	m_PlotDefaultsCheck.SetCheck(m_pParent->m_bAutoFormatMode);
	m_AutoScaleCheck.SetCheck(m_pParent->m_Graph.get_AutoScaleY());

	BOOL enable = !m_pParent->m_Graph.get_AutoScaleY();
	m_VScalePlusButton.EnableWindow(enable);
	m_VScaleMinusButton.EnableWindow(enable);
	m_VScaleUpButton.EnableWindow(enable);
	m_VScaleDownButton.EnableWindow(enable);
	m_HScalePlusButton.EnableWindow(enable);
	m_HScaleMinusButton.EnableWindow(enable);
	m_HScaleLeftButton.EnableWindow(enable);
	m_HScaleRightButton.EnableWindow(enable);

	int i;
	for (i = 0; i < PIXEL_TYPES_TOTAL; i++)
	{
		if (m_pixelTypeChecks[i] != NULL)
			m_pixelTypeChecks[i]->SetCheck(m_pParent->m_elts[i].bShow);
	}
	for (i = 0; i < CHANNEL_0; i++)
	{
		// Do not enable the checkboxes if we're in a mode that uses the generic channels
		if (m_pixelTypeChecks[i] != NULL)
            m_pixelTypeChecks[i]->EnableWindow(!m_pParent->m_bAutoFormatMode && m_pParent->m_PixelChannels == PIXEL_CHANNELS_NONE);
	}
	for (; i <= CHANNEL_2; i++)
	{
		// Do not enable the channel checkboxes unless we're in channelized mode.
		if (m_pixelTypeChecks[i] != NULL)
			m_pixelTypeChecks[i]->EnableWindow(m_pParent->m_PixelChannels != PIXEL_CHANNELS_NONE);
	}
	for (i = 0; i < PIXEL_TYPES_TOTAL; i++)
	{
		if (m_pParent->m_elts[i].bShow)
		{
			// Display mean and stddev
			CString s;
			HWND hwnd = m_meanEdits[i]->GetSafeHwnd();
			if (::IsWindow(hwnd))
			{
				::EnableWindow(hwnd, TRUE);
				s.Format("%lg", m_pParent->m_elts[i].mean);
				::SetWindowText(hwnd, s);
			}
			hwnd = m_stddevEdits[i]->GetSafeHwnd();
			if (::IsWindow(hwnd))
			{
				::EnableWindow(hwnd, TRUE);
				s.Format("%lg", m_pParent->m_elts[i].stddev);
				::SetWindowText(hwnd, s);
			}
		}
		else
		{
			// Empty and grey out the Mean/SD display.
			HWND hwnd = m_meanEdits[i]->GetSafeHwnd();
			if (::IsWindow(hwnd))
			{
				::SetWindowText(hwnd, "");
				::EnableWindow(hwnd, FALSE);
			}
			hwnd = m_stddevEdits[i]->GetSafeHwnd();
			if (::IsWindow(hwnd))
			{
				::SetWindowText(hwnd, "");
				::EnableWindow(hwnd, FALSE);
			}
		}
	}

	// FrameRate
	CString fr;
	fr.Format(_T("%g"), this->m_pParent->m_cameraRate);
	m_CameraFrameRateEdit.SetWindowText(fr);
	fr.Format(_T("%g"), this->m_pParent->m_hostRate);
	m_HostFrameRateEdit.SetWindowText(fr);
}

void CHistogramControlPanel::UpdateChannelLabels(ULONG pixelFormat)
{
}

void CHistogramControlPanel::OnBnClickedHdPauseButton()
{
	m_pParent->m_bPaused = !m_pParent->m_bPaused;
	m_PauseButton.SetWindowText(m_pParent->m_bPaused ? _T("Re&sume") : _T("Pau&se"));
}


void CHistogramControlPanel::OnBnClickedHdUsedefaultsCheck()
{
	m_pParent->m_bAutoFormatMode = (m_PlotDefaultsCheck.GetCheck() == BST_CHECKED);
}

void CHistogramControlPanel::OnBnClickedHdAllpixelsCheck()
{
	m_pParent->m_elts[ALL_PIXELS].bShow = (m_ShowAllPixelsCheck.GetCheck() == BST_CHECKED);
}

//void CHistogramControlPanel::OnBnClickedHdEvenpixelsCheck()
//{
//	m_pParent->m_elts[EVEN_COLUMN_PIXELS].bShow = (m_ShowEvenColsCheck.GetCheck() == BST_CHECKED);
//}
//
//void CHistogramControlPanel::OnBnClickedHdOddpixelsCheck()
//{
//	m_pParent->m_elts[ODD_COLUMN_PIXELS].bShow = (m_ShowOddColsCheck.GetCheck() == BST_CHECKED);
//}
//
//void CHistogramControlPanel::OnBnClickedHdBayerblueCheck()
//{
//	m_pParent->m_elts[BAYER_BLUE_PIXELS].bShow = (m_ShowBayerBlueCheck.GetCheck() == BST_CHECKED);
//}
//
//void CHistogramControlPanel::OnBnClickedHdBayergreen1Check()
//{
//	m_pParent->m_elts[BAYER_GREEN1_PIXELS].bShow = (m_ShowBayerGreen1Check.GetCheck() == BST_CHECKED);
//}
//
//void CHistogramControlPanel::OnBnClickedHdBayergreen2Check()
//{
//	m_pParent->m_elts[BAYER_GREEN2_PIXELS].bShow = (m_ShowBayerGreen2Check.GetCheck() == BST_CHECKED);
//}
//
//void CHistogramControlPanel::OnBnClickedHdBayerredCheck()
//{
//	m_pParent->m_elts[BAYER_RED_PIXELS].bShow = (m_ShowBayerRedCheck.GetCheck() == BST_CHECKED);
//}



void CHistogramControlPanel::OnBnClickedHdAutoscaleCheck()
{
	BOOL _auto = (m_AutoScaleCheck.GetCheck() == BST_CHECKED);
	m_pParent->m_Graph.put_AutoScaleY(_auto);
	m_pParent->m_Graph.put_AutoScaleX(_auto);
}


void CHistogramControlPanel::OnBnClickedHdVscaleplusButton()
{
	double min, max;
	m_pParent->m_Graph.GetYRange(&min, &max);
	max += (max-min); // double the scale
	m_pParent->m_Graph.SetYRange(min, max);
	m_pParent->m_Graph.Invalidate();
}

void CHistogramControlPanel::OnBnClickedHdVscaleminusButton()
{
	double min, max;
	m_pParent->m_Graph.GetYRange(&min, &max);
	max -= (max-min)/2.0; // double the scale
	m_pParent->m_Graph.SetYRange(min, max);
	m_pParent->m_Graph.Invalidate();
}

void CHistogramControlPanel::OnBnClickedHdVscaleupButton()
{
	double min, max;
	m_pParent->m_Graph.GetYRange(&min, &max);
	double incr = (max-min)/4.0;
	if (incr > 10)
		incr = (int)incr;
	min += incr;
	max += incr;
	m_pParent->m_Graph.SetYRange(min, max);
	m_pParent->m_Graph.Invalidate();
}

void CHistogramControlPanel::OnBnClickedHdVscaledownButton()
{
	double min, max;
	m_pParent->m_Graph.GetYRange(&min, &max);
	double incr = (max-min)/4.0;
	if (incr > 10)
		incr = (int)incr;
	min -= incr;
	max -= incr;
	m_pParent->m_Graph.SetYRange(min, max);
	m_pParent->m_Graph.Invalidate();
}

void CHistogramControlPanel::OnBnClickedHdHscaleplusButton()
{
	double min, max;
	m_pParent->m_Graph.GetXRange(&min, &max);
	max += (max-min); // double the scale
	m_pParent->m_Graph.SetXRange(min, max);
	m_pParent->m_Graph.Invalidate();
}

void CHistogramControlPanel::OnBnClickedHdHscaleminusButton()
{
	double min, max;
	m_pParent->m_Graph.GetXRange(&min, &max);
	max -= (max-min)/2.0; // halve the scale
	m_pParent->m_Graph.SetXRange(min, max);
	m_pParent->m_Graph.Invalidate();
}

void CHistogramControlPanel::OnBnClickedHdHscaleleftButton()
{
	double min, max;
	m_pParent->m_Graph.GetXRange(&min, &max);
	double incr = (max-min)/4.0;
	if (incr > 10)
		incr = (int)incr;
	min -= incr;
	max -= incr;
	m_pParent->m_Graph.SetXRange(min, max);
	m_pParent->m_Graph.Invalidate();
}

void CHistogramControlPanel::OnBnClickedHdHscalerightButton()
{
	double min, max;
	m_pParent->m_Graph.GetXRange(&min, &max);
	double incr = (max-min)/4.0;
	if (incr > 10)
		incr = (int)incr;
	min += incr;
	max += incr;
	m_pParent->m_Graph.SetXRange(min, max);
	m_pParent->m_Graph.Invalidate();
}



