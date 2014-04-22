#pragma once

#ifdef  KALTURACAPTUREENGINECOMMON_EXPORTS
#define  DLL_DECL __declspec(dllexport)  
#else
#define DLL_DECL __declspec(dllimport)  
#pragma comment(lib,"..\\Debug\\Kaltura.CaptureEngine.Common.lib")
#endif