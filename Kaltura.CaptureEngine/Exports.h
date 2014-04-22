#include "..\Kaltura.CaptureEngine.Common\CaptureEngineSamplesProcessor.h"

typedef void * CCaptureEnginePtr;


extern "C" __declspec(dllexport) CCaptureEnginePtr CreatePipeline();
extern "C" __declspec(dllexport) void AddAudioSource(CCaptureEnginePtr pipeLine, char * videoInput);
extern "C" __declspec(dllexport) void AddCameraSource(CCaptureEnginePtr pipeLine, char * videoInput);
extern "C" __declspec(dllexport) void AddScreenSource(CCaptureEnginePtr pipeLine, void * handle);
extern "C" __declspec(dllexport) void AddProcessor(CCaptureEnginePtr pipeLine, CCaptureEngineSamplesProcessor* pProcessor);
extern "C" __declspec(dllexport) void SetOutputFile(CCaptureEnginePtr pipeLine, char * fileName);

extern "C" __declspec(dllexport) void Start(CCaptureEnginePtr pipeLine);

extern "C" __declspec(dllexport) void Stop(CCaptureEnginePtr pipeLine);


