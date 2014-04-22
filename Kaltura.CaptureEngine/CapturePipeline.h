#pragma once

#include ".\Capture\ScreenCapture.h"
#include "FFMpegWrappers\FFMpegCommon.h"
#include "FFMpegWrappers\FileWriter.h"
#include "FFMpegWrappers\AudioEncoder.h"
#include ".\Capture\AudioCapture.h"
#include ".\Capture\VideoCapture.h"
#include "FFMpegWrappers\VideoEncoder.h"

#include <thread>         // std::thread

class CCapturePipeline
{
	std::shared_ptr<CFileWriter>  m_fileWriter;
	std::unique_ptr<CVideoEncoder> m_videoEncoder;
	std::unique_ptr<CAudioEncoder> m_audioEncoder;
	std::unique_ptr<CScreenCapture> m_screenCapture;
	std::unique_ptr<CVideoCapture> m_videoCapture;
	std::unique_ptr<CAudioCapture> m_audioCapture;
	std::unique_ptr<std::thread> m_encoderThread;
	bool m_bDone;
	void EncoderThread();
public:
	CCapturePipeline();
	virtual ~CCapturePipeline();
	void Init(HWND hScreen, const std::string& videoInput, const std::string& audioInput, const std::string& fileOutput);
	void Start();
	void Stop();
};
