// EngineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ScreenCapture.h"
#include "H264Encoder.h"
#include <conio.h>
#include <iostream>

#include <GDIPlus.h>

void GetCLSID(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
		}
	}
}

void SaveImage(AVFrame *frame)
{
	Gdiplus::Bitmap image(frame->width, frame->height, frame->linesize[0], PixelFormat32bppARGB, frame->data[0]);
	CLSID clsID;
	GetCLSID(L"image/png", &clsID);
	wchar_t fileName[100];
	swprintf(fileName, 100, L"c:\\1\\pic-%d.png", frame->pts);
	image.Save(fileName, &clsID);
}

//AVFramePtr audioSample = AVFramePtr(capture.ReadPacket(), [](AVFrame* f) {  av_frame_free(&f); });
//		audioEncoder.Encode(audioSample.get());





int _tmain(int argc, _TCHAR* argv[])
{	
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CScreenCapture capture;
	HWND hWnd = GetDesktopWindow();

	CAudioCapture audioCapture;
	std::shared_ptr<CFileWriter>  m_writer(new CFileWriter());
	CVideoEncoder encoder(m_writer);
	CAudioEncoder audioEncoder(m_writer);

	m_writer->InitFile("c:\\1.mp4");
	encoder.AddVideoStream(1920, 1200, 4000);
	//audioEncoder.AddAudioStream();
	audioCapture.Init();
	m_writer->Start();

	while (!_kbhit())
	{
		auto t0=clock();
		CSample* sample;
		capture.Grab(hWnd, &sample);
		auto t1 = clock();
		encoder.Encode(sample->get());
		auto t2 = clock();
		printf("time to grab (%d)  encode (%d)\n", t1 - t0, t2 - t1);
		long timePassed=t2 - t0;
		Sleep(__max(0,33 - timePassed));
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}

