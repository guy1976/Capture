#include "stdafx.h"
#include "CaptureEngineSample.h"


CCaptureEngineSample::CCaptureEngineSample()
{
}


CCaptureEngineSample::~CCaptureEngineSample()
{
}


CCaptureEngineVideoSample::CCaptureEngineVideoSample(int width, int height, int stride, int pixelFormat, unsigned char* buffer) :
m_width(width), m_height(height), m_pBuffer(buffer), m_stride(stride), m_pixelFormat(pixelFormat)
{
}
CCaptureEngineVideoSample::~CCaptureEngineVideoSample()
{
}