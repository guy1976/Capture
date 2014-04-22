
#include "stdafx.h"
#include "Exports.h"
#include "CapturePipeline.h"
#include "EnumDevices.h"

std::string ws2s(const std::wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

CCaptureEnginePtr CreatePipeline()
{
	return new CCapturePipeline();
}

void AddSource(CCaptureEnginePtr pipeLine, CaptureDevice*  device)
{
	auto pipeline = ((CCapturePipeline*)pipeLine);
	if (device->DeviceType == CaptureDeviceType::Video)
		pipeline->AddCameraSource(ws2s(device->FFMpegInput));
	if (device->DeviceType == CaptureDeviceType::Audio)
		pipeline->AddAudioSource(ws2s(device->FFMpegInput));
}

void AddCameraSource(CCaptureEnginePtr pipeLine, char * inputName)
{
	((CCapturePipeline*)pipeLine)->AddCameraSource(inputName);
}
void AddScreenSource(CCaptureEnginePtr pipeLine, void * handle)
{
	((CCapturePipeline*)pipeLine)->AddScreenSource((HWND)handle);
}
void SetOutputFile(CCaptureEnginePtr pipeLine, char * fileName)
{
	((CCapturePipeline*)pipeLine)->SetOutputFile(fileName);
}
void Start(CCaptureEnginePtr pipeLine)
{
	((CCapturePipeline*)pipeLine)->Start();
}

void Stop(CCaptureEnginePtr pipeLine)
{

	((CCapturePipeline*)pipeLine)->Stop();
}
void AddProcessor(CCaptureEnginePtr pipeLine, CCaptureEngineSamplesProcessor* pProcessor)
{

	((CCapturePipeline*)pipeLine)->AddProcessor(pProcessor);
}


void Preview(CaptureDevice* device)
{
}

void EnumDevices(CaptureDevice devices[32])
{
	memset(devices, 0, sizeof(CaptureDevice[32]));
//	CoInitialize(NULL);
	std::vector<std::wstring> devicesName;
	CEnumDevices a;
	a.EnumInputs(true, devicesName);

	int index = 0;
	for each (auto vidDevice in devicesName)
	{
		CaptureDevice& device = devices[index++];
		device.DeviceType = CaptureDeviceType::Video;
		wcsncpy(device.FFMpegDevice, L"dshow",100);
		wcsncpy(device.FriendlyName, vidDevice.c_str(), 100);

		auto s = vidDevice.substr(0, 32);
		s=s.substr(0,32);
		wcsncpy(device.FFMpegInput, (std::wstring(L"video=") + s).c_str(),100);
	}
	devicesName.clear();
	a.EnumInputs(false, devicesName);

	for each (auto vidDevice in devicesName)
	{
		CaptureDevice& device = devices[index++];
		device.DeviceType = CaptureDeviceType::Audio;
		wcsncpy(device.FFMpegDevice, L"dshow",100);
		wcsncpy(device.FriendlyName, vidDevice.c_str(), 100);

		auto s = vidDevice.substr(0, 32);
		wcsncpy(device.FFMpegInput, (std::wstring(L"audio=") + s).c_str(),100);
	}
//	CoUninitialize();
}
