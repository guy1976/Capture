#pragma once
#include "Common.h"

class DLL_DECL CCaptureEngineSample
{
public:
	CCaptureEngineSample();
	virtual ~CCaptureEngineSample();
};

class DLL_DECL CCaptureEngineVideoSample
{
	int m_width;
	int m_height;
	int m_stride;
	unsigned char *m_pBuffer;
	int m_pixelFormat;
public:
	CCaptureEngineVideoSample();
	virtual ~CCaptureEngineVideoSample();


	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }
	int GetStride() { return m_stride; }
	unsigned char* GetBuffer() { return m_pBuffer; }
	int GetPixelFormat() { return m_pixelFormat; }


};

