// Rct.h
#include <gdiplus.h>

// This is called Rct because the name Rect is already taken (by GDI+)
template<typename T>
class Rct
{
public:
	typedef T coord_type;

	T left;
	T top;
	T right;
	T bottom;
	int lineWidth;
	COLORREF lineColor;
	bool filled;
	COLORREF fillColor;

	Rct() : left(0), top(0), right(0), bottom(0), lineWidth(1), lineColor(RGB(0,0,0)), filled(false), fillColor(RGB(255,255,255)) {}
	Rct(T _left, T _top, T _right, T _bottom, int _lineWidth=1, COLORREF _lineColor=RGB(0,0,0), bool _filled=false, COLORREF _fillColor=RGB(255,255,255))
		: left(_left), top(_top), right(_right), bottom(_bottom),
		  lineWidth(_lineWidth), lineColor(_lineColor), filled(_filled), fillColor(_fillColor)
		{}

	Rct(Rct const& r)
		: left(r.left), top(r.top), right(r.right), bottom(r.bottom),
		  lineWidth(r.lineWidth), lineColor(r.lineColor), filled(r.filled), fillColor(r.fillColor)
		{}

	Rct(RECT const& r, int lineWidth_=1, COLORREF lineColor_=RGB(0,0,0), bool filled_=false, COLORREF fillColor_=RGB(255,255,255))
		: left(r.left), top(r.top), right(r.right), bottom(r.bottom),
		  lineWidth(lineWidth_), lineColor(lineColor_), filled(filled_), fillColor(fillColor_)
		{}

	T Width() const { return right - left; }
	T Height() const { return bottom - top; }
	T InnerWidth() const { return Width() - 2 * lineWidth; }
	T InnerHeight() const { return Height() - 2 * lineWidth; }

	T InnerLeft() const { return left + lineWidth; }
	T InnerTop() const { return top + lineWidth; }
	T InnerRight() const { return right - lineWidth; }
	T InnerBottom() const { return bottom - lineWidth; }

	template<typename U>
	void SetPos(Rct<U> const& r) { left=static_cast<T>(r.left); top=static_cast<T>(r.top); right=static_cast<T>(r.right); bottom=static_cast<T>(r.bottom); }
	void SetPos(RECT const& r) { left=r.left; top=r.top; right=r.right; bottom=r.bottom; }
	void SetPos(T _left, T _top, T _width, T _height) { left=_left; top=_top; right=left+_width; bottom=top+_height; }

	template<typename U>
	void ShiftTo(U _left, U _top) { T w=Width(); T h=Height(); left=_left; top=_top; right=(left+w); bottom=(top+h); }

	Rct Inner() const { return Rct(InnerLeft(), InnerTop(), InnerRight(), InnerBottom()); }

	bool operator==(Rct const& other) const { return left==other.left && top==other.top && right==other.right && bottom==other.bottom && lineWidth==other.lineWidth && lineColor==other.lineColor && filled==other.filled && fillColor==other.fillColor; }
	bool operator!=(Rct const& other) const { return !(*this==other); }
	bool PosEquals(Rct const& other) const { return left==other.left && top==other.top && right==other.right && bottom==other.bottom; }
	void Draw(HDC hdc) const;
};

template<typename T>
void
Rct<T>::Draw(HDC hdc) const
{
	HPEN hpen = CreatePen(PS_SOLID | PS_INSIDEFRAME, lineWidth, lineColor);
	HBRUSH hbrush = filled ? CreateSolidBrush(fillColor) : (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	HGDIOBJ hPenOld = SelectObject(hdc, hpen);
	HGDIOBJ hBrushOld = SelectObject(hdc, hbrush);

	::Rectangle(hdc,
				roundTo<int>(left),
				roundTo<int>(top),
				roundTo<int>(right),
				roundTo<int>(bottom));

	SelectObject(hdc, hBrushOld);
	SelectObject(hdc, hPenOld);

	DeleteObject(hpen);
	if (filled)
		DeleteObject(hbrush);
}

