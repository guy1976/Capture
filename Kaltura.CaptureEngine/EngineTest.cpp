// EngineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <iostream>
#include "EnumDevices.h"
#include <GDIPlus.h>
#include "CapturePipeline.h"
#include "Exports.h"

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

//	CEnumDevices a;
	//a.Enum();
	
	HWND hWnd = GetDesktopWindow();

	{
		CCapturePipeline pipeline1, pipeline2;

		pipeline1.Init(hWnd, "", "", "c:\\screen.mp4");
		pipeline2.Init(NULL, "video=Integrated Camera", "audio=Microphone (Realtek High Defini", "c:\\cam.mp4");


		pipeline1.Start();
		pipeline2.Start();
		while (!_kbhit())
		{
			Sleep(10);
		}
		pipeline1.Stop();
		pipeline2.Stop();

	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}

