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
	CCapture();
	void Start();
	void Stop();
	CSample * GetSample();
};
