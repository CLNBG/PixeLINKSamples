#pragma once

#include "afxwin.h"
#include "HistogramControlPanel.h"


class CHistogramDialog2;


// CHistogramControlPanel_Simple dialog

class CHistogramControlPanel_Simple : public CHistogramControlPanel
{
	friend class CHistogramDialog2;

	DECLARE_DYNAMIC(CHistogramControlPanel_Simple)

	DECLARE_MESSAGE_MAP()

public:
	CHistogramControlPanel_Simple(CHistogramDialog2* pParent);   // standard constructor
	virtual ~CHistogramControlPanel_Simple();

	void OnIdleUpdateControls(CCmdUI* pCmdUI);
    void UpdateChannelLabels(ULONG pixelFormat);

// Dialog Data
	enum { IDD = IDD_HISTOGRAM_CONTROL_PANEL_SIMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnInitDialog();

private:
	//CButton m_ShowRGBBlueCheck;
	//CButton m_ShowRGBGreenCheck;
	//CButton m_ShowRGBRedCheck;

public:
	//afx_msg void OnBnClickedHdRGBblueCheck();
	//afx_msg void OnBnClickedHdRGBgreenCheck();
	//afx_msg void OnBnClickedHdRGBredCheck();

private:
	CEdit m_RedMeanEdit;
	CEdit m_GreenMeanEdit;
	CEdit m_BlueMeanEdit;

	CEdit m_RedSDEdit;
	CEdit m_GreenSDEdit;
	CEdit m_BlueSDEdit;

	CEdit m_SharpnessScoreEdit;
};















