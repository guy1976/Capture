#include "stdafx.h"
#include "ScreenCapture.h"

#include "H264Encoder.h"


#pragma comment(lib,"Gdiplus.lib")

#include <GDIPlus.h>


int frameIndex = 0;
CScreenCapture::CScreenCapture()
{
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
	while (!m_hBitmaps.empty())
	{
		HBITMAP ret = m_hBitmaps.top();
		DeleteObject(ret);
	}
	m_hWND = NULL;
	m_currentRect.left = m_currentRect.bottom = m_currentRect.right = m_currentRect.top = 0;
}

HBITMAP CScreenCapture::GetHBitmap()
{
	HBITMAP ret=m_hBitmaps.top();
	m_hBitmaps.pop();
	return ret;
}
void CScreenCapture::ReleaseHBitmap(HBITMAP hBmp)
{
	m_hBitmaps.push(hBmp);
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

	for (int i = 0; i < 5; i++)
	{
		void *buffer = NULL;
		HBITMAP hbmp = CreateDIBSection(m_destHDC, &bmi, DIB_RGB_COLORS, &buffer, NULL, 0);

		ReleaseHBitmap(hbmp);
	}

}


HRESULT CScreenCapture::Grab(HWND hWnd, CSample**pFrame)
{

	RECT ClientRect;
	GetClientRect(hWnd, &ClientRect);
	Init(hWnd, ClientRect);

	HBITMAP hBitmap=GetHBitmap();

	SelectObject(m_destHDC, hBitmap);
	BITMAP bmp;
	/* Get info from the bitmap */
	GetObject(hBitmap, sizeof(BITMAP), &bmp);

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

	AVFrame* videoFrame = CVideoEncoder::CreateVideoFrame((BYTE*)bmp.bmBits, bmp.bmWidth, abs(bmp.bmHeight), bmp.bmWidthBytes, AVPixelFormat::PIX_FMT_ARGB);
	videoFrame->pts = (frameIndex++);
	*pFrame = new CSample(videoFrame);

	ReleaseHBitmap(hBitmap);
	return S_OK;
}
