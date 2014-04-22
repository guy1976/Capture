#include "stdafx.h"
#include "CapturePipeline.h"

CCapturePipeline::CCapturePipeline()
{
}


CCapturePipeline::~CCapturePipeline()
{
}

void CCapturePipeline::AddCameraSource(const std::string& videoInput)
{
	m_videoCapture = std::make_unique<CVideoCapture>();
	auto pCapture = ((CVideoCapture*)m_videoCapture.get());
	pCapture->Init(videoInput.c_str());
	auto context = m_videoCapture->GetAVCodecContext(0);
	AddVideo(context);
}

void CCapturePipeline::AddAudioSource(const std::string& input)
{
	m_audioCapture = std::make_unique<CAudioCapture>();
	auto pCapture = ((CAudioCapture*)m_audioCapture.get());
	pCapture->Init(input.c_str());
	m_audioEncoder = std::make_unique<CAudioEncoder>(m_fileWriter);
	m_audioEncoder->AddAudioStream(64);
}


void CCapturePipeline::AddScreenSource(HWND hScreen)
{
	m_videoCapture = std::make_unique<CScreenCapture>();
	RECT ClientRect;
	GetClientRect(hScreen, &ClientRect);
	((CScreenCapture*)m_videoCapture.get())->Init(hScreen, ClientRect);
	auto context = m_videoCapture->GetAVCodecContext(0);
	AddVideo(context);
}

void CCapturePipeline::AddVideo(AVCodecContext* context)
{
	m_videoEncoder = std::make_unique<CVideoEncoder>(m_fileWriter);
	m_videoEncoder->AddVideoStream(context->width, context->height, context->pix_fmt, 1000);
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
	if (m_videoCapture)
		m_videoCapture->Start();


	if (m_audioCapture)
		m_audioCapture->Start();
}

void CCapturePipeline::EncoderThread()
{
	m_fileWriter->Start();
	while (!m_bDone)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		CSample* pVideoSample = m_videoCapture->GetSample();

		if (pVideoSample != NULL)
		{
			for (auto iter = m_processors.begin(); iter < m_processors.end(); iter++)
			{
				CCaptureEngineVideoSample videoSample(pVideoSample->get()->width, pVideoSample->get()->height, pVideoSample->get()->linesize[0], pVideoSample->get()->format, pVideoSample->get()->data[0]);
				(*iter)->ProcessSample((CCaptureEngineSample*)&videoSample);
			}
			auto t0 = clock();
			m_videoEncoder->Encode(pVideoSample->get());
			auto t1 = clock();
		//	printf("time to  encode video (%d)\n", t1 - t0);
			delete pVideoSample;
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
	if (m_videoCapture)
		m_videoCapture->Stop();

	if (m_audioCapture)
		m_audioCapture->Stop();

	m_bDone = true;
	m_encoderThread->join();
	m_encoderThread.release();
}