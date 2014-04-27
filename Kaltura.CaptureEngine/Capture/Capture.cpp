#include "Capture.h"

#include "..\FFMpegWrappers\FFMpegCommon.h"



CCapture::CCapture() : m_sdlWindow(NULL), m_sdlRenderer(NULL), m_sdlTexture(NULL), m_bPreview(false)
{
}



CCapture::~CCapture()
{
}

void CCapture::CaptureThread()
{
	ADDLOG(LogLevel::Info, L"CaptureThread started");
	while (!m_bDone)
	{
		CSample* pSample = Capture();
		if (m_bPreview)
		{
			RenderSample(pSample);
		}
		{
			std::lock_guard<std::mutex> lock(m_samplesMutex);
			if (m_samples.size() < 5)
			{
				m_samples.push_back(std::shared_ptr<CSample>(pSample));
				cond_var.notify_one();
			}
			else
			{
				printf("Skipping frame!");
				delete pSample;
			}
		}

	}
	ADDLOG(LogLevel::Info, L"CaptureThread finished");
}
void CCapture::Start()
{
	ADDLOG(LogLevel::Debug, L"Starting Capture thread");
	m_bDone = false;
	m_captureThread = std::make_unique<std::thread>(&CCapture::CaptureThread, this);
}

void CCapture::Stop()
{
	ADDLOG(LogLevel::Debug, L"Stopping Capture thread");
	m_bDone = true;
	m_captureThread->join();
	ADDLOG(LogLevel::Debug, L"Capture thread stopped");
};


std::shared_ptr<CSample>  CCapture::GetSample()
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


void CCapture::ShowPreviewWindow()
{
	if (m_sdlWindow == NULL)
	{
		SDL_Init(SDL_INIT_EVERYTHING);

		SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_BORDERLESS, &m_sdlWindow, &m_sdlRenderer);
		SDL_RendererInfo info;
		SDL_GetRendererInfo(m_sdlRenderer, &info);

	}
	m_bPreview = true;
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
	}
}

void CCapture::RenderSample(CSample *pVideoSample)
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
	printf("Time to render frame (%d)\n", t1 - t0);
}


int CFFCapture::TotalStreams()
{
	if (m_avFormatContext)
		return m_avFormatContext->nb_streams;
	else
	{
		return 0;
	}

}
AVCodecContext* CFFCapture::GetAVCodecContext(int index)
{
	if (m_avFormatContext)
		return m_avFormatContext->streams[index]->codec;
	else
	{
		return 0;
	}
}

void CFFCapture::Init(const std::string& device)
{
	m_inputDevice = std::unique_ptr<AVInputFormat, std::function<void(AVInputFormat *)>>(av_find_input_format("dshow"), [](AVInputFormat* f) {  av_free(f); });
	AVFormatContext *formatContext = avformat_alloc_context();
	m_avFormatContext = std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>>(formatContext, [](AVFormatContext* f) {  av_free(f); });
	AVDictionary* options = NULL;
	int error = avformat_open_input(&formatContext, device.c_str(), m_inputDevice.get(), NULL);
	if (error < 0)
	{
		error = 0;
	}	
	
	auto codecCtx=formatContext->streams[0]->codec;
	auto codec = avcodec_find_decoder(codecCtx->codec_id);


	if (avcodec_open2(codecCtx, codec, NULL) < 0)
	{

	}
}

int frame = 0;

AVFrame* CFFCapture::ReadPacket()
{
	AVFrame* sample = NULL;
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	auto codecCtx = m_avFormatContext->streams[0]->codec;
	int frameFinished;
	if (0 == av_read_frame(m_avFormatContext.get(), &pkt))
	{
		

		int index = pkt.stream_index;
		auto avStream = m_avFormatContext->streams[index];
		auto codec = avStream->codec;
		if (codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			sample = ::CreateAudioFrame(pkt.data, pkt.size);
			//avcodec_decode_audio4(codecCtx, sample, &frameFinished, &pkt);
			sample->pts = pkt.pts;
		}
		if (codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			sample = av_frame_alloc();
			avcodec_decode_video2(codecCtx, sample, &frameFinished, &pkt);
			if (frameFinished != 1)
			{
				_ASSERT(false);
			}
			sample->pts = frame++;
		}
	}
	av_free_packet(&pkt);

	return sample;
}




CSample* CFFCapture::Capture()
{
	auto frame = ReadPacket();
	return new CSample(frame);
}