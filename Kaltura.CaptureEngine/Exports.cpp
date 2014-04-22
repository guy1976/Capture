
#include "stdafx.h"
#include "Exports.h"
#include "CapturePipeline.h"

CCaptureEnginePtr CreatePipeline()
{
	return new CCapturePipeline();
}

void AddAudioSource(CCaptureEnginePtr pipeLine, char * inputName)
{
	((CCapturePipeline*)pipeLine)->AddAudioSource(inputName);
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
