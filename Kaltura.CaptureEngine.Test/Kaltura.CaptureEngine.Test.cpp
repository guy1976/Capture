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


		SetOutputFile(pipeline1, "c:\\screen.mp4");
		AddScreenSource(pipeline1, hWnd);

		auto pipeline2 = CreatePipeline();
		SetOutputFile(pipeline2, "c:\\cam.mp4");
		AddCameraSource(pipeline2, "video=Integrated Camera");
		AddAudioSource(pipeline2,"audio=Microphone (Realtek High Defini");

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

