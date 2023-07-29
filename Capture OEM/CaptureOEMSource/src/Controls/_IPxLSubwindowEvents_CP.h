#pragma once

template<class T>
class CProxy_IPxLSubwindowEvents :
	public IConnectionPointImpl<T, &__uuidof(_IPxLSubwindowEvents)>
{
public:
	HRESULT Fire_SubwindowChanged( LONG  left,  LONG  top,  LONG  width,  LONG  height)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant avarParams[4];
				avarParams[3] = left;	avarParams[3].vt = VT_I4;
				avarParams[2] = top;	avarParams[2].vt = VT_I4;
				avarParams[1] = width;	avarParams[1].vt = VT_I4;
				avarParams[0] = height;	avarParams[0].vt = VT_I4;
				DISPPARAMS params = { avarParams, NULL, 4, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
			}
		}
		return hr;
	}
	HRESULT Fire_SubwindowChanging( LONG  left,  LONG  top,  ULONG  width,  ULONG  height)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant avarParams[4];
				avarParams[3] = left;	avarParams[3].vt = VT_I4;
				avarParams[2] = top;	avarParams[2].vt = VT_I4;
				avarParams[1] = width;	avarParams[1].vt = VT_I4;
				avarParams[0] = height;	avarParams[0].vt = VT_I4;
				DISPPARAMS params = { avarParams, NULL, 4, 0 };
				hr = pConnection->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
			}
		}
		return hr;
	}
};

