// PxLLookup.h : Declaration of the CPxLLookup
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "PxLControls.h"
#include "_IPxLLookupEvents_CP.h"
#include "Common.h"
#include <vector>
#include <list>

// There must be a built-in to do this???
COLORREF OleToRGB(OLE_COLOR clr)
{
	COLORREF c;
	OleTranslateColor(clr, NULL, &c);
	return c;
}

// CPxLLookup
class ATL_NO_VTABLE CPxLLookup : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CPxLLookup, IPxLLookup>,
	public IPersistStreamInitImpl<CPxLLookup>,
	public IOleControlImpl<CPxLLookup>,
	public IOleObjectImpl<CPxLLookup>,
	public IOleInPlaceActiveObjectImpl<CPxLLookup>,
	public IViewObjectExImpl<CPxLLookup>,
	public IOleInPlaceObjectWindowlessImpl<CPxLLookup>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CPxLLookup>,
	public CProxy_IPxLLookupEvents<CPxLLookup>, 
	public IPersistStorageImpl<CPxLLookup>,
	public ISpecifyPropertyPagesImpl<CPxLLookup>,
	public IQuickActivateImpl<CPxLLookup>,
	public IDataObjectImpl<CPxLLookup>,
	public IProvideClassInfo2Impl<&CLSID_PxLLookup, &__uuidof(_IPxLLookupEvents), &LIBID_PxLControlsLib>,
	public CComCoClass<CPxLLookup, &CLSID_PxLLookup>,
	public CComControl<CPxLLookup>
{
public:

enum eObjType
{
	eNone,
	ePoint,
	eControlPoint,
	eLine,
	eCurve
};

	CPxLLookup()
		: m_activePoint(-1)
		, m_bDragging(false)
		, m_MouseOffsetLeft(0)
		, m_MouseOffsetTop(0)
		, m_bNewPoint(false)
		, m_bResized(false)
		, m_pointSize(3)
	{
		// Set some defaults. The ActiveX design environment (eg: VB, VC++, etc.) will 
		// reset many (all?) of these values after this point.
		m_bWindowOnly = true;
		m_nBorderWidth = 1;
		m_nDrawWidth = 2;
		m_clrForeColor = RGB(0xC0,0,0);
		m_clrBorderColor = RGB(0,0,0);

		// We need to fake a size for m_rectLimits.
		// There are times when the vertices get set (via SetFromTable) before
		// the control is drawn for the first time - in that case, the actual
		// pixel-coordinates are set based on this fake size, and then rescaled
		// in the first call to OnDraw. So we need to choose a reasonable size here.
		m_rectLimits.left = 0;
		m_rectLimits.top = 0;
		m_rectLimits.right = 400;
		m_rectLimits.bottom = 400;
	}

// blah
DECLARE_OLEMISC_STATUS(
	OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_PXLLOOKUP)

BEGIN_COM_MAP(CPxLLookup)
	COM_INTERFACE_ENTRY(IPxLLookup)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_PROP_MAP(CPxLLookup)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	PROP_ENTRY_TYPE("BorderColor", DISPID_BORDERCOLOR, CLSID_StockColorPage, VT_EMPTY)
	PROP_ENTRY_TYPE("BorderWidth", DISPID_BORDERWIDTH, CLSID_NULL, VT_EMPTY)
	PROP_ENTRY_TYPE("DrawWidth", DISPID_DRAWWIDTH, CLSID_NULL, VT_EMPTY)
	PROP_ENTRY_TYPE("Enabled", DISPID_ENABLED, CLSID_NULL, VT_EMPTY)
	PROP_ENTRY_TYPE("ForeColor", DISPID_FORECOLOR, CLSID_StockColorPage, VT_EMPTY)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CPxLLookup)
	CONNECTION_POINT_ENTRY(__uuidof(_IPxLLookupEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CPxLLookup)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	CHAIN_MSG_MAP(CComControl<CPxLLookup>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] = 
		{
			&IID_IPxLLookup,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IPxLLookup
public:
	HRESULT OnDraw(ATL_DRAWINFO& di);

#define DISABLED_GRAPH_COLOR RGB(0x80,0x80,0x80)
#define DISABLED_BACK_COLOR RGB(0xF0,0xF0,0xF0)
#define BACKGROUND_PATTERN_COLOR RGB(0xE0,0xE0,0xE0)

	OLE_COLOR m_clrBorderColor;
	void OnBorderColorChanged()
	{
		ATLTRACE(_T("OnBorderColorChanged\n"));
		DeleteObject(m_hBorderPen);
		m_hBorderPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, m_nBorderWidth, OleToRGB(m_clrBorderColor));
	}
	LONG m_nBorderWidth;
	void OnBorderWidthChanged()
	{
		ATLTRACE(_T("OnBorderWidthChanged\n"));
		DeleteObject(m_hBorderPen);
		m_hBorderPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, m_nBorderWidth, OleToRGB(m_clrBorderColor));
		m_bResized = true;
	}
	LONG m_nDrawWidth;
	void OnDrawWidthChanged()
	{
		ATLTRACE(_T("OnDrawWidthChanged\n"));
		DeleteObject(m_hGraphPen);
		m_hGraphPen = ::CreatePen(PS_SOLID, m_nDrawWidth, OleToRGB(m_clrForeColor));
		DeleteObject(m_hDisabledGraphPen);
		m_hDisabledGraphPen = ::CreatePen(PS_SOLID, m_nDrawWidth, DISABLED_GRAPH_COLOR);
	}
	BOOL m_bEnabled;
	void OnEnabledChanged()
	{
		ATLTRACE(_T("OnEnabledChanged\n"));
	}
	OLE_COLOR m_clrForeColor;
	void OnForeColorChanged()
	{
		ATLTRACE(_T("OnForeColorChanged\n"));
		DeleteObject(m_hGraphPen);
		m_hGraphPen = ::CreatePen(PS_SOLID, m_nDrawWidth, OleToRGB(m_clrForeColor));
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_hBorderPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, m_nBorderWidth, RGB(0,0,0));
		m_hInsideBorderPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, m_pointSize, BACKGROUND_PATTERN_COLOR);
		m_hGraphPen = ::CreatePen(PS_SOLID, m_nDrawWidth, OleToRGB(m_clrForeColor));

		m_hDisabledGraphPen = ::CreatePen(PS_SOLID, m_nDrawWidth, DISABLED_GRAPH_COLOR);
		m_hDisabledBGBrush = ::CreateSolidBrush(DISABLED_BACK_COLOR);

		m_hBGBrush = ::CreateHatchBrush(HS_CROSS, BACKGROUND_PATTERN_COLOR);
		m_hPointPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, 1, RGB(0,0,128));
		m_hActivePointPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, 2, RGB(0,0,255));

		Reset(); // Add initial two points

		return S_OK;
	}
	
	void FinalRelease() 
	{
		DeleteObject(m_hBorderPen);
		DeleteObject(m_hBGBrush);
		DeleteObject(m_hGraphPen);
		DeleteObject(m_hPointPen);
		DeleteObject(m_hActivePointPen);
		DeleteObject(m_hDisabledBGBrush);
		DeleteObject(m_hDisabledGraphPen);
	}

	std::vector<POINT> m_points;
	std::vector<BYTE> m_types;
private:
	void AddLine(long xStart, long yStart, long xEnd, long yEnd);
public:
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	int m_activePoint;
	bool m_bDragging;
	int GetPointAt(int x, int y, int slop);
public:
//	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	void EndDrag(void);
public:
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	int m_MouseOffsetLeft;
	int m_MouseOffsetTop;
	RECT m_rectLimits;
	bool m_bNewPoint;
public:
//	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	int GetMouseOver(int x, int y, int *idx);
	std::vector<bool> m_controlpoints;
	void RemovePoint(int idx);
	void AddPoint(int idx, POINT pt, bool isControl = false);
	void Rescale(void);
public:
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	bool m_bResized;
	int m_pointSize;

	HPEN m_hBorderPen;
	HPEN m_hInsideBorderPen;
	HBRUSH m_hBGBrush;
	HBRUSH m_hDisabledBGBrush;
	HPEN m_hGraphPen;
	HPEN m_hDisabledGraphPen;
	HPEN m_hPointPen;
	HPEN m_hActivePointPen;

#ifdef _DEBUG
	bool Dump(void);
#endif

public:
	STDMETHOD(FillTable)(LONG length, LONG* table);
	STDMETHOD(SetFromTable)(LONG length, LONG* table);
	STDMETHOD(GetPointCount)(LONG* count);
	STDMETHOD(GetPoints)(LONG* points, BYTE* lineTypes, LONG nPoints);
	STDMETHOD(SetPoints)(LONG* points, BYTE* lineTypes, LONG nPoints);
	STDMETHOD(Reset)(void);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
};

OBJECT_ENTRY_AUTO(__uuidof(PxLLookup), CPxLLookup)
