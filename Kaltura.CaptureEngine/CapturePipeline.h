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
	SDL_Window*  m_sdlWindow;
	SDL_Renderer* m_sdlRenderer;
	SDL_Texture * m_sdlTexture;

	std::shared_ptr<CFileWriter>  m_fileWriter;
	std::unique_ptr<CVideoEncoder> m_videoEncoder;
	std::unique_ptr<CAudioEncoder> m_audioEncoder;
	std::unique_ptr<CCapture> m_videoCapture;
	std::unique_ptr<CCapture> m_audioCapture;
	std::unique_ptr<std::thread> m_encoderThread;
	std::unique_ptr<std::thread> m_processorThread;

	std::vector<CCaptureEngineSamplesProcessor*> m_processors;
	bool m_bDone,m_bPreview;
	void EncoderThread();
	void AddVideo(AVCodecContext* context);
	void ClosePreview();
public:
	CCapturePipeline();
	virtual ~CCapturePipeline();
	void AddCameraSource(const std::string& input);
	void AddAudioSource(const std::string& input);

	void AddScreenSource(HWND hScreen);
	void SetOutputFile(const std::string& fileName);
	void AddProcessor(CCaptureEngineSamplesProcessor* pProcessor) { m_processors.push_back(pProcessor); }
	void ShowPreview();
	void Start();
	void Stop();
};
