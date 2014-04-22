#pragma once

#include "Common.h"
#include "CaptureEngineSample.h"

class DLL_DECL  CCaptureEngineSamplesProcessor
{
public:
	CCaptureEngineSamplesProcessor();
	virtual void ProcessSample(CCaptureEngineSample *pSample) = 0;
};