

typedef void * CCaptureEnginePtr;


extern "C" __declspec(dllexport) CCaptureEnginePtr CreatePipeline();
extern "C" __declspec(dllexport) void InitPipeline(CCaptureEnginePtr pipeLine, void* hScreen, char * videoInput, char* audioInput, char* fileoutput);

extern "C" __declspec(dllexport) void Start(CCaptureEnginePtr pipeLine);

extern "C" __declspec(dllexport) void Stop(CCaptureEnginePtr pipeLine);

