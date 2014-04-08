#include "..\stdafx.h"
#include "ScreenCapture.h"
#include <chrono>

#include "..\FFMpegWrappers\FFMpegCommon.h"




#pragma comment(lib,"Gdiplus.lib")

#include <GDIPlus.h>

CScreenCaptureSample::CScreenCaptureSample(std::shared_ptr<CBitmapCache> cache, HBITMAP hBMP)
{
	m_bitmapCache= cache;
	m_hBMP = hBMP; 

	BITMAP bmp;
	/* Get info from the bitmap */
	GetObject(hBMP, sizeof(BITMAP), &bmp);

	auto frame = ::CreateVideoFrame((BYTE*)bmp.bmBits, bmp.bmWidth, abs(bmp.bmHeight), bmp.bmWidthBytes, AVPixelFormat::PIX_FMT_ARGB);
	Attach(frame);
}
CScreenCaptureSample::~CScreenCaptureSample()
{
	m_bitmapCache->Add(m_hBMP);
}

CScreenCapture::CScreenCapture()
{
	m_bitmapCache = std::make_shared<CBitmapCache>();
}


CScreenCapture::~CScreenCapture()
{
}


void CScreenCapture::Free()
{
	if (m_sourceHDC != NULL)
	{
		ReleaseDC(m_hWND, m_sourceHDC);
		m_sourceHDC = NULL;
	}
	if (m_destHDC != NULL)
	{
		DeleteDC(m_destHDC);
		m_destHDC = NULL;
	}
	m_hWND = NULL;
	m_currentRect.left = m_currentRect.bottom = m_currentRect.right = m_currentRect.top = 0;
}

HRESULT CScreenCapture::Init(HWND hWND,RECT rect)
{

	if (m_hWND == hWND &&
		m_currentRect.bottom == rect.bottom &&
		m_currentRect.top == rect.top&&
		m_currentRect.left == rect.left&&
		m_currentRect.right == rect.right)
	{
		return S_OK;
	}

	Free();
	
	m_currentRect = rect;
	m_hWND = hWND;
	m_sourceHDC = GetDC(hWND);

	int bpp = GetDeviceCaps(m_sourceHDC, BITSPIXEL);
	m_destHDC = CreateCompatibleDC(m_sourceHDC);

	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = rect.right - rect.left;
	bmi.bmiHeader.biHeight = -(rect.bottom - rect.top);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = bpp;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;

	for (int i = 0; i < 2; i++)
	{
		void *buffer = NULL;
		HBITMAP hBmp = CreateDIBSection(m_destHDC, &bmi, DIB_RGB_COLORS, &buffer, NULL, 0);

		m_bitmapCache->Add(hBmp);
	}
	return S_OK;
}


CSample* CScreenCapture::Capture()
{  
	auto time = std::chrono::system_clock::now();

	auto diff = time-m_lastCaptureTime;

	auto timeToSleep=std::chrono::milliseconds(33) - diff;
	if (timeToSleep>std::chrono::seconds(0))
	{
		std::this_thread::sleep_for(timeToSleep);
	}
	std::chrono::system_clock::time_point m = std::chrono::system_clock::from_time_t(0);
	if (m_firstCaptureFrame==m )
		m_firstCaptureFrame = time;
		
		
	m_lastCaptureTime = time;

	auto t1 = clock();


	HWND hWnd=GetDesktopWindow();
	RECT ClientRect;
	GetClientRect(hWnd, &ClientRect);
	Init(hWnd, ClientRect);

	HBITMAP hBitmap = m_bitmapCache->Pop();

	SelectObject(m_destHDC, hBitmap);

	RECT clip_rect;
	GetClientRect(hWnd, &clip_rect);
	if (!BitBlt(m_destHDC, 0, 0,
			clip_rect.right - clip_rect.left,
			clip_rect.bottom - clip_rect.top,
			m_sourceHDC,
			clip_rect.left, clip_rect.top, SRCCOPY | CAPTUREBLT)) 
	{
		printf("error grabbing...");
	}

	auto pFrame = new CScreenCaptureSample(m_bitmapCache, hBitmap);
	auto diff2 = (time - m_firstCaptureFrame);
	pFrame->get()->pts = diff2.count()/333333;

	auto t2 = clock();
	printf("time to capture (%d) %I64d\n", t2 - t1, pFrame->get()->pts);
	return pFrame;
}





