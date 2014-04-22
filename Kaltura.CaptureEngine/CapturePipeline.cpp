#include "stdafx.h"
#include "CapturePipeline.h"


CCapturePipeline::CCapturePipeline()
{
}


CCapturePipeline::~CCapturePipeline()
{
}

void CCapturePipeline::Init(HWND hScreen, const std::string& videoInput, const std::string& audioInput, const std::string& fileOutput)
{
	m_fileWriter = std::make_shared<CFileWriter>();
	m_fileWriter->InitFile(fileOutput.c_str());

	m_videoEncoder = std::make_unique<CVideoEncoder>(m_fileWriter);
	AVCodecContext* context;
	if (!videoInput.empty())
	{
		m_videoCapture = std::make_unique<CVideoCapture>();
		m_videoCapture->Init(videoInput.c_str());

		context = m_videoCapture->GetAVCodecContext(0);
	}
	else
	{
		m_screenCapture = std::make_unique<CScreenCapture>();
		RECT ClientRect;
		GetClientRect(hScreen, &ClientRect);
		m_screenCapture->Init(hScreen, ClientRect);
		context = m_screenCapture->GetAVCodecContext(0);
	}

	
	m_videoEncoder->AddVideoStream(context->width, context->height, context->pix_fmt, 1000);

	
	if (!audioInput.empty())
	{
		m_audioCapture = std::make_unique<CAudioCapture>();
		m_audioCapture->Init(audioInput.c_str());
		m_audioEncoder = std::make_unique<CAudioEncoder>(m_fileWriter);
		m_audioEncoder->AddAudioStream(64);
	}
}

void CCapturePipeline::Start()
{
	m_bDone = false;
	m_encoderThread = std::make_unique<std::thread>(&CCapturePipeline::EncoderThread, this);
	if (m_videoCapture)
		m_videoCapture->Start();

	if (m_screenCapture)
		m_screenCapture->Start();

	if (m_audioCapture)
		m_audioCapture->Start();
}

void CCapturePipeline::EncoderThread()
{
	m_fileWriter->Start();
	while (!m_bDone)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		CSample* pSample = m_screenCapture ? m_screenCapture->GetSample() : m_videoCapture->GetSample();
		if (pSample != NULL)
		{
			auto t0 = clock();
			m_videoEncoder->Encode(pSample->get());
			auto t1 = clock();
		//	printf("time to  encode video (%d)\n", t1 - t0);

			///if (m_screenCapture)
			//	m_sceneDetector->ProcessImage(pSample);
			delete pSample;
		}
		CSample* pAudioSample = m_audioCapture ? m_audioCapture->GetSample() : NULL;
		if (pAudioSample != NULL)
		{
			auto t0 = clock();
			m_audioEncoder->Encode(pAudioSample->get());
			auto t1 = clock();
		//	printf("time to  encode audio (%d)\n", t1 - t0);
			delete pAudioSample;
		}

	}
	m_fileWriter->Close();
}

void CCapturePipeline::Stop()
{
	if (m_screenCapture)
		m_screenCapture->Stop();
	if (m_videoCapture)
		m_videoCapture->Stop();

	if (m_audioCapture)
		m_audioCapture->Stop();

	m_bDone = true;
	m_encoderThread->join();
}