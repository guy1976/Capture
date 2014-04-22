#include "Common.h"
#include "CaptureEngineSample.h"

class DLL_DECL CCaptureEngineSamplesProcessor
{
public:
	virtual void ProcessSample(CCaptureEngineSample *pSample) = 0;
};