// PxLHistogram2.h : Declaration of the CPxLHistogram2
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "PxLControls.h"

// CPxLHistogram2
class ATL_NO_VTABLE CPxLHistogram2 : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CPxLHistogram2, IPxLHistogram2>,
	public IPersistStreamInitImpl<CPxLHistogram2>,
	public IOleControlImpl<CPxLHistogram2>,
	public IOleObjectImpl<CPxLHistogram2>,
	public IOleInPlaceActiveObjectImpl<CPxLHistogram2>,
	public IViewObjectExImpl<CPxLHistogram2>,
	public IOleInPlaceObjectWindowlessImpl<CPxLHistogram2>,
	public CComCoClass<CPxLHistogram2, &CLSID_PxLHistogram2>,
	public CComControl<CPxLHistogram2>
{
public:

	CPxLHistogram2();

	DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
		OLEMISC_CANTLINKINSIDE | 
		OLEMISC_INSIDEOUT | 
		OLEMISC_ACTIVATEWHENVISIBLE | 
		OLEMISC_SETCLIENTSITEFIRST
	)

	DECLARE_REGISTRY_RESOURCEID(IDR_PXLHISTOGRAM2)

	BEGIN_COM_MAP(CPxLHistogram2)
		COM_INTERFACE_ENTRY(IPxLHistogram2)
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
	END_COM_MAP()

	BEGIN_PROP_MAP(CPxLHistogram2)
		PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		PROP_ENTRY("BackColor", DISPID_BACKCOLOR, CLSID_StockColorPage)
		// Example entries
		// PROP_ENTRY("Property Description", dispid, clsid)
		// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()


	BEGIN_MSG_MAP(CPxLHistogram2)
		CHAIN_MSG_MAP(CComControl<CPxLHistogram2>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	// IPxLHistogram2
public:
	HRESULT OnDrawAdvanced(ATL_DRAWINFO& di);

	OLE_COLOR m_clrBackColor;
	void OnBackColorChanged();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

};

OBJECT_ENTRY_AUTO(__uuidof(PxLHistogram2), CPxLHistogram2)



