#include "FileWriter.h"
#include "AudioEncoder.h"



void CAudioEncoder::AddAudioStream(int bitrate)
{
	if (m_audioCodec)
	{
		//nothing to do!
		return;
	}


	m_audioCodec = std::unique_ptr < AVCodec, std::function<void(AVCodec *) >> (avcodec_find_encoder(CODEC_ID_AAC), [](AVCodec* ptr) { av_free(ptr); });
	if (!m_audioCodec)
	{
		fprintf(stderr, "Codec not found\n");
	}
	else
		printf("AAC codec found\n");

	auto pContext = m_pfileWriter->GetContext();
	m_audioStream = std::unique_ptr < AVStream, std::function<void(AVStream *) >>(avformat_new_stream(pContext, m_audioCodec.get()), [](AVStream* f) {  av_free(f); });
	m_audioStream->id = pContext->nb_streams - 1;

	AVCodecContext* cc = m_audioStream->codec;
	cc->sample_fmt = AV_SAMPLE_FMT_S16;
	cc->bit_rate = bitrate*1000;
	cc->sample_rate = 44100;
	cc->channel_layout = AV_CH_LAYOUT_STEREO;
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

	m_frame_size = m_audioStream->codec->frame_size;

	m_fifo =av_audio_fifo_alloc(AV_SAMPLE_FMT_S16,2, 1);


	m_tempFrame = std::unique_ptr<AVFrame, std::function<void(AVFrame *)>>(av_frame_alloc(), [](AVFrame* ptr) {av_frame_free(&ptr); });
	
	m_tempFrame->nb_samples = m_frame_size;
	m_tempFrame->channel_layout = cc->channel_layout;
	m_tempFrame->format = cc->sample_fmt;
	m_tempFrame->sample_rate = cc->sample_rate;

	int error = 0;
	/**
	* Allocate the samples of the created frame. This call will make
	* sure that the audio frame can hold as many samples as specified.
	*/
	if ((error = av_frame_get_buffer(m_tempFrame.get(), 0)) < 0)
	{
		return ;
	}

}


void CAudioEncoder::Encode(AVFrame* inputSample)
{
	AVCodecContext *pContext = m_audioStream->codec;


	int error;

	if ((error = av_audio_fifo_realloc(m_fifo, av_audio_fifo_size(m_fifo) + m_frame_size)) < 0)
	{
		fprintf(stderr, "Could not reallocate FIFO\n");
		return ;
	}

	/** Store the new samples in the FIFO buffer. */
	if (av_audio_fifo_write(m_fifo, (void **)inputSample->data, inputSample->nb_samples) < m_frame_size)
	{
		fprintf(stderr, "Could not write data to FIFO\n");
		return ;
	}
	do
	{
		int current_frame_size = FFMIN(av_audio_fifo_size(m_fifo), m_frame_size);
		if (current_frame_size < m_frame_size)
			break;

		m_tempFrame->nb_samples = current_frame_size;

		if (av_audio_fifo_read(m_fifo, (void **)m_tempFrame->data, current_frame_size) < current_frame_size)
		{
			fprintf(stderr, "Could not read data from FIFO\n");
			return;
		}

		AVPacket pkt;
		av_init_packet(&pkt);
		pkt.data = NULL;    // packet data will be allocated by the encoder
		pkt.size = 0;
		int got_output;

		int out_size = avcodec_encode_audio2(pContext, &pkt, m_tempFrame.get(), &got_output);
		if (got_output)
		{
			pkt.stream_index = m_audioStream->index;

			pkt.pts = av_rescale_q_rnd(pkt.pts, pContext->time_base, m_audioStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			pkt.dts = av_rescale_q_rnd(pkt.dts, pContext->time_base, m_audioStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			pkt.duration = av_rescale_q(pkt.duration, pContext->time_base, m_audioStream->time_base);
			pkt.pos = -1;

			//printf("AAC wrote %d bytes \n", pkt.size);
			auto pContext = m_pfileWriter->GetContext();
			av_interleaved_write_frame(pContext, &pkt);
			//fwrite(pkt.data, 1, pkt.size, m_fileName.get());
		}
		av_free_packet(&pkt);
	} while (1);


}