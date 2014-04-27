#include "stdafx.h"
#include "CapturePipeline.h"

#pragma comment(lib,"SDL2.lib")

CCapturePipeline::CCapturePipeline() : m_pAudioCapture(NULL), m_pVideoCapture(NULL)
{
}


CCapturePipeline::~CCapturePipeline()
{
}

void CCapturePipeline::AddVideoSource(CCapture* pCapture)
{
	m_pVideoCapture = pCapture;
	auto context = pCapture->GetAVCodecContext(0);
	m_videoEncoder = std::make_unique<CVideoEncoder>(m_fileWriter);
	m_videoEncoder->AddVideoStream(context->width, context->height, context->pix_fmt, 1000);
}

void CCapturePipeline::AddAudioSource(CCapture* pCapture)
{
	m_pAudioCapture = pCapture;
	m_audioEncoder = std::make_unique<CAudioEncoder>(m_fileWriter);
	m_audioEncoder->AddAudioStream(64);
}


void CCapturePipeline::SetOutputFile(const std::string& fileName)
{
	m_fileWriter = std::make_shared<CFileWriter>();
	m_fileWriter->InitFile(fileName.c_str());
}

void CCapturePipeline::Start()
{
	if (m_encoderThread)
	{
		return;
	}
	m_bDone = false;
	m_encoderThread = std::make_unique<std::thread>(&CCapturePipeline::EncoderThread, this);


	


}

void CCapturePipeline::EncoderThread()
{
	m_fileWriter->Start();
	while (!m_bDone)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		std::shared_ptr<CSample> videoSample = m_pVideoCapture != NULL ? m_pVideoCapture->GetSample() : nullptr;

		if (videoSample)
		{
			for (auto iter = m_processors.begin(); iter < m_processors.end(); iter++)
			{
				CCaptureEngineVideoSample videoSample(videoSample->get()->width, videoSample->get()->height, videoSample->get()->linesize[0], videoSample->get()->format, videoSample->get()->data[0]);
				(*iter)->ProcessSample((CCaptureEngineSample*)&videoSample);
			}
			auto t0 = clock();
			m_videoEncoder->Encode(videoSample->get());
			auto t1 = clock();

			

		//	printf("time to  encode video (%d)\n", t1 - t0);
		}
		
		
		std::shared_ptr<CSample> audioample = m_pAudioCapture != NULL ? m_pAudioCapture->GetSample() : nullptr;

		if (audioample)
		{
			auto t0 = clock();
			m_audioEncoder->Encode(audioample->get());
			auto t1 = clock();
		//	printf("time to  encode audio (%d)\n", t1 - t0);
		}

	}
	m_fileWriter->Close();
}

void CCapturePipeline::Stop()
{
	m_bDone = true;
	m_encoderThread->join();
	m_encoderThread.release();
}