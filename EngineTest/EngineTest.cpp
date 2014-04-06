// EngineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ScreenCapture.h"
#include "H264Encoder.h"
#include <conio.h>
#include <iostream>

#include <GDIPlus.h>
using namespace Gdiplus;



int _tmain(int argc, _TCHAR* argv[])
{	
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CScreenCapture capture;
	HWND hWnd = GetDesktopWindow();
	while (!_kbhit())
	{
		auto t0=clock();
		capture.Grab(hWnd);
		auto t1 = clock();
		long timePassed=t1 - t0;
		Sleep(__max(0,33 - timePassed));
	}
	GdiplusShutdown(gdiplusToken);
	return 0;
}

