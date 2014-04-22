#pragma once

#include ".\Capture\ScreenCapture.h"
#include "FFMpegWrappers\FFMpegCommon.h"
#include "FFMpegWrappers\FileWriter.h"
#include "FFMpegWrappers\AudioEncoder.h"
#include ".\Capture\AudioCapture.h"
#include ".\Capture\VideoCapture.h"
#include "FFMpegWrappers\VideoEncoder.h"
#include "..\Kaltura.CaptureEngine.Common\CaptureEngineSamplesProcessor.h"

#include <thread>         // std::thread
#include <vector>         // std::thread

class CCapturePipeline
{
	std::shared_ptr<CFileWriter>  m_fileWriter;
	std::unique_ptr<CVideoEncoder> m_videoEncoder;
	std::unique_ptr<CAudioEncoder> m_audioEncoder;
	std::unique_ptr<CScreenCapture> m_screenCapture;
	std::unique_ptr<CVideoCapture> m_videoCapture;
	std::unique_ptr<CAudioCapture> m_audioCapture;
	std::unique_ptr<std::thread> m_encoderThread;
	std::unique_ptr<std::thread> m_processorThread;

	std::vector<CCaptureEngineSamplesProcessor*> m_processors;
	bool m_bDone;
	void EncoderThread();
	void AddVideo(AVCodecContext* context);
public:
	CCapturePipeline();
	virtual ~CCapturePipeline();
	void AddVideo(const std::string& input);
	void AddAudio(const std::string& input);

	void AddScreenCapture(HWND hScreen);
	void SetOutputFile(const std::string& fileName);
	void AddProcessor(CCaptureEngineSamplesProcessor* pProcessor) { m_processors.push_back(pProcessor); }
	void Start();
	void Stop();
};
