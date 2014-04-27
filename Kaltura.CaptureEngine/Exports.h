#include "..\Kaltura.CaptureEngine.Common\CaptureEngineSamplesProcessor.h"

typedef void * CCaptureEnginePtr;
typedef void * CaptureDevicePtr;


extern "C" __declspec(dllexport) CCaptureEnginePtr CreatePipeline();
extern "C" __declspec(dllexport) void DestroyPipeline(CCaptureEnginePtr pipeLine);


extern "C" __declspec(dllexport) CaptureDevicePtr CreateDesktopCapture(void * handle);
extern "C" __declspec(dllexport) CaptureDevicePtr CreateDevice(CaptureDeviceInfo*  device);
extern "C" __declspec(dllexport) void DestroyDevice(CaptureDevicePtr device);


extern "C" __declspec(dllexport) void StartDevice(CaptureDevicePtr CaptureDeviceInfo);
extern "C" __declspec(dllexport) void StopDevice(CaptureDevicePtr CaptureDeviceInfo);


extern "C" __declspec(dllexport) void AddVideoSource(CCaptureEnginePtr pipeLine, CaptureDevicePtr device);
extern "C" __declspec(dllexport) void AddAudioSource(CCaptureEnginePtr pipeLine, CaptureDevicePtr device);
extern "C" __declspec(dllexport) void AddProcessor(CCaptureEnginePtr pipeLine, CCaptureEngineSamplesProcessor* pProcessor);
extern "C" __declspec(dllexport) void SetOutputFile(CCaptureEnginePtr pipeLine, char * fileName);

extern "C" __declspec(dllexport) void Init(CCaptureEnginePtr pipeLine);
extern "C" __declspec(dllexport) void Close(CCaptureEnginePtr pipeLine);

extern "C" __declspec(dllexport) void EnumDevices(CaptureDeviceInfo devices[32]);
extern "C" __declspec(dllexport) void ShowPreviewWindow(CaptureDevicePtr device);
