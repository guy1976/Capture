// Kaltura.CaptureEngine.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "..\Kaltura.CaptureEngine\Exports.h"

#include <string>

#pragma comment(lib,"..\\Debug\\Kaltura.CaptureEngine.lib")
#pragma comment(lib,"..\\Debug\\Kaptura.VideoSegmentation.lib")


extern "C" __declspec(dllimport) CCaptureEngineSamplesProcessor* CreateSceneDetector();

int _tmain(int argc, _TCHAR* argv[])
{
	
	CaptureDeviceInfo devices[32];
	EnumDevices(devices);


	HWND hWnd = GetDesktopWindow();

	auto sceneDetector = CreateSceneDetector();
	
	{
		auto pipeline1 = CreatePipeline();



		SetOutputFile(pipeline1, "c:\\screen.mp4");

		auto screenCapture = CreateDesktopCapture(hWnd);
		auto camDevice = CreateDevice(&devices[1]);
		auto mic = CreateDevice(&devices[2]);

		AddVideoSource(pipeline1, screenCapture);
		//AddProcessor(pipeline1, sceneDetector);

		
		auto pipeline2 = CreatePipeline();
		SetOutputFile(pipeline2, "c:\\cam.mp4");
		AddVideoSource(pipeline2, camDevice);
		AddAudioSource(pipeline2, mic);
		//SetPreview(pipeline2);
		Init(pipeline1);
		Init(pipeline2);

		StartDevice(screenCapture);
		StartDevice(camDevice);
		StartDevice(mic);
		while (!_kbhit())
		{
			//ShowPreviewWindow(camDevice);
			Sleep(10);
		}
		Close(pipeline1);
		Close(pipeline2);

	}
	return 0;
}

