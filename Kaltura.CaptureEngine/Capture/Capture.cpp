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
	

	//avformat_open_input(&formatC, "audio=Microphone (Microsoft LifeCam V", m_inputDevice.get(), NULL);

}

int frame = 0;

AVFrame* CFFCapture::ReadPacket()
{
	AVFrame* sample = NULL;
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	if (0 == av_read_frame(m_avFormatContext.get(), &pkt))
	{
		int index = pkt.stream_index;
		auto avStream = m_avFormatContext->streams[index];
		auto codec = avStream->codec;
		if (codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			sample = ::CreateAudioFrame(pkt.data, pkt.size);
			sample->pts = pkt.pts;
		}
		if (codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			sample = ::CreateVideoFrame(pkt.data, codec->width,codec->height,codec->width*3,codec->pix_fmt,false);
			memcpy(sample->data[0], pkt.data, pkt.size);
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