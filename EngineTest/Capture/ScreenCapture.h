#pragma once

#include "..\FFMpegWrappers\FFMpegCommon.h"
#include "Utils.h"
#include "Capture.h"
#include  <chrono>

class CBitmapCache : public CCache<HBITMAP>
{
public:
	CBitmapCache()
	{
		m_deleter = [](HBITMAP hBmp) 
		{ 
			::DeleteObject(hBmp); 
		};
	}

};

class CScreenCaptureSample : public CSample
{
	HBITMAP m_hBMP; 
	std::shared_ptr<CBitmapCache> m_bitmapCache;
public:
	CScreenCaptureSample(std::shared_ptr<CBitmapCache>  cache, HBITMAP hBMP);
	virtual ~CScreenCaptureSample();
};

class CScreenCapture : public CCapture
{
	HDC m_sourceHDC,m_destHDC;
	HWND m_hWND;
	RECT m_currentRect;

	void Free(); 
	std::shared_ptr<CBitmapCache> m_bitmapCache;
	std::chrono::system_clock::time_point m_lastCaptureTime,m_firstCaptureFrame;
	CSample* Capture();
	AVCodecContext m_codecContext;
public:
	HRESULT Init(HWND hWND, RECT rect);
	int TotalStreams();
	AVCodecContext* GetAVCodecContext(int index);
	CScreenCapture();
	virtual ~CScreenCapture();
};

