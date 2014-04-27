#pragma once

#ifdef  KALTURACAPTUREENGINECOMMON_EXPORTS
#define  DLL_DECL __declspec(dllexport)  
#else
#define DLL_DECL __declspec(dllimport)  
#pragma comment(lib,"..\\Debug\\Kaltura.CaptureEngine.Common.lib")
#endif

#include "Logger.h"

typedef enum CaptureDeviceType
{
	None,
	Video,
	Audio
};

struct CaptureDeviceInfo
{
	CaptureDeviceType DeviceType;
	wchar_t FriendlyName[100];
	wchar_t FFMpegInput[100];
	wchar_t FFMpegDevice[100];
};


void Log(LogLevel level);