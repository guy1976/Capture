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

	CCapture* m_pVideoCapture;
	CCapture* m_pAudioCapture;
	std::unique_ptr<std::thread> m_encoderThread;
	std::unique_ptr<std::thread> m_processorThread;

	std::vector<CCaptureEngineSamplesProcessor*> m_processors;
	bool m_bDone;
	void EncoderThread();
	void AddVideo(AVCodecContext* context);
public:
	CCapturePipeline();
	virtual ~CCapturePipeline();
	void AddVideoSource(CCapture* pSource);
	void AddAudioSource(CCapture* pSource);
	void SetOutputFile(const std::string& fileName);
	void AddProcessor(CCaptureEngineSamplesProcessor* pProcessor) { m_processors.push_back(pProcessor); }
	void Init();
	void Close();
};
