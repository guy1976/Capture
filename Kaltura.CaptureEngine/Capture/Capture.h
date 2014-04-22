#pragma once

#include "..\FFMpegWrappers\FFMpegCommon.h"
#include <stack>
#include <mutex>
#include <thread>
#include <condition_variable>


class CCapture
{
	std::unique_ptr<std::thread> m_captureThread;
	std::deque<CSample*> m_samples;
	std::mutex m_samplesMutex;
	std::condition_variable cond_var;

	
	void CaptureThread();
	bool m_bDone;
protected:
	virtual CSample* Capture()=0;
public:
	virtual int TotalStreams() = 0;
	virtual AVCodecContext* GetAVCodecContext(int index) = 0;
	CCapture();
	void Start();
	void Stop();
	CSample * GetSample();
};


class CFFCapture : public CCapture
{
protected:
	std::unique_ptr<AVInputFormat, std::function<void(AVInputFormat *)>> m_inputDevice;
	std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>> m_avFormatContext;
	AVFrame* ReadPacket();
protected:
	void InitDevcie();	
public:
	int TotalStreams();
	AVCodecContext* GetAVCodecContext(int index);
	CSample* Capture();
	virtual void Init(const std::string& device);
};
