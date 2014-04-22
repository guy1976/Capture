// Kaltura.CaptureEngine.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "..\Kaltura.CaptureEngine\Exports.h"


#pragma comment(lib,"..\\Debug\\Kaltura.CaptureEngine.lib")


int _tmain(int argc, _TCHAR* argv[])
{
	

	//	CEnumDevices a;
	//a.Enum();

	HWND hWnd = GetDesktopWindow();

	{
		auto pipeline1 = CreatePipeline();
		auto pipeline2 = CreatePipeline();

		InitPipeline(pipeline1,hWnd, "", "", "c:\\screen.mp4");
		InitPipeline(pipeline2,NULL, "video=Integrated Camera", "audio=Microphone (Realtek High Defini", "c:\\cam.mp4");

		Start(pipeline1);
		Start(pipeline2);
		while (!_kbhit())
		{
			Sleep(10);
		}
		Stop(pipeline1);
		Stop(pipeline2);

	}
	return 0;
}

