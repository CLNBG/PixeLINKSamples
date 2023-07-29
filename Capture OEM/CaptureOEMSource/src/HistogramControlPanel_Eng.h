#pragma once

#include "afxwin.h"
#include "HistogramControlPanel.h"
#include "afxbutton.h"


class CHistogramDialog2;


// CHistogramControlPanel_Eng dialog

class CHistogramControlPanel_Eng : public CHistogramControlPanel
{
	friend class CHistogramDialog2;

	DECLARE_DYNAMIC(CHistogramControlPanel_Eng)

	DECLARE_MESSAGE_MAP()

public:
	CHistogramControlPanel_Eng(CHistogramDialog2* pParent);   // standard constructor
	virtual ~CHistogramControlPanel_Eng();

// Dialog Data
	enum { IDD = IDD_HISTOGRAM_CONTROL_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnInitDialog();

private:
	CButton m_ShowEvenColsCheck;
	CButton m_ShowOddColsCheck;
	CButton m_ShowBayerBlueCheck;
	CButton m_ShowBayerGreen1Check;
	CButton m_ShowBayerGreen2Check;
	CButton m_ShowBayerRedCheck;
    CFrameWnd  m_ShowChannelChecks;
	CButton	m_ShowChannel0Check;
	CButton	m_ShowChannel1Check;
	CButton	m_ShowChannel2Check;
   CButton	m_ShowAlphaCheck;
   CFrameWnd m_ShowChannel0Stats;
    CFrameWnd m_ShowChannel1Stats;
    CFrameWnd m_ShowChannel2Stats;
    CFrameWnd m_ShowBlueAlphaStats;

public:
	afx_msg void OnBnClickedHdEvenpixelsCheck();
	afx_msg void OnBnClickedHdOddpixelsCheck();
	afx_msg void OnBnClickedHdBayerblueCheck();
	afx_msg void OnBnClickedHdBayergreen1Check();
	afx_msg void OnBnClickedHdBayergreen2Check();
	afx_msg void OnBnClickedHdBayerredCheck();
	afx_msg void OnBnClickedHdChannel0Check();
	afx_msg void OnBnClickedHdChannel1Check();
	afx_msg void OnBnClickedHdChannel2Check();
   afx_msg void OnBnClickedHdAlphaCheck();

	afx_msg void OnClickAccumNoneRadio();
	afx_msg void OnClickAccumBottomUpRadio();
	afx_msg void OnClickAccumTopDownRadio();

	void OnIdleUpdateControls(CCmdUI* pCmdUI);
    void UpdateChannelLabels(ULONG pixelFormat);

	afx_msg void OnUpdateAccumNoneRadio(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAccumBottomUpRadio(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAccumTopDownRadio(CCmdUI *pCmdUI);

private:
	CEdit m_EvenMeanEdit;
	CEdit m_OddMeanEdit;
	CEdit m_RedMeanEdit;
	CEdit m_Green1MeanEdit;
	CEdit m_Green2MeanEdit;
	CEdit m_BlueMeanEdit;
	CEdit m_Channel0MeanEdit;
	CEdit m_Channel1MeanEdit;
	CEdit m_Channel2MeanEdit;

	CEdit m_EvenSDEdit;
	CEdit m_OddSDEdit;
	CEdit m_RedSDEdit;
	CEdit m_Green1SDEdit;
	CEdit m_Green2SDEdit;
	CEdit m_BlueSDEdit;
	CEdit m_Channel0SDEdit;
	CEdit m_Channel1SDEdit;
	CEdit m_Channel2SDEdit;

	CEdit m_SharpnessScoreEdit;

public:
	CButton		m_AccumNoneRadio;
	CButton		m_AccumBottomUpRadio;
	CButton		m_AccumTopDownRadio;
};















