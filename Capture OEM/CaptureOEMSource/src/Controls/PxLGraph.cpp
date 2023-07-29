// PxLGraph.cpp : Implementation of CPxLGraph
#include "stdafx.h"
#include ".\PxLGraph.h"
#include "Common.h"
#include <math.h>

/*
Properties:
	AutoScaleX
	AutoScaleY

Methods:
	AddElement
	RemoveElement
	Clear
	GetElementCount

	SetXRange
	SetYRange

	SetXYData
	SetYData
	SetPoint

	SetLineColor
	SetLineWidth

*/


template<typename T>
bool contains(T const& container, typename T::value_type val)
{
	return std::find(container.begin(), container.end(), val) != container.end();
}

// Specialization for std::map
template<typename T, typename U>
bool contains(std::map<T,U> const& container, T key)
{
	return container.find(key) != container.end();
}

static inline bool PointInRect(long x, long y, RECT& r)
{
	return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

static inline bool PointInRect(POINT& pt, RECT& r)
{
	return PointInRect(pt.x, pt.y, r);
}


GraphElement::GraphElement()
	: m_pen(NULL)
	, m_Xmin(0)
	, m_Xmax(0)
	, m_Ymin(0)
	, m_Ymax(0)
{
	m_lineColor = RGB(0,0,0);
	m_lineWidth = 0;
	ResetPen();
}

GraphElement::~GraphElement()
{
	if (m_pen != NULL)
		::DeleteObject(m_pen);
}

void GraphElement::ResetPen(void)
{
	if (m_pen != NULL)
		::DeleteObject(m_pen);
	m_pen = ::CreatePen(PS_SOLID, m_lineWidth, m_lineColor);
}

void GraphElement::SetLineColor(OLE_COLOR color)
{
	m_lineColor = color;
	ResetPen();
}

void GraphElement::SetLineWidth(long width)
{
	m_lineWidth = width;
	ResetPen();
}

void GraphElement::SetXYData(double* pXData, double* pYData, long nDataPoints)
{
	m_Xmin = m_Xmax = m_Ymin = m_Ymax = 0;

	m_Xdata.resize(nDataPoints);
	if (nDataPoints > 0)
		m_Xmin = m_Xmax = pXData[0];
	for (int i =0; i < nDataPoints; ++i)
	{
		double x = pXData[i];
		m_Xdata[i] = x;
		if (x < m_Xmin) m_Xmin = x;
		if (x > m_Xmax) m_Xmax = x;
	}
	m_Ydata.resize(nDataPoints);
	if (nDataPoints > 0)
		m_Ymin = m_Ymax = pYData[0];
	for (int i =0; i < nDataPoints; ++i)
	{
		double y = pYData[i];
		m_Ydata[i] = y;
		if (y < m_Ymin) m_Ymin = y;
		if (y > m_Ymax) m_Ymax = y;
	}
}

void GraphElement::SetYData(double* pData, long nDataPoints)
{
	m_Xmin = m_Xmax = m_Ymin = m_Ymax = 0;

	m_Xdata.resize(nDataPoints);
	for (int i = 0; i < nDataPoints; ++i)
		m_Xdata[i] = i;
	m_Xmin = 0; m_Xmax = nDataPoints-1;

	m_Ydata.resize(nDataPoints);
	if (nDataPoints > 0)
		m_Ymin = m_Ymax = pData[0];
	for (int i =0; i < nDataPoints; ++i)
	{
		double y = pData[i];
		m_Ydata[i] = y;
		if (y < m_Ymin) m_Ymin = y;
		if (y > m_Ymax) m_Ymax = y;
	}
}


// Some data to add to the graph by default, so there's something to see at design time.
static double defaultData[] = {0.0 , 15.643446504 , 30.9016994375 , 45.399049974 , 58.7785252292 , 70.7106781187 , 80.9016994375 , 89.1006524188 , 95.1056516295 , 98.7688340595 , 100.0 , 98.7688340595 , 95.1056516295 , 89.1006524188 , 80.9016994375 , 70.7106781187 , 58.7785252292 , 45.399049974 , 30.9016994375 , 15.643446504 , 0.0};

// CPxLGraph

CPxLGraph::CPxLGraph()
	: m_memDC(NULL)
	, m_plotBrush(NULL)
	, m_bgBrush(NULL)
	, m_gridPen(NULL)
	, m_bmp(NULL)
	, m_plotRegion(NULL)
	, m_tickFont(NULL)
	, m_mouseTrackPen(NULL)
	, m_bResized(true)
	, m_bDrawAxes(true)
	, m_bAutoScaleX(true)
	, m_bAutoScaleY(true)
	, m_bTrackMouse(false)
	, m_gridX(8)
	, m_gridY(8)
	, m_backColor(RGB(0xFF,0xFF,0xFF))
	, m_plotColor(RGB(0xE0,0xE0,0xE0)) // XXX - non-white for now, for testing
	, m_gridColor(RGB(0xC0,0xC0,0xC0))
	, m_xMinVal(0)
	, m_xMaxVal(20)
	, m_yMinVal(0)
	, m_yMaxVal(100)
	, m_xAxisMin(0)
	, m_xAxisMax(20)
	, m_yAxisMin(0)
	, m_yAxisMax(120)
{
	m_bWindowOnly = true;
}

HRESULT CPxLGraph::FinalConstruct()
{
	m_plotBrush = ::CreateSolidBrush(m_plotColor);
	m_bgBrush = ::CreateSolidBrush(m_backColor);
	m_gridPen = ::CreatePen(PS_SOLID, 0, m_gridColor);
	m_mouseTrackPen = ::CreatePen(PS_DOT, 0, RGB(0,0,0x80));
	m_toolTipBrush = ::GetSysColorBrush(COLOR_INFOBK);

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = 14; //-MulDiv(8, GetDeviceCaps(m_memDC, LOGPIXELSY), 72); // ???
	strcpy(lf.lfFaceName, "Arial");
	//lf.lfWeight = 400;
	m_tickFont = ::CreateFontIndirect(&lf);

	long dtEltId;
	this->AddElement(&dtEltId);
	this->SetYData(dtEltId, &defaultData[0], sizeof(defaultData)/sizeof(defaultData[0]));

	return S_OK;
}

void CPxLGraph::FinalRelease() 
{
	this->Clear();

	DeleteObject(m_plotBrush);
	DeleteObject(m_bgBrush);
	DeleteObject(m_gridPen);
	if (m_bmp != NULL)
		DeleteObject(m_bmp);
	if (m_plotRegion != NULL)
		DeleteObject(m_plotRegion);
	DeleteObject(m_tickFont);
	DeleteObject(m_mouseTrackPen);
}


HRESULT CPxLGraph::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;

	if (m_memDC == NULL)
	{
		// First time drawing
		m_memDC = ::CreateCompatibleDC(di.hdcDraw);
	}

	RecalcScales(); // May set m_bDrawAxes to true

	if (m_bResized)
	{
		if (m_bmp != NULL)
			::DeleteObject(m_bmp);
		m_bmp = ::CreateCompatibleBitmap(di.hdcDraw, WIDTH(rc), HEIGHT(rc));
		::SelectObject(m_memDC, m_bmp);

		// If we have resized, then we also need to redraw the non-plot area.
		m_bDrawAxes = true;
	}

	if (m_bDrawAxes) // Will be true on the first call to OnDraw, and after rescaling or resizing.
	{
		this->DrawAxes(rc); // Also recalculates m_plotRect.
		//hRgnOld = 
		if (m_plotRegion != NULL)
			DeleteObject(m_plotRegion);
		m_plotRegion = ::CreateRectRgn(m_plotRect.left, m_plotRect.top, m_plotRect.right, m_plotRect.bottom);
	}

	// Draw the graph.

	// Plot area background:
	::FillRect(m_memDC, &m_plotRect, m_plotBrush);
	// Border around plot area
	::SelectObject(m_memDC, ::GetStockObject(BLACK_PEN));
	::Rectangle(m_memDC, m_plotRect.left-1, m_plotRect.top-1, m_plotRect.right+1, m_plotRect.bottom+1);

	// While drawing the graph, we want to use the plot area as the clipping region.
	::SelectClipRgn(m_memDC, m_plotRegion);

	// Grid:
	this->DrawGrid(m_plotRect);

	// Plot lines:
	double xscale = WIDTH(m_plotRect) / (m_xAxisMax - m_xAxisMin);
	double yscale = HEIGHT(m_plotRect) / (m_yAxisMax - m_yAxisMin);
	for (eltMap_t::const_iterator it = m_eltMap.begin(); it != m_eltMap.end(); it++)
	{
		GraphElement* pElt = it->second;
		if (pElt->m_Xdata.size() == 0)
			continue;
		::SelectObject(m_memDC, pElt->m_pen);
		double* px = &pElt->m_Xdata[0];
		double* py = &pElt->m_Ydata[0];

		::MoveToEx(
			m_memDC,
			m_plotRect.left + ((px[0] - m_xAxisMin) * xscale),
			m_plotRect.bottom - ((py[0] - m_yAxisMin) * yscale), NULL);
		for (int ptNo = 0; ptNo < pElt->m_Xdata.size(); ++ptNo)
		{
			::LineTo(
				m_memDC,
				m_plotRect.left + ((px[ptNo] - m_xAxisMin) * xscale),
				m_plotRect.bottom - ((py[ptNo] - m_yAxisMin) * yscale));
		}
	}

	// Restore the clip region of our memory DC
	::SelectClipRgn(m_memDC, NULL);

	m_bResized = false;
	m_bDrawAxes = false;

	// Draw the mouse-tracking lines
	if (m_bTrackMouse && PointInRect(m_mouseX, m_mouseY, m_plotRect))
	{
		char label[32];
		::SelectObject(m_memDC, m_mouseTrackPen);
		::SelectObject(m_memDC, m_tickFont);

		// Draw the dotted lines
		::MoveToEx(m_memDC, m_plotRect.left, m_mouseY, NULL);
		::LineTo(m_memDC, m_plotRect.right, m_mouseY);
		::MoveToEx(m_memDC, m_mouseX, m_plotRect.top, NULL);
		::LineTo(m_memDC, m_mouseX, m_plotRect.bottom);

		// Draw the tool-tip boxes for the x and y coordinates of the mouse.
		::SelectObject(m_memDC, ::GetStockObject(BLACK_PEN));
		::SelectObject(m_memDC, m_toolTipBrush);
		COLORREF oldBkColor = ::SetBkColor(m_memDC, ::GetSysColor(COLOR_INFOBK));
		SIZE szLabel;
		RECT rcLabel;

		// Y-label
		::SetTextAlign(m_memDC, TA_RIGHT | TA_BASELINE);
		sprintf(label, "%lg", m_yAxisMax - ((m_yAxisMax-m_yAxisMin) * (m_mouseY-m_plotRect.top) / (HEIGHT(m_plotRect))));
		::GetTextExtentPoint32(m_memDC, label, strlen(label), &szLabel);
		rcLabel.right = m_plotRect.left-4;
		rcLabel.left = max(min(m_plotRect.left-6-szLabel.cx-3, rcLabel.right-60),rcLabel.right-68);
		rcLabel.top = m_mouseY - (szLabel.cy/2-1);
		rcLabel.bottom = m_mouseY + (szLabel.cy/2 + 3);
		::Rectangle(m_memDC, rcLabel.left, rcLabel.top, rcLabel.right, rcLabel.bottom);
		::ExtTextOut(m_memDC, m_plotRect.left-7, m_mouseY+6, 0, NULL, label, strlen(label), NULL);

		// X-label
		::SetTextAlign(m_memDC, TA_CENTER | TA_TOP);
		sprintf(label, "%lg", m_xAxisMin + ((m_xAxisMax-m_xAxisMin) * (m_mouseX-m_plotRect.left) / (WIDTH(m_plotRect))));
		::GetTextExtentPoint32(m_memDC, label, strlen(label), &szLabel);
		rcLabel.right = m_mouseX + szLabel.cx/2 + 3;
		rcLabel.left = m_mouseX - szLabel.cx/2 - 4;
		rcLabel.top = m_plotRect.bottom + 4;
		rcLabel.bottom = rcLabel.top + szLabel.cy + 4;
		::Rectangle(m_memDC, rcLabel.left, rcLabel.top, rcLabel.right, rcLabel.bottom);
		::ExtTextOut(m_memDC, m_mouseX, m_plotRect.bottom + 6, 0, NULL, label, strlen(label), NULL);

		::SetBkColor(m_memDC, oldBkColor);
		m_bDrawAxes = true; // We will need to redraw the axes on the next OnDraw, since we have drawn the tool-tips over top of them.
	}

	if (true)//m_bDrawAxes)
	{
		// Blit the whole thing
		::BitBlt(
				di.hdcDraw,
				rc.left,
				rc.top,
				WIDTH(rc),
				HEIGHT(rc),
				m_memDC,
				0,
				0,
				SRCCOPY);
	}
	else // XXX - how to find out whether the axes/labels/etc need to be redrawn?
	{
		// Just blit the plot area (the area outside the plot, eg: axes & labels, have not changed).
		::BitBlt(
				di.hdcDraw,
				m_plotRect.left,
				m_plotRect.top,
				WIDTH(m_plotRect),
				HEIGHT(m_plotRect),
				m_memDC, 
				m_plotRect.left,
				m_plotRect.top,
				SRCCOPY);
	}


	return S_OK;
}


void CPxLGraph::DrawAxes(RECT const& rcBounds)
{
	// XXX
	m_plotRect.left = rcBounds.left + 72;
	m_plotRect.right = rcBounds.right - 20;
	m_plotRect.top = rcBounds.top + 16;
	m_plotRect.bottom = rcBounds.bottom - 40;

	::SelectObject(m_memDC, m_bgBrush);
	//::FillRect(m_memDC, &rcBounds, m_bgBrush);
	::SelectObject(m_memDC, ::GetStockObject(BLACK_PEN));
	::Rectangle(m_memDC, 0, 0, rcBounds.right-1, rcBounds.bottom-1);

	//HFONT oldFont = (HFONT)::SelectObject(m_memDC, m_tickFont);
	::SelectObject(m_memDC, m_tickFont);
	char label[32] = {0};
	::SetTextAlign(m_memDC, TA_RIGHT | TA_BASELINE);
	for (int y = 0; y <= m_gridY; ++y)
	{
		int y_pos = m_plotRect.top + (HEIGHT(m_plotRect) * y / m_gridY );
		::MoveToEx(m_memDC, m_plotRect.left-4, y_pos, NULL);
		::LineTo(m_memDC, m_plotRect.left, y_pos);
		sprintf(label, "%lg", m_yAxisMax - ((m_yAxisMax-m_yAxisMin) * y / m_gridY));
		::TextOut(m_memDC, m_plotRect.left-6, y_pos+6, label, strlen(label));
	}
	::SetTextAlign(m_memDC, TA_CENTER | TA_TOP);
	for (int x = 0; x <= m_gridX; ++x)
	{
		int x_pos = m_plotRect.left + (WIDTH(m_plotRect) * x / m_gridX );
		::MoveToEx(m_memDC, x_pos, m_plotRect.bottom, NULL);
		::LineTo(m_memDC, x_pos, m_plotRect.bottom+4);
		sprintf(label, "%lg", m_xAxisMin + ((m_xAxisMax-m_xAxisMin+1) * x / m_gridX));
		::TextOut(m_memDC, x_pos, m_plotRect.bottom+6, label, strlen(label));
	}
	//::SelectObject(m_memDC, oldFont);
}


void CPxLGraph::DrawGrid(RECT const& rcPlot)
{
	HPEN oldPen = (HPEN)::SelectObject(m_memDC, m_gridPen);
	for (int y = 1; y < m_gridY; ++y)
	{
		int y_pos = rcPlot.top + (HEIGHT(rcPlot) * y / m_gridY );
		::MoveToEx(m_memDC, rcPlot.left, y_pos, NULL);
		::LineTo(m_memDC, rcPlot.right, y_pos);
	}
	for (int x = 1; x < m_gridX; ++x)
	{
		int x_pos = rcPlot.left + (WIDTH(rcPlot) * x / m_gridX );
		::MoveToEx(m_memDC, x_pos, rcPlot.top, NULL);
		::LineTo(m_memDC, x_pos, rcPlot.bottom);
	}
	::SelectObject(m_memDC, oldPen);
}


void CPxLGraph::RecalcScales(void)
{
	RecalcExtrema(); // Sets m_xMinVal, m_xMaxVal, ...etc

	if (m_bAutoScaleX)
	{
		// Set the X scale so that the graph fits exactly end-to-end.
		if (m_xAxisMin != m_xMinVal || m_xAxisMax != m_xMaxVal)
			m_bDrawAxes = true;
		m_xAxisMin = m_xMinVal;
		m_xAxisMax = m_xMaxVal;
	}
	if (m_bAutoScaleY)
	{
		// For the Y scale, we try to be a bit more clever, so that in the common
		// case (I assume), where Y data gets updated more frequently, the scale
		// does not change on every update. (eg: Histogram of video data)
		if (m_yAxisMin != 0)
		{
			m_yAxisMin = 0;
			m_bDrawAxes = true;
		}
		if (m_yMaxVal > m_yAxisMax || m_yMaxVal < (m_yAxisMax * 0.45))
		{
			// Rescale Y.
			if ((m_yMaxVal - m_yMinVal) == 0.0)
				return; // Leave at the current scale.
			int mag = static_cast<int>(log10(m_yMaxVal - m_yMinVal));
			/*
			while (val > 10.0)
			{
				val /= 10.0;
				mag += 1;
			}
			while (val < 1.0)
			{
				val *= 10.0;
				mag -= 1;
			}
			*/
			double val = pow(10.0, mag);
			while (val < (m_yMaxVal - m_yMinVal))
				val *= 2.0;

			if (val != m_yAxisMax)
			{
				m_yAxisMax = m_yAxisMin + val;
				m_bDrawAxes = true;
			}
		}
	}
}


void CPxLGraph::UpdateExtrema(GraphElement const& elt)
{
	if (elt.m_Xmin < m_xMinVal)
		m_xMinVal = elt.m_Xmin;
	if (elt.m_Xmax > m_xMaxVal)
		m_xMaxVal = elt.m_Xmax;
	if (elt.m_Ymin < m_yMinVal)
		m_yMinVal = elt.m_Ymin;
	if (elt.m_Ymax > m_yMaxVal)
		m_yMaxVal = elt.m_Ymax;
}

void CPxLGraph::RecalcExtrema(void)
{
	// Called after an element is deleted.
	if (m_eltMap.size() > 0)
	{
		GraphElement* pElt = m_eltMap.begin()->second;
		m_xMinVal = pElt->m_Xmin;
		m_xMaxVal = pElt->m_Xmax;
		m_yMinVal = pElt->m_Ymin;
		m_yMaxVal = pElt->m_Ymax;

		for (eltMap_t::const_iterator it = m_eltMap.begin(); it != m_eltMap.end(); it++)
		{
			UpdateExtrema(*it->second);
		}
	}
	// else: don't change anything
}


STDMETHODIMP CPxLGraph::AddElement(LONG* elementId)
{
	static long uniqueId = 0;
	GraphElement* newElt = new GraphElement;
	long newId = uniqueId++;
	m_eltMap[newId] = newElt;
	if (elementId != NULL)
		*elementId = newId;
	return S_OK;
}

STDMETHODIMP CPxLGraph::RemoveElement(LONG elementNumber)
{
	if (!contains(m_eltMap, elementNumber))
		return E_INVALIDARG;

	delete m_eltMap[elementNumber];
	m_eltMap.erase(elementNumber);
	return S_OK;
}

STDMETHODIMP CPxLGraph::Clear(void)
{
	for (eltMap_t::const_iterator it = m_eltMap.begin(); it != m_eltMap.end(); it++)
		delete it->second;
	m_eltMap.clear();
	return S_OK;
}

STDMETHODIMP CPxLGraph::GetElementCount(LONG* elementCount)
{
	*elementCount = m_eltMap.size();
	return S_OK;
}

STDMETHODIMP CPxLGraph::SetXRange(DOUBLE min, DOUBLE max)
{
	m_xAxisMin = min;
	m_xAxisMax = max;
	m_bAutoScaleX = false;
	m_bDrawAxes = true; // Redraw axes in next OnDraw

	return S_OK;
}

STDMETHODIMP CPxLGraph::SetYRange(DOUBLE min, DOUBLE max)
{
	m_yAxisMin = min;
	m_yAxisMax = max;
	m_bAutoScaleY = false;
	m_bDrawAxes = true; // Redraw axes in next OnDraw

	return S_OK;
}

STDMETHODIMP CPxLGraph::SetXYData(LONG elementNumber, DOUBLE* xData, DOUBLE* yData, LONG nPoints)
{
	if (!contains(m_eltMap, elementNumber))
		return E_INVALIDARG;

	m_eltMap[elementNumber]->SetXYData(xData, yData, nPoints);
	return S_OK;
}

STDMETHODIMP CPxLGraph::SetYData(LONG elementNumber, DOUBLE* yData, LONG nPoints)
{
	if (!contains(m_eltMap, elementNumber))
		return E_INVALIDARG;

	m_eltMap[elementNumber]->SetYData(yData, nPoints);
	return S_OK;
}

STDMETHODIMP CPxLGraph::SetLineColor(LONG elementNumber, OLE_COLOR color)
{
	if (!contains(m_eltMap, elementNumber))
		return E_INVALIDARG;

	m_eltMap[elementNumber]->SetLineColor(color);
	return S_OK;
}

STDMETHODIMP CPxLGraph::SetLineWidth(LONG elementNumber, LONG width)
{
	if (!contains(m_eltMap, elementNumber))
		return E_INVALIDARG;

	m_eltMap[elementNumber]->SetLineWidth(width);
	return S_OK;
}


LRESULT CPxLGraph::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;
}


LRESULT CPxLGraph::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bResized = m_bDrawAxes = true;
	return 0;
}


LRESULT CPxLGraph::OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_mouseX = m_mouseY = 0;
	m_bDrawAxes = true;
	return 0;
}

LRESULT CPxLGraph::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	long x = GET_X_LPARAM(lParam);
	long y = GET_Y_LPARAM(lParam);

	if (m_bTrackMouse && PointInRect(x, y, m_plotRect))
	{
		// Make sure we are notified when the mouse leaves the control.
		TRACKMOUSEEVENT tme;
		memset(&tme, 0, sizeof(tme));
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 0;
		tme.hwndTrack = this->m_hWnd;
		::TrackMouseEvent(&tme);

		m_mouseX = x;
		m_mouseY = y;
		m_bDrawAxes = true;
		this->Invalidate(FALSE);
	}
	else
	{
		m_mouseX = m_mouseY = 0;
	}

	return 0;
}

STDMETHODIMP CPxLGraph::get_TrackMouse(VARIANT_BOOL* pVal)
{
	*pVal = m_bTrackMouse ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CPxLGraph::put_TrackMouse(VARIANT_BOOL newVal)
{
	m_bTrackMouse = (newVal == VARIANT_TRUE);
	return S_OK;
}

STDMETHODIMP CPxLGraph::get_AutoScaleX(VARIANT_BOOL* pVal)
{
	*pVal = m_bAutoScaleX ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CPxLGraph::put_AutoScaleX(VARIANT_BOOL newVal)
{
	m_bAutoScaleX = (newVal == VARIANT_TRUE);
	m_bDrawAxes = true;
	return S_OK;
}

STDMETHODIMP CPxLGraph::get_AutoScaleY(VARIANT_BOOL* pVal)
{
	*pVal = m_bAutoScaleY ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CPxLGraph::put_AutoScaleY(VARIANT_BOOL newVal)
{
	m_bAutoScaleY = (newVal == VARIANT_TRUE);
	m_bDrawAxes = true;
	return S_OK;
}

STDMETHODIMP CPxLGraph::GetXRange(DOUBLE* pMin, DOUBLE* pMax)
{
	*pMin = m_xAxisMin;
	*pMax = m_xAxisMax;

	return S_OK;
}

STDMETHODIMP CPxLGraph::GetYRange(DOUBLE* pMin, DOUBLE* pMax)
{
	*pMin = m_yAxisMin;
	*pMax = m_yAxisMax;

	return S_OK;
}
