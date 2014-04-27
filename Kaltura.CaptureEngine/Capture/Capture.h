#pragma once

#include "..\FFMpegWrappers\FFMpegCommon.h"
#include <stack>
#include <mutex>
#include <thread>
#include <condition_variable>


class CCapture
{
	std::unique_ptr<std::thread> m_captureThread;
	std::deque<std::shared_ptr<CSample>> m_samples;
	std::mutex m_samplesMutex;
	std::condition_variable cond_var;

	SDL_Window*  m_sdlWindow;
	SDL_Renderer* m_sdlRenderer;
	SDL_Texture * m_sdlTexture;

	void RenderSample(CSample *pVideoSample);
	void CaptureThread();
	bool m_bDone;
	bool m_bPreview;
protected:
	virtual CSample* Capture()=0;

public:
	void ShowPreviewWindow();
	virtual int TotalStreams() = 0;
	virtual AVCodecContext* GetAVCodecContext(int index) = 0;
	CCapture();
	virtual ~CCapture();
	void Start();
	void Stop();
	std::shared_ptr<CSample> GetSample();
};


class CFFCapture : public CCapture
{
protected:
	std::unique_ptr<AVInputFormat, std::function<void(AVInputFormat *)>> m_inputDevice;
	std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>> m_avFormatContext;
	AVFrame* ReadPacket();
protected:	
public:
	int TotalStreams();
	AVCodecContext* GetAVCodecContext(int index);
	CSample* Capture();
	virtual void Init(const std::string& device);
};
