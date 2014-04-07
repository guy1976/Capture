#include "..\stdafx.h"
#include "AudioCapture.h"

void CAudioCapture::Init()
{
	m_inputDevice = std::unique_ptr<AVInputFormat, std::function<void(AVInputFormat *)>>(av_find_input_format("dshow"), [](AVInputFormat* f) {  av_free(f); });
	AVFormatContext *formatC = avformat_alloc_context();
	m_avFormatContext = std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>>(formatC, [](AVFormatContext* f) {  av_free(f); });
	AVDictionary* options = NULL;
	avformat_open_input(&formatC, "audio=Microphone (Realtek High Defini", m_inputDevice.get(), NULL);
	//avformat_open_input(&formatC, "audio=Microphone (Microsoft LifeCam V", m_inputDevice.get(), NULL);

	for (int i = 0; i < formatC->nb_streams; i++)
	{
		auto v = formatC->streams[i];
	}
}

AVFrame* CAudioCapture::ReadPacket()
{
	AVFrame* audioSample = NULL;
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	if (0 == av_read_frame(m_avFormatContext.get(), &pkt))
	{
		audioSample = ::CreateAudioFrame(pkt.data, pkt.size);
		audioSample->pts = pkt.pts;
	}
	av_free_packet(&pkt);

	return audioSample;
}




CSample* CAudioCapture::Capture()
{
	auto frame=ReadPacket();
	return new CSample(frame);
}