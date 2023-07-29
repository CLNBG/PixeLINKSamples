#pragma once


// CScrollBox

class CScrollBox : public CWnd
{
	DECLARE_DYNAMIC(CScrollBox)

public:
	CScrollBox(CWnd &wnd);
	virtual ~CScrollBox();
	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	DECLARE_MESSAGE_MAP()

private:
	CWnd& m_ContainedWnd;
public:
	void ForceIntoView(int vPosTop, int vPosBottom);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};




















