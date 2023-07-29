// PxLLookup.cpp : Implementation of CPxLLookup
#include "stdafx.h"
#include ".\PxLLookup.h"
#include <algorithm>

POINT PT(long x, long y) { POINT p; p.x=x; p.y=y; return p; }


void DrawX(HDC hdc, int x, int y, int size)
{
	MoveToEx(hdc, x - size, y - size, NULL);
	LineTo(hdc, x + size+1, y + size+1);
	MoveToEx(hdc, x - size, y + size, NULL);
	LineTo(hdc, x + size+1, y - size-1);
	//Ellipse(hdc, x - size, y - size, x + size, y + size);
}

// CPxLLookup

#ifdef _DEBUG
bool CPxLLookup::Dump(void)
{
	char foo[200];
	for(int i = 0; i < m_points.size(); i++)
	{
		sprintf(foo, "ERROR:\nInvalid data passid to PolyDraw:\n%s : %d, %d\nClick OK to continue, or Cancel to abort.",
				m_types[i]==PT_LINETO ? "LINETO  " : m_types[i]==PT_BEZIERTO ? "BEZIERTO" : "MOVETO",
				m_points[i].x, m_points[i].y);
	}
	return MessageBox(foo, _T("Oops..."), MB_OKCANCEL) == IDOK;
}
#endif

STDMETHODIMP CPxLLookup::Reset(void)
{
	m_points.clear();
	m_types.clear();
	m_controlpoints.clear();
	AddLine(m_rectLimits.left, m_rectLimits.bottom, m_rectLimits.right, m_rectLimits.top);
	return S_OK;
}

BOOL MyPolyDraw(HDC hdc, POINT const* pt, BYTE const* byte, int nPts)
{
	//return ::PolyDraw(hdc, pt, byte, nPts); // Not available on Win98.
	int nDone = 0;
	while (nDone < nPts)
	{
		if (*byte == PT_MOVETO)
		{
			if (!::MoveToEx(hdc, pt->x, pt->y, NULL))
				return FALSE;
			++byte;
			++pt;
			++nDone;
		}
		else if (*byte == PT_LINETO)
		{
			if (!::LineTo(hdc, pt->x, pt->y))
				return FALSE;
			++byte;
			++pt;
			++nDone;
		}
		else if (*byte == PT_BEZIERTO)
		{
			if (!::PolyBezierTo(hdc, pt, 3))
				return FALSE;
			++byte;  ++byte;  ++byte; 
			++pt;    ++pt;    ++pt;   
			++nDone; ++nDone; ++nDone; 
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

// Change all the vertex coordinates to fit into the new size of the control.
void CPxLLookup::Rescale(void)
{
	RECT oldLimits = {m_points.front().x,		// left
					  m_points.back().y,		// top
					  m_points.back().x,		// right
					  m_points.front().y};		// botton

	double xscale = static_cast<double>(WIDTH(m_rectLimits)) / WIDTH(oldLimits);
	double yscale = static_cast<double>(WIDTH(m_rectLimits)) / WIDTH(oldLimits);

	for (int i = 0; i < m_points.size(); ++i)
	{
		m_points[i].x = m_rectLimits.left + xscale * (m_points[i].x - oldLimits.left);
		m_points[i].y = m_rectLimits.top + yscale * (m_points[i].y - oldLimits.top);

		// Just to be safe...
		if (m_points[i].x < m_rectLimits.left)
			m_points[i].x = m_rectLimits.left;
		if (m_points[i].y < m_rectLimits.top)
			m_points[i].y = m_rectLimits.top;
		if (m_points[i].x > m_rectLimits.right)
			m_points[i].x = m_rectLimits.right;
		if (m_points[i].y > m_rectLimits.bottom)
			m_points[i].y = m_rectLimits.bottom;
	}
}


HRESULT CPxLLookup::OnDraw(ATL_DRAWINFO& di)
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

	// m_rectLimits defines the limits of where graph vertices can be moved to.
	m_rectLimits.left = rc.left + m_pointSize + m_nBorderWidth; // Account for border-line thickness.
	m_rectLimits.top = rc.top + m_pointSize + m_nBorderWidth;
	m_rectLimits.right = rc.right - m_pointSize - m_nBorderWidth;
	m_rectLimits.bottom = rc.bottom - m_pointSize - m_nBorderWidth;

	// If resized, reset everything. Should only happen in design mode, I think.
	if (m_bResized)
	{
		Rescale();
		m_bResized = false;
	}

	// Draw this bad boy...
	// First the background:
	HPEN hOldPen = (HPEN)SelectObject(di.hdcDraw, m_hBorderPen);
	Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom); // Draws the outer border

	// The border:
	HBRUSH hOldBrush = (HBRUSH)SelectObject(di.hdcDraw, m_bEnabled ? m_hBGBrush : m_hDisabledBGBrush);
	SelectObject(di.hdcDraw, m_hInsideBorderPen);
	Rectangle(di.hdcDraw, rc.left + m_nBorderWidth, rc.top + m_nBorderWidth, rc.right - m_nBorderWidth, rc.bottom - m_nBorderWidth);
	
	// Draw the lines and curves:
	SelectObject(di.hdcDraw, m_bEnabled ? m_hGraphPen : m_hDisabledGraphPen);
	ATLASSERT(m_points.size() == m_types.size());
	bool ok = MyPolyDraw(di.hdcDraw, &m_points[0], &m_types[0], static_cast<int>(m_types.size()));
	ATLASSERT(ok || Dump());

	// Only draw the points when enabled.
	if (m_bEnabled)
	{
		// Pen for inactive points
		SelectObject(di.hdcDraw, m_hPointPen);
		// Brush for drawing boxes - make the interior transparent:
		//SelectObject(di.hdcDraw, GetStockObject(HOLLOW_BRUSH)); // decided it looks better without this

		for ( int i = 0 ; i < m_points.size() ; i++ )
		{
			HPEN temp;
			if (i == m_activePoint)
			{
				// We draw the active point (the one the mouse is over) differently.
				temp = (HPEN)SelectObject(di.hdcDraw, m_hActivePointPen);
				if (m_controlpoints[i]) // We draw control points as an X
				{
					DrawX(di.hdcDraw, m_points[i].x, m_points[i].y, m_pointSize);
				}
				else // regular point - draw as a box
				{
					Rectangle(di.hdcDraw, 
							  m_points[i].x - m_pointSize, 
							  m_points[i].y - m_pointSize, 
							  m_points[i].x + m_pointSize, 
							  m_points[i].y + m_pointSize);
				}
				SelectObject(di.hdcDraw, temp);
			}
			else
			{
				if (m_controlpoints[i]) // We draw control points as an X
				{
					DrawX(di.hdcDraw, m_points[i].x, m_points[i].y, m_pointSize);
				}
				else
				{
					Rectangle(di.hdcDraw,
							  m_points[i].x - m_pointSize, 
							  m_points[i].y - m_pointSize, 
							  m_points[i].x + m_pointSize, 
							  m_points[i].y + m_pointSize);
				}
			}
		}
	} // if (m_bEnabled)

	// Restore old stuff to the DC.
	SelectObject(di.hdcDraw, hOldPen);
	SelectObject(di.hdcDraw, hOldBrush);
	if (bSelectOldRgn)
	{
		SelectClipRgn(di.hdcDraw, hRgnOld);
	}
	DeleteObject(hRgnNew);

	return S_OK;
}



// This is how we get the array of points to pass to the PxLApi SetFeature call.
// TODO - I'm pretty sure that almost all the complexity of this function could
// be eliminated by using an appropriate Windows GDI Coordinate Transform, but
// I just haven't found the time to learn how to use them yet...
STDMETHODIMP CPxLLookup::FillTable(LONG length, LONG* table)
{
	if (length < 1 || table == NULL)
		return E_INVALIDARG;

	HDC hdc = this->GetDC();

	BeginPath(hdc);
	MyPolyDraw(hdc, &m_points[0], &m_types[0], static_cast<int>(m_types.size()));
	EndPath(hdc);

	// Turn curves into a series of line segments:
	FlattenPath(hdc);

	int nPoints = GetPath(hdc, NULL, NULL, 0);

	ATLASSERT(nPoints >= 2);

	std::vector<POINT> pts(nPoints);
	std::vector<BYTE> bts(nPoints);
	GetPath(hdc, &pts[0], &bts[0], nPoints);

	// Ratio of the external size (eg: 1024x1024) to the screen-size (in pixels).
	double xRatio = static_cast<double>(length-1) / (m_rectLimits.right - m_rectLimits.left);
	double yRatio = static_cast<double>(length-1) / (m_rectLimits.bottom - m_rectLimits.top);

	int pt = 1; // Index of the right-hand of the two points defining the line we are using.
	double slope = static_cast<double>(pts[pt].y - pts[pt-1].y) / (pts[pt].x - pts[pt-1].x);
	long segmentBase = pts[pt-1].y - m_rectLimits.top; // Y-coord at start of current line.
	for (int i = 0; i < length; i++)
	{
		// This needs to be a "while", not an "if" - multiple points can be at the same x-coord.
		while (i >= xRatio * (pts[pt].x - m_rectLimits.left) && pts[pt].x < pts.back().x)
		{
			// We have moved far enough right to start using the next line segment.
			pt++;
			slope = static_cast<double>(pts[pt].y - pts[pt-1].y) / (pts[pt].x - pts[pt-1].x);
			segmentBase = pts[pt-1].y - m_rectLimits.top; // Y-coord at start of current line.
		}

		// TODO - rewrite for maintainability. The following line in particular is pretty incomprehensible:
		table[i] = length - 1 - static_cast<long>(yRatio * (segmentBase + (slope * (i/xRatio - (pts[pt-1].x - m_rectLimits.left))) + 0.01 ));

		ATLASSERT(table[i] >= 0 && table[i] < length);
		ATLASSERT(pts.size() != 2 || table[i] == i); // Assert that identity transform works correctly.
	}

	return S_OK;
}


void CPxLLookup::AddLine(long xStart, long yStart, long xEnd, long yEnd)
{
	m_types.push_back(PT_MOVETO);
	m_points.push_back(PT(xStart,yStart));
	m_controlpoints.push_back(false);
	m_types.push_back(PT_LINETO);
	m_points.push_back(PT(xEnd,yEnd));
	m_controlpoints.push_back(false);
}


LRESULT CPxLLookup::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (!m_bEnabled)
		return 0;

	if (m_bDragging && (!wParam & MK_LBUTTON))
	{
		// Somehow we missed recieving a LButtonUp event (eg: debugging can cause this)
		EndDrag();
	}

	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	// m_bNewPoint indicates that a point has just been "split" (dragged with CTRL key down).
	// When this happens, we assume that it will be dragged to the left, so we set m_actitivePoint
	// to the index of the *first* (left-most) of the two identical points. If the user begins
	// dragging to the right instead, we simply set m_activePoint to the second of the two now.
	if (m_bNewPoint)
	{
		if (m_points[m_activePoint].x == m_rectLimits.right)
		{
			// Last point can only go left...
			m_bNewPoint = false;
		}
		else if (x - m_MouseOffsetLeft > m_points[m_activePoint].x + 2)
		{
			// Point has been dragged a significant distance to the RIGHT...
			m_points[m_activePoint] = m_points[m_activePoint + 1];
			m_activePoint += 1;
			m_bNewPoint = false;
		}
		else if (x - m_MouseOffsetLeft < m_points[m_activePoint].x - 2)
		{
			// Point has been dragged a significant distance to the LEFT...
			m_bNewPoint = false;
		}
		// else: Point has not been dragged far enough left or right to know yet which way the user intends to go.
	}

	if (m_bDragging)
	{
		// We are dragging a graph point
		int newX = x - m_MouseOffsetLeft;
		int newY = y - m_MouseOffsetTop;
		m_points[m_activePoint].x = max(min(newX, m_points[m_activePoint+1].x), m_points[m_activePoint-1].x);
		m_points[m_activePoint].y = max(min(newY, m_rectLimits.bottom), m_rectLimits.top);
		FireViewChange();
		Fire_GraphChanging();
	}
	else
	{
		// Not dragging
		int oldActivePoint = m_activePoint;
		m_activePoint = GetPointAt(x, y, 3);
		if (m_activePoint != oldActivePoint)
			FireViewChange();
	}

	return 0;
}

// Return the index (in m_points) of the point that x,y is on (within +/- slop);
int CPxLLookup::GetPointAt(int x, int y, int slop)
{
	int ret = -1; // == not over any point.

	for ( int i = 0; i < m_points.size() ; i++ )
	{
		if (x < m_points[i].x - slop)
			break; // Get out - all the rest of the points are even *farther* right, so don't bother.
		if (x >= m_points[i].x - slop 
			&& x <= m_points[i].x + slop
			&& y >= m_points[i].y - slop
			&& y <= m_points[i].y + slop)
		{
			// When a control point is on top of a regular point, we prefer to drag the control point.
			// Also, when a point is on top of the end-point, move the non-end point rather than creating a point.
			if (m_controlpoints[i] || m_points[i].x == m_points.back().x)
				return i;
			ret =  i;
		}
	}
	return ret;
}


void CPxLLookup::AddPoint(int idx, POINT pt, bool isControl)
{
	m_points.insert(m_points.begin() + idx, pt);
	m_types.insert(m_types.begin() + idx, isControl ? PT_BEZIERTO : PT_LINETO);
	if (isControl) m_types[idx+1] = PT_BEZIERTO; // Always insert control points in pairs!
	m_controlpoints.insert(m_controlpoints.begin() + idx, true);
}


LRESULT CPxLLookup::OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (m_bEnabled && m_activePoint != -1)
	{
		ATLASSERT(m_activePoint >= 0 && m_activePoint < m_points.size());

		this->SetCapture();
		m_MouseOffsetLeft = GET_X_LPARAM(lParam) - m_points[m_activePoint].x;
		m_MouseOffsetTop = GET_Y_LPARAM(lParam) - m_points[m_activePoint].y;

		// If we are dragging one of the end points, duplicate the point.
		if (m_activePoint == 0)
		{
			// Duplicate the first (bottom left) point
			m_points.insert(m_points.begin(), m_points[0]); // Duplicate first point
			m_types.insert(m_types.begin() + 1, PT_LINETO);
			m_controlpoints.insert(m_controlpoints.begin() + 1, false);
			m_activePoint = 1; // Drag the new point - first point never moves.
		}
		else if (m_activePoint == m_points.size() - 1)
		{
			// Duplicate the last (top right) point
			m_points.insert(m_points.end(), m_points.back());
			m_types.insert(m_types.end(), PT_LINETO);
			m_controlpoints.insert(m_controlpoints.end(), false);
		}
		else if (wParam & MK_CONTROL)
		{
			// Starting a drag with CTRL key held == duplicate point.
			// We assume (50/50 guess) that the user will drag to the LEFT.
			// If not, we sort it all out in the next call to OnMouseMove.
			m_points.insert(m_points.begin() + m_activePoint, m_points[m_activePoint]);
			m_types.insert(m_types.begin() + m_activePoint, m_types[m_activePoint]);
			m_types[m_activePoint + 1] = PT_LINETO; // Line from new point to duplicated point...
			m_controlpoints.insert(m_controlpoints.begin() + m_activePoint, false);
			m_bNewPoint = true;
		}

		m_bDragging = true;
	}

	return 0;
}

LRESULT CPxLLookup::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_bDragging)
	{
		EndDrag();
	}

	return 0;
}

void CPxLLookup::RemovePoint(int idx)
{
	bool curveJoin = !m_controlpoints[idx] && m_controlpoints[idx + 1] && m_controlpoints[idx - 1];

	m_points.erase(m_points.begin() + idx);
	// The following line merits some explanation - but I don't have time.
	// Lets just say that wonky things can happen if you remove a point that joins a line to a curve.
	m_types.erase(m_types.begin() + idx + (m_types[idx] == PT_BEZIERTO ? 1 : 0));
	m_controlpoints.erase(m_controlpoints.begin() + idx);

	if (curveJoin)
	{
		// Point that joins two curves. We need to delete two control points too.
		RemovePoint(idx);
		RemovePoint(idx-1);
	}
}

bool Overlapping(POINT p1, POINT p2)
{
	return abs(p1.x - p2.x) < 4 && abs(p1.y - p2.y) < 4;
}

// Handle the end of a drag operation on a point.
void CPxLLookup::EndDrag(void)
{
	ReleaseCapture();
	m_bDragging = false;
	m_bNewPoint = false; // In case user did mouse-down / mouse-up with no intervening mouse-move.

	// If the point is more-or-less on top of another point, merge them.
	if (Overlapping(m_points[m_activePoint], m_points[m_activePoint - 1]) 
			&& !m_controlpoints[m_activePoint] 
			&& !m_controlpoints[m_activePoint - 1]
		|| Overlapping(m_points[m_activePoint], m_points[m_activePoint + 1])
			&& !m_controlpoints[m_activePoint]
			&& !m_controlpoints[m_activePoint + 1])
	{
		RemovePoint(m_activePoint);
	}

	m_activePoint = -1;
	FireViewChange();
	Fire_GraphChanged();
}


// Helper functions used by SetFromTable
bool closeEnough(double a, double b) { return abs(b-a) <= 2.0; }
void markpoints(std::vector<bool>& ispoint, std::vector<long> const& table, int left, int right)
{
	if (right <= left+1)
		return;

	double slope = (table[right] - table[left]) / static_cast<double>(right-left);

	long middle = left + ((right - left) / 2);
	ATLASSERT(middle > left && middle < right);

	double middleY = table[left] + slope * (middle - left);
	if (!closeEnough(table[middle], middleY))
	{
		// The line does not pass through the middle point.
		ispoint[middle] = true;
	}

	markpoints(ispoint, table, left, middle);
	markpoints(ispoint, table, middle, right);

}


/**
* Function: SetFromTable
* Purpose:  Take an array of data points (eg: 1024, all 0 to 1023 inclusive),
*           and try to deduce a sequence of line segments that we can use to
*           approximate these points.
*/
STDMETHODIMP CPxLLookup::SetFromTable(LONG length, LONG* table)
{
	// Algorithm:
	// 1) Look for obvious peaks and valleys, and set vertices there.
	// 2) Between each pair of points, check whether the midpoint between them
	//    is colinear with the end points. If not, add a vertex at the midpoint.
	// Clean up phase:
	// 3) For each group of three vertices, remove the middle one if it is
	//    redundant - i.e. if the three are colinear.
	// 4) For each pair of vertices, merge them if they are too close together.

	std::vector<bool> ispoint(length, false);
	ispoint.front() = true;
	ispoint[length-1] = true;
	std::vector<long> data(length, 0);
	std::copy(table, table + length, data.begin());

	// 1) Find obvious sharp bends:
	int i;
	for (i = 1; i < length-1; ++i)
	{
		if ((table[i-1] < table[i] && table[i+1] < table[i]) // i is a peak
			|| table[i-1] > table[i] && table[i+1] > table[i]) // i is a vally
		{
			ispoint[i] = true;
		}
	}

	// Make a list of the indexes of all the corners found so far (there are
	// at least two - the end points):
	std::vector<int> ptIndex;
	for (i = 0; i < length; ++i)
	{
		if (ispoint[i])
			ptIndex.push_back(i);
	}

	// 2) Step 2 - see above:
	for (i = 1; i < ptIndex.size(); ++i)
	{
		markpoints(ispoint, data, ptIndex[i-1], ptIndex[i]);
	}

	// 3)
	// For each group of three points, if they are colinear, remove the middle
	// of the three. The markpoints algorithm finds all the sharp corners, but
	// it also marks extra, unneccesary points as corners too.
	ptIndex.clear();
	for (i = 0; i < length; ++i)
	{
		if (ispoint[i])
			ptIndex.push_back(i);
	}
	int left = 0;
	while (left < ptIndex.size() - 2)
	{
		// If the position of the middle point is more-or-less
		// on the line from the first to the third pionts, remove it.
		// Slope: (y2-y0)/(x2-x0)
		double slp = ((table[ptIndex[left+2]] - table[ptIndex[left]])
						/ static_cast<double>(ptIndex[left+2] - ptIndex[left]));
		if (abs(slp) > 1.0)
		{
			// Steep line segment - test based on x-position of middle point:
			if (abs((ptIndex[left+1]) - (ptIndex[left] + (table[ptIndex[left+1]] - table[ptIndex[left]]) / slp)) <= 2)
			{
				// Remove the middle point.
				ispoint[ptIndex[left+1]] = false;
				ptIndex.erase(ptIndex.begin() + left + 1);
			}
			else
			{
				++left;
			}
		}
		else
		{
			// Shallow line segment - test based on y position:
			if (abs((table[ptIndex[left+1]]) - (table[ptIndex[left]] + slp * (ptIndex[left+1] - ptIndex[left]))) <= 2)
			{
				// Remove the middle point.
				ispoint[ptIndex[left+1]] = false;
				ptIndex.erase(ptIndex.begin() + left + 1);
			}
			else
			{
				++left;
			}
		}
	}

	// Now each true-valued point in the ispoint vector corresponds to a corner
	// or a sharp bend in the line segment that we will use to render the graph
	// on-screen. Create vectors of POINTs and BYTEs in the format we need to
	// pass to the SetPoints method.
	double xRatio = (m_rectLimits.right - m_rectLimits.left) / static_cast<double>(length-1);
	double yRatio = (m_rectLimits.bottom - m_rectLimits.top) / static_cast<double>(length-1);

	std::vector<long> pts;
	std::vector<BYTE> types;
	long npts = 0;

	if (table[0] != 0)
	{
		// Add the left-hand end point - we need to do this ourselves, because of
		// the way the graph is drawn, with a point *always* at (0,0), even when
		// there is also a point at (0,y) for some non-zero y.
		pts.push_back(m_rectLimits.left);
		pts.push_back(m_rectLimits.bottom);
		types.push_back(PT_MOVETO);
		npts++;
	}

	for (i = 0; i < length; ++i)
	{
		if (ispoint[i])
		{
			pts.push_back(m_rectLimits.left + xRatio * i);
			pts.push_back(m_rectLimits.bottom - yRatio * table[i]);
			types.push_back(npts == 0 ? PT_MOVETO : PT_LINETO);
			npts++;
		}
	}

	if (table[length-1] != length-1)
	{
		// See comment above re: first point. Do same thing for last point.
		pts.push_back(m_rectLimits.right);
		pts.push_back(m_rectLimits.top);
		types.push_back(PT_LINETO);
		npts++;
	}

	// 4) Merge close-together vertices:
	POINT prevpt = {pts[0], pts[1]};
	POINT curpt;
	i = 2;
	while (i < pts.size())
	{
		curpt.x = pts[i];
		curpt.y = pts[i+1];
		if (Overlapping(curpt, prevpt))
		{
			// Remove the rightmost of the two (arbitrarily)
			pts.erase(pts.begin() + i);
			pts.erase(pts.begin() + i);
			types.pop_back(); // They are all PT_LINETO, so it doesn't matter which one we remove.
		}
		else
		{
			prevpt = curpt;
			i += 2;
		}
	}
	// We may have removed the last point because it was too close to the
	// second-last point. Force the last point to where it needs to be:
	pts[pts.size()-2] = m_rectLimits.right;
	pts[pts.size()-1] = m_rectLimits.top;

	return this->SetPoints(&pts[0], &types[0], types.size());
}


// TODO - this is a bit weird - basically we start with a menu containing
// all the possible options, then delete the ones that don't apply, based
// on the click location.
LRESULT CPxLLookup::OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (!m_bEnabled) return 0;

	POINT mouse;
	int x, y;
	mouse.x = x = GET_X_LPARAM(lParam);
	mouse.y = y = GET_Y_LPARAM(lParam);
	ScreenToClient(&mouse); // Leave x and y in screen cooords - needed for TrackContextMenu, below.

	// Figure out what the mouse is over - a point, a curve-control point, a line, a curve, or nothing.
	int idx = -1;
	int mouseOver = GetMouseOver(mouse.x, mouse.y, &idx);

	HMENU dummy = ::LoadMenu(GetModuleHandle(_T("PxLControls.dll")), MAKEINTRESOURCE(IDR_LOOKUP_POPUP_MENU));
	ATLASSERT(dummy != NULL);

	HMENU hPopup = GetSubMenu(dummy, 0);

	// TEMP - I haven't implemented "Insert Point", and I'm not sure I'm going to - it's redundant,
	//   and it could be a bit complicated  - what if the x-coord clicked is in the middle of a curve?
	DeleteMenu(hPopup, ID_INSERT_POINT, MF_BYCOMMAND);

	switch (mouseOver)
	{
	case ePoint:
		if (idx == 0 || idx == m_points.size() - 1)
			DeleteMenu(hPopup, ID_DELETE_POINT, MF_BYCOMMAND); // Can't remove first or last points.
		DeleteMenu(hPopup, ID_CONVERTTO_CURVE, MF_BYCOMMAND);
		DeleteMenu(hPopup, ID_CONVERTTO_LINE, MF_BYCOMMAND);
		// Leaves either Delete Point or nothing (for first and last points).
		break;
	case eControlPoint:
		DeleteMenu(hPopup, ID_DELETE_POINT, MF_BYCOMMAND);
		DeleteMenu(hPopup, ID_CONVERTTO_CURVE, MF_BYCOMMAND);
		// Leaves only Convert to Line.
		break;
	case eLine:
		DeleteMenu(hPopup, ID_DELETE_POINT, MF_BYCOMMAND);
		DeleteMenu(hPopup, ID_CONVERTTO_LINE, MF_BYCOMMAND);
		// Leaves only Convert to Curve.
		break;
	case eCurve:
		// Never executed - GetMouseOver does not (yet) test for hits on a curve, so never returns eCurve.
		DeleteMenu(hPopup, ID_DELETE_POINT, MF_BYCOMMAND);
		DeleteMenu(hPopup, ID_CONVERTTO_CURVE, MF_BYCOMMAND);
		break;
	case eNone:
		DeleteMenu(hPopup, ID_DELETE_POINT, MF_BYCOMMAND);
		DeleteMenu(hPopup, ID_CONVERTTO_LINE, MF_BYCOMMAND);
		DeleteMenu(hPopup, ID_CONVERTTO_CURVE, MF_BYCOMMAND);
		// Leaves nothing (for now - I *might* implement "Add Point" later)
		break;
	default:
		break;
	}

	if (GetMenuItemCount(hPopup) == 0)
	{
		DestroyMenu(dummy);
		return 0;
	}

	int menuId = (int)::TrackPopupMenu(
					hPopup,
					TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON, // | TPM_HORPOSANIMATION | TPM_VERPOSANIMATION,
					x, y, 0, this->m_hWnd, NULL);

	DestroyMenu(dummy);

	POINT pt; // May be needed (if action is Insert Point or Convert to Curve)

	switch (menuId)
	{
	case ID_INSERT_POINT:
		// Add a point at mouse.x, mouse.y
		// TODO - implement this?
		break;
	case ID_DELETE_POINT:
		// Delete the point that was clicked (point at index idx in m_points).
		RemovePoint(idx);
		FireViewChange();
		Fire_GraphChanged();
		break;
	case ID_CONVERTTO_CURVE:
		// Convert the line from point idx to idx+1 to a curve.
		// Add two control points between idx and idx+1
		pt.x = m_points[idx].x + (m_points[idx+1].x - m_points[idx].x) * 2 / 3;
		pt.y = m_points[idx].y + (m_points[idx+1].y - m_points[idx].y) * 2 / 3;
		AddPoint(idx + 1, pt, true);
		pt.x = m_points[idx].x + (m_points[idx+1].x - m_points[idx].x) / 2;
		pt.y = m_points[idx].y + (m_points[idx+1].y - m_points[idx].y) / 2;
		AddPoint(idx + 1, pt, true);
		FireViewChange();
		Fire_GraphChanged();
		break;
	case ID_CONVERTTO_LINE:
		// Convert the curve for which point idx is a control point to a line.
		RemovePoint(idx);
		RemovePoint(m_controlpoints[idx] ? idx : --idx);
		m_types[idx] = PT_LINETO;
		FireViewChange();
		Fire_GraphChanged();
		break;
	case ID_RESET_GRAPH:
		this->Reset();
		FireViewChange();
		Fire_GraphChanged();
		break;
	default:
		break;
	}

	return 0;
}


int CPxLLookup::GetMouseOver(int x, int y, int *idx)
{
	// Check if we are over a point.
	for (int i = 0; i < m_points.size(); i++)
	{
		if (abs(x - m_points[i].x) < 4 && abs(y - m_points[i].y) < 4)
		{
			*idx = i;
			return m_controlpoints[i] ? eControlPoint : ePoint;
		}
	}

	// Check if we are over a line.
	if (x < m_points[0].x || x > m_points.back().x)
		return eNone; // Clicked in the control, but outside the graph limits.

	int rPt = 0; // Right-hand of the two points that bound the line we are over.
	while (m_points[rPt].x < x)
		rPt++;
	if (rPt == 0) return eNone; // Playing it safe
	if (m_types[rPt] == PT_LINETO)
	{
		// The x coordinate of the mouse is on a linear section.
		// See if the y coordinate puts it close enough to the line to be considered a "hit".
		int lineY = static_cast<int>( // Y-coord of line at mouse's x-position
						static_cast<double>(x - m_points[rPt-1].x) 
						* (m_points[rPt].y - m_points[rPt-1].y) 
						/ (m_points[rPt].x - m_points[rPt-1].x) 
						+ 0.5)
					+ m_points[rPt-1].y;
		int lineX = static_cast<int>( // X-coord of line at mouse's y-position
						static_cast<double>(y - m_points[rPt-1].y) 
						* (m_points[rPt].x - m_points[rPt-1].x) 
						/ (m_points[rPt].y - m_points[rPt-1].y) 
						+ 0.5)
					+ m_points[rPt-1].x;
		if (abs(lineY - y) < 4 || abs(lineX - x) < 4)
		{
			*idx = rPt - 1;
			return eLine;
		}
	}

	// Check whether we are over a curve.
	// TODO - figure out how to do this!

	return eNone;
}

LRESULT CPxLLookup::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bResized = true;
	FireViewChange();
	Fire_GraphChanged();
	return 0;
}


STDMETHODIMP CPxLLookup::GetPointCount(LONG* count)
{
	*count = m_points.size();
	return S_OK;
}

STDMETHODIMP CPxLLookup::GetPoints(LONG* points, BYTE* lineTypes, LONG nPoints)
{
	if (nPoints < m_points.size())
		return E_INVALIDARG;

	for (int i = 0; i < nPoints; ++i)
	{
		points[2*i] = m_points[i].x;
		points[2*i + 1] = m_points[i].y;
		lineTypes[i] = m_types[i];
	}

	return S_OK;
}

STDMETHODIMP CPxLLookup::SetPoints(LONG* points, BYTE* lineTypes, LONG nPoints)
{
	if (nPoints < 2 || points == NULL || lineTypes == NULL)
		return E_FAIL;

	m_points.clear();
	m_types.clear();
	m_controlpoints.clear();

	int cpCount = 0; // count control points to make sure they come in groups
	POINT pt;
	bool err = false;
	for ( int i = 0; i < nPoints; ++i )
	{
		m_types.push_back(lineTypes[i]);
		pt.x = points[2*i];
		pt.y = points[2*i + 1];
		m_points.push_back(pt);

		// Make sure curves are handled correctly.
		if (m_types.back() == PT_BEZIERTO)
		{
			cpCount++;
			m_controlpoints.push_back(cpCount == 1 || cpCount == 2);
			if (cpCount == 3)
				cpCount = 0;
		}
		else if (cpCount == 1 || cpCount == 2)
		{
			// ERROR - PT_BEZIERTO *must* appear in 3's
			err = true;
			break;
		}
		else
		{
			m_controlpoints.push_back(false);
		}

	}

	if (err)
	{
		this->Reset();
		return E_INVALIDARG;
	}

	FireViewChange();
	return S_OK;
}

LRESULT CPxLLookup::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;
}




























