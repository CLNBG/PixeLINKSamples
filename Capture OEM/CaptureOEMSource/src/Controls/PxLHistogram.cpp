// PxLHistogram.cpp : Implementation of CPxLHistogram
#include "stdafx.h"
#include ".\PxLHistogram.h"
#include <math.h>

// CPxLHistogram

CPxLHistogram::CPxLHistogram()
	: m_drawWidth(0)
	, m_maxCount(1)
	, m_mean(0.0)
	, m_stddev(0.0)
	, m_memDC(NULL)
{
	// Set to identity trans.
	m_xform.eM11 = 1.0f;
	m_xform.eM12 = 0.0f;
	m_xform.eM21 = 0.0f;
	m_xform.eM22 = 1.0f;
	m_xform.eDx = 0.0f;
	m_xform.eDy = 0.0f;
}

	
STDMETHODIMP CPxLHistogram::SetHistogramData(LONG nDataPoints, LONG* pData)
{
	if (m_drawWidth <= 0) return S_OK;

	//m_data.resize(m_drawWidth);
	//std::fill(m_data.begin(), m_data.end(), 0);
	m_data.resize(nDataPoints);

	int max = 0, max2 = 0;
	double total = 0, nPts = 0;
	for (int i = 0; i < nDataPoints; i++)
	{
		nPts += pData[i];
		total += pData[i]*i;

		m_data[i] = pData[i];
		if (pData[i] > max)
		{
			max2 = max;
			max = pData[i];
		}
		else if (pData[i] > max2)
		{
			max2 = pData[i];
		}
	}

	m_stddev = m_mean = 0;
	if (nPts < 2)
		return S_OK;

	m_mean = total / nPts;

	for (int i = 0; i < nDataPoints; i++)
	{
		m_stddev += (i - m_mean) * (i - m_mean) * pData[i];
	}
	m_stddev /= (nPts - 1);
	m_stddev = sqrt(m_stddev);

	// Sometimes a very large number of the pixels have the same value (usuallly 0 or 255).
	// In this case, we let that spike at one value extend off the top of the screen,
	// and scale the histogram to the second most frequently occuring value.
	if (max > 2 * max2)
		m_maxCount = max2;
	else
		m_maxCount = max;

	if (m_maxCount == 0)
		m_maxCount = 1;

	FireViewChange();

	return S_OK;
}

HRESULT CPxLHistogram::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	// Set Clip region to the rectangle specified by di.prcBounds
	HRGN hRgnOld = NULL;
	if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
		hRgnOld = NULL;
	bool bSelectOldRgn = false;

	HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

	if (hRgnNew != NULL)
	{
		bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
	}



	//m_drawWidth = 1024;
	//while (m_drawWidth > WIDTH(rc)) m_drawWidth /= 2;
	m_drawWidth = WIDTH(rc);
	int drawHeight = rc.bottom - rc.top - 4/*space at top*/ - 20/*space for text at bottom*/;
	//int drawLeft = rc.left + ((WIDTH(rc) - m_drawWidth) / 2);
	int drawLeft = rc.left;

	Rectangle(di.hdcDraw, drawLeft, rc.top, drawLeft + m_drawWidth, rc.bottom - 20 /*leave space for text*/);

	if (m_data.size() > 0)
	{
		// We have data - draw the histogram.
		::MoveToEx(di.hdcDraw, drawLeft, rc.bottom-20, NULL);
		int n_dataPts = m_data.size();
		for (int x = 0; x < n_dataPts; x++)
		{
			::LineTo(di.hdcDraw,
					 drawLeft + (x * m_drawWidth / n_dataPts),
					 rc.bottom-21 - (m_data[x] * drawHeight / m_maxCount));
		}
	}

	::SetBkColor(di.hdcDraw, ::GetSysColor(COLOR_ACTIVEBORDER));
	RECT txtRect;
	txtRect.left	= drawLeft + 4;
	txtRect.right	= drawLeft + m_drawWidth/2 - 8;
	txtRect.top		= rc.bottom - 18;
	txtRect.bottom	= rc.bottom;
	TCHAR s[32] = {0};
	//_stprintf(s, _T("\x3BC:%4.2f"), m_mean);
	_stprintf(s, _T("mean: %4.2f"), m_mean);
	::DrawText(di.hdcDraw, s, _tcslen(s), &txtRect, DT_SINGLELINE);
	txtRect.left += m_drawWidth/2;
	txtRect.right += m_drawWidth/2;
	//_stprintf(s, _T("\x3C3:%3.3f"), m_stddev);
	_stprintf(s, _T("stdev: %3.3f"), m_stddev);
	::DrawText(di.hdcDraw, s, _tcslen(s), &txtRect, DT_SINGLELINE);




	if (bSelectOldRgn)
	{
		SelectClipRgn(di.hdcDraw, hRgnOld);
	}
	DeleteObject(hRgnNew);

	return S_OK;
}

LRESULT CPxLHistogram::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;
}
