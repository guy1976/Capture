#include "stdafx.h"
#include "ScreenCapture.h"

#include "H264Encoder.h"

CAudioCapture capture;
std::shared_ptr<CFileWriter>  m_writer(new CFileWriter());
CVideoEncoder encoder(m_writer);
CAudioEncoder audioEncoder(m_writer);

#pragma comment(lib,"d3d9.lib")


#pragma comment(lib,"Gdiplus.lib")

#include <GDIPlus.h>
using namespace Gdiplus;


int frameIndex = 0;
CScreenCapture::CScreenCapture()
{
	m_writer->InitFile("c:\\1.mp4");
	encoder.AddVideoStream( 1920, 1200, 4000);	
	//audioEncoder.AddAudioStream();
	capture.Init();
	m_writer->Start();
}


CScreenCapture::~CScreenCapture()
{
}
void GetCLSID(const WCHAR* format, CLSID* pClsid){
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
		}
	}
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
	if (m_sourceHDC != NULL)
	{
		ReleaseDC(m_hWND,m_sourceHDC);
		m_sourceHDC = NULL;
	}
	if (m_destHDC != NULL)
	{
		DeleteDC(m_destHDC);
		m_destHDC = NULL;
	}
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
	void *buffer = NULL;
	HBITMAP hbmp = CreateDIBSection(m_destHDC, &bmi, DIB_RGB_COLORS, &buffer, NULL, 0);

	SelectObject(m_destHDC, hbmp);
	/* Get info from the bitmap */
	GetObject(hbmp, sizeof(BITMAP), &m_bmp);

}

int imageId = 0;
HRESULT CScreenCapture::Grab(HWND hWnd)
{
	auto t0 = clock();

	RECT ClientRect;
	GetClientRect(hWnd, &ClientRect);
	Init(hWnd, ClientRect);


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
	auto t1 = clock();

	AVFramePtr videoFrame = AVFramePtr(CVideoEncoder::CreateVideoFrame((BYTE*)m_bmp.bmBits, m_bmp.bmWidth,abs(m_bmp.bmHeight), m_bmp.bmWidthBytes, AVPixelFormat::PIX_FMT_ARGB), [](AVFrame* f) {  av_frame_free(&f); });
	videoFrame->pts = (frameIndex++);
	encoder.Encode(videoFrame.get());
	auto t2 = clock();
	printf("time to grab (%d)  encode (%d)\n", t1 - t0, t2 - t1);


	/*
	Bitmap image((INT)Desc.Width, (INT)Desc.Height, Desc.Width*4,PixelFormat32bppARGB, (BYTE*)LockedRect.pBits);
	CLSID clsID;
	GetCLSID(L"image/png", &clsID);
	wchar_t fileName[100];
	swprintf(fileName, 100, L"c:\\1\\pic-%d.png", imageId++);
	image.Save(fileName, &clsID);
	*/
	//AVFramePtr audioSample = AVFramePtr(capture.ReadPacket(), [](AVFrame* f) {  av_frame_free(&f); });
	//		audioEncoder.Encode(audioSample.get());
	return S_OK;
}
