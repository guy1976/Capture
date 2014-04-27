#include "stdafx.h"
#include "CapturePipeline.h"

#pragma comment(lib,"SDL2.lib")

CCapturePipeline::CCapturePipeline() : m_sdlWindow(NULL), m_sdlRenderer(NULL), m_sdlTexture(NULL), m_pAudioCapture(NULL), m_pVideoCapture(NULL)
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
	if (m_pVideoCapture!=NULL)
		m_pVideoCapture->Start();


	if (m_pAudioCapture!=NULL)
		m_pAudioCapture->Start();


	


}

void CCapturePipeline::ShowPreview()
{
	if (m_sdlWindow == NULL)
	{
		SDL_Init(SDL_INIT_EVERYTHING);

		SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_BORDERLESS, &m_sdlWindow, &m_sdlRenderer);
		SDL_RendererInfo info;
		SDL_GetRendererInfo(m_sdlRenderer, &info); 

	}
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
	}
	m_bPreview = true;
}

void CCapturePipeline::HidePreview()
{
}
void CCapturePipeline::EncoderThread()
{
	m_fileWriter->Start();
	while (!m_bDone)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		CSample* pVideoSample = m_pVideoCapture != NULL ? m_pVideoCapture->GetSample() : NULL;

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

			if (m_bPreview)
			{
				if (m_sdlTexture == NULL)
				{
					int pixFormat = pVideoSample->get()->format;
					Uint32 sdlFormat = SDL_PIXELFORMAT_RGB888;
					
					if (PIX_FMT_RGB24 == pixFormat)
					{
						sdlFormat = SDL_PIXELFORMAT_RGB888;
					}
					if (PIX_FMT_BGR24 == pixFormat)
					{
						sdlFormat = SDL_PIXELFORMAT_BGR24;
					}
					if (PIX_FMT_RGB32 == pixFormat)
					{
						sdlFormat = SDL_PIXELFORMAT_ARGB8888;
					}
					if (PIX_FMT_YUYV422 == pixFormat)
					{
						sdlFormat = SDL_PIXELFORMAT_YV12;
					}
					m_sdlTexture = SDL_CreateTexture(m_sdlRenderer,
						sdlFormat,
						SDL_TEXTUREACCESS_STATIC,
						pVideoSample->get()->width, pVideoSample->get()->height);
				}
				auto t0 = clock();
				
				SDL_UpdateTexture(m_sdlTexture, NULL, pVideoSample->get()->data[0], pVideoSample->get()->linesize[0]);
				SDL_RenderClear(m_sdlRenderer);
				SDL_RenderCopy(m_sdlRenderer, m_sdlTexture, NULL, NULL);
				SDL_RenderPresent(m_sdlRenderer);
				
				auto t1 = clock();
				printf("Time to render frame (%d)\n", t1-t0);
			}


		//	printf("time to  encode video (%d)\n", t1 - t0);
			delete pVideoSample;
		}
		CSample* pAudioSample = m_pAudioCapture != NULL ? m_pAudioCapture->GetSample() : NULL;
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
	if (m_pVideoCapture)
		m_pVideoCapture->Stop();

	if (m_pAudioCapture)
		m_pAudioCapture->Stop();

	m_bDone = true;
	m_encoderThread->join();
	m_encoderThread.release();
}