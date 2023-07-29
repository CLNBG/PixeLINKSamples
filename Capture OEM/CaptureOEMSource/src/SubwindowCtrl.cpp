
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "PxLDevApp.h"
#include "DevAppPage.h"
#include "SubwindowCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSubwindowCtrl

IMPLEMENT_DYNCREATE(CSubwindowCtrl, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CSubwindowCtrl properties

/////////////////////////////////////////////////////////////////////////////
// CSubwindowCtrl operations
BEGIN_EVENTSINK_MAP(CSubwindowCtrl, CWnd)
	ON_EVENT(CSubwindowCtrl, IDC_RG_SUBWINDOW_CTRL, 2, SubwindowChanging, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CSubwindowCtrl, IDC_RD_SUBWINDOW_CTRL, 2, SubwindowChanging, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

// Non-member helper function
void SetEditVal(CWnd* pWnd, long val)
{
	CString s;
	if (pWnd != NULL)
	{
		s.Format(_T("%ld"), val);
		pWnd->SetWindowText(s);
	}
}

// Non-member helper function
void GetEditVal(CWnd* pWnd, long* pValue)
{
	ASSERT(NULL != pValue);
	CString s;
	if (pWnd != NULL)
	{
		pWnd->GetWindowText(s);
		if (s.GetLength() > 0) {
			_stscanf_s(s, _T("%ld"), pValue, sizeof(*pValue));
		}
	}
}


void CSubwindowCtrl::SubwindowChanging(long left, long top, unsigned long width, unsigned long height)
{
	this->UpdateEdits(left, top, width, height);
}

void CSubwindowCtrl::SetEdits(CWnd* pLeftEdit, CWnd* pTopEdit, CWnd* pWidthEdit, CWnd* pHeightEdit)
{
	m_pLeftEdit = pLeftEdit;
	m_pTopEdit = pTopEdit;
	m_pWidthEdit = pWidthEdit;
	m_pHeightEdit = pHeightEdit;
}


const int CSubwindowCtrl::WND_TOP		= 1<<0;
const int CSubwindowCtrl::WND_LEFT		= 1<<1;
const int CSubwindowCtrl::WND_HEIGHT	= 1<<2;
const int CSubwindowCtrl::WND_WIDTH		= 1<<3;

void CSubwindowCtrl::UpdateFromEdits(int which /*= WND_TOP | WND_LEFT | WND_WIDTH | WND_HEIGHT*/)
{
	long left, top, width, height;
	GetSubwindow(&left, &top, &width, &height);

	long newVal;

	if (which & WND_LEFT)
	{
		GetEditVal(m_pLeftEdit, &newVal);
		if (newVal != left)
			SetLeft(newVal);
	}
	if (which & WND_TOP)
	{
		GetEditVal(m_pTopEdit, &newVal);
		if (newVal != top)
			SetTop(newVal);
	}
	if (which & WND_WIDTH)
	{
		GetEditVal(m_pWidthEdit, &newVal);
		if (newVal != width)
			SetWidth(newVal);
	}
	if (which & WND_HEIGHT)
	{
		GetEditVal(m_pHeightEdit, &newVal);
		if (newVal != height)
			SetHeight(newVal);
	}

	UpdateEdits();

}

void CSubwindowCtrl::UpdateEdits(void)
{
	long left, top, width, height;
	GetSubwindow(&left, &top, &width, &height);
	UpdateEdits(left, top, width, height);
}

void CSubwindowCtrl::UpdateEdits(long left, long top, unsigned long width, unsigned long height)
{
	SetEditVal(m_pLeftEdit, left);
	SetEditVal(m_pTopEdit, top);
	SetEditVal(m_pWidthEdit, width);
	SetEditVal(m_pHeightEdit, height);
}


BEGIN_MESSAGE_MAP(CSubwindowCtrl, CWnd)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


BOOL CSubwindowCtrl::OnHelpInfo(HELPINFO* pHelpInfo)
{
	CDevAppPage* pPage = static_cast<CDevAppPage*>(this->GetParent());
	if (pPage->IsKindOf(RUNTIME_CLASS(CDevAppPage)))
		return pPage->OnHelpInfo(pHelpInfo);

//	return CWnd::OnHelpInfo(pHelpInfo);
	return FALSE;
}















