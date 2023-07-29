#pragma once


#include "FeatureDialog.h"


// CExtendedShutterDialog dialog

class CExtendedShutterDialog : public CFeatureDialog
{
	DECLARE_DYNAMIC(CExtendedShutterDialog)

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()

public:
	CExtendedShutterDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExtendedShutterDialog();

	enum { IDD = IDD_FEATUREEXTENDEDSHUTTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void Configure(void);
	virtual void Populate(void);

private:
	void SetKneePoints(void);

	int			m_MaxKnees;
	float		m_Exposure;

	CButton		m_KPCheck1;
	CStatic		m_KPMinLabel1;
	CSlider		m_KPSlider1;
	CStatic		m_KPMaxLabel1;
	CEdit		m_KPEdit1;

	CButton		m_KPCheck2;
	CStatic		m_KPMinLabel2;
	CSlider		m_KPSlider2;
	CStatic		m_KPMaxLabel2;
	CEdit		m_KPEdit2;

	CButton		m_KPCheck3;
	CStatic		m_KPMinLabel3;
	CSlider		m_KPSlider3;
	CStatic		m_KPMaxLabel3;
	CEdit		m_KPEdit3;

	CButton		m_KPCheck4;
	CStatic		m_KPMinLabel4;
	CSlider		m_KPSlider4;
	CStatic		m_KPMaxLabel4;
	CEdit		m_KPEdit4;

	afx_msg void OnScrollKneePointSlider1(void);
	afx_msg void OnChangeKneePointSlider1(void);
	afx_msg void OnKillFocusKneePointEdit1(void);
	afx_msg void OnClickKneePointCheck1(void);

	afx_msg void OnScrollKneePointSlider2(void);
	afx_msg void OnChangeKneePointSlider2(void);
	afx_msg void OnKillFocusKneePointEdit2(void);
	afx_msg void OnClickKneePointCheck2(void);

	afx_msg void OnScrollKneePointSlider3(void);
	afx_msg void OnChangeKneePointSlider3(void);
	afx_msg void OnKillFocusKneePointEdit3(void);
	afx_msg void OnClickKneePointCheck3(void);

	afx_msg void OnScrollKneePointSlider4(void);
	afx_msg void OnChangeKneePointSlider4(void);
	afx_msg void OnKillFocusKneePointEdit4(void);
	afx_msg void OnClickKneePointCheck4(void);

	afx_msg void OnUpdateKneePoint1(CCmdUI *pCmdUI);

	afx_msg void OnUpdateKneePointCheck2(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKneePoint2(CCmdUI *pCmdUI);

	afx_msg void OnUpdateKneePointCheck3(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKneePoint3(CCmdUI *pCmdUI);

	afx_msg void OnUpdateKneePointCheck4(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKneePoint4(CCmdUI *pCmdUI);
public:
	virtual BOOL OnInitDialog();
};
