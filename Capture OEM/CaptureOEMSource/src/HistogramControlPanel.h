#pragma once

#include "afxwin.h"


class CHistogramDialog2;


// CHistogramControlPanel dialog

class CHistogramControlPanel : public CDialog
{
	friend class CHistogramDialog2;

	DECLARE_DYNAMIC(CHistogramControlPanel)

public:
	CHistogramControlPanel(UINT nIDTemplate, CHistogramDialog2* pParent);
	virtual ~CHistogramControlPanel();

	virtual void OnIdleUpdateControls(CCmdUI* pCmdUI);
    virtual void UpdateChannelLabels(ULONG pixelFormat);

// Dialog Data
	//enum { IDD = NULL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CHistogramDialog2* m_pParent;
	CButton m_PauseButton;
	CButton m_TrackMouseCheck;
public:
	afx_msg void OnClickTrackMouseCheck();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedHdPauseButton();
protected:
	CButton m_PlotDefaultsCheck;
	CButton m_ShowAllPixelsCheck;
	//CButton m_ShowEvenColsCheck;
	//CButton m_ShowOddColsCheck;
	//CButton m_ShowBayerBlueCheck;
	//CButton m_ShowBayerGreen1Check;
	//CButton m_ShowBayerGreen2Check;
	//CButton m_ShowBayerRedCheck;

	std::vector<CButton*> m_pixelTypeChecks;
public:
	afx_msg void OnBnClickedHdUsedefaultsCheck();
	afx_msg void OnBnClickedHdAllpixelsCheck();
	//afx_msg void OnBnClickedHdEvenpixelsCheck();
	//afx_msg void OnBnClickedHdOddpixelsCheck();
	//afx_msg void OnBnClickedHdBayerblueCheck();
	//afx_msg void OnBnClickedHdBayergreen1Check();
	//afx_msg void OnBnClickedHdBayergreen2Check();
	//afx_msg void OnBnClickedHdBayerredCheck();
protected:
	CButton m_AutoScaleCheck;
public:
	afx_msg void OnBnClickedHdAutoscaleCheck();

protected:
	CEdit m_AllMeanEdit;
	//CEdit m_EvenMeanEdit;
	//CEdit m_OddMeanEdit;
	//CEdit m_RedMeanEdit;
	//CEdit m_Green1MeanEdit;
	//CEdit m_Green2MeanEdit;
	//CEdit m_BlueMeanEdit;

	CEdit m_AllSDEdit;
	//CEdit m_EvenSDEdit;
	//CEdit m_OddSDEdit;
	//CEdit m_RedSDEdit;
	//CEdit m_Green1SDEdit;
	//CEdit m_Green2SDEdit;
	//CEdit m_BlueSDEdit;

	CWnd*	m_meanEdits[PIXEL_TYPES_TOTAL];
	CWnd*	m_stddevEdits[PIXEL_TYPES_TOTAL];

	CEdit	m_CameraFrameRateEdit;
	CEdit	m_HostFrameRateEdit;

public:
	afx_msg void OnBnClickedHdVscaleplusButton();
protected:
	CButton m_VScalePlusButton;
	CButton m_VScaleMinusButton;
	CButton m_VScaleUpButton;
	CButton m_VScaleDownButton;
	CButton m_HScalePlusButton;
	CButton m_HScaleMinusButton;
	CButton m_HScaleLeftButton;
	CButton m_HScaleRightButton;
public:
	afx_msg void OnBnClickedHdVscaleminusButton();
	afx_msg void OnBnClickedHdVscaleupButton();
	afx_msg void OnBnClickedHdVscaledownButton();
	afx_msg void OnBnClickedHdHscaleplusButton();
	afx_msg void OnBnClickedHdHscaleminusButton();
	afx_msg void OnBnClickedHdHscaleleftButton();
	afx_msg void OnBnClickedHdHscalerightButton();

};















