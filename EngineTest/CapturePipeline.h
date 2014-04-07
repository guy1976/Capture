#pragma once

#include ".\Capture\ScreenCapture.h"
#include "FFMpegWrappers\FFMpegCommon.h"
#include "FFMpegWrappers\FileWriter.h"
#include "FFMpegWrappers\AudioEncoder.h"
#include "FFMpegWrappers\AudioCapture.h"
#include "FFMpegWrappers\VideoEncoder.h"

#include ".\SceneDetector.h"
#include <thread>         // std::thread

class CCapturePipeline
{
	std::shared_ptr<CFileWriter>  m_fileWriter;
	std::unique_ptr<CVideoEncoder> m_videoEncoder;
	std::unique_ptr<CAudioEncoder> m_audioEncoder;
	std::unique_ptr<CScreenCapture> m_screenCapture;
	std::unique_ptr<CAudioCapture> m_audioCapture;
	std::unique_ptr<CSceneDetector> m_sceneDetector;
	std::unique_ptr<std::thread> m_encoderThread;
	bool m_bDone;
	void EncoderThread();
public:
	CCapturePipeline();
	virtual ~CCapturePipeline();
	void Init(HWND hWND);
	void Start();
	void Stop();
};

