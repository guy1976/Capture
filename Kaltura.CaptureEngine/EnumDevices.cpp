#include "stdafx.h"
#include "EnumDevices.h"
#include "Mfidl.h"
#include "Mfapi.h"
#pragma comment(lib,"Mfplat.lib")
#pragma comment(lib,"mf.lib")

CEnumDevices::CEnumDevices()
{
}


CEnumDevices::~CEnumDevices()
{
}

HRESULT CEnumDevices::Enum()
{
	CoInitialize(NULL);

	IMFMediaSource *pSource = NULL;
	IMFAttributes *pAttributes = NULL;
	IMFActivate **ppDevices = NULL;

	// Create an attribute store to specify the enumeration parameters.
	HRESULT hr = MFCreateAttributes(&pAttributes, 1);
	if (FAILED(hr))
	{
		goto done;
	}

	// Source type: video capture devices
	hr = pAttributes->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE
		);
	if (FAILED(hr))
	{
		goto done;
	}

	// Enumerate devices.
	UINT32 count;
	hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
	if (FAILED(hr))
	{
		goto done;
	}

	if (count == 0)
	{
		hr = E_FAIL;
		goto done;
	}

	// Create the media source object.
	hr = ppDevices[0]->ActivateObject(IID_PPV_ARGS(&pSource));
	if (FAILED(hr))
	{
		goto done;
	}

	//*ppSource = pSource;
	//(*ppSource)->AddRef();

done:
	//SafeRelease(&pAttributes);

	for (DWORD i = 0; i < count; i++)
	{
	//	SafeRelease(&ppDevices[i]);
	}
	CoTaskMemFree(ppDevices);
//	SafeRelease(&pSource);

	CoUninitialize();
	return hr;
}