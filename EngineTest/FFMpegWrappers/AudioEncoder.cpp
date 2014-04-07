#include "FileWriter.h"
#include "AudioEncoder.h"

void CAudioEncoder::AddAudioStream()
{
	if (m_audioCodec)
	{
		//nothing to do!
		return;
	}


	m_audioCodec = std::unique_ptr<AVCodec, std::function<void(AVCodec *)>>(avcodec_find_encoder(CODEC_ID_AAC), [](AVCodec* ptr) { av_free(ptr); });
	if (!m_audioCodec)
	{
		fprintf(stderr, "Codec not found\n");
	}
	else
		printf("AAC codec found\n");

	auto pContext = m_pfileWriter->GetContext();
	m_audioStream = std::unique_ptr<AVStream, std::function<void(AVStream *)>>(avformat_new_stream(pContext, m_audioCodec.get()), [](AVStream* f) {  av_free(f); });
	m_audioStream->id = pContext->nb_streams - 1;

	AVCodecContext* cc = m_audioStream->codec;
	cc->sample_fmt = AV_SAMPLE_FMT_S16;
	cc->bit_rate = 64000;
	cc->sample_rate = 44100;
	cc->channels = 2;


	if (pContext->oformat->flags & AVFMT_GLOBALHEADER)
		cc->flags |= CODEC_FLAG_GLOBAL_HEADER;

	// Open the encoder
	if (avcodec_open2(cc, m_audioCodec.get(), NULL) < 0)
	{
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}
	else
		printf("H264 codec opened\n");




}



void CAudioEncoder::Encode(AVFrame* inputSample)
{
	AVCodecContext *pContext = m_audioStream->codec;



	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	int got_output;

	inputSample->nb_samples = 1024;
	int out_size = avcodec_encode_audio2(pContext, &pkt, inputSample, &got_output);
	if (got_output)
	{
		pkt.stream_index = m_audioStream->index;

		pkt.pts = av_rescale_q_rnd(pkt.pts, pContext->time_base, m_audioStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, pContext->time_base, m_audioStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, pContext->time_base, m_audioStream->time_base);
		pkt.pos = -1;

		printf("AAC wrote %d bytes \n", pkt.size);
		auto pContext = m_pfileWriter->GetContext();
		av_interleaved_write_frame(pContext, &pkt);
		//fwrite(pkt.data, 1, pkt.size, m_fileName.get());
	}
	av_free_packet(&pkt);

}