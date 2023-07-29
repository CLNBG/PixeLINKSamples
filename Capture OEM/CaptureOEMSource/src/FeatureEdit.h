#pragma once


// CFeatureEdit

class CFeatureEdit : public CEdit
{
	DECLARE_DYNAMIC(CFeatureEdit)

public:
	CFeatureEdit();
	virtual ~CFeatureEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


