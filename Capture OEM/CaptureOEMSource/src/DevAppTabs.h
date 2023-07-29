#pragma once

#include <vector>

class CDevAppPage;


/******************************************************************************
* Class CDevAppTabs
******************************************************************************/

class CDevAppTabs : public CPropertySheet
{
	DECLARE_DYNAMIC(CDevAppTabs)

	DECLARE_MESSAGE_MAP()

public:
	CDevAppTabs(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDevAppTabs(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CDevAppTabs();

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void SetSingleTabMode(CDevAppPage* pPage);
	void SetMultiTabMode(int activeIndex);

private:
	HACCEL m_hAccellTable;

};













