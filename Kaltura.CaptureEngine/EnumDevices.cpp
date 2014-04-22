#include "stdafx.h"
#include "EnumDevices.h"

#include <dshow.h>
#pragma comment(lib,"Strmiids.lib")

#include "atlbase.h"
CEnumDevices::CEnumDevices()
{
}


CEnumDevices::~CEnumDevices()
{
}


HRESULT CEnumDevices::EnumInputs(bool bVideo, std::vector<std::wstring>& devicesName)
{
	CComPtr<ICreateDevEnum> spDeviceNum;
	HRESULT hr=spDeviceNum.CoCreateInstance(CLSID_SystemDeviceEnum);

	CComPtr<IEnumMoniker> spClassEnumerator; 
	hr = spDeviceNum->CreateClassEnumerator(bVideo ? CLSID_VideoInputDeviceCategory : CLSID_AudioInputDeviceCategory, &spClassEnumerator, 0);
	int tCount = -1;

	CComPtr<IMoniker> spMoniker;
	// access video input device
	while (spClassEnumerator->Next(1, &spMoniker, NULL) == S_OK)
	{

		CComPtr<IPropertyBag> spPropertyBag;
		// bind to storage
		hr = spMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&spPropertyBag);
		if (FAILED(hr))
		{
			continue;
		}

		CComVariant variant;
		// retrieve the device's name
		hr = spPropertyBag->Read(L"FriendlyName", &variant, 0);
		devicesName.push_back(variant.bstrVal);
		VariantClear(&variant);

		// retrieve the device's description
		hr = spPropertyBag->Read(L"Description", &variant, 0);
		VariantClear(&variant);

		// retrieve the device's description
		hr = spPropertyBag->Read(L"DevicePath", &variant, 0);
		VariantClear(&variant);

		spMoniker.Release();
	}

		/*
	CComPtr<IMFMediaSource> spSource;
	CComPtr<IMFAttributes> spAttributes;
	IMFActivate **ppDevices = NULL;

	// Create an attribute store to specify the enumeration parameters.
	HRESULT hr = MFCreateAttributes(&spAttributes, 1);
	if (FAILED(hr))
	{
		return hr;
	}

	// Source type: video capture devices

	GUID cap = bVideo ? MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID : MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID;
	hr = spAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, cap);
	if (FAILED(hr))
	{
		return hr;
	}

	// Enumerate devices.
	UINT32 count(0);
	hr = MFEnumDeviceSources(spAttributes, &ppDevices, &count);
	if (FAILED(hr))
	{
		return hr;
	}

	if (count == 0)
	{
		return E_FAIL;
	}


	for (DWORD i = 0; i < count; i++)
	{
		HRESULT hr = S_OK;
		WCHAR *szFriendlyName = NULL;

		// Try to get the display name.
		UINT32 cchName;
		hr = ppDevices[i]->GetAllocatedString(
			MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
			&szFriendlyName, &cchName);

		if (SUCCEEDED(hr))
		{
			devicesName.push_back(szFriendlyName);
		}
		CoTaskMemFree(szFriendlyName);
		ppDevices[i]->Release();
	}
	CoTaskMemFree(ppDevices);
	*/
	return hr;
}