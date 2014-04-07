#include "stdafx.h"
#include "CapturePipeline.h"


CCapturePipeline::CCapturePipeline()
{
}


CCapturePipeline::~CCapturePipeline()
{
}

void CCapturePipeline::Init(HWND hWnd)
{
	m_fileWriter = std::make_shared<CFileWriter>();
	m_fileWriter->InitFile("c:\\1.mp4");


	m_screenCapture = std::make_unique<CScreenCapture>();
	m_videoEncoder = std::make_unique<CVideoEncoder>(m_fileWriter);
	m_videoEncoder->AddVideoStream(1920, 1080, 1000);

	m_audioCapture = std::make_unique<CAudioCapture>();
	m_audioCapture->Init();
	m_audioEncoder = std::make_unique<CAudioEncoder>(m_fileWriter);
	m_audioEncoder->AddAudioStream();

}

void CCapturePipeline::Start()
{
	m_bDone = false;
	m_encoderThread = std::make_unique<std::thread>(&CCapturePipeline::EncoderThread, this);
	m_screenCapture->Start();
	m_audioCapture->Start();
}

void CCapturePipeline::EncoderThread()
{
	m_fileWriter->Start();
	while (!m_bDone)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(10));
		CSample* pSample=m_screenCapture->GetSample();
		if (pSample != NULL)
		{
			auto t0 = clock();
			m_videoEncoder->Encode(pSample->get());
			auto t1 = clock();
			printf("time to  encode video (%d)\n", t1 - t0);
			delete pSample;
		}
		CSample* pAudioSample = m_audioCapture->GetSample();
		if (pAudioSample != NULL)
		{
			auto t0 = clock();
			m_audioEncoder->Encode(pAudioSample->get());
			auto t1 = clock();
			printf("time to  encode audio (%d)\n", t1 - t0);
			delete pAudioSample;
		}

	}
	m_fileWriter->Close();
}

void CCapturePipeline::Stop()
{
	m_screenCapture->Stop();
	m_audioCapture->Stop();
	m_bDone = true;
	m_encoderThread->join();
}