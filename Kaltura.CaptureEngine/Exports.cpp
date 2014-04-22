
#include "stdafx.h"
#include "Exports.h"
#include "CapturePipeline.h"

CCaptureEnginePtr CreatePipeline()
{
	return new CCapturePipeline();
}

void InitPipeline(CCaptureEnginePtr pipeLine, void* hScreen, char * videoInput, char* audioInput, char* fileoutput)
{
	((CCapturePipeline*)pipeLine)->Init((HWND)hScreen, videoInput, audioInput, fileoutput);
}

void Start(CCaptureEnginePtr pipeLine)
{
	((CCapturePipeline*)pipeLine)->Start();
}

void Stop(CCaptureEnginePtr pipeLine)
{

	((CCapturePipeline*)pipeLine)->Stop();
}
