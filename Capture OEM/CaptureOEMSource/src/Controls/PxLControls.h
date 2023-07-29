

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Mon Apr 26 15:07:47 2021
 */
/* Compiler settings for PxLControls.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __PxLControls_h__
#define __PxLControls_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPxLSubwindow_FWD_DEFINED__
#define __IPxLSubwindow_FWD_DEFINED__
typedef interface IPxLSubwindow IPxLSubwindow;

#endif 	/* __IPxLSubwindow_FWD_DEFINED__ */


#ifndef __IPxLLookup_FWD_DEFINED__
#define __IPxLLookup_FWD_DEFINED__
typedef interface IPxLLookup IPxLLookup;

#endif 	/* __IPxLLookup_FWD_DEFINED__ */


#ifndef __IPxLHistogram_FWD_DEFINED__
#define __IPxLHistogram_FWD_DEFINED__
typedef interface IPxLHistogram IPxLHistogram;

#endif 	/* __IPxLHistogram_FWD_DEFINED__ */


#ifndef __IPxLGraph_FWD_DEFINED__
#define __IPxLGraph_FWD_DEFINED__
typedef interface IPxLGraph IPxLGraph;

#endif 	/* __IPxLGraph_FWD_DEFINED__ */


#ifndef ___IPxLSubwindowEvents_FWD_DEFINED__
#define ___IPxLSubwindowEvents_FWD_DEFINED__
typedef interface _IPxLSubwindowEvents _IPxLSubwindowEvents;

#endif 	/* ___IPxLSubwindowEvents_FWD_DEFINED__ */


#ifndef __PxLSubwindow_FWD_DEFINED__
#define __PxLSubwindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class PxLSubwindow PxLSubwindow;
#else
typedef struct PxLSubwindow PxLSubwindow;
#endif /* __cplusplus */

#endif 	/* __PxLSubwindow_FWD_DEFINED__ */


#ifndef ___IPxLLookupEvents_FWD_DEFINED__
#define ___IPxLLookupEvents_FWD_DEFINED__
typedef interface _IPxLLookupEvents _IPxLLookupEvents;

#endif 	/* ___IPxLLookupEvents_FWD_DEFINED__ */


#ifndef __PxLLookup_FWD_DEFINED__
#define __PxLLookup_FWD_DEFINED__

#ifdef __cplusplus
typedef class PxLLookup PxLLookup;
#else
typedef struct PxLLookup PxLLookup;
#endif /* __cplusplus */

#endif 	/* __PxLLookup_FWD_DEFINED__ */


#ifndef __PxLHistogram_FWD_DEFINED__
#define __PxLHistogram_FWD_DEFINED__

#ifdef __cplusplus
typedef class PxLHistogram PxLHistogram;
#else
typedef struct PxLHistogram PxLHistogram;
#endif /* __cplusplus */

#endif 	/* __PxLHistogram_FWD_DEFINED__ */


#ifndef __PxLGraph_FWD_DEFINED__
#define __PxLGraph_FWD_DEFINED__

#ifdef __cplusplus
typedef class PxLGraph PxLGraph;
#else
typedef struct PxLGraph PxLGraph;
#endif /* __cplusplus */

#endif 	/* __PxLGraph_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IPxLSubwindow_INTERFACE_DEFINED__
#define __IPxLSubwindow_INTERFACE_DEFINED__

/* interface IPxLSubwindow */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IPxLSubwindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EBBDFD15-4DAC-445D-9BA0-001A9F11183E")
    IPxLSubwindow : public IDispatch
    {
    public:
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BackColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BackColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_FillColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_FillColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_ForeColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_ForeColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL vbool) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL *pbool) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBounds( 
            /* [in] */ LONG width,
            /* [in] */ LONG height) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSubwindow( 
            /* [in] */ LONG left,
            /* [in] */ LONG top,
            /* [in] */ LONG width,
            /* [in] */ LONG height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBounds( 
            /* [out] */ LONG *width,
            /* [out] */ LONG *height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubwindow( 
            /* [out] */ LONG *left,
            /* [out] */ LONG *top,
            /* [out] */ LONG *width,
            /* [out] */ LONG *height) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSize( 
            /* [in] */ LONG width,
            /* [in] */ LONG height) = 0;
        
        virtual /* [source][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MinWidth( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [source][helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MinWidth( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [source][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MinHeight( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [source][helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MinHeight( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLeft( 
            /* [in] */ LONG left) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTop( 
            /* [in] */ LONG top) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBottom( 
            /* [in] */ LONG bottom) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRight( 
            /* [in] */ LONG right) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWidth( 
            /* [in] */ LONG width) = 0;
        
        virtual /* [source][helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHeight( 
            /* [in] */ LONG height) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AmbientColor( 
            /* [retval][out] */ OLE_COLOR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AmbientColor( 
            /* [in] */ OLE_COLOR newVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPxLSubwindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPxLSubwindow * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPxLSubwindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPxLSubwindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPxLSubwindow * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPxLSubwindow * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPxLSubwindow * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPxLSubwindow * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BackColor )( 
            IPxLSubwindow * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BackColor )( 
            IPxLSubwindow * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderColor )( 
            IPxLSubwindow * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderColor )( 
            IPxLSubwindow * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FillColor )( 
            IPxLSubwindow * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FillColor )( 
            IPxLSubwindow * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ForeColor )( 
            IPxLSubwindow * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ForeColor )( 
            IPxLSubwindow * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            IPxLSubwindow * This,
            /* [in] */ VARIANT_BOOL vbool);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            IPxLSubwindow * This,
            /* [retval][out] */ VARIANT_BOOL *pbool);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetBounds )( 
            IPxLSubwindow * This,
            /* [in] */ LONG width,
            /* [in] */ LONG height);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSubwindow )( 
            IPxLSubwindow * This,
            /* [in] */ LONG left,
            /* [in] */ LONG top,
            /* [in] */ LONG width,
            /* [in] */ LONG height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBounds )( 
            IPxLSubwindow * This,
            /* [out] */ LONG *width,
            /* [out] */ LONG *height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSubwindow )( 
            IPxLSubwindow * This,
            /* [out] */ LONG *left,
            /* [out] */ LONG *top,
            /* [out] */ LONG *width,
            /* [out] */ LONG *height);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSize )( 
            IPxLSubwindow * This,
            /* [in] */ LONG width,
            /* [in] */ LONG height);
        
        /* [source][helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MinWidth )( 
            IPxLSubwindow * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [source][helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MinWidth )( 
            IPxLSubwindow * This,
            /* [in] */ LONG newVal);
        
        /* [source][helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MinHeight )( 
            IPxLSubwindow * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [source][helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MinHeight )( 
            IPxLSubwindow * This,
            /* [in] */ LONG newVal);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLeft )( 
            IPxLSubwindow * This,
            /* [in] */ LONG left);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTop )( 
            IPxLSubwindow * This,
            /* [in] */ LONG top);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetBottom )( 
            IPxLSubwindow * This,
            /* [in] */ LONG bottom);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRight )( 
            IPxLSubwindow * This,
            /* [in] */ LONG right);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWidth )( 
            IPxLSubwindow * This,
            /* [in] */ LONG width);
        
        /* [source][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHeight )( 
            IPxLSubwindow * This,
            /* [in] */ LONG height);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AmbientColor )( 
            IPxLSubwindow * This,
            /* [retval][out] */ OLE_COLOR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AmbientColor )( 
            IPxLSubwindow * This,
            /* [in] */ OLE_COLOR newVal);
        
        END_INTERFACE
    } IPxLSubwindowVtbl;

    interface IPxLSubwindow
    {
        CONST_VTBL struct IPxLSubwindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPxLSubwindow_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPxLSubwindow_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPxLSubwindow_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPxLSubwindow_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPxLSubwindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPxLSubwindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPxLSubwindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IPxLSubwindow_put_BackColor(This,clr)	\
    ( (This)->lpVtbl -> put_BackColor(This,clr) ) 

#define IPxLSubwindow_get_BackColor(This,pclr)	\
    ( (This)->lpVtbl -> get_BackColor(This,pclr) ) 

#define IPxLSubwindow_put_BorderColor(This,clr)	\
    ( (This)->lpVtbl -> put_BorderColor(This,clr) ) 

#define IPxLSubwindow_get_BorderColor(This,pclr)	\
    ( (This)->lpVtbl -> get_BorderColor(This,pclr) ) 

#define IPxLSubwindow_put_FillColor(This,clr)	\
    ( (This)->lpVtbl -> put_FillColor(This,clr) ) 

#define IPxLSubwindow_get_FillColor(This,pclr)	\
    ( (This)->lpVtbl -> get_FillColor(This,pclr) ) 

#define IPxLSubwindow_put_ForeColor(This,clr)	\
    ( (This)->lpVtbl -> put_ForeColor(This,clr) ) 

#define IPxLSubwindow_get_ForeColor(This,pclr)	\
    ( (This)->lpVtbl -> get_ForeColor(This,pclr) ) 

#define IPxLSubwindow_put_Enabled(This,vbool)	\
    ( (This)->lpVtbl -> put_Enabled(This,vbool) ) 

#define IPxLSubwindow_get_Enabled(This,pbool)	\
    ( (This)->lpVtbl -> get_Enabled(This,pbool) ) 

#define IPxLSubwindow_SetBounds(This,width,height)	\
    ( (This)->lpVtbl -> SetBounds(This,width,height) ) 

#define IPxLSubwindow_SetSubwindow(This,left,top,width,height)	\
    ( (This)->lpVtbl -> SetSubwindow(This,left,top,width,height) ) 

#define IPxLSubwindow_GetBounds(This,width,height)	\
    ( (This)->lpVtbl -> GetBounds(This,width,height) ) 

#define IPxLSubwindow_GetSubwindow(This,left,top,width,height)	\
    ( (This)->lpVtbl -> GetSubwindow(This,left,top,width,height) ) 

#define IPxLSubwindow_SetSize(This,width,height)	\
    ( (This)->lpVtbl -> SetSize(This,width,height) ) 

#define IPxLSubwindow_get_MinWidth(This,pVal)	\
    ( (This)->lpVtbl -> get_MinWidth(This,pVal) ) 

#define IPxLSubwindow_put_MinWidth(This,newVal)	\
    ( (This)->lpVtbl -> put_MinWidth(This,newVal) ) 

#define IPxLSubwindow_get_MinHeight(This,pVal)	\
    ( (This)->lpVtbl -> get_MinHeight(This,pVal) ) 

#define IPxLSubwindow_put_MinHeight(This,newVal)	\
    ( (This)->lpVtbl -> put_MinHeight(This,newVal) ) 

#define IPxLSubwindow_SetLeft(This,left)	\
    ( (This)->lpVtbl -> SetLeft(This,left) ) 

#define IPxLSubwindow_SetTop(This,top)	\
    ( (This)->lpVtbl -> SetTop(This,top) ) 

#define IPxLSubwindow_SetBottom(This,bottom)	\
    ( (This)->lpVtbl -> SetBottom(This,bottom) ) 

#define IPxLSubwindow_SetRight(This,right)	\
    ( (This)->lpVtbl -> SetRight(This,right) ) 

#define IPxLSubwindow_SetWidth(This,width)	\
    ( (This)->lpVtbl -> SetWidth(This,width) ) 

#define IPxLSubwindow_SetHeight(This,height)	\
    ( (This)->lpVtbl -> SetHeight(This,height) ) 

#define IPxLSubwindow_get_AmbientColor(This,pVal)	\
    ( (This)->lpVtbl -> get_AmbientColor(This,pVal) ) 

#define IPxLSubwindow_put_AmbientColor(This,newVal)	\
    ( (This)->lpVtbl -> put_AmbientColor(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPxLSubwindow_INTERFACE_DEFINED__ */


#ifndef __IPxLLookup_INTERFACE_DEFINED__
#define __IPxLLookup_INTERFACE_DEFINED__

/* interface IPxLLookup */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IPxLLookup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("24D06C4B-195F-43DB-B4C8-7F2E327F0517")
    IPxLLookup : public IDispatch
    {
    public:
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderWidth( 
            /* [in] */ long width) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderWidth( 
            /* [retval][out] */ long *width) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_DrawWidth( 
            /* [in] */ long width) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_DrawWidth( 
            /* [retval][out] */ long *pwidth) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_ForeColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_ForeColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL vbool) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL *pbool) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FillTable( 
            /* [in] */ LONG length,
            /* [in] */ LONG *table) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPointCount( 
            /* [retval][out] */ LONG *count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPoints( 
            /* [out] */ LONG *points,
            /* [out] */ BYTE *lineTypes,
            /* [in] */ LONG nPoints) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPoints( 
            /* [in] */ LONG *points,
            /* [in] */ BYTE *lineTypes,
            /* [in] */ LONG nPoints) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFromTable( 
            /* [in] */ LONG length,
            /* [in] */ LONG *table) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPxLLookupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPxLLookup * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPxLLookup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPxLLookup * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPxLLookup * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPxLLookup * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPxLLookup * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPxLLookup * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderColor )( 
            IPxLLookup * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderColor )( 
            IPxLLookup * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderWidth )( 
            IPxLLookup * This,
            /* [in] */ long width);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderWidth )( 
            IPxLLookup * This,
            /* [retval][out] */ long *width);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DrawWidth )( 
            IPxLLookup * This,
            /* [in] */ long width);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DrawWidth )( 
            IPxLLookup * This,
            /* [retval][out] */ long *pwidth);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ForeColor )( 
            IPxLLookup * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ForeColor )( 
            IPxLLookup * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            IPxLLookup * This,
            /* [in] */ VARIANT_BOOL vbool);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            IPxLLookup * This,
            /* [retval][out] */ VARIANT_BOOL *pbool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FillTable )( 
            IPxLLookup * This,
            /* [in] */ LONG length,
            /* [in] */ LONG *table);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPointCount )( 
            IPxLLookup * This,
            /* [retval][out] */ LONG *count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPoints )( 
            IPxLLookup * This,
            /* [out] */ LONG *points,
            /* [out] */ BYTE *lineTypes,
            /* [in] */ LONG nPoints);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPoints )( 
            IPxLLookup * This,
            /* [in] */ LONG *points,
            /* [in] */ BYTE *lineTypes,
            /* [in] */ LONG nPoints);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IPxLLookup * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetFromTable )( 
            IPxLLookup * This,
            /* [in] */ LONG length,
            /* [in] */ LONG *table);
        
        END_INTERFACE
    } IPxLLookupVtbl;

    interface IPxLLookup
    {
        CONST_VTBL struct IPxLLookupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPxLLookup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPxLLookup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPxLLookup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPxLLookup_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPxLLookup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPxLLookup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPxLLookup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IPxLLookup_put_BorderColor(This,clr)	\
    ( (This)->lpVtbl -> put_BorderColor(This,clr) ) 

#define IPxLLookup_get_BorderColor(This,pclr)	\
    ( (This)->lpVtbl -> get_BorderColor(This,pclr) ) 

#define IPxLLookup_put_BorderWidth(This,width)	\
    ( (This)->lpVtbl -> put_BorderWidth(This,width) ) 

#define IPxLLookup_get_BorderWidth(This,width)	\
    ( (This)->lpVtbl -> get_BorderWidth(This,width) ) 

#define IPxLLookup_put_DrawWidth(This,width)	\
    ( (This)->lpVtbl -> put_DrawWidth(This,width) ) 

#define IPxLLookup_get_DrawWidth(This,pwidth)	\
    ( (This)->lpVtbl -> get_DrawWidth(This,pwidth) ) 

#define IPxLLookup_put_ForeColor(This,clr)	\
    ( (This)->lpVtbl -> put_ForeColor(This,clr) ) 

#define IPxLLookup_get_ForeColor(This,pclr)	\
    ( (This)->lpVtbl -> get_ForeColor(This,pclr) ) 

#define IPxLLookup_put_Enabled(This,vbool)	\
    ( (This)->lpVtbl -> put_Enabled(This,vbool) ) 

#define IPxLLookup_get_Enabled(This,pbool)	\
    ( (This)->lpVtbl -> get_Enabled(This,pbool) ) 

#define IPxLLookup_FillTable(This,length,table)	\
    ( (This)->lpVtbl -> FillTable(This,length,table) ) 

#define IPxLLookup_GetPointCount(This,count)	\
    ( (This)->lpVtbl -> GetPointCount(This,count) ) 

#define IPxLLookup_GetPoints(This,points,lineTypes,nPoints)	\
    ( (This)->lpVtbl -> GetPoints(This,points,lineTypes,nPoints) ) 

#define IPxLLookup_SetPoints(This,points,lineTypes,nPoints)	\
    ( (This)->lpVtbl -> SetPoints(This,points,lineTypes,nPoints) ) 

#define IPxLLookup_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IPxLLookup_SetFromTable(This,length,table)	\
    ( (This)->lpVtbl -> SetFromTable(This,length,table) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPxLLookup_INTERFACE_DEFINED__ */


#ifndef __IPxLHistogram_INTERFACE_DEFINED__
#define __IPxLHistogram_INTERFACE_DEFINED__

/* interface IPxLHistogram */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IPxLHistogram;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FB4DE4CF-7F6B-440F-AFAD-C8FB4EDCE613")
    IPxLHistogram : public IDispatch
    {
    public:
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BackColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BackColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderWidth( 
            /* [in] */ long width) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderWidth( 
            /* [retval][out] */ long *width) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_ForeColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_ForeColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL vbool) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL *pbool) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHistogramData( 
            /* [in] */ LONG nDataPoints,
            /* [in] */ LONG *pData) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPxLHistogramVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPxLHistogram * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPxLHistogram * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPxLHistogram * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPxLHistogram * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPxLHistogram * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPxLHistogram * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPxLHistogram * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BackColor )( 
            IPxLHistogram * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BackColor )( 
            IPxLHistogram * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderColor )( 
            IPxLHistogram * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderColor )( 
            IPxLHistogram * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderWidth )( 
            IPxLHistogram * This,
            /* [in] */ long width);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderWidth )( 
            IPxLHistogram * This,
            /* [retval][out] */ long *width);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ForeColor )( 
            IPxLHistogram * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ForeColor )( 
            IPxLHistogram * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            IPxLHistogram * This,
            /* [in] */ VARIANT_BOOL vbool);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            IPxLHistogram * This,
            /* [retval][out] */ VARIANT_BOOL *pbool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHistogramData )( 
            IPxLHistogram * This,
            /* [in] */ LONG nDataPoints,
            /* [in] */ LONG *pData);
        
        END_INTERFACE
    } IPxLHistogramVtbl;

    interface IPxLHistogram
    {
        CONST_VTBL struct IPxLHistogramVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPxLHistogram_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPxLHistogram_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPxLHistogram_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPxLHistogram_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPxLHistogram_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPxLHistogram_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPxLHistogram_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IPxLHistogram_put_BackColor(This,clr)	\
    ( (This)->lpVtbl -> put_BackColor(This,clr) ) 

#define IPxLHistogram_get_BackColor(This,pclr)	\
    ( (This)->lpVtbl -> get_BackColor(This,pclr) ) 

#define IPxLHistogram_put_BorderColor(This,clr)	\
    ( (This)->lpVtbl -> put_BorderColor(This,clr) ) 

#define IPxLHistogram_get_BorderColor(This,pclr)	\
    ( (This)->lpVtbl -> get_BorderColor(This,pclr) ) 

#define IPxLHistogram_put_BorderWidth(This,width)	\
    ( (This)->lpVtbl -> put_BorderWidth(This,width) ) 

#define IPxLHistogram_get_BorderWidth(This,width)	\
    ( (This)->lpVtbl -> get_BorderWidth(This,width) ) 

#define IPxLHistogram_put_ForeColor(This,clr)	\
    ( (This)->lpVtbl -> put_ForeColor(This,clr) ) 

#define IPxLHistogram_get_ForeColor(This,pclr)	\
    ( (This)->lpVtbl -> get_ForeColor(This,pclr) ) 

#define IPxLHistogram_put_Enabled(This,vbool)	\
    ( (This)->lpVtbl -> put_Enabled(This,vbool) ) 

#define IPxLHistogram_get_Enabled(This,pbool)	\
    ( (This)->lpVtbl -> get_Enabled(This,pbool) ) 

#define IPxLHistogram_SetHistogramData(This,nDataPoints,pData)	\
    ( (This)->lpVtbl -> SetHistogramData(This,nDataPoints,pData) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPxLHistogram_INTERFACE_DEFINED__ */


#ifndef __IPxLGraph_INTERFACE_DEFINED__
#define __IPxLGraph_INTERFACE_DEFINED__

/* interface IPxLGraph */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IPxLGraph;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("41279666-1BC7-4DEB-8E60-64F0C4E7EAFB")
    IPxLGraph : public IDispatch
    {
    public:
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BackColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BackColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddElement( 
            /* [retval][out] */ LONG *elementId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveElement( 
            /* [in] */ LONG elementNumber) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetElementCount( 
            /* [retval][out] */ LONG *elementCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetXRange( 
            /* [in] */ DOUBLE min,
            /* [in] */ DOUBLE max) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetYRange( 
            /* [in] */ DOUBLE min,
            /* [in] */ DOUBLE max) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetXYData( 
            /* [in] */ LONG elementNumber,
            /* [in] */ DOUBLE *xData,
            /* [in] */ DOUBLE *yData,
            /* [in] */ LONG nPoints) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetYData( 
            /* [in] */ LONG elementNumber,
            /* [in] */ DOUBLE *yData,
            /* [in] */ LONG nPoints) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLineColor( 
            /* [in] */ LONG elementNumber,
            /* [in] */ OLE_COLOR color) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLineWidth( 
            /* [in] */ LONG elementNumber,
            /* [in] */ LONG width) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoScaleX( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoScaleX( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TrackMouse( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TrackMouse( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoScaleY( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoScaleY( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetXRange( 
            /* [out] */ DOUBLE *pMin,
            /* [out] */ DOUBLE *pMax) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetYRange( 
            /* [out] */ DOUBLE *pMin,
            /* [out] */ DOUBLE *pMax) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPxLGraphVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPxLGraph * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPxLGraph * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPxLGraph * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPxLGraph * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPxLGraph * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPxLGraph * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPxLGraph * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BackColor )( 
            IPxLGraph * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BackColor )( 
            IPxLGraph * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddElement )( 
            IPxLGraph * This,
            /* [retval][out] */ LONG *elementId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveElement )( 
            IPxLGraph * This,
            /* [in] */ LONG elementNumber);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IPxLGraph * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetElementCount )( 
            IPxLGraph * This,
            /* [retval][out] */ LONG *elementCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetXRange )( 
            IPxLGraph * This,
            /* [in] */ DOUBLE min,
            /* [in] */ DOUBLE max);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetYRange )( 
            IPxLGraph * This,
            /* [in] */ DOUBLE min,
            /* [in] */ DOUBLE max);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetXYData )( 
            IPxLGraph * This,
            /* [in] */ LONG elementNumber,
            /* [in] */ DOUBLE *xData,
            /* [in] */ DOUBLE *yData,
            /* [in] */ LONG nPoints);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetYData )( 
            IPxLGraph * This,
            /* [in] */ LONG elementNumber,
            /* [in] */ DOUBLE *yData,
            /* [in] */ LONG nPoints);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLineColor )( 
            IPxLGraph * This,
            /* [in] */ LONG elementNumber,
            /* [in] */ OLE_COLOR color);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLineWidth )( 
            IPxLGraph * This,
            /* [in] */ LONG elementNumber,
            /* [in] */ LONG width);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AutoScaleX )( 
            IPxLGraph * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AutoScaleX )( 
            IPxLGraph * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TrackMouse )( 
            IPxLGraph * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TrackMouse )( 
            IPxLGraph * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AutoScaleY )( 
            IPxLGraph * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AutoScaleY )( 
            IPxLGraph * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetXRange )( 
            IPxLGraph * This,
            /* [out] */ DOUBLE *pMin,
            /* [out] */ DOUBLE *pMax);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetYRange )( 
            IPxLGraph * This,
            /* [out] */ DOUBLE *pMin,
            /* [out] */ DOUBLE *pMax);
        
        END_INTERFACE
    } IPxLGraphVtbl;

    interface IPxLGraph
    {
        CONST_VTBL struct IPxLGraphVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPxLGraph_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPxLGraph_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPxLGraph_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPxLGraph_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPxLGraph_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPxLGraph_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPxLGraph_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IPxLGraph_put_BackColor(This,clr)	\
    ( (This)->lpVtbl -> put_BackColor(This,clr) ) 

#define IPxLGraph_get_BackColor(This,pclr)	\
    ( (This)->lpVtbl -> get_BackColor(This,pclr) ) 

#define IPxLGraph_AddElement(This,elementId)	\
    ( (This)->lpVtbl -> AddElement(This,elementId) ) 

#define IPxLGraph_RemoveElement(This,elementNumber)	\
    ( (This)->lpVtbl -> RemoveElement(This,elementNumber) ) 

#define IPxLGraph_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IPxLGraph_GetElementCount(This,elementCount)	\
    ( (This)->lpVtbl -> GetElementCount(This,elementCount) ) 

#define IPxLGraph_SetXRange(This,min,max)	\
    ( (This)->lpVtbl -> SetXRange(This,min,max) ) 

#define IPxLGraph_SetYRange(This,min,max)	\
    ( (This)->lpVtbl -> SetYRange(This,min,max) ) 

#define IPxLGraph_SetXYData(This,elementNumber,xData,yData,nPoints)	\
    ( (This)->lpVtbl -> SetXYData(This,elementNumber,xData,yData,nPoints) ) 

#define IPxLGraph_SetYData(This,elementNumber,yData,nPoints)	\
    ( (This)->lpVtbl -> SetYData(This,elementNumber,yData,nPoints) ) 

#define IPxLGraph_SetLineColor(This,elementNumber,color)	\
    ( (This)->lpVtbl -> SetLineColor(This,elementNumber,color) ) 

#define IPxLGraph_SetLineWidth(This,elementNumber,width)	\
    ( (This)->lpVtbl -> SetLineWidth(This,elementNumber,width) ) 

#define IPxLGraph_get_AutoScaleX(This,pVal)	\
    ( (This)->lpVtbl -> get_AutoScaleX(This,pVal) ) 

#define IPxLGraph_put_AutoScaleX(This,newVal)	\
    ( (This)->lpVtbl -> put_AutoScaleX(This,newVal) ) 

#define IPxLGraph_get_TrackMouse(This,pVal)	\
    ( (This)->lpVtbl -> get_TrackMouse(This,pVal) ) 

#define IPxLGraph_put_TrackMouse(This,newVal)	\
    ( (This)->lpVtbl -> put_TrackMouse(This,newVal) ) 

#define IPxLGraph_get_AutoScaleY(This,pVal)	\
    ( (This)->lpVtbl -> get_AutoScaleY(This,pVal) ) 

#define IPxLGraph_put_AutoScaleY(This,newVal)	\
    ( (This)->lpVtbl -> put_AutoScaleY(This,newVal) ) 

#define IPxLGraph_GetXRange(This,pMin,pMax)	\
    ( (This)->lpVtbl -> GetXRange(This,pMin,pMax) ) 

#define IPxLGraph_GetYRange(This,pMin,pMax)	\
    ( (This)->lpVtbl -> GetYRange(This,pMin,pMax) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPxLGraph_INTERFACE_DEFINED__ */



#ifndef __PxLControlsLib_LIBRARY_DEFINED__
#define __PxLControlsLib_LIBRARY_DEFINED__

/* library PxLControlsLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PxLControlsLib;

#ifndef ___IPxLSubwindowEvents_DISPINTERFACE_DEFINED__
#define ___IPxLSubwindowEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IPxLSubwindowEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IPxLSubwindowEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FD8B757C-5221-4C31-A29F-B6D68AB89A5A")
    _IPxLSubwindowEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IPxLSubwindowEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IPxLSubwindowEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IPxLSubwindowEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IPxLSubwindowEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IPxLSubwindowEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IPxLSubwindowEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IPxLSubwindowEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IPxLSubwindowEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } _IPxLSubwindowEventsVtbl;

    interface _IPxLSubwindowEvents
    {
        CONST_VTBL struct _IPxLSubwindowEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IPxLSubwindowEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IPxLSubwindowEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IPxLSubwindowEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IPxLSubwindowEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IPxLSubwindowEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IPxLSubwindowEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IPxLSubwindowEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IPxLSubwindowEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PxLSubwindow;

#ifdef __cplusplus

class DECLSPEC_UUID("3964AE07-DDDC-470C-8B43-2AB15DB6627A")
PxLSubwindow;
#endif

#ifndef ___IPxLLookupEvents_DISPINTERFACE_DEFINED__
#define ___IPxLLookupEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IPxLLookupEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IPxLLookupEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("1A1E63C1-7859-47C6-9159-339A7BE7FCDA")
    _IPxLLookupEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IPxLLookupEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IPxLLookupEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IPxLLookupEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IPxLLookupEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IPxLLookupEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IPxLLookupEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IPxLLookupEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IPxLLookupEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } _IPxLLookupEventsVtbl;

    interface _IPxLLookupEvents
    {
        CONST_VTBL struct _IPxLLookupEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IPxLLookupEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IPxLLookupEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IPxLLookupEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IPxLLookupEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IPxLLookupEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IPxLLookupEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IPxLLookupEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IPxLLookupEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PxLLookup;

#ifdef __cplusplus

class DECLSPEC_UUID("7555A9BA-088F-490D-B355-A90821E33171")
PxLLookup;
#endif

EXTERN_C const CLSID CLSID_PxLHistogram;

#ifdef __cplusplus

class DECLSPEC_UUID("1505F715-4138-4366-B73C-347CC2AB1908")
PxLHistogram;
#endif

EXTERN_C const CLSID CLSID_PxLGraph;

#ifdef __cplusplus

class DECLSPEC_UUID("85F80D1D-7645-4984-9782-344C7653D1E9")
PxLGraph;
#endif
#endif /* __PxLControlsLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


