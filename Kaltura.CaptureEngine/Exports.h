#include "..\Kaltura.CaptureEngine.Common\CaptureEngineSamplesProcessor.h"

typedef void * CCaptureEnginePtr;
typedef void * CaptureDevicePtr;


extern "C" __declspec(dllexport) CCaptureEnginePtr CreatePipeline();
extern "C" __declspec(dllexport) CaptureDevicePtr CreateDesktopCapture(void * handle);
extern "C" __declspec(dllexport) CaptureDevicePtr CreateDevice(CaptureDevice*  device);

extern "C" __declspec(dllexport) void AddVideoSource(CCaptureEnginePtr pipeLine, CaptureDevicePtr device);
extern "C" __declspec(dllexport) void AddAudioSource(CCaptureEnginePtr pipeLine, CaptureDevicePtr device);
extern "C" __declspec(dllexport) void AddProcessor(CCaptureEnginePtr pipeLine, CCaptureEngineSamplesProcessor* pProcessor);
extern "C" __declspec(dllexport) void SetOutputFile(CCaptureEnginePtr pipeLine, char * fileName);

extern "C" __declspec(dllexport) void Start(CCaptureEnginePtr pipeLine);

extern "C" __declspec(dllexport) void Stop(CCaptureEnginePtr pipeLine);
extern "C" __declspec(dllexport) void EnumDevices(CaptureDevice devices[32]);
extern "C" __declspec(dllexport) void ShowPreviewWindow(CaptureDevicePtr device);
