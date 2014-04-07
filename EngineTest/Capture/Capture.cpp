#include "Capture.h"

#include "..\FFMpegWrappers\FFMpegCommon.h"





CCapture::CCapture()
{
}



void CCapture::CaptureThread()
{
	while (!m_bDone)
	{
		CSample* pSample = Capture();
		{
			std::lock_guard<std::mutex> lock(m_samplesMutex);
			m_samples.push_back(pSample);
			cond_var.notify_one();
		}

	}
}
void CCapture::Start()
{
	m_bDone = false;
	m_captureThread = std::make_unique<std::thread>(&CCapture::CaptureThread, this);
}

void CCapture::Stop()
{
	m_bDone = true;
	m_captureThread->join();
};


CSample * CCapture::GetSample()
{
	std::unique_lock<std::mutex> lock(m_samplesMutex);	

	if (!m_samples.empty())
	{
		auto sample = m_samples.front();

		m_samples.pop_front();
		return sample;
	}
	return NULL;
}